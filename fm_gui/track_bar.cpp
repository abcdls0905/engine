//--------------------------------------------------------------------
// 文件名:		track_bar.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "track_bar.h"

/// \file track_bar.cpp
/// \brief 数值调节器

/// entity: TrackBar
/// \brief 数值调节器实体
/// \see 继承自IControl
DECLARE_ENTITY(TrackBar, 0, IControl);

/// readonly: object TrackButton
/// \brief 调节按钮
DECLARE_PROPERTY_GET(PERSISTID, TrackBar, TrackButton, GetTrackButton);

/// property: string Orient
/// \brief 方向（"Vertical"-垂直，"Horizon"-水平）
DECLARE_PROPERTY(const char*, TrackBar, Orient, GetOrient, SetOrient);
/// property: int Minimum
/// \brief 最小值
DECLARE_PROPERTY(int, TrackBar, Minimum, GetMinimum, SetMinimum);
/// property: int Maximum
/// \brief 最大值
DECLARE_PROPERTY(int, TrackBar, Maximum, GetMaximum, SetMaximum);
/// property: int Value
/// \brief 当前值
DECLARE_PROPERTY(int, TrackBar, Value, GetCurValue, SetCurValue);

/// event: int on_value_changed(object self, int old_value)
/// \brief 当数值改变时
/// \param self 本控件
/// \param old_value 旧值
DECLARE_EVENT(on_value_changed);
/// event: int on_drag_leave(object self, object subor)
/// \brief 拖动结束
/// \param self 本控件
/// \param subor 附属控件
DECLARE_EVENT(on_drag_leave);

// TrackBar

TrackBar::TrackBar()
{
	m_pTrackBtn = NULL;
	m_nOrient = ORIENT_HORIZON;
	m_nValue = 0;
	m_nMinimum = 0;
	m_nMaximum = 100;

	SetDesignSize(80, 16);
}

TrackBar::~TrackBar()
{
}

bool TrackBar::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	m_pTrackBtn = (IControl*)CreateSuborControl(this, "Button");
	m_pTrackBtn->SetWidth(12);
	m_pTrackBtn->SetHeight(12);

	CalcSuborRect();
	
	return true;
}

bool TrackBar::Shut()
{
	SAFE_RELEASE(m_pTrackBtn);
	
	return IControl::Shut();
}

void TrackBar::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("Font");
	di->RemovePropInfo("Text");
	
	di->AddPropInfo("Orient", IDesignInfo::TYPE_STRING, true, true);
	di->AddPropInfo("Minimum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Maximum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Value", IDesignInfo::TYPE_INT, true, true);

	di->AddSuborInfo("TrackButton", "Button");

	di->AddEventInfo("on_value_changed");
	di->AddEventInfo("on_drag_leave");
}

bool TrackBar::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pTrackBtn && !m_pTrackBtn->GetLoadFinish())
	{
		return false;
	}
	
	return IControl::GetLoadFinish();
}

void TrackBar::Paint(float seconds)
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
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}

	m_pTrackBtn->Paint(seconds);
}

int TrackBar::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int TrackBar::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

void TrackBar::SwapWidthHeight()
{
	m_pTrackBtn->SetWidth(12);
	m_pTrackBtn->SetHeight(12);

	int w = GetWidth();
	int h = GetHeight();
	
	SetWidth(h);
	SetHeight(w);
}

void TrackBar::CalcSuborRect()
{
	int diff = m_nMaximum - m_nMinimum;

	Assert(diff >= 0);
	
	if (diff == 0)
	{
		m_pTrackBtn->SetVisible(false);
	}
	else
	{
		m_pTrackBtn->SetVisible(true);
	}
	
	if (m_nOrient == ORIENT_VERTICAL)
	{
		int h = GetHeight() - m_pTrackBtn->GetHeight();

		if (h < 0)
		{
			h = 0;
		}

		int t = 0;

		if (diff > 0)
		{
			t += (GetCurValue() - m_nMinimum) * h / diff;
		}

		m_pTrackBtn->SetLeft(0);
		m_pTrackBtn->SetTop(t);
		m_pTrackBtn->SetWidth(GetWidth());
	}
	else
	{
		int w = GetWidth() - m_pTrackBtn->GetWidth();

		if (w < 0)
		{
			w = 0;
		}

		int t = 0;

		if (diff > 0)
		{
			t += (GetCurValue() - m_nMinimum) * w / diff;
		}

		m_pTrackBtn->SetLeft(t);
		m_pTrackBtn->SetTop(0);
		m_pTrackBtn->SetHeight(GetHeight());
	}
}

void TrackBar::IncValue(int value)
{
	int old_value = GetCurValue();

	m_nValue = NormalizeValue(old_value + value);
	
	CalcSuborRect();

	if (m_nValue != old_value)
	{
		if (GetDelegateControl() != NULL)
		{
			GetDelegateControl()->OnSuborEvent(this, 
				CGuiConst::EVENT_VALUE_CHANGED, CVarList() << old_value);
		}
		else
		{
			if (GetEnabled())
			{
				GuiUtil_RunCallback(this, "on_value_changed", 
					CVarList() << old_value);
			}
		}
	}
}

