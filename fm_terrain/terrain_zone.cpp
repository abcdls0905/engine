//--------------------------------------------------------------------
// 文件名:		terrain_zone.cpp
// 内  容:
// 说  明:
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_zone.h"
#include "terrain.h"
#include "terrain_factory.h"
#include "terrain_height.h"
#include "terrain_light.h"
#include "terrain_chunk.h"
#include "terrain_visuals.h"
#include "terrain_walkable.h"
#include "terrain_region.h"
#include "light_map_config.h"
#ifdef PHYSICS_HAVOK_TERRAIN
#include "terrain_rigid.h"
#endif // PHYSICS_HAVOK_TERRAIN
#include "tex_alpha_map.h"
#include "tex_light_map.h"
#include "height_loader.h"
#include "light_loader.h"
#include "alpha_loader.h"
#include "visual_loader.h"
#include "walk_loader.h"
#include "region_loader.h"
#ifdef PHYSICS_HAVOK_TERRAIN
#include "rigid_loader.h"
#endif // PHYSICS_HAVOK_TERRAIN
#include "chunk_style.h"
#include "quad_tree.h"
#include "quad_node.h"
#include "quad_ground.h"
#include "quad_collide.h"
#include "../public/core_log.h"
#include "../public/inlines.h"

// CTerrainZone

CTerrainZone::CTerrainZone(Terrain* pTerrain, CTerrainFactory* pFactory,
	size_t index, size_t row, size_t col)
{
	Assert(pTerrain != NULL);
	Assert(pFactory != NULL);

	size_t ZONE_SCALE = pTerrain->GetZoneScale();
	size_t ALPHA_SCALE = pTerrain->GetAlphaScale();
	
	m_pRender = NULL;
	m_pTerrain = pTerrain;
	m_pQuadNode = NULL;
	m_pFactory = pFactory;
	m_nIndex = index;
	m_nRow = row;
	m_nCol = col;
	
	// 优化
	m_nGlobalRow = row * ZONE_SCALE;
	m_nGlobalCol = col * ZONE_SCALE;
	m_nAlphaRow = row * ALPHA_SCALE;
	m_nAlphaCol = col * ALPHA_SCALE;
	m_fLeft = pTerrain->GetLeft() + col * pTerrain->GetZoneWidth();
	m_fTop = pTerrain->GetTop() + row * pTerrain->GetZoneHeight();
	m_fRight = m_fLeft + pTerrain->GetZoneWidth();
	m_fBottom = m_fTop + pTerrain->GetZoneHeight();

	m_fSilenceTime = 0.0F;
	
	m_pHeight = NULL;
	m_pLight = NULL;
	m_pVisuals = NULL;
	m_pWalkable = NULL;
	m_pRegion = NULL;
#ifdef PHYSICS_HAVOK_TERRAIN
	m_pRigid = NULL;
#endif // PHYSICS_HAVOK_TERRAIN
	
	m_pHeightLoader = NULL;
	m_pLightLoader = NULL;
	m_pAlphaLoader = NULL;
	m_pVisualLoader = NULL;
	m_pWalkLoader = NULL;
	m_pRegionLoader = NULL;
#ifdef PHYSICS_HAVOK_TERRAIN
	m_pRigidLoader = NULL;
#endif // PHYSICS_HAVOK_TERRAIN

	m_bHeightLoaded = false;
	m_bLightLoaded = false;
	m_bAlphaLoaded = false;
	m_bVisualLoaded = false;
	m_bWalkLoaded = false;
	m_bRegionLoaded = false;
#ifdef PHYSICS_HAVOK_TERRAIN
	m_bRigidLoaded = false;
#endif // PHYSICS_HAVOK_TERRAIN

	m_bHeightChanged = false;
	m_bAlphaChanged = false;
	m_bLightChanged = false;
	m_bVisualChanged = false;
	m_bWidgetChanged = false;
	m_bHelperModelChanged = false;
	m_bCollideInfoChanged = false;
	m_bDescChanged = false;
	m_bSoundChanged = false;
	m_bAreaChanged = false;
	m_bRegionChanged = false;
	m_bNoUnload = false;
	m_bHasWater = false;
	
	m_fInvChunkSize = 
		1.0F / (pTerrain->GetChunkScale() * pTerrain->GetUnitSize());
	m_pChunks = NULL;
	m_nChunkDims = 0;
	m_nChunkNum = 0;
	m_nLoadCollideNum = 0;
	m_nCollideDims = 0;
	m_nCollideNum = 0;
	m_pCollides = NULL;
}

