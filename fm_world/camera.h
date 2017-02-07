//--------------------------------------------------------------------
// �ļ���:		camera.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CAMERA_H
#define _CAMERA_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_camera.h"
// �����

class IRenderContext;
class ISceneView;

class Camera: public ICamera
{
public:
	Camera();
	virtual ~Camera();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
 
	void Realize();
	
	// λ��
	bool SetPosition(float x, float y, float z);
	FmVec3 GetPosition() const;

	// ����
	bool SetAngle(float x, float y, float z);
	FmVec3 GetAngle() const;

	// �˶��ٶ�
	bool SetVelocity(float x, float y, float z);
	FmVec3 GetVelocity() const;

	// ���ٶ�
	bool SetAngleSpeed(float x, float y, float z);
	FmVec3 GetAngleSpeed() const;

	// ��ͷ�Ƕ�
	void SetFov(float value);
	float GetFov() const;

	// ��������
	void SetNearPlane(float value);
	float GetNearPlane() const;

	// Զ������
	void SetFarPlane(float value);
	float GetFarPlane() const;
	
	// �Ƿ������׶����
	void SetMakeClipPlane(bool value);
	bool GetMakeClipPlane() const;

	// �Ƿ�ʹ���������
	void SetRelativeCoordinate(bool value);
	bool GetRelativeCoordinate() const;

	// �󶨶���
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const { return m_BindID; }
	
	// ��λ��
	float GetBindPositionX() const { return m_vBindPos.x; }
	float GetBindPositionY() const { return m_vBindPos.y; }
	float GetBindPositionZ() const { return m_vBindPos.z; }
	
	// ���ӵ�����ĸ�������
	void LinkToPoint(const IVarList& args, IVarList& result);

	// �ƶ�һ�ξ���
	bool MoveLength(float len);
	// ����ƽ��
	bool MoveShift(float delta);
	// �������½�
	bool MoveUp(float delta);

	// �Ƿ�ı�߶�
	bool ChangeHeight(bool value);
	// ƽ���϶�
	bool DragMove(float drag_speed, float delta_x, float delta_y);

	// ��ת�Ƕ�
	bool YawAngle(float angle);
	// ǰ����
	bool PitchAngle(float angle);
	// ����ƫת
	bool RollAngle(float angle);
	// �󶨵���������
	bool SetBindPosition(float x, float y, float z);
	// 3D�ռ������ת����Ļ2D����
	void GetScreenPos(const IVarList& args, IVarList& result);
	// ��Ļ����ת��3Dʰȡ����
	bool GetRayTrace(int screen_x, int screen_y, FmVec3* start, 
		FmVec3* direction);
	void GetTraceFromScreen(const IVarList& args, IVarList& result);

private:
	// ���°�λ��
	bool UpdateBind();
	// ���ӵ��߼�ģ�͵ĸ�������
	bool LinkToModelPoint(const PERSISTID& id, const char* helper_name);
	// ���ӵ������ĸ�������
	bool LinkToActorPoint(const PERSISTID& id, const char* helper_name, 
		const char* node_name);
private:
	IRenderContext* m_pContext;
	ISceneView* m_pSceneView;
	FmVec3 m_vPosition;
	FmVec3 m_vVelocity;
	FmVec3 m_vAngleSpeed;
	FmVec3 m_vAngle;
	float m_fFov;
	float m_fNearPlane;
	float m_fFarPlane;
	bool m_bMakeClipPlane;
	bool m_bAllowControl;
	bool m_bRelativeCoordinate;
	PERSISTID m_BindID;
	FmVec3 m_vBindPos;
	int m_nHelperID;
	core_string m_strHelperName;
	core_string m_strNodeName;
	bool  m_bToChangeHeight;
};

#endif // _CAMERA_H
