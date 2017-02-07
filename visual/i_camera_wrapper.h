//--------------------------------------------------------------------
// 文件名:		i_camera_wrapper.h
// 内  容:		摄像机包装类接口
// 说  明:		
// 创建日期:	2014年1月13日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __i_camera_wrapper_H__
#define __i_camera_wrapper_H__

#include "i_vis_base.h"

class ICameraWrapper: public IVisBase
{
public:
	// 摄像机ID
	virtual void SetCameraID(const PERSISTID& value) = 0;
	virtual PERSISTID GetCameraID() const = 0;

	// 目标ID
	virtual void SetTargetID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTargetID() const = 0;

	// 是否存在目标
	virtual bool IsTarget() const = 0;

	// DepthOfField
	virtual void SetDepthOfFieldID(const PERSISTID& value) = 0;
	virtual PERSISTID GetDepthOfFieldID() const = 0;

	// 视野角度
	virtual void SetFovx(float value) = 0;
	virtual float GetFovx() const = 0;
	virtual void SetFovy(float value) = 0;
	virtual float GetFovy() const = 0;

	// 宽高比
	virtual void SetAspect(float value) = 0;
	virtual float GetAspect() const = 0;

	// 景深
	virtual void SetFocusDepth(float value) = 0;
	virtual float GetFocusDepth() const = 0;

    // 位置
    virtual bool SetPosition(float x, float y, float z) = 0;
    virtual FmVec3 GetPosition() const = 0;

    // 方向
    virtual bool SetAngle(float x, float y, float z) = 0;
    virtual FmVec3 GetAngle() const = 0;

    // 设置矩阵
    virtual void SetWorldMatrix(const FmMat4& mat) = 0;

	// 视锥体是否可见
	virtual void SetPyramidVisible(bool value) = 0;
	virtual bool GetPyramidVisible() const = 0;

	// 根据宽高比更新fovy
	virtual void UpdateFovy(float aspect) = 0;

	// 景深后处理对象
	virtual void SetDepthOfField(const PERSISTID &DepthOfField) = 0;
	virtual PERSISTID GetDepthOfField() const = 0;

    // 是否自动旋转
    virtual void SetRotateAble(bool able) = 0;
    virtual bool GetRotateAble() const = 0;
};

#endif // __i_camera_wrapper_H__
