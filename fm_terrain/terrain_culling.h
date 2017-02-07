//--------------------------------------------------------------------
// �ļ���:		terrain_culling.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��2��23��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_CULLING_H
#define _TERRAIN_CULLING_H

#include "../public/macros.h"
#include "../public/core_mem.h"

#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
#include "../utils/array_pod.h"
#include "terrain_chunk.h"

class TTerrainCullingAlloc
{
public:
	TTerrainCullingAlloc() {}
	~TTerrainCullingAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TTerrainCullingAlloc& src) {}
};

// ���μ���

class Terrain;
class CTerrainZone;
class CQuadNode;
class CQuadVisual;

// Ҫ����Ⱦ������
struct render_zone_t
{
	int nZoneIndex;
	CTerrainChunk** pChunks;// ������Ҫ����Ⱦ�ķֿ�
	size_t nZoneChunkNum;
	size_t nChunkNum;		// ������Ҫ����Ⱦ�ķֿ���
};

// Ҫ����Ⱦ������
struct render_visual_t
{
	//PERSISTID VisualID;		// ��������
	IVisBase* pVisBase;
	float fViewZ;			// ����
	bool bUnderWater;		// �Ƿ���ˮ��
};

// Ҫ����Ⱦ������
struct render_role_t
{
	PERSISTID VisualID;		// ��������
	IVisBase* pVisBase;
	bool bVisible;			// �Ƿ�ɼ�
	bool bUnderWater;		// �Ƿ���ˮ��
};

// ��Ⱦ���󼯺�
struct render_collect_t
{
	enum { MAX_CHUNK_NUM = 1024 };
	enum { MAX_VISUAL_NUM = 1024 };

	struct chunk_t
	{
		CTerrainChunk* pChunk;
		float fViewZ;
	};

	struct visual_t
	{
		IVisBase* pVisBase;
		float fViewZ;
	};

	chunk_t* pChunks;
	size_t nChunkNum;
	visual_t* pVisuals;
	size_t nVisualNum;

	render_collect_t()
	{
		nChunkNum = 0;
		nVisualNum = 0;
		pChunks = (chunk_t*)CORE_ALLOC(sizeof(chunk_t) * MAX_CHUNK_NUM);
		pVisuals = (visual_t*)CORE_ALLOC(sizeof(visual_t) * MAX_VISUAL_NUM);
		memset(pChunks, 0, sizeof(chunk_t) * MAX_CHUNK_NUM);
		memset(pVisuals, 0, sizeof(visual_t) * MAX_VISUAL_NUM);
	}

	~render_collect_t()
	{
		CORE_FREE(pChunks, sizeof(chunk_t) * MAX_CHUNK_NUM);
		CORE_FREE(pVisuals, sizeof(visual_t) * MAX_VISUAL_NUM);
	}

	void ClearChunk()
	{
		nChunkNum = 0;
	}

	void ClearVisual()
	{
		nVisualNum = 0;
	}

	bool AddChunk(CTerrainChunk* pChunk, float view_z)
	{
		if (nChunkNum >= MAX_CHUNK_NUM)
		{
			return false;
		}

		pChunks[nChunkNum].pChunk = pChunk;
		pChunks[nChunkNum].fViewZ = view_z;
		nChunkNum++;

		return true;
	}
	
	bool AddVisual(IVisBase* pVisBase, float view_z)
	{
		if (nVisualNum >= MAX_VISUAL_NUM)
		{
			return false;
		}

		pVisuals[nVisualNum].pVisBase = pVisBase;
		pVisuals[nVisualNum].fViewZ = view_z;
		nVisualNum++;

		return true;
	}
};

// ������ͼҪ��Ⱦ�Ķ��󼯺�
struct env_map_collect_t
{
	enum { MAX_ZONE_NUM = 64 };
	enum { MAX_VISUAL_NUM = 1024 };
	
	render_zone_t* pZones;		// ������ͼҪ��Ⱦ��������Ϣ
	size_t nZoneNum;
	IVisBase** pVisuals;
	size_t nVisualNum;

