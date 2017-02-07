//--------------------------------------------------------------------
// 文件名:		gui_msg_proc.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
//#define _WIN32_WINNT 0x0400
//#include <windows.h>
#include "gui.h"
#include "gui_input.h"
#include "i_control.h"
#include "main_menu.h"
#include "menu.h"
#include "selector.h"
#include "form.h"
#include "caret.h"
#include "cursor.h"
//#include "GameHand.h"
#include "../public/core_mem.h"
//#pragma comment(lib, "imm32.lib")
#include "../public/converts.h"

#ifdef _WIN32
void showKeyBoard()
{
}
#else
extern void showKeyBoard();
#endif 

static inline int get_x_lparam(size_t lparam)
{
	return (short)(LOWORD(lparam));
}

static inline int get_y_lparam(size_t lparam)
{
	return (short)(HIWORD(lparam));
}

static inline int get_x_coord(IPainter* pPainter, size_t lparam)
{
	int x = get_x_lparam(lparam);

	if (pPainter->GetNeedScaleX())
	{
		x = int(x * pPainter->GetScaleFactorX());
	}

	return x;
}

static inline int get_y_coord(IPainter* pPainter, size_t lparam)
{
	int y = get_y_lparam(lparam);

	if (pPainter->GetNeedScaleY())
	{
		y = int(y * pPainter->GetScaleFactorY());
	}

	return y;
}

// 消息处理

bool Gui::ChangeFocus(IControl* get_control, IControl* lost_control)
{
	if (get_control == lost_control)
	{
		return false;
	}
	/*
	WIN32输入法先注掉
	if (get_control == NULL || GetIMEInputControl(get_control) == NULL)
	{
		// 上一个可以输入, 这次不可以输入
		if (GetIMEInputControl(lost_control) != NULL)
		{
			// 记录上次输入时的,输入法状态,是英文还是指定输入法
			HIMC imm_context = ::ImmGetContext((HWND)GetCore()->GetWindowHandle());

			if (imm_context)
			{
				m_bLastInputOpenState = ImmGetOpenStatus(imm_context);
			}
			else
			{
				m_bLastInputOpenState = FALSE;
			}
		}

		// 禁用IME
		this->DisableIME((HWND)GetCore()->GetWindowHandle());
		m_bFocusCanInput = false;
	}
	else
	{
		// 起用IME
		this->EnableIME((HWND)GetCore()->GetWindowHandle());
		m_bFocusCanInput = true;
	}
*/

	if (get_control != NULL)
	{
		if (!get_control->GetEnabled())
		{
			return false;
		}
		
		if (!get_control->GetCanFocus())
		{
			// 不能获得输入焦点
			return false;
		}
	}
	
	PERSISTID get_id;
	
	if (get_control != NULL)
	{
		get_id = get_control->GetID();
	}
	
	// 获得焦点
	// 需要先设置新的焦点ID，否则有可能因为
	// lost_control->OnLostFocus(get_id)时进行了ShowModal的操作而递归
	m_FocusID = get_id;

	PERSISTID lost_id;
	
	if (lost_control != NULL)
	{
		lost_id = lost_control->GetID();
	}
	
	// 失去焦点
	if (lost_control != NULL)
	{
		lost_control->SetFocused(false);
		lost_control->OnLostFocus(get_id);
	}

	//// 获得焦点
	//m_FocusID = get_id;
	
	if (get_control != NULL)
	{
		get_control->SetFocused(true);
		get_control->OnGetFocus(get_id);
		
		// 改变当前显示的光标
		m_CaretID = GetCaret(get_control->GetCaretName());

		// 重置光标状态
		Caret* pCaret = InnerGetCaret();

		if (pCaret)
		{
			pCaret->Reset();
		}
	}
	
	return true;
}

bool Gui::ChangeCapture(IControl* get_control, IControl* lost_control)
{
	if (get_control == lost_control)
	{
		return false;
	}
	
	PERSISTID get_id;

	if (get_control != NULL)
	{
		get_id = get_control->GetID();
	}

	PERSISTID lost_id;

	if (lost_control != NULL)
	{
		lost_id = lost_control->GetID();
	}
	
	// 失去鼠标
	if (lost_control != NULL)
	{
		lost_control->SetCapture(false);
		lost_control->OnLostCapture(get_id);
	}
	
	// 获得鼠标
	m_CaptureID = get_id;
	
	if (get_control != NULL)
	{
		get_control->SetCapture(true);
		get_control->OnGetCapture(lost_id);
	}

	RunCallback("on_capture_changed", CVarList() << get_id << lost_id);
		
	return true;
}

int Gui::OnDesignEntry(IControl* pControl)
{
	Assert(pControl != NULL);

	return RunCallback("on_design_enter", 
		CVarList() << pControl->GetID());
}

int Gui::OnDesignClick(IControl* pControl, int x, int y)
{
	Assert(pControl != NULL);
	
	return RunCallback("on_design_click", 
		CVarList() << pControl->GetID() << x << y);
}

int Gui::OnShowHint(int x, int y)
{
	if (NULL == GetModalForm())
	{
		return 0;
	}
	
	if (!m_HintID.IsNull())
	{
		return 0;
	}
	
	if (m_CaptureID.IsNull())
	{
		return 0;
	}
	
    IControl* pModalForm = GetModalForm();

	IControl* pControl = (IControl*)GetCore()->GetEntity(m_CaptureID);

    if (pControl == NULL)
    {
        return 0;
    }

    IControl* pParentControl = pControl->GetParentControl();

    // 判断m_CaptureID控件是不是pModalForm的子控件，如果是子控件才显示浮动提示，不然不用
    while(pParentControl != NULL && pParentControl != pModalForm)
    {
        pParentControl = pParentControl->GetParentControl();
    }

    if (pParentControl == NULL)
    {
        return 0;
    }

	while (pControl)
	{
		if (ShowControlHint(pControl, x, y))
		{
			return 1;
		}
		
		pControl = pControl->GetParentControl();
	}
	
	return 0;
}

int Gui::OnShowContext(IControl* pControl, int x, int y)
{
	if (NULL == GetModalForm())
	{
		return 0;
	}
	
	if (!m_ContextID.IsNull())
	{
		return 0;
	}
	
	while (pControl)
	{
		if (ShowControlContext(pControl, x, y))
		{
			return 1;
		}
		
		pControl = pControl->GetParentControl();
	}
	
	return 0;
}

int Gui::GetCursorHeight() const
{
	Cursor* cursor = InnerGetCursor();

	if (NULL == cursor)
	{
		return 0;
	}
#ifdef _WIN32
	if (cursor->GetIsWinCursor())
	{
		return GetSystemMetrics(SM_CYCURSOR) - 8;
	}
	else
#endif
	{
		return cursor->GetHeight();
	}
}

