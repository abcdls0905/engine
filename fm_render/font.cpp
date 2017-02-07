//--------------------------------------------------------------------
// 文件名:      font.cpp
// 内  容:        
// 说  明:        
// 创建日期:    2007年3月26日
// 创建人:      陆利民
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "font.h"
#include "font_manager.h"
#include "font_outline.h"
#include "render.h"
#include "painter.h"
#include "dynamic_tex.h"
#include "../public/core_mem.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../public/core_file.h"
#include "../visual/vis_utils.h"
#include "../utils/language.h"


#ifdef _COMPRESSED_VERTEX
	#define INIT_VERTEX_FONT(v, x1, y1, z1, w1, color1, tu1, tv1) \
		{ v->x = Vis_ConvertFloatToHalfFloat(x1); v->y = Vis_ConvertFloatToHalfFloat(y1); v->z = Vis_ConvertFloatToHalfFloat(z1); v->w = Vis_ConvertFloatToHalfFloat(w1); v->color = color1; \
		v->tu = Vis_ConvertFloatToHalfFloat(tu1); v->tv = Vis_ConvertFloatToHalfFloat(tv1); } 
#else
	#define INIT_VERTEX_FONT(v, x1, y1, z1, w1, color1, tu1, tv1) \
		{ v->x = x1; v->y = y1; v->z = z1; v->w = w1; v->color = color1; \
		v->tu = tu1; v->tv = tv1; } 
#endif

static inline float get_tex_uv(int value, int maximum)
{
    return (float(value)) / float(maximum);
}

inline float get_fade_value(float per, int type)
{
	switch(type)
	{
	case FONT_FADE_UP:
		{
			return per;
		}break;
	case FONT_FADE_DOWN:
		{
			return 1-per;
		}break;
	case FONT_FADE_MIDDLE:
		{
			if(per <= 0.5)
				return per / 0.5f;
			else
				return (1.0f - per) / 0.5f;
		}break;
	default:
		return 1.0f;
		break;
	}
}

inline void fade_color(unsigned char* bits, int cols, float per, int type)
{
	switch(type)
	{
	case FONT_FADE_UP:
		{
			for(int i = 0; i < cols; i++)
				bits[i] = (unsigned char)((float)bits[i]*per);
		}break;
	case FONT_FADE_DOWN:
		{
			for(int i = 0; i < cols; i++)
				bits[i] = (unsigned char)((float)bits[i]*(1-per));
		}break;
	case FONT_FADE_MIDDLE:
		{
			if(per <= 0.5)
			{
				for(int i = 0; i < cols; i++)
					bits[i] = (unsigned char)((float)bits[i]*(per/0.5));
			}
			else
			{
				for(int i = 0; i < cols; i++)
					bits[i] = (unsigned char)((float)bits[i]*(1.0f-per/0.5));
			}
		}break;
	}
}

// text, 原始unicode字符串
// begin, 深测起始下标
// return, 深测到的组合字符所需要的unicode个数
inline size_t get_next_language_character(const wchar_t * text, 
    size_t begin)
{
    if (text == NULL)
    {
        return 0;
    }

    size_t len = 0;
    size_t end = wcslen(text);

    if (end == 0)
    {
        return 0;
    }

    if (begin >= end)
    {
        return 1;
    }

    // 泰文处理
    if (FxLanguage::IsThaiCharacter(text[begin]))
    {
        // 是否是泰文的辅音
        if (FxLanguage::IsThaiCONS(text[begin]))
        {
            if (begin + 1 < end)
            {
                // 是否是泰文的第一个合成注音字符
                if (FxLanguage::IsThaiCompose1(text[begin + 1]))
                {
                    if (begin + 2 < end)
                    {
                        // 是否是泰文的第二个合成注音字符
                        if (FxLanguage::IsThaiCompose2(text[begin + 2]))
                        {
                            return 3;
                        }

                        return 2;
                    }

                    return 2;
                }

                return 1;
            }

            return 1;
        }

        return 1;
    }

    // 越南文处理
    // 越南文只有单元音后面跟音调合成
    if (FxLanguage::IsVietnameseSYan(text[begin]))
    {
        if (begin + 1 < len)
        {
            if (FxLanguage::IsVietnameseVoiceLevel(text[begin + 1]))
            {
                return 2;
            }

            return 1;
        }

        return 1;
    }

    return 1;
}

inline int get_y_offset(FT_Face face)
{
	/* Handy routines for converting from fixed point */
	#define FT_FLOOR(X)	((X & -64) / 64)
	#define FT_CEIL(X)	(((X + 63) & -64) / 64)

	FT_Fixed scale = face->size->metrics.y_scale;
	int ascent  = FT_CEIL(FT_MulFix(face->ascender, scale));
	int yoffset = ascent - FT_FLOOR(face->glyph->metrics.horiBearingY);
	return yoffset;
}

CFont* CFont::NewInstance(Painter* pPainter, Render* pRender, 
	CFontManager* pManager, const char* font_file, unsigned int tex_size, 
	unsigned int height, unsigned int interval, bool bold,
	bool smooth, bool italic, bool underline, bool strike_out, bool var_extent, 
	bool outline, int outline_num, const FmVec3* outline_color, int fade_type, 
	unsigned int shadow_color, int shadow_x, int shadow_y)
{
	CFont* p = (CFont*)CORE_ALLOC(sizeof(CFont));

	new (p) CFont(pPainter, pRender, pManager, font_file, tex_size, height, 
		interval, bold, smooth, italic, underline, strike_out, var_extent, 
		outline, outline_num, outline_color, fade_type, shadow_color, shadow_x, shadow_y);

	return p;
}