void TrackBar::SetOrient(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Horizon") == 0)
	{
		if (m_nOrient == ORIENT_VERTICAL)
		{
			m_nOrient = ORIENT_HORIZON;
			
			SwapWidthHeight();
		}
	}
	else
	{
		if (m_nOrient == ORIENT_HORIZON)
		{
			m_nOrient = ORIENT_VERTICAL;
			
			SwapWidthHeight();
		}
	}
}

const char* TrackBar::GetOrient() const
{
	if (m_nOrient == ORIENT_VERTICAL)
	{
		return "Vertical";
	}
	else
	{
		return "Horizon";
	}
}

void TrackBar::SetMinimum(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nMinimum = value;
	
	if (value >= m_nMaximum)
	{
		SetMaximum(value);
	}
}

int TrackBar::GetMinimum() const
{
	return m_nMinimum;
}

void TrackBar::SetMaximum(int value)
{
	if (value < m_nMinimum)
	{
		return;
	}
	
	m_nMaximum = value;

	IncValue(0);
}

int TrackBar::GetMaximum() const
{
	return m_nMaximum;
}

void TrackBar::SetCurValue(int value)
{
	IncValue(NormalizeValue(value) - GetCurValue());
}

int TrackBar::GetCurValue() const
{
	return NormalizeValue(m_nValue);
}

PERSISTID TrackBar::GetTrackButton() const
{
	if (NULL == m_pTrackBtn)
	{
		return PERSISTID();
	}
	
	return m_pTrackBtn->GetID();
}

int TrackBar::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int diff = m_nMaximum - m_nMinimum;

	if (diff <= 0)
	{
		return 0;
	}

	if (m_nOrient == ORIENT_VERTICAL)
	{
		int h = GetHeight() - m_pTrackBtn->GetHeight();

		int new_y = y - InnerGetAbsTop();

		int new_value = new_y * diff / h + m_nMinimum;
		
		int inc = new_value - m_nValue;
		
		if (inc != 0)
		{
			IncValue(new_value - m_nValue);
		}
	}
	else
	{
		int w = GetWidth() - m_pTrackBtn->GetWidth();

		int new_x = x - InnerGetAbsLeft();
		
		int new_value = new_x * diff / w + m_nMinimum;
		
		int inc = new_value - m_nValue;
		
		if (inc != 0)
		{
			IncValue(new_value - m_nValue);
		}
	}

	return 1;
}

int TrackBar::OnSuborEvent(IControl* subor, int event, 
						   const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (event == CGuiConst::EVENT_DRAG_MOVE)
	{
		if (subor != m_pTrackBtn)
		{
			return 0;
		}
		
		if (m_nOrient == ORIENT_VERTICAL)
		{
			int click_y = args.IntVal(1);
			
			int y = args.IntVal(3);
			
			int new_y = y - m_pTrackBtn->GetAbsTop();
			
			int delta_y = new_y - click_y;
			
			int new_top = m_pTrackBtn->GetTop() + delta_y;
			
			int diff = m_nMaximum - m_nMinimum;
			
			if (diff > 0)
			{
				int h = GetHeight() - m_pTrackBtn->GetHeight();
				
				int new_value = new_top	* diff / h + m_nMinimum;
				
				int inc = new_value - m_nValue;
				
				if (inc != 0)
				{
					IncValue(new_value - m_nValue);
				}
			}
		}
		else
		{
			int click_x = args.IntVal(0);
			
			int x = args.IntVal(2);
			
			int new_x = x - m_pTrackBtn->GetAbsLeft();
			
			int delta_x = new_x - click_x;
			
			int new_left = m_pTrackBtn->GetLeft() + delta_x;
			
			int diff = m_nMaximum - m_nMinimum;
			
			if (diff > 0)
			{
				int w = GetWidth() - m_pTrackBtn->GetWidth();
				
				if (w != 0)
				{
					int new_value = new_left * diff / w + m_nMinimum;

					int inc = new_value - m_nValue;

					if (inc != 0)
					{
						IncValue(new_value - m_nValue);
					}
				}
			}
		}
		
		return 1;
	}
	
	if (event == CGuiConst::EVENT_DRAG_LEAVE)
	{
		if (subor != m_pTrackBtn)
		{
			return 0;
		}

		GuiUtil_RunCallback(this, "on_drag_leave", 
			CVarList() << subor->GetID());

		return 1;
	}
		
	return 0;
}

bool TrackBar::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        if (m_pTrackBtn != NULL) m_pTrackBtn->SetRefreshInnerAbsPos(true);
        IControl::RefreshInnerAbsPos();
    }

    return true;
}