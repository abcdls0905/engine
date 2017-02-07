//--------------------------------------------------------------------
// 文件名:		actor_node.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月3日
// 创建人:		惠军超
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "actor_node.h"
#include "actor.h"
#include "model.h"
#include "skin.h"
#include "effect_model.h"
#include "broken_shadow.h"
#include "../visual/vis_utils.h"
#include "../visual/i_model_player.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

// ActorNode

static bool init_rotation_180(FmMat4* pOut)
{
	FmMatrixRotationY(pOut, 3.1415926f);
	return true;
}

static FmMat4 s_mtxRotation180;
static bool s_bInitRotation180 = init_rotation_180(&s_mtxRotation180);

static Skin* get_skin(Actor* pActor, const PERSISTID& id)
{
	Skin* pSkin = (Skin*)pActor->GetCore()->GetEntity(id);

	if (NULL == pSkin)
	{
		return NULL;
	}

	if (!pSkin->GetEntInfo()->IsKindOf("Skin"))
	{
		return NULL;
	}

	return pSkin;
}

CActorNode::CActorNode()
{
	m_pActor = NULL;
	m_nHash = 0;
	m_pParentNode = NULL;
	m_bAngleKeep = false;
	m_bEsseObj = true;
	m_bNodeVisible = true;
	m_nColor = 0xFFFFFFFF;
	FmMatrixIdentity(&m_LocalMatrix);
	FmMatrixIdentity(&m_mtxWorld);
}

CActorNode::~CActorNode()
{
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		CORE_DELETE(m_Children[i]);
	}
}

void CActorNode::ReleaseAll()
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->Release();
		m_VisualID = PERSISTID();
	}

	IVisBase* pBrokenShadow = (IVisBase*)g_pCore->GetEntity(m_BrokenShadow);

	if (pBrokenShadow)
	{
		pBrokenShadow->Release();
		m_BrokenShadow = PERSISTID();
	}	
	
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->ReleaseAll();
		CORE_DELETE(m_Children[i]);
	}

	m_Children.clear();
}

void CActorNode::SetName(const char* value)
{
	Assert(value != NULL);

	m_strName = value;
	m_nHash = GetHashValue(value);
}

void CActorNode::SetPoint(const char* value)
{
	Assert(value != NULL);

	m_strPoint = value;
}

void CActorNode::SetVisualID(const PERSISTID& value)
{
	if (!m_VisualID.IsNull())
	{
		IVisBase* pVisBase = GetVisBase();

		if (NULL == pVisBase)
		{
			ClearChild();
		}
	}

	m_VisualID = value;
}

bool CActorNode::ExistMainModel()
{
	Skin* pSkin = get_skin(m_pActor, m_VisualID);

	if (pSkin)
	{
		if (pSkin->ExistMainModel())
		{
			return true;
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->ExistMainModel())
		{
			return true;
		}
	}

	return false;
}

void CActorNode::SetTraceMainModel(bool value)
{
	Skin* pSkin = get_skin(m_pActor, m_VisualID);

	if (pSkin)
	{
		pSkin->SetTraceMainModel(value);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetTraceMainModel(value);
	}
}

bool CActorNode::GetTraceMainModel()
{
	Skin* pSkin = get_skin(m_pActor, m_VisualID);

	if (pSkin)
	{
		if (pSkin->GetTraceMainModel())
		{
			return true;
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->GetTraceMainModel())
		{
			return true;
		}
	}

	return false;
}