CFont::CFont(Painter* pPainter, Render* pRender, 
	CFontManager* pManager, const char* font_file, unsigned int tex_size, 
	unsigned int height, unsigned int interval, bool bold,
    bool smooth, bool italic, bool underline, bool strike_out, bool var_extent, 
	bool outline, int outline_num, const FmVec3* outline_color, 
	int fade_type, unsigned int shadow_color, int shadow_x, int shadow_y)
{
    Assert(pPainter != NULL);
    Assert(pRender != NULL);
	Assert(pManager != NULL);
    Assert(font_file != NULL);
    Assert(height > 0);
    Assert((tex_size > 0) && (tex_size <= 2048));

    m_pPainter = pPainter;
	m_pRender = pRender;
    m_pFontManager = pManager;
    m_strFontFile = font_file;
	m_nTexSize = tex_size;
    m_nHeight = height;
    m_nInterval = interval;
    m_bBold = bold;
	m_bSmooth = smooth;
    m_bItalic = italic;
    m_bUnderline = underline;
	m_bStrikeOut = strike_out;
    m_bVarExtent = var_extent;
	m_bOutline = outline;
	if(outline_color)
		m_nOutlineColor = *outline_color;
	else
		m_nOutlineColor = FmVec3(0, 0, 0);
    m_nDefShadowColor = shadow_color;
    m_nShadowColor = shadow_color;
    m_nShadowOffsetX = shadow_x;
    m_nShadowOffsetY = shadow_y;
	m_nTexUnitHeight = height;
	m_FontFace = NULL;
    m_nFontIndex = -1;
    m_nTexIndex = -1;
	m_bNoBitmap = false;
	m_nOutlineNum = outline_num;

	m_nFadeType = fade_type;
	if(outline_num > 0)
	{
		FT_Stroker_New(m_pFontManager->GetFontLibrary(), &m_Stroker);
		FT_Stroker_Set(m_Stroker,
			(int)(outline_num * 64),
			FT_STROKER_LINECAP_ROUND,
			FT_STROKER_LINEJOIN_ROUND,
			0);
	}
	else
	{
		m_Stroker = 0;
	}
}

CFont::~CFont()
{
    Shutdown();
}

void CFont::Release()
{
	m_pRender->UnloadCache(m_strFontFile.c_str());
	CORE_DELETE(this);
}

bool CFont::Initialize()
{    
    if ((m_nHeight + 3) > MAX_FONT_HEIGHT)
	{
		return false;
	}

	if(!m_pRender->LoadCache(m_strFontFile.c_str(), false))
	{ 
		char buf[256];
		sprintf(buf, "can't open font file %s", m_strFontFile.c_str());
		CORE_TRACE(buf);
	}
	
	size_t size;
	const void* data = m_pRender->GetCacheData(m_strFontFile.c_str(), size);

	int error = FT_New_Memory_Face(m_pFontManager->GetFontLibrary(), (const unsigned char*)data, (FT_Long)size, 0, &m_FontFace);
	
	if (error == FT_Err_Unknown_File_Format)
	{
		return false;
	}
	else if (error)
	{
		return false;
	}

	error = FT_Set_Pixel_Sizes(m_FontFace, 0, m_nHeight); 

	if (error)
	{
		return false;
	}

	if (m_bItalic)
	{
		float lean = 0.5F;
		FT_Matrix matrix;

		matrix.xx = 0x10000L;
		matrix.xy = (FT_Fixed)(lean * (float)0x10000L);
		matrix.yx = 0;
		matrix.yy = 0x10000L;

		FT_Set_Transform(m_FontFace, &matrix, NULL);
	}

	

	if (!m_pFontManager->AddFont(this, m_nHeight, m_nTexSize, m_nTexSize,
		m_bOutline, IsUseBigTexture(), m_nFontIndex, m_nTexIndex))
	{
		return false;
	}

	m_nTexUnitHeight = m_nHeight + 1;

	if (m_bOutline)
	{
		m_nTexUnitHeight += 2;
	}

	if(this->m_nOutlineNum)
	{
		m_nTexUnitHeight += m_nOutlineNum;
	}

	if (m_bItalic || m_bBold || m_bSmooth)
	{
		m_bNoBitmap = true;
	}

    return true;
}

bool CFont::Shutdown()
{
	if (m_Stroker)
    {
        FT_Stroker_Done(m_Stroker);
		m_Stroker = NULL;
    }

    if (m_FontFace)
	{
		FT_Done_Face(m_FontFace);
		m_FontFace = NULL;
	}

    return true;
}

int CFont::GetInterval()
{
    return m_nInterval;
}

int CFont::GetTextHeight()
{
    return m_nTexUnitHeight;
}

bool CFont::FillCharInfo(wchar_t ch, char_data_t& info, bool commit_gpu)
{
#ifdef NEW_FREETYPE_FONT
	long outWidth, outHeight;
	Rect outRect;
	int xAdvance;
	unsigned char* data = getGlyphBitmap(ch, outWidth, outHeight, outRect, xAdvance);
	if(data == 0)
		return false;


	info.nCharWidth = outRect.w;
	info.nCharHeight = outRect.h;
	info.nXOffset = outRect.x;
	info.nYOffset = m_nHeight + outRect.y;
	info.nXAdvance = xAdvance;

	if(commit_gpu)
	{
		char_data_t* pCharInfo = GetCharInfo(ch);
		pCharInfo->nCharSlot = m_pFontManager->GetOneSlot(m_nTexIndex);
		m_pFontManager->SetSlotData(m_nTexIndex, pCharInfo->nCharSlot, m_nFontIndex, ch);

		CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
		int slot_left;
		int slot_top;

		m_pFontManager->GetSlotPosition(m_nTexIndex, pCharInfo->nCharSlot,
			slot_left, slot_top);

		int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);

		if(m_nOutlineNum > 0)
		{
			static unsigned char s_nFontData24[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT*3];
			memset(s_nFontData24, 0, slot_size*slot_size*3);
			unsigned char* dst_data = s_nFontData24;
			unsigned char* src_data = data;
			for (int y = 0; y < outHeight; ++y)
			{
				unsigned char* prow = dst_data;
				for(int x = 0; x < outWidth; x++)
				{
					prow[x*3+0] = src_data[x*2+0];
					prow[x*3+1] = src_data[x*2+1];
					prow[x*3+2] = 0xff;
				}
				dst_data += slot_size*3;
				src_data += outWidth*2;
			}

			m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
				slot_size, slot_size, s_nFontData24);
		}
		else
		{
			static unsigned char s_nFontData[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT];
			memset(s_nFontData, 0, MAX_FONT_HEIGHT * MAX_FONT_HEIGHT);

			unsigned char* dst_data = s_nFontData;
			unsigned char* src_data = data;
			for (int r = 0; r < outHeight; ++r)
			{
				memcpy(dst_data, src_data, outWidth);
				dst_data += slot_size;
				src_data += outWidth;
			}
			m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
				slot_size, slot_size, s_nFontData);
		}
	}
	if(this->m_nOutlineNum > 0)
		delete(data);