CTerrainZone::~CTerrainZone()
{
	CORE_DELETE(m_pHeight);
	CORE_DELETE(m_pLight);
	CORE_DELETE(m_pVisuals);
	CORE_DELETE(m_pWalkable);
	CORE_DELETE(m_pRegion);
#ifdef PHYSICS_HAVOK_TERRAIN
	CORE_DELETE(m_pRigid);
#endif // PHYSICS_HAVOK_TERRAIN

	if (m_pChunks)
	{
		for (size_t i = 0; i < m_nChunkNum; ++i)
		{
			m_pChunks[i].~CTerrainChunk();
		}

		CORE_FREE(m_pChunks, sizeof(CTerrainChunk) * m_nChunkNum);
	}

	if (m_pCollides)
	{
		for (size_t k = 0; k < m_nCollideNum; ++k)
		{
			m_pCollides[k].~CQuadGround();
		}
		
		CORE_FREE(m_pCollides, sizeof(CQuadGround) * m_nCollideNum);
	}

	ReleaseAllLoader();
}

void CTerrainZone::Release()
{
	if (this->DecRefs() == 0)
	{
		m_pFactory->Destroy(this);
	}
}

bool CTerrainZone::Init()
{
	m_pRender = m_pTerrain->GetRender();
	
	size_t zone_scale = m_pTerrain->GetZoneScale();
	
	// 顶点的数据
	m_pHeight = (CTerrainHeight*)CORE_ALLOC(sizeof(CTerrainHeight));
	new (m_pHeight) CTerrainHeight(m_pTerrain, this, zone_scale + 1, 
		zone_scale + 1);
	
	// 光照
	size_t light_scale = zone_scale * m_pTerrain->GetLightPerUnit();
	
	m_pLight = (CTerrainLight*)CORE_ALLOC(sizeof(CTerrainLight));
	new (m_pLight) CTerrainLight(m_pTerrain, this, light_scale, light_scale);
	
	// 地面物体
	m_pVisuals = (CTerrainVisuals*)CORE_ALLOC(sizeof(CTerrainVisuals));
	new (m_pVisuals) CTerrainVisuals(m_pTerrain, this);

	// 可行走信息
	size_t collide_per_unit = m_pTerrain->GetCollidePerUnit();

	m_pWalkable = (CTerrainWalkable*)CORE_ALLOC(sizeof(CTerrainWalkable));
	new (m_pWalkable) CTerrainWalkable(m_pTerrain, this, 
		zone_scale * collide_per_unit, zone_scale * collide_per_unit);

	// 标记
	m_pRegion = (CTerrainRegion*)CORE_ALLOC(sizeof(CTerrainRegion));
	new (m_pRegion) CTerrainRegion(m_pTerrain, this, zone_scale, zone_scale);

#ifdef PHYSICS_HAVOK_TERRAIN
	if (m_pRender->GetSupportPhysics())
	{
		// 刚体地形
		m_pRigid = (CTerrainRigid*)CORE_ALLOC(sizeof(CTerrainRigid));
		new (m_pRigid) CTerrainRigid(m_pTerrain, this, m_nRow, m_nCol);
	}
#endif // PHYSICS_HAVOK_TERRAIN

	// 地形分块
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	
	m_nChunkDims = m_pHeight->GetCols() / chunk_scale;
	m_nChunkNum = m_nChunkDims * m_nChunkDims;
	
	// 分块的数据
	m_pChunks = (CTerrainChunk*)CORE_ALLOC(
		sizeof(CTerrainChunk) * m_nChunkNum);

	for (size_t row = 0; row < m_nChunkDims; row++)
	{
		for (size_t col = 0; col < m_nChunkDims; col++)
		{
			size_t index = row * m_nChunkDims + col;
			CTerrainChunk* p = m_pChunks + index;

			new (p) CTerrainChunk(m_pTerrain, this, m_pHeight, index, row, 
				col);
		}
	}

	// 地表碰撞数据
	m_nCollideDims = zone_scale / m_pTerrain->GetCollideScale(); 
	m_nCollideNum = m_nCollideDims * m_nCollideDims;
	m_pCollides = (CQuadGround*)CORE_ALLOC(
		sizeof(CQuadGround) * m_nCollideNum);

	for (size_t k = 0; k < m_nCollideNum; ++k)
	{
		new (&m_pCollides[k]) CQuadGround;
	}
	
	return true;
}