	env_map_collect_t()
	{
		nZoneNum = 0;
		nVisualNum = 0;
		pZones = (render_zone_t*)CORE_ALLOC(
			sizeof(render_zone_t) * MAX_ZONE_NUM);
		pVisuals = (IVisBase**)CORE_ALLOC(sizeof(IVisBase*) * MAX_VISUAL_NUM);
		memset(pZones, 0, sizeof(render_zone_t) * MAX_ZONE_NUM);
		memset(pVisuals, 0, sizeof(IVisBase*) * MAX_VISUAL_NUM);
	}

	~env_map_collect_t()
	{
		for (size_t i = 0; i < MAX_ZONE_NUM; ++i)
		{
			if (pZones[i].pChunks)
			{
				CORE_FREE(pZones[i].pChunks, 
					sizeof(CTerrainChunk*) * pZones[i].nZoneChunkNum);
			}
		}

		CORE_FREE(pZones, sizeof(render_zone_t) * MAX_ZONE_NUM);
		CORE_FREE(pVisuals, sizeof(IVisBase*) * MAX_VISUAL_NUM);
	}

	void ClearChunk()
	{
		nZoneNum = 0;
		pZones[0].nZoneIndex = -1;
	}

	void ClearVisual()
	{
		nVisualNum = 0;
	}

	bool AddChunk(int zone_index, size_t zone_chunks, CTerrainChunk* chunk)
	{
		size_t index;

		if (nZoneNum > 0)
		{
			index = nZoneNum - 1;
		}
		else
		{
			index = 0;
		}
		
		if (pZones[index].nZoneIndex != zone_index)
		{
			if (nZoneNum >= (MAX_ZONE_NUM - 1))
			{
				return false;
			}
			
			pZones[nZoneNum].nZoneIndex = zone_index;
			pZones[nZoneNum].nChunkNum = 0;

			if (NULL == pZones[nZoneNum].pChunks)
			{
				pZones[nZoneNum].pChunks = (CTerrainChunk**)CORE_ALLOC(
					sizeof(CTerrainChunk*) * zone_chunks);
				pZones[nZoneNum].nZoneChunkNum = zone_chunks;
			}

			nZoneNum++;
			pZones[nZoneNum].nZoneIndex = -1;
			index = nZoneNum - 1;
		}

		size_t chunk_num = pZones[index].nChunkNum;

		Assert(chunk_num < zone_chunks);

		pZones[index].pChunks[chunk_num] = chunk;
		pZones[index].nChunkNum++;

		return true;
	}

	bool AddVisual(IVisBase* pVisBase)
	{
		if (nVisualNum >= MAX_VISUAL_NUM)
		{
			return false;
		}

		pVisuals[nVisualNum++] = pVisBase;
		
		return true;
	}
};

// �ٽ���Ԫ��Ϣ
struct near_info_t
{
	int nRowOffset;
	int nColOffset;
	float fDistance;
};

class CTerrainCulling
{
private:
	enum { NEAR_CHUNK_DIMS = 31 };
	enum { NEAR_GRID_DIMS = 127 };
	
	// �����Ⱦ������
	enum { MAX_RENDER_ZONES = 1024 };
	// �����Ⱦ�ֿ���
	enum { MAX_RENDER_CHUNKS = 65536 };
	// �����Ⱦ����������
	enum { MAX_RENDER_VISUALS = 16 * 1024 };
	// �����Ⱦ������
	enum { MAX_RENDER_ROLES = 2 * 1024 };
	// �����µ���������
	enum { MAX_UPDATE_VISUALS = MAX_RENDER_VISUALS + MAX_RENDER_ROLES };
	
	enum { HORIZON_POINT_NUM = 32 };
	enum { HORIZON_BUCKET_SIZE = 160 };

	// ˮƽ�赲����Ϣ
	struct horizon_bucket_t
	{
		float fScreenY[32];
		float fScreenZ[32];
		int nCount;

		horizon_bucket_t()
		{
			nCount = 0;
		}
	};

public:
	CTerrainCulling();
	~CTerrainCulling();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// ��ʼ��
	bool Initialize(IRender* pRender);
	// �ر�
	//bool Shut();

	// ����
	void Update(float seconds);

	// ˢ��Ҫ��Ⱦ�ķֿ�
	bool RefreshChunks();
	// ˢ��Ҫ��Ⱦ�ĵ�������
	bool RefreshVisuals();
	// ˢ����Ұ
	bool RefreshAll();
	