#else

	static unsigned char s_nFontData24[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT*3];
			
	static unsigned char s_nFontData[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT];

	FT_UInt glyph_index = FT_Get_Char_Index(m_FontFace, ch);

	int error;

	if (m_bNoBitmap)
	{
		error = FT_Load_Glyph(m_FontFace, glyph_index, FT_LOAD_NO_BITMAP);
	}
	else
	{
		error = FT_Load_Glyph(m_FontFace, glyph_index, FT_LOAD_DEFAULT);
	}

	if (error)
	{
		return false;
	}

	if (m_bBold)
	{
		error = FT_Outline_Embolden(&m_FontFace->glyph->outline, 0x1 << 6);
	}

	if(m_nOutlineNum < 1)
	{
		error = FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);

		if (error)
		{
			return false;
		}

		info.nCharWidth = m_FontFace->glyph->bitmap.width;
		info.nCharHeight = m_FontFace->glyph->bitmap.rows;
		info.nXAdvance = (short)(m_FontFace->glyph->advance.x >> 6);
		info.nXOffset = (short)m_FontFace->glyph->bitmap_left;
		info.nYOffset = m_nHeight - m_FontFace->glyph->bitmap_top;
		info.nCharSlot = m_pFontManager->SetSlotData(m_nTexIndex, m_nFontIndex, ch);
		int slot_left;
		int slot_top;
		m_pFontManager->GetSlotPosition(m_nTexIndex, info.nCharSlot,
		slot_left, slot_top);
		int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);

		FT_GlyphSlot glyph = m_FontFace->glyph;

		memset(s_nFontData, 0, sizeof(s_nFontData));

		int rows = glyph->bitmap.rows;
		int cols = glyph->bitmap.width;

		unsigned char* dst_data = s_nFontData;
		unsigned int src_pitch = glyph->bitmap.pitch;
		unsigned char* src_data = glyph->bitmap.buffer; 

		if (glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					unsigned char v = src_data[c >> 3];
					unsigned char mask = 0x1 << (7 - (c & 0x7));

					if (v & mask)
					{
						dst_data[c] = 0xFF;
					}
					else
					{
						dst_data[c] = 0x0;
					}
				}

				dst_data += slot_size;
				src_data += src_pitch;
			}
		}
		else
		{
			for (int r = 0; r < rows; ++r)
			{
				memcpy(dst_data, src_data, cols);
				fade_color(dst_data, cols, r/float(rows), m_nFadeType);
				dst_data += slot_size;
				src_data += src_pitch;
			}
		}

		CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
		m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
					slot_size, slot_size, s_nFontData);
	}
	else
	{
		USE_FONT_OUTLINE;
		char * alpha_map = 0;
		int imgWidth=0;
		int imgHeight=0;
		int color_top=0;
		int color_bottom=0;
		FontRect outRect;
		int xAdvance;
		bool result = FontOutline::WriteOutline(m_pFontManager->GetFontLibrary(), ch, m_FontFace, 
			-1, Pixel32(255, 255, 255), Pixel32(0, 0, 0), 
			(float)m_nOutlineNum, alpha_map, imgWidth, imgHeight, color_top, color_bottom, outRect, xAdvance);
		info.nCharWidth = (unsigned short)outRect.w;
		info.nCharHeight = (unsigned short)outRect.h;
		info.nXAdvance = xAdvance;
		info.nYOffset = (short)(m_nHeight + outRect.y);
		info.nXOffset = (short)outRect.x;
	
		if(commit_gpu && result)
		{
			info.nCharSlot = m_pFontManager->SetSlotData(m_nTexIndex, m_nFontIndex, ch);
			CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
			int slot_left;
			int slot_top;
			m_pFontManager->GetSlotPosition(m_nTexIndex, info.nCharSlot,
			slot_left, slot_top);

			int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);
	
			if(IsUseBigTexture())
			{
				memset(s_nFontData24, 0, sizeof(s_nFontData24));
				unsigned char* pDst = s_nFontData24;
				unsigned int src_pitch = imgWidth;
				char* src_data = alpha_map;
				int rows = FM_MIN(imgHeight, MAX_FONT_HEIGHT);
				int dstOffset = 0;
				int srcOffset = 0;
				unsigned char* pSrc = (unsigned char*)src_data;
				int n = 0;
				float height = (float)(color_bottom - color_top);
				for(int y = 0; y < rows; y++)
				{
					float fade_per = 1.0f;
					if(y >= color_top && y <= color_bottom)
					{
						float per = n / height;
						fade_per = get_fade_value( per, this->m_nFadeType);
						n++;
					}
					else
					{
						fade_per = 1.0f;
					}

					for(int x = 0; x < imgWidth; x++)
					{
						pDst[x*3] = pSrc[x];
						pDst[x*3+1] = (unsigned char)(255.0f*fade_per);			
					}
					pDst += slot_size*3;
					pSrc += imgWidth;
				}
				m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
					slot_size, slot_size, s_nFontData24);
			}
			else
			{
				memset(s_nFontData, 0, sizeof(s_nFontData));
				unsigned char* dst_data = s_nFontData;
				unsigned int src_pitch = imgWidth;
				char* src_data = alpha_map;
				int rows = FM_MIN(imgHeight, MAX_FONT_HEIGHT);
				for(int y = 0; y < rows; y++)
				{
					memcpy(dst_data, src_data, src_pitch);
					dst_data += slot_size;
					src_data += src_pitch;
				}
				m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
					slot_size, slot_size, s_nFontData);
			}
		}
		else if(commit_gpu && ch == 32)
		{
			CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
			char_data_t* pCharInfo = GetCharInfo(ch);
			int slot_left;
			int slot_top;
			m_pFontManager->GetSlotPosition(m_nTexIndex, pCharInfo->nCharSlot,
			slot_left, slot_top);
			int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);

			if(IsUseBigTexture())
			{
				memset(s_nFontData24, 0, sizeof(s_nFontData24));
				m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
				slot_size, slot_size, s_nFontData24);
			}
			else
			{
				memset(s_nFontData, 0, sizeof(s_nFontData));
				m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
					slot_size, slot_size, s_nFontData);
			}
		}

		delete(alpha_map);
		if(!result)
		{
			return false;
		}
	}
#endif
	return true;
}

