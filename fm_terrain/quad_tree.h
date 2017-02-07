//--------------------------------------------------------------------
// 文件名:		quad_tree.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _QUAD_TREE_H
#define _QUAD_TREE_H

#include "../public/macros.h"
#include "../public/core_mem.h"


#include "../utils/memory_bucket.h"
#include "quad_node.h"

class TQuadTreeAlloc
{
public:
	TQuadTreeAlloc() {}
	~TQuadTreeAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TQuadTreeAlloc& src) {}
};

// 四叉树

class CQuadNode;

class CQuadTree
{
private:
	struct node_pool_t
	{
		CQuadNode data;
		node_pool_t* pNext;
	};

public:
	CQuadTree();
	~CQuadTree();

	// 释放
	void ReleaseAll();
	
	// 创建
	bool Create(size_t scale, float x, float z);

	// 新建节点
	CQuadNode* NewNode(CQuadNode* parent, size_t scale, float x, float z);
	// 删除节点
	void DeleteNode(CQuadNode* node);

	// 设置当前区域的根节点
	bool SetLocalRoot(float x, float z, float radius);

	// 获得根节点
	CQuadNode* GetRoot() const { return m_pRoot; }
	// 获得当前区域的根节点
	CQuadNode* GetLocalRoot() const { return m_pLocalRoot; }
	// 节点计数
	size_t GetNodeCount() const { return m_nNodeCount; }

	// 对应地形区域的节点尺寸
	void SetZoneScale(size_t value) { m_nZoneScale = value; }
	size_t GetZoneScale() const { return m_nZoneScale; }

private:
	CQuadTree(const CQuadTree&);
	CQuadTree& operator=(const CQuadTree&);

private:
	CQuadNode* m_pRoot;
	CQuadNode* m_pLocalRoot;
	size_t m_nZoneScale;
	size_t m_nNodeCount;
	node_pool_t* m_pFreeList;
	TMemoryBucket<node_pool_t, 256, TQuadTreeAlloc> m_MemBucket;
};

#endif // _QUAD_TREE_H
