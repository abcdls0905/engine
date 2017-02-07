//--------------------------------------------------------------------
// 文件名:		progress_bar.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "progress_bar.h"
#include "../public/inlines.h"

/// \file progress_bar.cpp
/// \brief 进度条

/// entity: ProgressBar
/// \brief 进度条实体
/// \see 继承自IControl
DECLARE_ENTITY(ProgressBar, 0, IControl);

/// property: int Minimum
/// \brief 最小值
DECLARE_PROPERTY(int, ProgressBar, Minimum, GetMinimum, SetMinimum);
/// property: int Maximum
/// \brief 最大值
DECLARE_PROPERTY(int, ProgressBar, Maximum, GetMaximum, SetMaximum);
/// property: int Value
/// \brief 当前值
DECLARE_PROPERTY(int, ProgressBar, Value, GetCurValue, SetCurValue);
/// property: int WidthGap
/// \brief 显示条的宽度
DECLARE_PROPERTY(int, ProgressBar, WidthGap, GetWidthGap, SetWidthGap);
/// property: int HeightGap
/// \brief 显示条的高度
DECLARE_PROPERTY(int, ProgressBar, HeightGap, GetHeightGap, SetHeightGap);

/// property: string ProgressImage
/// \brief 显示图元名
DECLARE_PROPERTY(const char*, ProgressBar, ProgressImage, GetProgressImage, 
	SetProgressImage);
/// property: string ProgressMode
/// \brief 进度显示模式
DECLARE_PROPERTY(const char*, ProgressBar, ProgressMode, GetProgressMode, 
	SetProgressMode);

/// property: bool TextVisible
/// \brief 显示进度文本
DECLARE_PROPERTY(bool, ProgressBar, TextVisible, GetTextVisible, 
	SetTextVisible);
/// property: bool TextPercent
/// \brief 进度文本显示方式,true为百分比显示，false为实际数字显示
DECLARE_PROPERTY(bool, ProgressBar, TextPercent, GetTextPercent, 
	SetTextPercent);
/// property: string TextColor
/// \brief 进度文本颜色
DECLARE_PROPERTY(result_string, ProgressBar, TextColor, GetTextColorString, 
	SetTextColorString);

/// property: int BlendAlpha
/// \brief 半透明混合
DECLARE_PROPERTY(int, ProgressBar, BlendAlpha, GetAlpha, SetAlpha);

// ProgressBar

ProgressBar::ProgressBar()
{
	m_nMinimum = 0;
	m_nMaximum = 100;
	m_nValue = m_nMinimum;
	m_nHeightGap = 2;
	m_nWidthGap = 2;
    m_bTextVisible = false;
    m_bTextPercent = false;
	m_TextColor.SetValue(CGuiConst::WHITE_COLOR);
    m_eProgressMode = LEFT_TO_RIGHT;

	SetDesignSize(80, 16);
}

ProgressBar::~ProgressBar()
{
}

bool ProgressBar::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	return true;
}

bool ProgressBar::Shut()
{
	m_ProgressImage.Release();

	return IControl::Shut();
}

void ProgressBar::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	di->RemovePropInfo("Text");
    
    
	
	di->AddPropInfo("Minimum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Maximum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Value", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("ProgressMode", IDesignInfo::TYPE_STRING, true, true);
	di->AddPropInfo("WidthGap", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("HeightGap", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("ProgressImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("TextVisible", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("TextPercent", IDesignInfo::TYPE_BOOL, true, true);
    CVar v(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::WHITE_COLOR).c_str());
	di->AddPropInfo("TextColor", IDesignInfo::TYPE_COLOR, true, true, 
		&v);
}

bool ProgressBar::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_ProgressImage.IsLoadCompelte())
	{
		return false;
	}
	
	return IControl::GetLoadFinish();
}

void ProgressBar::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
    if (!GetLoadFinish())
    {
        return;
    }

    RefreshInnerAbsPos();

	int old_alpha = GetPainter()->GetBlendAlpha();

    // 设置当前透明度
    GetPainter()->SetBlendAlpha(old_alpha * GetAlpha() / 255);

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

    if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}
	
	if ((m_nMaximum > m_nMinimum) && (m_nValue > m_nMinimum))
	{
		float delta = (float)(m_nValue - m_nMinimum) 
			/ (float)(m_nMaximum - m_nMinimum);

		if (m_ProgressImage.IsReady())
		{
			int w = m_ProgressImage.GetWidth();
			int h = m_ProgressImage.GetHeight();
			
            if (m_eProgressMode == LEFT_TO_RIGHT)
            {
			    int x3 = x1 + int(float(x2 - x1) * delta);

                rect_t rdest = { x1, y1, x3, y2 };

                int s_w =  (int)(w * delta);

                rect_t rsrc = { 0, 0, s_w, h };

                m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
            }
            else if (m_eProgressMode == RIGHT_TO_LEFT)
            {
                int x3 = x2 - int(float(x2 - x1) * delta);

                rect_t rdest = { x3, y1, x2, y2 };
                rect_t rsrc = { (int)((1-delta) * w), 0, w, h };

                m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
            }
            else if (m_eProgressMode == BOTTOM_TO_TOP)
            {
                int y3 = y2 - int(float(y2 - y1) * delta);

                rect_t rdest = { x1, y3, x2, y2 };
                rect_t rsrc = { 0, (int)((1 - delta) * h), w, h };

                m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
            }
            else if (m_eProgressMode == TOP_TO_BOTTOM)
            {
                int y3 = y1 + int(float(y2 - y1) * delta);
                
				rect_t rdest = { x1, y1, x2, y3 };

                int s_h = (int)(delta * h);

                rect_t rsrc = { 0, 0, w, s_h };

                m_ProgressImage.DrawParts(GetDrawMode(), rdest, rsrc);
            }
		}
		else
		{
			x1 += m_nWidthGap;
			x2 -= m_nWidthGap;
			y1 += m_nHeightGap;
			y2 -= m_nHeightGap;

			//x2 = x1 + int(float(x2 - x1) * delta);

			if (m_eProgressMode == LEFT_TO_RIGHT)
			{
				x2 = x1 + int(float(x2 - x1) * delta);
			}
			else if (m_eProgressMode == RIGHT_TO_LEFT)
			{
				x1 = x2 - int(float(x2 - x1) * delta);
			}
			else if (m_eProgressMode == BOTTOM_TO_TOP)
			{
				y1 = y2 - int(float(y2 - y1) * delta);
			}
			else if (m_eProgressMode == TOP_TO_BOTTOM)
			{
				y2 = y1 + int(float(y2 - y1) * delta);
			}
			
			GetPainter()->FillRect(x1, y1, x2, y2, GetForeColor());
		}
	}
    
    if (m_bTextVisible)
    {
        GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

        wchar_t wcsBuffer[256] = L"";

        if (m_bTextPercent)
        {
            SafeSwprintf(wcsBuffer, sizeof(wcsBuffer), L"%d%%", 
				m_nValue * 100 / m_nMaximum);
        }
        else
        {
            SafeSwprintf(wcsBuffer, sizeof(wcsBuffer), L"%d/%d", m_nValue, 
				m_nMaximum);
        }

        //GetPainter()->WriteTextCenter(x1, y1, x2, y2, 0xffffffff, wcsBuffer);
		GetPainter()->WriteTextCenter(x1, y1, x2, y2, GetTextColor(), 
			wcsBuffer);
    }

	// 恢复
	GetPainter()->SetBlendAlpha(old_alpha);
}