bool Gui::ShowControlHint(IControl* pControl, int x, int y)
{
	Assert(pControl != NULL);

	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	const wchar_t* hint_text = pControl->GetRealHintText();
	
    if (GetCore()->GetEntity(pControl->GetID()) == NULL)
    {
        return false;
    }

	if (WideStrEmpty(hint_text))
	{
		return false;
	}

	
	const char* hint_type = pControl->GetHintType();
	
	if (StringEmpty(hint_type))
	{
		hint_type = "Default";
	}
	
	PERSISTID hint_id = GetHint(hint_type);
	
	if (hint_id.IsNull())
	{
		return false;
	}
	
	IControl* pHint = GetControl(hint_id);

	if (NULL == pHint)
	{
		return false;
	}

	// 执行回调
	// 参数：当前焦点控件的tips文本、tips显示类型、坐标
	if(RunCallback("on_show_hint", 
		CVarList() << hint_text << hint_type << x << y) > 0)
	{
		m_HintID = hint_id;
		return true;
	}
	
	pHint->SetText(hint_text);
	pHint->SetAbsLeft(x);
	pHint->SetAbsTop(y + GetCursorHeight());

	// 控件自行设置浮动提示
	pControl->OnSetHint(pHint, x, y);

	// 防止超出可视范围
	if ((pHint->GetAbsTop() + pHint->GetHeight()) > GetHeight())
	{
		pHint->SetAbsTop(y - pHint->GetHeight());
	}

	if ((pHint->GetAbsLeft() + pHint->GetWidth()) > GetWidth())
	{
		pHint->SetAbsLeft(x - pHint->GetWidth());
	}
	
	pModalForm->AddTopLayer(pHint->GetID());
	m_HintID = hint_id;
	return true;
}

// 
bool Gui::IsControlVisible(IControl* pControl)
{
    if (NULL == pControl)
    {
        return false;
    }

	IControl* pRootParent = pControl;

	while (pControl)
	{
		pRootParent = pControl;

		if (!pRootParent->GetVisible())
		{
			return false;
		}

		pControl = pControl->GetParentControl();
	}

	if (pRootParent == m_pDesktop)
	{
		return true;
	}

	if (m_pDesktop->FindTopLayer(pRootParent->GetID()))
	{
		return true;
	}

    size_t count = m_ModalFormStack.size();

    for (size_t i = 0; i < count; ++i)
    {
        if (m_ModalFormStack[i] == pRootParent->GetID())
        {
            return true;
        }
    }

    count = m_BackControls.size();

    for (size_t i = 0; i < count; ++i)
    {
        if (m_BackControls[i] == pRootParent->GetID())
        {
            return true;
        }
    }

	return false;
}

bool Gui::ResetHintStatus(int x, int y)
{
	if (!m_HintID.IsNull())
	{
		if(RunCallback("on_hide_hint", 
			CVarList()) > 0)
		{
			// 重新开始计时
			m_HintID = PERSISTID();
			m_fHintCount = 0.0F;
			return true;
		}

		IControl* pControl = GetControl(m_CaptureID);

		if (pControl)
		{
			// 判断鼠标是否离开
			if (IsControlVisible(pControl) && pControl->InRectangle(x, y))
			{
				return false;
			}
			else
			{
                // 当前的模式窗口
		        IControl* pModalForm = GetModalForm();

                if (pModalForm != NULL)
                {
				    pModalForm->RemoveTopLayer(m_HintID);
                }
			}
		}
		else
		{
            // 当前的模式窗口
	        IControl* pModalForm = GetModalForm();

            if (pModalForm != NULL)
            {
                pModalForm->RemoveTopLayer(m_HintID);
            }
		}

		m_HintID = PERSISTID();
		
		// 重新开始计时
		m_fHintCount = 0.0F;
	}
	
	return true;
}

bool Gui::ShowControlContext(IControl* pControl, int x, int y)
{
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	const char* context = pControl->GetContext();
	
	if (StringEmpty(context))
	{
		context = "Default";
	}
	
	PERSISTID context_id = GetContext(context);
	
	if (context_id.IsNull())
	{
		return false;
	}
	
	IControl* pContext = GetControl(context_id);
	
	if (NULL == pContext)
	{
		return false;
	}
	
	pContext->SetAbsLeft(x);
	pContext->SetAbsTop(y);
	
	// 控件自行设置上下文菜单
	pControl->OnSetContext(pContext);
	
	// 防止超出可视范围
	if ((pContext->GetAbsTop() + pContext->GetHeight()) > GetHeight())
	{
		pContext->SetAbsTop(y - pContext->GetHeight());
	}
	
	if ((pContext->GetAbsLeft() + pContext->GetWidth()) > GetWidth())
	{
		pContext->SetAbsLeft(x - pContext->GetWidth());
	}
	
	pModalForm->AddTopLayer(pContext->GetID());
	
	m_ContextID = context_id;
	
	return true;
}

bool Gui::ResetContextStatus(int x, int y)
{
	if (!m_ContextID.IsNull())
	{
		IControl* pContext = GetControl(m_ContextID);
		
		if (pContext)
		{
			// 判断鼠标是否离开
			if (pContext->InRectangle(x, y))
			{
				return false;
			}
		}
		
		// 当前的模式窗口
		IControl* pModalForm = GetModalForm();
		
		if (pModalForm)
		{
			pModalForm->RemoveTopLayer(m_ContextID);
		}
	}
	
	m_ContextID = PERSISTID();
	
	return true;
}

bool Gui::ResetMainMenuStatus(IControl* capture)
{
	Form* form = (Form*)GetModalForm();

	if (NULL == form)
	{
		return false;
	}

	PERSISTID menu_id = form->GetMainMenuID();

	if (menu_id.IsNull())
	{
		return false;
	}

	MainMenu* pMainMenu = (MainMenu*)GetCore()->GetEntity(menu_id);

	if (NULL == pMainMenu)
	{
		return false;
	}

	if (pMainMenu == capture)
	{
		return false;
	}
	
	pMainMenu->ResetMenu();
	
	return true;
}


// 显示弹出式菜单
bool Gui::ShowPopupMenu(IControl* pMenu, int x, int y)
{
	//隐藏之前的菜单
	HidePopupMenu();

	if (!pMenu->GetEntInfo()->IsKindOf("Menu"))
	{
		return false;
	}

	((Menu*)pMenu)->SetVisible(true);

	pMenu->SetAbsLeft(x);
	pMenu->SetAbsTop(y);

	m_PopupMenuID = pMenu->GetID();
	
	return true;
}

