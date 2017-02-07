//--------------------------------------------------------------------
// �ļ���:		i_input.h
// ��  ��:		
// ˵  ��:		�����¼�ϵͳ
// ��������:	2013��6��18��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __I_INPUT_H_
#define __I_INPUT_H_
#include "../public/core_type.h"
#include "../public/module.h"

#include "../math/fm_math.h"



//�����¼�����
enum InputEventType
{
	EVENT_TOUCH,//�����¼�
	EVENT_MOUSE,//����¼�
	EVENT_KEYBOARD,//���������¼�
	EVENT_UNKNOW,//δ֪�¼�
};


//�¼�����
class IInputEvent
{
public:
	//��ȡ�¼�������
	virtual PERSISTID GetEventSenderID() = 0;

	//��ȡ�¼�����
    virtual IVarList* GetEventContent() = 0;

	//�¼�����ʱ��
	virtual double GetEventGenTime() = 0 ;

	//�¼�����
    virtual InputEventType GetEventType() = 0;
};

//�����¼��ص�����
enum InputEventCallBackType
{
	TOUCH_CALLBACK,
	MOUSE_CALLBACK,
	KEYBOARD_CALLBACK,
};

//�¼��ص�����
class IInputEventCallBack
{
public:
   virtual InputEventCallBackType GetInputEventCallBackType() = 0;
};


// �����¼�����
enum TouchEventType
{
	Touch_Began ,//��ʼ����
	Touch_Moved ,//�ƶ�����
	Touch_Ended ,//��������
	Touch_Cancel ,//ȡ���¼�
};


// �����¼�
class  ITouchEvent :public IInputEvent
{
public:
   //�����¼�����
   virtual TouchEventType GetTouchEventType() = 0;
   //��ǰ��������Ŀ
   virtual unsigned int GetTouchNum() = 0;
   //��ȡ��ѯ��ǰ���Ƶ�Ĵ���λ��
   virtual FmVec2 GetTouchPointPos(unsigned int index) = 0;
   //��ѯ��һ�����Ƶ�Ĵ���λ��
   virtual FmVec2 GetTouchPrePointPos(unsigned int index) = 0;
};

class IInputTouchCallBack :public IInputEventCallBack
{
public:
   //��ʼ���µ�ʱ����Ӧ
   virtual void TouchesBegan(ITouchEvent* Event) = 0;
   //�ƶ���ʱ����Ӧ
   virtual void TouchesMoved(ITouchEvent* Event) = 0;
   //���ֵ�ʱ����Ӧ
   virtual void TouchesEnded(ITouchEvent* Event) = 0;
   //��Ϊ����ԭ���µ��¼�ȡ��������˵�绰�¼�
   virtual void TouchesCancelled(ITouchEvent* Event) = 0;

   InputEventCallBackType GetInputEventCallBackType()
   {
	   return TOUCH_CALLBACK;
   }
};

// ����¼�����
enum MouseEventType
{
	Mouse_R_Down,//�Ҽ�����
	Mouse_R_Up,  //�Ҽ��ɿ�
	Mouse_L_Down,//�������
	Mouse_L_Up,  //����ɿ�
	Mouse_M_Down,//�м�����
	Mouse_M_Up,  //�м��ɿ�
	Mouse_W, //�����ֹ����¼�
    Mouse_Move,//����ƶ��¼�
};

// ����¼�
class IMouseEvent :public IInputEvent
{
public:
	//��ȡ��ǰ���λ�� ��ֵ��Χ(0.0 �� 1.0)*��Ļ�ֱ���
	virtual FmVec2 GetMousePos() = 0;
	//��ȡ��ǰ�������� ��ֵ��Χ(0.0 �� 1.0)*��Ļ�ֱ���
	//xy ΪHorizontal position of the pointer
	//zw ΪVertical position of the pointer
	virtual FmVec4 GetWheelPos() = 0;
	//��ȡ��ǰ�����¼�����
    virtual MouseEventType GetMouseEventType() = 0;
};

class IInputMouseCallBack :public IInputEventCallBack
{
public:
	//����¼��ص�
    virtual void MouseCallBack(IMouseEvent* Event) = 0;

   InputEventCallBackType GetInputEventCallBackType()
   {
	   return MOUSE_CALLBACK;
   }
};

// ���̰�������
enum KeyBoardEventType
{
	KeyBoard_Down,//����
	KeyBoard_Up,//�ɿ�
};

// �����¼�
class IInputKeyBoardEvent :public IInputEvent
{
public:
	//��ȡ��ǰ�����İ����¼����������¼���
	virtual unsigned char GetCurrentEventKey() = 0;

	//��ȡ��ǰ�����İ�������
	virtual KeyBoardEventType GetKeyBoardEventType() = 0;
};

class IInputKeyBoardCallBack:public IInputEventCallBack
{
public:
	//���̻ص�
	virtual void KeyBoardCallBack(IInputKeyBoardEvent* Event) = 0;

    InputEventCallBackType GetInputEventCallBackType()
    {
 	   return KEYBOARD_CALLBACK;
    }
}; 


//������
class IInputManager:public IInterface
{
public:
	// ����һ�������¼� �����¼����� ÿ������������� ��ǰ��Ŀ������ ��ͨ������10���㣩 pVecTouchPoint�����ݰڷŽṹΪ[��ǰ������λ�ã���ǰ�������ϴ�λ��] Ҳ����pVecTouchPoint�����ݳ�����2����ϵ
	virtual ITouchEvent* CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum) = 0;

	// ����һ������¼� ����¼����� ���λ�� ������λ��
	virtual IMouseEvent* CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos) = 0;

	// ����һ�������¼�
	virtual IInputKeyBoardEvent* CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype) = 0;

	// ɾ��һ������
	virtual void DeleteTouchEvent(IInputEvent* pInputEvent) = 0;

	// �����¼�Ͷ��
	// Ͷ���¼�һ���ǲ������¼����Ի��浽��֡����
	virtual void PostInputEvent(IInputEvent* pInputEvent) = 0;

	//  ����ִ���¼�
	// һ��Ϊ����ִ���¼�
	virtual void SendInputEvent(IInputEvent* pInputEvent) = 0;

	// �����¼��ص�
    virtual bool RegisterInputEventCallBack(IInputTouchCallBack* pCallBack) = 0;
	virtual void UnRegisterInputEventCallBack(IInputTouchCallBack* pCallBack) = 0;
	// ����¼��ص�
	virtual bool RegisterInputEventCallBack(IInputMouseCallBack* pCallBack) = 0;
	virtual void UnRegisterInputEventCallBack(IInputMouseCallBack* pCallBack) = 0;
};




#endif