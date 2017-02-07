//--------------------------------------------------------------------
// 文件名:		camera.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "camera.h"
#include "scene.h"
#include "render_tex.h"
#include "../visual/vis_utils.h"
//#include "../visual/i_model.h"
#include "../visual/i_actor.h"
//#include "../visual/i_model_player.h"
#include "../public/var_list.h"
#include "../public/core_log.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"

/// \file camera.cpp
/// \brief 摄像机

/// entity: Camera
/// \brief 摄像机实体
/// \see 继承自IEntity
DECLARE_ENTITY(Camera, 0, IEntity);

/// property: object Bind
/// \brief 当前绑定的对象
DECLARE_PROPERTY(PERSISTID, Camera, Bind, GetBindID, SetBindID);

/// readonly: float BindPositionX
/// \brief 当前绑定的相对位置坐标X
DECLARE_PROPERTY_GET(float, Camera, BindPositionX, GetBindPositionX);
/// readonly: float BindPositionY
/// \brief 当前绑定的相对位置坐标Y
DECLARE_PROPERTY_GET(float, Camera, BindPositionY, GetBindPositionY);
/// readonly: float BindPositionZ
/// \brief 当前绑定的相对位置坐标Z
DECLARE_PROPERTY_GET(float, Camera, BindPositionZ, GetBindPositionZ);

/// property: float Fov
/// \brief 视野角度
DECLARE_PROPERTY(float, Camera, Fov, GetFov, SetFov);
/// property: float NearPlane
/// \brief 近剪裁面
DECLARE_PROPERTY(float, Camera, NearPlane, GetNearPlane, SetNearPlane);
/// property: float FarPlane
/// \brief 远剪裁面
DECLARE_PROPERTY(float, Camera, FarPlane, GetFarPlane, SetFarPlane);
/// property: bool MakeClipPlane
/// \brief 是否要进行视锥剪裁
DECLARE_PROPERTY(bool, Camera, MakeClipPlane, GetMakeClipPlane, 
	SetMakeClipPlane);
/// property: RelativeCoordinate
/// \brief 是否使用相对坐标
DECLARE_PROPERTY(bool, Camera, RelativeCoordinate, GetRelativeCoordinate, 
	SetRelativeCoordinate);

// Camera

Camera::Camera()
{
	m_pContext = NULL;
	m_pSceneView = NULL;
	m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_vAngle = FmVec3(0.0F, 0.0F, 0.0F);
	m_fFov = FLOAT_PI / 4;
	m_fNearPlane = 0.1F;
	m_fFarPlane = 1000.0F;
	m_bMakeClipPlane = true;
	m_bRelativeCoordinate = true;
	m_vBindPos = FmVec3(0.0F, 0.0F, 0.0F);
	m_nHelperID = -1;
	m_bToChangeHeight = true;
}

Camera::~Camera()
{
}

bool Camera::Init(const IVarList& args)
{
	PERSISTID id = args.ObjectVal(0);
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		CORE_TRACE("(Camera::Init)no entity");
		return false;
	}

	if (pEntity->GetEntInfo()->IsKindOf("Scene"))
	{
		Scene* pScene = (Scene*)pEntity;
		
		m_pContext = pScene->GetContext();
		m_pSceneView = pScene->GetSceneView();
	}
	else if (pEntity->GetEntInfo()->IsKindOf("RenderTex"))
	{
		RenderTex* pRenderTex = (RenderTex*)pEntity;

		m_pContext = pRenderTex->GetContext();
		m_pSceneView = pRenderTex->GetSceneView();
	}
	else
	{
		CORE_TRACE("(Camera::Init)entity type error");
		return false;
	}

	return true;
}

bool Camera::Shut()
{
	//SAFE_RELEASE(m_pRender);

	return true;
}


void Camera::Realize()
{
	UpdateBind();

	// 加上预测的偏移值
	FmVec3 position = m_vPosition;
	FmVec3 angle = m_vAngle;

	if (m_bRelativeCoordinate)
	{
		// 设置参考位置
		int refer_x = int(position.x) / 32 * 32;
		int refer_y = 0;
		int refer_z = int(position.z) / 32 * 32;

		m_pContext->SetReferPosition(float(refer_x), float(refer_y), 
			float(refer_z));
	}
	else
	{
		m_pContext->SetReferPosition(0.0F, 0.0F, 0.0F);
	}

	// 获得当前视口宽高比例
	int view_width = m_pContext->GetViewWidth();
	int view_height = m_pContext->GetViewHeight();
	float ratio = float(view_width) / float(view_height);

	m_pContext->SetCameraView(position, angle);
	m_pContext->SetCameraProj(m_fFov, m_fNearPlane, m_fFarPlane, ratio);
	m_pContext->MakeHelperMatrix();

	if (m_bMakeClipPlane)
	{
		m_pContext->MakeClipPlane();
	}
}