// 隐藏弹出式菜单
bool Gui::HidePopupMenu()
{
	if (m_PopupMenuID.IsNull())
	{
		return false;
	}

	IControl* pMenu = (IControl*)GetCore()->GetEntity(m_PopupMenuID);

	if (pMenu == NULL)
	{
		return false;
	}

	((Menu*)pMenu)->SetVisible(false);
	m_PopupMenuID = PERSISTID();
	
	return true;
}

/*
bool Gui::GenMouseHoverMessage(int delay_ms)
{
	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = (HWND)GetWinHandle();
	tme.dwHoverTime = delay_ms;

	return (TrackMouseEvent(&tme) == TRUE);
}
*/

bool Gui::DesignControl(IControl* control, int x, int y)
{
	Assert(control != NULL);
	
	if (control->GetEntInfo()->IsKindOf("Selector"))
	{
		Selector* selector = (Selector*)control;
		IControl* bind = GetControl(selector->GetBindID());

		if (NULL == bind)
		{
			return false;
		}

		// 获得选择器之后的非附属控件
		control = bind->GetInWhat(x, y);

		if (NULL == control)
		{
			return false;
		}

		while (control->GetDelegateControl() != NULL)
		{
			control = control->GetDelegateControl();
		}
		
		if (!control->GetDesignMode())
		{
			return false;
		}
	}
	
	int offset_x = x - control->GetAbsLeft();
	int offset_y = y - control->GetAbsTop();
		
	if (OnDesignClick(control, offset_x, offset_y) != 0)
	{
		return true;
	}
	
	return (OnDesignEntry(control) != 0);
}

