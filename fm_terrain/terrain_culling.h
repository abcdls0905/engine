//--------------------------------------------------------------------
// 文件名:		terrain_culling.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年2月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainCullingAlloc& src) {}
};

// 地形剪裁

class Terrain;
class CTerrainZone;
class CQuadNode;
class CQuadVisual;

// 要被渲染的区域
struct render_zone_t
{
	int nZoneIndex;
	CTerrainChunk** pChunks;// 区域内要被渲染的分块
	size_t nZoneChunkNum;
	size_t nChunkNum;		// 区域内要被渲染的分块数
};

// 要被渲染的物体
struct render_visual_t
{
	//PERSISTID VisualID;		// 物体对象号
	IVisBase* pVisBase;
	float fViewZ;			// 距离
	bool bUnderWater;		// 是否在水下
};

// 要被渲染的人物
struct render_role_t
{
	PERSISTID VisualID;		// 人物对象号
	IVisBase* pVisBase;
	bool bVisible;			// 是否可见
	bool bUnderWater;		// 是否在水下
};

// 渲染对象集合
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

// 环境贴图要渲染的对象集合
struct env_map_collect_t
{
	enum { MAX_ZONE_NUM = 64 };
	enum { MAX_VISUAL_NUM = 1024 };
	
	render_zone_t* pZones;		// 环境贴图要渲染的区域信息
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

// 临近单元信息
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
	
	// 最大渲染区域数
	enum { MAX_RENDER_ZONES = 1024 };
	// 最大渲染分块数
	enum { MAX_RENDER_CHUNKS = 65536 };
	// 最大渲染地面物体数
	enum { MAX_RENDER_VISUALS = 16 * 1024 };
	// 最大渲染人物数
	enum { MAX_RENDER_ROLES = 2 * 1024 };
	// 最大更新地面物体数
	enum { MAX_UPDATE_VISUALS = MAX_RENDER_VISUALS + MAX_RENDER_ROLES };
	
	enum { HORIZON_POINT_NUM = 32 };
	enum { HORIZON_BUCKET_SIZE = 160 };

	// 水平阻挡物信息
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

	// 初始化
	bool Initialize(IRender* pRender);
	// 关闭
	//bool Shut();

	// 更新
	void Update(float seconds);

	// 刷新要渲染的分块
	bool RefreshChunks();
	// 刷新要渲染的地面物体
	bool RefreshVisuals();
	// 刷新视野
	bool RefreshAll();
	
	// 刷新环境贴图要渲染的分块
	env_map_collect_t* GetEnvMapCollect(size_t index);
	bool RefreshEnvMapChunks(size_t index);
	bool RefreshEnvMapVisuals(size_t index, bool only_selected);

	// 刷新阴影渲染对象
	render_collect_t* GetShadowMapCollect(size_t index);
	bool RefreshShadowMapCollects();
	render_collect_t* GetShadowMapCollect() { return m_pShadowMapCollect; }
	bool RefreshShadowMapChunks();
	bool RefreshShadowMapVisuals();

	// 测试包围盒是否被地平线遮挡
	bool IsHorizonClip(const FmVec3& v_min, const FmVec3& v_max);
	// 导出地平线信息到文件
	bool DumpHorizon(const char* file_name);
	
	// 需要渲染的分块 
	size_t GetRenderChunkCount() const { return m_nRenderChunkNum; }
	CTerrainChunk** GetRenderChunks() { return m_pRenderChunks; }
	// 需要渲染的区域
	size_t GetRenderZoneCount() const { return m_nRenderZoneNum; }
	render_zone_t* GetRenderZones() { return m_pRenderZones; }
	// 需要渲染的物体
	size_t GetRenderVisualCount() const { return m_nRenderVisualNum; }
	render_visual_t* GetRenderVisuals() { return m_pRenderVisuals; }
	// 需要渲染的角色
	size_t GetRenderRoleCount() const { return m_nRenderRoleNum; }
	render_role_t* GetRenderRoles() { return m_pRenderRoles; }

	// 获得完全在雾中的地形信息
	size_t GetInFogChunkCount() const { return m_nInFogChunkNum; }
	size_t GetInFogVertexCount() const { return m_InFogVertex.size(); }
	size_t GetInFogIndexCount() const { return m_InFogIndex.size(); }
	FmVec4* GetInFogVertices() { return &m_InFogVertex[0]; }
	unsigned short* GetInFogIndices() { return &m_InFogIndex[0]; }

private:
	CTerrainCulling(const CTerrainCulling&);
	CTerrainCulling& operator=(const CTerrainCulling&);

	// 释放资源
	void ReleaseAll();
	// 设置分块的LOD
	void CheckRenderChunkLod();
	// 生成地平线
	bool GenerateHorizon();

private:
	Terrain* m_pTerrain;
	IRender* m_pRender;
	
	render_zone_t* m_pRenderZones;		// 要渲染的区域信息
	size_t m_nRenderZoneNum;
	CTerrainChunk** m_pRenderChunks;	// 要渲染的分块
	size_t m_nRenderChunkNum;
	render_visual_t* m_pRenderVisuals;	// 要渲染的地面物体
	size_t m_nRenderVisualNum;
	render_role_t* m_pRenderRoles;		// 要渲染的地面物体
	size_t m_nRenderRoleNum;
	PERSISTID* m_pUpdateVisuals;		// 要更新的地面物体
	size_t m_nUpdateVisualNum;
	TArrayPod<env_map_collect_t*, 1, TTerrainCullingAlloc> m_EnvMapCollects;
	TArrayPod<render_collect_t*, 1, TTerrainCullingAlloc> m_ShadowMapCollects;
	near_info_t m_NearChunkInfo[NEAR_CHUNK_DIMS * NEAR_CHUNK_DIMS];
	near_info_t m_NearGridInfo[NEAR_GRID_DIMS * NEAR_GRID_DIMS];
	render_collect_t* m_pShadowMapCollect;

	size_t m_nInFogChunkNum;
	TArrayPod<FmVec4, 1, TTerrainCullingAlloc> m_InFogVertex;
	TArrayPod<unsigned short, 1, TTerrainCullingAlloc> m_InFogIndex;

	horizon_bucket_t* m_pHorizonBuckets;	// 渐进地平线信息
	size_t m_nHorizonBucketCount;
	FmMat4 m_mtxViewProj;				// 当前的转换矩阵
};

#endif // _TERRAIN_CULLING_H