void CTerrainZone::UpdateChunkSibling()
{
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		m_pChunks[i].UpdateAllSiblings();
	}
}

bool CTerrainZone::CreateChunkCollide(size_t index)
{
	Assert(index < m_nChunkNum);

	CTerrainChunk* pChunk = m_pChunks + index;
	CQuadNode* pTreeRoot = m_pTerrain->GetCollide()->GetQuadTree()->GetRoot();
	bool design_mode = m_pTerrain->GetDesignMode();
	float unit_size = m_pTerrain->GetUnitSize();
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	size_t collide_scale = m_pTerrain->GetCollideScale();
	size_t chunk_row = pChunk->GetRow();
	size_t chunk_col = pChunk->GetCol();
	size_t local_row = pChunk->GetLocalRow();
	size_t local_col = pChunk->GetLocalCol();
	size_t dims = m_nCollideDims / m_nChunkDims;
	int scale = (int)collide_scale * (int)unit_size;
	CQuadGround* p = m_pCollides + chunk_row * dims * m_nCollideDims 
		+ chunk_col * dims;

	FmVec3 v_delta(0.001F, 0.0F, 0.001F);
	FmVec3 v_min;
	FmVec3 v_max;

	for (size_t r = 0; r < dims; ++r)
	{
		for (size_t c = 0; c < dims; ++c)
		{
			size_t row = local_row + r * collide_scale;
			size_t col = local_col + c * collide_scale;

			float min_y;
			float max_y;

			if (design_mode)
			{
				CHeightLoader::CalcBlockMinMaxY(m_pHeight->GetHeightValues(),
					m_pHeight->GetCols(), row, col, collide_scale + 1,
					min_y, max_y);
			}
			else
			{
				m_pHeight->GetBlockMinMaxY((int)row, (int)col, 
					(int)collide_scale, min_y, max_y);
			}

			v_min.x = m_fLeft + col * unit_size;
			v_min.y = min_y;
			v_min.z = m_fTop + row * unit_size;
			v_max.x = v_min.x + collide_scale * unit_size;
			v_max.y = max_y;
			v_max.z = v_min.z + collide_scale * unit_size;
			// 防止水平方向跨越多个四叉树节点
			v_min += v_delta;
			v_max -= v_delta;

			p[c].Init(m_pTerrain, scale, v_min, v_max, m_nIndex, index, 
				r * dims + c);

			pTreeRoot->AddGround(&p[c]);
		}
		
		p += m_nCollideDims;
	}

	return true;
}

bool CTerrainZone::ReleaseChunkCollide(size_t index)
{
	Assert(index < m_nChunkNum);

	CTerrainChunk* pChunk = m_pChunks + index;
	size_t chunk_row = pChunk->GetRow();
	size_t chunk_col = pChunk->GetCol();
	size_t dims = m_nCollideDims / m_nChunkDims;
	CQuadGround* p = m_pCollides + chunk_row * dims * m_nCollideDims 
		+ chunk_col * dims;

	for (size_t r = 0; r < dims; ++r)
	{
		for (size_t c = 0; c < dims; ++c)
		{
			CQuadGround* pGround = &p[c];

			if (pGround->m_pNode)
			{
				pGround->m_pNode->RemoveGround(pGround);
				pGround->m_pNode = NULL;
			}
		}

		p += m_nCollideDims;
	}

	return true;
}

