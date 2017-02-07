//--------------------------------------------------------------------
// �ļ���:		scroll_bar.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "scroll_bar.h"
#include "button.h"

/// \file scroll_bar.cpp
/// \brief ������

/// entity: ScrollBar
/// \brief ������ʵ��
/// \see �̳���IControl
DECLARE_ENTITY(ScrollBar, 0, IControl);

/// readonly: object DecButton
/// \brief ��ֵ���ٰ�ť����
DECLARE_PROPERTY_GET(PERSISTID, ScrollBar, DecButton, GetDecButton);
/// readonly: object IncButton
/// \brief ��ֵ���Ӱ�ť����
DECLARE_PROPERTY_GET(PERSISTID, ScrollBar, IncButton, GetIncButton);
/// readonly: object TrackButton
/// \brief �϶���ť����
DECLARE_PROPERTY_GET(PERSISTID, ScrollBar, TrackButton, GetTrackButton);

/// property: int ButtonSize
/// \brief ������ť�Ĵ�С
DECLARE_PROPERTY(int, ScrollBar, ButtonSize, GetButtonSize, SetButtonSize);
/// property: int TrackSize
/// \brief �϶���ť�Ĵ�С��Ϊ0��ʾ���̶���С��
DECLARE_PROPERTY(int, ScrollBar, TrackSize, GetTrackSize, SetTrackSize);

/// property: bool FullBarBack
/// \brief ������������ʾ��Χ���Ƿ��������������ķ�Χ��������ť֮��ķ�Χ
DECLARE_PROPERTY(bool, ScrollBar, FullBarBack, GetFullBarBack, SetFullBarBack);

/// property: string Orient
/// \brief ����"Vertical"-��ֱ��"Horizon"-ˮƽ��
DECLARE_PROPERTY(const char*, ScrollBar, Orient, GetOrient, SetOrient);
/// property: int Minimum
/// \brief ��Сֵ
DECLARE_PROPERTY(int, ScrollBar, Minimum, GetMinimum, SetMinimum);
/// property: int Maximum
/// \brief ���ֵ
DECLARE_PROPERTY(int, ScrollBar, Maximum, GetMaximum, SetMaximum);
/// property: int Value
/// \brief ��ǰֵ
DECLARE_PROPERTY(int, ScrollBar, Value, GetCurValue, SetCurValue);
/// property: int SmallChange
/// \brief С�ı䵥λ�����������ť�ı仯����
DECLARE_PROPERTY(int, ScrollBar, SmallChange, 
				 GetSmallChange, SetSmallChange);
/// property: int LargeChange
/// \brief ��ı䵥λ��Ӱ���϶���ť�Ĵ�С��
DECLARE_PROPERTY(int, ScrollBar, LargeChange, 
				 GetLargeChange, SetLargeChange);

// ScrollBar

ScrollBar::ScrollBar()
{
	m_pDecBtn = NULL;
	m_pIncBtn = NULL;
	m_pTrackBtn = NULL;
    m_bFullBarBack = true;
	m_nButtonSize = DEF_BUTTON_SIZE;
	m_nTrackSize = 0;
	m_nOrient = 0;
	m_nValue = 0;
	m_nMinimum = 0;
	m_nMaximum = 10;
	m_nSmallChange = 1;
	m_nLargeChange = 5;

	SetDesignSize(18, 60);
}

ScrollBar::~ScrollBar()
{
}

bool ScrollBar::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	SetBackColor(SCROLL_BACK_COLOR);

	m_pDecBtn = (Button*)CreateSuborControl(this, "Button");
	m_pIncBtn = (Button*)CreateSuborControl(this, "Button");
	m_pTrackBtn = (Button*)CreateSuborControl(this, "Button");

	m_pDecBtn->SetHoverRepeat(true);
	m_pIncBtn->SetHoverRepeat(true);
	
	AdjustButtonStyle();
	CalcSuborRect();

	return true;
}

bool ScrollBar::Shut()
{
	SAFE_RELEASE(m_pDecBtn);
	SAFE_RELEASE(m_pIncBtn);
	SAFE_RELEASE(m_pTrackBtn);

	return IControl::Shut();
}

