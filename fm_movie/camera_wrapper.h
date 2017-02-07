//--------------------------------------------------------------------
// �ļ���:		CameraWrapper.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __CAMERA_WRAPPER_H__
#define __CAMERA_WRAPPER_H__

#include "../visual/i_camera_wrapper.h"
#include "../visual/i_render.h"
#include "pyramid.h"

// �������װ��
struct sObjectData;
class CameraWrapper: public ICameraWrapper
{
public:
	CameraWrapper();
	virtual ~CameraWrapper() {}

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();

	// �Ƿ�������
	bool GetLoadFinish();

	// �����ID
	void SetCameraID(const PERSISTID& value);
	PERSISTID GetCameraID() const;

	// Ŀ��ID
	void SetTargetID(const PERSISTID& value);
	PERSISTID GetTargetID() const;

	// �Ƿ����Ŀ��
	bool IsTarget() const;

	// DepthOfField
	void SetDepthOfFieldID(const PERSISTID& value);
	PERSISTID GetDepthOfFieldID() const;

	// ��Ұ�Ƕ�
	void SetFovx(float value);
	float GetFovx() const;
	void SetFovy(float value);
	float GetFovy() const;

	// ��߱�
	void SetAspect(float value);
	float GetAspect() const;

	// ����
	void SetFocusDepth(float value);
	float GetFocusDepth() const;

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;
	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ���þ���
	virtual void SetWorldMatrix(const FmMat4& mat);
	//virtual void GetWorldMatrix(FmMat4& mat);

	// ����
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// ��׶���Ƿ�ɼ�
	void SetPyramidVisible(bool value);
	bool GetPyramidVisible() const;

	// ���ݿ�߱ȸ���fovy
	void UpdateFovy(float aspect);

	// ����������
	void SetDepthOfField(const PERSISTID &DepthOfField) {m_DepthOfField = DepthOfField;}
	PERSISTID GetDepthOfField() const {return m_DepthOfField;}

    // �Ƿ��Զ���ת
    void SetRotateAble(bool able)
    {
        m_bRotateAble = able;
    }

    bool GetRotateAble() const
    {
        return m_bRotateAble;
    }

	void UpdateTargetCameraKey(FmVec3 *pPos, FmVec3 *pAngle,
		const sObjectData &object_data, bool b_need_rotation = false);

protected:
	IEntity* GetEntity(const PERSISTID& id) const { return GetCore()->GetEntity(id); }
	IVisBase* GetVisBase(const PERSISTID& id) const;
	ICamera* GetRealCamera(const PERSISTID& id) const;

	// ����Ŀ�귽��
	bool UpdateTarget();

	float InnerGetFovyByFovx(float fovx, float aspect) const;
	float InnerGetFovxByFovy(float fovy, float aspect) const;

	bool UpdateDepth(float depth);

private:
    IRender* m_pRender;

	PERSISTID m_CameraID;
	PERSISTID m_TargetID;
	PERSISTID m_DepthOfField;

	//bool m_bFixAspect;	// ������߱�

	Pyramid m_pyramid;
	float	m_fAspect;
	float	m_fFovx;	// Ϊ���ڿ�߱ȱ��޸ĺ�Ҳ�ܱ��ֺ�3dsmax��һ�£����Դ�fovx������fovy

	float m_fRealAspect;
	bool  m_bVisible;
    bool  m_bRotateAble;

	int  m_nUpdatePyramid;
};

#endif // __CAMERA_WRAPPER_H__