void CTerrainZone::ReleaseVisualObjects()
{
	m_pVisuals->ReleaseVisuals();
}

void CTerrainZone::ReleaseChunkObjects()
{
	for (size_t i = 0; i < m_nCollideNum; ++i)
	{
		CQuadGround* pGround = &m_pCollides[i];

		if (pGround->m_pNode)
		{
			pGround->m_pNode->RemoveGround(pGround);
			pGround->m_pNode = NULL;
		}
	}
}

void CTerrainZone::ReleaseChunkVB()
{
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		m_pChunks[i].ReleaseVertexBuffer();
	}
}

void CTerrainZone::ReleaseChunkAlphaMap()
{
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		m_pChunks[i].ReleaseAlphaMap();
	}
}

void CTerrainZone::ReleaseChunkAlphaTex()
{
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		m_pChunks[i].ReleaseAlphaTex();
	}
}

void CTerrainZone::ReleaseChunkGrassVB()
{
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		m_pChunks[i].ReleaseGrassVertexBuffer();
	}
}

void CTerrainZone::ReleaseAllLoader()
{
	SAFE_RELEASE(m_pHeightLoader);
	SAFE_RELEASE(m_pLightLoader);
	SAFE_RELEASE(m_pAlphaLoader);
	SAFE_RELEASE(m_pVisualLoader);
	SAFE_RELEASE(m_pWalkLoader);
	SAFE_RELEASE(m_pRegionLoader);
#ifdef PHYSICS_HAVOK_TERRAIN
	SAFE_RELEASE(m_pRigidLoader);
#endif // PHYSICS_HAVOK_TERRAIN
}

size_t CTerrainZone::GetChunkAlphaMapCount()
{
	size_t count = 0;
	
	for (size_t i = 0; i < m_nChunkNum; ++i)
	{
		count += m_pChunks[i].GetBlendTexCount();
	}

	return count;
}

void CTerrainZone::SetChunkInvisible()
{
	size_t chunk_num = m_nChunkNum;
	CTerrainChunk* pChunks = m_pChunks;

	for (size_t i = 0; i < chunk_num; ++i)
	{
		pChunks[i].SetVisible(false);
	}
}

bool CTerrainZone::CreateHeight()
{
	if (!BeginCreateHeight())
	{
		return false;
	}
	
	if (!m_pHeightLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_HEIGHT, false);
		return false;
	}
	
	if (!m_pHeightLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_HEIGHT, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_HEIGHT, true))
	{
		return false;
	}
	
	return true;
}

bool CTerrainZone::BeginCreateHeight()
{
	m_pHeightLoader = CHeightLoader::NewInstance();
	m_pHeightLoader->SetOwner(this);

	const char* file_path = m_pTerrain->GetFilePath();
	char fname[256];
	
	if (m_pTerrain->GetDesignMode())
	{
		// 加载高度文件
		SafeSprintf(fname, sizeof(fname), "%s%s%s.height", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pHeightLoader->SetFileName(fname);
	}
	else
	{
		// 加载压缩高度文件
		SafeSprintf(fname, sizeof(fname), "%s%s%s.ground", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pHeightLoader->SetGroundFile(fname);
	}

	const char* height_map = m_pTerrain->GetHeightMap();
	
	if (!StringEmpty(height_map))
	{
		// 加载测试用的高度图
		SafeSprintf(fname, sizeof(fname), "%s%s%s", 
			m_pRender->GetPathPrefix(file_path), file_path, height_map);
		
		int mirror = (int)(((m_nRow & 0x1) << 1) + (m_nCol & 0x1));

		m_pHeightLoader->SetBitmap(fname, mirror);
	}
	
	// 水面设计信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.water", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pHeightLoader->SetWaterFile(fname);
	// 碰撞块的尺寸
	m_pHeightLoader->SetCollideScale(m_pTerrain->GetCollideScale());

	return true;
}

 
bool CTerrainZone::CreateLight()
{
	if (!BeginCreateLight())
	{
		return false;
	}
	
	if (!m_pLightLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_LIGHT, false);
		return false;
	}
	
	if (!m_pLightLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_LIGHT, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_LIGHT, true))
	{
		return false;
	}
	
	return true;
}

