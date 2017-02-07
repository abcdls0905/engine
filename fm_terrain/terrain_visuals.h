//--------------------------------------------------------------------
// �ļ���:		terrain_visuals.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��10��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTerrainVisualsAlloc& src) {}
};

// �����еĵ�������

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

	// �������
	bool AddVisual(const char* name, IVisBase* pVisBase, bool is_role, 
		float clip_radius);
	// �Ƴ�ָ����������
	bool RemoveByIndex(size_t index);
	// ��ղ�ɾ����������
	void ReleaseVisuals();
	// ���¶�λ����
	bool MoveVisual(IVisBase* pVisBase, float x, float y, float z);

	// ���������ߴ�
	size_t GetVisualSize() const { return m_Visuals.size(); }
	// �����������
	CQuadVisual** GetVisuals() { return m_Visuals.data(); }
	// ������壨����ΪNULL��
	CQuadVisual* GetVisualByIndex(size_t index) const
	{
		Assert(index < m_Visuals.size());

		return m_Visuals[index];
	}

	// ����
	bool FindVisualIndex(const PERSISTID& id, size_t& index) const;
	bool FindNameIndex(const char* name, size_t& index) const;

	// ���ָ����Χ�ڵ�����
	size_t GetRectVisuals(float x1, float z1, float x2, float z2, 
		IVarList& result);
	// ���ָ����Χ�ڵķǽ�ɫ�������
	size_t GetRectActors(float x1, float z1, float x2, float z2, 
		IVarList& result);

		// ���ָ���뾶������
	size_t GetCircleVisuals(float center_x, float center_z, float radius, 
		IVarList& result);
	
private:
	CTerrainVisuals();
	CTerrainVisuals(const CTerrainVisuals&);
	CTerrainVisuals& operator=(const CTerrainVisuals&);

	// ��������İ�Χ��
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
