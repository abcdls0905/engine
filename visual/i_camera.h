#ifndef _I_CAMERA_H
#define _I_CAMERA_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/fm_define.h"

class ICamera: public IEntity
{
public:
	// λ��
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// ����
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// �˶��ٶ�
	virtual bool SetVelocity(float x, float y, float z) = 0;
	virtual FmVec3 GetVelocity() const = 0;

	// ���ٶ�
	virtual bool SetAngleSpeed(float x, float y, float z) = 0;
	virtual FmVec3 GetAngleSpeed() const = 0;

	// ��ͷ�Ƕ�
	virtual void SetFov(float value) = 0;
	virtual float GetFov() const = 0;

	// ��������
	virtual void SetNearPlane(float value) = 0;
	virtual float GetNearPlane() const = 0;

	// Զ������
	virtual void SetFarPlane(float value) = 0;
	virtual float GetFarPlane() const = 0;
	
	// �Ƿ������׶����
	virtual void SetMakeClipPlane(bool value) = 0;
	virtual bool GetMakeClipPlane() const = 0;

	// �Ƿ�ʹ���������
	virtual void SetRelativeCoordinate(bool value) = 0;
	virtual bool GetRelativeCoordinate() const = 0;

	// �󶨶���
	virtual void SetBindID(const PERSISTID& value) = 0;
	virtual PERSISTID GetBindID() const = 0;
	
	// ��λ��
	virtual float GetBindPositionX() const = 0;
	virtual float GetBindPositionY() const = 0;
	virtual float GetBindPositionZ() const = 0;
	
	// ���ӵ�����ĸ�������
	virtual void LinkToPoint(const IVarList& args, IVarList& result) = 0;

	// �ƶ�һ�ξ���
	virtual bool MoveLength(float len) = 0;
	// ����ƽ��
	virtual bool MoveShift(float delta) = 0;
	// �������½�
	virtual bool MoveUp(float delta) = 0;

	// �Ƿ�ı�߶�
	virtual bool ChangeHeight(bool value) = 0;
	// ƽ���϶�
	virtual bool DragMove(float drag_speed, float delta_x, float delta_y) = 0;

	// ��ת�Ƕ�
	virtual bool YawAngle(float angle) = 0;
	// ǰ����
	virtual bool PitchAngle(float angle) = 0;
	// ����ƫת
	virtual bool RollAngle(float angle) = 0;
	// �󶨵���������
	virtual bool SetBindPosition(float x, float y, float z) = 0;
	// 3D�ռ������ת����Ļ2D����
	virtual void GetScreenPos(const IVarList& args, IVarList& result) = 0;
	// ��Ļ����ת��3Dʰȡ����
	virtual bool GetRayTrace(int screen_x, int screen_y, FmVec3* start, FmVec3* direction) = 0;
	virtual void GetTraceFromScreen(const IVarList& args, IVarList& result) = 0;
};
#endif 