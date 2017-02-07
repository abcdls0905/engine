//--------------------------------------------------------------------
// 文件名:		CameraWrapper.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "camera_wrapper.h"
#include "time_axis.h"
#include "../visual/i_camera.h"
#include "helper.h"

/// \file CameraWrapper.cpp
/// \brief 摄像机包装类

/// entity: CameraWrapper
/// \brief 摄像机包装类
/// \see 继承自IVisBase
DECLARE_ENTITY(CameraWrapper, 0, IVisBase);
/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, CameraWrapper, Visible, GetVisible, SetVisible);
/// property: object Camera
/// \brief 摄像机
DECLARE_PROPERTY(PERSISTID, CameraWrapper, Camera, GetCameraID, SetCameraID);
/// property: object Target
/// \brief 目标
DECLARE_PROPERTY(PERSISTID, CameraWrapper, Target, GetTargetID, SetTargetID);
/// property: object DepthOfField
/// \brief 景深后处理对象
DECLARE_PROPERTY(PERSISTID, CameraWrapper, DepthOfField, GetDepthOfField, SetDepthOfField);

/// readonly: float Aspect
/// \brief 宽高比
DECLARE_PROPERTY(float, CameraWrapper, Aspect, GetAspect, SetAspect);
/// property: float Fovx
/// \brief 视野角度
DECLARE_PROPERTY(float, CameraWrapper, Fovx, GetFovx, SetFovx);
/// property: float Fovy
/// \brief 视野角度
DECLARE_PROPERTY(float, CameraWrapper, Fovy, GetFovy, SetFovy);
/// property: float FocusDepth
/// \brief 摄像机景深
DECLARE_PROPERTY(float, CameraWrapper, FocusDepth, GetFocusDepth, SetFocusDepth);

/// property: bool PyramidVisible
/// \brief 视锥体是否可见
DECLARE_PROPERTY(bool, CameraWrapper, PyramidVisible, GetPyramidVisible, SetPyramidVisible);

/// \brief 根据宽高比更新fovy
//DECLARE_METHOD_VOID_1(CameraWrapper, UpdateFovy, float);

CameraWrapper::CameraWrapper()
{
    m_pRender = NULL;

	m_fAspect = 1920.0f / 1080.0f;
	m_fFovx = FM_PIf * 0.5f;

	m_bVisible = true;
    m_bRotateAble = true;

	m_fRealAspect = m_fAspect;
	m_nUpdatePyramid = 10;

	m_pyramid.SetAspect(m_fAspect);
	m_pyramid.SetFovy(InnerGetFovyByFovx(m_fFovx, m_fAspect));
}

bool CameraWrapper::Init(const IVarList& args)
{
    m_pRender = (IRender*)GetCore()->GetInterface("Render");
    if (NULL == m_pRender)
    {
        return false;
    }

	return true;
}

bool CameraWrapper::Shut()
{
    SAFE_RELEASE(m_pRender);

	return true;
}

void CameraWrapper::Update(float seconds)
{
	UpdateTarget();
}

void CameraWrapper::Realize()
{
	if(!m_bVisible)
	{
		return;
	}

	m_pyramid.Realize(m_pRender);
}

bool CameraWrapper::GetLoadFinish()
{
	IVisBase *pCamera = GetVisBase(m_CameraID);
	return pCamera->GetLoadFinish();
}

void CameraWrapper::SetCameraID(const PERSISTID& value)
{
	m_CameraID = value;
	m_pyramid.SetCamera(value);

	UpdateDepth(m_pyramid.GetFocusDepth());
}

PERSISTID CameraWrapper::GetCameraID() const
{
	return m_CameraID;
}

void CameraWrapper::SetTargetID(const PERSISTID& value)
{
	IEntity* pTarget = (IEntity*)GetCore()->GetEntity(value);
	if (NULL == pTarget)
	{
		m_TargetID = PERSISTID();
		return;
	}

	if (!pTarget->GetEntInfo()->IsKindOf("IVisBase"))
	{
		m_TargetID = PERSISTID();
		return;
	}

	m_TargetID = value;

	UpdateTarget();
}

bool CameraWrapper::IsTarget() const
{
	return GetVisBase(m_TargetID) != NULL;
}

PERSISTID CameraWrapper::GetTargetID() const
{
	return m_TargetID;
}

