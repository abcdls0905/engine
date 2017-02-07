//--------------------------------------------------------------------
// 文件名:		mark_string.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月12日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "mark_string.h"
#include "gui_const.h"
#include "gui_utils.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../visual/i_painter.h"

// CMarkStringStatus

CMarkStringStatus::~CMarkStringStatus()
{
	if (m_pRef)
	{
		CORE_FREE(m_pRef, m_nRefMemSize);
	}
}

void CMarkStringStatus::SetFont(const wchar_t* value, size_t size)
{
	if (size == 0)
	{
		m_strFont[0] = 0;
		return;
	}
	
	result_wstring ws(value, size);
	
	ToString(ws.c_str(), m_strFont, sizeof(m_strFont));
}

void CMarkStringStatus::SetRef(const wchar_t* value, size_t size)
{
	if (m_pRef)
	{
		CORE_FREE(m_pRef, m_nRefMemSize);
	}
	
	m_nRefMemSize = sizeof(wchar_t) * (size + 1);
	m_pRef = (wchar_t*)CORE_ALLOC(m_nRefMemSize);
	memcpy(m_pRef, value, size * sizeof(wchar_t));
	m_pRef[size] = 0;
}

// CMarkString

CMarkString* CMarkString::NewInstance(IPainter* painter)
{
	CMarkString* p = CORE_NEW(CMarkString);
	
	p->m_pPainter = painter;
	
	return p;
}