/*
bool Gui::MsgSetCursor(void* hwnd, size_t param1, size_t param2, int& result)
{
	unsigned int hit_test_code = LOWORD(param2);

	if (hit_test_code != HTCLIENT)
	{
		// 非客户区域
		//result = FALSE;
		//return true;
		return false;
	}
	
	Cursor* pCursor = InnerGetCursor();
	
	if (NULL == pCursor)
	{
		return false;
	}

	if (!pCursor->Show())
	{
		return false;
	}
	
	if (!pCursor->Apply())
	{
		return false;
	}
	
	// 这样返回使WINDOWS不会显示缺省的鼠标箭头
	result = TRUE;
	
	return true;
}

bool Gui::MsgLButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	if (control != NULL)
	{
		m_ClickedID = control->GetID();
		
		if (control->OnLButtonDoubleClick(x, y))
		{
			return true;
		}
	}

	return false;
}

bool Gui::MsgRButtonDown(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
	// 捕获鼠标
	SetCapture((HWND)GetWinHandle());
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	// 当前点击的控件
	//IControl* control = GetControl(m_ClickedID);

	ChangeFocus(NULL, GetControl(m_FocusID));
	
    ResetMainMenuStatus(control);

    bool ret = false;

	if (control != NULL)
	{
		if (control->OnRButtonDown(x, y))
		{
			ret =  true;
		}
	}

    HidePopupMenu();

	return ret;
}

bool Gui::MsgRButtonUp(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
	// 释放鼠标
	ReleaseCapture();

	// 释放手栏
	//GameHand* pGameHand = InnerGetGameHand();
	
	//if (pGameHand)
	//{
	//	pGameHand->ClearHand();
	//}

	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	ResetContextStatus(x, y);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	if (control != NULL)
	{
		OnShowContext(control, x, y);
		
		if (control->OnRButtonUp(x, y))
		{
			return true;
		}
	}

	return false;
}

bool Gui::MsgRButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	if (control != NULL)
	{
		if (control->OnRButtonDoubleClick(x, y))
		{
			return true;
		}
	}

	return false;
}

bool Gui::MsgMouseWheel(void* hwnd, size_t param1, size_t param2, int& result)
{
	if (!m_bVisible)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	// 屏幕坐标
	POINT point;
	
	point.x = get_x_lparam(param2);
	point.y = get_y_lparam(param2);
	
	ScreenToClient((HWND)GetWinHandle(), &point);
	
	int x = point.x;
	int y = point.y;

	if (m_pPainter->GetNeedScaleX())
	{
		x = int(x * m_pPainter->GetScaleFactorX());
	}

	if (m_pPainter->GetNeedScaleY())
	{
		y = int(y * m_pPainter->GetScaleFactorY());
	}
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	int keys = LOWORD(param1);
	int delta = short(HIWORD(param1));
	
	delta = delta / WHEEL_DELTA;
	
	IControl* focus = GetFocusControl();
	
	while (focus != NULL)
	{
		if (focus->InRectangle(x, y))
		{
			if (focus->OnMouseWheel(x, y, delta))
			{
				return true;
			}
		}
		
		focus = focus->GetParentControl();
	}

	return false;
}

bool Gui::MsgMouseHover(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	if (control != NULL)
	{
		if (param1 & MK_LBUTTON)
		{
			if (control->OnLButtonHover(x, y))
			{
				return true;
			}
		}
		else if (param1 & MK_RBUTTON)
		{
			if (control->OnRButtonHover(x, y))
			{
				return true;
			}
		}
	}

	return false;
}

bool Gui::MsgSysKeyDown(void* hwnd, size_t param1, size_t param2, int& result)
{
	char key[32];

	if (GetKeyNameText((LONG)param2, key, sizeof(key)) != 0)
	{
		bool shift = GuiUtil_ShiftPressed();
		bool ctrl = GuiUtil_ControlPressed();
		
		if (GuiUtil_RunCallback(this, "on_sys_key_down", 
			CVarList() << key << shift << ctrl))
		{
			return true;
		}
	}

	return false;
}

// 禁用IME,并清理输入内容
void Gui::DisableIME(HWND hwnd) 
{
	// A renderer process have moved its input focus to a password input
	// when there is an ongoing composition, e.g. a user has clicked a
	// mouse button and selected a password input while composing a text.
	// For this case, we have to complete the ongoing composition and
	// clean up the resources attached to this object BEFORE DISABLING THE IME.

	HIMC imm_context = ::ImmGetContext(hwnd);

	if (imm_context)
	{
		ImmNotifyIME(imm_context, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
		//ImmReleaseContext(hwnd, imm_context);
		ImmSetOpenStatus(imm_context, false);
	}

	//ImmAssociateContextEx(hwnd, NULL, IACE_IGNORENOCONTEXT);
}

// 启用IME
void Gui::EnableIME(HWND hwnd) 
{
	// Load the default IME context.
	// NOTE(hbono)
	// IMM ignores this call if the IME context is loaded. Therefore, we do
	// not have to check whether or not the IME context is loaded.
	HIMC imm_context = ImmGetContext(hwnd);

	if (imm_context)
	{
		ImmSetOpenStatus(imm_context, m_bLastInputOpenState);
	}
	else
	{
		ImmAssociateContextEx(hwnd, NULL, IACE_DEFAULT);

		imm_context = ImmGetContext(hwnd);

		if (imm_context)
		{
			ImmSetOpenStatus(imm_context, m_bLastInputOpenState);
		}
	}

	// 下面代码修正刚进游戏时,有时输入不能必须切出游戏再切入才能输入的BUG
	static bool first = true;

	if (first)
	{
		first = false;
		SendMessage(hwnd, WM_ACTIVATE, WA_INACTIVE, (LPARAM)hwnd);
		SendMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, (LPARAM)hwnd);
		// 默认先关闭输入法
		ImmSetOpenStatus(imm_context, false);
	}
}

IControl * Gui::GetIMEInputControl(IControl* pControl)
{
    if (pControl != NULL)
    {
        if (stricmp(pControl->GetEntInfo()->GetEntityName(), "Edit") == 0 ||
			stricmp(pControl->GetEntInfo()->GetEntityName(), "RichInputBox") == 0 ||
			stricmp(pControl->GetEntInfo()->GetEntityName(), "RichEdit") == 0)
        {
            return pControl;
        }
    }

    return NULL;
}

// 输入法改变回调
bool Gui::MsgInputLanguage(void* hwnd, size_t param1, size_t param2, 
	int& result)
{
    // param2为键盘布局
    m_hkl = (HKL)param2;
    m_ImeProp = ImmGetProperty(m_hkl, IGP_PROPERTY);
#ifdef UNICODE
    m_ImeProp = m_ImeProp | IME_PROP_UNICODE;
#endif
    // param1 当前字符集信息
    // 通过字符集获取代码页
    TranslateCharsetInfo((DWORD*)param1, &m_csInfo, TCI_SRCCHARSET);
    m_CurCP = m_csInfo.ciACP;

    // 已经切换了输入法，与原来的数据再无关系
    //asbSubmit();

    return false;
}

bool Gui::MsgIMECompositon(void* hwnd, size_t param1, size_t param2, 
	int& result)
{
    IControl* pInputEdit = GetIMEInputControl(GetFocusControl());

    if (pInputEdit != NULL)
    {
        POINT caret_pos = pInputEdit->GetCaretPos();
        COMPOSITIONFORM cp;

        cp.dwStyle = CFS_FORCE_POSITION;
        cp.ptCurrentPos.x = caret_pos.x;
        cp.ptCurrentPos.y = caret_pos.y + 5;

        HIMC hIMC = ImmGetContext((HWND)hwnd);

		// 设置组词窗口
        ImmSetCompositionWindow(hIMC, &cp);

        CANDIDATEFORM immdataform;
   
        immdataform.dwIndex = 0;
        immdataform.dwStyle = CFS_CANDIDATEPOS;
        immdataform.ptCurrentPos.x = cp.ptCurrentPos.x + 100;
        immdataform.ptCurrentPos.y = cp.ptCurrentPos.y;

		// 设置当前输入显示列表
        ImmSetCandidateWindow(hIMC, &immdataform);

		// 设置输入法状态条
        //ImmSetStatusWindowPos(hIMC, &caret_pos);

        // 释放输入上下文
        //ImmReleaseContext((HWND)hwnd, hIMC);
    }

    if (param2 & GCS_RESULTSTR)
    {
        byte* lpBuf = NULL;
		size_t buf_mem_size = 0;
        LONG cchBuf = 0;
        HIMC hIMC = ImmGetContext((HWND)GetCore()->GetWindowHandle());

        if (hIMC != NULL)	
        {
            // 如果当前支持UNICODE
            if (m_ImeProp & IME_PROP_UNICODE)	
            {
                // 取得文本bytes数量
                cchBuf = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);

                // 有内容
                if (cchBuf > 0)	
                {
                    // 创建缓冲区存放数据
                    lpBuf = (byte*)CORE_ALLOC(cchBuf);
					buf_mem_size = cchBuf;

                    if (lpBuf != NULL)	
                    {
                        // 获取输入内容到缓冲区
                        cchBuf = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, 
							lpBuf, cchBuf);
                        cchBuf = cchBuf / sizeof(wchar_t);
                    }
                    else	
                    {
                        cchBuf = 0;
                    }
                }
            }
            else	// 如果不是UNICODE情况
            {
                LPBYTE lpStr = NULL;
				size_t str_mem_size = 0;
                LONG cchStr = 0;

                // 取得文本数据
                cchStr = ImmGetCompositionStringA(hIMC, GCS_RESULTSTR, NULL, 0);

                if (cchStr > 0)	
                {
                    lpStr = (byte*)CORE_ALLOC(cchStr);
					str_mem_size = cchStr;

                    if (lpStr != NULL)	
                    {
                        cchStr = ImmGetCompositionStringA(hIMC, GCS_RESULTSTR, 
							lpStr, cchStr);
                    }
                    else	
                    {
                        cchStr = 0;
                    }
                }

                // 转成Unicode
                if (cchStr > 0)	
                {
                    cchBuf = MultiByteToWideChar(CP_ACP, 0, (LPSTR)lpStr, 
						cchStr, NULL, 0);

                    if (cchBuf > 0)	
                    {
						buf_mem_size = sizeof(wchar_t) * cchBuf;
                        lpBuf = (byte*)CORE_ALLOC(buf_mem_size);

                        if (lpStr != NULL)	
                        {
                            cchBuf = MultiByteToWideChar(CP_ACP, 0, 
								(LPSTR)lpStr, cchStr, (LPWSTR)lpBuf, cchBuf);
                        }
                        else	
                        {
                            cchBuf = 0;
                        }
                    }
                }

                // 释放
                if (lpStr != NULL)	
                {
                    CORE_FREE(lpStr, str_mem_size);
                    lpStr = NULL;
                }

            }

            //ImmReleaseContext((HWND)GetCore()->GetWindowHandle(), hIMC);
        }

        if (cchBuf > 0)	
		{
            for (LONG i = 0; i < cchBuf; ++i)
            {
                wchar_t* pwstring = (wchar_t*)lpBuf;

                SendCharToControl(pwstring[i]);
            }
        }

        if (lpBuf != NULL) 
        {
            CORE_FREE(lpBuf, buf_mem_size);
            lpBuf = NULL;
        }

        if (cchBuf > 0)	
        {
            result = TRUE;
            return true;
        }

    }
    
    return false;
}

bool Gui::MsgKillFocus(void* hwnd, size_t param1, size_t param2, int& result)
{
	// 清除输入焦点
	//ChangeFocus(NULL, GetControl(m_FocusID));
	
	return false;
}

bool Gui::MsgSize(void* hwnd, size_t param1, size_t param2, int& result)
{
	if (SIZE_RESTORED == param1)
	{
		GuiUtil_RunCallback(this, "on_restore", CVarList());
	}
	else if (SIZE_MINIMIZED == param1)
	{
		GuiUtil_RunCallback(this, "on_minimize", CVarList());
	}
	else if (SIZE_MAXIMIZED == param1)
	{
		GuiUtil_RunCallback(this, "on_maximize", CVarList());
	}

	// 新的尺寸
	int width = unsigned int(LOWORD(param2));
	int height = unsigned int(HIWORD(param2));
	
	if ((width == 0) || (height == 0))
	{
		// 窗口被最小化
		return false;
	}
	
    if (width != m_pPainter->GetWinWidth() ||
        height != m_pPainter->GetWinHeight())
    {
        // 更新显示窗口尺寸
	    m_pPainter->UpdateWindowSize();
    }

	if ((width == m_pPainter->GetDeviceWidth())
		&& (height == m_pPainter->GetDeviceHeight()))
	{
		// 尺寸未变化
		return false;
	}

	RECT rect;

	GetClientRect((HWND)GetWinHandle(), &rect);

	int cur_win_width = rect.right - rect.left;
	int cur_win_height = rect.bottom - rect.top;

	if ((width != cur_win_width) || (height != cur_win_height))
	{
		// 可能已经是过时的消息
		return false;
	}

	GuiUtil_RunCallback(this, "on_size", CVarList() << width << height);

	return false;
}

bool Gui::MsgSizing(void* hwnd, size_t param1, size_t param2, int& result)
{
	//RECT* pRect = (RECT*)param2;
	
	return false;
}

bool Gui::MsgMove(void* hwnd, size_t param1, size_t param2, int& result)
{
	// 新的尺寸
	int left = get_x_lparam(param2);
	int top = get_y_lparam(param2);
    RECT rect;

    GetWindowRect((HWND)hwnd, &rect);

    WINDOWINFO info;

    GetWindowInfo((HWND)hwnd, &info);

    bool minimun = ((info.dwStyle & WS_MINIMIZE) != 0);
    bool maximun = ((info.dwStyle & WS_MAXIMIZE) != 0);
    bool need_reset = false;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int win_left = rect.left;

    if (rect.left <= 0)
    {
        need_reset = true;
        win_left = 0;
    }

    int win_top = rect.top;

    if (win_top <= 0)
    {
        need_reset = true;
        win_top = 0;
    }

    if (need_reset && !(maximun || minimun))
    {
        MoveWindow((HWND)hwnd, win_left, win_top, width, height, true);
    }
    else
    {
	    GuiUtil_RunCallback(this, "on_move", CVarList() << left << top);
    }

	return false;
}

bool Gui::MsgMoving(void* hwnd, size_t param1, size_t param2, int& result)
{
	RECT* pRect = (RECT*)param2;
	int left = pRect->left;
	int top = pRect->top;
	
	GuiUtil_RunCallback(this, "on_moving", CVarList() << left << top);

	return false;
}

bool Gui::MsgClose(void* hwnd, size_t param1, size_t param2, int& result)
{
	if (GuiUtil_RunCallback(this, "on_close", CVarList()))
	{
		result = 0;
		return true;
	}

	return false;
}

bool Gui::MsgActivate(void* hwnd, size_t param1, size_t param2, int& result)
{
	if ((param1 == WA_ACTIVE) || (param1 == WA_CLICKACTIVE))
	{
		GuiUtil_RunCallback(this, "on_active", CVarList());

		if (!m_bFocusCanInput)
		{
			DisableIME((HWND)hwnd);
		}

	}
	else if (param1 == WA_INACTIVE)
	{
		GuiUtil_RunCallback(this, "on_unactive", CVarList());
	}

	// 恢复鼠标
	//Cursor* pCursor = InnerGetCursor();

	//if (pCursor)
	//{
	//	pCursor->Reset();
	//}

	return false;
}

// 获取当前输入法设置
bool Gui::LoadCurIMESetting()
{
    HKL hklList[2];
    UINT hkl_count = GetKeyboardLayoutList(2, hklList);

    if (hkl_count > 0)
    {
        // 当前键盘布局
        m_hkl = hklList[0];
        // 当前输入法输性
        m_ImeProp = ImmGetProperty(m_hkl, IGP_PROPERTY);
#ifdef UNICODE
        m_ImeProp = m_ImeProp | IME_PROP_UNICODE;
#endif
    }
    else
    {
        m_hkl = 0;
        m_ImeProp = 0;
    }

    // 获取当前字符集
    HWND hwnd = (HWND)GetCore()->GetWindowHandle();
    HDC hdc = GetDC(hwnd);
    //int charset = GetTextCharset(hdc);
	size_t charset = GetTextCharset(hdc);

    // 通过字符集获取代码页
    TranslateCharsetInfo((DWORD*)charset, &m_csInfo, TCI_SRCCHARSET);
    m_CurCP = m_csInfo.ciACP;       // 当前代码页

    return true;
}

bool Gui::MsgProc(void* hwnd, unsigned int msg, size_t param1, size_t param2, 
	int& result)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		return MsgMouseMove(hwnd, param1, param2, result);
	case WM_SETCURSOR:
		return MsgSetCursor(hwnd, param1, param2, result);
	case WM_LBUTTONDOWN:
		return MsgLButtonDown(hwnd, param1, param2, result);
	case WM_LBUTTONUP:
		return MsgLButtonUp(hwnd, param1, param2, result);
	case WM_LBUTTONDBLCLK:
		return MsgLButtonDblclk(hwnd, param1, param2, result);
	case WM_RBUTTONDOWN:
		return MsgRButtonDown(hwnd, param1, param2, result);
	case WM_RBUTTONUP:
		return MsgRButtonUp(hwnd, param1, param2, result);
	case WM_RBUTTONDBLCLK:
		return MsgRButtonDblclk(hwnd, param1, param2, result);
	case WM_MOUSEWHEEL:
		return MsgMouseWheel(hwnd, param1, param2, result);
	case WM_MOUSEHOVER:
		return MsgMouseHover(hwnd, param1, param2, result);
	case WM_SYSKEYDOWN:
		return MsgSysKeyDown(hwnd, param1, param2, result);
	case WM_KEYDOWN:
		return MsgKeyDown(hwnd, param1, param2, result);
	case WM_KEYUP:
		return MsgKeyUp(hwnd, param1, param2, result);
    //多国语言输入支持
	case WM_CHAR:
		return MsgChar(hwnd, param1, param2, result);
    case WM_INPUTLANGCHANGE: //当切换键盘布局时收到
        return MsgInputLanguage(hwnd, param1, param2, result);
    case WM_IME_COMPOSITION:
        return MsgIMECompositon(hwnd, param1, param2, result);
    //多国语言输入支持
	case WM_KILLFOCUS:
		return MsgKillFocus(hwnd, param1, param2, result);
	case WM_SIZE:
		return MsgSize(hwnd, param1, param2, result);
	case WM_SIZING:
		return MsgSizing(hwnd, param1, param2, result);
	case WM_MOVE:
		return MsgMove(hwnd, param1, param2, result);
	case WM_MOVING:
		return MsgMoving(hwnd, param1, param2, result);
	case WM_CLOSE:
		return MsgClose(hwnd, param1, param2, result);
	case WM_ACTIVATE:
		return MsgActivate(hwnd, param1, param2, result);
	case WM_IME_SETCONTEXT:
	case WM_IME_NOTIFY:
		// 当前输入焦点不能输入则不处理输入法消息
		if (!m_bFocusCanInput)
		{
			return true;
		}
		break;
	default:
		Assert(0);
		break;
	}
	
	return false;
}
*/

