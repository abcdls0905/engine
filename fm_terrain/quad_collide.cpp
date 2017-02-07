//--------------------------------------------------------------------
// 文件名:		quad_collide.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "quad_collide.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "zone_manager.h"
#include "quad_tree.h"
#include "quad_node.h"
#include "quad_ground.h"
#include "quad_visual.h"
#include "../utils/tick_timer.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

// CQuadCollide

CQuadCollide::CQuadCollide()
{
	m_pTerrain = NULL;
	m_pQuadTree = CORE_NEW(CQuadTree);
	m_bFound = false;
	m_fDistance = 0.0F;
	m_pProcessed = NULL;
}

CQuadCollide::~CQuadCollide()
{
	CORE_DELETE(m_pQuadTree);
}

void CQuadCollide::SetTerrain(Terrain* pTerrain)
{
	Assert(pTerrain != NULL);
	
	m_pTerrain = pTerrain;
}

bool CQuadCollide::Init()
{
	CZoneManager* pManager = m_pTerrain->GetZoneManager();
	// 初始化四叉树
	size_t zone_rows = pManager->GetRows();
	size_t zone_cols = pManager->GetCols();
	size_t zone_scale = m_pTerrain->GetZoneScale();
	size_t size = FM_MAX(zone_rows + 1, zone_cols + 1) * zone_scale;
	size_t scale = 1;

	while (scale < size)
	{
		scale = scale << 1;
	}
	
	size_t origin_row = pManager->GetOriginRow();
	size_t origin_col = pManager->GetOriginCol();
	int offset_x = int(scale / 2) - (int(origin_col) - int(zone_cols / 2));
	int offset_z = int(scale / 2) - (int(origin_row) - int(zone_rows / 2));

	// 让四叉树节点与地形区域对齐
	offset_x += (int)zone_scale / 2;
	offset_z += (int)zone_scale / 2;

	float unit_size = m_pTerrain->GetUnitSize();
	float left = -offset_x * unit_size;
	float top = -offset_z * unit_size;

	//scale = scale * size_t(unit_size + 0.999f);

	m_pQuadTree->SetZoneScale(zone_scale * (size_t)unit_size);
	m_pQuadTree->ReleaseAll();
	m_pQuadTree->Create(scale * size_t(unit_size), left, top);
	
	return true;
}

#define DELTA 0.001F
#define FLOAT_EQUAL(v1, v2) \
	((v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON)))

// 判断射线如何离开矩形
static int node_hit_back(CQuadNode* node, const FmVec3& src, 
	const FmVec3& dst, const FmVec3& dir)
{
	int result = 0;
	
	float x1 = node->m_vMin.x;
	float z1 = node->m_vMin.z;
	float x2 = node->m_vMax.x;
	float z2 = node->m_vMax.z;
	
	if (dir.x < 0)
	{
		float dist1 = x1 - src.x;
		float dist2 = x1 - dst.x;
		
		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// left
				result |= 0x1;
			}
		}
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float z = src.z + dir.z * (dist1 / (dist1 - dist2));
			
			if ((z >= z1) && (z <= z2))
			{
				// left
				result |= 0x1;
			}
		}
	}
	else
	{
		float dist1 = src.x - x2;
		float dist2 = dst.x - x2;
		
		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// right
				result |= 0x4;
			}
		}
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float z = src.z + dir.z * (dist1 / (dist1 - dist2));
			
			if ((z >= z1) && (z <= z2))
			{
				// right
				result |= 0x4;
			}
		}
	}
	
	if (dir.z < 0)
	{
		float dist1 = z1 - src.z;
		float dist2 = z1 - dst.z;
		
		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// top
				result |= 0x2;
			}
		}
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float x = src.x + dir.x * (dist1 / (dist1 - dist2));
			
			if ((x >= x1) && (x <= x2))
			{
				// top
				result |= 0x2;
			}
		}
	}
	else
	{
		float dist1 = src.z - z2;
		float dist2 = dst.z - z2;
		
		if (FLOAT_EQUAL(dist1, dist2))
		{
			if (FLOAT_EQUAL(dist1, 0))
			{
				// bottom
				result |= 0x8;
			}
		}
		else if ((dist1 <= 0) && (dist2 >= 0))
		{
			// 与此边框的交点位置
			float x = src.x + dir.x * (dist1 / (dist1 - dist2));
			
			if ((x >= x1) && (x <= x2))
			{
				// bottom
				result |= 0x8;
			}
		}
	}
	
	return result;
}

