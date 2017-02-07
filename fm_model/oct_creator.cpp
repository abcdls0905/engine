//--------------------------------------------------------------------
// 文件名:		oct_creator.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "oct_creator.h"
#include "oct_tree.h"
#include "model_data.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// 误差范围
#define DELTA 0.0001F

// COctCreator

COctCreator::COctCreator()
{
	m_pOctTree = NULL;
}

COctCreator::~COctCreator()
{
	if (m_pOctTree)
	{
		CORE_DELETE(m_pOctTree);
	}
}

bool COctCreator::Clear()
{
	// 数据所有权已经转移
	m_pOctTree = NULL;
	return true;
}

bool COctCreator::Create(model_node_t* pModelNode)
{
	Assert(pModelNode != NULL);

	unsigned int mat_num = pModelNode->nMaterialCount;
	
	Assert(mat_num > 0);
	
	unsigned int triangle_num = 0;

	// 先求出包围盒
	FmVec3 bound_min(1e8, 1e8, 1e8);
	FmVec3 bound_max(-1e8, -1e8, -1e8);

	for (unsigned int mat = 0; mat < mat_num; ++mat)
	{
		node_material_t* pMat = &pModelNode->Materials[mat];
		unsigned short* pIB = pMat->pLODIB[0];
		vertex_data_t* pVB = &(pMat->SingleVB);
		//char* pVB = (char*)pMat->pSingleVB;
		//unsigned int stride = pMat->uSingleVertexSize;
		unsigned int mat_tri_num = pMat->pLODIndexCount[0] / 3;
		unsigned int mat_vtx_num = pMat->nVertexCount;
		unsigned int index = 0;

		for (unsigned int tri = 0; tri < mat_tri_num; ++tri)
		{
			unsigned int vi0 = pIB[index + 0];
			unsigned int vi1 = pIB[index + 1];
			unsigned int vi2 = pIB[index + 2];

			index += 3;

			if ((vi0 >= mat_vtx_num) || (vi1 >= mat_vtx_num) 
				|| (vi2 >= mat_vtx_num))
			{
				CORE_TRACE("(COctCreator::CreateOctTree)vertex error");
				return false;
			}

			//FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
			//FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
			//FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
			FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
			FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
			FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

			FmVec3Minimize(&bound_min, &bound_min, v0);
			FmVec3Minimize(&bound_min, &bound_min, v1);
			FmVec3Minimize(&bound_min, &bound_min, v2);
			FmVec3Maximize(&bound_max, &bound_max, v0);
			FmVec3Maximize(&bound_max, &bound_max, v1);
			FmVec3Maximize(&bound_max, &bound_max, v2);
		}

		triangle_num += mat_tri_num;
	}
	
	// 加上误差范围防止浮点误差
	bound_min.x -= DELTA;
	bound_min.y -= DELTA;
	bound_min.z -= DELTA;
	bound_max.x += DELTA;
	bound_max.y += DELTA;
	bound_max.z += DELTA;

	// 计算八叉树的最大深度
	m_nMaxDepth = 0;

	size_t tri_num = triangle_num;

	while (tri_num > 0)
	{
		tri_num >>= 3;
		m_nMaxDepth++;
	}

	m_TriangleNode.resize(triangle_num, NULL);

	// 根节点
	oct_node_t* root = m_NodeBucket.New();

	memset(root, 0, sizeof(oct_node_t));
	root->vBoundMin = bound_min;
	root->vBoundMax = bound_max;

	// 节点计数
	m_nTotalNodes = 1;

	unsigned int tri_count = 0;

	for (unsigned int mat = 0; mat < mat_num; ++mat)
	{
		node_material_t* pMat = &pModelNode->Materials[mat];
		unsigned short* pIB = pMat->pLODIB[0];
		vertex_data_t* pVB = &(pMat->SingleVB);
		//char* pVB = (char*)pMat->pSingleVB;
		//unsigned int stride = pMat->uSingleVertexSize;
		unsigned int mat_tri_num = pMat->pLODIndexCount[0] / 3;
		unsigned int index = 0;

		for (unsigned int tri = 0; tri < mat_tri_num; ++tri)
		{
			unsigned int vi0 = pIB[index + 0];
			unsigned int vi1 = pIB[index + 1];
			unsigned int vi2 = pIB[index + 2];

			index += 3;

			//FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
			//FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
			//FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
			FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
			FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
			FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

			// 三角形的包围盒
			m_vTriangleMin = *v0;
			m_vTriangleMax = *v0;

			FmVec3Minimize(&m_vTriangleMin, &m_vTriangleMin, v1);
			FmVec3Minimize(&m_vTriangleMin, &m_vTriangleMin, v2);
			FmVec3Maximize(&m_vTriangleMax, &m_vTriangleMax, v1);
			FmVec3Maximize(&m_vTriangleMax, &m_vTriangleMax, v2);

			m_vTriangleMin.x -= DELTA;
			m_vTriangleMin.y -= DELTA;
			m_vTriangleMin.z -= DELTA;
			m_vTriangleMax.x += DELTA;
			m_vTriangleMax.y += DELTA;
			m_vTriangleMax.z += DELTA;
			m_vTriangleCenter = (m_vTriangleMin + m_vTriangleMax) * 0.5F;

			AddToOctNodeLoose(root, tri_count + tri, bound_min, bound_max);
		}

		tri_count += mat_tri_num;
	}

	// 最终的八叉树数据
	m_nTreeSize = m_nTotalNodes * sizeof(COctNode) 
		+ m_nTotalNodes * sizeof(unsigned int) 
		+ triangle_num * sizeof(unsigned int) - sizeof(unsigned int);
	m_pTreeData = (char*)CORE_ALLOC(m_nTreeSize);

	// 压缩八叉树数据
	m_nDataCount = 0;
	
	CompressOctNode(root, m_pTreeData);

	Assert(m_nDataCount == m_nTreeSize);

	// 三角形信息写入节点
	tri_count = 0;

	for (unsigned int mat = 0; mat < mat_num; ++mat)
	{
		node_material_t* pMat = &pModelNode->Materials[mat];
		unsigned int mat_tri_num = pMat->pLODIndexCount[0] / 3;

		for (unsigned int tri = 0; tri < mat_tri_num; ++tri)
		{
			COctNode* pNode = m_TriangleNode[tri_count + tri]->pFinalNode;
			char* p = (char*)pNode + sizeof(COctNode) 
				+ pNode->m_nTriangleNum * sizeof(unsigned int);

			*(unsigned short*)p = mat;
			*(unsigned short*)(p + sizeof(unsigned short)) = tri;
			pNode->m_nTriangleNum++;
		}

		tri_count += mat_tri_num;
	}

	// 建立最终的八叉树
	COctTree* pOctTree = CORE_NEW(COctTree);

	pOctTree->Init(pModelNode, bound_min, bound_max, m_pTreeData, m_nTreeSize);
	m_pOctTree = pOctTree;
	
	return true;
}

