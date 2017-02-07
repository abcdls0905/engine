//--------------------------------------------------------------------
// �ļ���:		gui_msg_proc.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ��Ϣ����

bool Gui::ChangeFocus(IControl* get_control, IControl* lost_control)
{
	if (get_control == lost_control)
	{
		return false;
	}
	/*
	WIN32���뷨��ע��
	if (get_control == NULL || GetIMEInputControl(get_control) == NULL)
	{
		// ��һ����������, ��β���������
		if (GetIMEInputControl(lost_control) != NULL)
		{
			// ��¼�ϴ�����ʱ��,���뷨״̬,��Ӣ�Ļ���ָ�����뷨
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

		// ����IME
		this->DisableIME((HWND)GetCore()->GetWindowHandle());
		m_bFocusCanInput = false;
	}
	else
	{
		// ����IME
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
			// ���ܻ�����뽹��
			return false;
		}
	}
	
	PERSISTID get_id;
	
	if (get_control != NULL)
	{
		get_id = get_control->GetID();
	}
	
	// ��ý���
	// ��Ҫ�������µĽ���ID�������п�����Ϊ
	// lost_control->OnLostFocus(get_id)ʱ������ShowModal�Ĳ������ݹ�
	m_FocusID = get_id;

	PERSISTID lost_id;
	
	if (lost_control != NULL)
	{
		lost_id = lost_control->GetID();
	}
	
	// ʧȥ����
	if (lost_control != NULL)
	{
		lost_control->SetFocused(false);
		lost_control->OnLostFocus(get_id);
	}

	//// ��ý���
	//m_FocusID = get_id;
	
	if (get_control != NULL)
	{
		get_control->SetFocused(true);
		get_control->OnGetFocus(get_id);
		
		// �ı䵱ǰ��ʾ�Ĺ��
		m_CaretID = GetCaret(get_control->GetCaretName());

		// ���ù��״̬
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
	
	// ʧȥ���
	if (lost_control != NULL)
	{
		lost_control->SetCapture(false);
		lost_control->OnLostCapture(get_id);
	}
	
	// ������
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

    // �ж�m_CaptureID�ؼ��ǲ���pModalForm���ӿؼ���������ӿؼ�����ʾ������ʾ����Ȼ����
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

	// ��ǰ��ģʽ����
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

	// ִ�лص�
	// ��������ǰ����ؼ���tips�ı���tips��ʾ���͡�����
	if(RunCallback("on_show_hint", 
		CVarList() << hint_text << hint_type << x << y) > 0)
	{
		m_HintID = hint_id;
		return true;
	}
	
	pHint->SetText(hint_text);
	pHint->SetAbsLeft(x);
	pHint->SetAbsTop(y + GetCursorHeight());

	// �ؼ��������ø�����ʾ
	pControl->OnSetHint(pHint, x, y);

	// ��ֹ�������ӷ�Χ
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
			// ���¿�ʼ��ʱ
			m_HintID = PERSISTID();
			m_fHintCount = 0.0F;
			return true;
		}

		IControl* pControl = GetControl(m_CaptureID);

		if (pControl)
		{
			// �ж�����Ƿ��뿪
			if (IsControlVisible(pControl) && pControl->InRectangle(x, y))
			{
				return false;
			}
			else
			{
                // ��ǰ��ģʽ����
		        IControl* pModalForm = GetModalForm();

                if (pModalForm != NULL)
                {
				    pModalForm->RemoveTopLayer(m_HintID);
                }
			}
		}
		else
		{
            // ��ǰ��ģʽ����
	        IControl* pModalForm = GetModalForm();

            if (pModalForm != NULL)
            {
                pModalForm->RemoveTopLayer(m_HintID);
            }
		}

		m_HintID = PERSISTID();
		
		// ���¿�ʼ��ʱ
		m_fHintCount = 0.0F;
	}
	
	return true;
}

bool Gui::ShowControlContext(IControl* pControl, int x, int y)
{
	// ��ǰ��ģʽ����
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
	
	// �ؼ��������������Ĳ˵�
	pControl->OnSetContext(pContext);
	
	// ��ֹ�������ӷ�Χ
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
			// �ж�����Ƿ��뿪
			if (pContext->InRectangle(x, y))
			{
				return false;
			}
		}
		
		// ��ǰ��ģʽ����
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


// ��ʾ����ʽ�˵�
bool Gui::ShowPopupMenu(IControl* pMenu, int x, int y)
{
	//����֮ǰ�Ĳ˵�
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

// ���ص���ʽ�˵�
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

		// ���ѡ����֮��ķǸ����ؼ�
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
		// �ǿͻ�����
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
	
	// ��������ʹWINDOWS������ʾȱʡ������ͷ
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
	
	// ��ǰ��ģʽ����
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
	
	// �������
	SetCapture((HWND)GetWinHandle());
	
	// ��ǰ��ģʽ����
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
	
	// ��ǰ����Ŀؼ�
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
	
	// �ͷ����
	ReleaseCapture();

	// �ͷ�����
	//GameHand* pGameHand = InnerGetGameHand();
	
	//if (pGameHand)
	//{
	//	pGameHand->ClearHand();
	//}

	// ��ǰ��ģʽ����
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
	
	// ��ǰ��ģʽ����
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
	
	// ��ǰ��ģʽ����
	IControl* pModalForm = GetModalForm();
	
	if (NULL == pModalForm)
	{
		return false;
	}
	
	// ��Ļ����
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
	
	// ��ǰ��ģʽ����
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

// ����IME,��������������
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

// ����IME
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

	// ������������ս���Ϸʱ,��ʱ���벻�ܱ����г���Ϸ��������������BUG
	static bool first = true;

	if (first)
	{
		first = false;
		SendMessage(hwnd, WM_ACTIVATE, WA_INACTIVE, (LPARAM)hwnd);
		SendMessage(hwnd, WM_ACTIVATE, WA_ACTIVE, (LPARAM)hwnd);
		// Ĭ���ȹر����뷨
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

// ���뷨�ı�ص�
bool Gui::MsgInputLanguage(void* hwnd, size_t param1, size_t param2, 
	int& result)
{
    // param2Ϊ���̲���
    m_hkl = (HKL)param2;
    m_ImeProp = ImmGetProperty(m_hkl, IGP_PROPERTY);
#ifdef UNICODE
    m_ImeProp = m_ImeProp | IME_PROP_UNICODE;
#endif
    // param1 ��ǰ�ַ�����Ϣ
    // ͨ���ַ�����ȡ����ҳ
    TranslateCharsetInfo((DWORD*)param1, &m_csInfo, TCI_SRCCHARSET);
    m_CurCP = m_csInfo.ciACP;

    // �Ѿ��л������뷨����ԭ�����������޹�ϵ
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

		// ������ʴ���
        ImmSetCompositionWindow(hIMC, &cp);

        CANDIDATEFORM immdataform;
   
        immdataform.dwIndex = 0;
        immdataform.dwStyle = CFS_CANDIDATEPOS;
        immdataform.ptCurrentPos.x = cp.ptCurrentPos.x + 100;
        immdataform.ptCurrentPos.y = cp.ptCurrentPos.y;

		// ���õ�ǰ������ʾ�б�
        ImmSetCandidateWindow(hIMC, &immdataform);

		// �������뷨״̬��
        //ImmSetStatusWindowPos(hIMC, &caret_pos);

        // �ͷ�����������
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
            // �����ǰ֧��UNICODE
            if (m_ImeProp & IME_PROP_UNICODE)	
            {
                // ȡ���ı�bytes����
                cchBuf = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);

                // ������
                if (cchBuf > 0)	
                {
                    // �����������������
                    lpBuf = (byte*)CORE_ALLOC(cchBuf);
					buf_mem_size = cchBuf;

                    if (lpBuf != NULL)	
                    {
                        // ��ȡ�������ݵ�������
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
            else	// �������UNICODE���
            {
                LPBYTE lpStr = NULL;
				size_t str_mem_size = 0;
                LONG cchStr = 0;

                // ȡ���ı�����
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

                // ת��Unicode
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

                // �ͷ�
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
	// ������뽹��
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

	// �µĳߴ�
	int width = unsigned int(LOWORD(param2));
	int height = unsigned int(HIWORD(param2));
	
	if ((width == 0) || (height == 0))
	{
		// ���ڱ���С��
		return false;
	}
	
    if (width != m_pPainter->GetWinWidth() ||
        height != m_pPainter->GetWinHeight())
    {
        // ������ʾ���ڳߴ�
	    m_pPainter->UpdateWindowSize();
    }

	if ((width == m_pPainter->GetDeviceWidth())
		&& (height == m_pPainter->GetDeviceHeight()))
	{
		// �ߴ�δ�仯
		return false;
	}

	RECT rect;

	GetClientRect((HWND)GetWinHandle(), &rect);

	int cur_win_width = rect.right - rect.left;
	int cur_win_height = rect.bottom - rect.top;

	if ((width != cur_win_width) || (height != cur_win_height))
	{
		// �����Ѿ��ǹ�ʱ����Ϣ
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
	// �µĳߴ�
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

	// �ָ����
	//Cursor* pCursor = InnerGetCursor();

	//if (pCursor)
	//{
	//	pCursor->Reset();
	//}

	return false;
}

// ��ȡ��ǰ���뷨����
bool Gui::LoadCurIMESetting()
{
    HKL hklList[2];
    UINT hkl_count = GetKeyboardLayoutList(2, hklList);

    if (hkl_count > 0)
    {
        // ��ǰ���̲���
        m_hkl = hklList[0];
        // ��ǰ���뷨����
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

    // ��ȡ��ǰ�ַ���
    HWND hwnd = (HWND)GetCore()->GetWindowHandle();
    HDC hdc = GetDC(hwnd);
    //int charset = GetTextCharset(hdc);
	size_t charset = GetTextCharset(hdc);

    // ͨ���ַ�����ȡ����ҳ
    TranslateCharsetInfo((DWORD*)charset, &m_csInfo, TCI_SRCCHARSET);
    m_CurCP = m_csInfo.ciACP;       // ��ǰ����ҳ

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
    //�����������֧��
	case WM_CHAR:
		return MsgChar(hwnd, param1, param2, result);
    case WM_INPUTLANGCHANGE: //���л����̲���ʱ�յ�
        return MsgInputLanguage(hwnd, param1, param2, result);
    case WM_IME_COMPOSITION:
        return MsgIMECompositon(hwnd, param1, param2, result);
    //�����������֧��
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
		// ��ǰ���뽹�㲻�������򲻴������뷨��Ϣ
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

	// �����������ƶ�
	if (m_bAbandomMouseMove)
	{
		return false;
	}
	
	// ��ǰ��ģʽ����
	IControl* pModalForm = GetModalForm();

	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	// ���ظ�����ʾ
	ResetHintStatus(x, y);
	
	// ��ǰ����Ŀؼ�
	IControl* clicked = GetControl(m_ClickedID);
	
	if (clicked != NULL)
	{
		// �϶�
		clicked->OnDragMove(x, y);
		
        return true;
	}
	
	// �ڴ����϶�֮���ж�
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	// ���ù����״
	if (control != NULL)
	{
		control->OnSetCursor(x, y);
	}
	else
	{
		SetCurrentCursor(m_strCursorName.c_str());
	}
	
	// ��꽹��ı�
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
	
    // ���ظ���
    ResetHintStatus(-100, -100);

	// �������
#ifdef _WIN32
	//SetCapture((HWND)GetWinHandle());
#endif
	
	// ��ǰ��ģʽ����
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
		
		// �Ǹ����ؼ�
		while (master->GetDelegateControl() != NULL)
		{
			master = master->GetDelegateControl();
		}
		
		// ���״̬�ؼ�
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
		// �ؼ��Ƿ���Ա�ѡ��
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
			// ���Ի�����뽹��Ŀؼ�
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
			// ���뽹��ı�
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
	
    // ���ظ���
    ResetHintStatus(-100, -100);
	// �ͷ����
#ifdef _WIN32
	ReleaseCapture();
#endif
	
	// ��ǰ��ģʽ����
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
		
		// ģ��һ������ƶ���Ϣ
		int res;

		MsgMouseMove(hwnd, param1, param2, res);

		if (NULL != control)
		{
			control->OnDropIn(x, y);
		}
		
		// �����϶�
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

	// �����ڴ����϶�֮��
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

        // ���������ؼ�����
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

	// ��ǰ��ģʽ����
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

	// ����TAB��
	if (param1 == CGuiInput::KEY_TAB)
	{
		Form* form = (Form*)pModalForm;
		// �����뽹��������һ���ؼ�
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

		// ���������ؼ�����
		control = control->GetDelegateControl();
	}
	
    // ���»س�
    if (param1 == CGuiInput::KEY_RETURN)
    {
        Form* form = (Form*)pModalForm;

        // ȱʡִ�ж�������
        IControl* def_control = GetControl(form->GetDefaultID());

        if (def_control != NULL)
        {
            //û�н��ö�enter����Ӧ
            if (!def_control->GetDisableEnter() 
				&& def_control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }
        }
    }

	// ���Լ�CTRL+F1-F12
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
		
		// ���������ؼ�����
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
		//�������
	case WM_KEYDOWN:
		return MsgKeyDown(0, param1, param2, result);
	case WM_KEYUP:
		return MsgKeyUp(0, param1, param2, result);
    //�����������֧��
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
    //�����������֧��
	case WM_CHAR:
		return MsgChar(hwnd, param1, param2, result);
    case WM_INPUTLANGCHANGE: //���л����̲���ʱ�յ�
        return MsgInputLanguage(hwnd, param1, param2, result);
    case WM_IME_COMPOSITION:
        return MsgIMECompositon(hwnd, param1, param2, result);
    //�����������֧��
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
		// ��ǰ���뽹�㲻�������򲻴������뷨��Ϣ
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
	
    // ���ظ���
    ResetHintStatus(-100, -100);

	// �������
#ifdef _WIN32
	//SetCapture((HWND)GetWinHandle());
#endif
	
	// ��ǰ��ģʽ����
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
		
		// �Ǹ����ؼ�
		while (master->GetDelegateControl() != NULL)
		{
			master = master->GetDelegateControl();
		}
		
		// ���״̬�ؼ�
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
		// �ؼ��Ƿ���Ա�ѡ��
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
			// ���Ի�����뽹��Ŀؼ�
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
			// ���뽹��ı�
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

	// �����������ƶ�
	if (m_bAbandomMouseMove)
	{
		return false;
	}
	
	// ��ǰ��ģʽ����
	IControl* pModalForm = pControl;

	if (NULL == pModalForm)
	{
		return false;
	}

	int x = get_x_coord(m_pPainter, param2);
	int y = get_y_coord(m_pPainter, param2);
	
	// ���ظ�����ʾ
	ResetHintStatus(x, y);
	
	// ��ǰ����Ŀؼ�
	IControl* clicked = GetControl(m_ClickedID);
	
	if (clicked != NULL)
	{
		// �϶�
		clicked->OnDragMove(x, y);
		
        return true;
	}
	
	// �ڴ����϶�֮���ж�
	if (!pModalForm->InRectangle(x, y))
	{
		return true;
	}
	
	IControl* control = pModalForm->GetInWhat(x, y);
	
	// ���ù����״
	if (control != NULL)
	{
		control->OnSetCursor(x, y);
	}
	else
	{
		SetCurrentCursor(m_strCursorName.c_str());
	}
	
	// ��꽹��ı�
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
	
    // ���ظ���
    ResetHintStatus(-100, -100);
	// �ͷ����
#ifdef _WIN32
	ReleaseCapture();
#endif
	
	// ��ǰ��ģʽ����
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
		
		// ģ��һ������ƶ���Ϣ
		int res;

		MsgMouseMove(0, param1, param2, res);

		if (NULL != control)
		{
			control->OnDropIn(x, y);
		}
		
		// �����϶�
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

	// �����ڴ����϶�֮��
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

	// ��ǰ��ģʽ����
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

	// ����TAB��
	if (param1 == CGuiInput::KEY_TAB)
	{
		Form* form = (Form*)pModalForm;
		// �����뽹��������һ���ؼ�
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

		// ���������ؼ�����
		control = control->GetDelegateControl();
	}
	
    // ���»س�
    if (param1 == CGuiInput::KEY_RETURN)
    {
        Form* form = (Form*)pModalForm;

        // ȱʡִ�ж�������
        IControl* def_control = GetControl(form->GetDefaultID());

        if (def_control != NULL)
        {
            //û�н��ö�enter����Ӧ
            if (!def_control->GetDisableEnter() 
				&& def_control->OnKeyDown((unsigned int)param1))
            {
                return true;
            }
        }
    }

	// ���Լ�CTRL+F1-F12
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
