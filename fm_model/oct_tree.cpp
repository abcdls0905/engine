//--------------------------------------------------------------------
// 文件名:		oct_tree.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "oct_tree.h"
#include "math_3d.h"
#include "model_player.h"
#include "../visual/vis_utils.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/portable.h"

// 获得贴图指定位置的ALPHA值
bool get_texture_alpha(ITexture* pTex, float u, float v, unsigned int* alpha);
// 获得三角形的贴图坐标
bool get_triangle_tex_uv(model_node_t* pNode, node_material_t* pMat, 
	unsigned int tri_id, FmVec2* uv0, FmVec2* uv1, FmVec2* uv2);

#pragma pack(push, 1)

struct triangle_info_t
{
	unsigned short nMatIndex;
	unsigned short nTriIndex;
};

#pragma pack(pop)

static CModelPlayer* s_pModelRef = NULL;
static model_node_t* s_pModelNode = NULL;
static COctTree* s_pTraceTree = NULL;
static FmVec3 s_vTraceSrc(0.0F, 0.0F, 0.0F);
static FmVec3 s_vTraceDir(0.0F, 0.0F, 0.0F);
static int s_nTraceAlphaRef = 0;
static bool s_bTraceOnlyFront = false;
static bool s_bTraceHitted = false;
static trace_info_t* s_TraceResult = NULL;
static FmVec3 s_vTraceCenter(0.0F, 0.0F, 0.0F);
static float s_fTraceRadius = 0.0F;
static const FmPlane* s_CullPlanes = NULL;
static size_t s_nCullPlaneNum = 0;
static CULL_FUNC s_CullFunc = NULL;
static void* s_CullContext = NULL;
static const FmMat4* s_CullMatrix = NULL;
static FmVec3 s_vTriangle0(0.0F, 0.0F, 0.0F);
static FmVec3 s_vTriangle1(0.0F, 0.0F, 0.0F);
static FmVec3 s_vTriangle2(0.0F, 0.0F, 0.0F);

// COctTree

COctTree::COctTree()
{
	m_pModelNode = NULL;
	m_vBoundMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoundMax = FmVec3(0.0F, 0.0F, 0.0F);
	m_pTreeData = NULL;
	m_nTreeSize = NULL;
}	

COctTree::~COctTree()
{
	if (m_pTreeData)
	{
		CORE_FREE(m_pTreeData, m_nTreeSize);
	}
}

void COctTree::Init(model_node_t* pModelNode, const FmVec3& bound_min, 
	const FmVec3& bound_max, char* tree_data, size_t tree_size)
{
	Assert(pModelNode != NULL);
	Assert(tree_data != NULL);

	m_pModelNode = pModelNode;
	m_vBoundMin = bound_min;
	m_vBoundMax = bound_max;
	m_pTreeData = tree_data;
	m_nTreeSize = tree_size;
}

bool COctTree::TraceDetail(CModelPlayer* ref, 
	const FmVec3& src, const FmVec3& dir, int alpha_ref, 
	bool only_front, trace_info_t& result)
{
	Assert(m_pTreeData != NULL);

	if (!FmBoxBoundProbe(&m_vBoundMin, &m_vBoundMax, &src, &dir))
	{
		// 先与包围盒做测试
		return false;
	}

	s_pModelRef = ref;
	s_pModelNode = m_pModelNode;
	s_pTraceTree = this;
	s_vTraceSrc = src;
	s_vTraceDir = dir;
	s_nTraceAlphaRef = alpha_ref;
	s_bTraceOnlyFront = only_front;
	s_bTraceHitted = false;
	s_TraceResult = &result;

	TraceNode((COctNode*)m_pTreeData);
	
	return s_bTraceHitted;
}