int CFont::GetTextLenWidth(const wchar_t* text, size_t len)
{
    Assert(text != NULL);
    
    if (0 == len)
    {
        return 0;
    }

    if (wcslen(text) == 0)
    {
        return 0;
    }
    
    float width = 0;

    if (m_bVarExtent)
    {
        size_t begin = 0;

        while (begin < len)
        {
			wchar_t ch = text[begin];
			int xadvance = 0;
			int xoffset = 0;
			int char_width = 0;
			int user_xoffset = 0;

			xadvance = get_hide_char_width(ch);
			if(xadvance > -1)
			{
				xadvance = m_nTexUnitHeight;
				char_width = xadvance;
				xoffset = 0;
			}
			else
			{
				char_data_t* pCharInfo = GetCharInfo(ch);

			
				if (pCharInfo->nCharSlot == 0xFFFF && !pCharInfo->bInit)
				{
					if(!FillCharInfo(ch, *pCharInfo))
					{
						if(!FillCharInfo(L'?', *pCharInfo))
						{
							Assert(0);
						}
					}
					pCharInfo->bInit = true;
				}
				xadvance = pCharInfo->nXAdvance;
				char_width = pCharInfo->nCharWidth;
				xoffset = pCharInfo->nXOffset;
				user_xoffset = pCharInfo->nUserXOffset;
			}

			
			
			if(begin +1 == len) //最后一个字符
			{
				width += xoffset;
				width += char_width;
			}
			else
			{
				width += float(xadvance);
				width += float(m_nInterval);
				width += float(user_xoffset);
			}
    
            ++begin;
        }

        return (int)width;
    }
    else
    {
        for (size_t i = 0; i < len; ++i)
        {
            if (text[i] < 256)
            {
                width += (m_nTexUnitHeight + 1) / 2 + m_nInterval;
            }
            else
            {
                width += m_nTexUnitHeight + m_nInterval;
            }
        }
        
        return (int)width;
    }
}

// 获取下一个语言字符的宽度
int CFont::GetNextLanguageCharacter(const wchar_t * text, const size_t begin)
{
	return (int)get_next_language_character(text, begin);
}

bool CFont::SetShadowColor(unsigned int color)
{
    // 当设置为0时还原成默认色
    if (color == 0) 
    {
        m_nShadowColor = m_nDefShadowColor;
    }
    else
    {
        m_nShadowColor = color;
    }
    
	return true;
}

bool CFont::WriteTextLen(int x, int y, float depth_z, float depth_w, 
	unsigned int color, const wchar_t* text, size_t len, float ratio)
{
    if (m_nShadowColor & 0xFF000000)
    {
        // 画文字阴影
        InnerWriteTextLen(x + m_nShadowOffsetX, y + m_nShadowOffsetY,
            depth_z, depth_w, m_nShadowColor, text, len, ratio);
    }

    return InnerWriteTextLen(x, y, depth_z, depth_w, color, text, len, ratio);
}