void ScrollBar::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("Font");
	di->RemovePropInfo("Text");
    CVar v = CVar(VTYPE_INT, DEF_BUTTON_SIZE);
	di->AddPropInfo("ButtonSize", IDesignInfo::TYPE_INT, true, true, &v);
    
	di->AddPropInfo("TrackSize", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_BOOL, true);
    di->AddPropInfo("FullBarBack", IDesignInfo::TYPE_BOOL, true, true,&v);
    v = CVar(VTYPE_STRING, "Vertical");
	di->AddPropInfo("Orient", IDesignInfo::TYPE_STRING, true, false, &v);
	di->AddPropInfo("Minimum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Maximum", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Value", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("SmallChange", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("LargeChange", IDesignInfo::TYPE_INT, true, true);

	di->AddSuborInfo("DecButton", "Button");
	di->AddSuborInfo("IncButton", "Button");
	di->AddSuborInfo("TrackButton", "Button");

	di->AddEventInfo("on_value_changed");
}

bool ScrollBar::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pIncBtn && !m_pIncBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pDecBtn && !m_pDecBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pTrackBtn && !m_pTrackBtn->GetLoadFinish())
	{
		return false;
	}

	return IControl::GetLoadFinish();
}

void ScrollBar::Paint(float seconds)
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
        if (m_bFullBarBack)
        {
            DrawBackImage(seconds, x1, y1, x2, y2);
        }
        else
        {
            if (m_nOrient == ORIENT_HORIZON)
            {
                // ˮƽ������
                x1 += m_pDecBtn->GetWidth();
                x2 -= m_pIncBtn->GetWidth();
            }
            else
            {
                // ��ֱ������
                y1 += m_pDecBtn->GetHeight();
                y2 -= m_pIncBtn->GetHeight();
            }

            DrawBackImage(seconds, x1, y1, x2, y2);
        }
        
	}
	else
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}

	m_pDecBtn->Paint(seconds);
	m_pIncBtn->Paint(seconds);
	m_pTrackBtn->Paint(seconds);
}

PERSISTID ScrollBar::GetDecButton() const
{
	if (NULL == m_pDecBtn)
	{
		return PERSISTID();
	}

	return m_pDecBtn->GetID();
}

PERSISTID ScrollBar::GetIncButton() const
{
	if (NULL == m_pIncBtn)
	{
		return PERSISTID();
	}

	return m_pIncBtn->GetID();
}

PERSISTID ScrollBar::GetTrackButton() const
{
	if (NULL == m_pTrackBtn)
	{
		return PERSISTID();
	}

	return m_pTrackBtn->GetID();
}

void ScrollBar::SetButtonSize(int value)
{
	if ((value < 0) || (value > 100))
	{
		return;
	}

	m_nButtonSize = value;

	CalcSuborRect();
}

int ScrollBar::GetButtonSize() const
{
	return m_nButtonSize;
}

void ScrollBar::SetTrackSize(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nTrackSize = value;

	CalcSuborRect();
}

int ScrollBar::GetTrackSize() const
{
	return m_nTrackSize;
}

// �������������ķ�Χ������������ť֮��ķ�Χ
void ScrollBar::SetFullBarBack(bool value)
{
    m_bFullBarBack = value;
}

bool ScrollBar::GetFullBarBack() const
{
    return m_bFullBarBack;
}

void ScrollBar::SetOrient(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "Horizon") == 0)
	{
		if (m_nOrient == ORIENT_VERTICAL)
		{
			m_nOrient = ORIENT_HORIZON;

			AdjustButtonStyle();
			SwapWidthHeight();
		}
	}
	else
	{
		if (m_nOrient == ORIENT_HORIZON)
		{
			m_nOrient = ORIENT_VERTICAL;
			
			AdjustButtonStyle();
			SwapWidthHeight();
		}
	}
}

const char* ScrollBar::GetOrient() const
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

void ScrollBar::SetMinimum(int value)
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

int ScrollBar::GetMinimum() const
{
	return m_nMinimum;
}

void ScrollBar::SetMaximum(int value)
{
	if (value < m_nMinimum)
	{
		return;
	}
	
	m_nMaximum = value;

	IncValue(0);
}

int ScrollBar::GetMaximum() const
{
	return m_nMaximum;
}

