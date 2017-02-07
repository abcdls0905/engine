//--------------------------------------------------------------------
// 文件名:		selector.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "selector.h"
#include "button.h"
#include "gui_input.h"

/// \file selector.cpp
/// \brief 选择器

/// entity: Selector
/// \brief 选择器实体
DECLARE_ENTITY(Selector, 0, IControl);

/// property: int EdgeSize
/// \brief 边框的宽度
DECLARE_PROPERTY(int, Selector, EdgeSize, GetEdgeSize, SetEdgeSize);
/// property: int GridSize
/// \brief 对齐网格的尺寸
DECLARE_PROPERTY(int, Selector, GridSize, GetGridSize, SetGridSize);
/// property: int SnapToGrid
/// \brief 是否对齐到网格
DECLARE_PROPERTY(bool, Selector, SnapToGrid, GetSnapToGrid, SetSnapToGrid);
/// property: bool Active
/// \brief 是否活动状态
DECLARE_PROPERTY(bool, Selector, Active, GetActive, SetActive);
/// property: object Bind
/// \brief 当前绑定的控件对象
DECLARE_PROPERTY(PERSISTID, Selector, Bind, GetBindID, SetBindID);

/// event: int on_rect_changed(object self)
/// \brief 选择范围发生改变
/// \param self 本控件
DECLARE_EVENT(on_rect_changed);
/// event: int on_drag_leave(object self, object subor)
/// \brief 拖动结束
/// \param self 本控件
/// \param subor 附属控件
DECLARE_EVENT(on_drag_leave);

// Selector

Selector::Selector()
{
	m_nClickX = 0;
	m_nClickY = 0;
	m_nEdgeSize = 5;
	m_nGridSize = 8;
	m_bSnapToGrid = true;
	m_bActive = true;

	m_nOldLeft = 0;
	m_nOldTop = 0;
	m_nOldWidth = 0;
	m_nOldHeight = 0;

	m_pLeftBtn = NULL;
	m_pTopBtn = NULL;
	m_pRightBtn = NULL;
	m_pBottomBtn = NULL;
	m_pLeftTopBtn = NULL;
	m_pRightTopBtn = NULL;
	m_pLeftBottomBtn = NULL;
	m_pRightBottomBtn = NULL;

	SetDesignSize(80, 20);
	
	SetCanFocus(true);
}

Selector::~Selector()
{
}

bool Selector::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	SetCursorName("WIN_SIZEALL");
	
	m_pLeftBtn = (Button*)CreateSuborControl(this, "Button");
	m_pTopBtn = (Button*)CreateSuborControl(this, "Button");
	m_pRightBtn = (Button*)CreateSuborControl(this, "Button");
	m_pBottomBtn = (Button*)CreateSuborControl(this, "Button");
	m_pLeftTopBtn = (Button*)CreateSuborControl(this, "Button");
	m_pRightTopBtn = (Button*)CreateSuborControl(this, "Button");
	m_pLeftBottomBtn = (Button*)CreateSuborControl(this, "Button");
	m_pRightBottomBtn = (Button*)CreateSuborControl(this, "Button");

	m_pLeftBtn->SetCursorName("WIN_SIZEWE");
	m_pTopBtn->SetCursorName("WIN_SIZENS");
	m_pRightBtn->SetCursorName("WIN_SIZEWE");
	m_pBottomBtn->SetCursorName("WIN_SIZENS");
	m_pLeftTopBtn->SetCursorName("WIN_SIZENWSE");
	m_pRightTopBtn->SetCursorName("WIN_SIZENESW");
	m_pLeftBottomBtn->SetCursorName("WIN_SIZENESW");
	m_pRightBottomBtn->SetCursorName("WIN_SIZENWSE");
	
	CalcSuborRect();
	
	return true;
}

bool Selector::Shut()
{
	return IControl::Shut();
}

bool Selector::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pLeftBtn && !m_pLeftBtn->GetLoadFinish())
	{
		return false;
	}
	
	if (m_pTopBtn && !m_pTopBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pRightBtn && !m_pRightBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pBottomBtn && !m_pBottomBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pLeftTopBtn && !m_pLeftTopBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pRightTopBtn && !m_pRightTopBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pLeftBottomBtn && !m_pLeftBottomBtn->GetLoadFinish())
	{
		return false;
	}

	if (m_pRightBottomBtn && !m_pRightBottomBtn->GetLoadFinish())
	{
		return false;
	}

	return IControl::GetLoadFinish();
}

