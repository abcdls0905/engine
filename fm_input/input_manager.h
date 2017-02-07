//--------------------------------------------------------------------
// 文件名:		input_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年5月14日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "../visual/i_input.h"
#include "../utils/array_pod.h"
#include "input_touch.h"
#include "input_mouse.h"
#include "input_keyboard.h"

/*
输入事件管理器
*/

//管理器
class CInputManager:public IInputManager
{
public:
	~CInputManager();
	// 创建一个触摸事件 触摸事件类型
	ITouchEvent* CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum);

	// 创建一个鼠标事件 鼠标事件类型 鼠标位置 鼠标滚轮位置
	IMouseEvent* CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos);

	// 创建一个键盘事件
    IInputKeyBoardEvent* CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype);

	// 删除一个输入
	void DeleteTouchEvent(IInputEvent* pInputEvent);

	// 进行事件投递
	// 投递事件一般是不主动事件可以缓存到下帧出发
	void PostInputEvent(IInputEvent* pInputEvent);

	//  立即执行事件
	// 一般为主动执行事件
	void SendInputEvent(IInputEvent* pInputEvent);

    bool RegisterInputEventCallBack(IInputTouchCallBack* pCallBack);
	void UnRegisterInputEventCallBack(IInputTouchCallBack* pCallBack);

	// 鼠标事件回调
	bool RegisterInputEventCallBack(IInputMouseCallBack* pCallBack) ;
	void UnRegisterInputEventCallBack(IInputMouseCallBack* pCallBack) ;

	// 开始执行
	virtual bool Init();
 
	virtual bool Shut() ;

	// 是否需要每帧运行（调用ExecFrameBegin和ExecFrameEnd）
	virtual bool NeedExecPerFrame() { return true; }
	// 每帧开始时调用
	virtual void ExecFrameBegin();
	// 每帧结束时调用
	virtual void ExecFrameEnd();

private:
    //派发触摸输入事件
	void DoTouchInputEvent(IInputEvent* pInputEvent);
	//派发鼠标输入事件
	void DoMouseInputEvent(IInputEvent* pInputEvent);

    //查找是否已经存在该事件回调
    bool FindPodInputEventCallBack(IInputEventCallBack* pCallBack);
	//查找是否已经存在触发事件
	bool FindPodPostInputEventRes(IInputEvent* pInputEvent);

	// 触摸毁掉
    TArrayPod<IInputEventCallBack*, 1, TCoreAlloc> m_PodInputEventCallBack;
	// 输入事件托管器
	TArrayPod<IInputEvent*,1,TCoreAlloc>  m_PodInputEventRes;//用于存档事件资源的防止泄漏问题发生
	TArrayPod<IInputEvent*,1,TCoreAlloc>  m_PodPostInputEvent;//用于缓存下一帧用来触发的事件
};



#endif