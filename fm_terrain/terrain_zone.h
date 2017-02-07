//--------------------------------------------------------------------
// 文件名:		terrain_zone.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_ZONE_H
#define _TERRAIN_ZONE_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../public/i_interface.h"
#include "../visual/i_res_base.h"
#include "../visual/i_res_loader.h"
#include "terrain_chunk.h"
// 调试物理引擎需要的头文件
//#include "../physics/physics_macros.h"
//#include <xmmintrin.h>

inline int FloatToInt(float val)
{
	//return _mm_cvtt_ss2si(_mm_load_ss(&val));
	return int(val);
}

// 地形区域（包含一组分块）

class Terrain;
class CQuadNode;
class CTerrainFactory;
class CTerrainChunk;
class CTerrainHeight;
class CTerrainLight;
class CTerrainVisuals;
class CTerrainWalkable;
class CTerrainRegion;
#ifdef PHYSICS_HAVOK_TERRAIN
class CTerrainRigid;
#endif // PHYSICS_HAVOK_TERRAIN
class CHeightLoader;
class CLightLoader;
class CAlphaLoader;
class CVisualLoader;
class CWalkLoader;
class CRegionLoader;
#ifdef PHYSICS_HAVOK_TERRAIN
class CRigidLoader;
#endif // PHYSICS_HAVOK_TERRAIN

class CTerrainZone: public IResBase
{
public:
	// 加载资源类型
	enum
	{
		RES_CATEGORY_HEIGHT,	// 顶点高度信息
		RES_CATEGORY_LIGHT,		// 光照信息
		RES_CATEGORY_ALPHA,		// 贴图混合信息
		RES_CATEGORY_VISUAL,	// 地面物体信息
		RES_CATEGORY_WALK,		// 行走信息
		RES_CATEGORY_REGION,	// 标记信息
#ifdef PHYSICS_HAVOK_TERRAIN
		RES_CATEGORY_RIGID,		// 刚体地形信息
#endif // PHYSICS_HAVOK_TERRAIN
	};

public:
	CTerrainZone(Terrain* pTerrain, CTerrainFactory* pFactory,
		size_t index, size_t row, size_t col);
	~CTerrainZone();

	// 释放
	virtual void Release();
	
	Terrain* GetTerrain() const { return m_pTerrain; }

	// 指向对应的四叉树节点
	void SetQuadNode(CQuadNode* value) { m_pQuadNode = value; }
	CQuadNode* GetQuadNode() const { return m_pQuadNode; }

	// 对象号
	PERSISTID GetID() const { return m_PersistID; }
	void SetID(const PERSISTID& value) { m_PersistID = value; }

	// 设置名称
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// 获得名称
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// 索引
	size_t GetIndex() const { return m_nIndex; }
	// 行
	size_t GetRow() const { return m_nRow; }
	// 列
	size_t GetCol() const { return m_nCol; }
	
	// 顶点高度
	CTerrainHeight* GetHeight() const { return m_pHeight; }
	
	// 光照
	CTerrainLight* GetLight() const { return m_pLight; }

	// 地面物体
	CTerrainVisuals* GetVisuals() const { return m_pVisuals; }
	// 可行走信息
	CTerrainWalkable* GetWalkable() const { return m_pWalkable; }
	// 标记信息
	CTerrainRegion* GetRegion() const { return m_pRegion; }
#ifdef PHYSICS_HAVOK_TERRAIN
	// 刚体地形信息
	CTerrainRigid* GetRigid() const { return m_pRigid; };
#endif // PHYSICS_HAVOK_TERRAIN
	
	// 初始化
	bool Init();
	
	// 在整个地形中的格子起始行列
	size_t GetGlobalRow() const { return m_nGlobalRow; }
	size_t GetGlobalCol() const { return m_nGlobalCol; }
	
	// 在整个地形中的ALPHA值起始行列
	size_t GetAlphaRow() const { return m_nAlphaRow; }
	size_t GetAlphaCol() const { return m_nAlphaCol; }

	// 起始坐标
	float GetLeft() const { return m_fLeft; }
	float GetTop() const { return m_fTop; }
	float GetRight() const { return m_fRight; }
	float GetBottom() const { return m_fBottom; }

	// 无显示静默时间
	void SetSilenceTime(float value) { m_fSilenceTime = 0.0F; }
	float GetSilenceTime() const { return m_fSilenceTime; }
	float IncSilenceTime(float d) { m_fSilenceTime += d; return m_fSilenceTime; }
	