CMarkString::CMarkString()
{
	m_pPainter = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

CMarkString::~CMarkString()
{
	ReleaseAll();
}

void CMarkString::ReleaseAll()
{
	for (size_t i = 0; i < m_Parts.size(); ++i)
	{
		int type = m_Parts[i]->nType;
		
		if (type == PARTTYPE_STRING)
		{
			CORE_DELETE((part_string_t*)m_Parts[i]);
		}
		else if (type == PARTTYPE_IMAGE)
		{
			part_image_t* pImage = (part_image_t*)m_Parts[i];

			pImage->Image.Release();
			CORE_DELETE(pImage);
		}
		else
		{
			CORE_DELETE((part_base_t*)m_Parts[i]);
		}
	}
	
	m_Parts.clear();
}

void CMarkString::Release()
{
	CORE_DELETE(this);
}

bool CMarkString::AddStringPart(wchar_t* buf, size_t& count, 
	const CMarkStringStatus& status)
{
	if (0 == count)
	{
		return false;
	}
	
	buf[count] = 0;
	
	part_string_t* p = CORE_NEW(part_string_t);
	
	p->nType = PARTTYPE_STRING;
	p->nWidth = m_pPainter->GetTextWidth(buf);
	p->nHeight = m_pPainter->GetTextHeight(buf);
	p->wsRef = status.GetRef();
	p->strFont = status.GetFont();
	p->nColor = status.GetColor();
	p->wsContent = buf;
	
	m_Parts.push_back(p);
	
	// 开始新的字符串
	count = 0;
	
	if (p->nHeight > m_nHeight)
	{
		m_nHeight = p->nHeight;
	}
	
	return true;
}

bool CMarkString::AddImagePart(Gui* gui, const char* image, 
	const CMarkStringStatus& status)
{
	part_image_t* p = CORE_NEW(part_image_t);
	
	p->nType = PARTTYPE_IMAGE;
	p->wsRef = status.GetRef();
	p->Image.SetName(image);
	p->Image.Create(m_pPainter, gui, false);
	
	if (p->Image.IsReady())
	{
		p->nWidth = p->Image.GetWidth() + m_pPainter->GetTextInterval();
		p->nHeight = p->Image.GetHeight();
	}
	else
	{
		p->nWidth = 0;
		p->nHeight = 0;
	}
	
	m_Parts.push_back(p);
	
	return true;
}

/*
// 十六进制转成字符
static unsigned int hex_to_int(const wchar_t* s, size_t count)
{
	unsigned int v = 0;
	
	for (size_t i = 0; i < count; ++i)
	{
		v <<= 4;
		
		wchar_t c = s[i];
		
		if ((c >= L'0') && (c <= L'9'))
		{
			v += (unsigned int)(c - L'0');
		}
		else if ((c >= L'A') && (c <= L'F'))
		{
			v += (unsigned int)(c - L'A' + 10);
		}
		else if ((c >= L'a') && (c <= L'f'))
		{
			v += (unsigned int)(c - L'a' + 10);
		}
		else
		{
			return 0;
		}
	}
	
	return v;
}
*/

int CMarkString::Create(Gui* gui, const wchar_t* ws)
{
	CMarkStringStatus status;
	
	return CreateInWidth(gui, ws, 0, status);
}

int CMarkString::CreateInWidth(Gui* gui, const wchar_t* ws, int width,
	CMarkStringStatus& status)
{
	Assert(ws != NULL);
	
	ReleaseAll();
	
	m_nWidth = 0;
	m_nHeight = m_pPainter->GetTextHeight(L" ");
	
	if (WideStrEmpty(ws))
	{
		return 0;
	}
	
	char old_font[FONT_SIZE];
	
	CopyString(old_font, sizeof(old_font), m_pPainter->GetFont());
	
	const char* cur_font = old_font;

	if (!StringEmpty(status.GetFont()))
	{
		if (m_pPainter->SetFont(status.GetFont()))
		{
			cur_font = status.GetFont();
			
			m_nHeight = m_pPainter->GetTextHeight(L" ");
		}
	}
	
	TAutoMem<wchar_t, 256, TCoreAlloc> auto_buf(wcslen(ws) + 1);
	wchar_t* buf = auto_buf.GetBuffer();
	size_t count = 0;
	const wchar_t* p = ws;
	
	while (*p)
	{
		if (*p == L'{')
		{
			wchar_t flag = p[1];
			
			if ((flag == L'c') || (flag == L'C'))
			{
				// 颜色改变
				const wchar_t* beg = p + 2;
				const wchar_t* end = wcschr(beg, L'}');
				
				if (end != NULL)
				{
					AddStringPart(buf, count, status);

					size_t color_size = end - beg;
					TAutoMem<char, 32, TCoreAlloc> auto_buf(color_size * 2 + 1);
					char* color_str = auto_buf.GetBuffer();
					result_wstring ws(beg, end - beg);

					ToString(ws.c_str(), color_str, color_size * 2 + 1);
					//ToStringLen(beg, end - beg, color_str, 
					//	color_size * 2 + 1);

					status.SetColor(GuiUtil_ColorToInt(color_str));
					p = end + 1;
					
					continue;
				}
			}
			else if ((flag == L'f') || (flag == L'F'))
			{
				// 字体改变
				const wchar_t* beg = p + 2;
				const wchar_t* end = wcschr(beg, L'}');
				
				if (end != NULL)
				{
					AddStringPart(buf, count, status);
					
					status.SetFont(beg, end - beg);
					
					if (!StringEmpty(status.GetFont()))
					{
						if (m_pPainter->SetFont(status.GetFont()))
						{
							cur_font = status.GetFont();
						}
					}
					else
					{
						if (m_pPainter->SetFont(old_font))
						{
							cur_font = old_font;
						}
					}

					p = end + 1;
					
					continue;
				}
			}
			else if ((flag == L'i') || (flag == L'I'))
			{
				// 显示图元
				const wchar_t* beg = p + 2;
				const wchar_t* end = wcschr(beg, L'}');
				
				if (end != NULL)
				{
					AddStringPart(buf, count, status);
					
					result_wstring ws(beg, end - beg);
					char image[128];
					
					ToString(ws.c_str(), image, sizeof(image));
					//ToStringLen(beg, end - beg, image, sizeof(image));
					
					AddImagePart(gui, image, status);
					
					int w = m_Parts.back()->nWidth;

					m_nWidth += w;
					
					if ((width > 0) && (m_nWidth > width))
					{
						// 如果一个图元就超过一行的宽度，则将其作为一行
						if (!m_Parts.empty())
						{
							m_nWidth -= w;
							CORE_DELETE(m_Parts.back());
							m_Parts.pop_back();
							break;
						}
					}

					int h = m_Parts.back()->nHeight;

					if (h > m_nHeight)
					{
						m_nHeight = h;
					}
					
					p = end + 1;
					
					continue;
				}
			}
			else if ((flag == L'r') || (flag == L'R'))
			{
				// 引用数据
				const wchar_t* beg = p + 2;
				const wchar_t* end = wcschr(beg, L'}');
				
				if (end != NULL)
				{
					AddStringPart(buf, count, status);
					
					status.SetRef(beg, end - beg);
					
					p = end + 1;
					
					continue;
				}
			}
		}

		int w = m_pPainter->GetTextLenWidth(p, 1);
		
		if ((width > 0) && ((m_nWidth + w) > width))
		{
			// 如果一个字符就超过一行的宽度，则将其作为一行
			if ((!m_Parts.empty()) || (count > 0))
			{
				break;
			}
		}

		m_nWidth += w;
		
		buf[count++] = *p++;
	}
	
	AddStringPart(buf, count, status);
	
	if (strcmp(cur_font, old_font) != 0)
	{
		m_pPainter->SetFont(old_font);
	}
	
	return (int)(p - ws);
}

bool CMarkString::Write(float seconds, int x, int y, unsigned int color)
{
	if (m_Parts.empty())
	{
		return true;
	}
	
	// 需要的高度
	int height = GetHeight();

	unsigned int old_color = color;
	
	char old_font[FONT_SIZE];
	
	CopyString(old_font, sizeof(old_font), m_pPainter->GetFont());
	
	const char* cur_font = old_font;
	
	for (size_t i = 0; i < m_Parts.size(); ++i)
	{
		if (m_Parts[i]->nType == PARTTYPE_STRING)
		{
			part_string_t* p = (part_string_t*)m_Parts[i];
			
			if (!p->strFont.empty())
			{
				if (m_pPainter->SetFont(p->strFont.c_str()))
				{
					cur_font = p->strFont.c_str();
				}
			}
			else
			{
				if (m_pPainter->SetFont(old_font))
				{
					cur_font = old_font;
				}
			}

			if (p->nColor != 0)
			{
				color = p->nColor;
			}
			else
			{
				color = old_color;
			}
			
			int h = m_pPainter->GetTextHeight(p->wsContent.c_str());
			int w = m_pPainter->GetTextWidth(p->wsContent.c_str());
			
			m_pPainter->WriteText(x, y + (height - h), color, 
				p->wsContent.c_str());

			x += w;
		}
		else if (m_Parts[i]->nType == PARTTYPE_IMAGE)
		{
			part_image_t* p = (part_image_t*)m_Parts[i];
			
			if (p->Image.IsReady())
			{
				int h = p->Image.GetHeight();
				int w = p->Image.GetWidth();

				p->Image.Draw(CGuiConst::DRAWMODE_TILE,
					x, y + (height - h), x + w, y + height);
			
				x += w;
				x += m_pPainter->GetTextInterval();
			}
		}
	}
	
	if (strcmp(cur_font, old_font) != 0)
	{
		m_pPainter->SetFont(old_font);
	}
	
	return true;
}

bool CMarkString::GetCenterStart(int left, int top, int right, int bottom, 
	int& start_x, int& start_y)
{
	int h = GetHeight();
	int w = GetWidth();
	
	// 起始位置
	start_x = left + (right - left - w) / 2;
	start_y = top + (bottom - top - h) / 2;

	return true;
}

bool CMarkString::GetLeftStart(int left, int top, int right, int bottom, 
							   int& start_x, int& start_y)
{
	int h = GetHeight();
	
	start_x = left;
	start_y = top + (bottom - top - h) / 2;
	
	return true;
}

bool CMarkString::GetRightStart(int left, int top, int right, int bottom, 
								int& start_x, int& start_y)
{
	int h = GetHeight();
	int w = GetWidth();
	
	start_x = right - w;
	start_y = top + (bottom - top - h) / 2;
	
	return true;
}

int CMarkString::GetHeight() const
{
	return m_nHeight;
}

int CMarkString::GetWidth() const
{
	return m_nWidth;
}

int CMarkString::GetInPart(int offset_x, int offset_y)
{
	if ((offset_x < 0) || (offset_x >= m_nWidth))
	{
		return -1;
	}

	if ((offset_y < 0) || (offset_y >= m_nHeight))
	{
		return -1;
	}
	
	int x1 = 0;
	int y2 = m_nHeight;

	for (size_t i = 0; i < m_Parts.size(); ++i)
	{
		part_base_t* p = m_Parts[i];
		int x2 = x1 + p->nWidth;
		int y1 = m_nHeight - p->nHeight;

		if ((offset_x >= x1) && (offset_x < x2) 
			&& (offset_y >= y1) && (offset_y < y2))
		{
			return (int)i;
		}

		x1 = x2;
	}
	
	return -1;
}

size_t CMarkString::GetPartCount() const
{
	return m_Parts.size();
}

int CMarkString::GetPartType(size_t index) const
{
	Assert(index < m_Parts.size());

	return m_Parts[index]->nType;
}

const wchar_t* CMarkString::GetPartRef(size_t index) const
{
	Assert(index < m_Parts.size());
	
	return m_Parts[index]->wsRef.c_str();
}

const wchar_t* CMarkString::GetPartContent(size_t index) const
{
	Assert(index < m_Parts.size());
	
	if (m_Parts[index]->nType == PARTTYPE_STRING)
	{
		part_string_t* p = (part_string_t*)m_Parts[index];
		
		return p->wsContent.c_str();
	}
	
	return L"";
}