bool Camera::SetPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;

	return true;
}

FmVec3 Camera::GetPosition() const
{ 
	return m_vPosition; 
}

bool Camera::SetAngle(float x, float y, float z)
{
	m_vAngle.x = x;
	m_vAngle.y = y;
	m_vAngle.z = z;

	return true;
}

FmVec3 Camera::GetAngle() const
{ 
	return m_vAngle; 
}


bool Camera::SetVelocity(float x, float y, float z)
{
	m_vVelocity.x = x;
	m_vVelocity.y = y;
	m_vVelocity.z = z;

	return true;
}

FmVec3 Camera::GetVelocity() const
{
	return m_vVelocity;
}

bool Camera::SetAngleSpeed(float x, float y, float z)
{
	m_vAngleSpeed.x = x;
	m_vAngleSpeed.y = y;
	m_vAngleSpeed.z = z;

	return true;
}

FmVec3 Camera::GetAngleSpeed() const
{
	return m_vAngleSpeed;
}

void Camera::SetFov(float value)
{
	m_fFov = value;
}

float Camera::GetFov() const
{
	return m_fFov;
}

void Camera::SetNearPlane(float value)
{
	m_fNearPlane = value;
}

float Camera::GetNearPlane() const
{
	return m_fNearPlane;
}

void Camera::SetFarPlane(float value)
{
	m_fFarPlane = value;
}

float Camera::GetFarPlane() const
{
	return m_fFarPlane;
}

void Camera::SetMakeClipPlane(bool value)
{
	m_bMakeClipPlane = value;
}

bool Camera::GetMakeClipPlane() const
{
	return m_bMakeClipPlane;
}

void Camera::SetRelativeCoordinate(bool value)
{
	m_bRelativeCoordinate = value;
}

bool Camera::GetRelativeCoordinate() const
{
	return m_bRelativeCoordinate;
}

bool Camera::MoveLength(float len)
{
	// 垂直方向运动
	float v_delta = len * sinf(m_vAngle.x);

	if (m_bToChangeHeight)
	{
		m_vPosition.y -= v_delta;
	}
	// 水平方向运动
	float h_delta = len * cosf(m_vAngle.x);
	
	m_vPosition.x += h_delta * sinf(m_vAngle.y);
	m_vPosition.z += h_delta * cosf(m_vAngle.y);
	
	return true;
}

bool Camera::MoveShift(float delta)
{
	float angle = m_vAngle.y + FLOAT_PI * 0.5F;
	
	m_vPosition.x += delta * sinf(angle);
	m_vPosition.z += delta * cosf(angle);
	
	return true;
}

bool Camera::MoveUp(float delta)
{
	m_vPosition.y += delta;

	return true;
}

bool Camera::ChangeHeight(bool value)
{
	m_bToChangeHeight = value;

	return true;
}


bool Camera::DragMove(float drag_speed, float delta_x, float delta_y)
{
	FmMat4 mtx;
	FmMat4 mtxRotate;
	FmMat4 mtxTranslate;

	FmMat4RotationYawPitchRoll(&mtxRotate, m_vAngle.y, m_vAngle.x, 
		m_vAngle.z);
	FmMat4Translation(&mtxTranslate, m_vPosition.x, m_vPosition.y, 
		m_vPosition.z);
	FmMat4Multiply(&mtx, &mtxRotate, &mtxTranslate);
	FmMat4Inverse(&mtx, NULL, &mtx);

	FmVec3 up;

	up.x = mtx._12;
	up.y = mtx._22;
	up.z = mtx._32;

	FmVec3 pos = m_vPosition + up * (delta_y * drag_speed);
	FmVec3 right;

	right.x = mtx._11;
	right.y = mtx._21;
	right.z = mtx._31;
	pos += -right * (delta_x * drag_speed);

	return this->SetPosition(pos.x, pos.y, pos.z);
}

bool Camera::YawAngle(float angle)
{
	m_vAngle.y += angle;
	
	return true;
}

bool Camera::PitchAngle(float angle)
{
	m_vAngle.x += angle;
	
	return true;
}

bool Camera::RollAngle(float angle)
{
	m_vAngle.z += angle;

	return true;
}

