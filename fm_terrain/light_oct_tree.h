//--------------------------------------------------------------------
// 文件名:		light_oct_tree.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_OCT_TREE_H
#define _LIGHT_OCT_TREE_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../visual/dx_header.h"
#include "light_types.h"

// 光照渲染空间管理

class CLightGeometry;

class CLightOctTree  
{
private:
	struct oct_node_t
	{
		oct_node_t(const FmVec4& v_min, const FmVec4& v_max);
		~oct_node_t();

		oct_node_t* node[8];
		FmVec4 min;
		FmVec4 max;
		vertex_t** vrt;
		size_t num;
	};

public:
	CLightOctTree();
	~CLightOctTree();

	// 初始化
	void Init(CLightGeometry* g);	
	// 查找指定范围内的顶点
	void FindVerts(FmVec4& pos, float r);

	// 返回查找结果
	size_t GetFindVertexCount() const { return m_FindVertices.size(); }
	oct_find_vertex_t* GetFindVertices() { return &m_FindVertices[0]; }

private:
	bool AddVertex(oct_node_t* node, vertex_t* v);
	void Optimize(oct_node_t* node);
	void FindNodeVerts(oct_node_t* node);
	size_t Check(oct_node_t* node, vertex_t* v, size_t num);

private:
	oct_node_t* m_pRoot;	
	FmVec4 m_vFindPos;
	FmVec4 m_vFindPosMin;
	FmVec4 m_vFindPosMax;
	float m_fFindSquare;
	TArrayPod<oct_find_vertex_t, 1, TCoreAlloc> m_FindVertices;
};

#endif // _LIGHT_OCT_TREE_H
*/