void Selector::Paint(float seconds)
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

	if (m_bActive)
	{
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		GetPainter()->DrawRect(x1 + m_nEdgeSize, y1 + m_nEdgeSize, 
			x2 - m_nEdgeSize, y2 - m_nEdgeSize, GetLineColor());

		m_pLeftBtn->Paint(seconds);
		m_pTopBtn->Paint(seconds);
		m_pRightBtn->Paint(seconds);
		m_pBottomBtn->Paint(seconds);
		m_pLeftTopBtn->Paint(seconds);
		m_pRightTopBtn->Paint(seconds);
		m_pLeftBottomBtn->Paint(seconds);
		m_pRightBottomBtn->Paint(seconds);
	}
	else
	{
		GetPainter()->DrawRect(x1 + m_nEdgeSize, y1 + m_nEdgeSize, 
			x2 - m_nEdgeSize, y2 - m_nEdgeSize, GetLineColor());
	}
}

void Selector::SetActive(bool value)
{
	m_bActive = value;
}

bool Selector::GetActive() const
{
	return m_bActive;
}

void Selector::SetEdgeSize(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nEdgeSize = value;
}

int Selector::GetEdgeSize() const
{
	return m_nEdgeSize;
}

void Selector::SetGridSize(int value)
{
	if ((value < 4) || (value > 16))
	{
		return;
	}

	m_nGridSize = value;
}

int Selector::GetGridSize() const
{
	return m_nGridSize;
}

void Selector::SetSnapToGrid(bool value)
{
	m_bSnapToGrid = value;
}

bool Selector::GetSnapToGrid() const
{
	return m_bSnapToGrid;
}

void Selector::SetBindID(const PERSISTID& value)
{
	IControl* pControl = GetControl(value);

	if (NULL == pControl)
	{
		return;
	}

	m_BindID = pControl->GetID();

	SyncFrame();
}

PERSISTID Selector::GetBindID() const
{
	return m_BindID;
}

bool Selector::SyncFrame()
{
	IControl* pControl = GetControl(m_BindID);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	SetAbsLeft(pControl->GetAbsLeft() - m_nEdgeSize);
	SetAbsTop(pControl->GetAbsTop() - m_nEdgeSize);
	SetWidth(pControl->GetWidth() + m_nEdgeSize * 2);
	SetHeight(pControl->GetHeight() + m_nEdgeSize * 2);

	return true;
}

bool Selector::SetFrameRegion(int left, int top, int right, int bottom)
{
	int w = right - left;
	int h = bottom - top;

	if ((w < 0) || (h < 0))
	{
		return false;
	}

	SetLeft(left - m_nEdgeSize);
	SetTop(top - m_nEdgeSize);
	SetWidth(w + m_nEdgeSize * 2);
	SetHeight(h + m_nEdgeSize * 2);
	
	return true;
}

int Selector::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();
	
	Assert( (m_nClickX >= 0) && (m_nClickX < GetWidth()) );
	Assert( (m_nClickY >= 0) && (m_nClickY < GetHeight()) );
	
	m_nOldLeft = GetLeft();
	m_nOldTop = GetTop();
	m_nOldWidth = GetWidth();
	m_nOldHeight = GetHeight();

	return 1;
}

int Selector::OnLButtonUp(int x, int y)
{
	if ((m_nOldLeft != GetLeft())
		|| (m_nOldTop != GetTop())
		|| (m_nOldWidth != GetWidth())
		|| (m_nOldHeight != GetHeight()))
	{
		OnRectChanged();
	}

	return 1;
}

