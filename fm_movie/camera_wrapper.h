//--------------------------------------------------------------------
// 文件名:		CameraWrapper.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __CAMERA_WRAPPER_H__
#define __CAMERA_WRAPPER_H__

#include "../visual/i_camera_wrapper.h"
#include "../visual/i_render.h"
#include "pyramid.h"

// 摄像机包装类
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

	// 是否加载完成
	bool GetLoadFinish();

	// 摄像机ID
	void SetCameraID(const PERSISTID& value);
	PERSISTID GetCameraID() const;

	// 目标ID
	void SetTargetID(const PERSISTID& value);
	PERSISTID GetTargetID() const;

	// 是否存在目标
	bool IsTarget() const;

	// DepthOfField
	void SetDepthOfFieldID(const PERSISTID& value);
	PERSISTID GetDepthOfFieldID() const;

	// 视野角度
	void SetFovx(float value);
	float GetFovx() const;
	void SetFovy(float value);
	float GetFovy() const;

	// 宽高比
	void SetAspect(float value);
	float GetAspect() const;

	// 景深
	void SetFocusDepth(float value);
	float GetFocusDepth() const;

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;
	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 设置矩阵
	virtual void SetWorldMatrix(const FmMat4& mat);
	//virtual void GetWorldMatrix(FmMat4& mat);

	// 缩放
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// 视锥体是否可见
	void SetPyramidVisible(bool value);
	bool GetPyramidVisible() const;

	// 根据宽高比更新fovy
	void UpdateFovy(float aspect);

	// 景深后处理对象
	void SetDepthOfField(const PERSISTID &DepthOfField) {m_DepthOfField = DepthOfField;}
	PERSISTID GetDepthOfField() const {return m_DepthOfField;}

    // 是否自动旋转
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

	// 更新目标方向
	bool UpdateTarget();

	float InnerGetFovyByFovx(float fovx, float aspect) const;
	float InnerGetFovxByFovy(float fovy, float aspect) const;

	bool UpdateDepth(float depth);

private:
    IRender* m_pRender;

	PERSISTID m_CameraID;
	PERSISTID m_TargetID;
	PERSISTID m_DepthOfField;

	//bool m_bFixAspect;	// 锁定宽高比

	Pyramid m_pyramid;
	float	m_fAspect;
	float	m_fFovx;	// 为了在宽高比被修改后也能保持和3dsmax里一致，所以存fovx而不是fovy

	float m_fRealAspect;
	bool  m_bVisible;
    bool  m_bRotateAble;

	int  m_nUpdatePyramid;
};

#endif // __CAMERA_WRAPPER_H__
