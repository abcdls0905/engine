//--------------------------------------------------------------------
// �ļ���:		oct_tree.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��8��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _OCT_TREE_H
#define _OCT_TREE_H

#include "../public/macros.h"

#include "../visual/i_render.h"

// ģ�͵İ˲����ڵ�

class COctNode
{
public:
	unsigned int m_nTriangleNum;
	unsigned int m_nChildNum;
	unsigned int m_nChildMask;
	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
};

// ģ�͵İ˲���

class CModelPlayer;
struct model_node_t;

class COctTree
{
public:
	COctTree();
	~COctTree();

	// ��ʼ��
	void Init(model_node_t* pModelNode, const FmVec3& bound_min, 
		const FmVec3& bound_max, char* tree_data, size_t tree_size);
	// ��ײ�����ϸ��Ϣ
	bool TraceDetail(CModelPlayer* ref, const FmVec3& src, 
		const FmVec3& dir, int alpha_ref, bool only_front, 
		trace_info_t& result);
	// ��ײ���
	bool TraceHitted(CModelPlayer* ref, const FmVec3& src, 
		const FmVec3& dir, int alpha_ref, bool only_front);
	// ���巶Χ��ײ
	bool TraceSphere(CModelPlayer* ref, const FmVec3& center, 
		float radius);
	// ��������ײ
	bool TraceTriangle(CModelPlayer* ref, const FmVec3& v0, 
        const FmVec3& v1, const FmVec3& v2);
	// ��ѡ
	bool Cull(CModelPlayer* ref, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context, const FmMat4& mat);

private:
	COctTree(const COctTree&);
	COctTree& operator=(const COctTree&);

	// �Խڵ�����ײ���
	static bool TraceNode(COctNode* node);
	// �Խڵ���������ײ
    static bool TraceSphereNode(COctNode* node);
    // �Խڵ�����������ײ
    static bool TraceTriangleNode(COctNode* node);
	// �Խڵ�����ѡ
	static bool CullNode(COctNode* node);
	// �����ӽڵ�
	static bool TravelChildren(COctNode* node, 
		bool (*travel_func)(COctNode*));

private:
	model_node_t* m_pModelNode;
	FmVec3 m_vBoundMin;
    FmVec3 m_vBoundMax;
	char* m_pTreeData;
	size_t m_nTreeSize;
};

#endif // _OCT_TREE_H