int Selector::OnDragMove(int x, int y)
{
	int new_x = x - InnerGetAbsLeft();
	int new_y = y - InnerGetAbsTop();
	
	int delta_x = new_x - m_nClickX; 
	int delta_y = new_y - m_nClickY;

	int delta = GetDelta();

	if (GuiUtil_ShiftPressed())
	{
		int new_width = (GetWidth() + delta_x) / delta* delta;
		int new_height = (GetHeight() + delta_y) / delta* delta;

		m_nClickX = new_x / delta* delta;
		m_nClickY = new_y / delta* delta;
		
		if (new_width < 0)
		{
			new_width = 0;
		}

		if (new_height < 0)
		{
			new_height = 0;
		}
		
		SetRectWidth(new_width);
		SetRectHeight(new_height);
	}
	else
	{
		int new_left = (GetLeft() + delta_x) / delta* delta;
		int new_top = (GetTop() + delta_y) / delta* delta;

		SetRectLeft(new_left);
		SetRectTop(new_top);
	}

	return 1;
}

int Selector::OnKeyDown(unsigned int vk)
{
	IControl* parent = GetParentControl();

	if (NULL == parent)
	{
		return 0;
	}
	
	int delta = GetDelta();

	if (GuiUtil_ShiftPressed())
	{
		switch (vk)
		{
		case CGuiInput::KEY_UP:
			{
				int h = (GetHeight() - delta) / delta* delta;

				// 键盘控制的时候要限制范围
				SetRectHeight(GuiUtil_NormalizeHeight(parent, h));
			}	
			break;
		case CGuiInput::KEY_DOWN:
			{
				int h = (GetHeight() + delta) / delta* delta;

				SetRectHeight(GuiUtil_NormalizeHeight(parent, h));
			}	
			break;
		case CGuiInput::KEY_LEFT:
			{
				int w = (GetWidth() - delta) / delta* delta;

				SetRectWidth(GuiUtil_NormalizeWidth(parent, w));
			}	
			break;
		case CGuiInput::KEY_RIGHT:
			{
				int w = (GetWidth() + delta) / delta* delta;

				SetRectWidth(GuiUtil_NormalizeWidth(parent, w));
			}	
			break;
		default:
			return 0;
		}
	}
	else
	{
		switch (vk)
		{
		case CGuiInput::KEY_UP:
			{
				int t = (GetTop() - delta) / delta* delta;

				SetRectTop(GuiUtil_NormalizeTop(parent, t));
			}	
			break;
		case CGuiInput::KEY_DOWN:
			{
				int t = (GetTop() + delta) / delta* delta;

				SetRectTop(GuiUtil_NormalizeTop(parent, t));
			}	
			break;
		case CGuiInput::KEY_LEFT:
			{
				int l = (GetLeft() - delta) / delta* delta;

				SetRectLeft(GuiUtil_NormalizeLeft(parent, l));
			}	
			break;
		case CGuiInput::KEY_RIGHT:
			{
				int l = (GetLeft() + delta) / delta* delta;

				SetRectLeft(GuiUtil_NormalizeLeft(parent, l));
			}	
			break;
		default:
			return 0;
		}
	}
	
	OnRectChanged();

	return 1;
}

int Selector::GetDelta() const
{
	if (GuiUtil_ControlPressed())
	{
		if (m_bSnapToGrid)
		{
			return 1;
		}
		else
		{
			return m_nGridSize;
		}
	}
	else
	{
		if (m_bSnapToGrid)
		{
			return m_nGridSize;
		}
		else
		{
			return 1;
		}
	}
}