	// ˢ�»�����ͼҪ��Ⱦ�ķֿ�
	env_map_collect_t* GetEnvMapCollect(size_t index);
	bool RefreshEnvMapChunks(size_t index);
	bool RefreshEnvMapVisuals(size_t index, bool only_selected);

	// ˢ����Ӱ��Ⱦ����
	render_collect_t* GetShadowMapCollect(size_t index);
	bool RefreshShadowMapCollects();
	render_collect_t* GetShadowMapCollect() { return m_pShadowMapCollect; }
	bool RefreshShadowMapChunks();
	bool RefreshShadowMapVisuals();

	// ���԰�Χ���Ƿ񱻵�ƽ���ڵ�
	bool IsHorizonClip(const FmVec3& v_min, const FmVec3& v_max);
	// ������ƽ����Ϣ���ļ�
	bool DumpHorizon(const char* file_name);
	
	// ��Ҫ��Ⱦ�ķֿ� 
	size_t GetRenderChunkCount() const { return m_nRenderChunkNum; }
	CTerrainChunk** GetRenderChunks() { return m_pRenderChunks; }
	// ��Ҫ��Ⱦ������
	size_t GetRenderZoneCount() const { return m_nRenderZoneNum; }
	render_zone_t* GetRenderZones() { return m_pRenderZones; }
	// ��Ҫ��Ⱦ������
	size_t GetRenderVisualCount() const { return m_nRenderVisualNum; }
	render_visual_t* GetRenderVisuals() { return m_pRenderVisuals; }
	// ��Ҫ��Ⱦ�Ľ�ɫ
	size_t GetRenderRoleCount() const { return m_nRenderRoleNum; }
	render_role_t* GetRenderRoles() { return m_pRenderRoles; }

	// �����ȫ�����еĵ�����Ϣ
	size_t GetInFogChunkCount() const { return m_nInFogChunkNum; }
	size_t GetInFogVertexCount() const { return m_InFogVertex.size(); }
	size_t GetInFogIndexCount() const { return m_InFogIndex.size(); }
	FmVec4* GetInFogVertices() { return &m_InFogVertex[0]; }
	unsigned short* GetInFogIndices() { return &m_InFogIndex[0]; }

private:
	CTerrainCulling(const CTerrainCulling&);
	CTerrainCulling& operator=(const CTerrainCulling&);

	// �ͷ���Դ
	void ReleaseAll();
	// ���÷ֿ��LOD
	void CheckRenderChunkLod();
	// ���ɵ�ƽ��
	bool GenerateHorizon();

private:
	Terrain* m_pTerrain;
	IRender* m_pRender;
	
	render_zone_t* m_pRenderZones;		// Ҫ��Ⱦ��������Ϣ
	size_t m_nRenderZoneNum;
	CTerrainChunk** m_pRenderChunks;	// Ҫ��Ⱦ�ķֿ�
	size_t m_nRenderChunkNum;
	render_visual_t* m_pRenderVisuals;	// Ҫ��Ⱦ�ĵ�������
	size_t m_nRenderVisualNum;
	render_role_t* m_pRenderRoles;		// Ҫ��Ⱦ�ĵ�������
	size_t m_nRenderRoleNum;
	PERSISTID* m_pUpdateVisuals;		// Ҫ���µĵ�������
	size_t m_nUpdateVisualNum;
	TArrayPod<env_map_collect_t*, 1, TTerrainCullingAlloc> m_EnvMapCollects;
	TArrayPod<render_collect_t*, 1, TTerrainCullingAlloc> m_ShadowMapCollects;
	near_info_t m_NearChunkInfo[NEAR_CHUNK_DIMS * NEAR_CHUNK_DIMS];
	near_info_t m_NearGridInfo[NEAR_GRID_DIMS * NEAR_GRID_DIMS];
	render_collect_t* m_pShadowMapCollect;

	size_t m_nInFogChunkNum;
	TArrayPod<FmVec4, 1, TTerrainCullingAlloc> m_InFogVertex;
	TArrayPod<unsigned short, 1, TTerrainCullingAlloc> m_InFogIndex;

	horizon_bucket_t* m_pHorizonBuckets;	// ������ƽ����Ϣ
	size_t m_nHorizonBucketCount;
	FmMat4 m_mtxViewProj;				// ��ǰ��ת������
};

#endif // _TERRAIN_CULLING_H
