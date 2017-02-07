//--------------------------------------------------------------------
// 文件名:		quad_node.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TQuadNodeAlloc& src) {}
};

// 四叉树节点

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

	// 获得尺度符合的所在节点
	CQuadNode* GetInNode(size_t scale, float x, float z);
	CQuadNode* GetInNewNode(size_t scale, float x, float z);

	// 获得同级别的相邻节点
	CQuadNode* GetLeftNode();
	CQuadNode* GetTopNode();
	CQuadNode* GetRightNode();
	CQuadNode* GetBottomNode();

	// 设置包含的地表
	bool AddGround(CQuadGround* pGround);
	// 移除包含的地表
	bool RemoveGround(CQuadGround* pGround);

	// 添加碰撞对象
	bool AddObject(CQuadVisual* pObject);
	// 移除碰撞对象
	bool RemoveObject(CQuadVisual* pObject);

	// 清理全为空的子节点
	void Compress(CTickTimer* tick_counter, size_t ms);

private:
	CQuadNode();
	CQuadNode(const CQuadNode&);
	CQuadNode& operator=(const CQuadNode&);

	// 更新节点的高度范围
	void UpdateScaleY();
	// 更新父节点的高度范围
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