/*
bool COctCreator::AddToOctNode(oct_node_t* node, size_t triangle)
{
	if (node->nDepth >= m_nMaxDepth)
	{
		// 已经达到树的最大深度
		node->nTriangleCount++;
		m_TriangleNode[triangle] = node;
		return true;
	}

	size_t flag1 = 0;
	size_t flag2 = 0;

	FmVec3 v_min = node->vBoundMin;
	FmVec3 v_max = node->vBoundMax;
	FmVec3 v_center = (v_min + v_max) * 0.5F;

	if (m_vTriangleMin.x >= v_center.x)
	{
		flag1 |= 0x1;
	}

	if (m_vTriangleMin.y >= v_center.y)
	{
		flag1 |= 0x2;
	}

	if (m_vTriangleMin.z >= v_center.z)
	{
		flag1 |= 0x4;
	}

	if (m_vTriangleMax.x >= v_center.x)
	{
		flag2 |= 0x1;
	}

	if (m_vTriangleMax.y >= v_center.y)
	{
		flag2 |= 0x2;
	}

	if (m_vTriangleMax.z >= v_center.z)
	{
		flag2 |= 0x4;
	}

	if (flag1 == flag2)
	{
		oct_node_t* child = node->pChildren[flag1];

		if (NULL == child)
		{
			child = m_NodeBucket.New();

			memset(child, 0, sizeof(oct_node_t));

			FmVec3 child_min;
			FmVec3 child_max;

			switch (flag1)
			{
			case 0:
				child_min.x = v_min.x;
				child_min.y = v_min.y;
				child_min.z = v_min.z;
				child_max.x = v_center.x;
				child_max.y = v_center.y;
				child_max.z = v_center.z;
				break;
			case 1:
				child_min.x = v_center.x;
				child_min.y = v_min.y;
				child_min.z = v_min.z;
				child_max.x = v_max.x;
				child_max.y = v_center.y;
				child_max.z = v_center.z;
				break;
			case 2:
				child_min.x = v_min.x;
				child_min.y = v_center.y;
				child_min.z = v_min.z;
				child_max.x = v_center.x;
				child_max.y = v_max.y;
				child_max.z = v_center.z;
				break;
			case 3:
				child_min.x = v_center.x;
				child_min.y = v_center.y;
				child_min.z = v_min.z;
				child_max.x = v_max.x;
				child_max.y = v_max.y;
				child_max.z = v_center.z;
				break;
			case 4:
				child_min.x = v_min.x;
				child_min.y = v_min.y;
				child_min.z = v_center.z;
				child_max.x = v_center.x;
				child_max.y = v_center.y;
				child_max.z = v_max.z;
				break;
			case 5:
				child_min.x = v_center.x;
				child_min.y = v_min.y;
				child_min.z = v_center.z;
				child_max.x = v_max.x;
				child_max.y = v_center.y;
				child_max.z = v_max.z;
				break;
			case 6:
				child_min.x = v_min.x;
				child_min.y = v_center.y;
				child_min.z = v_center.z;
				child_max.x = v_center.x;
				child_max.y = v_max.y;
				child_max.z = v_max.z;
				break;
			case 7:
				child_min.x = v_center.x;
				child_min.y = v_center.y;
				child_min.z = v_center.z;
				child_max.x = v_max.x;
				child_max.y = v_max.y;
				child_max.z = v_max.z;
				break;
			}

			child->vBoundMin = child_min;
			child->vBoundMax = child_max;
			child->nDepth = node->nDepth + 1;

			node->pChildren[flag1] = child;

			m_nTotalNodes++;
		}

		return AddToOctNode(child, triangle);
	}
	else
	{
		// 不能容于任何一个子节点
		node->nTriangleCount++;
		m_TriangleNode[triangle] = node;
		return true;
	}
}
*/

