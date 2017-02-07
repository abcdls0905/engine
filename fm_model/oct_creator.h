//--------------------------------------------------------------------
// �ļ���:		oct_creator.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��8��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TResGeoOctCreatorAlloc& src) {}
};

// ģ�Ͱ˲�������

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
	
	// ��ð˲���
	COctTree* GetOctTree() const { return m_pOctTree; }
	// �ͷŰ˲���������Ȩ
	bool Clear();
	// �����˲���
	bool Create(model_node_t* pModelNode);
	
private:
	COctCreator(const COctCreator&);
	COctCreator& operator=(const COctCreator&);

	// ��������ε�����
	bool AddToOctNodeLoose(oct_node_t* node, size_t tri_index, 
		const FmVec3& v_min, const FmVec3& v_max);
	// ��ýڵ����ݴ�С
	size_t GetOctNodeSize(oct_node_t* node);
	// ѹ���ڵ�����
	COctNode* CompressOctNode(oct_node_t* node, char* pdata);
	// ɾ���ڵ�
	void DeleteOctNode(oct_node_t* node);
	
private:
	COctTree* m_pOctTree;
	TMemoryBucket<oct_node_t, 256, TResGeoOctCreatorAlloc> m_NodeBucket; // �ڵ��ڴ�
	TArrayPod<oct_node_t*, 1, TResGeoOctCreatorAlloc> m_TriangleNode; // ��¼�����������Ľڵ�
	char* m_pTreeData;		// ���յİ˲�������	
	size_t m_nTreeSize;		// ���յİ˲������ݴ�С
	size_t m_nTotalNodes;	// �ܽڵ��� 
	size_t m_nDataCount;	// ���ݼ���
	size_t m_nMaxDepth;		// �˲���������
	FmVec3 m_vTriangleMin;
	FmVec3 m_vTriangleMax;
	FmVec3 m_vTriangleCenter;
};

#endif // _OCT_CREATOR_H