bool Gui::MsgMouseMove(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}

	// 如果忽略鼠标移动
	if (m_bAbandomMouseMove)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();

	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	// 隐藏浮动提示
	ResetHintStatus(x, y);
	
	// 当前点击的控件
	IControl* clicked = GetControl(m_ClickedID);
	
	if (clicked != NULL)
	{
		// 拖动
		clicked->OnDragMove(x, y);
		
        return true;
	}
	
	// 在处理拖动之后判断
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	// 设置光标形状
	if (control != NULL)
	{
		control->OnSetCursor(x, y);
	}
	else
	{
		SetCurrentCursor(m_strCursorName.c_str());
	}
	
	// 鼠标焦点改变
	ChangeCapture(control, GetControl(m_CaptureID));
	
	if (control != NULL)
	{
		if (control->OnMouseMove(x, y))
		{
			return true;
		}
	}

	return false;
}

bool Gui::MsgLButtonDown(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
    // 隐藏浮动
    ResetHintStatus(-100, -100);

	// 捕获鼠标
#ifdef _WIN32
	//SetCapture((HWND)GetWinHandle());
#endif
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	//ResetMainMenuStatus(control);
	//HidePopupMenu();
	
	if (control != NULL)
	{
		IControl* master = control;
		
		// 非附属控件
		while (master->GetDelegateControl() != NULL)
		{
			master = master->GetDelegateControl();
		}
		
		// 设计状态控件
		if (master->GetDesignMode())
		{
			if (DesignControl(master, x, y))
			{
				return true;
			}
		}
	}
	
	while (control != NULL)
	{
		// 控件是否可以被选择
		if (control->GetCanSelect())
		{
			break;
		}
		
		control = control->GetParentControl();
	}
	
	if (control != NULL)
	{
		IControl* can_focus = control;
		
		if (!can_focus->GetCanFocus())
		{
			// 可以获得输入焦点的控件
			while (can_focus->GetDelegateControl() != NULL)
			{
				can_focus = can_focus->GetDelegateControl();
				
				if (can_focus->GetCanFocus())
				{
					break;
				}
			}
		}
		
		if (can_focus->GetCanFocus())
		{
			// 输入焦点改变
			ChangeFocus(can_focus, GetControl(m_FocusID));
           
            if(can_focus->GetCanIMEFocus())
            {
                showKeyBoard();
            }
		}

		m_ClickedID = control->GetID();
		
		if (control->OnLButtonDown(x, y))
		{
			return true;
		}
	}
	else
	{
		ChangeFocus(NULL, GetControl(m_FocusID));
	}

	return false;
}