void CActorNode::SetShowBoundBox(bool value)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		if (pVisBase->GetEntInfo()->IsKindOf( "Model"))
		{
			((Model*)pVisBase)->SetShowBoundBox(value);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Skin"))
		{
			((Skin*)pVisBase)->SetShowBoundBox(value);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
		{
			((EffectModel*)pVisBase)->SetShowBoundBox(value);
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetShowBoundBox(value);
	}
}

void CActorNode::SetBrokenShadowID(const PERSISTID& value)
{
	m_BrokenShadow = value;
}

void CActorNode::GetVisualNodeList( IVarList& result )
{
	IVisBase* pIVisBase = GetVisBase();

	if (pIVisBase)
	{
		result.AddPointer(this);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->GetVisualNodeList(result);
	}
}

void CActorNode::GetVisualList(IVarList& result)
{
	if (!m_VisualID.IsNull())
	{
		result.AddObject(m_VisualID);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->GetVisualList(result);
	}
}

void CActorNode::SetNodeVisible(const char* name, bool value)
{
	if (m_strName == name)
	{
		m_bNodeVisible = value;
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetNodeVisible(name, value);
	}
}

bool CActorNode::GetNodeVisible(const char* name)
{
	CActorNode* pActorNode = GetNode(name);

	if (pActorNode)
	{
		return pActorNode->m_bNodeVisible;
	}
	else
	{
		return false;
	}
}

bool CActorNode::SetActionPlayer(IActionPlayer* ani_player)
{
	Skin* pSkin = get_skin(m_pActor, m_VisualID);

	if (pSkin)
	{
		//IModelPlayer* pModelPlayer = pSkin->GetModelPlayer();

		//if (pModelPlayer)
		//{
		//	pModelPlayer->SetActionPlayer(ani_player);
		//}

		pSkin->SetActionPlayer(ani_player);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetActionPlayer(ani_player);
	}

	return true;
}

void CActorNode::SetParentNode(CActorNode* pNode)
{
	m_pParentNode = pNode;
}

CActorNode* CActorNode::AddChild(const char* name, const char* point, 
	const PERSISTID& id)
{
	Assert(name != NULL);
	Assert(point != NULL);
	Assert(name[0] != 0);
	Assert(this->GetNode(name) == NULL);

//	CActorNode* node = GetNode(name); 
//
//	if (node)
//	{
//		return node;
//	}
	
	CActorNode* node = CORE_NEW(CActorNode);

	node->SetActor(m_pActor);
	node->SetName(name);
	node->SetPoint(point);
	node->SetVisualID(id);
	node->SetParentNode(this);

	m_Children.push_back(node);

	return node;
}

bool CActorNode::ClearChild()
{
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->ReleaseAll();
		CORE_DELETE(m_Children[i]);
	}
	
	m_Children.clear();

	return true;
}

bool CActorNode::DeleteChild(const char* pName)
{
	Assert(pName != NULL);

	for (int i = (int)m_Children.size() - 1; i >= 0; --i)
	{
		if (0 == strcmp(m_Children[i]->GetName(), pName))
		{
			CActorNode* pNode = m_Children[i];

			if (pNode)
			{
				m_Children.remove(i);
				pNode->ReleaseAll();
				CORE_DELETE(pNode);
			}
		}
	}

	return true;
}

size_t CActorNode::GetChildCount() const
{
	return m_Children.size();
}

size_t CActorNode::GetVisBaseCount() const
{
	size_t nRet = 0;

	for (size_t i = 0; i < m_Children.size(); i++)
	{
		nRet += m_Children[i]->GetVisBaseCount();
	}

	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		nRet++;
	}

	return nRet;
}

CActorNode* CActorNode::GetChildByIndex(size_t index) const
{
	Assert(index < m_Children.size());

	return m_Children[index];
}

/*
CActorNode* CActorNode::GetNode(const char* name)
{
	Assert(name != NULL);

	if (m_strName == name)
	{
		return this;
	}

	size_t child_num = m_Children.size();

	for (size_t i = 0; i < child_num; ++i)
	{
		CActorNode* pNode = m_Children[i]->GetNode(name);

		if (pNode != NULL)
		{
			return pNode;
		}
	}

	return NULL;
}
*/

CActorNode* CActorNode::GetNode(const char* name, const unsigned int hash)
{
	Assert(name != NULL);

	if (hash == m_nHash && m_strName == name)
	{
		return this;
	}

	size_t child_num = m_Children.size();

	for (size_t i = 0; i < child_num; ++i)
	{
		CActorNode* pNode = m_Children[i]->GetNode(name, hash);

		if (pNode != NULL)
		{
			return pNode;
		}
	}

	return NULL;
}

CActorNode* CActorNode::GetNode(const char* name)
{
	Assert(name != NULL);

	int hash = GetHashValue(name);

	return GetNode(name, hash);
}

