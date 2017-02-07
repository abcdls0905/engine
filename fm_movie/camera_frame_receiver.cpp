//--------------------------------------------------------------------
// 文件名:		CameraFrameReceiver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "camera_frame_receiver.h"
#include "camera_wrapper.h"
#include "utility.h"
#include "helper.h"

/// entity: CameraFrameReceiver
/// \brief 摄像机帧数据接收器
/// \see 继承自IFrameReceiver
DECLARE_ENTITY("", CameraFrameReceiver, IFrameReceiver);
/// property: object CameraWrapper
/// \brief 摄像机
DECLARE_PROPERTY(PERSISTID, CameraFrameReceiver, CameraWrapper, GetCameraWrapperID, SetCameraWrapperID);
/// property: bool CameraReversal
/// \brief 摄像机是否绕Y轴旋转180°
DECLARE_PROPERTY(bool, CameraFrameReceiver, CameraReversal, GetCameraReversal, SetCameraReversal);
/// method: void SetCameraRefAngle(float rx, float ry, float rz);
/// \brief 设置摄像机相对物的角度
/// \param rx x轴的转角
/// \param ry y轴的转角
/// \param rz z轴的转角
DECLARE_METHOD_VOID_3(CameraFrameReceiver, SetCameraRefAngle, float, float, float);
/// event: int on_camera_position(object self, object camera_wrapper, float px, float py, float pz)
/// \brief 角色坐标移动
/// \param self 本对象
/// \param camera_wrapper 摄像机对象
/// \param px x坐标
/// \param py y坐标
/// \param pz z坐标
DECLARE_EVENT(on_camera_position);

// 摄像机帧数据标记
//#define CAMERA_FRAME_FLAG 0xCAEA

#pragma pack(push, 1)

