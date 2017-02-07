//--------------------------------------------------------------------
// �ļ���:		WinFreeCamera.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#pragma once

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_camera.h"

#include "camera_base.h"

// WinFreeCamera
// �༭��������������߼���

class WinFreeCamera: public CameraBase
{
public:
	WinFreeCamera();
	virtual ~WinFreeCamera();

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
private:
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

public:
	bool GetAllowControl() const;
	// ����ƶ��ٶ�
	float GetCurMoveSpeed();
	float GetCurDragSpeed();
	float GetCurYawSpeed();

    // add by hutian 2012-1-4
    float GetCurPitchSpeed();

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

	void OnEnable();
	void OnDisable();
private:

	// �������
	bool EnableControl();
	// ȡ������
	bool DisableControl();

	int m_nClickX;
	int m_nClickY;
	float m_fDirection;	// �˶�����0��ʾֹͣ
	float m_fUpDirection;
	float m_fYawDirection;



    // add by hutian 2012-1-4
    // ���Ӹ������Ӽ��̿���
    float m_fPitchDirection;

	float m_fShiftDirection;
	bool   m_bAllowControl;
};


