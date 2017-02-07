//--------------------------------------------------------------------
// 文件名:		form.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "form.h"
#include "group_box.h"
#include "scroll_bar.h"
#include "label.h"
#include "gui_scroll.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

/// \file form.cpp
/// \brief 表单

/// entity: Form
/// \brief 表单实体
/// \see 继承自IContainer
DECLARE_ENTITY(Form, 0, IContainer);

/// readonly: object Panel
/// \brief 内嵌面板对象
DECLARE_PROPERTY_GET(PERSISTID, Form, Panel, GetPanel);
/// readonly: object HScrollBar
/// \brief 水平滚动条对象
DECLARE_PROPERTY_GET(PERSISTID, Form, HScrollBar, GetHScrollBar);
/// readonly: object VScrollBar
/// \brief 垂直滚动条对象
DECLARE_PROPERTY_GET(PERSISTID, Form, VScrollBar, GetVScrollBar);
/// readonly: object CornerLabel
/// \brief 右下角标签对象
DECLARE_PROPERTY_GET(PERSISTID, Form, CornerLabel, GetCornerLabel);

/// readonly: bool IsDesktop
/// \brief 是否主窗口表单
DECLARE_PROPERTY_GET(bool, Form, IsDesktop, GetIsDesktop);
/// readonly: bool IsModal
/// \brief 是否模式窗口表单
DECLARE_PROPERTY_GET(bool, Form, IsModal, GetIsModal);

/// property: bool Fixed
/// \brief 是否固定位置
DECLARE_PROPERTY(bool, Form, Fixed, GetFixed, SetFixed);
/// property: bool Transparent
/// \brief 是否透明
DECLARE_PROPERTY(bool, Form, Transparent, GetTransparent, SetTransparent);
/// property: bool ClipChildren
/// \brief 是否要剪裁子控件
DECLARE_PROPERTY(bool, Form, ClipChildren, GetClipChildren, 
	SetClipChildren);
/// property: bool LimitInScreen
/// \brief 是否限制任何部分不可拖动超出屏幕范围
DECLARE_PROPERTY(bool, Form, LimitInScreen, GetLimitInScreen, 
	SetLimitInScreen);

/// property: bool ShowGrid
/// \brief 是否显示网格
DECLARE_PROPERTY(bool, Form, ShowGrid, GetShowGrid, SetShowGrid);
/// property: bool GridSize
/// \brief 网格间距
DECLARE_PROPERTY(int, Form, GridSize, GetGridSize, SetGridSize);
/// property: object MainMenu
/// \brief 主菜单对象
DECLARE_PROPERTY(PERSISTID, Form, MainMenu, GetMainMenuID, SetMainMenuID);
/// property: object Default
/// \brief 缺省执行控件对象
DECLARE_PROPERTY(PERSISTID, Form, Default, GetDefaultID, SetDefaultID);

/// event: int on_open(object self, object parent)
/// \brief 表单打开时的回调
/// \param self 本控件
/// \param parent 父控件
DECLARE_EVENT(on_open);
/// event: int on_close(object self, object parent)
/// \brief 表单关闭时的回调
/// \param self 本控件
/// \param parent 父控件
DECLARE_EVENT(on_close);
/// event: int on_move(object self, object parent)
/// \brief 移动表单时的回调
/// \param self 本控件
DECLARE_EVENT(on_move);

/// event: int on_active(object self)
/// \brief 窗口激活
/// \param self 本控件
DECLARE_EVENT(on_active);

/// event: int on_visible(object self, object parent)
/// \brief 控件恢复显示时候得回调
/// \param self 本控件
DECLARE_EVENT(on_visible);

/// event: int on_shut(object self, object parent)
/// \brief 表单释放删除时回调
/// \param self 本控件
/// \param parent 父控件
DECLARE_EVENT(on_shut);
// Form

Form::Form()
{
	m_nClickX = 0;
	m_nClickY = 0;
	m_bIsDesktop = false;
	m_bFixed = true;
	m_bTransparent = false;
	m_bClipChildren = true;
	m_bLimitInScreen = false;
	m_bIsModal = false;
	m_nGridSize = 8;
	m_bShowGrid = false;
	m_pPanel = NULL;
	m_pVScrollBar = NULL;
	m_pHScrollBar = NULL;
	m_pCornerLabel = NULL;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
    m_bVisibleEvent = true;
}