/*
bool CFont::AddToTexture(wchar_t ch)
{
	//仅描边字空格不进行绘制
	if(ch == 32 && this->m_nOutlineNum > 0)
		return false;
	
	if(ch == 32 && this->m_nOutlineNum > 0)
	{
		char_data_t* pCharInfo = GetCharInfo(ch);
		pCharInfo->nCharSlot = m_pFontManager->GetOneSlot(m_nTexIndex);
		m_pFontManager->SetSlotData(m_nTexIndex, pCharInfo->nCharSlot, m_nFontIndex, ch);
		return true;
	}
	
#ifdef NEW_FREETYPE_FONT
	long outWidth, outHeight;
	Rect outRect;
	int xAdvance;
	unsigned char* data = getGlyphBitmap(ch, outWidth, outHeight, outRect, xAdvance);
	char_data_t* pCharInfo = GetCharInfo(ch);
	
	if(data == 0)
		return false;

	pCharInfo->nCharSlot = m_pFontManager->GetOneSlot(m_nTexIndex);
	m_pFontManager->SetSlotData(m_nTexIndex, pCharInfo->nCharSlot, m_nFontIndex, ch);

	CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
	int slot_left;
	int slot_top;

	m_pFontManager->GetSlotPosition(m_nTexIndex, pCharInfo->nCharSlot,
		slot_left, slot_top);

	int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);

	if(m_nOutlineNum > 0)
	{
		static unsigned char s_nFontData24[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT*3];
		memset(s_nFontData24, 0, slot_size*slot_size*3);
		unsigned char* dst_data = s_nFontData24;
		unsigned char* src_data = data;
		for (int y = 0; y < outHeight; ++y)
		{
			unsigned char* prow = dst_data;
			for(int x = 0; x < outWidth; x++)
			{
				prow[x*3+0] = src_data[x*2+0];
				prow[x*3+1] = src_data[x*2+1];
				prow[x*3+2] = 0xff;
			}
			dst_data += slot_size*3;
			src_data += outWidth*2;
		}

		m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
			slot_size, slot_size, s_nFontData24);
	}
	else
	{
		static unsigned char s_nFontData[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT];
		memset(s_nFontData, 0, MAX_FONT_HEIGHT * MAX_FONT_HEIGHT);

		unsigned char* dst_data = s_nFontData;
		unsigned char* src_data = data;
		for (int r = 0; r < outHeight; ++r)
		{
			memcpy(dst_data, src_data, outWidth);
			dst_data += slot_size;
			src_data += outWidth;
		}
		m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
			slot_size, slot_size, s_nFontData);
	}
	if(m_nOutlineNum > 0)
		delete(data);
	
	pCharInfo->nXAdvance = xAdvance;
	pCharInfo->nCharWidth = outRect.w;
	pCharInfo->nCharHeight = outRect.h;
	pCharInfo->nXOffset = outRect.x;
	pCharInfo->nYOffset = m_nHeight + outRect.y ;
#else
	static unsigned char s_nFontData[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT];
	static unsigned char s_nFontData24[MAX_FONT_HEIGHT * MAX_FONT_HEIGHT*3];
	int color_top;
	int color_bottom;
	
	FT_UInt glyph_index = FT_Get_Char_Index(m_FontFace, ch);

	int error;

	if (m_bNoBitmap)
	{
		error = FT_Load_Glyph(m_FontFace, glyph_index, FT_LOAD_NO_BITMAP);
	}
	else
	{
		error = FT_Load_Glyph(m_FontFace, glyph_index, FT_LOAD_DEFAULT);
	}

	if (error)
	{
		return false;
	}

	if (m_bBold)
	{
		error = FT_Outline_Embolden(&m_FontFace->glyph->outline, 0x1 << 6);
	}

	char * alpha_map = 0;
	int imgWidth;
	int imgHeight;
	FT_GlyphSlot glyph = m_FontFace->glyph;

	char_data_t* pCharInfo = GetCharInfo(ch);
	if(m_nOutlineNum < 1)
	{
		error = FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);

		if (error)
		{
			Assert(0);
			return false;
		}
		pCharInfo->nCharWidth = glyph->bitmap.width;
		pCharInfo->nCharHeight = glyph->bitmap.rows;
		pCharInfo->nXOffset = m_FontFace->glyph->bitmap_left;
		pCharInfo->nYOffset = m_nHeight - m_FontFace->glyph->bitmap_top;
		pCharInfo->nXAdvance = glyph->advance.x >> 6;
	}
	else
	{
		USE_FONT_OUTLINE;
		FontRect r;
		int xAdvance;
		bool result = FontOutline::WriteOutline(m_pFontManager->GetFontLibrary(), ch, m_FontFace, -1, Pixel32(255, 255, 255), Pixel32(0, 0, 0), 
			m_nOutlineNum, alpha_map, imgWidth, imgHeight, color_top, color_bottom,r, xAdvance);
		Assert(result);
		pCharInfo->nCharHeight = imgHeight;
		pCharInfo->nCharWidth = imgWidth;
		pCharInfo->nXOffset = r.x;
		pCharInfo->nYOffset = m_nHeight + r.y;
		pCharInfo->nXAdvance = xAdvance;
	}
	
	pCharInfo->nCharSlot = m_pFontManager->GetOneSlot(m_nTexIndex);
	m_pFontManager->SetSlotData(m_nTexIndex, pCharInfo->nCharSlot, m_nFontIndex, ch);
	

	CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
	int slot_left;
	int slot_top;

	m_pFontManager->GetSlotPosition(m_nTexIndex, pCharInfo->nCharSlot,
		slot_left, slot_top);

	int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);
	int data_num = slot_size * slot_size;
	int rows = glyph->bitmap.rows;
	int cols = glyph->bitmap.width;

	memset(s_nFontData, 0, data_num);
	

	if(this->m_nOutlineNum < 1)
	{
		unsigned char* dst_data = s_nFontData;
		unsigned int src_pitch = glyph->bitmap.pitch;
		unsigned char* src_data = glyph->bitmap.buffer; 

		if (glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					unsigned char v = src_data[c >> 3];
					unsigned char mask = 0x1 << (7 - (c & 0x7));

					if (v & mask)
					{
						dst_data[c] = 0xFF;
					}
					else
					{
						dst_data[c] = 0x0;
					}
				}

				dst_data += slot_size;
				src_data += src_pitch;
			}
		}
		else
		{
			for (int r = 0; r < rows; ++r)
			{
				memcpy(dst_data, src_data, cols);
				fade_color(dst_data, cols, r/float(rows), m_nFadeType);
				dst_data += slot_size;
				src_data += src_pitch;
			}
		}
	}
	else
	{
		if(IsUseBigTexture())
		{
			memset(s_nFontData24, 0, sizeof(s_nFontData24));
			unsigned char* pDst = s_nFontData24;
			unsigned int src_pitch = imgWidth;
			char* src_data = alpha_map;
			int rows = FM_MIN(imgHeight, MAX_FONT_HEIGHT);
			int dstOffset = 0;
			int srcOffset = 0;
			unsigned char* pSrc = (unsigned char*)src_data;
			int n = 0;
			float height = color_bottom - color_top;
			for(int y = 0; y < rows; y++)
			{
				float fade_per = 1.0f;
				if(y >= color_top && y <= color_bottom)
				{
					float per = n / height;
					fade_per = get_fade_value( per, this->m_nFadeType);
					n++;
				}
				else
				{
					fade_per = 1.0f;
				}

				for(int x = 0; x < imgWidth; x++)
				{
					pDst[x*3] = pSrc[x];
					pDst[x*3+1] = 255*fade_per;			
				}
				pDst += slot_size*3;
				pSrc += imgWidth;
			}
			delete alpha_map;
		}
		else
		{
			unsigned char* dst_data = s_nFontData;
			unsigned int src_pitch = imgWidth;
			char* src_data = alpha_map;
			int rows = FM_MIN(imgHeight, MAX_FONT_HEIGHT);
			for(int y = 0; y < rows; y++)
			{
				memcpy(dst_data, src_data, src_pitch);
				dst_data += slot_size;
				src_data += src_pitch;
			}
			delete alpha_map;
		}
	}

	if (m_bUnderline)
	{
		if (m_bVarExtent)
		{
			memset(s_nFontData + slot_size * m_nHeight, 0xFF, 
				glyph->bitmap.width + m_nInterval);
		}
		else
		{
			if (ch < 256)
			{
				memset(s_nFontData + slot_size * m_nHeight, 0xFF, 
					(m_nTexUnitHeight + 1) / 2 + m_nInterval);
			}
			else
			{
				memset(s_nFontData + slot_size * m_nHeight, 0xFF, 
					m_nTexUnitHeight + m_nInterval);
			}
		}
	}

	if(IsUseBigTexture())
		m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
			slot_size, slot_size, s_nFontData24);
	else
		m_pRender->UpdateDynamicTex(pFontTex, 0, slot_left, slot_top,
			slot_size, slot_size, s_nFontData);

	
#endif
	return true;
}*/


bool CFont::AddToTexture(wchar_t ch)
{
	char_data_t* pCharInfo = GetCharInfo(ch);
	if(!FillCharInfo(ch, *pCharInfo, true))
		return false;

	return true;
}

// 填充顶点缓冲
size_t CFont::FillVertexBufferText(const wchar_t* text, int len, int x, int y, float depth_z, float depth_w,
	unsigned int color, float ratio)
{
	IShaderTex* pFirstTexture = m_pFontManager->GetFontTexture(m_nTexIndex);
	float x1 = float(x);
	float y1 = float(y);
	size_t count = 0;
	size_t begin = 0;

	int w = GetTextLenWidth(text, len);
//	m_pPainter->DrawRect(x1, y1, x1 + w, y1 + m_nTexUnitHeight, 0xffff0000);
    while (begin < (size_t)len)
    {
		wchar_t ch = text[begin];
		
		int w = get_hide_char_width(ch);
		if(w > -1)
		{
			x1 += w * ratio;
		}
		else
		{
			CPaintBatchs::vertex_pict_t* pv;
		
			if(m_nOutlineNum > 0)
			{
				pv = m_pPainter->NewPictOutlineBatch(
				IRenderDrawOp::DRAW_TRIANGLES, (int)1 * 2, pFirstTexture, &m_nOutlineColor, IsUseBigTexture(), this->m_nFadeType != 0);
			}
			else
			{
				pv = m_pPainter->NewPictA8Batch(
					IRenderDrawOp::DRAW_TRIANGLES, (int)1 * 2, pFirstTexture);
			}
			
			FillVertexBufferChar(pv, ch, x1, y1, depth_z, depth_w, color, ratio);
		}
        count++;
        begin++;
    }

    return count;
}

