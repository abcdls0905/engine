//--------------------------------------------------------------------
// 文件名:		terrain_walkable.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年8月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_WALKABLE_H
#define _TERRAIN_WALKABLE_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"

// 最大行走层数
#define MAX_FLOOR_NUM 15
// 无效高度值
#define HEIGHT_NULL 1e8

// 层标记
#define FLOOR_MARKER_EXISTS 0x1
#define FLOOR_MARKER_MOVE	0x2
#define FLOOR_MARKER_STAND	0x4
#define FLOOR_MARKER_WALL	0x8

// 高度值是否无效
inline bool height_is_null(float height)
{
	return (height > 1e7);
}

// 地形的可行走信息

class Terrain;
class CTerrainZone;
class CWalkHeightMap;
class CWalkFlagMap;
class CWalkIDMap;
class IVisBase;

class CTerrainWalkable
{
public:
	CTerrainWalkable(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainWalkable();

	// 清除所有数据
	void ReleaseAll();

	// 行列数
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	CWalkHeightMap* GetWaterHeightMap() { return m_pWaterHeight; }
	CWalkHeightMap* GetFloorHeightMap(int i) { return m_pFloorHeights[i]; }
	CWalkHeightMap* GetSpaceHeightMap(int i) { return m_pSpaceHeights[i]; }
	CWalkFlagMap* GetWalkMarkerMap() { return m_pWalkMarker; }
	CWalkFlagMap* GetFloorMarkerMap(int i) { return m_pFloorMarkers[i]; }

	CWalkHeightMap* GetDynamicFloorHeightMap(int i) { return m_pDynamicFloorHeights[i]; }
	CWalkHeightMap* GetDynamicSpaceHeightMap(int i) { return m_pDynamicSpaceHeights[i]; }
	CWalkFlagMap* GetDynamicFloorMarkerMap(int i) { return m_pDynamicFloorMarkers[i]; }
	CWalkIDMap* GetDynamicFloorIDMap(int i) { return m_pDynamicFloorIDs[i]; }

	// 数据是否就绪
	void SetReady(bool value) { m_bReady = value; }
	bool GetReady() const { return m_bReady; }

	// 获得水面高度
	float GetWaterHeight(size_t row, size_t col);
	// 获得主行走层标记
	unsigned int GetWalkMarker(size_t row, size_t col);
	// 获得总的行走层数量
	int GetFloorCount()	{ return m_nFloorCount + ((m_nDynamicFloorCount > 1) ? (m_nDynamicFloorCount - 1) : m_nDynamicFloorCount); }
	// 获得行走层标记
	unsigned int GetFloorMarker(size_t row, size_t col, int floor);
	// 获得行走层高度
	float GetFloorHeight(size_t row, size_t col, int floor);
	// 获得行走层的无障碍高度
	float GetFloorSpace(size_t row, size_t col, int floor);
	// 获得指定高度位置是否存在墙面
	bool GetWallExists(size_t row, size_t col, float y);
	// 获得行走层ID
	IVisBase* GetFloorObject(size_t row, size_t col, int floor);

	// 获得动态的行走层数量
	int GetStaticFloorCount()	{ return m_nFloorCount; }
	// 获得动态的行走层数量
	int GetDynamicFloorCount()	{ return m_nDynamicFloorCount; }

	int GetApexStaticFloor(size_t row, size_t col);
	int GetApexDynamicFloor(size_t row, size_t col);
	// 获得行走静态层标记
	unsigned int GetStaticFloorMarker(size_t row, size_t col, int floor);
	// 获得行走静态层高度
	float GetStaticFloorHeight(size_t row, size_t col, int floor);
	// 获得行走静态层的无障碍高度
	float GetStaticFloorSpace(size_t row, size_t col, int floor);

	// 获得行走动态层标记
	unsigned int GetDynamicFloorMarker(size_t row, size_t col, int floor);
	// 获得行走动态层高度
	float GetDynamicFloorHeight(size_t row, size_t col, int floor);
	// 获得行走动态层的无障碍高度
	float GetDynamicFloorSpace(size_t row, size_t col, int floor);

	// 保存行走数据图
	bool SaveWalkMap(const char* file_name);

	// 更新高度数据
	bool UpdateGroundHeight(size_t rows, size_t cols, float* pValues);
	bool UpdateWaterHeight(size_t rows, size_t cols, float* pValues);
	bool UpdateFloorHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	bool UpdateSpaceHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	// 更新标记数据
	bool UpdateWalkMarker(size_t rows, size_t cols, 
		unsigned char* pValues);
	bool UpdateFloorMarker(int floor, size_t rows, size_t cols, 
		unsigned char* pValues);

	// 更新高度数据
	bool UpdateDynamicFloorHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	bool UpdateDynamicSpaceHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	// 更新标记数据
	bool UpdateDynamicFloorMarker(int floor, size_t rows, size_t cols, 
		unsigned char* pValues);
	// 更新ID数据
	bool UpdateDynamicFloorID(int floor, size_t rows, size_t cols, 
		PERSISTID* pValues);

	// 创建
	bool BuildWaterHeight(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);
	bool BuildFloorHeight(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size, int floor);
	bool BuildSpaceHeight(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size, int floor);
	bool BuildWalkMarker(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size);
	bool BuildFloorMarker(size_t rows, size_t cols, unsigned char* pdata, 
		size_t size, int floor);

private:
	CTerrainWalkable();
	CTerrainWalkable(const CTerrainWalkable&);
	CTerrainWalkable& operator=(const CTerrainWalkable&);

	// 保存高度图
	bool SaveHeightMap(CWalkHeightMap* pHeight, const char* file_name);
	// 保存标记图
	bool SaveFlagMap(CWalkFlagMap* pFlagMap, const char* file_name);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nFloorCount;
	size_t m_nDynamicFloorCount;
	bool m_bReady;

	CWalkHeightMap* m_pGroundHeight;				// 地表高度
	CWalkHeightMap* m_pWaterHeight;					// 水面高度
	CWalkHeightMap* m_pFloorHeights[MAX_FLOOR_NUM];	// 层高度
	CWalkHeightMap* m_pSpaceHeights[MAX_FLOOR_NUM];	// 无障碍空间高度
	CWalkFlagMap* m_pWalkMarker;					// 可行走标记
	CWalkFlagMap* m_pFloorMarkers[MAX_FLOOR_NUM];	// 行走层标记

	CWalkHeightMap* m_pDynamicFloorHeights[MAX_FLOOR_NUM];	// 动态层高度
	CWalkHeightMap* m_pDynamicSpaceHeights[MAX_FLOOR_NUM];	// 动态层无障碍空间高度
	CWalkFlagMap* m_pDynamicFloorMarkers[MAX_FLOOR_NUM];	// 动态行走层标记
	CWalkIDMap* m_pDynamicFloorIDs[MAX_FLOOR_NUM];			// 行走层ID
};

#endif // _TERRAIN_WALKABLE_H