float CameraWrapper::InnerGetFovyByFovx(float fovx, float aspect) const
{
	if (fovx >= FM_PIf || fovx <= 0)
	{
		return FM_PIf * 0.5f;
	}

	float fovx_half = fovx * 0.5f;
	float fovx_half_tan = tanf(fovx_half);
	float fovy_half_tan = fovx_half_tan / aspect;
	return atanf(fovy_half_tan) * 2;
}

float CameraWrapper::InnerGetFovxByFovy(float fovy, float aspect) const
{
	if (fovy >= FM_PIf || fovy <= 0)
	{
		return FM_PIf * 0.5f;
	}

	float fovy_half = fovy * 0.5f;
	float fovy_half_tan = tanf(fovy_half);
	float fovx_half_tan = fovy_half_tan * aspect;
	return atanf(fovx_half_tan) * 2;
}

void CameraWrapper::SetFovx(float value)
{
	m_fFovx = value;

	m_pyramid.SetFovy(InnerGetFovyByFovx(m_fFovx, m_fAspect));

	UpdateFovy(m_fRealAspect);
}

float CameraWrapper::GetFovx() const
{
	return m_fFovx;
}

void CameraWrapper::SetFovy(float value)
{
	SetFovx(InnerGetFovxByFovy(value, m_fRealAspect));
}

float CameraWrapper::GetFovy() const
{
	return InnerGetFovyByFovx(m_fFovx, m_fRealAspect);
}

void CameraWrapper::SetAspect(float value)
{
	m_fAspect = value;
	m_pyramid.SetAspect(value);

}

float CameraWrapper::GetAspect() const
{
	return m_fAspect;
}

void CameraWrapper::SetFocusDepth(float value)
{
	m_pyramid.SetFocusDepth(value);
}

float CameraWrapper::GetFocusDepth() const
{
	return m_pyramid.GetFocusDepth();
}

IVisBase* CameraWrapper::GetVisBase(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return NULL;
	}

	return (IVisBase*)pEntity;
}

ICamera* CameraWrapper::GetRealCamera(const PERSISTID& id) const
{
	IEntity* pEntity = GetEntity(id);
	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Camera"))
	{
		return NULL;
	}
	return (ICamera*)pEntity;
}

inline void fx_quaternion_from_rot_matrix(FmQuat *quat,
										  const FmMat4* kRot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float fTrace = kRot->m[0][0]+kRot->m[1][1]+kRot->m[2][2];
	float fRoot;

	if ( fTrace > 0.0 )
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0f);  // 2w
		quat->w = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;  // 1/(4w)
		quat->x = (kRot->m[2][1]-kRot->m[1][2])*fRoot;
		quat->y = (kRot->m[0][2]-kRot->m[2][0])*fRoot;
		quat->z = (kRot->m[1][0]-kRot->m[0][1])*fRoot;
	}
	else
	{
		// |w| <= 1/2
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;
		if ( kRot->m[1][1] > kRot->m[0][0] )
			i = 1;
		if ( kRot->m[2][2] > kRot->m[i][i] )
			i = 2;
		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		fRoot = sqrt(kRot->m[i][i]-kRot->m[j][j]-kRot->m[k][k] + 1.0f);
		float* apkQuat[3] = { &quat->x, &quat->y, &quat->z };
		*apkQuat[i] = 0.5f*fRoot;
		fRoot = 0.5f/fRoot;
		quat->w = (kRot->m[k][j]-kRot->m[j][k])*fRoot;
		*apkQuat[j] = (kRot->m[j][i]+kRot->m[i][j])*fRoot;
		*apkQuat[k] = (kRot->m[k][i]+kRot->m[i][k])*fRoot;
	}
}

inline void fx_quaternion_from_axes(FmQuat *quat, 
									const FmVec3* x, const FmVec3* y, const FmVec3* z)
{
	FmMat4 kRot;

	kRot.m[0][0] = x->x;
	kRot.m[1][0] = x->y;
	kRot.m[2][0] = x->z;

	kRot.m[0][1] = y->x;
	kRot.m[1][1] = y->y;
	kRot.m[2][1] = y->z;

	kRot.m[0][2] = z->x;
	kRot.m[1][2] = z->y;
	kRot.m[2][2] = z->z;

	fx_quaternion_from_rot_matrix(quat, &kRot);
}