size_t CFont::FillVertexBuffer(CPaintBatchs::vertex_pict_t* pv, int len, 
    const wchar_t* text, float x, float y, float depth_z, float depth_w, 
	unsigned int color, float ratio)
{
    int slot_cols = m_pFontManager->GetSlotColCount(m_nTexIndex);
    int tex_width = m_pFontManager->GetTextureWidth(m_nTexIndex);
	int tex_height = m_pFontManager->GetTextureHeight(m_nTexIndex);
    int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);
	int unit_size = slot_size;
	float x1 = float(x);
	size_t count = 0;
	size_t begin = 0;

    while (begin < (size_t)len)
    {
		wchar_t ch = text[begin];
		FillVertexBufferChar(pv, ch, x, y, depth_z, depth_w, color, ratio);

		/*
        char_data_t* pCharInfo = GetCharInfo(ch);
		if (pCharInfo->nCharSlot == 0xFFFF)
		{
			if (!AddToTexture(ch))
			{
				ch = L'?';
				pCharInfo = GetCharInfo(ch);
				if (pCharInfo->nCharSlot == 0xFFFF)
				{
					if(!AddToTexture(ch))
						return 0;
				}
			}
		}
		
		Assert(pCharInfo->nCharSlot != 0xFFFF);
		
		int old_x = x1;
		x1 += pCharInfo->nXOffset;
		int slot = (unsigned int)pCharInfo->nCharSlot;
		int row = (unsigned int)slot / (unsigned int)slot_cols;
		int col = (unsigned int)slot % (unsigned int)slot_cols;
		int ux = col * slot_size;
		int vy = row * slot_size;
		int char_w = pCharInfo->nCharWidth;
		int char_h = pCharInfo->nCharHeight;
		//int char_w = unit_size;
		//int char_h = unit_size;
		float u1 = get_tex_uv(ux, tex_width);
		float v1 = get_tex_uv(vy, tex_height);
		float u2 = get_tex_uv(ux + char_w, tex_width);
		float v2 = get_tex_uv(vy + char_h, tex_height);
		float x2 = x1 + float(char_w) * ratio;

		float y1 = float(y) + pCharInfo->nYOffset + pCharInfo->nUserYOffset;
		float y2 = y1 + float(char_h) * ratio;

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
	
        float char_advance_x;

        if (m_bVarExtent)
        {
			//取得水平步长
			char_advance_x = pCharInfo->nXAdvance;
        }
        else
        {
            if (ch < 256)
            {
                char_advance_x = float((m_nTexUnitHeight + 1) / 2);
            }
            else
            {
                char_advance_x = float(m_nTexUnitHeight);
            }
        }

        char_advance_x *= ratio;
		x1 = old_x;
		x1 += char_advance_x + float(m_nInterval);
		x1 += (pCharInfo->nUserXOffset); 
		*/
        count++;
        begin++;
    }

    return count;
}

size_t CFont::FillVertexBufferChar(CPaintBatchs::vertex_pict_t* pv, 
    wchar_t ch, float &x, float &y, float depth_z, float depth_w, 
	unsigned int color, float ratio)
{
    int slot_cols = m_pFontManager->GetSlotColCount(m_nTexIndex);
    int tex_width = m_pFontManager->GetTextureWidth(m_nTexIndex);
	int tex_height = m_pFontManager->GetTextureHeight(m_nTexIndex);
    int slot_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);
	int unit_size = slot_size;
	float x1 = float(x);
 
	char_data_t* pCharInfo = GetCharInfo(ch);
	if (pCharInfo->nCharSlot == 0xFFFF)
	{
		if (!AddToTexture(ch))
		{
			ch = L'?';
			pCharInfo = GetCharInfo(ch);
			if (pCharInfo->nCharSlot == 0xFFFF)
			{
				if(!AddToTexture(ch))
					return 0;
			}
		}
	}
	
	Assert(pCharInfo->nCharSlot != 0xFFFF);
	
	float old_x = x1;
	if (m_bVarExtent)
		x1 += pCharInfo->nXOffset;
	int slot = (unsigned int)pCharInfo->nCharSlot;
	int row = (unsigned int)slot / (unsigned int)slot_cols;
	int col = (unsigned int)slot % (unsigned int)slot_cols;
	int ux = col * slot_size;
	int vy = row * slot_size;
	int char_w = pCharInfo->nCharWidth;
	int char_h = pCharInfo->nCharHeight;
	//int char_w = unit_size;
	//int char_h = unit_size;
	float u1 = get_tex_uv(ux, tex_width);
	float v1 = get_tex_uv(vy, tex_height);
	float u2 = get_tex_uv(ux + char_w, tex_width);
	float v2 = get_tex_uv(vy + char_h, tex_height);
	float x2 = x1 + float(char_w) * ratio;

	float y1 = float(y) + pCharInfo->nYOffset + pCharInfo->nUserYOffset;
	float y2 = y1 + float(char_h) * ratio;

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

    float char_advance_x;

    if (m_bVarExtent)
    {
		//取得水平步长
		char_advance_x = pCharInfo->nXAdvance;
    }
    else
    {
        if (ch < 256)
        {
            char_advance_x = float((m_nTexUnitHeight + 1) / 2);
        }
        else
        {
            char_advance_x = float(m_nTexUnitHeight);
        }
    }

    char_advance_x *= ratio;
	x1 = old_x;
	x1 += char_advance_x + float(m_nInterval);
	x1 += (pCharInfo->nUserXOffset); 
	
	x = x1;
	
    return 1;
}

