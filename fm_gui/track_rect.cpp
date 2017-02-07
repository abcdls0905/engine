//--------------------------------------------------------------------
// 文件名:		track_rect.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "track_rect.h"

/// \file track_rect.cpp
/// \brief 数值调节框

/// entity: TrackRect
/// \brief 数值调节框实体
/// \see 继承自IControl
DECLARE_ENTITY(TrackRect, 0, IControl);

/// readonly: object TrackButton
/// \brief 调节按钮
DECLARE_PROPERTY_GET(PERSISTID, TrackRect, TrackButton, GetTrackButton);

/// property: int HorizonMin
/// \brief 水平方向最小值
DECLARE_PROPERTY(int, TrackRect, HorizonMin, GetHorizonMin, SetHorizonMin);
/// property: int HorizonMax
/// \brief 水平方向最大值
DECLARE_PROPERTY(int, TrackRect, HorizonMax, GetHorizonMax, SetHorizonMax);
/// property: int HorizonValue
/// \brief 水平方向当前值
DECLARE_PROPERTY(int, TrackRect, HorizonValue, GetHorizonValue, 
	SetHorizonValue);

/// property: int VerticalMin
/// \brief 垂直方向最小值
DECLARE_PROPERTY(int, TrackRect, VerticalMin, GetVerticalMin, SetVerticalMin);
/// property: int VerticalMax
/// \brief 垂直方向最大值
DECLARE_PROPERTY(int, TrackRect, VerticalMax, GetVerticalMax, SetVerticalMax);
/// property: int VerticalValue
/// \brief 垂直方向当前值
DECLARE_PROPERTY(int, TrackRect, VerticalValue, GetVerticalValue, 
	SetVerticalValue);

/// event: int on_horizon_changed(object self, int old_value)
/// \brief 当水平数值改变时
/// \param self 本控件
/// \param old_value 旧值
DECLARE_EVENT(on_horizon_changed);
/// event: int on_vertical_changed(object self, int old_value)
/// \brief 当垂直数值改变时
/// \param self 本控件
/// \param old_value 旧值
DECLARE_EVENT(on_vertical_changed);
/// event: int on_drag_leave(object self, object subor)
/// \brief 拖动结束
/// \param self 本控件
/// \param subor 附属控件
DECLARE_EVENT(on_drag_leave);

// TrackRect

TrackRect::TrackRect()
{
	m_pTrackBtn = NULL;
	
	m_nHorizonMin = 0;
	m_nHorizonMax = 100;
	m_nHorizonValue = 0;
	
	m_nVerticalMin = 0;
	m_nVerticalMax = 100;
	m_nVerticalValue = 0;
	
	SetDesignSize(80, 80);
}

TrackRect::~TrackRect()
{
}

bool TrackRect::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	m_pTrackBtn = (IControl*)CreateSuborControl(this, "Button");
	m_pTrackBtn->SetWidth(8);
	m_pTrackBtn->SetHeight(8);
	
	CalcSuborRect();
	
	return true;
}

bool TrackRect::Shut()
{
	SAFE_RELEASE(m_pTrackBtn);
	
	return IControl::Shut();
}

