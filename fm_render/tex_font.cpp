//--------------------------------------------------------------------
// 文件名:		tex_font.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_font.h"
#include "painter.h"
#include "render.h"
#include "texture.h"
#include "../visual/i_texture.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// 这边需加0.5个像素偏移,有些显卡需要这个
//#define SETTEXX(x) ((float(x) - 0.5F))
//#define SETTEXY(y) (-(float(y) - 0.5F))

#define INIT_VERTEX_FONT(v, x1, y1, z1, w1, color1, tu1, tv1) \
	{ v->x = x1; v->y = y1; v->z = z1; v->w = w1; \
	v->color = color1; v->tu = tu1; v->tv = tv1; } 

static inline float get_tex_uv(int value, int maximum)
{
	return float(value) / float(maximum);
}

// CTexFont

CTexFont* CTexFont::NewInstance(Painter* pPainter, Render* pRender, 
	const char* font_tex, size_t tex_width, size_t tex_height, size_t height, 
	size_t interval, const int* char_info, size_t char_num)
{
	CTexFont* p = (CTexFont*)CORE_ALLOC(sizeof(CTexFont));

	new (p) CTexFont(pPainter, pRender, font_tex, tex_width, tex_height,
		height, interval, char_info, char_num);

	return p;
}

CTexFont::CTexFont(Painter* pPainter, Render* pRender, const char* font_tex, 
	size_t tex_width, size_t tex_height, size_t height, size_t interval, 
	const int* char_info, size_t char_num)
{
	Assert(pPainter != NULL);
	Assert(pRender != NULL);
	Assert(font_tex != NULL);
	Assert(height > 0);

	m_pPainter = pPainter;
	m_pRender = pRender;
	m_pTex = NULL;
	m_strFontTex = font_tex;
	m_nTexWidth = tex_width;
	m_nTexHeight = tex_height;
	m_nHeight = height;
	m_nInterval = (interval / 2) * 2;
	
	for (size_t i = 0; i < char_num; ++i)
	{
		int ch = char_info[0];

		if ((ch > 0) && (ch < 0x10000))
		{
			char_data_t* info = (char_data_t*)CORE_ALLOC(sizeof(char_data_t));

			info->nLeft = char_info[1];
			info->nTop = char_info[2];
			info->nRight = char_info[3];
			info->nBottom = char_info[4];

			m_CharInfo.Add(ch, info);
		}

		char_info += 5;
	}
}

CTexFont::~CTexFont()
{
	Shutdown();
	
	TPodHashMap<int, char_data_t*, TPodTraits<int>, 
		TCoreAlloc>::iterator it = m_CharInfo.Begin();
	TPodHashMap<int, char_data_t*, TPodTraits<int>, 
		TCoreAlloc>::iterator end = m_CharInfo.End();

	for (; it != end; ++it)
	{
		CORE_FREE(it.GetData(), sizeof(char_data_t));
	}
}

void CTexFont::Release()
{
	CORE_DELETE(this);
}

bool CTexFont::Initialize()
{
	// 贴图
	m_pTex = m_pRender->CreateGlyphTexture(m_strFontTex.c_str(), false);

	if (NULL == m_pTex)
	{
		CORE_TRACE("(CTexFont::Initialize)create texture failed");
		CORE_TRACE(m_strFontTex.c_str());
		return false;
	}

	return true;
}

bool CTexFont::Shutdown()
{
	SAFE_RELEASE(m_pTex);
	
	return true;
}

int CTexFont::GetInterval()
{
	return (int)m_nInterval;
}

int CTexFont::GetTextHeight()
{
	return (int)m_nHeight;
}

