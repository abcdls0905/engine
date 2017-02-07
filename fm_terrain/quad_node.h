//--------------------------------------------------------------------
// �ļ���:		quad_node.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _QUAD_NODE_H
#define _QUAD_NODE_H

#include "../public/macros.h"
#include "../public/core_mem.h"


#include "../visual/fm_define.h"
#include "../utils/array_pod.h"

class TQuadNodeAlloc
{
public:
	TQuadNodeAlloc() {}
	~TQuadNodeAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TQuadNodeAlloc& src) {}
};

// �Ĳ����ڵ�

class CQuadGround;
class CQuadVisual;
class CQuadTree;
class CTickTimer;

class CQuadNode
{
public:
	CQuadNode(CQuadTree* tree, CQuadNode* parent, size_t scale, float x, 
		float z);
	~CQuadNode();

	// ��ó߶ȷ��ϵ����ڽڵ�
	CQuadNode* GetInNode(size_t scale, float x, float z);
	CQuadNode* GetInNewNode(size_t scale, float x, float z);

	// ���ͬ��������ڽڵ�
	CQuadNode* GetLeftNode();
	CQuadNode* GetTopNode();
	CQuadNode* GetRightNode();
	CQuadNode* GetBottomNode();

	// ���ð����ĵر�
	bool AddGround(CQuadGround* pGround);
	// �Ƴ������ĵر�
	bool RemoveGround(CQuadGround* pGround);

	// �����ײ����
	bool AddObject(CQuadVisual* pObject);
	// �Ƴ���ײ����
	bool RemoveObject(CQuadVisual* pObject);

	// ����ȫΪ�յ��ӽڵ�
	void Compress(CTickTimer* tick_counter, size_t ms);

private:
	CQuadNode();
	CQuadNode(const CQuadNode&);
	CQuadNode& operator=(const CQuadNode&);

	// ���½ڵ�ĸ߶ȷ�Χ
	void UpdateScaleY();
	// ���¸��ڵ�ĸ߶ȷ�Χ
	void UpdateParentScaleY(size_t zone_scale);

public:
	CQuadTree* m_pTree;
	CQuadNode* m_pParent;
	CQuadNode* m_pChild0;
	CQuadNode* m_pChild1;
	CQuadNode* m_pChild2;
	CQuadNode* m_pChild3;
	unsigned int m_nScale;		 
	FmVec3 m_vMin;
	FmVec3 m_vMax;
	CQuadGround* m_pGround;
	TArrayPod<CQuadVisual*, 8, TQuadNodeAlloc> m_Objects;
};

#endif // _QUAD_NODE_H