bool Gui::MsgLButtonUp(void* hwnd, size_t param1, size_t param2, int& result)
{
    m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
    // 隐藏浮动
    ResetHintStatus(-100, -100);
	// 释放鼠标
#ifdef _WIN32
	ReleaseCapture();
#endif
	
	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	ResetContextStatus(x, y);
	
	IControl* clicked = GetControl(m_ClickedID);
	IControl* control = pModalForm->GetInWhat(x, y);
    ResetMainMenuStatus(clicked);

    bool ret = false;

	if (clicked != NULL)
	{
		m_ClickedID = PERSISTID();
		
		// 模拟一个鼠标移动消息
		int res;

		MsgMouseMove(hwnd, param1, param2, res);

		if (NULL != control)
		{
			control->OnDropIn(x, y);
		}
		
		// 结束拖动
		clicked->OnDragLeave(x, y);

		if (clicked->OnLButtonUp(x, y))
		{
			ret =  true;
		}
	}
	
    HidePopupMenu();

    if (ret)
    {
        return true;
    }

	// 必须在处理拖动之后
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}

	return false;
}

bool Gui::SendCharToControl(wchar_t ch)
{
    IControl* control = GetFocusControl();

    while (control != NULL)
    {
        if (control->OnChar(ch))
        {
            return true;
        }

        // 交由宿主控件处理
        control = control->GetDelegateControl();
    }

    return false;
}