bool COctTree::TraceHitted(CModelPlayer* ref, 
	const FmVec3& src, const FmVec3& dir, int alpha_ref, 
	bool only_front)
{
	Assert(m_pTreeData != NULL);

	if (!FmBoxBoundProbe(&m_vBoundMin, &m_vBoundMax, &src, &dir))
	{
		// 先与包围盒做测试
		return false;
	}
	
	s_pModelRef = ref;
	s_pModelNode = m_pModelNode;
	s_pTraceTree = this;
	s_vTraceSrc = src;
	s_vTraceDir = dir;
	s_nTraceAlphaRef = alpha_ref;
	s_bTraceOnlyFront = only_front;
	s_TraceResult = NULL;

	return TraceNode((COctNode*)m_pTreeData);
}

bool COctTree::TraceNode(COctNode* node)
{
	// 这个还是用微软的效率高一些 除非改进上面的算法
	if (!FmBoxBoundProbe(&node->m_vBoundMin, &node->m_vBoundMax, 
		&s_vTraceSrc, &s_vTraceDir))
	{
		// 先与包围盒做测试
		return false;
	}
	
	size_t triangle_num = node->m_nTriangleNum;

	if (triangle_num > 0)
	{
		triangle_info_t* pTriangleInfo = 
			(triangle_info_t*)((char*)node + sizeof(COctNode));

		for (size_t k = 0; k < triangle_num; ++k)
		{
			unsigned int mat_index = pTriangleInfo[k].nMatIndex;
			unsigned int tri_index = pTriangleInfo[k].nTriIndex;

			Assert(mat_index < s_pModelNode->nMaterialCount);

			node_material_t* pMat = &s_pModelNode->Materials[mat_index];

			Assert(tri_index < (pMat->pLODIndexCount[0] / 3));

			unsigned short* pIB = pMat->pLODIB[0];
			vertex_data_t* pVB = &(pMat->SingleVB);
			//char* pVB = (char*)pMat->pSingleVB;
			//size_t stride = pMat->uSingleVertexSize;
			size_t index = tri_index * 3;
			size_t vi0 = pIB[index + 0];
			size_t vi1 = pIB[index + 1];
			size_t vi2 = pIB[index + 2];

			Assert(vi0 < pMat->nVertexCount);
			Assert(vi1 < pMat->nVertexCount);
			Assert(vi2 < pMat->nVertexCount);

			//FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
			//FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
			//FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
			FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
			FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
			FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

			float u;
			float v;
			float t;

			if (VisUtil_IntersectTri(v0, v1, v2, &s_vTraceSrc, &s_vTraceDir, 
				&u, &v, &t))
			{
				if ((t < 0.0F) || (t > 1.0F) || Port_IsNaN(t))
				{
					continue;
				}

				if (s_bTraceOnlyFront)
				{
					// 三角形对应的平面
					FmPlane plane;

					FmPlaneFromPoints(&plane, v0, v1, v2);
					
					// 射线起点到面的距离
					float dist = FmPlaneDotCoord(&plane, &s_vTraceSrc);

					if (dist < 0.0F)
					{
						// 不对反面进行碰撞
						continue;
					}
				}

				if ((s_nTraceAlphaRef > 0) && pMat->MatInfo.bAlphaTest
					&& ((NULL == s_TraceResult) 
					|| (s_TraceResult->nHitMax > 1)
					|| (t < s_TraceResult->fDistance)))
				{
					ITexture* pTex = pMat->MatInfo.DiffuseMap.pTex;

					if (pTex)
					{
						FmVec2 uv0;
						FmVec2 uv1;
						FmVec2 uv2;

						get_triangle_tex_uv(s_pModelNode, pMat, 
							tri_index * 3, &uv0, &uv1, &uv2);

						FmVec2 uv;

						uv = uv0 * (1.0F - u - v) + uv1 * u + uv2 * v;

						unsigned int alpha = 0;

						get_texture_alpha(pTex, uv.x, uv.y, &alpha);

						if (alpha < (unsigned int)s_nTraceAlphaRef)
						{
							continue;
						}
					}
					//else
					//{
					//	FXLOG_ERROR("[error 030024] FXVANodeRayTraceAlpha:The destination triangle has no diffuse texture!\n");
					//}
				}

				if (NULL == s_TraceResult)
				{
					// 只测试第一次碰撞
					return true;
				}

				if (!s_pModelRef->SetTraceInfo(s_TraceResult, s_pModelNode, 
					pMat, tri_index * 3, u, v, t, *v0, *v1, *v2))
				{
					return false;
				}
			}
		}
	}

	if (node->m_nChildNum > 0)
	{
		if (TravelChildren(node, TraceNode))
		{
			return true;
		}
	}

	return false;
}

