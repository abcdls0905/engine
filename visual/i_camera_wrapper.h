//--------------------------------------------------------------------
// �ļ���:		i_camera_wrapper.h
// ��  ��:		�������װ��ӿ�
// ˵  ��:		
// ��������:	2014��1��13��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __i_camera_wrapper_H__
#define __i_camera_wrapper_H__

#include "i_vis_base.h"

class ICameraWrapper: public IVisBase
{
public:
	// �����ID
	virtual void SetCameraID(const PERSISTID& value) = 0;
	virtual PERSISTID GetCameraID() const = 0;

	// Ŀ��ID
	virtual void SetTargetID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTargetID() const = 0;

	// �Ƿ����Ŀ��
	virtual bool IsTarget() const = 0;

	// DepthOfField
	virtual void SetDepthOfFieldID(const PERSISTID& value) = 0;
	virtual PERSISTID GetDepthOfFieldID() const = 0;

	// ��Ұ�Ƕ�
	virtual void SetFovx(float value) = 0;
	virtual float GetFovx() const = 0;
	virtual void SetFovy(float value) = 0;
	virtual float GetFovy() const = 0;

	// ��߱�
	virtual void SetAspect(float value) = 0;
	virtual float GetAspect() const = 0;

	// ����
	virtual void SetFocusDepth(float value) = 0;
	virtual float GetFocusDepth() const = 0;

    // λ��
    virtual bool SetPosition(float x, float y, float z) = 0;
    virtual FmVec3 GetPosition() const = 0;

    // ����
    virtual bool SetAngle(float x, float y, float z) = 0;
    virtual FmVec3 GetAngle() const = 0;

    // ���þ���
    virtual void SetWorldMatrix(const FmMat4& mat) = 0;

	// ��׶���Ƿ�ɼ�
	virtual void SetPyramidVisible(bool value) = 0;
	virtual bool GetPyramidVisible() const = 0;

	// ���ݿ�߱ȸ���fovy
	virtual void UpdateFovy(float aspect) = 0;

	// ����������
	virtual void SetDepthOfField(const PERSISTID &DepthOfField) = 0;
	virtual PERSISTID GetDepthOfField() const = 0;

    // �Ƿ��Զ���ת
    virtual void SetRotateAble(bool able) = 0;
    virtual bool GetRotateAble() const = 0;
};

#endif // __i_camera_wrapper_H__