bool Gui::MsgKeyDown(void* hwnd, size_t param1, size_t param2, int& result)
{
	char key[32];
	bool shift = GuiUtil_ShiftPressed();
	bool ctrl = GuiUtil_ControlPressed();
#ifdef _WIN32
	bool get_key_name = GetKeyNameText((LONG)param2, key, sizeof(key)) != 0;

	if (get_key_name)
	{
        if (stricmp(key, "Shift") == 0)
        {
            return false;
        }

		RunCallback("on_whatever_key_down", 
			CVarList() << key << shift << ctrl);
	}
#endif

	// 当前的模式窗口
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	IControl* control = GetFocusControl();

	if (!m_bFocusCanInput)
	{
		//DisableIME((HWND)hwnd);
	}

	if (control != NULL)
	{
		if (((param1 == CGuiInput::KEY_TAB)	&& control->GetAllowTab())
			|| ((param1 == CGuiInput::KEY_RETURN) 
			&& control->GetAllowReturn()))
		{
			if (control->OnKeyDown((unsigned int)param1))
			{
				return true;
			}
		}
	}

	// 按下TAB键
	if (param1 == CGuiInput::KEY_TAB)
	{
		Form* form = (Form*)pModalForm;
		// 将输入焦点移至下一个控件
		IControl* focus = GetControl(m_FocusID);
		
		if (focus != NULL)
		{
			if (focus->GetRootControl() != pModalForm)
			{
				focus = NULL;
			}
		}
		
		IControl* next_focus = form->GetNextFocus(focus);
		
		if (next_focus != NULL)
		{
			ChangeFocus(next_focus, GetControl(m_FocusID));
		}
	}

	control = GetFocusControl();
	
	while (control != NULL)
	{
        if (param1 == CGuiInput::KEY_RETURN)
        {
            if (!control->GetDisableEnter() 
				&& control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }

        }
		else if(control->OnKeyDown((unsigned int)param1))
		{
			return true;
		}

		// 交由宿主控件处理
		control = control->GetDelegateControl();
	}
	
    // 按下回车
    if (param1 == CGuiInput::KEY_RETURN)
    {
        Form* form = (Form*)pModalForm;

        // 缺省执行动作对象
        IControl* def_control = GetControl(form->GetDefaultID());

        if (def_control != NULL)
        {
            //没有禁用对enter的响应
            if (!def_control->GetDisableEnter() 
				&& def_control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }
        }
    }

	// 调试键CTRL+F1-F12
	/*
	if ((!GetCore()->GetRetailVersion()) && GuiUtil_ControlPressed())
	{
		if ((param1 >= CGuiInput::KEY_F1) && (param1 <= CGuiInput::KEY_F12))
		{
			char key[3];
			
			key[0] = 'F';
			key[1] = '1' + ((char)param1 - CGuiInput::KEY_F1);
			key[2] = 0;
			
			if (RunCallback("on_func_key", 
				CVarList() << key))
			{
				return true;
			}
		}
	}*/
#ifdef _WIN32
	if (get_key_name)
	{
		if (RunCallback("on_key_down", 
			CVarList() << key << shift << ctrl))
		{
			return true;
		}
	}
#endif

	return false;
}

bool Gui::MsgKeyUp(void* hwnd, size_t param1, size_t param2, int& result)
{
	IControl* control = GetFocusControl();
	
	while (control != NULL)
	{
		if (control->OnKeyUp((unsigned int)param1))
		{
			return true;
		}
		
		// 交由宿主控件处理
		control = control->GetDelegateControl();
	}
	
#ifdef _WIN32
	
	char key[32];
	if (GetKeyNameText((LONG)param2, key, sizeof(key)) != 0)
	{
		bool shift = GuiUtil_ShiftPressed();
		bool ctrl = GuiUtil_ControlPressed();
		
		if (RunCallback("on_key_up", 
			CVarList() << key << shift << ctrl))
		{
			return true;
		}
	}

#endif
	
	return false;
}

bool Gui::MsgChar(void* hwnd, size_t param1, size_t param2, int& result)
{
	wchar_t wcschar = param1;

	SendCharToControl(wcschar);
	return false;
}

bool Gui::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		return MsgMouseMove(0, param1, param2, result);
	case WM_LBUTTONDOWN:
		return MsgLButtonDown(0, param1, param2, result);
	case WM_LBUTTONUP:
		return MsgLButtonUp(0, param1, param2, result);
		//输入相关
	case WM_KEYDOWN:
		return MsgKeyDown(0, param1, param2, result);
	case WM_KEYUP:
		return MsgKeyUp(0, param1, param2, result);
    //多国语言输入支持
	case WM_CHAR:
		return MsgChar(0, param1, param2, result);
		/*
	case WM_LBUTTONDBLCLK:
		return MsgLButtonDblclk(hwnd, param1, param2, result);
	case WM_RBUTTONDOWN:
		return MsgRButtonDown(hwnd, param1, param2, result);
	case WM_RBUTTONUP:
		return MsgRButtonUp(hwnd, param1, param2, result);
	case WM_RBUTTONDBLCLK:
		return MsgRButtonDblclk(hwnd, param1, param2, result);
	case WM_MOUSEWHEEL:
		return MsgMouseWheel(hwnd, param1, param2, result);
	case WM_MOUSEHOVER:
		return MsgMouseHover(hwnd, param1, param2, result);
		
	case WM_SYSKEYDOWN:
		return MsgSysKeyDown(hwnd, param1, param2, result);
	case WM_KEYDOWN:
		return MsgKeyDown(hwnd, param1, param2, result);
	case WM_KEYUP:
		return MsgKeyUp(hwnd, param1, param2, result);
    //多国语言输入支持
	case WM_CHAR:
		return MsgChar(hwnd, param1, param2, result);
    case WM_INPUTLANGCHANGE: //当切换键盘布局时收到
        return MsgInputLanguage(hwnd, param1, param2, result);
    case WM_IME_COMPOSITION:
        return MsgIMECompositon(hwnd, param1, param2, result);
    //多国语言输入支持
	case WM_KILLFOCUS:
		return MsgKillFocus(hwnd, param1, param2, result);
	case WM_SIZE:
		return MsgSize(hwnd, param1, param2, result);
	case WM_SIZING:
		return MsgSizing(hwnd, param1, param2, result);
	case WM_MOVE:
		return MsgMove(hwnd, param1, param2, result);
	case WM_MOVING:
		return MsgMoving(hwnd, param1, param2, result);
	case WM_CLOSE:
		return MsgClose(hwnd, param1, param2, result);
	case WM_ACTIVATE:
		return MsgActivate(hwnd, param1, param2, result);
	case WM_IME_SETCONTEXT:
	case WM_IME_NOTIFY:
		// 当前输入焦点不能输入则不处理输入法消息
		if (!m_bFocusCanInput)
		{
			return true;
		}
		break;*/
	default:
	//	Assert(0);
		break;
	}
	
	return false;
}