bool Camera::UpdateBind() 
{ 
	if (m_BindID.IsNull())
	{
		return false;
	}
	
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_BindID);
	
	if (NULL == pVisBase)
	{
		return false;
	}

	/*
	if (pVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		if (m_nHelperID != -1)
		{
			IModel* pModel = (IModel*)pVisBase;
			IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

			Assert(NULL != pModelPlayer);

			FmMat4 mtxNode;
			FmMat4 mtxModel;

			pModelPlayer->GetLabelMatrix(m_nHelperID, mtxNode);
			pModel->GetWorldMatrix(mtxModel);

			FmMat4 mtxLink = mtxNode * mtxModel;
			FmVec3 pos(mtxLink._41, mtxLink._42, mtxLink._43);

			m_vPosition = pos;

			VisUtil_GetMatrixAngle(&m_vAngle, &mtxLink);
		}
		else
		{
			m_vPosition = pVisBase->GetPosition() + m_vBindPos;
		}

		return true;
	}
	else 
		*/
	
	if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		PERSISTID id = ((IActor*)pVisBase)->GetLinkObject(
			m_strNodeName.c_str());

		if (!id.IsNull())
		{
			IVisBase* pLinkVisual = (IVisBase*)GetCore()->GetEntity(id);

			if (NULL == pLinkVisual)
			{
				m_vPosition = pVisBase->GetPosition() + m_vBindPos;
				return false;
			}

			CVarList result_pos, result_angle;

			/*
			GetCore()->InvokeMethod(pLinkVisual, "GetHelperPosition", 
				CVarList() << "" << m_strHelperName.c_str(), result_pos);
			GetCore()->InvokeMethod(pLinkVisual, "GetHelperAngle", 
				CVarList() << "" << m_strHelperName.c_str(), result_angle);*/

			if (3 == result_pos.GetCount() && 3 == result_angle.GetCount())
			{
				m_vPosition.x = result_pos.FloatVal(0);
				m_vPosition.y = result_pos.FloatVal(1);
				m_vPosition.z = result_pos.FloatVal(2);

				m_vAngle.x = result_angle.FloatVal(0);
				m_vAngle.y = result_angle.FloatVal(1);
				m_vAngle.z = result_angle.FloatVal(2);
			}
			else
			{
				m_vPosition = pVisBase->GetPosition() + m_vBindPos;
			}
		}
		else
		{
			m_vPosition = pVisBase->GetPosition() + m_vBindPos;
		}

		return true;
	}


	return true;
}

void Camera::SetBindID(const PERSISTID& id)
{
	m_nHelperID = -1;

	if (id.IsNull())
	{
		m_BindID = PERSISTID();
		return;
	}
	
	IEntity* pEntity = (IEntity*)GetCore()->GetEntity(id);
	
	if (NULL == pEntity)
	{
		m_BindID = PERSISTID();
		return;
	}
	
	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		m_BindID = PERSISTID();
		return;
	}
	
	m_BindID = id;
	UpdateBind();
}

bool Camera::SetBindPosition(float x, float y, float z)
{
	m_vBindPos.x = x;
	m_vBindPos.y = y;
	m_vBindPos.z = z;
	
	UpdateBind();

	return true;
}

void Camera::LinkToPoint(const IVarList& args, IVarList& result)
{
	int args_count = (int)args.GetCount();

	if ((2 != args_count) && (3 != args_count))
	{
		result.AddBool(false);
		return;
	}

	if (2 == args_count)
	{
		if (VTYPE_OBJECT != args.GetType(0) || VTYPE_STRING != args.GetType(1))
		{
			result.AddBool(false);
			return;
		}
	}
	else if (3 == args_count)
	{
		if (VTYPE_OBJECT != args.GetType(0) || VTYPE_STRING != args.GetType(1) 
			|| VTYPE_STRING != args.GetType(2))
		{
			result.AddBool(false);
			return;
		}
	}

	const PERSISTID id = args.ObjectVal(0);
	IEntity* pLink = (IEntity*)GetCore()->GetEntity(id);

	if (NULL == pLink)
	{
		result.AddBool(false);
		return;
	}

	if (pLink->GetEntInfo()->IsKindOf("Model"))
	{
		const char* helper_name = args.StringVal(1);

		result.AddBool(LinkToModelPoint(id, helper_name));
	}
	else if (pLink->GetEntInfo()->IsKindOf("Actor"))
	{
		const char* helper_name = args.StringVal(1);
		const char* node_name = args.StringVal(2);

		result.AddBool(LinkToActorPoint(id, helper_name, node_name));
	}
	else
	{
		result.AddBool(false);
	}
}

