//--------------------------------------------------------------------
// 文件名:		walk_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年8月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WALK_LOADER_H
#define _WALK_LOADER_H

#include "terrain_walkable.h"

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "../public/i_interface.h"

// 行走信息加载器

class CTerrainZone;
class IBlockReader;

class CWalkLoader: public IResLoader
{
public:
	static CWalkLoader* NewInstance();

	static IBlockReader* CreateBlockReader(int block_id, void* context);

	struct walk_all_t
	{
		unsigned char* pWaterHeight;
		size_t nWaterHeightSize;
		unsigned char* pWalkMarker;
		size_t nWalkMarkerSize;
		unsigned char* pFloorHeights[MAX_FLOOR_NUM];
		size_t nFloorHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pSpaceHeights[MAX_FLOOR_NUM];
		size_t nSpaceHeightsSize[MAX_FLOOR_NUM];
		unsigned char* pFloorMarkers[MAX_FLOOR_NUM];
		size_t nFloorMarkersSize[MAX_FLOOR_NUM];
		//unsigned char* pStandMarkers[MAX_FLOOR_NUM];
		//size_t nStandMarkersSize[MAX_FLOOR_NUM];
		//unsigned char* pWallMarkers[MAX_FLOOR_NUM];
		//size_t nWallMarkersSize[MAX_FLOOR_NUM];
		size_t nFloorCount;
		//size_t nWallCount;
	};
	
public:
	CWalkLoader();
	virtual ~CWalkLoader();

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);
	
	// 资源主体
	void SetOwner(CTerrainZone* pOwner);
	// 高度文件名
	void SetFileName(const char* file_name);
	
	// 保存行走信息文件
	static bool SaveBlockWalk(const char* file_name, CTerrainZone* pZone);
	
private:
	CWalkLoader(const CWalkLoader&);
	CWalkLoader& operator=(const CWalkLoader&);
	
	// 加载行走信息
	bool LoadBlockWalk(const char* file_name);

private:
	CTerrainZone* m_pOwner;
	core_string m_strFileName;
	size_t m_nRows;
	size_t m_nCols;
	walk_all_t m_Walk;
};

#endif // _WALK_LOADER_H
