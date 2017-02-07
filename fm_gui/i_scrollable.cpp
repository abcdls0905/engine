//--------------------------------------------------------------------
// 文件名:		i_scrollable.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "i_scrollable.h"
#include "scroll_bar.h"
#include "label.h"
#include "gui_scroll.h"

/// \file i_scrollable.cpp
/// \brief 滚动框控件

/// entity: IScrollable
/// \brief 滚动框控件基类
/// \see 继承自IControl
DECLARE_ABSTRACT_ENTITY(IScrollable, 0, IControl);

/// readonly: object HScrollBar
/// \brief 水平滚动条对象
DECLARE_PROPERTY_GET(PERSISTID, IScrollable, HScrollBar, GetHScrollBar);
/// readonly: object VScrollBar
/// \brief 垂直滚动条对象
DECLARE_PROPERTY_GET(PERSISTID, IScrollable, VScrollBar, GetVScrollBar);
/// readonly: CornorLabel
/// \brief 右下角标签对象
DECLARE_PROPERTY_GET(PERSISTID, IScrollable, CornerLabel, GetCornerLabel);

/// property: int ScrollSize
/// \brief 滚动条的尺寸
DECLARE_PROPERTY(int, IScrollable, ScrollSize, GetScrollSize, SetScrollSize);
/// property: bool HasVScroll
/// \brief 是否包含垂直滚动条
DECLARE_PROPERTY(bool, IScrollable, HasVScroll, GetHasVScroll, SetHasVScroll);
/// property: bool HasHScroll
/// \brief 是否包含水平滚动条
DECLARE_PROPERTY(bool, IScrollable, HasHScroll, GetHasHScroll, SetHasHScroll);
/// property: bool ClipChildren
/// \brief 是否要剪裁子控件
DECLARE_PROPERTY(bool, IScrollable, ClipChildren, GetClipChildren, 
	SetClipChildren);
/// property: bool AlwaysVScroll
/// \brief 是否永久显示垂直滚动条
DECLARE_PROPERTY(bool, IScrollable, AlwaysVScroll, GetAlwaysVScroll, 
	SetAlwaysVScroll);
/// property: bool AlwaysHScroll
/// \brief 是否永久显示水平滚动条
DECLARE_PROPERTY(bool, IScrollable, AlwaysHScroll, GetAlwaysHScroll, 
	SetAlwaysHScroll);
/// property: bool VScrollLeft
/// \brief 垂直滚动条是否在左边显示
DECLARE_PROPERTY(bool, IScrollable, VScrollLeft, GetVScrollLeft, 
	SetVScrollLeft);
/// property: bool HideScroll
/// \brief 是否隐藏滚动条
DECLARE_PROPERTY(bool, IScrollable, HideScroll, GetHideScroll, SetHideScroll);
/// property: bool Solid
/// \brief 是否用背景色填充
DECLARE_PROPERTY(bool, IScrollable, Solid, GetSolid, SetSolid);

/// event: int on_vscroll_changed(object self, int value)
/// \brief 滚动时回调
/// \param self 本控件
/// \param value 当前index值
DECLARE_EVENT(on_vscroll_changed);

// IScrollable

IScrollable::IScrollable()
{
	m_pHScrollBar = NULL;
	m_pVScrollBar = NULL;
	m_pCornerLabel = NULL;
	m_nScrollSize = DEF_SCROLL_SIZE;
	m_bClipChildren = true;
	m_bAlwaysVScroll = false;
	m_bAlwaysHScroll = false;
	m_bHasVScroll = false;
	m_bHasHScroll = false;
	m_bVScrollLeft = false;
	m_bHideScroll = false;
	m_bSolid = true;
	m_bInUpdate = false;
	SetDesignSize(48, 50);
}

IScrollable::~IScrollable()
{
}

bool IScrollable::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	return true;
}