bool Gui::MsgLButtonDownToControl(IControl* pControl, size_t param1, size_t param2, int& result)
{
	long m_nMouseClientX = get_x_lparam(param2);
    long m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
    // 隐藏浮动
    ResetHintStatus(-100, -100);

	// 捕获鼠标
#ifdef _WIN32
	//SetCapture((HWND)GetWinHandle());
#endif
	
	// 当前的模式窗口
	IControl* pModalForm = pControl;
	
	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	//ResetMainMenuStatus(control);
	//HidePopupMenu();
	
	if (control != NULL)
	{
		IControl* master = control;
		
		// 非附属控件
		while (master->GetDelegateControl() != NULL)
		{
			master = master->GetDelegateControl();
		}
		
		// 设计状态控件
		if (master->GetDesignMode())
		{
			if (DesignControl(master, x, y))
			{
				return true;
			}
		}
	}
	
	while (control != NULL)
	{
		// 控件是否可以被选择
		if (control->GetCanSelect())
		{
			break;
		}
		
		control = control->GetParentControl();
	}
	
	if (control != NULL)
	{
		IControl* can_focus = control;
		
		if (!can_focus->GetCanFocus())
		{
			// 可以获得输入焦点的控件
			while (can_focus->GetDelegateControl() != NULL)
			{
				can_focus = can_focus->GetDelegateControl();
				
				if (can_focus->GetCanFocus())
				{
					break;
				}
			}
		}
		
		if (can_focus->GetCanFocus())
		{
			// 输入焦点改变
			ChangeFocus(can_focus, GetControl(m_FocusID));
           
            if(can_focus->GetCanIMEFocus())
            {
                showKeyBoard();
            }
		}

		m_ClickedID = control->GetID();
		
		if (control->OnLButtonDown(x, y))
		{
			return true;
		}
	}
	else
	{
		ChangeFocus(NULL, GetControl(m_FocusID));
	}

	return false;
}

bool Gui::MsgMouseMoveToControl(IControl* pControl, size_t param1, size_t param2, int& result)
{
	m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}

	// 如果忽略鼠标移动
	if (m_bAbandomMouseMove)
	{
		return false;
	}
	
	// 当前的模式窗口
	IControl* pModalForm = pControl;

	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	// 隐藏浮动提示
	ResetHintStatus(x, y);
	
	// 当前点击的控件
	IControl* clicked = GetControl(m_ClickedID);
	
	if (clicked != NULL)
	{
		// 拖动
		clicked->OnDragMove(x, y);
		
        return true;
	}
	
	// 在处理拖动之后判断
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	// 设置光标形状
	if (control != NULL)
	{
		control->OnSetCursor(x, y);
	}
	else
	{
		SetCurrentCursor(m_strCursorName.c_str());
	}
	
	// 鼠标焦点改变
	ChangeCapture(control, GetControl(m_CaptureID));
	
	if (control != NULL)
	{
		if (control->OnMouseMove(x, y))
		{
			return true;
		}
	}

	return false;
}

bool Gui::MsgLButtonUpToControl(IControl* pControl, size_t param1, size_t param2, int& result)
{
	m_nMouseClientX = get_x_lparam(param2);
    m_nMouseClientY = get_y_lparam(param2);

	if (!m_bVisible)
	{
		return false;
	}
	
    // 隐藏浮动
    ResetHintStatus(-100, -100);
	// 释放鼠标
#ifdef _WIN32
	ReleaseCapture();
#endif
	
	// 当前的模式窗口
	IControl* pModalForm =pControl;
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	ResetContextStatus(x, y);
	
	IControl* clicked = GetControl(m_ClickedID);
	IControl* control = pModalForm->GetInWhat(x, y);
    ResetMainMenuStatus(clicked);

    bool ret = false;

	if (clicked != NULL)
	{
		m_ClickedID = PERSISTID();
		
		// 模拟一个鼠标移动消息
		int res;

		MsgMouseMove(0, param1, param2, res);

		if (NULL != control)
		{
			control->OnDropIn(x, y);
		}
		
		// 结束拖动
		clicked->OnDragLeave(x, y);

		if (clicked->OnLButtonUp(x, y))
		{
			ret =  true;
		}
	}
	
    HidePopupMenu();

    if (ret)
    {
        return true;
    }

	// 必须在处理拖动之后
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}

	return false;
}

bool Gui::MsgKeyDownToControl(IControl* pControl, size_t param1, size_t param2, int& result)
{
	char key[32];
	bool shift = GuiUtil_ShiftPressed();
	bool ctrl = GuiUtil_ControlPressed();
#ifdef _WIN32
	bool get_key_name = GetKeyNameText((LONG)param2, key, sizeof(key)) != 0;

	if (get_key_name)
	{
        if (stricmp(key, "Shift") == 0)
        {
            return false;
        }

		RunCallback("on_whatever_key_down", 
			CVarList() << key << shift << ctrl);
	}
#endif

	// 当前的模式窗口
	IControl* pModalForm = pControl;
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	IControl* control = pModalForm;

	if (!m_bFocusCanInput)
	{
		//DisableIME((HWND)hwnd);
	}

	if (control != NULL)
	{
		if (((param1 == CGuiInput::KEY_TAB)	&& control->GetAllowTab())
			|| ((param1 == CGuiInput::KEY_RETURN) 
			&& control->GetAllowReturn()))
		{
			if (control->OnKeyDown((unsigned int)param1))
			{
				return true;
			}
		}
	}

	// 按下TAB键
	if (param1 == CGuiInput::KEY_TAB)
	{
		Form* form = (Form*)pModalForm;
		// 将输入焦点移至下一个控件
		IControl* focus = GetControl(m_FocusID);
		
		if (focus != NULL)
		{
			if (focus->GetRootControl() != pModalForm)
			{
				focus = NULL;
			}
		}
		
		IControl* next_focus = form->GetNextFocus(focus);
		
		if (next_focus != NULL)
		{
			ChangeFocus(next_focus, GetControl(m_FocusID));
		}
	}

	control = GetFocusControl();
	
	while (control != NULL)
	{
        if (param1 == CGuiInput::KEY_RETURN)
        {
            if (!control->GetDisableEnter() 
				&& control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }

        }
		else if(control->OnKeyDown((unsigned int)param1))
		{
			return true;
		}

		// 交由宿主控件处理
		control = control->GetDelegateControl();
	}
	
    // 按下回车
    if (param1 == CGuiInput::KEY_RETURN)
    {
        Form* form = (Form*)pModalForm;

        // 缺省执行动作对象
        IControl* def_control = GetControl(form->GetDefaultID());

        if (def_control != NULL)
        {
            //没有禁用对enter的响应
            if (!def_control->GetDisableEnter() 
				&& def_control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }
        }
    }

	// 调试键CTRL+F1-F12
	/*
	if ((!GetCore()->GetRetailVersion()) && GuiUtil_ControlPressed())
	{
		if ((param1 >= CGuiInput::KEY_F1) && (param1 <= CGuiInput::KEY_F12))
		{
			char key[3];
			
			key[0] = 'F';
			key[1] = '1' + ((char)param1 - CGuiInput::KEY_F1);
			key[2] = 0;
			
			if (RunCallback("on_func_key", 
				CVarList() << key))
			{
				return true;
			}
		}
	}*/
#ifdef _WIN32
	if (get_key_name)
	{
		if (RunCallback("on_key_down", 
			CVarList() << key << shift << ctrl))
		{
			return true;
		}
	}
#endif

	return false;
}