inline FmVec3* GetFaceAngle(FmVec3* pOut, FmVec3* pEye, FmVec3* pAt, FmVec3* pUp)
{
	FmVec3 up;
	if (pUp)
	{
		up = *pUp;
	}
	else
	{
		up = FmVec3(0.0f, 1.0f, 0.0f);
	}

	// 朝向向量
	FmVec3 v = *pAt - *pEye;
	FmVec3Normalize(&v, &v);

	// 右方向向量
	FmVec3Cross(pOut, &up, &v);
	FmVec3Normalize(pOut, pOut);

	// 向上的向量
	FmVec3Cross(&up, &v, pOut);
	FmVec3Normalize(&up, &up);

	FmQuat quater_rotate;
	fx_quaternion_from_axes(&quater_rotate, pOut, &up, &v);
	VisUtil_QuaternionToPitchYawRoll(&quater_rotate, &pOut->x, 
		&pOut->y, &pOut->z);

	return pOut;
}

bool CameraWrapper::UpdateTarget()
{
	IVisBase* pTarget = GetVisBase(m_TargetID);
	if (NULL == pTarget)
	{
		// 没有目标
		return true;
	}

	ICamera* pCamera = GetRealCamera(m_CameraID);
	
	if (NULL == pCamera)
	{
		// 还没设置对象
		return true;
	}

	FmVec3 obj_pos = pCamera->GetPosition();
	FmVec3 target_pos = pTarget->GetPosition();
    FmVec3 dir = obj_pos - target_pos;
	float len = FmVec3Length(&dir);
	if (FloatEqual(m_pyramid.GetFocusDepth(), len))
	{
		return true;
	}

	// 摄像机到目标对象的距离
	m_pyramid.SetFocusDepth(len);

	if (m_nUpdatePyramid++ < 5)
	{
		// 当在执行UpdateTargetCameraKey更新关键数据时, 停止下面的执行.
		return true;
	}

	FmVec3 angle;
    FmVec3 up(0, 1, 0);
	GetFaceAngle(&angle, &obj_pos, &target_pos, &up);
    FmVec3 oldAngle = pCamera->GetAngle();
	pCamera->SetAngle(angle.x, angle.y, oldAngle.z);

	return true;
}

void CameraWrapper::UpdateTargetCameraKey(FmVec3 *pPos, FmVec3 *pAngle,
					const sObjectData &object_data, bool b_need_rotation)
{
	if (NULL == pPos || NULL == pAngle)
	{
		return;
	}

	IVisBase *pTarget = GetVisBase(m_TargetID);
	if (NULL == pTarget)
	{
		return;
	}

	FmVec3 angle;
	FmVec3 target_pos = pTarget->GetPosition();
	FmVec3 origin(object_data.fOriginX, object_data.fOriginY, object_data.fOriginZ);
	// 不需要旋转轨迹处理
	if (!b_need_rotation)
	{
		*pPos += origin;
        
        FmVec3 up(0, 1, 0);
		GetFaceAngle(&angle, pPos, &target_pos, &up);
		pAngle->x = angle.x;
		pAngle->y = angle.y;
		return;
	}

	// 参照相对目标作旋转处理.
	FmMat4	mat_ref;
	FmMatrixIdentity(&mat_ref);
	FmMat4RotationYawPitchRoll(&mat_ref, object_data.fRotationY, 
		object_data.fRotationX, object_data.fRotationZ);

	// 如果是max导入的轨迹,增加y轴180旋转.
	FmMat4 mat;
	if(0.f != object_data.fRAddY)
	{	
		FmMatrixIdentity(&mat);
		FmMatrixRotationY(&mat, object_data.fRAddY);
		mat_ref = mat * mat_ref; 	
	}
	
	// 计算旋转变换的逆矩阵,不含位置信息
	FmMat4 mat_inverse;
	FmMatrixIdentity(&mat_inverse);
	FmMatrixInverse(&mat_inverse, &mat_ref);

	// 将目标的位置还原,求出旋转之前的摄像机的角度	
	target_pos -= origin;
	FmVec3TransformCoord(&target_pos, &target_pos, &mat_inverse);	//目标的变换矩阵和摄像机的相同.
    FmVec3 vUp(0, 1, 0);
	GetFaceAngle(&angle, pPos, &target_pos, &vUp);
	pAngle->x = angle.x;
	pAngle->y = angle.y;

	// 计算旋转后的位移.
	FmVec3TransformCoord(pPos, pPos, &mat_ref);
	//*pPos += origin;

	// 计算旋转后的角度.
	FmMatrixIdentity(&mat);
	FmMat4RotationYawPitchRoll(&mat, pAngle->y, pAngle->x, pAngle->z);
	mat_ref = mat * mat_ref;  
	VisUtil_GetMatrixAngle(pAngle, &mat_ref);

	// 屏蔽wrapper自更新
	m_nUpdatePyramid = 0;
}