	// 获得分块数量
	CTerrainChunk* GetChunks() { return m_pChunks; }
	size_t GetChunkCount() const { return m_nChunkNum; }
	size_t GetChunkDims() const { return m_nChunkDims; }

	// 获得指定索引分块
	CTerrainChunk* GetChunkByIndex(size_t index)
	{
		Assert(index < m_nChunkNum);

		return m_pChunks + index;
	}
	
	// 获得指定位置分块
	CTerrainChunk* GetChunk(size_t row, size_t col)
	{
		Assert(row < m_nChunkDims);
		Assert(col < m_nChunkDims);

		return m_pChunks + row * m_nChunkDims + col;
	}
	
	// 获得相邻的分块
	CTerrainChunk* GetLeftChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		int col = chunk->GetCol();

		if ((col - 1) >= 0)
		{
			return m_pChunks + chunk->GetIndex() - 1;
		}

		return NULL;
	}

	CTerrainChunk* GetRightChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		size_t col = chunk->GetCol();

		if ((col + 1) < m_nChunkDims)
		{
			return m_pChunks + (chunk->GetIndex() + 1);
		}

		return NULL;
	}

	CTerrainChunk* GetUpChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		int row = chunk->GetRow();

		if ((row - 1) >= 0)
		{
			return m_pChunks + (chunk->GetIndex() - m_nChunkDims);
		}

		return NULL;
	}

	CTerrainChunk* GetDownChunk(CTerrainChunk* chunk)
	{
		Assert(chunk != NULL);

		size_t row = chunk->GetRow();

		if ((row + 1) < m_nChunkDims)
		{
			return m_pChunks + (chunk->GetIndex() + m_nChunkDims);
		}

		return NULL;
	}

	// 获得边缘的分块
	CTerrainChunk* GetLeftEdgeChunk(size_t row)
	{
		Assert(row < m_nChunkDims);

		return m_pChunks + row * m_nChunkDims;
	}

	CTerrainChunk* GetRightEdgeChunk(size_t row)
	{
		Assert(row < m_nChunkDims);

		return m_pChunks + (row * m_nChunkDims + m_nChunkDims - 1);
	}

	CTerrainChunk* GetUpEdgeChunk(size_t col)
	{
		Assert(col < m_nChunkDims);

		return m_pChunks + col;
	}

	CTerrainChunk* GetDownEdgeChunk(size_t col)
	{
		Assert(col < m_nChunkDims);

		return m_pChunks + ((m_nChunkDims - 1) * m_nChunkDims + col);
	}

	// 更新相邻分块
	void UpdateChunkSibling();

	// 指定位置属于哪个分块
	CTerrainChunk* GetInChunk(float x, float z)
	{
		int chunk_r = FloatToInt((z - m_fTop) * m_fInvChunkSize);
		int chunk_c = FloatToInt((x - m_fLeft) * m_fInvChunkSize);
		size_t chunk_dims = m_nChunkDims;

		if ((size_t(chunk_r) >= chunk_dims) || (size_t(chunk_c) >= chunk_dims))
		{
			return NULL;
		}

		return &m_pChunks[chunk_r * chunk_dims + chunk_c];
	}

	// 创建分块的碰撞信息
	bool CreateChunkCollide(size_t index);
	// 释放分块的碰撞信息
	bool ReleaseChunkCollide(size_t index);

	// 获得碰撞对象
	size_t GetCollideNum() const { return m_nCollideNum; }
	size_t GetCollideDims() const { return m_nCollideDims; }
	CQuadGround* GetCollides() { return m_pCollides; }

	// 释放所有可见物体及其碰撞信息
	void ReleaseVisualObjects();
	// 释放所有分块的碰撞信息
	void ReleaseChunkObjects();
	// 重置所有分块的顶点缓冲
	void ReleaseChunkVB();
	// 释放所有分块的AlphaMap
	void ReleaseChunkAlphaMap();
	// 释放所有分块的AlphaMap占用的贴图资源
	void ReleaseChunkAlphaTex();
	// 释放所有分块的花草顶点缓冲
	void ReleaseChunkGrassVB();
	// 释放所有加载器
	void ReleaseAllLoader();
	
	// 获得所有分块的AlphaMap数量
	size_t GetChunkAlphaMapCount();

	// 设置所有分块不可见
	void SetChunkInvisible();
	
	// 同步创建
	bool CreateHeight();
	// 开始异步创建 
	bool BeginCreateHeight();
	
	// 同步创建
	bool CreateLight();
	// 开始异步创建 
	bool BeginCreateLight();

	// 同步创建
	bool CreateAlpha();
	// 开始异步创建 
	bool BeginCreateAlpha();

	// 同步创建
	bool CreateVisual();
	// 开始异步创建 
	bool BeginCreateVisual();
	
	// 同步创建
	bool CreateWalk();
	// 开始异步创建 
	bool BeginCreateWalk();

	// 同步创建
	bool CreateRegion();
	// 开始异步创建 
	bool BeginCreateRegion();

