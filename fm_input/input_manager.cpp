#include "input_manager.h"

DECLARE_FUNCTION_INTERFACE(CInputManager);

CInputManager::~CInputManager()
{
	//释放所有资源
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

// 创建一个触摸事件
ITouchEvent* CInputManager::CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum)
{
	//获取当前时间产生事件
	double dtime = GetCore()->GetTotalSeconds();
	CTouchEvent* pTouchEvent = (CTouchEvent*)CORE_ALLOC(sizeof(CTouchEvent));
	new (pTouchEvent) CTouchEvent(Sender_ID,pContent,dtime,touch_event_type,pVecTouchPoint,nNum);
	m_PodInputEventRes.push_back((IInputEvent*)pTouchEvent);
	return pTouchEvent;
}

// 创建一个鼠标事件 鼠标事件类型 鼠标位置 鼠标滚轮位置
IMouseEvent* CInputManager::CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos)
{
	//获取当前时间
	double dtime = GetCore()->GetTotalSeconds();
	CMouseEvent* pMouseEvent = (CMouseEvent*)CORE_ALLOC(sizeof(CMouseEvent));
	new (pMouseEvent) CMouseEvent(Sender_ID,pContent,dtime,mouse_event_type,mousepos,wheelpos);
	m_PodInputEventRes.push_back((IInputEvent*)pMouseEvent);
	return pMouseEvent;
}

// 创建一个键盘事件
IInputKeyBoardEvent* CInputManager::CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype)
{
	double dtime = GetCore()->GetTotalSeconds();
	CKeyBoardEvent* pKeyBoardEvent = (CKeyBoardEvent*)CORE_ALLOC(sizeof(CKeyBoardEvent));
	new (pKeyBoardEvent) CKeyBoardEvent(Sender_ID,pContent,dtime,key,keytype);
    m_PodInputEventRes.push_back((IInputEvent*)pKeyBoardEvent);
	return pKeyBoardEvent;
}

// 删除一个输入
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

// 进行事件投递
// 投递事件一般是不主动事件可以缓存到下帧出发
void CInputManager::PostInputEvent(IInputEvent* pInputEvent)
{
	// 存入实际存在的有效资源实体
	if(!FindPodPostInputEventRes(pInputEvent))
	{
		m_PodPostInputEvent.push_back(pInputEvent);
	}
}

//  立即执行事件
// 一般为主动执行事件
void CInputManager::SendInputEvent(IInputEvent* pInputEvent)
{
	//验证事件的有效性(实际上,主要是防止客户端去可以保存事件指针这种东西导致无效事件)，后期可以考虑优化这个查找
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

// 注册事件类
bool CInputManager::RegisterInputEventCallBack(IInputTouchCallBack* pCallBack)
{
	if(!FindPodInputEventCallBack(pCallBack))
	{
		m_PodInputEventCallBack.push_back(pCallBack);
	}
	return true;
}

// 卸载注册类
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

// 鼠标事件回调
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


// 开始执行
bool CInputManager::Init()
{
	return true;
}

bool CInputManager::Shut()
{
	return true;
}

// 每帧开始时调用
void CInputManager::ExecFrameBegin()
{
	//这个里面会自动派发所以投递的事件
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

// 每帧结束时调用
void CInputManager::ExecFrameEnd()
{
	m_PodPostInputEvent.clear();
}


//派发触摸输入事件
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

//派发鼠标输入事件
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


//查找是否已经存在该事件毁掉
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

//查找是否已经存在触发事件
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