bool CTerrainZone::BeginCreateLight()
{
	m_pLightLoader = CLightLoader::NewInstance();
	m_pLightLoader->SetOwner(this);
	
	char file_path[256];
	const char* light_path = m_pTerrain->GetZoneLightPath();

	if (StringEmpty(light_path))
	{
		SafeSprintf(file_path, sizeof(file_path), 
			m_pTerrain->GetFilePath());
	}
	else if (light_path[strlen(light_path) - 1] == '\\')
	{
		SafeSprintf(file_path, sizeof(file_path), "%s%s", 
			m_pTerrain->GetFilePath(), light_path);
	}
	else
	{
		SafeSprintf(file_path, sizeof(file_path),	"%s%s\\", 
			m_pTerrain->GetFilePath(), light_path);
	}
	
	char fname[256];
	
	// 加载混合颜色图
	if (m_pTerrain->GetDesignMode())
	{
		SafeSprintf(fname, sizeof(fname), "%s%s%s.tga", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());

		m_pLightLoader->SetBlendFile(fname);
	}
	
	// 加载光贴图
	if (!m_pTerrain->GetNoLight())
	{
		if (m_pTerrain->GetDesignMode())
		{
			// 未合并混合颜色的位图
			SafeSprintf(fname, sizeof(fname), "%s%s%s.bmp", 
				m_pRender->GetPathPrefix(file_path), file_path, GetName());
			
			m_pLightLoader->SetBitmap(fname);
		}
		else
		{
			// 合并混合颜色的光贴图
			SafeSprintf(fname, sizeof(fname), "%s%s%s%s", 
				m_pRender->GetPathPrefix(file_path), file_path, GetName(),m_pRender->GetTexExtfix());

			m_pLightLoader->SetTexFile(fname);
		}

		CLightMapConfig* pLightMapConfig = m_pTerrain->GetLigthMapConfig();
		float x;
		float y;
		float z;

		pLightMapConfig->GetZoneScale(GetName(), x, y, z);
		GetLight()->SetLightConfig(FmVec3(x, y, z));
	}

	return true;
}
 

bool CTerrainZone::CreateAlpha()
{
	if (!BeginCreateAlpha())
	{
		return false;
	}
	
	if (!m_pAlphaLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_ALPHA, false);
		return false;
	}
	
	if (!m_pAlphaLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_ALPHA, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_ALPHA, true))
	{
		return false;
	}
	
	return true;
}

bool CTerrainZone::BeginCreateAlpha()
{
	m_pAlphaLoader = CAlphaLoader::NewInstance();
	m_pAlphaLoader->SetOwner(this);
	
	const char* file_path = m_pTerrain->GetFilePath();
	char fname[256];
	
	if (m_pTerrain->GetBaseTexCount() > 1)
	{
		// 基础贴图设计信息
		SafeSprintf(fname, sizeof(fname), "%s%s%s.alphabase", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		
		m_pAlphaLoader->SetBaseFile(fname);
	}
	
	// 最终混合贴图信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.alphamap", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	
	m_pAlphaLoader->SetMapFile(fname);
	
	return true;
}

bool CTerrainZone::CreateVisual()
{
	if (!BeginCreateVisual())
	{
		return false;
	}
	
	if (!m_pVisualLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_VISUAL, false);
		return false;
	}
	
	if (!m_pVisualLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_VISUAL, false);
		return false;
	}
	
	if (!EndCreate(RES_CATEGORY_VISUAL, true))
	{
		return false;
	}
	
	return true;
}