bool IScrollable::Shut()
{
	SAFE_RELEASE(m_pHScrollBar);
	SAFE_RELEASE(m_pVScrollBar);
	SAFE_RELEASE(m_pCornerLabel);
	
	return IControl::Shut();
}

void IScrollable::Paint(float seconds)
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
		if (m_bSolid)
		{
			GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		}
	}

	int cx1 = GetContentLeft();
	int cy1 = GetContentTop();
	int cx2 = cx1 + GetContentWidth();
	int cy2 = cy1 + GetContentHeight();
	
	if (m_bClipChildren)
	{
		if (GetPainter()->SetViewport(cx1, cy1, cx2, cy2))
		{
			// 派生类画滚动内容
			PaintScrollRect(seconds);
			GetPainter()->ResetViewport();
		}
	}
	else
	{
		// 不剪裁子控件
		PaintScrollRect(seconds);
	}

	if (!m_bHideScroll)
	{
		if (m_pCornerLabel && m_pCornerLabel->GetVisible())
		{
			m_pCornerLabel->Paint(seconds);
		}
		
		if (m_pHScrollBar && m_pHScrollBar->GetVisible())
		{
			m_pHScrollBar->Paint(seconds);
		}
		
		if (m_pVScrollBar && m_pVScrollBar->GetVisible())
		{
			m_pVScrollBar->Paint(seconds);
		}
	}
	
	if (!GetNoFrame())
	{
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}
}

void IScrollable::PrepareDesignInfo(IDesignInfo* di)
{
    IControl::PrepareDesignInfo(di);

    CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("ClipChildren", IDesignInfo::TYPE_BOOL, true, true,
		&v);
    di->AddEventInfo("on_vscroll_changed");
}

bool IScrollable::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pHScrollBar && !m_pHScrollBar->GetLoadFinish())
	{
		return false;
	}

	if (m_pVScrollBar && !m_pVScrollBar->GetLoadFinish())
	{
		return false;
	}

	if (m_pCornerLabel && !m_pCornerLabel->GetLoadFinish())
	{
		return false;
	}

	return IControl::GetLoadFinish();
}

PERSISTID IScrollable::GetHScrollBar() const
{
	if (NULL == m_pHScrollBar)
	{
		return PERSISTID();
	}

	return m_pHScrollBar->GetID();
}

PERSISTID IScrollable::GetVScrollBar() const
{
	if (NULL == m_pVScrollBar)
	{
		return PERSISTID();
	}

	return m_pVScrollBar->GetID();
}

PERSISTID IScrollable::GetCornerLabel() const
{
	if (NULL == m_pCornerLabel)
	{
		return PERSISTID();
	}

	return m_pCornerLabel->GetID();
}

void IScrollable::SetScrollSize(int value)
{
	if ((value < 1) || (value >= 100))
	{
		return;
	}

	m_nScrollSize = value;

	if (m_pVScrollBar)
	{
		m_pVScrollBar->SetWidth(m_nScrollSize);
	}

	if (m_pHScrollBar)
	{
		m_pHScrollBar->SetHeight(m_nScrollSize);
	}

	UpdateScroll();
}

int IScrollable::GetScrollSize() const
{
	return m_nScrollSize;
}

void IScrollable::SetAlwaysVScroll(bool value) 
{ 
	m_bAlwaysVScroll = value; 
	
	if (m_bAlwaysVScroll)
	{
		SetHasVScroll(true);
	}
	
	UpdateScroll();
}

bool IScrollable::GetAlwaysVScroll() const 
{ 
	return m_bAlwaysVScroll; 
}

void IScrollable::SetAlwaysHScroll(bool value)
{
	m_bAlwaysHScroll = value;

	if (m_bAlwaysHScroll)
	{
		SetHasHScroll(true);
	}
	
	UpdateScroll();
}

bool IScrollable::GetAlwaysHScroll() const
{
	return m_bAlwaysHScroll;
}

