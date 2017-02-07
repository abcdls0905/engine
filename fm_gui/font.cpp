//--------------------------------------------------------------------
// 文件名:		font.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "font.h"
#include "gui_utils.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../visual/i_painter.h"

/// \file font.cpp
/// \brief 字体

/// entity: Font
/// \brief 字体实体
DECLARE_ENTITY(Font, 0, IEntity);

/// property: string Name
/// \brief 名字
DECLARE_PROPERTY(const char*, Font, Name, GetName, SetName);
/// property: string SysFont
/// \brief 使用的操作系统字体名
//DECLARE_PROPERTY(const char*, Font, SysFont, GetSysFont, SetSysFont);
/// property: string FontFile
/// \brief 字体文件名
DECLARE_PROPERTY(const char*, Font, FontFile, GetFontFile, SetFontFile);
/// property: string FontTex
/// \brief 字体贴图名
DECLARE_PROPERTY(const char*, Font, FontTex, GetFontTex, SetFontTex);
/// property: int TexWidth
/// \brief 使用贴图缓冲的宽度
DECLARE_PROPERTY(int, Font, TexWidth, GetTexWidth, SetTexWidth);
/// property: int TexHeight
/// \brief 使用贴图缓冲的宽度
DECLARE_PROPERTY(int, Font, TexHeight, GetTexHeight, SetTexHeight);
/// property: int Height
/// \brief 字体高度
DECLARE_PROPERTY(int, Font, Height, GetHeight, SetHeight);
/// property: int Interval
/// \brief 文字间距
DECLARE_PROPERTY(int, Font, Interval, GetInterval, SetInterval);
/// property: bool Bold
/// \brief 是否粗体
DECLARE_PROPERTY(bool, Font, Bold, GetBold, SetBold);
/// property: bool Smooth
/// \brief 是否平滑
DECLARE_PROPERTY(bool, Font, Smooth, GetSmooth, SetSmooth);
/// property: bool Italic
/// \brief 是否斜体
DECLARE_PROPERTY(bool, Font, Italic, GetItalic, SetItalic);
/// property: bool Underline
/// \brief 是否有下划线
DECLARE_PROPERTY(bool, Font, Underline, GetUnderline, SetUnderline);
/// property: bool StrikeOut
/// \brief 是否有删除线
DECLARE_PROPERTY(bool, Font, StrikeOut, GetStrikeOut, SetStrikeOut);
/// property: bool VarExtent
/// \brief 是否可变尺寸
DECLARE_PROPERTY(bool, Font, VarExtent, GetVarExtent, SetVarExtent);
/// property: string ShadowColor
/// \brief 阴影颜色（为0表示没有）
DECLARE_PROPERTY(result_string, Font, ShadowColor, GetShadowColorString, 
	SetShadowColorString);
/// property: int ShadowOffsetX
/// \brief 阴影的X偏移
DECLARE_PROPERTY(int, Font, ShadowOffsetX, GetShadowOffsetX, SetShadowOffsetX);
/// property: int ShadowOffsetY
/// \brief 阴影的Y偏移
DECLARE_PROPERTY(int, Font, ShadowOffsetY, GetShadowOffsetY, SetShadowOffsetY);

// Font

Font::Font()
{
	m_pPainter = NULL;
	m_nHash = 0;
	m_nTexWidth = 512;
	m_nTexHeight = 512;
	m_nHeight = 12;
	m_nInterval = 0;
	m_bBold = false;
	m_bSmooth = false;
	m_bItalic = false;
	m_bUnderline = false;
	m_bStrikeOut = false;
	m_bVarExtent = true;
    m_bOutline = false;
	m_nShadowColor = 0;
	m_nShadowOffsetX = 0;
	m_nShadowOffsetY = 0;
	m_nOutlineNum = 0;
	m_nFadeType = 0;
	m_OutlineColor = FmVec3(0, 0, 0);
}

Font::~Font()
{
}

bool Font::Init(const IVarList& args)
{
	const char* painter_name = args.StringVal(0);
	
	m_pPainter = (IPainter*)GetCore()->GetInterface(painter_name);
	
	if (NULL == m_pPainter)
	{
		CORE_TRACE("(Font::Init)no painter");
		CORE_TRACE(painter_name);
		return false;
	}
	
	return true;
}

bool Font::Shut()
{
	SAFE_RELEASE(m_pPainter);
	
	return true;
}

void Font::SetName(const char* value)
{
	Assert(value != NULL);
	
	m_strName = value;
	m_nHash = GetHashValueCase(value);
}

const char* Font::GetName() const
{
	return m_strName.c_str();
}

unsigned int Font::GetHash() const
{
	return m_nHash;
}

//void Font::SetSysFont(const char* value)
//{
//	Assert(value != NULL);
//	
//	m_strSysFont = value;
//}

//const char* Font::GetSysFont() const
//{
//	return m_strSysFont.c_str();
//}

void Font::SetFontFile(const char* value)
{
	Assert(value != NULL);
	
	m_strFontFile = value;
}

const char* Font::GetFontFile() const
{
	return m_strFontFile.c_str();
}

void Font::SetFontTex(const char* value)
{
	Assert(value != NULL);

	m_strFontTex = value;
}

const char* Font::GetFontTex() const
{
	return m_strFontTex.c_str();
}

void Font::SetTexWidth(int value)
{
	if ((value != 64) && (value != 128) && (value != 256) 
		&& (value != 512) && (value != 1024) && (value != 2048))
	{
		CORE_TRACE("(Font::SetTexWidth)size error");
		return;
	}
	
	m_nTexWidth = value;
}

