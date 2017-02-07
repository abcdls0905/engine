//--------------------------------------------------------------------
// 文件名:		caret.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "caret.h"
#include "i_control.h"
#include "gui_utils.h"
#include "gui_const.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../visual/i_painter.h"

/// \file caret.cpp
/// \brief 输入提示光标

/// entity: Caret
/// \brief 输入提示光标实体
DECLARE_ENTITY(Caret, 0, IEntity);

/// property: string Name
/// \brief 名字
DECLARE_PROPERTY(const char*, Caret, Name, GetName, SetName);
/// property: float ShowInterval
/// \brief 显示状态的时间间隔（单位为秒）
DECLARE_PROPERTY(float, Caret, ShowInterval, 
				 GetShowInterval, SetShowInterval);
/// property: float HideInterval
/// \brief 隐藏状态的时间间隔（单位为秒）
DECLARE_PROPERTY(float, Caret, HideInterval, 
				 GetHideInterval, SetHideInterval);
/// property: float WidthScale
/// \brief 显示宽度和当前字体的比例
DECLARE_PROPERTY(float, Caret, WidthScale, GetWidthScale, SetWidthScale);
/// property: float HeightScale
/// \brief 显示高度和当前字体的比例
DECLARE_PROPERTY(float, Caret, HeightScale, GetHeightScale, SetHeightScale);
/// property: int FixWidth
/// \brief 固定的宽度
DECLARE_PROPERTY(int, Caret, FixWidth, GetFixWidth, SetFixWidth);
/// property: int FixHeight
/// \brief 固定的高度
DECLARE_PROPERTY(int, Caret, FixHeight, GetFixHeight, SetFixHeight);
/// property: string color
/// \brief 颜色值
DECLARE_PROPERTY(result_string, Caret, Color, GetColorString, SetColorString);

// Caret

Caret::Caret()
{
	m_pPainter = NULL;
	m_nHash = 0;
	m_bDisplay = false;
	m_fShowInterval = 0.6F;
	m_fHideInterval = 0.6F;
	m_fFlashCount = 0.0F;
	m_fWidthScale = 1.0F;
	m_fHeightScale = 1.0F;
	m_nFixWidth = 0;
	m_nFixHeight = 0;
	m_nColor = 0;
}

Caret::~Caret()
{
}

bool Caret::Init(const IVarList& args)
{
	const char* painter_name = args.StringVal(0);
	
	m_pPainter = (IPainter*)GetCore()->GetInterface(painter_name);
	
	if (NULL == m_pPainter)
	{
		CORE_TRACE("(Caret::Init)no painter");
		CORE_TRACE(painter_name);
		return false;
	}
	
	return true;
}

bool Caret::Shut()
{
	SAFE_RELEASE(m_pPainter);

	return true;
}

void Caret::SetName(const char* name)
{
	Assert(name != NULL);

	m_strName = name;
	m_nHash = GetHashValueCase(name);
}

const char* Caret::GetName() const
{
	return m_strName.c_str();
}

unsigned int Caret::GetHash() const
{
	return m_nHash;
}

void Caret::SetShowInterval(float value)
{
	if ((value < 0.1F) || (value > 10.0F))
	{
		return;
	}
	
	m_fShowInterval = value;
}

float Caret::GetShowInterval() const
{
	return m_fShowInterval;
}

void Caret::SetHideInterval(float value)
{
	if (value > 10.0F)
	{
		return;
	}
	
	m_fHideInterval = value;
}

float Caret::GetHideInterval() const
{
	return m_fHideInterval;
}

void Caret::SetWidthScale(float value)
{
	if (value < 0.0F)
	{
		return;
	}
	
	m_fWidthScale = value;
}

float Caret::GetWidthScale() const
{
	return m_fWidthScale;
}

void Caret::SetHeightScale(float value)
{
	if (value < 0.0F)
	{
		return;
	}
	
	m_fHeightScale = value;
}

float Caret::GetHeightScale() const
{
	return m_fHeightScale;
}

void Caret::SetFixWidth(int value)
{
	if ((value < 0) || (value >= MAX_CARET_WIDTH))
	{
		return;
	}
	
	m_nFixWidth = value;
}

int Caret::GetFixWidth() const
{
	return m_nFixWidth;
}

void Caret::SetFixHeight(int value)
{
	if ((value < 0) || (value >= MAX_CARET_HEIGHT))
	{
		return;
	}
	
	m_nFixHeight = value;
}

int Caret::GetFixHeight() const
{
	return m_nFixHeight;
}

void Caret::SetColorString(const char* val)
{
	Assert(val != NULL);
	
	m_nColor = GuiUtil_ColorToInt(val);
}

result_string Caret::GetColorString() const
{
	return GuiUtil_IntToColor(m_nColor).c_str();
}

int Caret::GetWidth(int width)
{
	if (m_nFixWidth > 0)
	{
		return m_nFixWidth;
	}
	else
	{
		return int(float(width) * m_fWidthScale);
	}
}

int Caret::GetHeight(int height)
{
	if (m_nFixHeight > 0)
	{
		return m_nFixHeight;
	}
	else
	{
		return int(float(height) * m_fHeightScale);
	}
}

unsigned int Caret::GetActualColor(unsigned int color)
{
	if (m_nColor > 0)
	{
		return m_nColor;
	}
	else
	{
		return color;
	}
}

bool Caret::Create()
{
	return true;
}

void Caret::Reset()
{
	m_bDisplay = true;
	m_fFlashCount = 0.0F;
}

void Caret::Flash(float time)
{
	if (m_fHideInterval <= 0.0F)
	{
		m_bDisplay = true;
		return;
	}
	
	m_fFlashCount += time;
	
	for (;;)
	{
		if (m_bDisplay)
		{
			if (m_fFlashCount >= m_fShowInterval)
			{
				m_fFlashCount -= m_fShowInterval;
				m_bDisplay = false;
				
				if (m_fFlashCount >= m_fHideInterval)
				{
					continue;
				}
			}
		}
		else
		{
			if (m_fFlashCount >= m_fHideInterval)
			{
				m_fFlashCount -= m_fHideInterval;
				m_bDisplay = true;
				
				if (m_fFlashCount >= m_fShowInterval)
				{
					continue;
				}
			}
		}
		
		break;
	}
}

bool Caret::Draw(int left, int bottom, int width, int height, unsigned int color)
{
	if (!m_bDisplay)
	{
		return false;
	}
	
	unsigned int caret_color = GetActualColor(color);
	int caret_w = GetWidth(width);
	int caret_h = GetHeight(height);
	int caret_left = left;
	int caret_top = bottom - caret_h;
	int caret_right = caret_left + caret_w;
	int caret_bottom = caret_top + caret_h;
	
	m_pPainter->FillRect(caret_left, caret_top, 
		caret_right, caret_bottom, caret_color);
	
	return true;
}

