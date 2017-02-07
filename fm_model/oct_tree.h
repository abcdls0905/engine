//--------------------------------------------------------------------
// 文件名:		oct_tree.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _OCT_TREE_H
#define _OCT_TREE_H

#include "../public/macros.h"

#include "../visual/i_render.h"

// 模型的八叉树节点

class COctNode
{
public:
	unsigned int m_nTriangleNum;
	unsigned int m_nChildNum;
	unsigned int m_nChildMask;
	FmVec3 m_vBoundMin;
	FmVec3 m_vBoundMax;
};

// 模型的八叉树

class CModelPlayer;
struct model_node_t;

class COctTree
{
public:
	COctTree();
	~COctTree();

	// 初始化
	void Init(model_node_t* pModelNode, const FmVec3& bound_min, 
		const FmVec3& bound_max, char* tree_data, size_t tree_size);
	// 碰撞检测详细信息
	bool TraceDetail(CModelPlayer* ref, const FmVec3& src, 
		const FmVec3& dir, int alpha_ref, bool only_front, 
		trace_info_t& result);
	// 碰撞检测
	bool TraceHitted(CModelPlayer* ref, const FmVec3& src, 
		const FmVec3& dir, int alpha_ref, bool only_front);
	// 球体范围碰撞
	bool TraceSphere(CModelPlayer* ref, const FmVec3& center, 
		float radius);
	// 三角形碰撞
	bool TraceTriangle(CModelPlayer* ref, const FmVec3& v0, 
        const FmVec3& v1, const FmVec3& v2);
	// 拣选
	bool Cull(CModelPlayer* ref, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context, const FmMat4& mat);

private:
	COctTree(const COctTree&);
	COctTree& operator=(const COctTree&);

	// 对节点做碰撞检测
	static bool TraceNode(COctNode* node);
	// 对节点做球体碰撞
    static bool TraceSphereNode(COctNode* node);
    // 对节点做三角形碰撞
    static bool TraceTriangleNode(COctNode* node);
	// 对节点做拣选
	static bool CullNode(COctNode* node);
	// 遍历子节点
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
