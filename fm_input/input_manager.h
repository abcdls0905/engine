//--------------------------------------------------------------------
// �ļ���:		input_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��5��14��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include "../visual/i_input.h"
#include "../utils/array_pod.h"
#include "input_touch.h"
#include "input_mouse.h"
#include "input_keyboard.h"

/*
�����¼�������
*/

//������
class CInputManager:public IInputManager
{
public:
	~CInputManager();
	// ����һ�������¼� �����¼�����
	ITouchEvent* CreateTouchEvent(PERSISTID Sender_ID,IVarList* pContent,TouchEventType touch_event_type,const FmVec2* pVecTouchPoint,unsigned nNum);

	// ����һ������¼� ����¼����� ���λ�� ������λ��
	IMouseEvent* CreateMouseEvent(PERSISTID Sender_ID,IVarList* pContent,MouseEventType mouse_event_type,FmVec2 mousepos,FmVec4 wheelpos);

	// ����һ�������¼�
    IInputKeyBoardEvent* CreateKeyBoardEvent(PERSISTID Sender_ID,IVarList* pContent,unsigned char key,KeyBoardEventType keytype);

	// ɾ��һ������
	void DeleteTouchEvent(IInputEvent* pInputEvent);

	// �����¼�Ͷ��
	// Ͷ���¼�һ���ǲ������¼����Ի��浽��֡����
	void PostInputEvent(IInputEvent* pInputEvent);

	//  ����ִ���¼�
	// һ��Ϊ����ִ���¼�
	void SendInputEvent(IInputEvent* pInputEvent);

    bool RegisterInputEventCallBack(IInputTouchCallBack* pCallBack);
	void UnRegisterInputEventCallBack(IInputTouchCallBack* pCallBack);

	// ����¼��ص�
	bool RegisterInputEventCallBack(IInputMouseCallBack* pCallBack) ;
	void UnRegisterInputEventCallBack(IInputMouseCallBack* pCallBack) ;

	// ��ʼִ��
	virtual bool Init();
 
	virtual bool Shut() ;

	// �Ƿ���Ҫÿ֡���У�����ExecFrameBegin��ExecFrameEnd��
	virtual bool NeedExecPerFrame() { return true; }
	// ÿ֡��ʼʱ����
	virtual void ExecFrameBegin();
	// ÿ֡����ʱ����
	virtual void ExecFrameEnd();

private:
    //�ɷ����������¼�
	void DoTouchInputEvent(IInputEvent* pInputEvent);
	//�ɷ���������¼�
	void DoMouseInputEvent(IInputEvent* pInputEvent);

    //�����Ƿ��Ѿ����ڸ��¼��ص�
    bool FindPodInputEventCallBack(IInputEventCallBack* pCallBack);
	//�����Ƿ��Ѿ����ڴ����¼�
	bool FindPodPostInputEventRes(IInputEvent* pInputEvent);

	// �����ٵ�
    TArrayPod<IInputEventCallBack*, 1, TCoreAlloc> m_PodInputEventCallBack;
	// �����¼��й���
	TArrayPod<IInputEvent*,1,TCoreAlloc>  m_PodInputEventRes;//���ڴ浵�¼���Դ�ķ�ֹй©���ⷢ��
	TArrayPod<IInputEvent*,1,TCoreAlloc>  m_PodPostInputEvent;//���ڻ�����һ֡�����������¼�
};



#endif