#ifndef _I_CAMERA_H
#define _I_CAMERA_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/fm_define.h"

class ICamera: public IEntity
{
public:
	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 运动速度
	virtual bool SetVelocity(float x, float y, float z) = 0;
	virtual FmVec3 GetVelocity() const = 0;

	// 角速度
	virtual bool SetAngleSpeed(float x, float y, float z) = 0;
	virtual FmVec3 GetAngleSpeed() const = 0;

	// 镜头角度
	virtual void SetFov(float value) = 0;
	virtual float GetFov() const = 0;

	// 近剪裁面
	virtual void SetNearPlane(float value) = 0;
	virtual float GetNearPlane() const = 0;

	// 远剪裁面
	virtual void SetFarPlane(float value) = 0;
	virtual float GetFarPlane() const = 0;
	
	// 是否进行视锥剪裁
	virtual void SetMakeClipPlane(bool value) = 0;
	virtual bool GetMakeClipPlane() const = 0;

	// 是否使用相对坐标
	virtual void SetRelativeCoordinate(bool value) = 0;
	virtual bool GetRelativeCoordinate() const = 0;

	// 绑定对象
	virtual void SetBindID(const PERSISTID& value) = 0;
	virtual PERSISTID GetBindID() const = 0;
	
	// 绑定位置
	virtual float GetBindPositionX() const = 0;
	virtual float GetBindPositionY() const = 0;
	virtual float GetBindPositionZ() const = 0;
	
	// 连接到物体的辅助点上
	virtual void LinkToPoint(const IVarList& args, IVarList& result) = 0;

	// 移动一段距离
	virtual bool MoveLength(float len) = 0;
	// 左右平移
	virtual bool MoveShift(float delta) = 0;
	// 上升或下降
	virtual bool MoveUp(float delta) = 0;

	// 是否改变高度
	virtual bool ChangeHeight(bool value) = 0;
	// 平移拖动
	virtual bool DragMove(float drag_speed, float delta_x, float delta_y) = 0;

	// 旋转角度
	virtual bool YawAngle(float angle) = 0;
	// 前后俯仰
	virtual bool PitchAngle(float angle) = 0;
	// 左右偏转
	virtual bool RollAngle(float angle) = 0;
	// 绑定到其他对象
	virtual bool SetBindPosition(float x, float y, float z) = 0;
	// 3D空间点坐标转到屏幕2D坐标
	virtual void GetScreenPos(const IVarList& args, IVarList& result) = 0;
	// 屏幕坐标转到3D拾取向量
	virtual bool GetRayTrace(int screen_x, int screen_y, FmVec3* start, FmVec3* direction) = 0;
	virtual void GetTraceFromScreen(const IVarList& args, IVarList& result) = 0;
};
#endif 