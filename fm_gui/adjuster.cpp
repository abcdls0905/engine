//--------------------------------------------------------------------
// 文件名:		adjuster.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "adjuster.h"

/// \file adjuster.cpp
/// \brief 分割调节器

/// entity: Adjuster
/// \brief 分割调节器控件
/// \see 继承自IControl
DECLARE_ENTITY(Adjuster, 0, IControl);

/// event: int on_drag_enter(object self)
/// \brief 开始拖动的回调
/// \param self 本控件
DECLARE_EVENT(on_drag_enter);
/// event: int on_drag_leave(object self, int new_x, int new_y)
/// \brief 结束拖动的回调
/// \param self 本控件
/// \param new_x 新的X坐标
/// \param new_y 新的Y坐标
DECLARE_EVENT(on_drag_leave);
/// event: int on_drag_move(object self, int new_x, int new_y)
/// \brief 拖动中的回调
/// \param self 本控件
/// \param new_x 新的X坐标
/// \param new_y 新的Y坐标
DECLARE_EVENT(on_drag_move);

// Adjuster

Adjuster::Adjuster()
{
	m_bPush = false;
	m_nClickX = 0;
	m_nClickY = 0;
	
	SetDesignSize(4, 20);
}

Adjuster::~Adjuster()
{
}

bool Adjuster::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	SetCursorName("WIN_SIZEWE");

	return true;
}

bool Adjuster::Shut()
{
	return IControl::Shut();
}

void Adjuster::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->AddEventInfo("on_drag_enter");
	di->AddEventInfo("on_drag_leave");
	di->AddEventInfo("on_drag_move");
}

void Adjuster::Paint(float seconds)
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

	bool draw_image = false;
	
	if (HasBackImage())
	{
		draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
	}
	
	if (!draw_image)
	{
		if (m_bPush)
		{
			GetPainter()->FillRect(x1, y1, x2, y2, GetForeColor());
		}
		else
		{
			GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		}
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}
}

int Adjuster::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();
	
	Assert( (m_nClickX >= 0) && (m_nClickX < GetWidth()) );
	Assert( (m_nClickY >= 0) && (m_nClickY < GetHeight()) );

	if (!m_bPush)
	{
		m_bPush = true;
	}

	GuiUtil_RunCallback(this, "on_drag_enter", CVarList());

	return 1; 
}

int Adjuster::OnLButtonUp(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (m_bPush)
	{
		int new_x = x - m_nClickX;
		int new_y = y - m_nClickY;

		GuiUtil_RunCallback(this, "on_drag_leave", 
			CVarList() << new_x << new_y);
		
		if (GetDelegateControl() != NULL)
		{
			GetDelegateControl()->OnSuborEvent(
				this, CGuiConst::EVENT_DRAG_LEAVE, 
				CVarList() << m_nClickX << m_nClickY << x << y);
		}
	}

	if (m_bPush)
	{
		m_bPush = false;
	}
	
	return 1; 
}

int Adjuster::OnDragMove(int x, int y)
{
	int new_x = x - m_nClickX;
	int new_y = y - m_nClickY;
	
	int res = GuiUtil_RunCallback(this, "on_drag_move", 
		CVarList() << new_x << new_y);

	if (res)
	{
		return res;
	}
	
	if (GetDelegateControl() != NULL)
	{
		return GetDelegateControl()->OnSuborEvent(
			this, CGuiConst::EVENT_DRAG_MOVE, 
			CVarList() << m_nClickX << m_nClickY << x << y);
	}

	return 0;
}