#ifdef PHYSICS_HAVOK_TERRAIN
	// 同步创建
	bool CreateRigid();
	// 开始异步创建 
	bool BeginCreateRigid();
#endif // PHYSICS_HAVOK_TERRAIN

	// 结束异步创建 
	bool EndCreate(int category, bool succeed);

	// 获得异步加载器
	IResLoader* GetHeightLoader();
	IResLoader* GetLightLoader();
	IResLoader* GetAlphaLoader();
	IResLoader* GetVisualLoader();
	IResLoader* GetWalkLoader();
	IResLoader* GetRegionLoader();
#ifdef PHYSICS_HAVOK_TERRAIN
	IResLoader* GetRigidLoader();
#endif // PHYSICS_HAVOK_TERRAIN

	// 加载完成标志
	void SetHeightLoaded(bool value) { m_bHeightLoaded = value; }
	bool GetHeightLoaded() const { return m_bHeightLoaded; }
	void SetLightLoaded(bool value) { m_bLightLoaded = value; }
	bool GetLightLoaded() const { return m_bLightLoaded; }
	void SetAlphaLoaded(bool value) { m_bAlphaLoaded = value; }
	bool GetAlphaLoaded() const { return m_bAlphaLoaded; }
	void SetVisualLoaded(bool value) { m_bVisualLoaded = value; }
	bool GetVisualLoaded() const { return m_bVisualLoaded; }
	void SetWalkLoaded(bool value) { m_bWalkLoaded = value; }
	bool GetWalkLoaded() const { return m_bWalkLoaded; }
	void SetRegionLoaded(bool value) { m_bRegionLoaded = value; }
	bool GetRegionLoaded() const { return m_bRegionLoaded; }
#ifdef PHYSICS_HAVOK_TERRAIN
	void SetRigidLoaded(bool value) { m_bRigidLoaded = value; }
	bool GetRigidLoaded() const { return m_bRigidLoaded; }
#endif // PHYSICS_HAVOK_TERRAIN
	bool GetCollideLoaded() const { return m_nLoadCollideNum == m_nChunkNum; }
	
	// 所有资源加载完成
	bool GetAllLoaded() const
	{
#ifdef PHYSICS_HAVOK_TERRAIN
		bool rigid_loaded = true;

		if (m_pRender->GetSupportPhysics())
		{
			rigid_loaded = m_bRigidLoaded;
		}
#endif // PHYSICS_HAVOK_TERRAIN

		//return m_bHeightLoaded && m_bLightLoaded && m_bAlphaLoaded 
		return m_bHeightLoaded && m_bAlphaLoaded 
			&& m_bVisualLoaded && m_bWalkLoaded	&& m_bRegionLoaded
#ifdef PHYSICS_HAVOK_TERRAIN
			&& rigid_loaded
#endif // PHYSICS_HAVOK_TERRAIN
			&& (m_nLoadCollideNum == m_nChunkNum);
	}

	// 获得完成加载的比例
	float GetLoadPercent() const;

	// 数据是否被编辑器改变
	void SetHeightChanged(bool value) { m_bHeightChanged = value; }
	bool GetHeightChanged() const { return m_bHeightChanged; }
	void SetAlphaChanged(bool value) { m_bAlphaChanged = value; }
	bool GetAlphaChanged() const { return m_bAlphaChanged; }
	void SetLightChanged(bool value) { m_bLightChanged = value; }
	bool GetLightChanged() const { return m_bLightChanged; }
	void SetVisualChanged(bool value) { m_bVisualChanged = value; }
	bool GetVisualChanged() const { return m_bVisualChanged; }
	void SetWidgetChanged(bool value) { m_bWidgetChanged = value; }
	bool GetWidgetChanged() const { return m_bWidgetChanged; }
	void SetHelperModelChanged(bool value) { m_bHelperModelChanged = value; }
	bool GetHelperModelChanged() const { return m_bHelperModelChanged; }
	void SetCollideInfoChanged(bool value) { m_bCollideInfoChanged = value; }
	bool GetCollideInfoChanged() const { return m_bCollideInfoChanged; }
	void SetDescChanged(bool value) { m_bDescChanged = value; }
	bool GetDescChanged() const { return m_bDescChanged; }
	void SetSoundChanged(bool value) { m_bSoundChanged = value; }
	bool GetSoundChanged() const { return m_bSoundChanged; }
	void SetAreaChanged(bool value) { m_bAreaChanged = value; }
	bool GetAreaChanged() const { return m_bAreaChanged; }
	void SetRegionChanged(bool value) { m_bRegionChanged = value; }
	bool GetRegionChanged() const { return m_bRegionChanged; }
	// 编辑器中设置指定区域是否卸载
	void SetNoUnload(bool value) { m_bNoUnload = value; }
	bool GetNoUnload() const { return m_bNoUnload; }
	
	// 是否有被改变的项目
	bool GetAnyChanged() const
	{ 
		return m_bHeightChanged || m_bAlphaChanged || m_bLightChanged 
			|| m_bVisualChanged || m_bWidgetChanged || m_bHelperModelChanged 
			|| m_bCollideInfoChanged || m_bDescChanged || m_bSoundChanged 
			|| m_bAreaChanged || m_bRegionChanged;
	}

	// 是否有水面
	void SetHasWater(bool value) { m_bHasWater = value; }
	bool GetHasWater() const { return m_bHasWater; }

	// 加载下一个分块的碰撞信息
	bool LoadNextCollide();
	
