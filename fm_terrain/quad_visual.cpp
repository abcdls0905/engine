//--------------------------------------------------------------------
// 文件名:		quad_visual.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "quad_visual.h"
#include "quad_node.h"
#include "terrain.h"
#include "../visual/i_vis_base.h"
#include "../visual/vis_utils.h"

// CQuadVisual

CQuadVisual::CQuadVisual(Terrain* pTerrain, size_t scale, 
	const FmVec4& v_min, const FmVec4& v_max, IVisBase* pVisBase,
	const char* name, unsigned int hash, bool is_role, float clip_radius)
{
	Assert(pVisBase != NULL);
	Assert(name != NULL);

	m_bUsed = false;
	m_pNext = NULL;
	m_pTerrain = pTerrain;
	m_nScale = scale;
	m_vMin = v_min;
	m_vMax = v_max;
	m_VisualID = pVisBase->GetID();
	m_strName = name;
	m_nHash = hash;
	m_bIsRole = is_role;
	m_fClipRadius = clip_radius;

	IEntInfo* pEntInfo = pVisBase->GetEntInfo();

	if (pEntInfo->IsKindOf("Model"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_MODEL;
	}
	else if (pEntInfo->IsKindOf("Actor"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_ACTOR;
	}
	else if (pEntInfo->IsKindOf("EffectModel"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_EFFECTMODEL;
	}
	else if (pEntInfo->IsKindOf("Particle"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_PARTICLE;
	}
	else if (pEntInfo->IsKindOf("Sound"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_SOUND;
	}
	else if (pEntInfo->IsKindOf("LightSource"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_LIGHT;
	}
	else if (pEntInfo->IsKindOf("Trigger"))
	{
		m_nTraceMask = ITerrain::TRACEMASK_TRIGGER;
	}
	else
	{
		//Assert(0);
		m_nTraceMask = 0;
	}
}

CQuadVisual::~CQuadVisual()
{
}

bool CQuadVisual::IsTraceMasked(unsigned int trace_mask)
{
	bool design_mode = m_pTerrain->GetDesignMode();
	
	if (!design_mode)
	{
		if (0 == trace_mask)
		{
			return false;
		}
	}
	
	if (m_bIsRole)
	{
		return (trace_mask & Terrain::TRACEMASK_ROLE) > 0;
	}

	if (design_mode) 
	{
		IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
			m_VisualID);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!m_pTerrain->GetShowTree()) 
		{
			if (VisUtil_GetCustomBool(pVisBase, "is_tree"))
			{
				// 树木不显示的时候也不能有碰撞
				return true;
			}
		}
		
		if ((trace_mask & Terrain::TRACEMASK_HELPER) > 0)
		{
			if (VisUtil_GetCustomBool(pVisBase, "only_design"))
			{
				return true;
			}
		}

		if ((trace_mask & Terrain::TRACEMASK_THROUGH) > 0)
		{
			if (VisUtil_GetCustomBool(pVisBase, "co_through"))
			{
				return true;
			}
		}

		if ((trace_mask & Terrain::TRACEMASK_ACTOR) > 0)
		{
			if (VisUtil_GetCustomBool(pVisBase, "need_collide"))
			{
				return false;
			}
		}
	}

	return (trace_mask & m_nTraceMask) > 0;
}

bool CQuadVisual::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
		m_VisualID);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetTraceEnable())
	{
		return false;
	}
	
	return pVisBase->TraceDetail(src, dst, result);
}

bool CQuadVisual::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
		m_VisualID);

	if (NULL == pVisBase)
	{
		return false;
	}
	
	if (!pVisBase->GetTraceEnable())
	{
		return false;
	}

	return pVisBase->TraceHitted(src, dst);
}

bool CQuadVisual::TraceSphere(const FmVec3& center, float radius)
{
	ICore* pCore = m_pTerrain->GetCore();
	IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(m_VisualID);

	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetTraceEnable())
	{
		return false;
	}

	return pVisBase->TraceSphere(center, radius);
}

bool CQuadVisual::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	if (m_pTerrain->GetOnlyCullTerrain())
	{
		return false;
	}

	IVisBase* pVisBase = (IVisBase*)m_pTerrain->GetCore()->GetEntity(
		m_VisualID);
	
	if (NULL == pVisBase)
	{
		return false;
	}

	if (!pVisBase->GetCullEnable())
	{
		return false;
	}
	
	return pVisBase->Cull(planes, plane_num, center, radius, cull_func, 
		context);
}

bool CQuadVisual::AddNode(CQuadNode* node)
{
	Assert(node != NULL);

	m_Nodes.push_back(node);
	
	return true;
}

void CQuadVisual::RemoveNodes()
{
	size_t node_num = m_Nodes.size();

	for (size_t i = 0; i < node_num; ++i)
	{
		m_Nodes[i]->RemoveObject(this);
	}

	m_Nodes.clear();
}