void CActorNode::Update(float seconds)
{
	if (!m_VisualID.IsNull())
	{
		IVisBase* pVisBase = GetVisBase();

		if (pVisBase)
		{
			pVisBase->Update(seconds);

			if (!m_BrokenShadow.IsNull())
			{
				BrokenShadow* pBrokenShadow = 
					(BrokenShadow*)g_pCore->GetEntity(m_BrokenShadow);

				if (pBrokenShadow != NULL)
				{
					// 描画残影
					pBrokenShadow->Update(seconds);

					if (pBrokenShadow->GetComplete())
					{
						pBrokenShadow->Release();
						m_BrokenShadow = PERSISTID();
					}
				}
			}
		}
		else
		{
			ClearChild();
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Update(seconds);
	}
}

void CActorNode::UpdateMatrix(IVisBase* pVisBase)
{
	Assert(pVisBase != NULL);

	//更新绑定点位置
	FmMat4 mtxTemp;
	FmMat4 mtxObj;

	if (m_pParentNode)
	{
		m_pParentNode->GetWorldMatrix(mtxObj);
	}
	else
	{
		m_pActor->GetWorldMatrix(mtxObj);
	}

	if (m_strPoint.empty())
	{
#ifdef IMODEL_ADV_NODERADIAN180
		FmMatrixMultiply(&mtxTemp, &s_mtxRotation180, &mtxObj);
#else
		mtxTemp = mtxObj;
#endif // IMODEL_ADV_NODERADIAN180
	}
	else
	{
		if (m_pActor->GetCurrentNodeMatrix(m_pParentNode, m_strPoint.c_str(), 
			&mtxTemp))
		{
			FmMatrixMultiply(&mtxTemp, &mtxTemp, &mtxObj);
		}
		else
		{
			mtxTemp = mtxObj;
		}
	}

//	//更新绑定点位置
//	FmMat4 mtxTemp;
//	FmMat4 mtxObj;
//
//	if (m_strPoint.empty())
//	{
//#ifdef IMODEL_ADV_NODERADIAN180
//		FmMatrixRotationY(&mtxTemp, D3DXToRadian(180.0F));
//#else
//		FmMatrixIdentity(&mtxTemp);
//#endif // IMODEL_ADV_NODERADIAN180
//	}
//	else
//	{
//		if (!m_pActor->GetCurrentNodeMatrix(m_pParentNode, m_strPoint.c_str(), 
//			&mtxTemp))
//		{
//			FmMatrixIdentity(&mtxTemp);
//		}
//	}
//
//	if (m_pParentNode)
//	{
//		m_pParentNode->GetWorldMatrix(mtxObj);
//	}
//	else
//	{
//		m_pActor->GetWorldMatrix(mtxObj);
//	}
//
//	VisUtil_MatrixMultiply(&mtxTemp, &mtxTemp, &mtxObj);
	
	if (m_bAngleKeep)
	{
		FmVec3 scale;
		FmVec3 position;
		FmQuat no_rotate(0.0F, 0.0F, 0.0F, 1.0F);

		VisUtil_GetMatrixScale(&scale, &mtxTemp);
		
		position.x = mtxTemp._41;
		position.y = mtxTemp._42;
		position.z = mtxTemp._43;

		VisUtil_MatrixTransform(&mtxTemp, &scale, &no_rotate, &position);
	}

	FmMatrixMultiply(&m_mtxWorld, &m_LocalMatrix, &mtxTemp);

	pVisBase->SetWorldMatrix(m_mtxWorld);

	if (m_strPoint.empty() && pVisBase->GetEntInfo()->IsKindOf("Skin"))
	{
		if (m_bAngleKeep)
		{
			FmVec3 scale;
			FmVec3 position;
			FmQuat no_rotate(0.0F, 0.0F, 0.0F, 1.0F);

			VisUtil_GetMatrixScale(&scale, &mtxObj);

			position.x = mtxObj._41;
			position.y = mtxObj._42;
			position.z = mtxObj._43;

			VisUtil_MatrixTransform(&mtxObj, &scale, &no_rotate, &position);
		}

		FmMatrixMultiply(&m_mtxWorld, &m_LocalMatrix, &mtxObj);
	}
}

void CActorNode::UpdateNode()
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		UpdateMatrix(pVisBase);
	}

    for (int i = (int)m_Children.size() - 1; i >= 0; --i)
    {
        m_Children[i]->UpdateNode();

        // 删除无效的节点
        CActorNode* pNode = m_Children[i];
        if (!pNode->IsValid())
        {
            m_Children.remove(i);
            pNode->ReleaseAll();
            CORE_DELETE(pNode);
        }
    }
}

void CActorNode::Realize()
{
	if (m_bNodeVisible)
	{
		IVisBase* pVisBase = GetVisBase();

		if (pVisBase)
		{
			UpdateMatrix(pVisBase);

			pVisBase->Realize();

			if (!m_BrokenShadow.IsNull())
			{
				IVisBase* pBrokenShadow = 
					(IVisBase*)g_pCore->GetEntity(m_BrokenShadow);

				if (pBrokenShadow != NULL)
				{
					// 描画残影
					pBrokenShadow->Realize();
				}
			}
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Realize();
	}
}

void CActorNode::RealizeShadowMap()
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		UpdateMatrix(pVisBase);

		pVisBase->RealizeShadowMap();
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->RealizeShadowMap();
	}
}

// 是否有效
bool CActorNode::IsValid()
{
    if (GetVisBase())
    {
        return true;
    }
    return (m_Children.size() > 0);
}