bool COctTree::TraceSphere(CModelPlayer* ref, 
	const FmVec3& center, float radius)
{
	Assert(m_pTreeData != NULL);

	if (!VisUtil_BoxIntersectSphere(m_vBoundMin, m_vBoundMax, center, radius))
	{
		// 先与包围盒做测试
		return false;
	}

	s_pModelRef = ref;
	s_pModelNode = m_pModelNode;
	s_pTraceTree = this;
	s_vTraceCenter = center;
	s_fTraceRadius = radius;

	return TraceSphereNode((COctNode*)m_pTreeData);
}

// 三角形碰撞
bool COctTree::TraceTriangle(CModelPlayer* ref, const FmVec3& v0, 
    const FmVec3& v1, const FmVec3& v2)
{
    Assert(m_pTreeData != NULL);

    if (!math_intersect_bb_and_triangle(&m_vBoundMin, &m_vBoundMax, &v0, &v1, &v2))
    {
        // 先与包围盒做测试
        return false;
    }

    s_pModelRef = ref;
    s_pModelNode = m_pModelNode;
    s_pTraceTree = this;

    s_vTriangle0 = v0;
    s_vTriangle1 = v1;
    s_vTriangle2 = v2;

    return TraceTriangleNode((COctNode*)m_pTreeData);
}

bool COctTree::TraceSphereNode(COctNode* node)
{
	if (!VisUtil_BoxIntersectSphere(node->m_vBoundMin, node->m_vBoundMax, 
		s_vTraceCenter, s_fTraceRadius))
	{
		// 先与包围盒做测试
		return false;
	}

	size_t triangle_num = node->m_nTriangleNum;

	if (triangle_num > 0)
	{
		triangle_info_t* pTriangleInfo = 
			(triangle_info_t*)((char*)node + sizeof(COctNode));

		for (size_t k = 0; k < triangle_num; ++k)
		{
			unsigned int mat_index = pTriangleInfo[k].nMatIndex;
			unsigned int tri_index = pTriangleInfo[k].nTriIndex;

			Assert(mat_index < s_pModelNode->nMaterialCount);

			node_material_t* pMat = &s_pModelNode->Materials[mat_index];

			Assert(tri_index < (pMat->pLODIndexCount[0] / 3));

			unsigned short* pIB = pMat->pLODIB[0];
			vertex_data_t* pVB = &(pMat->SingleVB);
			//char* pVB = (char*)pMat->pSingleVB;
			//size_t stride = pMat->uSingleVertexSize;
			size_t index = tri_index * 3;
			size_t vi0 = pIB[index + 0];
			size_t vi1 = pIB[index + 1];
			size_t vi2 = pIB[index + 2];

			Assert(vi0 < pMat->nVertexCount);
			Assert(vi1 < pMat->nVertexCount);
			Assert(vi2 < pMat->nVertexCount);

			//FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
			//FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
			//FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
			FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
			FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
			FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

			if (math_sphere_triangle_collision(s_vTraceCenter, s_fTraceRadius, 
				*v0, *v1, *v2))
			{
				return true;
			}
		}
	}
	
	if (node->m_nChildNum > 0)
	{
		if (TravelChildren(node, TraceSphereNode))
		{
			return true;
		}
	}

	return false;
}

