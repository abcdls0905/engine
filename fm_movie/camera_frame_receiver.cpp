//--------------------------------------------------------------------
// �ļ���:		CameraFrameReceiver.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#include "camera_frame_receiver.h"
#include "camera_wrapper.h"
#include "utility.h"
#include "helper.h"

/// entity: CameraFrameReceiver
/// \brief �����֡���ݽ�����
/// \see �̳���IFrameReceiver
DECLARE_ENTITY("", CameraFrameReceiver, IFrameReceiver);
/// property: object CameraWrapper
/// \brief �����
DECLARE_PROPERTY(PERSISTID, CameraFrameReceiver, CameraWrapper, GetCameraWrapperID, SetCameraWrapperID);
/// property: bool CameraReversal
/// \brief ������Ƿ���Y����ת180��
DECLARE_PROPERTY(bool, CameraFrameReceiver, CameraReversal, GetCameraReversal, SetCameraReversal);
/// method: void SetCameraRefAngle(float rx, float ry, float rz);
/// \brief ��������������ĽǶ�
/// \param rx x���ת��
/// \param ry y���ת��
/// \param rz z���ת��
DECLARE_METHOD_VOID_3(CameraFrameReceiver, SetCameraRefAngle, float, float, float);
/// event: int on_camera_position(object self, object camera_wrapper, float px, float py, float pz)
/// \brief ��ɫ�����ƶ�
/// \param self ������
/// \param camera_wrapper ���������
/// \param px x����
/// \param py y����
/// \param pz z����
DECLARE_EVENT(on_camera_position);

// �����֡���ݱ��
//#define CAMERA_FRAME_FLAG 0xCAEA

#pragma pack(push, 1)

// �����֡����
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

	// ֻʹ����������,��������ͷ,����������
	char* pFrameBuffer = m_ProcessingFrameData.back() + 8;
	camera_frame_t frame = *((camera_frame_t*)pFrameBuffer);

	pWrapper->SetFovx(frame.fovx);

	// ��ȡ��ŷ������Ϣ
	FmQuat quat(0.7071f, 0, 0, 0.7071f);
	FmQuat quat_tmp(frame.quat_x, frame.quat_y, frame.quat_z, frame.quat_w);
	D3DXQuaternionMultiply(&quat, &quat, &quat_tmp);
	float ax, ay, az;
	QuaternionToPitchYawRoll(quat.x, quat.y, quat.z, quat.w,
		&ax, &ay, &az);

	if (m_bReversal)
	{
		// �����������ǿ��ѡ��180��ʹ����ģ�ͽǶȱ���һ��
		RotationPI(ay, frame.pos_x, frame.pos_y, frame.pos_z);
	}

	// �洢�������Ƕ���Ϣ
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

	// ���ݲ��ն�����½Ƕ�
	UpdateCameraAngle(m_matRef);

	// ���ݲ��ն�������
	FmVec3 pos(frame.pos_x, frame.pos_y, frame.pos_z);
	FmVec3TransformCoord(&pos, &pos, &m_matRef);

	// �����������
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

	// ��ȡ����ĽǶ�
	float ax, ay, az;
	ax = CHelper::QueryCustomFloat(pWrapper, "ax");
	ay = CHelper::QueryCustomFloat(pWrapper, "ay");
	az = CHelper::QueryCustomFloat(pWrapper, "az");

	// ������Ҫ���ӵľ���
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

		// �洢�������Ƕ���Ϣ
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

		// ������Ҫ���ӵľ���
		FmMat4 world_mat;
		FmMat4RotationYawPitchRoll(&world_mat, ay, ax, az);
		world_mat *= m_matRef;
		FmVec3 real_pos = pWrapper->GetPosition();
		world_mat._41 = real_pos.x;
		world_mat._42 = real_pos.y;
		world_mat._43 = real_pos.z;

		// ����������ĽǶ�
		pCamera->SetWorldMatrix(world_mat);

		// �����������
		FmVec3TransformCoord(&pos, &pos, &m_matRef);
		fm_movie::Util_RunCallback(this, "on_camera_position", CVarList()<<m_CameraWrapperID
			<<pos.x<<pos.y<<pos.z);

	}

}

bool CameraFrameReceiver::GetCameraReversal() const
{
	return m_bReversal;
}
