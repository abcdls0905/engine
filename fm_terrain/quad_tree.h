//--------------------------------------------------------------------
// �ļ���:		quad_tree.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TQuadTreeAlloc& src) {}
};

// �Ĳ���

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

	// �ͷ�
	void ReleaseAll();
	
	// ����
	bool Create(size_t scale, float x, float z);

	// �½��ڵ�
	CQuadNode* NewNode(CQuadNode* parent, size_t scale, float x, float z);
	// ɾ���ڵ�
	void DeleteNode(CQuadNode* node);

	// ���õ�ǰ����ĸ��ڵ�
	bool SetLocalRoot(float x, float z, float radius);

	// ��ø��ڵ�
	CQuadNode* GetRoot() const { return m_pRoot; }
	// ��õ�ǰ����ĸ��ڵ�
	CQuadNode* GetLocalRoot() const { return m_pLocalRoot; }
	// �ڵ����
	size_t GetNodeCount() const { return m_nNodeCount; }

	// ��Ӧ��������Ľڵ�ߴ�
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