bool CFont::InnerWriteTextLen(int x, int y, float depth_z, float depth_w, 
	unsigned int color, const wchar_t* text, size_t len, float ratio)
{
    Assert(text != NULL);
    
    if (0 == len)
    {
        return true;
    }
    
    unsigned int curcolor = color;
    IShaderTex* pFirstTexture = m_pFontManager->GetFontTexture(m_nTexIndex);
    // 只有阴影色不透明时才进行抗锯齿描边判定
    bool draw_outline = (m_nShadowColor & 0xFF000000) && m_bOutline; 

	/*
    if (draw_outline)
    {
        // 抗锯齿描边情况，应选画描边再画文字
        // 第一步画描边
        curcolor = m_nShadowColor;
        pFirstTexture = m_pFontManager->GetOutlineTexture(m_nTexIndex);
    }*/

    if (len > LINE_CHAR_NUM)
    {
        len = LINE_CHAR_NUM;
    }

	FillVertexBufferText(text, (int)len , x, y, depth_z, depth_w, color, ratio);

	/*
    CPaintBatchs::vertex_pict_t* pv;
	
	if(this->m_nOutlineNum > 0)
	{
		pv = m_pPainter->NewPictOutlineBatch(
		IRenderDrawOp::DRAW_TRIANGLES, (int)len * 2, pFirstTexture, &m_nOutlineColor, IsUseBigTexture(), this->m_nFadeType != 0);
	}
	else
		pv = m_pPainter->NewPictA8Batch(
			IRenderDrawOp::DRAW_TRIANGLES, (int)len * 2, pFirstTexture);

    FillVertexBuffer(pv, (int)len, text, float(x), float(y), depth_z, depth_w, 
		curcolor, ratio);

    if (draw_outline)
    {
        // 抗锯齿描边情况，第二遍画文字
		pv = m_pPainter->NewPictA8Batch(IRenderDrawOp::DRAW_TRIANGLES, (int)len * 2, 
			m_pFontManager->GetFontTexture(m_nTexIndex));
        FillVertexBuffer(pv, (int)len, text, float(x), float(y), depth_z, 
			depth_w, color, ratio);
    }*/

    return true;
}

CFont::char_data_t* CFont::GetCharInfo(wchar_t ch)
{
    TPodHashMap<wchar_t, char_data_t, TPodTraits<wchar_t>, 
		TCoreAlloc>::iterator it = m_HashCharInfo.Find(ch);

    if (it == m_HashCharInfo.End())
    {
        char_data_t data;
        
        data.nCharSlot = 0xFFFF;
		data.bInit = false;
        data.nCharWidth = 10;
		data.nXOffset = 0;
		data.nYOffset = 0;
		data.nUserYOffset = 0;
		data.nUserXOffset = 0;
        m_HashCharInfo.Add(ch, data);
        it = m_HashCharInfo.Find(ch);
    }

    return &it.GetData();
}

bool CFont::RemoveChar(wchar_t ch)
{
    TPodHashMap<wchar_t, char_data_t, TPodTraits<wchar_t>, 
		TCoreAlloc>::iterator it = m_HashCharInfo.Find(ch);

    if (it != m_HashCharInfo.End())
    {
        m_HashCharInfo.Erase(it);
        return true;
    }

    return false;
}

bool CFont::DumpTex(const char* file_name)
{
	Assert(file_name != NULL);

	CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
	CPaintBatchs::vertex_pict_t* pv = m_pPainter->NewPictA8Batch(
			IRenderDrawOp::DRAW_TRIANGLES, (int)1 * 2, pFontTex);

	float depth_z = 0.0f, depth_w = 1.0f;
	unsigned long color = 0xffffffff;

	float u1 = 0.0f;
	float v1 = 0.0f;
	float u2 = 1.0f;
	float v2 = 1.0f;

	float x1 = 0.0f;
	float y1 = 0.0f;
	float x2 = (float)m_pFontManager->GetTextureWidth(m_nTexIndex);
	float y2 = (float)m_pFontManager->GetTextureHeight(m_nTexIndex);

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


	int unit_size = m_pFontManager->GetSlotUnitSize(m_nTexIndex);
	int max_count = m_pFontManager->GetSlotCount(m_nTexIndex);
	
	for(int i = 0; i < max_count; i++)
	{
		int left, top;
		m_pFontManager->GetSlotPosition(m_nTexIndex, i, left, top);
		m_pPainter->DrawRect(left ,top , left + unit_size, top + unit_size, 0xffffffff);
	}
	m_pPainter->DrawRect((int)x1, (int)y1, (int)x2, (int)y2, 0xffff0000);
	/*
	CDynamicTex* pFontTex = m_pFontManager->GetFontTexture(m_nTexIndex);
	HRESULT hr = D3DX11SaveTextureToFile(m_pRender->GetDeviceContext(),
		pFontTex->GetTexture(), D3DX11_IFF_DDS, file_name);

	if (FAILED(hr))
	{
		return false;
	}

	CDynamicTex* pOutlineTex = m_pFontManager->GetOutlineTexture(m_nTexIndex);

	if (pOutlineTex)
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "outline_%s", file_name);

		HRESULT hr = D3DX11SaveTextureToFile(m_pRender->GetDeviceContext(),
			pOutlineTex->GetTexture(), D3DX11_IFF_DDS, fname);

		if (FAILED(hr))
		{
			return false;
		}
	}
	*/

	return true;
}

void CFont::SetXOffset(wchar_t ch, int offset)
{
	char_data_t* info =  GetCharInfo(ch);
	info->nUserXOffset = offset;
}

void CFont::SetYOffset(wchar_t ch, int offset)
{
	char_data_t* info =  GetCharInfo(ch);
	info->nUserYOffset = offset;
}

int CFont::GetXOffset(wchar_t ch)
{
	char_data_t* info =  GetCharInfo(ch);
	return info->nUserXOffset;
}

int CFont::GetYOffset(wchar_t ch)
{
	char_data_t* info =  GetCharInfo(ch);
	return info->nUserYOffset;
}