void IScrollable::SetHasVScroll(bool value)
{
	m_bHasVScroll = value;

	if (m_bHasVScroll && (m_pVScrollBar == NULL))
	{
		m_pVScrollBar = CGuiScroll::CreateVScrollBar(this, m_nScrollSize);
	}

	if (m_pVScrollBar && m_pHScrollBar && (m_pCornerLabel == NULL))
	{
		m_pCornerLabel = CGuiScroll::CreateCornorLabel(this);
	}

    if (!m_bHasVScroll && (m_pVScrollBar != NULL))
    {
        this->RemoveSuborControl(m_pVScrollBar);
        SAFE_RELEASE(m_pVScrollBar);

        if (m_pCornerLabel != NULL)
        {
            this->RemoveSuborControl(m_pCornerLabel);
            SAFE_RELEASE(m_pCornerLabel);
        }
    }

	UpdateScroll();
}

bool IScrollable::GetHasVScroll() const
{
	return m_bHasVScroll;
}

void IScrollable::SetHasHScroll(bool value) 
{ 
	m_bHasHScroll = value; 

	if (m_bHasHScroll && (m_pHScrollBar == NULL))
	{
		m_pHScrollBar = CGuiScroll::CreateHScrollBar(this, m_nScrollSize);
	}
	
	if (m_pVScrollBar && m_pHScrollBar && (m_pCornerLabel == NULL))
	{
		m_pCornerLabel = CGuiScroll::CreateCornorLabel(this);
	}
	
	UpdateScroll();
}

bool IScrollable::GetHasHScroll() const 
{ 
	return m_bHasHScroll; 
}

void IScrollable::SetVScrollLeft(bool value)
{
	m_bVScrollLeft = value;
}

bool IScrollable::GetVScrollLeft() const
{
	return m_bVScrollLeft;
}

void IScrollable::SetHideScroll(bool value)
{
	m_bHideScroll = value;
}

bool IScrollable::GetHideScroll() const
{
	return m_bHideScroll;
}

void IScrollable::SetSolid(bool value)
{
	m_bSolid = value;
}

bool IScrollable::GetSolid() const
{
	return m_bSolid;
}

bool IScrollable::VScrollWillVisible() const
{
	if (m_bAlwaysVScroll)
	{
		return true;
	}

	return (m_pVScrollBar && m_pVScrollBar->GetVisible());
}

bool IScrollable::HScrollWillVisible() const
{
	if (m_bAlwaysHScroll)
	{
		return true;
	}
	
	return (m_pHScrollBar && m_pHScrollBar->GetVisible());
}

unsigned int IScrollable::CalcScrollVisible() const
{
	if (m_pVScrollBar)
	{
		m_pVScrollBar->SetVisible(CGuiScroll::GetVerticalVisible(
			m_pVScrollBar, GetVerticalCanDisp(), GetVerticalTotal(),
			m_bAlwaysVScroll));
	}

	if (m_pHScrollBar)
	{
		m_pHScrollBar->SetVisible(CGuiScroll::GetHorizonVisible(
			m_pHScrollBar, GetHorizonCanDisp(), GetHorizonTotal(),
			m_bAlwaysHScroll));
	}

	return CGuiScroll::GetScrollState(m_pVScrollBar, m_pHScrollBar);
}

bool IScrollable::UpdateScroll()
{
	if (m_bInUpdate)
	{
		return false;
	}
	
	unsigned int state = 0;
	
	while (true)
	{
		unsigned int new_state = CalcScrollVisible();
		
		if (new_state == state)
		{
			break;
		}
		else
		{
			state = new_state;
		}
	}
	
	if (!CGuiScroll::TestScrollValid(m_pVScrollBar))
	{
		// 滚动条无效时保证回到顶点
		SetVerticalValue(0);
	}
	
	if (!CGuiScroll::TestScrollValid(m_pHScrollBar))
	{
		// 滚动条无效时保证回到顶点
		SetHorizonValue(0);
	}

	CGuiScroll::UpdateVerticalScroll(m_pVScrollBar, 
		GetVerticalCanDisp(), GetVerticalTotal(), GetVerticalValue());
	CGuiScroll::UpdateHorizonScroll(m_pHScrollBar,
		GetHorizonCanDisp(), GetHorizonTotal(), GetHorizonValue());
	CGuiScroll::CalcScrollRect(this, m_pVScrollBar, m_pHScrollBar, 
		m_pCornerLabel, state, m_bVScrollLeft);
	
	return true;
}

