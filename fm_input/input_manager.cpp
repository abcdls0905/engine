#include "input_manager.h"

DECLARE_FUNCTION_INTERFACE(CInputManager);

CInputManager::~CInputManager()
{
	//�ͷ�������Դ
	for( size_t i = 0; i < m_PodInputEventRes.size() ;i ++ )
	{
		IInputEvent* pInputEvent = m_PodPostInputEvent[i];
		switch( pInputEvent->GetEventType() )
		{
		case EVENT_TOUCH:
			CORE_FREE(pInputEvent,sizeof(CTouchEvent));
			break;
		case EVENT_MOUSE:
			CORE_FREE(pInputEvent,sizeof(CMouseEvent));
			break;
		case EVENT_KEYBOARD:
			break;
		};
	} 

	m_PodInputEventRes.clear();
	m_PodInputEventCallBack.clear();
	m_PodPostInputEvent.clear();

}

// ����һ�������¼�
ITouchEvent* CInputManager::CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum)
{
	//��ȡ��ǰʱ������¼�
	double dtime = GetCore()->GetTotalSeconds();
	CTouchEvent* pTouchEvent = (CTouchEvent*)CORE_ALLOC(sizeof(CTouchEvent));
	new (pTouchEvent) CTouchEvent(Sender_ID,pContent,dtime,touch_event_type,pVecTouchPoint,nNum);
	m_PodInputEventRes.push_back((IInputEvent*)pTouchEvent);
	return pTouchEvent;
}

// ����һ������¼� ����¼����� ���λ�� ������λ��
IMouseEvent* CInputManager::CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos)
{
	//��ȡ��ǰʱ��
	double dtime = GetCore()->GetTotalSeconds();
	CMouseEvent* pMouseEvent = (CMouseEvent*)CORE_ALLOC(sizeof(CMouseEvent));
	new (pMouseEvent) CMouseEvent(Sender_ID,pContent,dtime,mouse_event_type,mousepos,wheelpos);
	m_PodInputEventRes.push_back((IInputEvent*)pMouseEvent);
	return pMouseEvent;
}

// ����һ�������¼�
IInputKeyBoardEvent* CInputManager::CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype)
{
	double dtime = GetCore()->GetTotalSeconds();
	CKeyBoardEvent* pKeyBoardEvent = (CKeyBoardEvent*)CORE_ALLOC(sizeof(CKeyBoardEvent));
	new (pKeyBoardEvent) CKeyBoardEvent(Sender_ID,pContent,dtime,key,keytype);
    m_PodInputEventRes.push_back((IInputEvent*)pKeyBoardEvent);
	return pKeyBoardEvent;
}

// ɾ��һ������
void CInputManager::DeleteTouchEvent(IInputEvent* pInputEvent)
{
	for( size_t i = 0; i < m_PodInputEventRes.size() ;i ++ )
	{
		if(pInputEvent == m_PodInputEventRes[i])
		{
			CORE_FREE(pInputEvent,sizeof(CTouchEvent));
			m_PodInputEventRes.remove(i);
		}
	}   
}

// �����¼�Ͷ��
// Ͷ���¼�һ���ǲ������¼����Ի��浽��֡����
void CInputManager::PostInputEvent(IInputEvent* pInputEvent)
{
	// ����ʵ�ʴ��ڵ���Ч��Դʵ��
	if(!FindPodPostInputEventRes(pInputEvent))
	{
		m_PodPostInputEvent.push_back(pInputEvent);
	}
}

//  ����ִ���¼�
// һ��Ϊ����ִ���¼�
void CInputManager::SendInputEvent(IInputEvent* pInputEvent)
{
	//��֤�¼�����Ч��(ʵ����,��Ҫ�Ƿ�ֹ�ͻ���ȥ���Ա����¼�ָ�����ֶ���������Ч�¼�)�����ڿ��Կ����Ż��������
	if(FindPodPostInputEventRes(pInputEvent))
	{
		switch( pInputEvent->GetEventType() )
		{
		case EVENT_TOUCH:
			DoTouchInputEvent(pInputEvent);
			break;
		case EVENT_MOUSE:
			DoMouseInputEvent(pInputEvent);
			break;
		case EVENT_KEYBOARD:
			break;
		};
	}

}

// ע���¼���
bool CInputManager::RegisterInputEventCallBack(IInputTouchCallBack* pCallBack)
{
	if(!FindPodInputEventCallBack(pCallBack))
	{
		m_PodInputEventCallBack.push_back(pCallBack);
	}
	return true;
}