// 摄像机帧数据
struct camera_frame_t
{
	//unsigned int flag;		// CAMERA_FRAME_FLAG

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

CameraFrameReceiver::CameraFrameReceiver()
{
	SetFrameSize(sizeof(camera_frame_t) + 8);
	FmMatrixIdentity(&m_matRef);
	m_bReversal = false;
}

CameraWrapper* CameraFrameReceiver::GetCameraWrapper(const PERSISTID& id)
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

void CameraFrameReceiver::ProcessFrameData()
{
	if (m_ProcessingFrameData.empty())
	{
		Assert(0);
		return;
	}

	CameraWrapper* pWrapper = GetCameraWrapper(m_CameraWrapperID);
	if (NULL == pWrapper)
	{
		//Assert(0);
		return;
	}

	// 只使用最新数据,跳过报文头,和数量数据
	char* pFrameBuffer = m_ProcessingFrameData.back() + 8;
	camera_frame_t frame = *((camera_frame_t*)pFrameBuffer);

	pWrapper->SetFovx(frame.fovx);

	// 提取出欧拉角信息
	FmQuat quat(0.7071f, 0, 0, 0.7071f);
	FmQuat quat_tmp(frame.quat_x, frame.quat_y, frame.quat_z, frame.quat_w);
	D3DXQuaternionMultiply(&quat, &quat, &quat_tmp);
	float ax, ay, az;
	QuaternionToPitchYawRoll(quat.x, quat.y, quat.z, quat.w,
		&ax, &ay, &az);

	if (m_bReversal)
	{
		// 将摄像机数据强行选择180，使其与模型角度保持一致
		RotationPI(ay, frame.pos_x, frame.pos_y, frame.pos_z);
	}

	// 存储相对坐标角度信息
	IVarTable *pVarTable = pWrapper->GetCustoms();
	if (pVarTable)
	{
		pVarTable->Assign("px", CVar(VTYPE_FLOAT, frame.pos_x));
		pVarTable->Assign("py", CVar(VTYPE_FLOAT, frame.pos_y));
		pVarTable->Assign("pz", CVar(VTYPE_FLOAT, frame.pos_z));
		pVarTable->Assign("ax", CVar(VTYPE_FLOAT, ax));
		pVarTable->Assign("ay", CVar(VTYPE_FLOAT, ay));
		pVarTable->Assign("az", CVar(VTYPE_FLOAT, az));
	}

	// 根据参照对象更新角度
	UpdateCameraAngle(m_matRef);

	// 根据参照对象坐标
	FmVec3 pos(frame.pos_x, frame.pos_y, frame.pos_z);
	FmVec3TransformCoord(&pos, &pos, &m_matRef);

	// 发送相对坐标
	fm_movie::Util_RunCallback(this, "on_camera_position", CVarList()<<m_CameraWrapperID
		<<pos.x<<pos.y<<pos.z);
}

void CameraFrameReceiver::SetCameraWrapperID(const PERSISTID& value)
{
	m_CameraWrapperID = value;
}

PERSISTID CameraFrameReceiver::GetCameraWrapperID() const
{
	return m_CameraWrapperID;
}

void CameraFrameReceiver::SetCameraRefAngle(float x, float y, float z)
{
	FmMat4RotationYawPitchRoll(&m_matRef, y, x, z);
	UpdateCameraAngle(m_matRef);
}

void CameraFrameReceiver::UpdateCameraAngle(const FmMat4 &mat)
{
	CameraWrapper* pWrapper = GetCameraWrapper(m_CameraWrapperID);
	if (NULL == pWrapper)
	{
		//Assert(0);
		return;
	}

	PERSISTID cameraID = pWrapper->GetCameraID();
	IVisBase* pCamera = (IVisBase*)GetCore()->GetEntity(cameraID);
	if (NULL == pCamera)
	{
		return;
	}

	// 获取自身的角度
	float ax, ay, az;
	ax = CHelper::QueryCustomFloat(pWrapper, "ax");
	ay = CHelper::QueryCustomFloat(pWrapper, "ay");
	az = CHelper::QueryCustomFloat(pWrapper, "az");

	// 计算需要叠加的矩阵
	FmMat4 world_mat;
	FmMat4RotationYawPitchRoll(&world_mat, ay, ax, az);
	world_mat *= mat;

	FmVec3 pos = pWrapper->GetPosition();
	world_mat._41 = pos.x;
	world_mat._42 = pos.y;
	world_mat._43 = pos.z;

	pCamera->SetWorldMatrix(world_mat);
}


void CameraFrameReceiver::RotationPI(float &angleY, float &x, float &y, float &z)
{
	angleY += FM_PIf;
	FmMat4 mtxRotate180;
	FmVec3 pos(x, y, z);
	FmMatrixRotationY(&mtxRotate180, FM_PIf);
	FmVec3TransformCoord(&pos, &pos, &mtxRotate180);
	x = pos.x;
	y = pos.y;
	z = pos.z;
}


void CameraFrameReceiver::SetCameraReversal(bool reversal)
{

	if (reversal != m_bReversal)
	{
		m_bReversal = reversal;

		CameraWrapper* pWrapper = GetCameraWrapper(m_CameraWrapperID);
		if (NULL == pWrapper)
		{
			//Assert(0);
			return;
		}

		PERSISTID cameraID = pWrapper->GetCameraID();
		IVisBase* pCamera = (IVisBase*)GetCore()->GetEntity(cameraID);
		if (NULL == pCamera)
		{
			return;
		}

		FmVec3 pos;
		float ax, ay, az;
		pos.x = CHelper::QueryCustomFloat(pWrapper, "px");
		pos.y = CHelper::QueryCustomFloat(pWrapper, "py");
		pos.z = CHelper::QueryCustomFloat(pWrapper, "pz");
		ax = CHelper::QueryCustomFloat(pWrapper, "ax");
		ay = CHelper::QueryCustomFloat(pWrapper, "ay");
		az = CHelper::QueryCustomFloat(pWrapper, "az");

		if (reversal)
		{
			ay += FM_PIf;
			FmMat4 mtxRotate180;
			FmMatrixRotationY(&mtxRotate180, +FM_PIf);
			FmVec3TransformCoord(&pos, &pos, &mtxRotate180);
		}
		else
		{
			ay -= FM_PIf;
			FmMat4 mtxRotate180;
			FmMatrixRotationY(&mtxRotate180, -FM_PIf);
			FmVec3TransformCoord(&pos, &pos, &mtxRotate180);
		}

		// 存储相对坐标角度信息
		IVarTable *pVarTable = pWrapper->GetCustoms();
		if (pVarTable)
		{
			pVarTable->Assign("px", CVar(VTYPE_FLOAT, pos.x));
			pVarTable->Assign("py", CVar(VTYPE_FLOAT, pos.y));
			pVarTable->Assign("pz", CVar(VTYPE_FLOAT, pos.z));
			pVarTable->Assign("ax", CVar(VTYPE_FLOAT, ax));
			pVarTable->Assign("ay", CVar(VTYPE_FLOAT, ay));
			pVarTable->Assign("az", CVar(VTYPE_FLOAT, az));
		}

		// 计算需要叠加的矩阵
		FmMat4 world_mat;
		FmMat4RotationYawPitchRoll(&world_mat, ay, ax, az);
		world_mat *= m_matRef;
		FmVec3 real_pos = pWrapper->GetPosition();
		world_mat._41 = real_pos.x;
		world_mat._42 = real_pos.y;
		world_mat._43 = real_pos.z;

		// 设置摄像机的角度
		pCamera->SetWorldMatrix(world_mat);

		// 发送相对坐标
		FmVec3TransformCoord(&pos, &pos, &m_matRef);
		fm_movie::Util_RunCallback(this, "on_camera_position", CVarList()<<m_CameraWrapperID
			<<pos.x<<pos.y<<pos.z);

	}

}

bool CameraFrameReceiver::GetCameraReversal() const
{
	return m_bReversal;
}