Form::~Form()
{
}

bool Form::Init(const IVarList& args)
{
	if (!IContainer::Init(args))
	{
		return false;
	}

	return true;
}

bool Form::Shut()
{
    GuiUtil_RunCallback(this, "on_shut", CVarList() << this->GetID());

	SAFE_RELEASE(m_pPanel);
	SAFE_RELEASE(m_pVScrollBar);
	SAFE_RELEASE(m_pHScrollBar);
	SAFE_RELEASE(m_pCornerLabel);

	return IContainer::Shut();
}

bool Form::GetIsModal() const
{
	return m_bIsModal;
}

void Form::SetIsDesktop(bool value)
{
	m_bIsDesktop = value;
}

bool Form::GetIsDesktop() const
{
	return m_bIsDesktop;
}

void Form::SetFixed(bool value) 
{ 
	m_bFixed = value; 
}

bool Form::GetFixed() const 
{ 
	return m_bFixed; 
}

void Form::SetTransparent(bool value)
{
	m_bTransparent = value;
}

bool Form::GetTransparent() const
{
	return m_bTransparent;
}

void Form::SetLimitInScreen(bool value)
{
	m_bLimitInScreen = value;
}

bool Form::GetLimitInScreen() const
{
	return m_bLimitInScreen;
}

void Form::SetShowGrid(bool value) 
{ 
	m_bShowGrid = value; 
}

bool Form::GetShowGrid() const 
{ 
	return m_bShowGrid; 
}

void Form::SetGridSize(int value)
{
	if ((value < 4) || (value > 16))
	{
		return;
	}

	m_nGridSize = value;
}

int Form::GetGridSize() const 
{ 
	return m_nGridSize; 
}

void Form::SetMainMenuID(const PERSISTID& value)
{
	if (!value.IsNull())
	{
		IEntity* pEntity = GetCore()->GetEntity(value);
		
		if (NULL == pEntity)
		{
			return;
		}

		if (!pEntity->GetEntInfo()->IsKindOf("MainMenu"))
		{
			return;
		}
	}
	
	m_MainMenuID = value;
}

PERSISTID Form::GetMainMenuID() const
{
	return m_MainMenuID;
}

void Form::SetDefaultID(const PERSISTID& value)
{
	m_DefaultID = value;
}

PERSISTID Form::GetDefaultID() const
{
	return m_DefaultID;
}

PERSISTID Form::GetPanel() const
{
	if (NULL == m_pPanel)
	{
		return PERSISTID();
	}

	return m_pPanel->GetID();
}

PERSISTID Form::GetHScrollBar() const
{
	if (NULL == m_pHScrollBar)
	{
		return PERSISTID();
	}
	
	return m_pHScrollBar->GetID();
}

PERSISTID Form::GetVScrollBar() const
{
	if (NULL == m_pVScrollBar)
	{
		return PERSISTID();
	}
	
	return m_pVScrollBar->GetID();
}

PERSISTID Form::GetCornerLabel() const
{
	if (NULL == m_pCornerLabel)
	{
		return PERSISTID();
	}
	
	return m_pCornerLabel->GetID();
}

void Form::PrepareDesignInfo(IDesignInfo* di)
{
	IContainer::PrepareDesignInfo(di);

	di->RemovePropInfo("Text");

    CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("LimitInScreen", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ShowGrid", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ClipChildren", IDesignInfo::TYPE_BOOL, true, true, 
		&v);
	
	di->AddSuborInfo("Panel", "GroupBox");
	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");
	
	di->AddEventInfo("on_open");
	di->AddEventInfo("on_close");
	di->AddEventInfo("on_move");
	di->AddEventInfo("on_active");
    di->AddEventInfo("on_shut");
	di->AddEventInfo("on_visible");
	
}

void Form::PaintBackground(float seconds)
{
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
		
		if (m_bShowGrid && GetDesignMode())
		{
			GetPainter()->DrawGrids(x1, y1, x2, y2, m_nGridSize, 
				CGuiConst::GRAY_COLOR);
		}
	}
}