bool CameraWrapper::SetPosition(float x, float y, float z)
{
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return false;
	}
	return pCamera->SetPosition(x, y, z);
}

void CameraWrapper::SetWorldMatrix(const FmMat4& mat)
{
	SetPosition(mat._41, mat._42, mat._43);

	FmVec3 vAngle;
	VisUtil_GetMatrixAngle(&vAngle, &mat);
	SetAngle(vAngle.x, vAngle.y, vAngle.z);
}

//void LightSource::GetWorldMatrix(FmMat4& mat)
//{
//
//}

FmVec3 CameraWrapper::GetPosition() const
{
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return FmVec3(0.0f, 0.0f, 0.0f);
	}
	return pCamera->GetPosition();
}

bool CameraWrapper::SetAngle(float x, float y, float z)
{
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return false;
	}
	return pCamera->SetAngle(x, y, z);
}

FmVec3 CameraWrapper::GetAngle() const
{
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return FmVec3(0.0f, 0.0f, 0.0f);
	}
	return pCamera->GetAngle();
}


bool CameraWrapper::SetScale(float x, float y, float z)
{
    /*
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return false;
	}
	return pCamera->SetScale(x, y, z);
    */
    return false;
}

FmVec3 CameraWrapper::GetScale() const
{
    /*
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (NULL == pCamera)
	{
		return FmVec3(0.0f, 0.0f, 0.0f);
	}
    return pCamera->GetScale();
    */
    return FmVec3(0.0f, 0.0f, 0.0f);
}

void CameraWrapper::SetVisible(bool value)
{
    /*
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (pCamera)
	{
		pCamera->SetVisible(value);
	}
    */

	m_bVisible = value;

	IVisBase* pTarget = GetVisBase(m_TargetID);
	if (pTarget)
	{
		pTarget->SetVisible(value);
	}
}

bool CameraWrapper::GetVisible() const
{
    /*
	ICamera* pCamera = GetRealCamera(m_CameraID);
	if (pCamera)
	{
		return pCamera->GetVisible();
	}
    */
	return m_bVisible;
}

void CameraWrapper::SetPyramidVisible(bool value)
{
	m_pyramid.SetShowPyramid(value);
}

bool CameraWrapper::GetPyramidVisible() const
{
	return m_pyramid.GetShowPyramid();
}


void CameraWrapper::SetDepthOfFieldID(const PERSISTID& value)
{
	m_DepthOfField = value;
}

PERSISTID CameraWrapper::GetDepthOfFieldID() const
{
	return m_DepthOfField;
}


void CameraWrapper::UpdateFovy(float aspect)
{
	m_fRealAspect = aspect;

	// 只有第一人称摄像机才需要实时根据窗口宽高比更新fovy
	ICamera* pRealCamera = GetRealCamera(m_CameraID);
	if (NULL == pRealCamera)
	{
		return;
	}

	float real_fovy = InnerGetFovyByFovx(m_fFovx, m_fRealAspect);

	CHelper::SetPropertyFloat(pRealCamera, "Fov", real_fovy);
}


bool CameraWrapper::UpdateDepth(float depth)
{
	//IVisBase* pRealCamera = GetRealCamera(m_CameraID);
	//if (pRealCamera)
	//{
	//	IVisBase* pEntity = GetVisBase(m_DepthOfField);
	//	if (NULL == pEntity)
	//	{
	//		return false;
	//	}

	//	if (!pEntity->GetEntInfo()->IsKindOf("PPDepthofField"))
	//	{
	//		return false;
	//	}

	//	CHelper::SetPropertyFloat(pEntity, "FocusDepth", depth);

	//	return true;
	//}

	return false;
}