bool Camera::LinkToModelPoint(const PERSISTID& id, const char* helper_name)
{
	/*
	m_strHelperName = "";
	m_strNodeName = "";

	IEntity* pLink = (IEntity*)GetCore()->GetEntity(id);

	if (NULL == pLink)
	{
		m_BindID = PERSISTID();
		m_nHelperID = -1;
		return false;
	}

	IModel* pModel = (IModel*)pLink;
	IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

	if (NULL == pModelPlayer)
	{
		m_BindID = PERSISTID();
		m_nHelperID = -1;
		CORE_TRACE("(Camera::LinkToModelPoint)model not valid");
		return false;
	}

	if (!pModelPlayer->IsReady())
	{
		m_BindID = PERSISTID();
		m_nHelperID = -1;
		// 模型未加载完成
		CORE_TRACE("(Camera::LinkToModelPoint)model not load complete");
		return false;
	}

	int helper_id = pModelPlayer->FindLabelNameIndex(helper_name);

	if (-1 == helper_id)
	{	
		m_BindID = PERSISTID();
		m_nHelperID = -1;
		CORE_TRACE("(Camera::LinkToModelPoint)Not find helper");
		CORE_TRACE(helper_name);
		CORE_TRACE(pModelPlayer->GetModelFileName());
		return false;
	}

	m_BindID = id;
	m_nHelperID = helper_id;
	*/

	return true;
}

bool Camera::LinkToActorPoint(const PERSISTID& id, const char* helper_name, 
	const char* node_name)
{
	m_nHelperID = -1;

	if (node_name[0] == 0)
	{
		m_BindID = PERSISTID();
		m_strHelperName = "";
		m_strNodeName = "";
		return false;
	}

	m_BindID = id;
	m_strHelperName = helper_name;
	m_strNodeName = node_name;

	return true;
}

void Camera::GetScreenPos(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	if (args.GetCount() < 3)
	{
		return;
	}

	if (NULL == m_pContext)
	{
		return;
	}

	float x = args.FloatVal(0);
	float y = args.FloatVal(1);
	float z = args.FloatVal(2);
	FmVec3 vTargetPos = FmVec3(x, y, z);
	int view_width = m_pContext->GetViewWidth();
	int view_height = m_pContext->GetViewHeight();
	const FmMat4& mtxViewProj = m_pContext->GetCamera().mtxViewProj;
	FmVec3 vScreenPos;

	// 转到标准屏幕坐标
	FmVec3TransformCoord(&vScreenPos, &vTargetPos, &mtxViewProj);
	// 根据视口转到实际屏幕坐标
	vScreenPos.x = (1.0F + vScreenPos.x) * view_width / 2.0F;
	vScreenPos.y = (1.0F - vScreenPos.y) * view_height / 2.0F;

	result.AddFloat(vScreenPos.x);
	result.AddFloat(vScreenPos.y);
	result.AddFloat(vScreenPos.z);
}

bool Camera::GetRayTrace(int screen_x, int screen_y, FmVec3* start, 
	FmVec3* direction)
{
	if (NULL == m_pContext)
	{
		return false;
	}

	int view_width = m_pContext->GetViewWidth();
	int view_height = m_pContext->GetViewHeight();
	const FmMat4& mtxProj = m_pContext->GetCamera().mtxProj;
	const FmMat4& m = m_pContext->GetCamera().mtxViewInverse;
	// Compute the vector of the pick ray in screen space
	FmVec3 v1;

	v1.x = (((2.0F * screen_x) / (float)view_width) - 1.0F) / mtxProj._11;
	v1.y = -(((2.0F * screen_y) / (float)view_height) - 1.0F) / mtxProj._22;
	v1.z = 1.0F;

	// Transform the screen space pick ray into 3D space
	direction->x = v1.x * m._11 + v1.y * m._21 + v1.z * m._31;
	direction->y = v1.x * m._12 + v1.y * m._22 + v1.z * m._32;
	direction->z = v1.x * m._13 + v1.y * m._23 + v1.z * m._33;
	FmVec3Normalize(direction, direction);

	start->x = m._41;
	start->y = m._42;
	start->z = m._43;

	return true;
}

void Camera::GetTraceFromScreen(const IVarList& args, IVarList& result)
{
	if (args.GetCount() < 2)
	{
		result.AddBool(false);
		return;
	}

	if (VTYPE_INT != args.GetType(0) || VTYPE_INT != args.GetType(1))
	{
		result.AddBool(false);
		return;
	}

	result.Clear();

	int screen_x = args.IntVal(0);
	int screen_y = args.IntVal(1);
	FmVec3 vPickRayDir;
	FmVec3 vPickRayOrig;

	if (!GetRayTrace(screen_x, screen_y, &vPickRayOrig, &vPickRayDir))
	{
		result.AddBool(false);
		return;
	}

	result.AddBool(true);
	result.AddFloat(vPickRayOrig.x);
	result.AddFloat(vPickRayOrig.y);
	result.AddFloat(vPickRayOrig.z);
	result.AddFloat(vPickRayDir.x);
	result.AddFloat(vPickRayDir.y);
	result.AddFloat(vPickRayDir.z);
}
