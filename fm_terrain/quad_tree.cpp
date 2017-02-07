//--------------------------------------------------------------------
// 文件名:		quad_tree.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "quad_tree.h"
#include "quad_node.h"
#include "../public/core_mem.h"

// CQuadTree

CQuadTree::CQuadTree()
{
	m_pRoot = NULL;
	m_pLocalRoot = NULL;
	m_nZoneScale = 0;
	m_nNodeCount = 0;
	m_pFreeList = NULL;
}

CQuadTree::~CQuadTree()
{
	ReleaseAll();
}

void CQuadTree::ReleaseAll()
{
	if (m_pRoot)
	{
		DeleteNode(m_pRoot);
		m_pRoot = NULL;
	}
}

bool CQuadTree::Create(size_t scale, float x, float z)
{
	Assert(NULL == m_pRoot);
	
	m_pRoot = NewNode(NULL, scale, x, z);
	m_pLocalRoot = m_pRoot;
	
	return true;
}

CQuadNode* CQuadTree::NewNode(CQuadNode* parent, size_t scale, float x, 
	float z)
{
	CQuadNode* pNode;
	
	if (m_pFreeList)
	{
		node_pool_t* p = m_pFreeList;
		m_pFreeList = p->pNext;
		pNode = &p->data;
	}
	else
	{
		node_pool_t* p = m_MemBucket.New();
		p->pNext = NULL;
		pNode = &p->data;
	}

	new (pNode) CQuadNode(this, parent, scale, x, z);
	++m_nNodeCount;

	return pNode;
}

void CQuadTree::DeleteNode(CQuadNode* node)
{
	Assert(node != NULL);

	node->~CQuadNode();
	--m_nNodeCount;

	node_pool_t* p = (node_pool_t*)node;
	p->pNext = m_pFreeList;
	m_pFreeList = p;
}

// 矩形是否完全包含圆形
static inline bool rect_contain_circle(float x, float z, float radius, 
	float x1, float z1, float x2, float z2)
{
	return (x1 < x - radius) && (x2 > x + radius)
		&& (z1 < z - radius) && (z2 > z + radius);
}

// 获得可以包含指定圆形的最小节点
static CQuadNode* node_contain_circle(CQuadNode* node, float x, float z, 
	float radius)
{
	float x1 = node->m_vMin.x;
	float z1 = node->m_vMin.z;
	float x2 = node->m_vMax.x;
	float z2 = node->m_vMax.z;

	if (!rect_contain_circle(x, z, radius, x1, z1, x2, z2))
	{
		return NULL;
	}

	if (node->m_pChild0)
	{
		CQuadNode* res = node_contain_circle(node->m_pChild0, x, z, radius);

		if (res)
		{
			return res;
		}
	}
	
	if (node->m_pChild1)
	{
		CQuadNode* res = node_contain_circle(node->m_pChild1, x, z, radius);

		if (res)
		{
			return res;
		}
	}

	if (node->m_pChild2)
	{
		CQuadNode* res = node_contain_circle(node->m_pChild2, x, z, radius);

		if (res)
		{
			return res;
		}
	}

	if (node->m_pChild3)
	{
		CQuadNode* res = node_contain_circle(node->m_pChild3, x, z, radius);

		if (res)
		{
			return res;
		}
	}

	return node;
}

bool CQuadTree::SetLocalRoot(float x, float z, float radius)
{
	CQuadNode* node = node_contain_circle(m_pRoot, x, z, radius);

	if (NULL == node)
	{
		return false;
	}

	m_pLocalRoot = node;
	
	return true;
}