bool CTerrainZone::BeginCreateVisual()
{
	m_pVisualLoader = CVisualLoader::NewInstance();
	m_pVisualLoader->SetOwner(this);
	
	const char* file_path = m_pTerrain->GetFilePath();
	char fname[256];
	
	// 地面物体信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.visual", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pVisualLoader->SetFileName(fname);
	
	// 装饰物信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.widget", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pVisualLoader->SetWidgetFile(fname);

	if (m_pTerrain->GetDesignMode())
	{
		// 辅助模型信息
		SafeSprintf(fname, sizeof(fname), "%s%s%s.helper_model", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pVisualLoader->SetHelperFile(fname);
		
		// 附加碰撞属性
		SafeSprintf(fname, sizeof(fname), "%s%s%s.collide_info", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pVisualLoader->SetCollideFile(fname);

		// 物体描述信息
		SafeSprintf(fname, sizeof(fname), "%s%s%s.desc", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pVisualLoader->SetDescFile(fname);
	}
	else
	{
		// if (不是中文版)
		{
			// 物体描述信息
			SafeSprintf(fname, sizeof(fname), "%s%s%s.desc", 
				m_pRender->GetPathPrefix(file_path), file_path, GetName());
			m_pVisualLoader->SetDescFile(fname);
		}
	}

	if (!m_pTerrain->GetNoLight())
	{
		// 合并光贴图映射信息
		SafeSprintf(fname, sizeof(fname), "%s%s%s.atlas", 
			m_pRender->GetPathPrefix(file_path), file_path, GetName());
		m_pVisualLoader->SetAtlasFile(fname);
	}

	// 声音信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.sound", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pVisualLoader->SetSoundFile(fname);

	// 花草信息
	SafeSprintf(fname, sizeof(fname), "%s%s%s.grass", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pVisualLoader->SetGrassFile(fname);
	
	return true;
}

bool CTerrainZone::CreateWalk()
{
	if (!BeginCreateWalk())
	{
		return false;
	}

	if (!m_pWalkLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_WALK, false);
		return false;
	}

	if (!m_pWalkLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_WALK, false);
		return false;
	}

	if (!EndCreate(RES_CATEGORY_WALK, true))
	{
		return false;
	}

	return true;
}

bool CTerrainZone::BeginCreateWalk()
{
	m_pWalkLoader = CWalkLoader::NewInstance();
	m_pWalkLoader->SetOwner(this);

	char file_path[256];
	const char* walk_path = m_pTerrain->GetWalkablePath();

	if (StringEmpty(walk_path))
	{
		SafeSprintf(file_path, sizeof(file_path), 
			m_pTerrain->GetFilePath());
	}
	else if (walk_path[strlen(walk_path) - 1] == '\\')
	{
		SafeSprintf(file_path, sizeof(file_path), "%s%s", 
			m_pTerrain->GetFilePath(), walk_path);
	}
	else
	{
		SafeSprintf(file_path, sizeof(file_path), "%s%s\\", 
			m_pTerrain->GetFilePath(), walk_path);
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s%s.walk", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pWalkLoader->SetFileName(fname);

	return true;
}

bool CTerrainZone::CreateRegion()
{
	if (!BeginCreateRegion())
	{
		return false;
	}

	if (!m_pRegionLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_REGION, false);
		return false;
	}

	if (!m_pRegionLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_REGION, false);
		return false;
	}

	if (!EndCreate(RES_CATEGORY_REGION, true))
	{
		return false;
	}

	return true;
}

bool CTerrainZone::BeginCreateRegion()
{
	m_pRegionLoader = CRegionLoader::NewInstance();
	m_pRegionLoader->SetOwner(this);

	const char* file_path = m_pTerrain->GetFilePath();

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s%s.region", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pRegionLoader->SetFileName(fname);

	SafeSprintf(fname, sizeof(fname), "%s%s%s.area", 
		m_pRender->GetPathPrefix(file_path), file_path, GetName());
	m_pRegionLoader->SetAreaFile(fname);

	return true;
}

#ifdef PHYSICS_HAVOK_TERRAIN
bool CTerrainZone::CreateRigid()
{
	if (!m_pRender->GetSupportPhysics())
	{
		return true;
	}

	if (!BeginCreateRigid())
	{
		return false;
	}

	if (!m_pRigidLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_RIGID, false);
		return false;
	}

	if (!m_pRigidLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_RIGID, false);
		return false;
	}

	if (!EndCreate(RES_CATEGORY_RIGID, true))
	{
		return false;
	}

	return true;
}
 
