//--------------------------------------------------------------------
// 文件名:		quad_ground.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _QUAD_GROUND_H
#define _QUAD_GROUND_H

#include "../public/macros.h"
#include "chunk_style.h"

// 地面三角形集合

class Terrain;
class CTerrainChunk;
class CQuadNode;

class CQuadGround
{
public:
	CQuadGround();
	~CQuadGround();

	// 初始化
	bool Init(Terrain* pTerrain, size_t scale, const FmVec3& v_min, 
		const FmVec3& v_max, size_t zone, size_t chunk, size_t index);

	// 是否被屏蔽碰撞
	bool IsTraceMasked(unsigned int trace_mask);
	// 碰撞测试详细信息
	bool TraceDetail(const FmVec3& src, const FmVec3& dir,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	bool TraceHitted(const FmVec3& src, const FmVec3& dir);
	// 球体范围碰撞
	bool TraceSphere(const FmVec3& center, float radius);
	// 拣选
	bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);

private:
	CQuadGround(const CQuadGround&);
	CQuadGround& operator=(const CQuadGround&);

public:
	FmVec3 m_vMin;
	FmVec3 m_vMax;
	size_t m_nScale;	// 尺度
	Terrain* m_pTerrain;
	size_t m_nZone;		// 区域索引
	size_t m_nChunk;	// 分块索引
	size_t m_nIndex;	// 在分块中的索引
	CQuadNode* m_pNode;
};

#endif // _QUAD_GROUND_H