int Font::GetTexWidth() const
{
	return m_nTexWidth;
}

void Font::SetTexHeight(int value)
{
	if ((value != 64) && (value != 128) && (value != 256) 
		&& (value != 512) && (value != 1024) && (value != 2048))
	{
		CORE_TRACE("(Font::SetTexWidth)size error");
		return;
	}
	
	m_nTexHeight = value;
}

int Font::GetTexHeight() const
{
	return m_nTexHeight;
}

void Font::SetHeight(int value)
{
	if (value < 5)
	{
		CORE_TRACE("(Font::SetHeight)must above equal 5");
		return;
	}
	
	m_nHeight = value;
}

int Font::GetHeight() const
{
	return m_nHeight;
}

void Font::SetInterval(int value)
{
	if ((value < 0) || (value > 256))
	{
		CORE_TRACE("(Font::SetInterval)must 0 to 256");
		return;
	}
	
	m_nInterval = value;
}

int Font::GetInterval() const
{
	return m_nInterval;
}

void Font::SetBold(bool value)
{
	m_bBold = value;
}

bool Font::GetBold() const
{
	return m_bBold;
}

void Font::SetSmooth(bool value)
{
	m_bSmooth = value;
}

bool Font::GetSmooth() const
{
	return m_bSmooth;
}

void Font::SetItalic(bool value)
{
	m_bItalic = value;
}

bool Font::GetItalic() const
{
	return m_bItalic;
}

void Font::SetUnderline(bool value)
{
	m_bUnderline = value;
}

bool Font::GetUnderline() const
{
	return m_bUnderline;
}

void Font::SetStrikeOut(bool value)
{
	m_bStrikeOut = value;
}

bool Font::GetStrikeOut() const
{
	return m_bStrikeOut;
}

void Font::SetVarExtent(bool value)
{
	m_bVarExtent = value;
}

bool Font::GetVarExtent() const
{
	return m_bVarExtent;
}

void Font::SetOutline(bool value)
{
    m_bOutline = value;
}

void Font::SetOutlineColor(const char* value)
{
	int outline_color = GuiUtil_ColorToInt(value);
	int a = outline_color >> 24;
	int r = (outline_color >> 16) & 0xff;
	int g = (outline_color >> 8) & 0xff;
	int b = (outline_color) & 0xff;

	m_OutlineColor = FmVec3(r, g, b);
}

void Font::SetOutlineNum(int value)
{
	this->m_nOutlineNum = value;
}

bool Font::GetOutline() const
{
    return m_bOutline;
}

void Font::SetShadowColorString(const char* value)
{
	m_nShadowColor = GuiUtil_ColorToInt(value);
}

result_string Font::GetShadowColorString() const
{
	return GuiUtil_IntToColor(m_nShadowColor).c_str();
}

void Font::SetShadowOffsetX(int value)
{
	if ((value < -32) || (value > 32))
	{
		CORE_TRACE("(Font::SetShadowOffsetX)value error");
		return;
	}
	
	m_nShadowOffsetX = value;
}

int Font::GetShadowOffsetX() const
{
	return m_nShadowOffsetX;
}

void Font::SetShadowOffsetY(int value)
{
	if ((value < -32) || (value > 32))
	{
		CORE_TRACE("(Font::SetShadowOffsetY)value error");
		return;
	}
	
	m_nShadowOffsetY = value;
}

void Font::SetFadeType(int type)
{
	m_nFadeType = type;
}

int Font::GetShadowOffsetY() const
{
	return m_nShadowOffsetY;
}

bool Font::AddTexCharInfo(int ch, int left, int top, int right, int bottom)
{
	m_CharInfos.push_back(ch);
	m_CharInfos.push_back(left);
	m_CharInfos.push_back(top);
	m_CharInfos.push_back(right);
	m_CharInfos.push_back(bottom);

	return true;
}

bool Font::CheckFullPath(const char* path)
{
	if(path == 0)
	{
		return false;
	}

#ifdef _WIN32
	size_t dwLen = strlen(path);
	if ( dwLen > 2 && path[1] == ':' && dwLen > 2 )
	{
		return true;
	}
#else
	if(path[0] == '/')
		return true;
#endif
	return false;
}

bool Font::Create()
{
	char font_file[256] = "";

	if(!CheckFullPath(m_strFontFile.c_str()))
	{
		::SafeSprintf(font_file, sizeof(font_file), "%s%s", 
			m_pPainter->GetDefaultPath(), m_strFontFile.c_str());
		m_strFontFile.clear();
		m_strFontFile.append(font_file);
	}
	else
	{
		::SafeSprintf(font_file, sizeof(font_file), "%s", 
			m_strFontFile.c_str());
	}

	if (!m_strFontFile.empty())
	{
		return m_pPainter->AddSysFont(m_strName.c_str(), 
			font_file, m_nTexWidth, m_nHeight, m_nInterval, 
			m_bBold, m_bSmooth, m_bItalic, m_bUnderline, m_bStrikeOut, 
			m_bVarExtent, m_bOutline, m_nOutlineNum, &m_OutlineColor, 
			m_nFadeType, m_nShadowColor, m_nShadowOffsetX, 
			m_nShadowOffsetY);
	}

	if (!m_strFontTex.empty())
	{
		int* char_info = &m_CharInfos[0];
		size_t char_num = m_CharInfos.size() / 5;
		
		return m_pPainter->AddTexFont(m_strName.c_str(), m_strFontTex.c_str(), 
			m_nTexWidth, m_nTexHeight, m_nHeight, m_nInterval, char_info, 
			(int)char_num);
	}
	
	return false;
}