void ProgressBar::SetMinimum(int value)
{
	if (value < 0)
	{
		return;
	}
	
	if (value >= m_nMaximum)
	{
		return;
	}
	
	m_nMinimum = value;
	
	if (m_nValue < m_nMinimum)
	{
		m_nValue = value;
	}
}

int ProgressBar::GetMinimum() const
{
	return m_nMinimum;
}

void ProgressBar::SetMaximum(int value)
{
	if (value <= 0)
	{
		return;
	}
	
	if (value <= m_nMinimum)
	{
		return;
	}
	
	m_nMaximum = value;
	
	if (m_nValue > m_nMaximum)
	{
		m_nValue = m_nMaximum;
	}
}

int ProgressBar::GetMaximum() const
{
	return m_nMaximum;
}

void ProgressBar::SetCurValue(int value)
{
	if (value < m_nMinimum)
	{
		value = m_nMinimum;
	}
	
	if (value > m_nMaximum)
	{
		value = m_nMaximum;
	}
	
	m_nValue = value;
}

int ProgressBar::GetCurValue() const
{
	return m_nValue;
}

void ProgressBar::SetTextVisible(bool value)
{
    m_bTextVisible = value;
}

bool ProgressBar::GetTextVisible() const
{
    return m_bTextVisible;
}

void ProgressBar::SetTextPercent(bool value)
{
    if (m_bTextPercent != value)
    {
        m_bTextPercent = value;
    }
}

bool ProgressBar::GetTextPercent() const
{
    return m_bTextPercent;
}

void ProgressBar::SetTextColor(unsigned int val)
{
	m_TextColor.SetValue(val);
}

unsigned int ProgressBar::GetTextColor() const
{
	return m_TextColor.GetValue();
}

void ProgressBar::SetTextColorString(const char* val)
{
	Assert(val != NULL);

	m_TextColor.SetName(GetGui(), val);
}

result_string ProgressBar::GetTextColorString() const
{
	return m_TextColor.GetName().c_str();
}

void ProgressBar::SetWidthGap(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nWidthGap = value;
}

int ProgressBar::GetWidthGap() const
{
	return m_nWidthGap;
}

void ProgressBar::SetHeightGap(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nHeightGap = value;
}

int ProgressBar::GetHeightGap() const
{
	return m_nHeightGap;
}

// 进度显示模式
void ProgressBar::SetProgressMode(const char* mode)
{
    if (stricmp(mode, "LeftToRight") == 0)
    {
        m_eProgressMode = LEFT_TO_RIGHT;
    }
    else if (stricmp(mode, "RightToLeft") == 0)
    {
        m_eProgressMode = RIGHT_TO_LEFT;
    }
    else if (stricmp(mode, "BottomToTop") == 0)
    {
        m_eProgressMode = BOTTOM_TO_TOP;
    }
    else if (stricmp(mode, "TopToBottom") == 0)
    {
        m_eProgressMode = TOP_TO_BOTTOM;
    }
    else
    {
        m_eProgressMode = LEFT_TO_RIGHT;
    }
}

const char* ProgressBar::GetProgressMode() const
{
    if (m_eProgressMode == LEFT_TO_RIGHT)
    {
        return "LeftToRight";
    }
    else if (m_eProgressMode == RIGHT_TO_LEFT)
    {
        return "RightToLeft";
    }
    else if (m_eProgressMode == BOTTOM_TO_TOP)
    {
        return "BottomToTop";
    }
    else if (m_eProgressMode == TOP_TO_BOTTOM)
    {
        return "TopToBottom";
    }

    return "LeftToRight";
}

void ProgressBar::SetProgressImage(const char* value)
{
	m_ProgressImage.SetName(value);
	m_ProgressImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ProgressBar::GetProgressImage() const
{
	return m_ProgressImage.GetName();
}