bool CQuadCollide::TraceNode(CQuadNode* node, const FmVec3& src, 
	const FmVec3& dst, const FmVec3& dir, trace_info_t* result)
{
	CQuadGround* pGround = node->m_pGround;

	if (NULL == pGround)
	{
		return false;
	}

	// 判断是否与节点的包围盒相交
	if (FmBoxBoundProbe(&node->m_vMin, &node->m_vMax, &src, &dir) == false)
	{
		return false;
	}

	unsigned int trace_mask = m_pTerrain->GetTraceMaskValue();

	if (!pGround->IsTraceMasked(trace_mask))
	{
		if (NULL == result)
		{
			if (pGround->TraceHitted(src, dir))
			{
				m_bFound = true;
				// 只测试第一次碰撞
				return true;
			}
		}
		else
		{
			if (pGround->TraceDetail(src, dir, *result))
			{
				// 距离最近的碰撞点
				if ((!m_bFound) || (result->fDistance < m_fDistance))
				{
					m_bFound = true;
					m_fDistance = result->fDistance;
				}
			}
		}
	}
	
	size_t object_size = node->m_Objects.size();
	
	for (size_t i = 0; i < object_size; ++i)
	{
		CQuadVisual* object = node->m_Objects[i];
		
		if (object->m_bUsed)
		{
			// 已处理
			continue;
		}

		if (object->IsTraceMasked(trace_mask))
		{
			// 碰撞被屏蔽
			continue;
		}
		
		// 设置已处理标志
		object->m_bUsed = true;
		object->m_pNext = m_pProcessed;

		m_pProcessed = object;

		if (NULL == result)
		{
			if (object->TraceHitted(src, dst))
			{
				m_bFound = true;
				// 只测试第一次碰撞
				return true;
			}
		}
		else
		{
			if (object->TraceDetail(src, dst, *result))
			{
				// 距离最近的碰撞点
				if ((!m_bFound) || (result->fDistance < m_fDistance))
				{
					m_bFound = true;
					m_fDistance = result->fDistance;
				}
			}
		}
	}

	return false;
}

bool CQuadCollide::TraceLeaf(CQuadNode* node, const FmVec3& src, 
	const FmVec3& dst, const FmVec3& dir, trace_info_t* result)
{
	if (m_bFound)
	{
		float node_x = (node->m_vMin.x + node->m_vMax.x) * 0.5F;
		float node_z = (node->m_vMin.z + node->m_vMax.z) * 0.5F;
		float sx = (src.x - node_x) + node->m_nScale;
		float sz = (src.z - node_z) + node->m_nScale;
		float x1 = src.x - dst.x;
		float z1 = src.z - dst.z;
		
		if ((sx * sx + sz * sz) > (x1 * x1 + z1 * z1) * m_fDistance)
		{
			// 可以停止
			return false;
		}
	}
	
	if (TraceNode(node, src, dst, dir, result))
	{
		return true;
	}
	
	int edge = node_hit_back(node, src, dst, dir);

	if (edge & 0x1)
	{
		// left
		CQuadNode* pLeft = node->GetLeftNode();

		if (pLeft)
		{
			if (TraceLeaf(pLeft, src, dst, dir, result))
			{
				return true;
			}
		}
	}
	else if (edge & 0x4)
	{
		// right
		CQuadNode* pRight = node->GetRightNode();

		if (pRight)
		{
			if (TraceLeaf(pRight, src, dst, dir, result))
			{
				return true;
			}
		}
	}

	if (edge & 0x2)
	{
		// top
		CQuadNode* pTop = node->GetTopNode();

		if (pTop)
		{
			if (TraceLeaf(pTop, src, dst, dir, result))
			{
				return true;
			}
		}
	}
	else if (edge & 0x8)
	{
		// bottom
		CQuadNode* pBottom = node->GetBottomNode();

		if (pBottom)
		{
			if (TraceLeaf(pBottom, src, dst, dir, result))
			{
				return true;
			}
		}
	}

	/*
	if (edge & 0x1)
	{
		// left
		CQuadNode* pLeft = node->GetLeftNode();

		if (pLeft)
		{
			if (TraceLeaf(pLeft, src, dst, dir, result))
			{
				return true;
			}
		}
	}

	if (edge & 0x2)
	{
		// top
		CQuadNode* pTop = node->GetTopNode();
		
		if (pTop)
		{
			if (TraceLeaf(pTop, src, dst, dir, result))
			{
				return true;
			}
		}
	}
	
	if (edge & 0x4)
	{
		// right
		CQuadNode* pRight = node->GetRightNode();
		
		if (pRight)
		{
			if (TraceLeaf(pRight, src, dst, dir, result))
			{
				return true;
			}
		}
	}

	if (edge & 0x8)
	{
		// bottom
		CQuadNode* pBottom = node->GetBottomNode();
		
		if (pBottom)
		{
			if (TraceLeaf(pBottom, src, dst, dir, result))
			{
				return true;
			}
		}
	}
	*/
	
	return false;
}

