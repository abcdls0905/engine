
#ifndef _FREECAMERA_H
#define _FREECAMERA_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_camera.h"
#include "../visual/i_painter.h"

#include "../visual/i_input.h"

// FreeCamera
// �༭��������������߼���

class FreeCamera: public IEntity,IInputTouchCallBack
{
public:
	FreeCamera();
	virtual ~FreeCamera();

	// ������Ϣ����
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);
	// ��ʼ��
	virtual bool Init(const IVarList& args);

	virtual bool Shut();

	virtual void Execute(float seconds);

	// ������������������
	void SetBindCamera(PERSISTID pid);

	// �Ƿ�������̺�������
	void SetAllowControl(bool value);

   //��ʼ���µ�ʱ����Ӧ
   void TouchesBegan(ITouchEvent* Event);
   //�ƶ���ʱ����Ӧ
   void TouchesMoved(ITouchEvent* Event);
   //���ֵ�ʱ����Ӧ
   void TouchesEnded(ITouchEvent* Event);
   //��Ϊ����ԭ���µ��¼�ȡ��������˵�绰�¼�
   void TouchesCancelled(ITouchEvent* Event);

private:
#ifdef FX_SYSTEM_WINDOWS
	bool OnKeyDown(size_t vk);
	bool OnKeyUp(size_t vk);
	bool OnChar(size_t vk);
	bool OnMouseMove(size_t param1, size_t param2);
	bool OnMouseWheel(size_t param1, size_t param2);
	bool OnLButtonDown(size_t param1, size_t param2);
	bool OnLButtonUp(size_t param1, size_t param2);
	bool OnMButtonDown(size_t param1, size_t param2);
	bool OnMButtonUp(size_t param1, size_t param2);
	bool OnRButtonDown(size_t param1, size_t param2);
	bool OnRButtonUp(size_t param1, size_t param2);
#endif
public:
	bool GetAllowControl() const;
#ifdef FX_SYSTEM_WINDOWS
	// ����ƶ��ٶ�
	float GetCurMoveSpeed();
	float GetCurDragSpeed();
	float GetCurYawSpeed();

    // add by hutian 2012-1-4
    float GetCurPitchSpeed();
#endif
	
	bool GetAllowWASD();	
	// �ƶ�һ�ξ���
	bool MoveLength(float len);
	// ����ƽ��
	bool MoveShift(float delta);
	// �������½�
	bool MoveUp(float delta);

	// ��ת�Ƕ�
	bool YawAngle(float angle);
	// ǰ����
	bool PitchAngle(float angle);
	// ����ƫת
	bool RollAngle(float angle);

	// �����ƶ��ٶ�
	bool SetVelocity(float x, float y, float z);
	// ���ý��ٶ�
	bool SetAngleSpeed(float x, float y, float z);

	// ��ʱ��ת�Ƕ�
	bool BindYawAngle(float angle);
	// ��ʱ�����Ƕ�
	bool BindPitchAngle(float angle);
	// �ı��λ��
	bool ChangeBindPosition();

private:
    //�лؾ�ͷ����
	void ChangeCarmraPos();

	void WriteLogTouchAction(ITouchEvent* Event);
	// �������
	bool EnableControl();
	// ȡ������
	bool DisableControl();

	int m_nClickX;
	int m_nClickY;
	float m_fDirection;	// �˶�����0��ʾֹͣ
	float m_fUpDirection;
	float m_fYawDirection;

    IPainter* m_pPainter;

    // add by hutian 2012-1-4
    // ���Ӹ������Ӽ��̿���
    float m_fPitchDirection;

	float m_fShiftDirection;
	ICamera* m_pCamera;
	bool   m_bAllowControl;

	IInputManager* m_InputManager;

	FmVec2 m_vec2TouchBegin;//��¼����ָ
	FmVec2 m_vec2TouchBeginTwo;//��¼��2����ָ
	int    m_touchonnum;//��ǰ�Ѿ����ڵ���ָ��Ŀ
	int    m_nCamraPos;
};

#endif // _FREECAMERA_H