bool IScrollable::BeginUpdate()
{
	if (m_bInUpdate)
	{
		return false;
	}
	
	m_bInUpdate = true;
	
	return true;
}

bool IScrollable::EndUpdate()
{
	if (!m_bInUpdate)
	{
		return false;
	}
	
	m_bInUpdate = false;
	UpdateScroll();
	
	return true;
}

int IScrollable::GetContentLeft() const
{
	if (m_bVScrollLeft && VScrollWillVisible() && m_pVScrollBar)
	{
		return InnerGetAbsLeft() + m_pVScrollBar->GetWidth();
	}
	else
	{
		return InnerGetAbsLeft();
	}
}

int IScrollable::GetContentTop() const
{
	return InnerGetAbsTop();
}

int IScrollable::GetContentWidth() const
{
	if (VScrollWillVisible() && m_pVScrollBar)
	{
		return GetWidth() - m_pVScrollBar->GetWidth();
	}
	else
	{
		return GetWidth();
	}
}

int IScrollable::GetContentHeight() const
{
	if (HScrollWillVisible() && m_pHScrollBar)
	{
		return GetHeight() - m_pHScrollBar->GetHeight();
	}
	else
	{
		return GetHeight();
	}
}

int IScrollable::OnMouseWheel(int x, int y, int delta)
{
	if (!CGuiScroll::TestScrollValid(m_pVScrollBar))
	{
		return 0;
	}
	
	m_pVScrollBar->IncValue(-delta* m_pVScrollBar->GetSmallChange());
	
	return 1;
}

int IScrollable::OnWidthChanged(int old)
{
	UpdateScroll();
	
	return 1;
}

int IScrollable::OnHeightChanged(int old)
{
	UpdateScroll();
	
	return 1;
}

int IScrollable::OnSuborEvent(IControl* subor, int event, 
	const IVarList& args)
{
	if (event == CGuiConst::EVENT_VALUE_CHANGED)
	{
		if (subor == m_pVScrollBar)
		{
			SetVerticalValue(m_pVScrollBar->GetCurValue());
            GuiUtil_RunCallback(this, "on_vscroll_changed", 
				CVarList() << m_pVScrollBar->GetCurValue());
			return 1;
		}
		else if (subor == m_pHScrollBar)
		{
			SetHorizonValue(m_pHScrollBar->GetCurValue());
			return 1;
		}
	}
	
	return 0;
}

IControl* IScrollable::OnGetScrollBar(int x, int y)
{
	if (m_pVScrollBar && m_pVScrollBar->GetVisible())
	{
		IControl* pControl = m_pVScrollBar->GetInWhat(x, y);

		if (pControl)
		{
			return pControl;
		}
	}

	if (m_pHScrollBar && m_pHScrollBar->GetVisible())
	{
		IControl* pControl = m_pHScrollBar->GetInWhat(x, y);

		if (pControl)
		{
			return pControl;
		}
	}

	return NULL;
}

bool IScrollable::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        m_nInnerAbsLeft = GetAbsLeft();
        m_nInnerAbsTop = GetAbsTop();

        if (m_pVScrollBar != NULL) 
		{
			m_pVScrollBar->SetRefreshInnerAbsPos(true);
		}

        if (m_pHScrollBar != NULL) 
		{
			m_pHScrollBar->SetRefreshInnerAbsPos(true);
		}

        if (m_pCornerLabel != NULL) 
		{
			m_pCornerLabel->SetRefreshInnerAbsPos(true);
		}

        SetRefreshInnerAbsPos(false);
    }

    return true;
}