// 获得起始节点
static CQuadNode* get_start_node(CQuadNode* root, int scale, float x, float z)
{
	if (x < root->m_vMin.x)
	{
		x = root->m_vMin.x;
	}
	else if (x >= root->m_vMax.x)
	{
		x = root->m_vMax.x - 0.001F;
	}

	if (z < root->m_vMin.z)
	{
		z = root->m_vMin.z;
	}
	else if (z >= root->m_vMax.z)
	{
		z = root->m_vMax.z - 0.001F;
	}

	return root->GetInNode(scale, x, z);
}

bool CQuadCollide::TraceDetail(const FmVec3& src, const FmVec3& dst,
	trace_info_t& result)
{
	CQuadNode* root = m_pQuadTree->GetLocalRoot();

	if (NULL == root)
	{
		return false;
	}
	
	m_bFound = false;
	m_fDistance = 1e8;

	// 获得起始节点
	int scale = 
		(int)m_pTerrain->GetCollideScale() * (int)m_pTerrain->GetUnitSize();
	CQuadNode* node = get_start_node(root, scale, src.x, src.z);
	
	if (NULL == node)
	{
		return false;
	}
	
	Assert(NULL == m_pProcessed);

	TraceLeaf(node, src, dst, dst - src, &result);

	// 清除物体的已处理标志
	while (m_pProcessed)
	{
		m_pProcessed->m_bUsed = false;
		m_pProcessed = m_pProcessed->m_pNext;
	}

	return m_bFound;
}

bool CQuadCollide::TraceAll(const FmVec3& src, const FmVec3& dst,
	trace_info_t& result)
{
	unsigned int old_trace_mask = m_pTerrain->GetTraceMaskValue();
	unsigned int mask = old_trace_mask;

	if (!m_pTerrain->GetGroundVisible())
	{
		mask |= Terrain::TRACEMASK_GROUND;
	}

	if (!m_pTerrain->GetVisualVisible())
	{
		mask |= Terrain::TRACEMASK_VISUAL;
	}
	
	m_pTerrain->SetTraceMaskValue(mask);
	
	bool res = TraceDetail(src, dst, result);

	m_pTerrain->SetTraceMaskValue(old_trace_mask);

	return res;
}

bool CQuadCollide::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	CQuadNode* root = m_pQuadTree->GetLocalRoot();
	
	if (NULL == root)
	{
		return false;
	}
	
	m_bFound = false;
	m_fDistance = 1e8;
	
	int scale = 
		(int)m_pTerrain->GetCollideScale() * (int)m_pTerrain->GetUnitSize();
	CQuadNode* node = get_start_node(root, scale, src.x, src.z);
	
	if (NULL == node)
	{
		return false;
	}
	
	Assert(NULL == m_pProcessed);

	TraceLeaf(node, src, dst, dst - src, NULL);
	
	// 清除物体的已处理标志
	while (m_pProcessed)
	{
		m_pProcessed->m_bUsed = false;
		m_pProcessed = m_pProcessed->m_pNext;
	}

	return m_bFound;
}

// 矩形是否包含球体
static inline bool sphere_in_rect(float x, float z, float radius, 
	float x1, float z1, float x2, float z2)
{
	return (x >= x1 - radius) && (x < x2 + radius)
		&& (z >= z1 - radius) && (z < z2 + radius);
}