// ж��ע����
void CInputManager::UnRegisterInputEventCallBack(IInputTouchCallBack* pCallBack)
{
	for( size_t i = 0; i < m_PodInputEventCallBack.size() ;i ++ )
	{
		if(pCallBack == m_PodInputEventCallBack[i])
		{
			m_PodInputEventCallBack.remove(i);
			return;
		}
	}
}

// ����¼��ص�
bool CInputManager::RegisterInputEventCallBack(IInputMouseCallBack* pCallBack)
{
	if(!FindPodInputEventCallBack(pCallBack))
	{
		m_PodInputEventCallBack.push_back(pCallBack);
	}
	return true;
}

void CInputManager::UnRegisterInputEventCallBack(IInputMouseCallBack* pCallBack)
{
	for( size_t i = 0; i < m_PodInputEventCallBack.size() ;i ++ )
	{
		if(pCallBack == m_PodInputEventCallBack[i])
		{
			m_PodInputEventCallBack.remove(i);
			return;
		}
	}
}


// ��ʼִ��
bool CInputManager::Init()
{
	return true;
}

bool CInputManager::Shut()
{
	return true;
}

// ÿ֡��ʼʱ����
void CInputManager::ExecFrameBegin()
{
	//���������Զ��ɷ�����Ͷ�ݵ��¼�
	for(size_t i = 0; i < m_PodPostInputEvent.size() ; i++ )
	{
		IInputEvent* pInputEvent = m_PodPostInputEvent[i];
		switch( pInputEvent->GetEventType() )
		{
		case EVENT_TOUCH:
			DoTouchInputEvent(pInputEvent);
			break;
		case EVENT_MOUSE:
			DoMouseInputEvent(pInputEvent);
			break;
		case EVENT_KEYBOARD:
			break;
		};
	}
}

// ÿ֡����ʱ����
void CInputManager::ExecFrameEnd()
{
	m_PodPostInputEvent.clear();
}


//�ɷ����������¼�
void CInputManager::DoTouchInputEvent(IInputEvent* pInputEvent)
{
	CTouchEvent* pTouchEvent = (CTouchEvent*)pInputEvent;

	for( size_t i = 0; i < m_PodInputEventCallBack.size() ;i ++ )
	{

		if(m_PodInputEventCallBack[i]->GetInputEventCallBackType() != TOUCH_CALLBACK )
		{
 			continue;
		}

		IInputTouchCallBack* pCallBack = (IInputTouchCallBack*)m_PodInputEventCallBack[i];
		switch(pTouchEvent->GetTouchEventType())
		{
		case Touch_Began:
			pCallBack->TouchesBegan(pTouchEvent);
			break;
		case Touch_Moved:
			pCallBack->TouchesMoved(pTouchEvent);
			break;
		case Touch_Ended:
			pCallBack->TouchesEnded(pTouchEvent);
			break;
		case Touch_Cancel:
			pCallBack->TouchesCancelled(pTouchEvent);
			break;
		default:
			break;
		};
	}
}

//�ɷ���������¼�
void CInputManager::DoMouseInputEvent(IInputEvent* pInputEvent)
{
	CMouseEvent* pMouseEvent = (CMouseEvent*)pInputEvent;

	for( size_t i = 0; i < m_PodInputEventCallBack.size() ;i ++ )
	{

		if(m_PodInputEventCallBack[i]->GetInputEventCallBackType() != MOUSE_CALLBACK )
		{
			continue;
		}

		IInputMouseCallBack* pCallBack = (IInputMouseCallBack*)m_PodInputEventCallBack[i];
		pCallBack->MouseCallBack(pMouseEvent);
	}
}


//�����Ƿ��Ѿ����ڸ��¼��ٵ�
bool CInputManager::FindPodInputEventCallBack(IInputEventCallBack* pCallBack)
{
	for( size_t i = 0; i < m_PodInputEventCallBack.size() ;i ++ )
	{
		if(pCallBack == m_PodInputEventCallBack[i])
		{
			return true;
		}
	}

	return false;
}

//�����Ƿ��Ѿ����ڴ����¼�
bool CInputManager::FindPodPostInputEventRes(IInputEvent* pInputEvent)
{
	for( size_t i = 0; i < m_PodInputEventRes.size() ;i ++ )
	{
		if(pInputEvent == m_PodInputEventRes[i])
		{
			return true;
		}
	}
	return false;
}