bool COctTree::TraceTriangleNode(COctNode* node)
{
    if (!math_intersect_bb_and_triangle(&node->m_vBoundMin, &node->m_vBoundMax, 
            &s_vTriangle0, &s_vTriangle1, &s_vTriangle2))
    {
        // 先与包围盒做测试
        return false;
    }

    size_t triangle_num = node->m_nTriangleNum;

    if (triangle_num > 0)
    {
        triangle_info_t* pTriangleInfo = 
            (triangle_info_t*)((char*)node + sizeof(COctNode));

        for (size_t k = 0; k < triangle_num; ++k)
        {
            unsigned int mat_index = pTriangleInfo[k].nMatIndex;
            unsigned int tri_index = pTriangleInfo[k].nTriIndex;

            Assert(mat_index < s_pModelNode->nMaterialCount);

            node_material_t* pMat = &s_pModelNode->Materials[mat_index];

            Assert(tri_index < (pMat->pLODIndexCount[0] / 3));

            unsigned short* pIB = pMat->pLODIB[0];
            vertex_data_t* pVB = &(pMat->SingleVB);
            //char* pVB = (char*)pMat->pSingleVB;
            //size_t stride = pMat->uSingleVertexSize;
            size_t index = tri_index * 3;
            size_t vi0 = pIB[index + 0];
            size_t vi1 = pIB[index + 1];
            size_t vi2 = pIB[index + 2];

            Assert(vi0 < pMat->nVertexCount);
            Assert(vi1 < pMat->nVertexCount);
            Assert(vi2 < pMat->nVertexCount);

            //FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
            //FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
            //FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
            FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
            FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
            FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

            if (math_triangle_triangle_collision(s_vTriangle0, s_vTriangle1, s_vTriangle2, 
                    *v0, *v1, *v2))
            {
                return true;
            }
        }
    }

    if (node->m_nChildNum > 0)
    {
        if (TravelChildren(node, TraceSphereNode))
        {
            return true;
        }
    }

    return false;
}

bool COctTree::Cull(CModelPlayer* ref, const FmPlane* planes, 
	size_t plane_num, const FmVec3& center, float radius, 
	CULL_FUNC cull_func, void* context, const FmMat4& mat)
{
	Assert(m_pTreeData != NULL);

	if (!VisUtil_BoxIntersectSphere(m_vBoundMin, m_vBoundMax, center, radius))
	{
		// 先与包围盒做测试
		return false;
	}

	s_pModelRef = ref;
	s_pModelNode = m_pModelNode;
	s_pTraceTree = this;
	s_vTraceCenter = center;
	s_fTraceRadius = radius;
	s_CullPlanes = planes;
	s_nCullPlaneNum = plane_num;
	s_CullFunc = cull_func;
	s_CullContext = context;
	s_CullMatrix = &mat;

	CullNode((COctNode*)m_pTreeData);

	return true;
}

