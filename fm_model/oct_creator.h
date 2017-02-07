//--------------------------------------------------------------------
// 文件名:		oct_creator.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _OCT_CREATOR_H
#define _OCT_CREATOR_H

#include "../public/macros.h"
#include "../public/core_mem.h"

#include "../utils/array_pod.h"
#include "../utils/memory_bucket.h"
#include "../math/fm_math.h"

class TResGeoOctCreatorAlloc
{
public:
	TResGeoOctCreatorAlloc() {}
	~TResGeoOctCreatorAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TResGeoOctCreatorAlloc& src) {}
};

// 模型八叉树创建

class COctTree;
class COctNode;
struct model_node_t;

class COctCreator
{
private:
	struct oct_node_t
	{
		FmVec3 vBoundMin;
		FmVec3 vBoundMax;
		size_t nDepth;
		size_t nTriangleCount;
		oct_node_t* pChildren[8];
		COctNode* pFinalNode;
	};

public:
	COctCreator();
	~COctCreator();
	
	// 获得八叉树
	COctTree* GetOctTree() const { return m_pOctTree; }
	// 释放八叉树的所有权
	bool Clear();
	// 创建八叉树
	bool Create(model_node_t* pModelNode);
	
private:
	COctCreator(const COctCreator&);
	COctCreator& operator=(const COctCreator&);

	// 添加三角形到树中
	bool AddToOctNodeLoose(oct_node_t* node, size_t tri_index, 
		const FmVec3& v_min, const FmVec3& v_max);
	// 获得节点数据大小
	size_t GetOctNodeSize(oct_node_t* node);
	// 压缩节点数据
	COctNode* CompressOctNode(oct_node_t* node, char* pdata);
	// 删除节点
	void DeleteOctNode(oct_node_t* node);
	
private:
	COctTree* m_pOctTree;
	TMemoryBucket<oct_node_t, 256, TResGeoOctCreatorAlloc> m_NodeBucket; // 节点内存
	TArrayPod<oct_node_t*, 1, TResGeoOctCreatorAlloc> m_TriangleNode; // 记录三角形所属的节点
	char* m_pTreeData;		// 最终的八叉树数据	
	size_t m_nTreeSize;		// 最终的八叉树数据大小
	size_t m_nTotalNodes;	// 总节点数 
	size_t m_nDataCount;	// 数据计数
	size_t m_nMaxDepth;		// 八叉树最大深度
	FmVec3 m_vTriangleMin;
	FmVec3 m_vTriangleMax;
	FmVec3 m_vTriangleCenter;
};

#endif // _OCT_CREATOR_H