inline bool box_in_box(const FmVec3& out_min, const FmVec3& out_max,
	const FmVec3& in_min, const FmVec3& in_max)
{
	return (in_min.x >= out_min.x) && (in_min.y >= out_min.y) 
		&& (in_min.z >= out_min.z)&& (in_max.x <= out_max.x) 
		&& (in_max.y <= out_max.y) && (in_max.z <= out_max.z);
}

bool COctCreator::AddToOctNodeLoose(oct_node_t* node, size_t tri_index, 
	const FmVec3& v_min, const FmVec3& v_max)
{
	if (node->nDepth >= m_nMaxDepth)
	{
		// 已经达到树的最大深度
		node->nTriangleCount++;
		m_TriangleNode[tri_index] = node;
		return true;
	}

	FmVec3 v_center = (v_min + v_max) * 0.5F;
	size_t flag1 = 0;

	if (m_vTriangleCenter.x >= v_center.x)
	{
		flag1 |= 0x1;
	}

	if (m_vTriangleCenter.y >= v_center.y)
	{
		flag1 |= 0x2;
	}

	if (m_vTriangleCenter.z >= v_center.z)
	{
		flag1 |= 0x4;
	}

	FmVec3 child_min;
	FmVec3 child_max;

	switch (flag1)
	{
	case 0:
		child_min.x = v_min.x;
		child_min.y = v_min.y;
		child_min.z = v_min.z;
		child_max.x = v_center.x;
		child_max.y = v_center.y;
		child_max.z = v_center.z;
		break;
	case 1:
		child_min.x = v_center.x;
		child_min.y = v_min.y;
		child_min.z = v_min.z;
		child_max.x = v_max.x;
		child_max.y = v_center.y;
		child_max.z = v_center.z;
		break;
	case 2:
		child_min.x = v_min.x;
		child_min.y = v_center.y;
		child_min.z = v_min.z;
		child_max.x = v_center.x;
		child_max.y = v_max.y;
		child_max.z = v_center.z;
		break;
	case 3:
		child_min.x = v_center.x;
		child_min.y = v_center.y;
		child_min.z = v_min.z;
		child_max.x = v_max.x;
		child_max.y = v_max.y;
		child_max.z = v_center.z;
		break;
	case 4:
		child_min.x = v_min.x;
		child_min.y = v_min.y;
		child_min.z = v_center.z;
		child_max.x = v_center.x;
		child_max.y = v_center.y;
		child_max.z = v_max.z;
		break;
	case 5:
		child_min.x = v_center.x;
		child_min.y = v_min.y;
		child_min.z = v_center.z;
		child_max.x = v_max.x;
		child_max.y = v_center.y;
		child_max.z = v_max.z;
		break;
	case 6:
		child_min.x = v_min.x;
		child_min.y = v_center.y;
		child_min.z = v_center.z;
		child_max.x = v_center.x;
		child_max.y = v_max.y;
		child_max.z = v_max.z;
		break;
	case 7:
		child_min.x = v_center.x;
		child_min.y = v_center.y;
		child_min.z = v_center.z;
		child_max.x = v_max.x;
		child_max.y = v_max.y;
		child_max.z = v_max.z;
		break;
	default:
		Assert(0);
		break;
	}

	FmVec3 child_center = (child_min + child_max) * 0.5F;
	FmVec3 child_box_size = child_max - child_min;
	FmVec3 child_loose_min = child_center - child_box_size;
	FmVec3 child_loose_max = child_center + child_box_size;

	if (box_in_box(child_loose_min, child_loose_max, m_vTriangleMin, 
		m_vTriangleMax))
	{
		oct_node_t* child = node->pChildren[flag1];

		if (NULL == child)
		{
			child = m_NodeBucket.New();

			memset(child, 0, sizeof(oct_node_t));
			child->vBoundMin = child_loose_min;
			child->vBoundMax = child_loose_max;
			child->nDepth = node->nDepth + 1;

			node->pChildren[flag1] = child;
			m_nTotalNodes++;
		}

		return AddToOctNodeLoose(child, tri_index, child_min, child_max);
	}
	else
	{
		// 不能容纳于子节点
		node->nTriangleCount++;
		m_TriangleNode[tri_index] = node;
		return true;
	}
}

