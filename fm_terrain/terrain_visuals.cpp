//--------------------------------------------------------------------
// �ļ���:		terrain_visuals.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��10��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "terrain_visuals.h"
#include "terrain.h"
#include "zone_manager.h"
#include "quad_tree.h"
#include "quad_node.h"
#include "quad_visual.h"
#include "quad_collide.h"
#include "../visual/i_vis_base.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
//#include "../utils/system_utils.h"

// CTerrainVisuals

CTerrainVisuals::CTerrainVisuals(Terrain* pTerrain, CTerrainZone* pZone)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_pCore = m_pTerrain->GetCore();
}

CTerrainVisuals::~CTerrainVisuals()
{
	size_t visual_size = m_Visuals.size();
	
	for (size_t i = 0; i < visual_size; ++i)
	{
		if (m_Visuals[i])
		{
			// ���Ӷ�����ͷŲ����ڵ��ε����������н���
			CORE_DELETE(m_Visuals[i]);
		}
	}
}

bool CTerrainVisuals::FindVisualIndex(const PERSISTID& id, 
	size_t& index) const
{
	return m_VisualIndex.GetData(id, index);
}

bool CTerrainVisuals::FindNameIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValue(name);
	size_t visual_size = m_Visuals.size();	
	
	for (size_t i = 0; i < visual_size; ++i)
	{
		CQuadVisual* pVisual = m_Visuals[i];

		if (NULL == pVisual)
		{
			continue;
		}
		
		if ((pVisual->GetHash() == hash)
			&& (stricmp(pVisual->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}
	
	return false;
}

bool CTerrainVisuals::CalcBoundBox(IVisBase* pVisBase, FmVec4& v_min, 
	FmVec4& v_max)
{
	Assert(pVisBase != NULL);
	
	FmVec3 angle = pVisBase->GetAngle();
	FmVec3 center = pVisBase->GetCenter();
	float radius = pVisBase->GetRadius();

	if ((radius > 1024.0F) || Port_IsNaN(radius))
	{
		// ��ʾ�����Χ�й���
		char info[256];

		SafeSprintf(info, sizeof(info),
			"(CTerrainVisuals::CalcBoundBox)%s too big, radius = %.3f", 
			pVisBase->GetEntInfo()->GetEntityName(), radius);
		CORE_TRACE(info);
		//CORE_TRACE("(CTerrainVisuals::CalcBoundBox)visual too big");
		//CORE_TRACE(pVisBase->GetEntInfo()->GetEntityName());
		return false;
	}
	
	if (FloatEqual(angle.x, 0.0F) && FloatEqual(angle.z, 0.0F))
	{
		FmVec3 box_size = pVisBase->GetBoxSize();

		if (FloatEqual(angle.y, 0.0F))
		{
			// û���κ���ת
			// ʹ������ԭ�еİ�Χ��
			v_min.x = center.x - box_size.x * 0.5F;
			v_min.y = center.y - box_size.y * 0.5F;
			v_min.z = center.z - box_size.z * 0.5F;
			v_max.x = center.x + box_size.x * 0.5F;
			v_max.y = center.y + box_size.y * 0.5F;
			v_max.z = center.z + box_size.z * 0.5F;
		}
		else
		{
			// ֻ�д�ֱ�������ת
			// ����İ뾶
			FmVec2 v(box_size.x, box_size.z);
			 
			// ����İ뾶
			float r = FmVec2Length(&v) * 0.5F;
			
			// ʹ��Բ����İ�Χ��
			v_min.x = center.x - r;
			v_min.y = center.y - box_size.y * 0.5F;
			v_min.z = center.z - r;
			v_max.x = center.x + r;
			v_max.y = center.y + box_size.y * 0.5F;
			v_max.z = center.z + r;
		}
	}
	else
	{
		// ʹ�����ĺͰ뾶���ɰ�Χ��
		v_min.x = center.x - radius;
		v_min.y = center.y - radius;
		v_min.z = center.z - radius;
		v_max.x = center.x + radius;
		v_max.y = center.y + radius;
		v_max.z = center.z + radius;
	}

	return true;
}

bool CTerrainVisuals::AddVisual(const char* name, IVisBase* pVisBase, 
	bool is_role, float clip_radius)
{
	Assert(name != NULL);
	Assert(pVisBase != NULL);

	size_t index;
	
	if (FindVisualIndex(pVisBase->GetID(), index))
	{
		CORE_TRACE("(CTerrainVisuals::AddVisual)visual id exists");
		CORE_TRACE(name);
		return false;
	}

	FmVec4 v_min;
	FmVec4 v_max;

	if (!CalcBoundBox(pVisBase, v_min, v_max))
	{
		return false;
	}

	if (!m_FreeList.empty())
	{
		index = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else
	{
		index = m_Visuals.size();
		m_Visuals.push_back(NULL);
	}
	
	// ����Ϊȱʡ����ײ����߶�
	int scale = 
		(int)m_pTerrain->GetCollideScale() * (int)m_pTerrain->GetUnitSize();
	//
	//FmVec4 v_min;
	//FmVec4 v_max;
	//
	//if (!CalcBoundBox(pVisBase, v_min, v_max))
	//{
	//	return false;
	//}

	CQuadVisual* p = (CQuadVisual*)CORE_ALLOC(sizeof(CQuadVisual));
	
	new (p) CQuadVisual(m_pTerrain, scale, v_min, v_max, 
		pVisBase, name, GetHashValue(name), is_role, clip_radius);
	
	m_Visuals[index] = p;
	m_VisualIndex.Add(pVisBase->GetID(), index);

	// ��ӵ���ײ��
	m_pTerrain->GetCollide()->GetQuadTree()->GetRoot()->AddObject(p);
	
	return true;
}

bool CTerrainVisuals::RemoveByIndex(size_t index)
{
	Assert(index < m_Visuals.size());
	
	CQuadVisual* pObject = m_Visuals[index];
	
	// ����ײ���ڵ��Ƴ�
	pObject->RemoveNodes();
	
	m_Visuals[index] = NULL;
	m_VisualIndex.RemoveData(pObject->GetVisualID(), index);
	m_FreeList.push_back(index);
	CORE_DELETE(pObject);

	return true;
}

void CTerrainVisuals::ReleaseVisuals()
{
	for (size_t i = 0; i < m_Visuals.size(); ++i)
	{
		CQuadVisual* pVisual = m_Visuals[i];

		if (NULL == pVisual)
		{
			continue;
		}
		
		PERSISTID id = pVisual->GetVisualID();

		RemoveByIndex(i);

		// ɾ������
		m_pCore->DeleteEntity(id);
	}

	m_Visuals.clear();
	m_VisualIndex.Clear();
	m_FreeList.clear();
}

bool CTerrainVisuals::MoveVisual(IVisBase* pVisBase, float x, float y, float z)
{
	Assert(pVisBase != NULL);
	
	size_t index;

	if (!FindVisualIndex(pVisBase->GetID(), index))
	{
		return false;
	}
	
	CQuadVisual* pObject = m_Visuals[index];
	
	// ����ײ���ڵ��Ƴ�
	pObject->RemoveNodes();
	
	// �����µ�λ��
	pVisBase->SetPosition(x, y, z);
	
	// ���¼����Χ��
	CalcBoundBox(pVisBase, pObject->m_vMin, pObject->m_vMax);

	// ��ӵ���ײ��
	m_pTerrain->GetCollide()->GetQuadTree()->GetRoot()->AddObject(pObject);
	
	return true;
}

size_t CTerrainVisuals::GetRectVisuals(float x1, float z1, float x2, 
	float z2, IVarList& result)
{
	size_t count = 0;
	size_t visual_size = m_Visuals.size();	
	
	for (size_t i = 0; i < visual_size; ++i)
	{
		CQuadVisual* pVisual = m_Visuals[i];

		if (NULL == pVisual)
		{
			continue;
		}
		
		IVisBase* pVisBase = (IVisBase*)m_pCore->GetEntity(
			pVisual->GetVisualID());

		if (NULL == pVisBase)
		{
			continue;
		}

		FmVec3 pos = pVisBase->GetPosition();

		if ((pos.x >= x1) && (pos.x <= x2) && (pos.z >= z1) && (pos.z <= z2))
		{
			result.AddObject(pVisBase->GetID());
			count++;
		}
	}
	
	return count;
}

size_t CTerrainVisuals::GetRectActors(float x1, float z1, float x2, float z2, 
	IVarList& result)
{
	size_t count = 0;
	size_t visual_size = m_Visuals.size();	

	for (size_t i = 0; i < visual_size; ++i)
	{
		CQuadVisual* pVisual = m_Visuals[i];

		if (NULL == pVisual)
		{
			continue;
		}

		if (pVisual->GetIsRole())
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)m_pCore->GetEntity(
			pVisual->GetVisualID());

		if (NULL == pVisBase)
		{
			continue;
		}

		if (!pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			continue;
		}

		FmVec3 pos = pVisBase->GetPosition();

		if ((pos.x >= x1) && (pos.x <= x2) && (pos.z >= z1) && (pos.z <= z2))
		{
			result.AddObject(pVisBase->GetID());
			count++;
		}
	}

	return count;
}


size_t CTerrainVisuals::GetCircleVisuals(float center_x, float center_z, 
	float radius, IVarList& result)
{
	size_t count = 0;
	size_t visual_size = m_Visuals.size();	
	float radiusSqr = radius * radius;

	for (size_t i = 0; i < visual_size; ++i)
	{
		CQuadVisual* pVisual = m_Visuals[i];

		if (NULL == pVisual)
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)m_pCore->GetEntity(
			pVisual->GetVisualID());

		if (NULL == pVisBase)
		{
			continue;
		}

		FmVec3 pos = pVisBase->GetPosition();

		float dx = pos.x - center_x;
		float dz = pos.z - center_z;

		if ( dx * dx + dz * dz < radiusSqr)
		{
			result.AddObject(pVisBase->GetID());
			count++;
		}
	}

	return count;
}