void Form::Paint(float seconds)
{
	if (!GetVisible() || GetAlpha() == 0)
	{
		return;
	}
	
    RefreshInnerAbsPos();

    int old_alpha = GetPainter()->GetBlendAlpha();

    // 如果父界面透明了，子界面也应透明
    GetPainter()->SetBlendAlpha(old_alpha * GetAlpha() / 255);

	if (NULL == m_pPanel)
	{
		// 透明状态时不画背景
		if (!m_bTransparent)
		{
			PaintBackground(seconds);
		}

		int x1 = InnerGetAbsLeft();
		int y1 = InnerGetAbsTop();
		int x2 = x1 + GetWidth();
		int y2 = y1 + GetHeight();

		if (m_bClipChildren)
		{
			if (GetPainter()->SetViewport(x1, y1, x2, y2, !GetParentID().IsNull()))
			{
				// 画子控件
				IContainer::Paint(seconds);
				GetPainter()->ResetViewport();
			}
		}
		else
		{
			// 不剪裁子控件
			IContainer::Paint(seconds);
		}
	}
	else
	{
		int x1 = GetContentLeft();
		int y1 = GetContentTop();
		int x2 = x1 + GetContentWidth();
		int y2 = y1 + GetContentHeight();
		bool succeed = true;
		
		if (m_bClipChildren)
		{
			succeed = GetPainter()->SetViewport(x1, y1, x2, y2);
		}

		if (succeed)
		{
			if (GetNoFrame())
			{
				m_pPanel->SetLeft(-m_nOffsetX);
				m_pPanel->SetTop(-m_nOffsetY);
			}
			else
			{
				m_pPanel->SetLeft(-m_nOffsetX + 1);
				m_pPanel->SetTop(-m_nOffsetY + 1);
			}
			
			m_pPanel->Paint(seconds);
			
			// 画子控件
			IContainer::Paint(seconds);
			
			if (m_bClipChildren)
			{
				GetPainter()->ResetViewport();
			}
		}
		
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

	// 必须在此
	if ((!GetNoFrame()) && (!HasBackImage()))
	{
		// 画边框线
		int x1 = InnerGetAbsLeft();
		int y1 = InnerGetAbsTop();
		int x2 = x1 + GetWidth();
		int y2 = y1 + GetHeight();
		
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}

    // 恢复
    GetPainter()->SetBlendAlpha(old_alpha);
}

bool Form::AddChild(IControl* child)
{
	if (!IContainer::AddChild(child))
	{
		return false;
	}

	if (child->GetEntInfo()->IsKindOf("MainMenu"))
	{
		// 是主菜单
		m_MainMenuID = child->GetID();
	}

	return true;
}

// 窗口滚动

int Form::GetVerticalTotal() const
{ 
	if (m_pPanel)
	{
		return m_pPanel->GetHeight();
	}
	else
	{
		return 0; 
	}
}

int Form::GetVerticalCanDisp() const
{ 
	return GetContentHeight();
}

void Form::SetVerticalValue(int value)
{
	m_nOffsetY = value;

	if (m_pPanel)
	{
		m_pPanel->SetTop(-value);
	}
}

int Form::GetVerticalValue() const
{ 
	return m_nOffsetY; 
}

int Form::GetHorizonTotal() const
{ 
	if (m_pPanel)
	{
		return m_pPanel->GetWidth();
	}
	else
	{
		return 0; 
	}
}

int Form::GetHorizonCanDisp() const
{ 
	return GetContentWidth();
}

void Form::SetHorizonValue(int value)
{
	m_nOffsetX = value;
	
	if (m_pPanel)
	{
		m_pPanel->SetLeft(-value);
	}
}

void Form::SetVisible(bool val)
{
    bool visible = GetVisible();

    IContainer::SetVisible(val);

	if (m_bVisibleEvent && !visible && val)
	{
        m_bVisibleEvent = false;
		GuiUtil_RunCallback(this, "on_visible", CVarList() << this->GetID());
        m_bVisibleEvent = true;
	}
}

// 显示事件是否回调
void Form::SetVisibleEvent(bool value)
{
    m_bVisibleEvent = value;
}

bool Form::GetVisibleEvent() const
{
    return m_bVisibleEvent;
}

int Form::GetHorizonValue() const
{ 
	return m_nOffsetX; 
}

//
void Form::SetFormFile(const char* strFormFile)
{
	m_strFormFile = strFormFile;
}

const char* Form::GetFormFile()
{
	return m_strFormFile.c_str();
}
// 窗体名称
void Form::SetFormName(const char* strFormName)
{
	m_strFormName = strFormName;
}
const char* Form::GetFormName()
{
	return m_strFormName.c_str();
}

int Form::GetContentLeft() const
{
	return InnerGetAbsLeft();
}

int Form::GetContentTop() const
{
	return InnerGetAbsTop();
}

int Form::GetContentWidth() const
{
	if (m_pVScrollBar && m_pVScrollBar->GetVisible())
	{
		return GetWidth() - m_pVScrollBar->GetWidth();
	}
	else
	{
		return GetWidth();
	}
}

int Form::GetContentHeight() const
{
	if (m_pHScrollBar && m_pHScrollBar->GetVisible())
	{
		return GetHeight() - m_pHScrollBar->GetHeight();
	}
	else
	{
		return GetHeight();
	}
}

unsigned int Form::CalcScrollVisible() const
{
	if (m_pVScrollBar)
	{
		m_pVScrollBar->SetVisible(CGuiScroll::GetVerticalVisible(
			m_pVScrollBar, GetVerticalCanDisp(), GetVerticalTotal()));
	}
	
	if (m_pHScrollBar)
	{
		m_pHScrollBar->SetVisible(CGuiScroll::GetHorizonVisible(
			m_pHScrollBar, GetHorizonCanDisp(), GetHorizonTotal()));
	}
	
	return CGuiScroll::GetScrollState(m_pVScrollBar, m_pHScrollBar);
}

void Form::CalcSuborRect()
{
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
	
	CGuiScroll::CalcScrollRect(this, m_pVScrollBar, m_pHScrollBar, 
		m_pCornerLabel, state, false);
}

bool Form::UpdateScroll()
{
	CalcSuborRect();
	
	CGuiScroll::UpdateVerticalScroll(m_pVScrollBar, GetVerticalCanDisp(), 
		GetVerticalTotal(), GetVerticalValue());
	
	CGuiScroll::UpdateHorizonScroll(m_pHScrollBar, GetHorizonCanDisp(), 
		GetHorizonTotal(), GetHorizonValue());
	
	return true;
}

// 方法

PERSISTID Form::CreatePanel(int width, int height)
{
	if (m_bIsDesktop)
	{
		return PERSISTID();
	}
	
	if (NULL == m_pPanel)
	{
		m_pVScrollBar = CGuiScroll::CreateVScrollBar(this, 17);
		m_pHScrollBar = CGuiScroll::CreateHScrollBar(this, 17);
		m_pCornerLabel = CGuiScroll::CreateCornorLabel(this);	

		// 必须在后面创建以避免挡住滚动条
		m_pPanel = (GroupBox*)CreateSuborControl(this, "GroupBox");
		
		m_pPanel->SetNoFrame(true);
		m_pPanel->SetWidth(width);
		m_pPanel->SetHeight(height);
		
		UpdateScroll();
	}
	else
	{
		CORE_TRACE("(Form::CreatePanel)panel created");
	}

	return m_pPanel->GetID();
}

bool Form::SetPanelSize(int width, int height)
{
	if (NULL == m_pPanel)
	{
		return false;
	}
	
	m_pPanel->SetWidth(width);
	m_pPanel->SetHeight(height);
	
	UpdateScroll();
	
	return true;
}

bool Form::Show()
{
	if (GetParentControl() != NULL)
	{
		return false;
	}
	
	IControl* pDesktop = GetGui()->GetDesktopForm();
	if (NULL == pDesktop)
	{
		return false;
	}
	
	m_bIsModal = false;
	
	return pDesktop->AddChild(this);
}

bool Form::ShowModal()
{
	if (GetParentControl() != NULL)
	{
		return false;
	}
	
	m_bIsModal = true;
	
	return GetGui()->AddModalForm(this);
}

// 删除属于当前控件的显示在最上层的控件
static void remove_top_layer(IControl* control)
{
	Assert(control != NULL);

	control->RemoveTopLayer(control->GetID());

	size_t child_num = control->GetChildCount();

	for (size_t i = 0; i < child_num; ++i)
	{
		IControl* child = control->GetChildByIndex(i);

		if (child)
		{
			remove_top_layer(child);
		}
	}
}

bool Form::Close()
{
	SetVisible(false);

	// 删除属于当前窗口的显示在最上层的控件
	remove_top_layer(this);

	IControl* parent = GetParentControl();

	if (m_bIsModal)
	{
		// 可能删除自身实体
		GetGui()->RemoveModalForm(this);
	}
	else if (parent)
	{
        // 如果remove失败则在此断决父子关系
		if (!parent->RemoveChild(this))
        {
            this->SetParentControl(NULL);
        }
	}
	
	//if (m_bIsModal)
	//{
	//	GetGui()->RemoveModalForm(this);
	//}
	
	return true;
}

void get_tab_controls(IControl* control, 
	TArrayPod<IControl*, 1, TCoreAlloc>& result)
{
	Assert(control != NULL);
	
	size_t child_num = control->GetChildCount();

	for (size_t i = 0; i < child_num; ++i)
	{
		IControl* child = control->GetChildByIndex(i);

		if (child->IsContainer())
		{
			get_tab_controls(child, result);
		}
		else
		{
			if (child->GetTabStop())
			{
				result.push_back(child);
			}
		}
	}
}

IControl* Form::GetNextFocus(IControl* focus)
{
	TArrayPod<IControl*, 1, TCoreAlloc> result;

	get_tab_controls(this, result);

	if (result.empty())
	{
		return NULL;
	}

	// TAB索引最小的控件
	IControl* min_control = NULL;
	// 下一个控件
	IControl* next_control = NULL;

	int focus_index = -1;
	int next_index = -1;
	
	for (size_t i = 0; i < result.size(); ++i)
	{
		IControl* control = result[i];
		
		if (NULL == min_control)
		{
			min_control = control;
		}
		else if (control->GetTabIndex() < min_control->GetTabIndex())
		{
			min_control = control;
		}

		if (NULL == focus)
		{
			continue;
		}

		if (focus == control)
		{
			focus_index = (int)i;
			continue;
		}

		if (control->GetTabIndex() < focus->GetTabIndex())
		{
			continue;
		}

		if (NULL == next_control)
		{
			next_control = control;
			next_index = (int)i;
		}
		else if (control->GetTabIndex() < next_control->GetTabIndex())
		{
			next_control = control;
			next_index = (int)i;
		}
		else if (control->GetTabIndex() == next_control->GetTabIndex())
		{
			if (next_index < focus_index)
			{
				next_control = control;
				next_index = (int)i;
			}
		}
	}
	
	if (next_control != NULL)
	{
		return next_control;
	}
	
	return min_control;
}

// 回调

int Form::OnEnter(const PERSISTID& parent_id)
{
	return GuiUtil_RunCallback(this, "on_open", CVarList() << this->GetID()/*parent_id*/);
}

int Form::OnLeave(const PERSISTID& parent_id)
{
	return GuiUtil_RunCallback(this, "on_close", CVarList() << this->GetID()/*parent_id*/);
}

int Form::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (m_bFixed)
	{
		return 0;
	}
	
	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();
	
	Assert((m_nClickX >= 0) && (m_nClickX < GetWidth()));
	Assert((m_nClickY >= 0) && (m_nClickY < GetHeight()));
	
	// 移到最前面
	IControl* parent = GetParentControl();
	
	if (parent != NULL)
	{
		bool bRet = parent->BringToFront(this);
		if (bRet)
		{
			GuiUtil_RunCallback(this, "on_active", CVarList());
		}
	}
	
	return 1;
}

