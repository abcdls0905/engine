#include "win_input.h"

#define _WIN32_WINNT 0x0400
#include <windows.h>

DECLARE_ENTITY(CInput, 0, IEntity);
// ³õÊ¼»¯
bool CInput::Init(const IVarList& args)
{

    m_InputManager = (IInputManager*)GetCore()->GetInterface("CInputManager");

   	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	EnableControl();
	return true;
}

bool CInput::MsgProc(unsigned int msg, size_t param1, size_t param2, int& result)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		{
			
		}break;
	case WM_KEYUP:
		{
			
		}break;
	case WM_MOUSEMOVE:
		{
			int nx = LOWORD(param2); 
			int ny = HIWORD(param2);
			float fX = (float)nx / m_pRender->GetDeviceWidth();
			float fY = (float)ny / m_pRender->GetDeviceHeight();
			IMouseEvent* pEvent = m_InputManager->CreateMouseEvent(this->GetID(),NULL,Mouse_Move,FmVec2(fX,fY),FmVec4());
			m_InputManager->SendInputEvent(pEvent);
			m_InputManager->DeleteTouchEvent(pEvent);

			if ((param1 & MK_RBUTTON) != 0)
			{

				FmVec2 vec(fX,fY);

				ITouchEvent* pTouchEvent = m_InputManager->CreateTouchEvent(this->GetID(),NULL,Touch_Moved,&vec,1);
				m_InputManager->SendInputEvent(pTouchEvent);
				m_InputManager->DeleteTouchEvent(pTouchEvent);
			}

		}break;
		/*
	case WM_MOUSEWHEEL:
		{
			
		}break;
		*/
	case WM_RBUTTONDOWN:
		{
			int nx = LOWORD(param2); 
			int ny = HIWORD(param2);
			float fX = (float)nx / m_pRender->GetDeviceWidth();
			float fY = (float)ny / m_pRender->GetDeviceHeight();
			IMouseEvent* pEvent = m_InputManager->CreateMouseEvent(this->GetID(),NULL,Mouse_R_Down,FmVec2(fX,fY),FmVec4());
			m_InputManager->SendInputEvent(pEvent);
			m_InputManager->DeleteTouchEvent(pEvent);	

			FmVec2 vec(fX,fY);

			ITouchEvent* pTouchEvent = m_InputManager->CreateTouchEvent(this->GetID(),NULL,Touch_Began,&vec,1);
            m_InputManager->SendInputEvent(pTouchEvent);
            m_InputManager->DeleteTouchEvent(pTouchEvent);
		}break;
	case WM_RBUTTONUP:
		{
			int nx = LOWORD(param2); 
			int ny = HIWORD(param2);
			float fX = (float)nx / m_pRender->GetDeviceWidth();
			float fY = (float)ny / m_pRender->GetDeviceHeight();
			IMouseEvent* pEvent = m_InputManager->CreateMouseEvent(this->GetID(),NULL,Mouse_R_Up,FmVec2(fX,fY),FmVec4());
			m_InputManager->SendInputEvent(pEvent);
			m_InputManager->DeleteTouchEvent(pEvent);
			FmVec2 vec(fX,fY);

			ITouchEvent* pTouchEvent = m_InputManager->CreateTouchEvent(this->GetID(),NULL,Touch_Ended,&vec,1);
            m_InputManager->SendInputEvent(pTouchEvent);
            m_InputManager->DeleteTouchEvent(pTouchEvent);
			
		}break;
	case WM_MBUTTONDOWN:
		{	
			int nx = LOWORD(param2); 
			int ny = HIWORD(param2);
			float fX = (float)nx / m_pRender->GetDeviceWidth();
			float fY = (float)ny / m_pRender->GetDeviceHeight();
			IMouseEvent* pEvent = m_InputManager->CreateMouseEvent(this->GetID(),NULL,Mouse_M_Down,FmVec2(fX,fY),FmVec4());
			m_InputManager->SendInputEvent(pEvent);
			m_InputManager->DeleteTouchEvent(pEvent);
			FmVec2 vec(fX,fY);

			ITouchEvent* pTouchEvent = m_InputManager->CreateTouchEvent(this->GetID(),NULL,Touch_Cancel,&vec,1);
            m_InputManager->SendInputEvent(pTouchEvent);
            m_InputManager->DeleteTouchEvent(pTouchEvent);
		}break;
	default:
		break;
	}


	return true;
}

bool CInput::Shut()
{
	return true;
}

bool CInput::EnableControl()
{
	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	GetCore()->AddMsgProc(this, WM_KEYUP);
	GetCore()->AddMsgProc(this, WM_CHAR);
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
//	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	GetCore()->AddMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_LBUTTONUP);
	GetCore()->AddMsgProc(this, WM_MBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_MBUTTONUP);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
	
	return true;
}

bool CInput::DisableControl()
{
	GetCore()->RemoveMsgProc(this, WM_KEYDOWN);
	GetCore()->RemoveMsgProc(this, WM_KEYUP);
	GetCore()->RemoveMsgProc(this, WM_CHAR);
	GetCore()->RemoveMsgProc(this, WM_MOUSEMOVE);
//	GetCore()->RemoveMsgProc(this, WM_MOUSEWHEEL);
	GetCore()->RemoveMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_LBUTTONUP);
	GetCore()->RemoveMsgProc(this, WM_MBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_MBUTTONUP);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONUP);
	
	return true;
}