bool CActorNode::GetLoaded()
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		if (!pVisBase->GetLoaded())
		{
			return false;
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (!m_Children[i]->GetLoaded())
		{
			return false;
		}
	}

	return true;
}

bool CActorNode::GetLoadFinish()
{
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (!m_Children[i]->GetLoadFinish())
		{
			return false;
		}
	}

	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		if (!pVisBase->GetLoadFinish())
		{
			return false;
		}
	}

	return true;
}

bool CActorNode::SetLocalPosition(float x, float y, float z)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		m_LocalMatrix._41 = x;
		m_LocalMatrix._42 = y;
		m_LocalMatrix._43 = z;
	}

	return true;
}

FmVec3 CActorNode::GetLocalPosition()
{
	return FmVec3(m_LocalMatrix._41, m_LocalMatrix._42, 
		m_LocalMatrix._43);
}

bool CActorNode::SetLocalAngle(float x, float y, float z)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		FmVec3 scale;
		FmVec3 position;
		FmQuat rotate;

		VisUtil_GetMatrixScale(&scale, &m_LocalMatrix);

		position.x = m_LocalMatrix._41;
		position.y = m_LocalMatrix._42;
		position.z = m_LocalMatrix._43;

		FmQuatRotationYawPitchRoll(&rotate, y, x, z);
		VisUtil_MatrixTransform(&m_LocalMatrix, &scale, &rotate, &position);
	}

	return true;
}

FmVec3 CActorNode::GetLocalAngle()
{
	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_LocalMatrix);

	return angle;
}

bool CActorNode::SetLocalScale(float x, float y, float z)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		VisUtil_SetMatrixScale(&m_LocalMatrix, x, y, z);
	}

	return true;
}

FmVec3 CActorNode::GetLocalScale()
{
	FmVec3 scale;
	
	VisUtil_GetMatrixScale(&scale, &m_LocalMatrix);

	return scale;
}

bool CActorNode::SetUnderWater(bool value)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->SetUnderWater(value);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetUnderWater(value);
	}

	return true;
}

bool CActorNode::SetWriteVelocity(int value)
{
	/*
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->SetWriteVelocity(value);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetWriteVelocity(value);
	}
*/
	return true;
}

bool CActorNode::SetColor(unsigned int value)
{
	m_nColor = value;

	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->SetColor(m_nColor);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetParentColor(m_nColor);
	}

	return true;
}

bool CActorNode::SetParentColor(unsigned int value)
{
	unsigned int realizecolor =
		((((m_nColor >> 24) & 0xFF) * ((value >> 24) & 0xFF) / 255) << 24)
		|((((m_nColor >> 16) & 0xFF) * ((value >> 16) & 0xFF) / 255) << 16)
		|((((m_nColor >> 8) & 0xFF) * ((value >> 8) & 0xFF) / 255) << 8)
		|((((m_nColor >> 0) & 0xFF) * ((value >> 0) & 0xFF) / 255) << 0);

	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->SetColor(realizecolor);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetParentColor(realizecolor);
	}

	return true;
}

bool CActorNode::SetLevelOfDetail(float value)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		pVisBase->SetLevelOfDetail(value);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetLevelOfDetail(value);
	}

	return true;
}