private:
	CTerrainZone();
	CTerrainZone(const CTerrainZone&);
	CTerrainZone& operator=(const CTerrainZone&);

private:
	IRender* m_pRender;
	Terrain* m_pTerrain;
	CQuadNode* m_pQuadNode;
	CTerrainFactory* m_pFactory;
	PERSISTID m_PersistID;
	core_string m_strName;
	size_t m_nIndex;		// 区域索引
	size_t m_nRow;			// 行列号
	size_t m_nCol;
	size_t m_nGlobalRow;	// 全局顶点起始行列号
	size_t m_nGlobalCol;
	size_t m_nAlphaRow;		// 全局ALPHA起始行列号
	size_t m_nAlphaCol;
	float m_fLeft;			// 起点坐标
	float m_fTop;
	float m_fRight;
	float m_fBottom;
	float m_fSilenceTime;	// 静默时间
	
	CTerrainHeight* m_pHeight;
	CTerrainLight* m_pLight;
	CTerrainVisuals* m_pVisuals;		// 区域内的可见物体
	CTerrainWalkable* m_pWalkable;
	CTerrainRegion* m_pRegion;
#ifdef PHYSICS_HAVOK_TERRAIN
	CTerrainRigid* m_pRigid;
#endif // PHYSICS_HAVOK_TERRAIN
	
	CHeightLoader* m_pHeightLoader;
	CLightLoader* m_pLightLoader;
	CAlphaLoader* m_pAlphaLoader;
	CVisualLoader* m_pVisualLoader;
	CWalkLoader* m_pWalkLoader;
	CRegionLoader* m_pRegionLoader;
#ifdef PHYSICS_HAVOK_TERRAIN
	CRigidLoader* m_pRigidLoader;
#endif // PHYSICS_HAVOK_TERRAIN

	bool m_bHeightLoaded;
	bool m_bLightLoaded;
	bool m_bAlphaLoaded;
	bool m_bVisualLoaded;
	bool m_bWalkLoaded;
	bool m_bRegionLoaded;
#ifdef PHYSICS_HAVOK_TERRAIN
	bool m_bRigidLoaded;
#endif // PHYSICS_HAVOK_TERRAIN

	bool m_bHeightChanged;
	bool m_bAlphaChanged;
	bool m_bLightChanged;
	bool m_bVisualChanged;
	bool m_bWidgetChanged;
	bool m_bHelperModelChanged;
	bool m_bCollideInfoChanged;
	bool m_bDescChanged;
	bool m_bSoundChanged;
	bool m_bAreaChanged;
	bool m_bRegionChanged;
	bool m_bNoUnload;
	bool m_bHasWater;
	
	float m_fInvChunkSize;
	CTerrainChunk* m_pChunks;	// 区域内的分块
	size_t m_nChunkDims;
	size_t m_nChunkNum;
	size_t m_nLoadCollideNum;	// 已加载碰撞信息的分块数量
	size_t m_nCollideDims;
	size_t m_nCollideNum;		
	CQuadGround* m_pCollides;
};

#endif // _TERRAIN_ZONE_H
