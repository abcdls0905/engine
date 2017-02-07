//--------------------------------------------------------------------
// 文件名:		camera.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CAMERA_H
#define _CAMERA_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_camera.h"
// 摄像机

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
	
	// 位置
	bool SetPosition(float x, float y, float z);
	FmVec3 GetPosition() const;

	// 方向
	bool SetAngle(float x, float y, float z);
	FmVec3 GetAngle() const;

	// 运动速度
	bool SetVelocity(float x, float y, float z);
	FmVec3 GetVelocity() const;

	// 角速度
	bool SetAngleSpeed(float x, float y, float z);
	FmVec3 GetAngleSpeed() const;

	// 镜头角度
	void SetFov(float value);
	float GetFov() const;

	// 近剪裁面
	void SetNearPlane(float value);
	float GetNearPlane() const;

	// 远剪裁面
	void SetFarPlane(float value);
	float GetFarPlane() const;
	
	// 是否进行视锥剪裁
	void SetMakeClipPlane(bool value);
	bool GetMakeClipPlane() const;

	// 是否使用相对坐标
	void SetRelativeCoordinate(bool value);
	bool GetRelativeCoordinate() const;

	// 绑定对象
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const { return m_BindID; }
	
	// 绑定位置
	float GetBindPositionX() const { return m_vBindPos.x; }
	float GetBindPositionY() const { return m_vBindPos.y; }
	float GetBindPositionZ() const { return m_vBindPos.z; }
	
	// 连接到物体的辅助点上
	void LinkToPoint(const IVarList& args, IVarList& result);

	// 移动一段距离
	bool MoveLength(float len);
	// 左右平移
	bool MoveShift(float delta);
	// 上升或下降
	bool MoveUp(float delta);

	// 是否改变高度
	bool ChangeHeight(bool value);
	// 平移拖动
	bool DragMove(float drag_speed, float delta_x, float delta_y);

	// 旋转角度
	bool YawAngle(float angle);
	// 前后俯仰
	bool PitchAngle(float angle);
	// 左右偏转
	bool RollAngle(float angle);
	// 绑定到其他对象
	bool SetBindPosition(float x, float y, float z);
	// 3D空间点坐标转到屏幕2D坐标
	void GetScreenPos(const IVarList& args, IVarList& result);
	// 屏幕坐标转到3D拾取向量
	bool GetRayTrace(int screen_x, int screen_y, FmVec3* start, 
		FmVec3* direction);
	void GetTraceFromScreen(const IVarList& args, IVarList& result);

private:
	// 更新绑定位置
	bool UpdateBind();
	// 连接到高级模型的辅助点上
	bool LinkToModelPoint(const PERSISTID& id, const char* helper_name);
	// 连接到组合体的辅助点上
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