bool CTerrainZone::BeginCreateRigid()
{
	if (!m_pRender->GetSupportPhysics())
	{
		return true;
	}

	m_pRigidLoader = CRigidLoader::NewInstance();

	m_pRigidLoader->SetOwner(this);
	m_pRigidLoader->SetHeights((float*)m_pHeight->GetHeightValues());

	return true;
}
#endif // PHYSICS_HAVOK_TERRAIN

bool CTerrainZone::EndCreate(int category, bool succeed)
{
	if (category == RES_CATEGORY_HEIGHT)
	{
		SAFE_RELEASE(m_pHeightLoader);
	}
	//else if (category == RES_CATEGORY_LIGHT)
	//{
	//	SAFE_RELEASE(m_pLightLoader);
	//}
	else if (category == RES_CATEGORY_ALPHA)
	{
		SAFE_RELEASE(m_pAlphaLoader);
	}
	else if (category == RES_CATEGORY_VISUAL)
	{
		SAFE_RELEASE(m_pVisualLoader);
	}
	else if (category == RES_CATEGORY_WALK)
	{
		SAFE_RELEASE(m_pWalkLoader);
	}
	else if (category == RES_CATEGORY_REGION)
	{
		SAFE_RELEASE(m_pRegionLoader);
	}
#ifdef PHYSICS_HAVOK_TERRAIN
	else if (category == RES_CATEGORY_RIGID)
	{
		SAFE_RELEASE(m_pRigidLoader);
	}
#endif // PHYSICS_HAVOK_TERRAIN
	
	return true;
}

IResLoader* CTerrainZone::GetHeightLoader()
{ 
	return m_pHeightLoader; 
}

IResLoader* CTerrainZone::GetLightLoader() 
{ 
	return m_pLightLoader; 
}

IResLoader* CTerrainZone::GetAlphaLoader()
{ 
	return m_pAlphaLoader; 
}

IResLoader* CTerrainZone::GetVisualLoader()
{
	return m_pVisualLoader;
}

IResLoader* CTerrainZone::GetWalkLoader()
{
	return m_pWalkLoader;
}

IResLoader* CTerrainZone::GetRegionLoader()
{
	return m_pRegionLoader;
}

#ifdef PHYSICS_HAVOK_TERRAIN
IResLoader* CTerrainZone::GetRigidLoader()
{
	return m_pRigidLoader;
}
#endif // PHYSICS_HAVOK_TERRAIN

float CTerrainZone::GetLoadPercent() const
{
	//float part = 1.0F / 7.0F;
	float part = 1.0F / 6.0F;
	float percent = 0.0F;

#ifdef PHYSICS_HAVOK_TERRAIN
	if (m_pRender->GetSupportPhysics())
	{
		part = 1.0F / 8.0F;
	}
#endif // PHYSICS_HAVOK_TERRAIN

	if (m_bHeightLoaded)
	{
		percent += part;
	}

	//if (m_bLightLoaded)
	//{
	//	percent += part;
	//}

	if (m_bAlphaLoaded)
	{
		percent += part;
	}

	if (m_bVisualLoaded)
	{
		percent += part;
	}

	if (m_bWalkLoaded)
	{
		percent += part;
	}

	if (m_bRegionLoaded)
	{
		percent += part;
	}

#ifdef PHYSICS_HAVOK_TERRAIN
	if (m_bRigidLoaded && m_pRender->GetSupportPhysics())
	{
		percent += part;
	}
#endif // PHYSICS_HAVOK_TERRAIN

	percent += float(m_nLoadCollideNum) / float(m_nChunkNum) * part;

	return percent;
}

bool CTerrainZone::LoadNextCollide()
{
	if (m_nLoadCollideNum >= m_nChunkNum)
	{
		return false;
	}

	CreateChunkCollide(m_nLoadCollideNum++);
	
	return true;
}