void TrackRect::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("Font");
	di->RemovePropInfo("Text");
	
	di->AddPropInfo("HorizonMin", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("HorizonMax", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("HorizonValue", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("VerticalMin", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("VerticalMax", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("VerticalValue", IDesignInfo::TYPE_INT, true, true);

	di->AddSuborInfo("TrackButton", "Button");

	di->AddEventInfo("on_horizon_changed");
	di->AddEventInfo("on_vertical_changed");
	di->AddEventInfo("on_drag_leave");
}

bool TrackRect::GetLoadFinish()
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

void TrackRect::Paint(float seconds)
{
	if (!GetVisible())
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

int TrackRect::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int TrackRect::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

bool TrackRect::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        if (m_pTrackBtn != NULL) m_pTrackBtn->SetRefreshInnerAbsPos(true);

        IControl::RefreshInnerAbsPos();
    }

    return true;
}

void TrackRect::CalcSuborRect()
{
	int horizon_diff = m_nHorizonMax - m_nHorizonMin;
	int vertical_diff = m_nVerticalMax - m_nVerticalMin;

	if ((horizon_diff <= 0) || (vertical_diff <= 0))
	{
		m_pTrackBtn->SetVisible(false);
	}
	else
	{
		m_pTrackBtn->SetVisible(true);
	}
	
	int track_half_w = m_pTrackBtn->GetWidth() / 2;
	int track_half_h = m_pTrackBtn->GetHeight() / 2;
	
	int left = m_nHorizonValue* GetWidth() / horizon_diff;
	int top = m_nVerticalValue* GetHeight() / vertical_diff;
		
	m_pTrackBtn->SetLeft(left - track_half_w);
	m_pTrackBtn->SetTop(top - track_half_h);
}

void TrackRect::IncHorizonValue(int value)
{
	int diff = m_nHorizonMax - m_nHorizonMin;
	
	int old_value = m_nHorizonValue;
	
	m_nHorizonValue += value;

	if (m_nHorizonValue < 0)
	{
		m_nHorizonValue = 0;
	}

	if (m_nHorizonValue > diff)
	{
		m_nHorizonValue = diff;
	}
	
	CalcSuborRect();

	if (m_nHorizonValue != old_value)
	{
		if (GetDelegateControl() != NULL)
		{
			GetDelegateControl()->OnSuborEvent(this, 
				CGuiConst::EVENT_VALUE_CHANGED, 
				CVarList() << (old_value + m_nHorizonMin));
		}
		else
		{
			if (GetEnabled())
			{
				GuiUtil_RunCallback(this, "on_horizon_changed", 
					CVarList() << (old_value + m_nHorizonMin));
			}
		}
	}
}

void TrackRect::IncVerticalValue(int value)
{
	int diff = m_nVerticalMax - m_nVerticalMin;
	
	int old_value = m_nVerticalValue;
	
	m_nVerticalValue += value;

	if (m_nVerticalValue < 0)
	{
		m_nVerticalValue = 0;
	}

	if (m_nVerticalValue > diff)
	{
		m_nVerticalValue = diff;
	}
	
	CalcSuborRect();

	if (m_nVerticalValue != old_value)
	{
		if (GetDelegateControl() != NULL)
		{
			GetDelegateControl()->OnSuborEvent(this, 
				CGuiConst::EVENT_VALUE_CHANGED, 
				CVarList() << (old_value + m_nVerticalMin));
		}
		else
		{
			if (GetEnabled())
			{
				GuiUtil_RunCallback(this, "on_vertical_changed", 
					CVarList() << (old_value + m_nVerticalMin));
			}
		}
	}
}

void TrackRect::SetHorizonMin(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nHorizonMin = value;
	
	if (value >= m_nHorizonMax)
	{
		SetHorizonMax(value);
	}
}

int TrackRect::GetHorizonMin() const
{
	return m_nHorizonMin;
}

void TrackRect::SetHorizonMax(int value)
{
	if (value < m_nHorizonMin)
	{
		return;
	}
	
	m_nHorizonMax = value;

	IncHorizonValue(0);
}

int TrackRect::GetHorizonMax() const
{
	return m_nHorizonMax;
}

void TrackRect::SetHorizonValue(int value)
{
	value -= m_nHorizonMin;
	
	if ((value < 0) || (value > m_nHorizonMax))
	{
		return;
	}
	
	IncHorizonValue(value - m_nHorizonValue);
}

int TrackRect::GetHorizonValue() const
{
	return (m_nHorizonValue + m_nHorizonMin);
}

void TrackRect::SetVerticalMin(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nVerticalMin = value;
	
	if (value >= m_nVerticalMax)
	{
		SetVerticalMax(value);
	}
}

int TrackRect::GetVerticalMin() const
{
	return m_nVerticalMin;
}

void TrackRect::SetVerticalMax(int value)
{
	if (value < m_nVerticalMin)
	{
		return;
	}
	
	m_nVerticalMax = value;

	IncVerticalValue(0);
}

int TrackRect::GetVerticalMax() const
{
	return m_nVerticalMax;
}

void TrackRect::SetVerticalValue(int value)
{
	value -= m_nVerticalMin;
	
	if ((value < 0) || (value > m_nVerticalMax))
	{
		return;
	}
	
	IncVerticalValue(value - m_nVerticalValue);
}

int TrackRect::GetVerticalValue() const
{
	return (m_nVerticalValue + m_nVerticalMin);
}

PERSISTID TrackRect::GetTrackButton() const
{
	if (NULL == m_pTrackBtn)
	{
		return PERSISTID();
	}
	
	return m_pTrackBtn->GetID();
}

int TrackRect::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int horizon_diff = m_nHorizonMax - m_nHorizonMin;
	int vertical_diff = m_nVerticalMax - m_nVerticalMin;

	if ((horizon_diff <= 0) || (vertical_diff <= 0))
	{
		return 0;
	}
	
	int new_x = x - InnerGetAbsLeft();
	int new_horizon = new_x* horizon_diff / GetWidth() + m_nHorizonMin;
	
	int horizon_inc = new_horizon - m_nHorizonValue;
	
	if (horizon_inc != 0)
	{
		IncHorizonValue(new_horizon - m_nHorizonValue);
	}
	
	int new_y = y - InnerGetAbsTop();
	int new_vertical = new_y* vertical_diff / GetHeight() + m_nVerticalMin;
	
	int vertical_inc = new_vertical - m_nVerticalValue;
	
	if (vertical_inc != 0)
	{
		IncVerticalValue(new_vertical - m_nVerticalValue);
	}

	return 1;
}

int TrackRect::OnSuborEvent(IControl* subor, int event, 
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

		{
			int click_y = args.IntVal(1);
			
			int y = args.IntVal(3);

			int track_half_h = m_pTrackBtn->GetHeight() / 2;
			
			int new_y = y - m_pTrackBtn->GetAbsTop() + track_half_h;
			
			int delta_y = new_y - click_y;
			
			int new_top = m_pTrackBtn->GetTop() + delta_y;
			
			int vertical_diff = m_nVerticalMax - m_nVerticalMin;
			
			if (vertical_diff > 0)
			{
				int new_value = new_top	* vertical_diff 
					/ int(GetHeight()) + m_nVerticalMin;
				
				int inc = new_value - m_nVerticalValue;
				
				if (inc != 0)
				{
					IncVerticalValue(new_value - m_nVerticalValue);
				}
			}
		}

		{
			int click_x = args.IntVal(0);
			
			int x = args.IntVal(2);
			
			int track_half_w = m_pTrackBtn->GetWidth() / 2;
			
			int new_x = x - m_pTrackBtn->GetAbsLeft() + track_half_w;
			
			int delta_x = new_x - click_x;
			
			int new_left = m_pTrackBtn->GetLeft() + delta_x;
			
			int horizon_diff = m_nHorizonMax - m_nHorizonMin;
			
			if (horizon_diff > 0)
			{
				int new_value = new_left* horizon_diff / 
					int(GetWidth()) + m_nHorizonMin;
				
				int inc = new_value - m_nHorizonValue;
				
				if (inc != 0)
				{
					IncHorizonValue(new_value - m_nHorizonValue);
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