int Form::OnMouseWheel(int x, int y, int delta)
{
	if (NULL == m_pVScrollBar)
	{
		return 0;
	}
	
	m_pVScrollBar->IncValue(-delta * m_pVScrollBar->GetSmallChange());
	
	return 1;
}

int Form::OnDragMove(int x, int y)
{
	if (m_bFixed)
	{
		return 0;
	}
	
	int new_x = x - InnerGetAbsLeft();
	int new_y = y - InnerGetAbsTop();
	
	int delta_x = new_x - m_nClickX; 
	int delta_y = new_y - m_nClickY;
	
	/*
	if (GetGui()->GetDesktopForm() == this)
	{
		// 移动窗口
		HWND hwnd = (HWND)GetGui()->GetWinHandle();
		
		WINDOWPLACEMENT wp;
		
		GetWindowPlacement(hwnd, &wp);
		
		if (wp.showCmd == SW_SHOWNORMAL)
		{
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			
			RECT r = wp.rcNormalPosition;
			
			int left = r.left;
			int top = r.top;
			int width = r.right - r.left;
			int height = r.bottom - r.top;
			
			left += delta_x;
			top += delta_y;
			
			if (left < 0)
			{
				left = 0;
			}
			
			if (left >= cx)
			{
				left = cx - 1;
			}
			
			if (top < 0)
			{
				top = 0;
			}
			
			if (top >= cy)
			{
				top = cy - 1;
			}
			
			if ((left != r.left) || (top != r.top))
			{
				SetWindowPos(hwnd, HWND_TOP, left, top, width, height, 
					SWP_FRAMECHANGED);
			}
		}

		return 1;
	}
	*/
	
	int new_left = InnerGetAbsLeft() + delta_x;
	int new_top = InnerGetAbsTop() + delta_y;
	
	IControl* parent = GetParentControl();
	
	if (parent != NULL)
	{
		int parent_left = parent->GetAbsLeft();
		int parent_top = parent->GetAbsTop();
		int parent_right = parent_left + parent->GetWidth();
		int parent_bottom = parent_top + parent->GetHeight();
		
		if (new_left < parent_left)
		{
			new_left = parent_left;
		}
		
		if (new_left >= parent_right)
		{
			new_left = parent_right - 1;
		}
		
		if (new_top < parent_top)
		{
			new_top = parent_top;
		}
		
		if (new_top >= parent_bottom)
		{
			new_top = parent_bottom - 1;
		}
	}
	
	if (m_bLimitInScreen)
	{
		int render_width = GetPainter()->GetRenderWidth();
		int render_height = GetPainter()->GetRenderHeight();

		if ((new_left + GetWidth()) > render_width)
		{
			new_left = render_width - GetWidth();
		}

		if ((new_top + GetHeight()) > render_height)
		{
			new_top = render_height - GetHeight();
		}
	}

	SetAbsLeft(new_left);
	SetAbsTop(new_top);
	
	return 1;
}

int Form::OnDragLeave(int x, int y) 
{ 
	GuiUtil_RunCallback(this, "on_move", CVarList());
	
	return 1;
}

int Form::OnLeftChanged(int old)
{
	return IContainer::OnLeftChanged(old);
}

int Form::OnTopChanged(int old)
{
	return IContainer::OnTopChanged(old);
}

int Form::OnWidthChanged(int old)
{
	UpdateScroll();
	
	return IContainer::OnWidthChanged(old);
}

int Form::OnHeightChanged(int old)
{
	UpdateScroll();
	
	return IContainer::OnHeightChanged(old);
}

IControl* Form::OnGetInWhat(int x, int y)
{
	IControl* pControl = IContainer::OnGetInWhat(x, y);

	if (NULL == pControl)
	{
		return NULL;
	}

	if ((pControl == this) && m_bTransparent)
	{
		// 透明状态时
		if (!GetDesignMode())
		{
			return NULL;
		}
	}
	
	return pControl;
}

int Form::OnSuborEvent(IControl* subor, int event, const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (event == CGuiConst::EVENT_VALUE_CHANGED)
	{
		if (subor == m_pVScrollBar)
		{
			SetVerticalValue(m_pVScrollBar->GetCurValue());
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