void Selector::CalcSuborRect()
{
	m_pLeftBtn->SetWidth(GetEdgeSize());
	m_pLeftBtn->SetHeight(GetEdgeSize());
	m_pLeftBtn->SetLeft(0);
	m_pLeftBtn->SetTop(
		(GetHeight() - m_pLeftBtn->GetHeight()) / 2);
	
	m_pTopBtn->SetWidth(GetEdgeSize());
	m_pTopBtn->SetHeight(GetEdgeSize());
	m_pTopBtn->SetLeft(
		(GetWidth() - m_pTopBtn->GetWidth()) / 2);
	m_pTopBtn->SetTop(0);
	
	m_pRightBtn->SetWidth(GetEdgeSize());
	m_pRightBtn->SetHeight(GetEdgeSize());
	m_pRightBtn->SetLeft(
		GetWidth() - m_pRightBtn->GetWidth());
	m_pRightBtn->SetTop(
		(GetHeight() - m_pRightBtn->GetHeight()) / 2);
	
	m_pBottomBtn->SetWidth(GetEdgeSize());
	m_pBottomBtn->SetHeight(GetEdgeSize());
	m_pBottomBtn->SetLeft(
		(GetWidth() - m_pBottomBtn->GetWidth()) / 2);
	m_pBottomBtn->SetTop(
		GetHeight() - m_pBottomBtn->GetHeight());
	
	m_pLeftTopBtn->SetWidth(GetEdgeSize());
	m_pLeftTopBtn->SetHeight(GetEdgeSize());
	m_pLeftTopBtn->SetLeft(0);
	m_pLeftTopBtn->SetTop(0);
	
	m_pRightTopBtn->SetWidth(GetEdgeSize());
	m_pRightTopBtn->SetHeight(GetEdgeSize());
	m_pRightTopBtn->SetLeft(
		GetWidth() - m_pRightTopBtn->GetWidth());
	m_pRightTopBtn->SetTop(0);
	
	m_pLeftBottomBtn->SetWidth(GetEdgeSize());
	m_pLeftBottomBtn->SetHeight(GetEdgeSize());
	m_pLeftBottomBtn->SetLeft(0);
	m_pLeftBottomBtn->SetTop(
		GetHeight() - m_pLeftBottomBtn->GetHeight());
	
	m_pRightBottomBtn->SetWidth(GetEdgeSize());
	m_pRightBottomBtn->SetHeight(GetEdgeSize());
	m_pRightBottomBtn->SetLeft(
		GetWidth() - m_pRightBottomBtn->GetWidth());
	m_pRightBottomBtn->SetTop(
		GetHeight() - m_pRightBottomBtn->GetHeight());
}

bool Selector::SetRectLeft(int value)
{
	SetLeft(value);

	return true;
}

bool Selector::SetRectTop(int value)
{
	SetTop(value);

	return true;
}

bool Selector::SetRectWidth(int value)
{
	if (value < (m_nEdgeSize * 2))
	{
		return false;
	}
	
	IControl* pControl = GetControl(m_BindID);
	
	if (pControl != NULL)
	{
		if ((value - m_nEdgeSize * 2) < pControl->GetMinWidth())
		{
			return false;
		}
	}
	
	SetWidth(value);

	return true;
}

bool Selector::SetRectHeight(int value)
{
	if (value < (m_nEdgeSize * 2))
	{
		return false;
	}
	
	IControl* pControl = GetControl(m_BindID);
	
	if (pControl != NULL)
	{
		if ((value - m_nEdgeSize * 2) < pControl->GetMinHeight())
		{
			return false;
		}
	}
	
	SetHeight(value);

	return true;
}

bool Selector::MoveLeft(IControl* parent, IControl* subor, 
						int delta, const IVarList& args)
{
	int click_x = args.IntVal(0);
	
	int x = args.IntVal(2);
	
	int new_x = x - subor->GetAbsLeft();
	
	int delta_x = new_x - click_x;
	
	int l = (GetLeft() + delta_x) / delta* delta;
	int w = GetWidth() + (GetLeft() - l);
				
	if (SetRectWidth(GuiUtil_NormalizeWidth(parent, w)))
	{
		SetRectLeft(GuiUtil_NormalizeLeft(parent, l));
	}
	
	return true;
}

bool Selector::MoveTop(IControl* parent, IControl* subor, 
					   int delta, const IVarList& args)
{
	int click_y = args.IntVal(1);
	
	int y = args.IntVal(3);
	
	int new_y = y - subor->GetAbsTop();
	
	int delta_y = new_y - click_y;
	
	int t = (GetTop() + delta_y) / delta* delta;
	int h = GetHeight() + (GetTop() - t);
				
	if (SetRectHeight(GuiUtil_NormalizeHeight(parent, h)))
	{
		SetRectTop(GuiUtil_NormalizeTop(parent, t));
	}
	
	return true;
}

bool Selector::MoveRight(IControl* parent, IControl* subor, 
						 int delta, const IVarList& args)
{
	int click_x = args.IntVal(0);
	
	int x = args.IntVal(2);
	
	int new_x = x - subor->GetAbsLeft();
	
	int delta_x = new_x - click_x;
	
	int w = (GetWidth() + delta_x) / delta* delta;
	
	SetRectWidth(GuiUtil_NormalizeWidth(parent, w));
	
	return true;
}

