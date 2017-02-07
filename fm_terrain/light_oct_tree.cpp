//--------------------------------------------------------------------
// 文件名:		light_oct_tree.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "light_oct_tree.h"
#include "light_geometry.h"
#include "../public/core_mem.h"

#define LLOT_MAX 128

// CLightOctTree

CLightOctTree::oct_node_t::oct_node_t(const FmVec4& v_min, 
	const FmVec4& v_max)
{
	for (size_t i = 0; i < 8; i++)
	{
		node[i] = NULL;
	}
	
	min = v_min; 
	max = v_max;
	vrt = (vertex_t**)CORE_ALLOC(sizeof(vertex_t*) * LLOT_MAX);
	num = 0;
}

CLightOctTree::oct_node_t::~oct_node_t()
{
	if (vrt)
	{
		CORE_FREE(vrt, sizeof(vertex_t*) * LLOT_MAX);
	}
	
	for (size_t i = 0; i < 8; i++)
	{
		if (node[i])
		{
			CORE_DELETE(node[i]);
		}
	}
}

CLightOctTree::CLightOctTree()
{
	m_pRoot = NULL;
}

CLightOctTree::~CLightOctTree()
{
	if (m_pRoot)
	{
		CORE_DELETE(m_pRoot);
	}
}

void CLightOctTree::Init(CLightGeometry* g)
{	
	vertex_t** vertices = g->GetVertices();
	size_t vertex_num = g->GetVertexCount();

	m_pRoot = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
	new (m_pRoot) oct_node_t(g->GetMin(), g->GetMax());

	for (size_t i = 0; i < vertex_num; i++)
	{
		AddVertex(m_pRoot, vertices[i]);
	}
	
	Optimize(m_pRoot);
}

size_t CLightOctTree::Check(oct_node_t* node, vertex_t* v, size_t num)
{
	if (node->vrt)
	{
		for (size_t i = 0; i < node->num; i++)
		{
			if (node->vrt[i] == v)
			{
				num++;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (node->node[i])
			{
				num += Check(node->node[i], v, 0);
			}
		}
	}

	return num;
}

bool CLightOctTree::AddVertex(oct_node_t* node, vertex_t* v)
{
	FmVec4& min = node->min;
	FmVec4& max = node->max;

	if (v->p.x < min.x || v->p.x > max.x)
	{
		return false;
	}

	if (v->p.y < min.y || v->p.y > max.y)
	{
		return false;
	}

	if (v->p.z < min.z || v->p.z > max.z)
	{
		return false;
	}

	if (node->vrt)
	{
		if (node->num < LLOT_MAX)
		{
			node->vrt[node->num++] = v;
			return true;
		}
		else
		{
			FmVec4 cnt = (node->min + node->max) * 0.5F;
			
			node->node[0] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[0]) oct_node_t(
				FmVec4(min.x, min.y, min.z), 
				FmVec4(cnt.x, cnt.y, cnt.z));

			node->node[1] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[1]) oct_node_t(
				FmVec4(min.x, min.y, cnt.z), 
				FmVec4(cnt.x, cnt.y, max.z));

			node->node[2] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[2]) oct_node_t(
				FmVec4(cnt.x, min.y, cnt.z), 
				FmVec4(max.x, cnt.y, max.z));

			node->node[3] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[3]) oct_node_t(
				FmVec4(cnt.x, min.y, min.z), 
				FmVec4(max.x, cnt.y, cnt.z));

			node->node[4] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[4]) oct_node_t(
				FmVec4(min.x, cnt.y, min.z), 
				FmVec4(cnt.x, max.y, cnt.z));

			node->node[5] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[5]) oct_node_t(
				FmVec4(min.x, cnt.y, cnt.z), 
				FmVec4(cnt.x, max.y, max.z));

			node->node[6] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[6]) oct_node_t(
				FmVec4(cnt.x, cnt.y, cnt.z), 
				FmVec4(max.x, max.y, max.z));

			node->node[7] = (oct_node_t*)CORE_ALLOC(sizeof(oct_node_t));
			new (node->node[7]) oct_node_t(
				FmVec4(cnt.x, cnt.y, min.z), 
				FmVec4(max.x, max.y, cnt.z));
			
			for (size_t n = 0; n < node->num; n++)
			{
				size_t c;
				
				for (c = 0; c < 8; c++)
				{
					if (AddVertex(node->node[c], node->vrt[n]))
					{
						break;
					}
				}

				Assert(c < 8);
			}

			CORE_FREE(node->vrt, sizeof(vertex_t*) * LLOT_MAX);
			node->vrt = NULL;
			node->num = 0;
		}
	}

	size_t c;

	for (c = 0; c < 8; c++)
	{
		if (AddVertex(node->node[c], v))
		{
			break;
		}
	}

	Assert(c < 8);	
	
	return true;
}

void CLightOctTree::Optimize(oct_node_t* node)
{
	if (!node->vrt)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (!node->node[i])
			{
				continue;
			}

			if (node->node[i]->vrt)
			{
				if (node->node[i]->num <= 0)
				{
					CORE_DELETE(node->node[i]);
					node->node[i] = NULL;
				}
			}
			else
			{
				Optimize(node->node[i]);
			}
		}
	}
}

void CLightOctTree::FindVerts(FmVec4& pos, float r)
{
	m_FindVertices.clear();
	
	m_vFindPos = pos;
	m_fFindSquare = r * r;

	r += 0.000001F;
	
	m_vFindPosMin = pos - FmVec4(r, r, r);
	m_vFindPosMax = pos + FmVec4(r, r, r);
	
	if (m_pRoot)
	{
		FindNodeVerts(m_pRoot);
	}
}

void CLightOctTree::FindNodeVerts(oct_node_t* node)
{
	FmVec4& min = node->min;
	FmVec4& max = node->max;

	if (m_vFindPosMin.x > max.x)
	{
		return;
	}

	if (m_vFindPosMax.x < min.x)
	{
		return;
	}

	if (m_vFindPosMin.y > max.y)
	{
		return;
	}

	if (m_vFindPosMax.y < min.y)
	{
		return;
	}

	if (m_vFindPosMin.z > max.z)
	{
		return;
	}

	if (m_vFindPosMax.z < min.z)
	{
		return;
	}

	if (node->vrt == NULL)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			if (node->node[i])
			{
				FindNodeVerts(node->node[i]);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < node->num; i++)
		{
			FmVec4& v = node->vrt[i]->p;
			float sx = v.x - m_vFindPos.x;
			float sy = v.y - m_vFindPos.y;
			float sz = v.z - m_vFindPos.z;
			float r = sx * sx + sy * sy + sz * sz;
			
			if (r < m_fFindSquare)
			{
				m_FindVertices.push_back(oct_find_vertex_t());

				oct_find_vertex_t& verts = m_FindVertices.back();

				verts.v = node->vrt[i];
				verts.r2 = r;
			}
		}
	}
}
*/