size_t COctCreator::GetOctNodeSize(oct_node_t* node)
{
	size_t count = sizeof(COctNode) 
		+ node->nTriangleCount * sizeof(unsigned int);

	for (size_t i = 0; i < 8; ++i)
	{
		if (node->pChildren[i])
		{
			count += sizeof(unsigned int);
		}
	}

	return count;
}

COctNode* COctCreator::CompressOctNode(oct_node_t* node, char* pdata)
{
	m_nDataCount += GetOctNodeSize(node);

	COctNode* pNode = (COctNode*)pdata;

	// 记录最终的节点指针
	node->pFinalNode = pNode;

	// 三角形数量先设置为0
	pNode->m_nTriangleNum = 0;

	char* p = pdata + sizeof(COctNode) 
		+ node->nTriangleCount * sizeof(unsigned int);
	size_t child_count = 0;
	size_t child_mask = 0;

	for (size_t i = 0; i < 8; ++i)
	{
		if (node->pChildren[i])
		{
			COctNode* pChild = CompressOctNode(node->pChildren[i], 
				m_pTreeData + m_nDataCount);

			// 节点的偏移值
			size_t offset = (char*)pChild - m_pTreeData;

			*(unsigned int*)p = (unsigned int)offset;
			p += sizeof(unsigned int);

			child_mask |= 0x1 << i;
			child_count++;
		}
	}

	pNode->m_nChildNum = (unsigned int)child_count;
	pNode->m_nChildMask = (unsigned int)child_mask;
	pNode->m_vBoundMin = node->vBoundMin;
	pNode->m_vBoundMax = node->vBoundMax;

	return pNode;
}

