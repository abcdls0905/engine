//--------------------------------------------------------------------
// 文件名:		VirtualCamera.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "virtual_camera.h"
#include "camera_wrapper.h"
#include "../public/core_log.h"

/// \file VirtualCamera.cpp
/// \brief 动作捕捉软件MVN和游戏交互实体

/// entity: VirtualCamera
/// \brief 动作捕捉软件MVN和游戏交互实体
/// \see 继承自IVisBase
DECLARE_ENTITY("", VirtualCamera, IVisBase);
/// property: object CameraWrapper
/// \brief 摄像机
DECLARE_PROPERTY(PERSISTID, VirtualCamera, CameraWrapper, GetCameraWrapperID, SetCameraWrapperID);
/// method: bool StartListen(const char* szMyIP, unsigned short nPort);
/// \brief 开始监听MVN发来的数据
/// \param szMyIP 本地IP
/// \param nPort 监听的端口
DECLARE_METHOD_2(bool, VirtualCamera, StartListen, const char*, unsigned short);
/// method: bool StopListen();
/// \brief 停止监听
DECLARE_METHOD_0(bool, VirtualCamera, StopListen);

// 摄像机帧数据标记
#define CAMERA_FRAME_FLAG 0xCAEA

#pragma pack(push, 1)

// 摄像机帧数据
struct sCameraFrame
{
	unsigned int flag;		// CAMERA_FRAME_FLAG

	float pos_x;
	float pos_y;
	float pos_z;
	float quat_x;
	float quat_y;
	float quat_z;
	float quat_w;

	float fovx;
	float aspect;
	float dof;
};

#pragma pack(pop)

inline void QuaternionToPitchYawRoll(float qx, float qy, float qz, float qw,
										float* fPitch, float* fYaw, float* fRoll)
{
	FLOAT sp = -2.0F * (qy * qz - qw * qx);

	if ((sp > 0.9999f) || (sp < -0.9999f))
	{
		*fPitch = 3.1415926F * 0.5F * sp;
		*fYaw = atan2(-qx * qz + qw * qy, 
			0.5F - qy * qy - qz * qz);
		*fRoll = 0.0F;
	}
	else
	{
		*fPitch	= asin(sp);
		*fYaw = atan2(qx * qz + qw * qy, 
			0.5F - qx * qx - qy * qy);
		*fRoll = atan2(qx * qy + qw * qz, 
			0.5F - qx * qx - qz * qz);
	}
}


VirtualCamera::VirtualCamera()
{ 
	SetFrameSize(sizeof(sCameraFrame));
}

CameraWrapper* VirtualCamera::GetCameraWrapper(const PERSISTID& id)
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("CameraWrapper"))
	{
		return NULL;
	}

	return (CameraWrapper*)pEntity;
}

void VirtualCamera::ProcessFrameData(char* pFrameBuffer)
{
	if (NULL == pFrameBuffer)
	{
		Assert(0);
		return;
	}

	CameraWrapper* pCamera = GetCameraWrapper(m_CameraWrapperID);
	if (NULL == pCamera)
	{
		Assert(0);
		return;
	}

	sCameraFrame frame = *((sCameraFrame*)pFrameBuffer);
	//frame.flag = ntohl(frame.flag);
	if (frame.flag != CAMERA_FRAME_FLAG)
	{
		//Assert(0);
		return;
	}

	//static char buf[2000];
	//SafeSprintf(buf, 2000, "pos x:%f, z:%f, y:%f", frame.pos_x, frame.pos_z, frame.pos_y);
	//CORE_TRACE(buf);
	//pCamera->SetPosition(frame.pos_x, frame.pos_y, frame.pos_z);
	pCamera->SetPosition(frame.pos_x, frame.pos_y, frame.pos_z);

	FmQuat quat(0.7071f, 0, 0, 0.7071f);
	FmQuat quat_tmp(frame.quat_x, frame.quat_y, frame.quat_z, frame.quat_w);
	D3DXQuaternionMultiply(&quat, &quat, &quat_tmp);

	float ax, ay, az;
	QuaternionToPitchYawRoll(quat.x, quat.y, quat.z, quat.w,
		&ax, &ay, &az);
	pCamera->SetAngle(ax/* + FM_PIf * 0.5f*/, ay, az);

	pCamera->SetFovx(frame.fovx);
	pCamera->SetAspect(frame.aspect);
}

void VirtualCamera::SetCameraWrapperID(const PERSISTID& value)
{
	m_CameraWrapperID = value;
}

PERSISTID VirtualCamera::GetCameraWrapperID() const
{
	return m_CameraWrapperID;
}