bool CQuadCollide::TraceSphereNode(CQuadNode* node, const FmVec3& center, 
	float radius, unsigned int trace_mask)
{
	float x1 = node->m_vMin.x;
	float z1 = node->m_vMin.z;
	float x2 = node->m_vMax.x;
	float z2 = node->m_vMax.z;
	
	// 加上偏移防止浮点误差
	if (!sphere_in_rect(center.x, center.z, radius + DELTA, x1, z1, x2, z2))
	{
		return false;
	}

	CQuadGround* pGround = node->m_pGround;

	if (pGround)
	{
		if (!pGround->IsTraceMasked(trace_mask))
		{
			if (pGround->TraceSphere(center, radius))
			{
				return true;
			}
		}
	}

	if (!node->m_Objects.empty())
	{
		size_t object_size = node->m_Objects.size();

		for (size_t i = 0; i < object_size; ++i)
		{
			CQuadVisual* object = node->m_Objects[i];

			if (object->m_bUsed)
			{
				continue;
			}

			if (object->IsTraceMasked(trace_mask))
			{
				// 碰撞被屏蔽
				continue;
			}

			// 设置已处理标志
			object->m_bUsed = true;
			object->m_pNext = m_pProcessed;

			m_pProcessed = object;

			if (object->TraceSphere(center, radius))
			{
				return true;
			}
		}
	}

	if (node->m_pChild0)
	{
		if (TraceSphereNode(node->m_pChild0, center, radius, trace_mask))
		{
			return true;
		}
	}

	if (node->m_pChild1)
	{
		if (TraceSphereNode(node->m_pChild1, center, radius, trace_mask))
		{
			return true;
		}
	}

	if (node->m_pChild2)
	{
		if (TraceSphereNode(node->m_pChild2, center, radius, trace_mask))
		{
			return true;
		}
	}

	if (node->m_pChild3)
	{
		if (TraceSphereNode(node->m_pChild3, center, radius, trace_mask))
		{
			return true;
		}
	}

	return false;
}

bool CQuadCollide::TraceSphere(const FmVec3& center, float radius)
{ 
	CQuadNode* root = m_pQuadTree->GetLocalRoot();

	if (NULL == root)
	{
		return false;
	}

	Assert(NULL == m_pProcessed);

	bool succeed = TraceSphereNode(root, center, radius, 
		m_pTerrain->GetTraceMaskValue());

	// 清除物体的已处理标志
	while (m_pProcessed)
	{
		m_pProcessed->m_bUsed = false;
		m_pProcessed = m_pProcessed->m_pNext;
	}

	return succeed;
}

bool CQuadCollide::CullNode(CQuadNode* node, const FmPlane* planes, 
	size_t plane_num, const FmVec3& center, float radius, 
	CULL_FUNC cull_func, void* context)
{
	float x1 = node->m_vMin.x;
	float z1 = node->m_vMin.z;
	float x2 = node->m_vMax.x;
	float z2 = node->m_vMax.z;

	// 加上偏移防止浮点误差
	if (!sphere_in_rect(center.x, center.z, radius + DELTA, x1, z1, x2, z2))
	{
		return false;
	}

	CQuadGround* pGround = node->m_pGround;

	if (pGround)
	{
		pGround->Cull(planes, plane_num, center, radius, cull_func, context);
	}

	if (!node->m_Objects.empty())
	{
		size_t object_size = node->m_Objects.size();

		for (size_t i = 0; i < object_size; ++i)
		{
			CQuadVisual* object = node->m_Objects[i];

			if (object->m_bUsed)
			{
				continue;
			}

			// 设置已处理标志
			object->m_bUsed = true;
			object->m_pNext = m_pProcessed;

			m_pProcessed = object;

			object->Cull(planes, plane_num, center, radius, cull_func, 
				context);
		}
	}
	
	if (node->m_pChild0)
	{
		CullNode(node->m_pChild0, planes, plane_num, center, radius, 
			cull_func, context);
	}
	
	if (node->m_pChild1)
	{
		CullNode(node->m_pChild1, planes, plane_num, center, radius, 
			cull_func, context);
	}

	if (node->m_pChild2)
	{
		CullNode(node->m_pChild2, planes, plane_num, center, radius, 
			cull_func, context);
	}

	if (node->m_pChild3)
	{
		CullNode(node->m_pChild3, planes, plane_num, center, radius, 
			cull_func, context);
	}

	return true;
}
					  
bool CQuadCollide::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context)
{
	CQuadNode* root = m_pQuadTree->GetLocalRoot();
	
	if (NULL == root)
	{
		return false;
	}
	
	Assert(NULL == m_pProcessed);

	bool succeed = CullNode(root, planes, plane_num, center, radius, 
		cull_func, context);

	// 清除物体的已处理标志
	while (m_pProcessed)
	{
		m_pProcessed->m_bUsed = false;
		m_pProcessed = m_pProcessed->m_pNext;
	}

	return succeed;
}

bool CQuadCollide::Compress()
{
	CTickTimer counter;

	m_pQuadTree->GetRoot()->Compress(&counter, 10);

	return true;
}

