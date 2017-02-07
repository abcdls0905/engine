//--------------------------------------------------------------------
// 文件名:		quad_collide.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _QUAD_COLLIDE_H
#define _QUAD_COLLIDE_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"

// 地形碰撞

class CQuadVisual;
class CQuadTree;
class CQuadNode;
class Terrain;
class CZoneManager;

class CQuadCollide
{
public:
	CQuadCollide();
	~CQuadCollide();

	CQuadTree* GetQuadTree() const { return m_pQuadTree; }

	void SetTerrain(Terrain* pTerrain);

	// 初始化
	bool Init();

	// 碰撞检测详细信息
	bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	bool TraceAll(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
	bool TraceSphere(const FmVec3& center, float radius);
	// 拣选
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

	// 压缩
	bool Compress();

private:
	CQuadCollide(const CQuadCollide&);
	CQuadCollide& operator=(const CQuadCollide&);

	bool TraceNode(CQuadNode* node, const FmVec3& src, 
		const FmVec3& dst, const FmVec3& dir, trace_info_t* result);
	bool TraceLeaf(CQuadNode* node, const FmVec3& src, 
		const FmVec3& dst, const FmVec3& dir, trace_info_t* result);
	bool TraceSphereNode(CQuadNode* node, const FmVec3& center, 
		float radius, unsigned int trace_mask);
	bool CullNode(CQuadNode* node, const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

private:
	Terrain* m_pTerrain;
	CQuadTree* m_pQuadTree;
	bool m_bFound;
	float m_fDistance;
	CQuadVisual* m_pProcessed;
};

#endif // _QUAD_COLLIDE_H

