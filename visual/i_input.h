//--------------------------------------------------------------------
// 文件名:		i_input.h
// 内  容:		
// 说  明:		输入事件系统
// 创建日期:	2013年6月18日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __I_INPUT_H_
#define __I_INPUT_H_
#include "../public/core_type.h"
#include "../public/module.h"

#include "../math/fm_math.h"



//输入事件类型
enum InputEventType
{
	EVENT_TOUCH,//触摸事件
	EVENT_MOUSE,//鼠标事件
	EVENT_KEYBOARD,//键盘输入事件
	EVENT_UNKNOW,//未知事件
};


//事件基类
class IInputEvent
{
public:
	//获取事件发送者
	virtual PERSISTID GetEventSenderID() = 0;

	//获取事件内容
    virtual IVarList* GetEventContent() = 0;

	//事件发起时间
	virtual double GetEventGenTime() = 0 ;

	//事件类型
    virtual InputEventType GetEventType() = 0;
};

//输入事件回调类型
enum InputEventCallBackType
{
	TOUCH_CALLBACK,
	MOUSE_CALLBACK,
	KEYBOARD_CALLBACK,
};

//事件回调函数
class IInputEventCallBack
{
public:
   virtual InputEventCallBackType GetInputEventCallBackType() = 0;
};


// 触摸事件类型
enum TouchEventType
{
	Touch_Began ,//开始触摸
	Touch_Moved ,//移动触摸
	Touch_Ended ,//结束触摸
	Touch_Cancel ,//取消事件
};


// 触摸事件
class  ITouchEvent :public IInputEvent
{
public:
   //触摸事件类型
   virtual TouchEventType GetTouchEventType() = 0;
   //当前触摸点数目
   virtual unsigned int GetTouchNum() = 0;
   //获取查询当前手势点的触摸位置
   virtual FmVec2 GetTouchPointPos(unsigned int index) = 0;
   //查询上一次手势点的触摸位置
   virtual FmVec2 GetTouchPrePointPos(unsigned int index) = 0;
};

class IInputTouchCallBack :public IInputEventCallBack
{
public:
   //开始按下的时候响应
   virtual void TouchesBegan(ITouchEvent* Event) = 0;
   //移动的时候响应
   virtual void TouchesMoved(ITouchEvent* Event) = 0;
   //松手的时候响应
   virtual void TouchesEnded(ITouchEvent* Event) = 0;
   //因为其他原因导致的事件取消，比如说电话事件
   virtual void TouchesCancelled(ITouchEvent* Event) = 0;

   InputEventCallBackType GetInputEventCallBackType()
   {
	   return TOUCH_CALLBACK;
   }
};

// 鼠标事件类型
enum MouseEventType
{
	Mouse_R_Down,//右键按下
	Mouse_R_Up,  //右键松开
	Mouse_L_Down,//左键按下
	Mouse_L_Up,  //左键松开
	Mouse_M_Down,//中键按下
	Mouse_M_Up,  //中间松开
	Mouse_W, //鼠标滚轮滚动事件
    Mouse_Move,//鼠标移动事件
};

// 鼠标事件
class IMouseEvent :public IInputEvent
{
public:
	//获取当前鼠标位置 数值范围(0.0 ～ 1.0)*屏幕分辨率
	virtual FmVec2 GetMousePos() = 0;
	//获取当前滚轮坐标 数值范围(0.0 ～ 1.0)*屏幕分辨率
	//xy 为Horizontal position of the pointer
	//zw 为Vertical position of the pointer
	virtual FmVec4 GetWheelPos() = 0;
	//获取当前鼠标的事件类型
    virtual MouseEventType GetMouseEventType() = 0;
};

class IInputMouseCallBack :public IInputEventCallBack
{
public:
	//鼠标事件回调
    virtual void MouseCallBack(IMouseEvent* Event) = 0;

   InputEventCallBackType GetInputEventCallBackType()
   {
	   return MOUSE_CALLBACK;
   }
};

// 键盘按键类型
enum KeyBoardEventType
{
	KeyBoard_Down,//按下
	KeyBoard_Up,//松开
};

// 键盘事件
class IInputKeyBoardEvent :public IInputEvent
{
public:
	//获取当前触发的按键事件（主按键事件）
	virtual unsigned char GetCurrentEventKey() = 0;

	//获取当前触发的按键类型
	virtual KeyBoardEventType GetKeyBoardEventType() = 0;
};

class IInputKeyBoardCallBack:public IInputEventCallBack
{
public:
	//键盘回调
	virtual void KeyBoardCallBack(IInputKeyBoardEvent* Event) = 0;

    InputEventCallBackType GetInputEventCallBackType()
    {
 	   return KEYBOARD_CALLBACK;
    }
}; 


//管理器
class IInputManager:public IInterface
{
public:
	// 创建一个触摸事件 触摸事件类型 每个触摸点的数据 当前数目点总数 （通常少于10个点） pVecTouchPoint的数据摆放结构为[当前触摸点位置，当前触摸点上次位置] 也就是pVecTouchPoint的数据长度是2倍关系
	virtual ITouchEvent* CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum) = 0;

	// 创建一个鼠标事件 鼠标事件类型 鼠标位置 鼠标滚轮位置
	virtual IMouseEvent* CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos) = 0;

	// 创建一个键盘事件
	virtual IInputKeyBoardEvent* CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype) = 0;

	// 删除一个输入
	virtual void DeleteTouchEvent(IInputEvent* pInputEvent) = 0;

	// 进行事件投递
	// 投递事件一般是不主动事件可以缓存到下帧出发
	virtual void PostInputEvent(IInputEvent* pInputEvent) = 0;

	//  立即执行事件
	// 一般为主动执行事件
	virtual void SendInputEvent(IInputEvent* pInputEvent) = 0;

	// 触控事件回调
    virtual bool RegisterInputEventCallBack(IInputTouchCallBack* pCallBack) = 0;
	virtual void UnRegisterInputEventCallBack(IInputTouchCallBack* pCallBack) = 0;
	// 鼠标事件回调
	virtual bool RegisterInputEventCallBack(IInputMouseCallBack* pCallBack) = 0;
	virtual void UnRegisterInputEventCallBack(IInputMouseCallBack* pCallBack) = 0;
};




#endif