bool CActorNode::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	IVisBase* pVisBase = GetVisBase();
	
	if (pVisBase && m_bEsseObj)
	{
		int old_hit_count = result.nHitCount;

		if (pVisBase->TraceDetail(src, dst, result))
		{
			FmMat4 mtxPoint;

			if (m_strPoint.empty())
			{
#ifdef IMODEL_ADV_NODERADIAN180
				//FmMatrixRotationY(&mtxPoint, D3DXToRadian(180.0F));
				mtxPoint = s_mtxRotation180;
#else
				FmMatrixIdentity(&mtxPoint);
#endif // IMODEL_ADV_NODERADIAN180
			}
			else
			{
				if(!m_pActor->GetNodeWorldMatrix(m_pParentNode, 
					m_strPoint.c_str(), &mtxPoint))
				{
					#ifdef IMODEL_ADV_NODERADIAN180
						//FmMatrixRotationY(&mtxPoint, D3DXToRadian(180.0F));
						mtxPoint = s_mtxRotation180;
					#else
						FmMatrixIdentity(&mtxPoint);
					#endif // IMODEL_ADV_NODERADIAN180
				}
				else
				{

					FmMat4 mtxWorld;
					FmMat4 mtxInverseWorld;

					m_pActor->GetWorldMatrix(mtxWorld);

					FmMatrixInverse(&mtxInverseWorld, &mtxWorld);
					FmMatrixMultiply(&mtxPoint, &mtxPoint, &mtxInverseWorld);
					FmMatrixMultiply(&mtxPoint, &m_LocalMatrix, &mtxPoint);
				}
				//VisUtil_MatrixMultiply(&mtxPoint, &mtxPoint, &mtxInverseWorld);
				//VisUtil_MatrixMultiply(&mtxPoint, &m_LocalMatrix, &mtxPoint);
			}

			if (result.nHitMax > 1)
			{
				for (int t = old_hit_count; t < result.nHitCount; ++t)
				{
					trace_info_t::hit_t* pHit = &result.Hits[t];

					FmVec3TransformCoord(&pHit->Vertex[0], &pHit->Vertex[0], 
						&mtxPoint);
					FmVec3TransformCoord(&pHit->Vertex[1], &pHit->Vertex[1], 
						&mtxPoint);
					FmVec3TransformCoord(&pHit->Vertex[2], &pHit->Vertex[2], 
						&mtxPoint);
				}
			}
			else
			{
				trace_info_t::hit_t* pHit = &result.Hits[0];

				FmVec3TransformCoord(&pHit->Vertex[0], &pHit->Vertex[0], 
					&mtxPoint);
				FmVec3TransformCoord(&pHit->Vertex[1], &pHit->Vertex[1], 
					&mtxPoint);
				FmVec3TransformCoord(&pHit->Vertex[2], &pHit->Vertex[2], 
					&mtxPoint);
			}

			return true;
		}
	}
	
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->TraceDetail(src, dst, result))
		{
			return true;
		}
	}
	
	return false;
}

bool CActorNode::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	IVisBase* pVisBase = GetVisBase();
	
	if (pVisBase && m_bEsseObj)
	{
		if (pVisBase->TraceHitted(src, dst))
		{
			return true;
		}
	}
	
	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->TraceHitted(src, dst))
		{
			return true;
		}
	}
	
	return false;
}

bool CActorNode::TraceSphere(const FmVec3& center, float radius)
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase && m_bEsseObj)
	{
		if (pVisBase->TraceSphere(center, radius))
		{
			return true;
		}
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->TraceSphere(center, radius))
		{
			return true;
		}
	}

	return false;
}

// 三角形碰撞
bool CActorNode::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
    IVisBase* pVisBase = GetVisBase();

    if (pVisBase && m_bEsseObj)
    {
        if (pVisBase->TraceTriangle(v0, v1, v2))
        {
            return true;
        }
    }

    for (size_t i = 0; i < m_Children.size(); ++i)
    {
        if (m_Children[i]->TraceTriangle(v0, v1, v2))
        {
            return true;
        }
    }

    return false;
}

bool CActorNode::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	bool res = false;
	
	IVisBase* pVisBase = GetVisBase();
	
	if (pVisBase && m_bEsseObj)
	{
		res = pVisBase->Cull(planes, plane_num, center, radius, cull_func, 
			context);
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->Cull(planes, plane_num, center, radius, cull_func, 
			context))
		{
			res = true;
		}
	}

	return res;
}

void CActorNode::GetWorldMatrix(FmMat4& mat) const
{
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase)
	{
		mat = m_mtxWorld;
	}
	else
	{
		m_pActor->GetWorldMatrix(mat);
	}
}

bool CActorNode::GetBoundingBox(FmVec3& v_min, FmVec3& v_max)
{
	bool succeed = false;
	IVisBase* pVisBase = GetVisBase();

	if (pVisBase && m_bEsseObj)
	{
		FmVec3 center = pVisBase->GetCenter();
		FmVec3 box_size = pVisBase->GetBoxSize();
		FmVec3 node_min = center - box_size * 0.5F;
		FmVec3 node_max = center + box_size * 0.5F;

		FmVec3Minimize(&v_min, &v_min, &node_min);
		FmVec3Maximize(&v_max, &v_max, &node_max);
		succeed = true;
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		if (m_Children[i]->GetBoundingBox(v_min, v_max))
		{
			succeed = true;
		}
	}

	return succeed;
}

void CActorNode::SetAngleKeep(bool value)
{
	m_bAngleKeep = value;
}

void CActorNode::SetIsEsseObj(bool value)
{
	m_bEsseObj = value;
}

void CActorNode::SetEsseObj(const char* name, bool value)
{
	if (m_strName == name)
	{
		m_bEsseObj = value;
	}

	for (size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->SetEsseObj(name, value);
	}
}

bool CActorNode::GetEsseObj(const char* name)
{
	CActorNode* pActorNode = GetNode(name);

	if (pActorNode)
	{
		return pActorNode->m_bEsseObj;
	}
	else
	{
		return true;
	}
}