void ScrollBar::SetCurValue(int value)
{
	value -= m_nMinimum;

	if (value < 0)
	{
		return;
	}
	
	int diff = m_nMaximum - m_nMinimum;
	
	if (value > diff)
	{
		value = diff;
	}
	
	IncValue(value - m_nValue);
}

int ScrollBar::GetCurValue() const
{
	return (m_nValue + m_nMinimum);
}

void ScrollBar::SetSmallChange(int value)
{
	if (value < 1)
	{
		value = 1;
	}
	
	m_nSmallChange = value;
}

int ScrollBar::GetSmallChange() const
{
	return m_nSmallChange;
}

void ScrollBar::SetLargeChange(int value)
{
	if (value < 1)
	{
		value = 1;
	}
	
	m_nLargeChange = value;

	CalcSuborRect();
}

int ScrollBar::GetLargeChange() const
{
	return m_nLargeChange;
}

void ScrollBar::CalcSuborRect()
{
	int diff = m_nMaximum - m_nMinimum;

	Assert(diff >= 0);
	
	if (diff == 0)
	{
		m_pDecBtn->SetEnabled(false);
		m_pIncBtn->SetEnabled(false);
		m_pTrackBtn->SetVisible(false);
	}
	else
	{
		m_pDecBtn->SetEnabled(true);
		m_pIncBtn->SetEnabled(true);
		m_pTrackBtn->SetVisible(true);
	}

	if (m_nButtonSize == 0)
	{
		m_pDecBtn->SetVisible(false);
		m_pIncBtn->SetVisible(false);
	}
	
	if (m_nOrient == ORIENT_VERTICAL)
	{
		int button_size = GetHeight() / 3;

		//if (button_size > MulScaledValue(m_nButtonSize))
		//{
		//	button_size = MulScaledValue(m_nButtonSize);
		//}
		if (button_size > m_nButtonSize)
		{
			button_size = m_nButtonSize;
		}

		m_pDecBtn->SetLeft(0);
		m_pDecBtn->SetTop(0);
		m_pDecBtn->SetWidth(GetWidth());
		m_pDecBtn->SetHeight(button_size);

		m_pIncBtn->SetLeft(0);
		m_pIncBtn->SetTop(GetHeight() - button_size);
		m_pIncBtn->SetWidth(GetWidth());
		m_pIncBtn->SetHeight(button_size);

		int height = GetHeight() - button_size * 2;

		Assert(height > 0);

		// �϶���ť�ĸ߶�
		//int h = MulScaledValue(m_nTrackSize);
		int h = m_nTrackSize;

		if (0 == h)
		{
			h = m_nLargeChange * height / (diff + m_nLargeChange);

			if (h >= height)
			{
				h = height;
			}

			if (h < MIN_TRACK_SIZE)
			{
				h = MIN_TRACK_SIZE;
			}
		}

		int t = button_size;

		if (diff > 0)
		{
			t += m_nValue* (height - h) / diff;
		}

		m_pTrackBtn->SetLeft(0);
		m_pTrackBtn->SetTop(t);
		m_pTrackBtn->SetWidth(GetWidth());
		m_pTrackBtn->SetHeight(h);
	}
	else
	{
		int button_size = GetWidth() / 3;

		//if (button_size > MulScaledValue(m_nButtonSize))
		//{
		//	button_size = MulScaledValue(m_nButtonSize);
		//}
		if (button_size > m_nButtonSize)
		{
			button_size = m_nButtonSize;
		}

		m_pDecBtn->SetLeft(0);
		m_pDecBtn->SetTop(0);
		m_pDecBtn->SetWidth(button_size);
		m_pDecBtn->SetHeight(GetHeight());

		m_pIncBtn->SetAbsLeft(GetAbsLeft() + GetWidth() - button_size);
		m_pIncBtn->SetTop(0);
		m_pIncBtn->SetWidth(button_size);
		m_pIncBtn->SetHeight(GetHeight());

		int width = GetWidth() - button_size * 2;

		Assert(width > 0);

		// �϶���ť�Ŀ��
		//int w = MulScaledValue(m_nTrackSize);
		int w = m_nTrackSize;
		
		if (0 == m_nTrackSize)
		{
			w = m_nLargeChange * width / (diff + m_nLargeChange);

			if (w >= width)
			{
				w = width;
			}

			if (w < MIN_TRACK_SIZE)
			{
				w = MIN_TRACK_SIZE;
			}
		}

		int t = button_size;

		if (diff > 0)
		{
			t += m_nValue* (width - w) / diff;
		}

		m_pTrackBtn->SetLeft(t);
		m_pTrackBtn->SetTop(0);
		m_pTrackBtn->SetWidth(w);
		m_pTrackBtn->SetHeight(GetHeight());
	}
}