int CTexFont::GetTextLenWidth(const wchar_t* text, size_t len)
{
	Assert(text != NULL);
	Assert(len <= wcslen(text));
	
	if (0 == len)
	{
		return 0;
	}

	float width = 0.0F;
	
	for (size_t i = 0; i < len; ++i)
	{
		wchar_t ch = text[i];

		if (ch == 0x20)
		{
			width += float(m_nHeight + m_nInterval) * 0.5F;
			continue;
		}
		
		char_data_t* pCharInfo = GetCharInfo(ch);
		
		if (NULL == pCharInfo)
		{
			// 不存在的字符作空格处理
			if (ch < 256)
			{
				width += float(m_nHeight + m_nInterval) * 0.5F;
			}
			else
			{
				width += float(m_nHeight + m_nInterval);
			}
			
			continue;
		}
		
		int left = pCharInfo->nLeft;
		int right = pCharInfo->nRight;
		
		if (ch < 256)
		{
			width += float(right - left) + float(m_nInterval) * 0.5F;
		}
		else
		{
			width += float(right - left) + float(m_nInterval);
		}
	}
	
	return (int)width;
}

bool CTexFont::WriteTextLen(int x, int y, float depth_z, float depth_w, 
	unsigned int color, const wchar_t* text, size_t len, float ratio)
{
	Assert(text != NULL);
	Assert(len <= wcslen(text));
	
	if (0 == len)
	{
		return true;
	}
	
	if (len > LINE_CHAR_NUM)
	{
		len = LINE_CHAR_NUM;
	}
	
	// 计算要画的三角形数量
	int primitive_count = 0;

	for (size_t k = 0; k < len; ++k)
	{
		wchar_t ch = text[k];
		
		if (ch == 0x20)
		{
			continue;
		}

		if (GetCharInfo(ch) == NULL)
		{
			continue;
		}

		primitive_count += 2;
	}

	if (primitive_count == 0)
	{
		// 无内容绘制直接返回
		return true;
	}

	CPaintBatchs::vertex_pict_t* pv = m_pPainter->NewPictBatch(
		IRenderDrawOp::DRAW_TRIANGLES, primitive_count, m_pTex->GetShaderTex());
	float x1 = (float)x;
	float y1 = (float)y;
	size_t count = 0;
	
	for (size_t i = 0; i < len; i++)
	{
		wchar_t ch = text[i];

		if (ch == 0x20)
		{
			x1 += float(m_nHeight + m_nInterval) * ratio * 0.5F;
			continue;
		}

		char_data_t* pCharInfo = GetCharInfo(ch);
		
		if (NULL == pCharInfo)
		{
			// 不存在的字符作空格处理
			if (ch < 256)
			{
				x1 += float(m_nHeight + m_nInterval) * ratio * 0.5F;
			}
			else
			{
				x1 += float(m_nHeight + m_nInterval) * ratio;
			}

			continue;
		}

		int left = pCharInfo->nLeft;
		int top = pCharInfo->nTop;
		int right = pCharInfo->nRight;
		int bottom = pCharInfo->nBottom;
		float u1 = get_tex_uv(left, (int)m_nTexWidth);
		float v1 = get_tex_uv(top, (int)m_nTexHeight);
		float u2 = get_tex_uv(right, (int)m_nTexWidth);
		float v2 = get_tex_uv(bottom, (int)m_nTexHeight);
		float x2 = x1 + (right - left)* ratio;
		float y2 = y1 + (bottom - top)* ratio;
		
		INIT_VERTEX_FONT(pv, SETTEXX(x1), SETTEXY(y1), depth_z, depth_w, 
			color, u1, v1);
		pv++;
		INIT_VERTEX_FONT(pv, SETTEXX(x2), SETTEXY(y1), depth_z, depth_w, 
			color, u2, v1);
		pv++;
		INIT_VERTEX_FONT(pv, SETTEXX(x1), SETTEXY(y2), depth_z, depth_w, 
			color, u1, v2);
		pv++;
		INIT_VERTEX_FONT(pv, SETTEXX(x2), SETTEXY(y1), depth_z, depth_w, 
			color, u2, v1);
		pv++;
		INIT_VERTEX_FONT(pv, SETTEXX(x1), SETTEXY(y2), depth_z, depth_w, 
			color, u1, v2);
		pv++;
		INIT_VERTEX_FONT(pv, SETTEXX(x2), SETTEXY(y2), depth_z, depth_w, 
			color, u2, v2);
		pv++;
		
		float w;
		
		if (ch < 256)
		{
			w = float(right - left) + float(m_nInterval) * 0.5F;
		}
		else
		{
			w = float(right - left) + float(m_nInterval);
		}

		x1 += w * ratio;
		count++;
	}

	return true;
}