bool Selector::MoveBottom(IControl* parent, IControl* subor, 
						  int delta, const IVarList& args)
{
	int click_y = args.IntVal(1);
	
	int y = args.IntVal(3);
	
	int new_y = y - subor->GetAbsTop();
	
	int delta_y = new_y - click_y;
	
	int h = (GetHeight() + delta_y) / delta* delta;
	
	SetRectHeight(GuiUtil_NormalizeHeight(parent, h));
	
	return true;
}

int Selector::OnRectChanged()
{
	IControl* pControl = GetControl(m_BindID);
	
	if (pControl != NULL)
	{
		pControl->SetAbsLeft(GetAbsLeft() + m_nEdgeSize);
		pControl->SetAbsTop(GetAbsTop() + m_nEdgeSize);
		pControl->SetWidth(GetWidth() - m_nEdgeSize * 2);
		pControl->SetHeight(GetHeight() - m_nEdgeSize * 2);

		SyncFrame();
	}
	
	return GuiUtil_RunCallback(this, "on_rect_changed", CVarList());
}

int Selector::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int Selector::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int Selector::OnSuborEvent(IControl* subor, int event, 
						   const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (event == CGuiConst::EVENT_DRAG_MOVE)
	{
		IControl* parent = GetParentControl();
		
		if (NULL == parent)
		{
			return 0;
		}
		
		int delta = GetDelta();
		
		if (subor == m_pLeftBtn)
		{
			MoveLeft(parent, m_pLeftBtn, delta, args);
		}
		else if (subor == m_pTopBtn)
		{
			MoveTop(parent, m_pTopBtn, delta, args);
		}
		else if (subor == m_pRightBtn)
		{
			MoveRight(parent, m_pRightBtn, delta, args);
		}
		else if (subor == m_pBottomBtn)
		{
			MoveBottom(parent, m_pBottomBtn, delta, args);
		}
		else if (subor == m_pLeftTopBtn)
		{
			MoveLeft(parent, m_pLeftTopBtn, delta, args);
			MoveTop(parent, m_pLeftTopBtn, delta, args);
		}
		else if (subor == m_pRightTopBtn)
		{
			MoveRight(parent, m_pRightTopBtn, delta, args);
			MoveTop(parent, m_pRightTopBtn, delta, args);
		}
		else if (subor == m_pLeftBottomBtn)
		{
			MoveLeft(parent, m_pLeftBottomBtn, delta, args);
			MoveBottom(parent, m_pLeftBottomBtn, delta, args);
		}
		else if (subor == m_pRightBottomBtn)
		{
			MoveRight(parent, m_pRightBottomBtn, delta, args);
			MoveBottom(parent, m_pRightBottomBtn, delta, args);
		}
		else
		{
			return 0;
		}

		OnRectChanged();
		
		return 1;
	}

	if (event == CGuiConst::EVENT_DRAG_LEAVE)
	{
		GuiUtil_RunCallback(this, "on_drag_leave", 
			CVarList() << subor->GetID());
		
		return 1;
	}
	
	return 0;
}

bool Selector::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        if (m_pLeftBtn != NULL) m_pLeftBtn->SetRefreshInnerAbsPos(true);
        if (m_pTopBtn != NULL) m_pTopBtn->SetRefreshInnerAbsPos(true);
        if (m_pRightBtn != NULL) m_pRightBtn->SetRefreshInnerAbsPos(true);
        if (m_pBottomBtn != NULL) m_pBottomBtn->SetRefreshInnerAbsPos(true);
        if (m_pLeftTopBtn != NULL) m_pLeftTopBtn->SetRefreshInnerAbsPos(true);
        if (m_pRightTopBtn != NULL) m_pRightTopBtn->SetRefreshInnerAbsPos(true);
        if (m_pLeftBottomBtn != NULL) m_pLeftBottomBtn->SetRefreshInnerAbsPos(true);
        if (m_pRightBottomBtn != NULL) m_pRightBottomBtn->SetRefreshInnerAbsPos(true);

        IControl::RefreshInnerAbsPos();
    }

    return true;
}