void ScrollBar::AdjustButtonStyle()
{
	if (m_nOrient == ORIENT_VERTICAL)
	{
		m_pDecBtn->SetStyleString("UpArrow");
		m_pIncBtn->SetStyleString("DownArrow");
	}
	else
	{
		m_pDecBtn->SetStyleString("LeftArrow");
		m_pIncBtn->SetStyleString("RightArrow");
	}
}

void ScrollBar::SwapWidthHeight()
{
	int w = GetWidth();
	int h = GetHeight();
	
	SetWidth(h);
	SetHeight(w);
}

void ScrollBar::IncValue(int value)
{
	int diff = m_nMaximum - m_nMinimum;
	
	int old_value = m_nValue;
	
	m_nValue += value;

	if (m_nValue < 0)
	{
		m_nValue = 0;
	}

	if (m_nValue > diff)
	{
		m_nValue = diff;
	}
	
	CalcSuborRect();

	if (m_nValue != old_value)
	{
		if (GetDelegateControl() != NULL)
		{
			GetDelegateControl()->OnSuborEvent(this, 
				CGuiConst::EVENT_VALUE_CHANGED, 
				CVarList() << (old_value + m_nMinimum));
		}
		else
		{
			if (GetEnabled())
			{
				GuiUtil_RunCallback(this, "on_value_changed", 
					CVarList() << (old_value + m_nMinimum));
			}
		}
	}
}

bool ScrollBar::Reset()
{
	IncValue(-(m_nValue - m_nMinimum));
	
	return true;
}

int ScrollBar::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int ScrollBar::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int ScrollBar::OnSuborEvent(IControl* subor, int event, 
							const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (event == CGuiConst::EVENT_CLICK)
	{
		if (subor == m_pDecBtn)
		{
			IncValue(- m_nSmallChange);
		}
		else if (subor == m_pIncBtn)
		{
			IncValue(m_nSmallChange);
		}

		return 1;
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
				int button_size = m_pDecBtn->GetHeight();
				
				int h = GetHeight() - button_size * 2 
					- m_pTrackBtn->GetHeight();

                int new_value = 0;
                int inc = 0;
                if (h <= 0)
                {
                    inc = 0;
                }
				else
                {
                    new_value = (new_top - button_size) 
					    * diff / h;
                    inc = new_value - m_nValue;
                }
				
				if (inc != 0)
				{
					IncValue(inc);
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
				int button_size = m_pDecBtn->GetWidth();
				
				int w = GetWidth() - button_size * 2 
					- m_pTrackBtn->GetWidth();

                if (w > 0)
                {
                    int new_value = (new_left - button_size) 
					* diff / w;

				    int inc = new_value - m_nValue;
    				
				    if (inc != 0)
				    {
					    IncValue(inc);
				    }
                }
			}
		}

		return 1;
	}

	if (event == CGuiConst::EVENT_DRAG_LEAVE)
	{
		IControl* parent = GetParentControl();
		
		if (parent != NULL)
		{
			return parent->OnSuborEvent(this, event, args);
		}
	}

	return 0;
}

bool ScrollBar::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        m_nInnerAbsLeft = GetAbsLeft();
        m_nInnerAbsTop = GetAbsTop();

        if (m_pDecBtn != NULL) m_pDecBtn->SetRefreshInnerAbsPos(true);
        if (m_pIncBtn != NULL) m_pIncBtn->SetRefreshInnerAbsPos(true);
        if (m_pTrackBtn != NULL) m_pTrackBtn->SetRefreshInnerAbsPos(true);

        SetRefreshInnerAbsPos(false);
    }

    return true;
}