unsigned char* CFont::getGlyphBitmap(wchar_t theChar, long &outWidth, long &outHeight, Rect &outRect,int &xAdvance)
{
	bool invalidChar = true;
	unsigned char * ret = 0;

    do 
    {
        if (!m_FontFace)
            break;

        FT_UInt glyphIndex = FT_Get_Char_Index(m_FontFace, theChar);
        if(!glyphIndex)
            break;

		if (FT_Load_Glyph(m_FontFace,glyphIndex, FT_LOAD_DEFAULT))
            break;

		if (m_bBold)
			FT_Outline_Embolden(&m_FontFace->glyph->outline, 0x1 << 6);

		if(FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL))
			break;

		outRect.x   =	(float)(m_FontFace->glyph->metrics.horiBearingX >> 6);
        outRect.y	=	- (float)(m_FontFace->glyph->metrics.horiBearingY >> 6);
        outRect.w	=	(float)(m_FontFace->glyph->metrics.width  >> 6);
        outRect.h	=   (float)(m_FontFace->glyph->metrics.height >> 6);

        xAdvance = (static_cast<int>(m_FontFace->glyph->metrics.horiAdvance >> 6));

        outWidth  = m_FontFace->glyph->bitmap.width;
        outHeight = m_FontFace->glyph->bitmap.rows;
        ret = m_FontFace->glyph->bitmap.buffer;
		
		if(m_nFadeType != 0)
		{
			unsigned char* fade_ptr = m_FontFace->glyph->bitmap.buffer;
			for(int i = 0; i < m_FontFace->glyph->bitmap.rows; i++)
			{
				fade_color(fade_ptr, m_FontFace->glyph->bitmap.width, i/float(m_FontFace->glyph->bitmap.rows), this->m_nFadeType);
				fade_ptr += m_FontFace->glyph->bitmap.width;
			}
		}
		
        if (this->m_nOutlineNum > 0)
        {
            unsigned char* copyBitmap = new unsigned char[outWidth * outHeight];
            memcpy(copyBitmap,ret,outWidth * outHeight * sizeof(unsigned char));

            long bitmapWidth;
            long bitmapHeight;
            FT_BBox bbox;
            unsigned char* outlineBitmap = getGlyphBitmapWithOutline(theChar, bbox);
            if(outlineBitmap == 0)
            {
                ret = 0;
                delete [] copyBitmap;
                break;
            }

            bitmapWidth = (bbox.xMax - bbox.xMin)>>6;
            bitmapHeight = (bbox.yMax - bbox.yMin)>>6;

            long index;
            unsigned char* blendImage = new unsigned char[bitmapWidth * bitmapHeight * 2];
            memset(blendImage, 0, bitmapWidth * bitmapHeight * 2);
            for (int x = 0; x < bitmapWidth; ++x)
            {
                for (int y = 0; y < bitmapHeight; ++y)
                {
                    index = x + ( y * bitmapWidth );
                    blendImage[2 * index] = outlineBitmap[index];
                }
            }

            long maxX = FM_MIN(bitmapWidth, outWidth + m_nOutlineNum);
            long maxY = FM_MIN(bitmapHeight, outHeight + m_nOutlineNum);

			for (int x = m_nOutlineNum; x < maxX; ++x)
            {
                for (int y = m_nOutlineNum; y < maxY; ++y)
                {
                    index = x + ( y * bitmapWidth );

                    blendImage[2 * index + 1] = copyBitmap[outWidth * (y - m_nOutlineNum) + x - m_nOutlineNum];
                }
            }

            outRect.x = float(bbox.xMin >> 6);
            outRect.y = - float(bbox.yMax >> 6);

            xAdvance += bitmapWidth - (int)outRect.w;

            outRect.w =  (float)bitmapWidth;
            outRect.h =  (float)bitmapHeight;
            outWidth  = bitmapWidth;
            outHeight = bitmapHeight;

            delete [] outlineBitmap;
            delete [] copyBitmap;
            ret = blendImage;
        }

		invalidChar = false;
    } while (0);

    if (invalidChar)
    {
        outRect.w  = 0;
        outRect.h = 0;
        xAdvance = 0;

        return 0;
    }
    else
    {
       return ret;
    }
}

unsigned char * CFont::getGlyphBitmapWithOutline(wchar_t theChar, FT_BBox &bbox)
{
	unsigned char* ret = 0;

    FT_UInt gindex = FT_Get_Char_Index(m_FontFace, theChar);
    if (FT_Load_Glyph(m_FontFace, gindex, FT_LOAD_NO_BITMAP) != 0)
		return 0;
    
	if(m_bBold)
		FT_Outline_Embolden(&m_FontFace->glyph->outline, 0x1<<6);

    if (m_FontFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
		return 0;
   
    FT_Glyph glyph;
    if (FT_Get_Glyph(m_FontFace->glyph, &glyph) != 0)
		return 0;
  
	FT_Glyph_StrokeBorder(&glyph, m_Stroker, 0, 1);
    if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
    {
        FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
        FT_Glyph_Get_CBox(glyph,FT_GLYPH_BBOX_GRIDFIT,&bbox);
        long width = (bbox.xMax - bbox.xMin)>>6;
        long rows = (bbox.yMax - bbox.yMin)>>6;

        FT_Bitmap bmp;
        bmp.buffer = new unsigned char[width * rows];
        memset(bmp.buffer, 0, width * rows);
        bmp.width = (int)width;
        bmp.rows = (int)rows;
        bmp.pitch = (int)width;
        bmp.pixel_mode = FT_PIXEL_MODE_GRAY;
        bmp.num_grays = 256;

        FT_Raster_Params params;
        memset(&params, 0, sizeof (params));
        params.source = outline;
        params.target = &bmp;
        params.flags = FT_RASTER_FLAG_AA;
        FT_Outline_Translate(outline,-bbox.xMin,-bbox.yMin);
		FT_Outline_Render(this->m_pFontManager->GetFontLibrary(), outline, &params);

        ret = bmp.buffer;
    }
	
    FT_Done_Glyph(glyph);
       
    return ret;
}

bool CFont::GetCharDataInfo(wchar_t ch, char_info_t & info)
{
	int w = get_hide_char_width(ch);
	if(w > -1)
	{
		info.nCharWidth = w;
		info.nCharHeight = m_nTexUnitHeight;
		info.nUserXOffset = 0;
		info.nUserYOffset = 0;
		info.nXAdvance = w;
		info.nXOffset = 0;
		info.nYOffset = 0;
		return true;
	}
	CFont::char_data_t* data = CFont::GetCharInfo(ch);
	if( data == 0)
		return false;

	if (data->nCharSlot == 0xFFFF && !data->bInit)
	{
		if(!FillCharInfo(ch, *data))
		{
			if(!FillCharInfo(L'?', *data))
			{
				Assert(0);
			}
		}
		data->bInit = true;
	}

	if(!m_bVarExtent)
	{
		info.nCharWidth = data->nCharWidth;
		info.nCharHeight = data->nCharHeight;
		info.nXAdvance = data->nCharWidth;
		info.nYOffset = 0;
		info.nXOffset = 0;
		info.nUserYOffset = data->nUserYOffset;
		info.nUserXOffset = data->nUserXOffset;
	}
	else
	{
		info.nCharWidth = data->nCharWidth;
		info.nCharHeight = data->nCharHeight;
		info.nXAdvance = data->nXAdvance;
		info.nYOffset = data->nYOffset;
		info.nXOffset = data->nXOffset;
		info.nUserYOffset = data->nUserYOffset;
		info.nUserXOffset = data->nUserXOffset;
	}
	return true;
}