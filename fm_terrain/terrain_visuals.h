//--------------------------------------------------------------------
// 文件名:		terrain_visuals.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年10月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_VISUALS_H
#define _TERRAIN_VISUALS_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../public/core_mem.h"


#include "../visual/fm_define.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"

class TTerrainVisualsAlloc
{
public:
	TTerrainVisualsAlloc() {}
	~TTerrainVisualsAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainVisualsAlloc& src) {}
};

// 区域中的地面物体

class ICore;
class IVisBase;
class Terrain;
class CTerrainZone;
class CQuadVisual;

class CTerrainVisuals
{
public:
	CTerrainVisuals(Terrain* pTerrain, CTerrainZone* pZone);
	~CTerrainVisuals();

	// 添加物体
	bool AddVisual(const char* name, IVisBase* pVisBase, bool is_role, 
		float clip_radius);
	// 移除指定索引物体
	bool RemoveByIndex(size_t index);
	// 清空并删除所有物体
	void ReleaseVisuals();
	// 重新定位物体
	bool MoveVisual(IVisBase* pVisBase, float x, float y, float z);

	// 物体容器尺寸
	size_t GetVisualSize() const { return m_Visuals.size(); }
	// 获得所有物体
	CQuadVisual** GetVisuals() { return m_Visuals.data(); }
	// 获得物体（可能为NULL）
	CQuadVisual* GetVisualByIndex(size_t index) const
	{
		Assert(index < m_Visuals.size());

		return m_Visuals[index];
	}

	// 查找
	bool FindVisualIndex(const PERSISTID& id, size_t& index) const;
	bool FindNameIndex(const char* name, size_t& index) const;

	// 获得指定范围内的物体
	size_t GetRectVisuals(float x1, float z1, float x2, float z2, 
		IVarList& result);
	// 获得指定范围内的非角色组合物体
	size_t GetRectActors(float x1, float z1, float x2, float z2, 
		IVarList& result);

		// 获得指定半径的物体
	size_t GetCircleVisuals(float center_x, float center_z, float radius, 
		IVarList& result);
	
private:
	CTerrainVisuals();
	CTerrainVisuals(const CTerrainVisuals&);
	CTerrainVisuals& operator=(const CTerrainVisuals&);

	// 计算物体的包围盒
	bool CalcBoundBox(IVisBase* pVisBase, FmVec4& v_min, 
		FmVec4& v_max);

private:
	ICore* m_pCore;
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	TArrayPod<CQuadVisual*, 1, TTerrainVisualsAlloc> m_Visuals;
	TArrayPod<size_t, 1, TTerrainVisualsAlloc> m_FreeList;
	TPodHashMap<PERSISTID, size_t, TPodTraits<PERSISTID>, 
		TTerrainVisualsAlloc> m_VisualIndex;
};

#endif // _TERRAIN_VISUALS_H