bool COctTree::CullNode(COctNode* node)
{
	if (!VisUtil_BoxIntersectSphere(node->m_vBoundMin, node->m_vBoundMax, 
		s_vTraceCenter, s_fTraceRadius))
	{
		// 先与包围盒做测试
		return false;
	}

	size_t triangle_num = node->m_nTriangleNum;

	if (triangle_num > 0)
	{
		triangle_info_t* pTriangleInfo = 
			(triangle_info_t*)((char*)node + sizeof(COctNode));

		FmVec3 v[3];

		for (size_t k = 0; k < triangle_num; ++k)
		{
			unsigned int mat_index = pTriangleInfo[k].nMatIndex;
			unsigned int tri_index = pTriangleInfo[k].nTriIndex;

			Assert(mat_index < s_pModelNode->nMaterialCount);

			node_material_t* pMat = &s_pModelNode->Materials[mat_index];

			Assert(tri_index < (pMat->pLODIndexCount[0] / 3));

			unsigned short* pIB = pMat->pLODIB[0];
			vertex_data_t* pVB = &(pMat->SingleVB);
			//char* pVB = (char*)pMat->pSingleVB;
			//size_t stride = pMat->uSingleVertexSize;
			size_t index = tri_index * 3;
			size_t vi0 = pIB[index + 0];
			size_t vi1 = pIB[index + 1];
			size_t vi2 = pIB[index + 2];

			Assert(vi0 < pMat->nVertexCount);
			Assert(vi1 < pMat->nVertexCount);
			Assert(vi2 < pMat->nVertexCount);

			//FmVec3* v0 = (FmVec3*)(pVB + vi0 * stride);
			//FmVec3* v1 = (FmVec3*)(pVB + vi1 * stride);
			//FmVec3* v2 = (FmVec3*)(pVB + vi2 * stride);
			FmVec3* v0 = (FmVec3*)get_vertex_by_index(pVB, vi0);
			FmVec3* v1 = (FmVec3*)get_vertex_by_index(pVB, vi1);
			FmVec3* v2 = (FmVec3*)get_vertex_by_index(pVB, vi2);

			if (s_nCullPlaneNum > 0)
			{
				// 使用剪裁面
				FmVec3 bound_min = *v0;
				FmVec3 bound_max = *v0;

				FmVec3Minimize(&bound_min, &bound_min, v1);
				FmVec3Minimize(&bound_min, &bound_min, v2);
				FmVec3Maximize(&bound_max, &bound_max, v1);
				FmVec3Maximize(&bound_max, &bound_max, v2);

				FmVec3 center = (bound_min + bound_max) * 0.5F;
				float sx = (bound_max.x - bound_min.x) * 0.5F;
				float sy = (bound_max.y - bound_min.y) * 0.5F;
				float sz = (bound_max.z - bound_min.z) * 0.5F;
				float radius_sq = sx * sx + sy * sy + sz * sz;
				bool bIn = true;
				
				for (size_t p = 0; p < s_nCullPlaneNum; p++)
				{
					float dist = FmPlaneDotCoord(&s_CullPlanes[p], &center);

					if (dist < 0.0F)
					{
						continue;
					}

					if ((dist * dist) > radius_sq)
					{
						bIn = false;
						break;
					}
				}

				if (!bIn)
				{
					continue;
				}
			}
			else
			{
				// 只用球体做碰撞检测
				if (!math_sphere_triangle_aabb_collision(s_vTraceCenter, 
					s_fTraceRadius, *v0, *v1, *v2))
				{
					continue;
				}
			}
			
			FmVec3TransformCoord(&v[0], v0, &s_pModelRef->GetCurrentTM());
			FmVec3TransformCoord(&v[1], v1, &s_pModelRef->GetCurrentTM());
			FmVec3TransformCoord(&v[2], v2, &s_pModelRef->GetCurrentTM());

			FmMat4 mtxWorldInverse;

			FmMat4Inverse(&mtxWorldInverse, NULL, 
				&s_pModelRef->GetWorldTM());

		//	FmVec3TransformCoordArray(&v[0], sizeof(FmVec3), 
		//		&v[0], sizeof(FmVec3), &mtxWorldInverse, 3);

			for( int j = 0; j < 3 ; j++ )
				FmVec3TransformCoord( &v[j],  &v[j], &mtxWorldInverse );


			//v[0] = *v0;
			//v[1] = *v1;
			//v[2] = *v2;

			if (!s_CullFunc(s_CullContext, *s_CullMatrix, &v[0], 3))
			{
				return true;
			}
		}
	}

	if (node->m_nChildNum > 0)
	{
		if (TravelChildren(node, CullNode))
		{
			return true;
		}
	}

	return false;
}

bool COctTree::TravelChildren(COctNode* node, 
	bool (*travel_func)(COctNode*))
{
	char* p = (char*)node + sizeof(COctNode) 
		+ node->m_nTriangleNum * sizeof(unsigned int);
	unsigned int child_mask = node->m_nChildMask;
	char* tree_data = s_pTraceTree->m_pTreeData;

	if (child_mask & 0x1)
	{
		// 节点000		
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x2)
	{
		// 节点001
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x4)
	{
		// 节点010
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x8)
	{
		// 节点011
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x10)
	{
		// 节点100
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x20)
	{
		// 节点101
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x40)
	{
		// 节点110
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	if (child_mask & 0x80)
	{
		// 节点111
		unsigned int child_offset = *(unsigned int*)p;

		Assert(child_offset < s_pTraceTree->m_nTreeSize);

		COctNode* pChild = (COctNode*)(tree_data + child_offset);

		if (travel_func(pChild))
		{
			return true;
		}

		p += sizeof(unsigned int);
	}

	return false;
}

