//--------------------------------------------------------------------
// 文件名:		WinFreeCamera.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#pragma once

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_camera.h"

#include "camera_base.h"

// WinFreeCamera
// 编辑器用摄像机控制逻辑类

class WinFreeCamera: public CameraBase
{
public:
	WinFreeCamera();
	virtual ~WinFreeCamera();

	// 窗口消息处理
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);
	// 初始化
	virtual bool Init(const IVarList& args);

	virtual bool Shut();

	virtual void Execute(float seconds);

	// 创建后必须设置摄像机
	void SetBindCamera(PERSISTID pid);

	// 是否允许键盘和鼠标控制
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
	// 获得移动速度
	float GetCurMoveSpeed();
	float GetCurDragSpeed();
	float GetCurYawSpeed();

    // add by hutian 2012-1-4
    float GetCurPitchSpeed();

	bool GetAllowWASD();
	
	// 移动一段距离
	bool MoveLength(float len);
	// 左右平移
	bool MoveShift(float delta);
	// 上升或下降
	bool MoveUp(float delta);

	// 旋转角度
	bool YawAngle(float angle);
	// 前后俯仰
	bool PitchAngle(float angle);
	// 左右偏转
	bool RollAngle(float angle);

	// 设置移动速度
	bool SetVelocity(float x, float y, float z);
	// 设置角速度
	bool SetAngleSpeed(float x, float y, float z);

	// 绑定时旋转角度
	bool BindYawAngle(float angle);
	// 绑定时俯仰角度
	bool BindPitchAngle(float angle);
	// 改变绑定位置
	bool ChangeBindPosition();

	void OnEnable();
	void OnDisable();
private:

	// 允许控制
	bool EnableControl();
	// 取消控制
	bool DisableControl();

	int m_nClickX;
	int m_nClickY;
	float m_fDirection;	// 运动方向，0表示停止
	float m_fUpDirection;
	float m_fYawDirection;



    // add by hutian 2012-1-4
    // 增加俯视仰视键盘控制
    float m_fPitchDirection;

	float m_fShiftDirection;
	bool   m_bAllowControl;
};


