//--------------------------------------------------------------------
// 文件名:		quad_visual.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _QUAD_VISUAL_H
#define _QUAD_VISUAL_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/fm_define.h"
#include "../utils/array_pod.h"

class TQuadVisualAlloc
{
public:
	TQuadVisualAlloc() {}
	~TQuadVisualAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TQuadVisualAlloc& src) {}
};

// 地面物体碰撞

class Terrain;
class IVisBase;
class CQuadNode;

class CQuadVisual
{
public:
	CQuadVisual(Terrain* pTerrain, size_t scale, const FmVec4& v_min, 
		const FmVec4& v_max, IVisBase* pVisBase, const char* name, 
		unsigned int hash, bool is_role, float clip_radius);
	~CQuadVisual();

	// 是否被屏蔽碰撞
	bool IsTraceMasked(unsigned int trace_mask);
	// 碰撞测试
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
	bool TraceSphere(const FmVec3& center, float radius);
	// 拣选
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 记录所属节点
	bool AddNode(CQuadNode* node);
	// 清除记录的所属节点
	void RemoveNodes();

	// 获得可见对象号
	const PERSISTID& GetVisualID() const { return m_VisualID; }
	// 名字
	const char* GetName() const { return m_strName.c_str(); }
	// 哈希值
	unsigned int GetHash() const { return m_nHash; }
	// 是否是人物
	bool GetIsRole() const { return m_bIsRole; }

	// 剪裁距离
	void SetClipRadius(float value) { m_fClipRadius = value; }
	float GetClipRadius() const { return m_fClipRadius; }
	
private:
	CQuadVisual();
	CQuadVisual(const CQuadVisual&);
	CQuadVisual& operator=(const CQuadVisual&);
		
public:
	FmVec4 m_vMin;
	FmVec4 m_vMax;
	size_t m_nScale;	// 尺度
	bool m_bUsed;
	CQuadVisual* m_pNext;

private:
	Terrain* m_pTerrain;
	core_string m_strName;
	unsigned int m_nHash;
	unsigned int m_nTraceMask;
	PERSISTID m_VisualID;
	bool m_bIsRole;
	float m_fClipRadius;
	TArrayPod<CQuadNode*, 4, TQuadVisualAlloc> m_Nodes;
};

#endif // _QUAD_VISUAL_H
