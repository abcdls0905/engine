//--------------------------------------------------------------------
// 文件名:		DynamicWalkable.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月28日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKABLE_H
#define _DYNAMICWALKABLE_H

//#include "TerrainWalkable.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

#define MAX_FLOOR_NUM 15
// 无效高度值
#define HEIGHT_NULL 1e8

// 层标记
#define FLOOR_MARKER_EXISTS 0x1
#define FLOOR_MARKER_MOVE	0x2
#define FLOOR_MARKER_STAND	0x4
#define FLOOR_MARKER_WALL	0x8
#define FLOOR_MARKER_EDGE	0x10

// 高度值是否无效
// inline bool height_is_null(float height)
// {
// 	return (height > 1e7);
// }

// 动态物件可行走信息

class CWalkHeightMap;
class CWalkFlagMap;
class Terrain;
class IVisBase;
class CDynamicWalkLoader;

class CDynamicWalkable
{
private:
	struct entry_point_t 
	{
		size_t row;
		size_t col;

		bool operator==(const entry_point_t& other) const
		{
			return other.row == row && other.col == col;
		}
	};
public:
	CDynamicWalkable(Terrain* pTerrain, IVisBase* pVisBase,
		const char* model_file_name, const char* file_name,
		size_t rows, size_t cols);
	~CDynamicWalkable();
	
	// 清除所有数据
	void ReleaseAll();

	// 文件名
	const char* GetFileName() const { return m_strFileName.c_str(); }
	const char* GetModelFileName() const { return m_strModelFileName.c_str(); }

	result_string GetWalkFileName() const;

	// 行列数
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	CWalkHeightMap* GetFloorHeightMap(size_t i) { return m_pFloorHeights[i]; }
	CWalkHeightMap* GetSpaceHeightMap(size_t i) { return m_pSpaceHeights[i]; }
	CWalkFlagMap* GetFloorMarkerMap(size_t i) { return m_pFloorMarkers[i]; }

	// 数据是否就绪
	void SetReady(bool value) { m_bReady = value; }
	bool GetReady() const { return m_bReady; }

	// 获得总的行走层数量
	size_t GetFloorCount() const { return m_nFloorCount; }
	// 获得指定位置的最大高度
	float GetApexHeight(float height, size_t row, size_t col) const;
	// 获得指定位置的最大高度所在的层
	int GetApexFloor(size_t row, size_t col);
	// 获得指定位置的最小高度
	float GetBottomHeight(float height, size_t row, size_t col) const;
	// 获得指定位置的最小高度所在的层
	int GetBottomFloor(size_t row, size_t col);
	// 获得行走层高度
	float GetFloorHeight(float height, size_t row, size_t col, int floor) const;
	// 获得行走层的无障碍高度
	float GetFloorSpace(size_t row, size_t col, int floor) const;
	// 获得行走层标记
	unsigned int GetFloorMarker(size_t row, size_t col, int floor) const;
	// 获得指定高度位置是否存在墙面
	bool GetWallExists(float height, size_t row, size_t col, float y) const;

	// 更新高度数据
	bool UpdateFloorHeight(int floor, size_t rows, size_t cols,
		float* pValues);
	bool UpdateSpaceHeight(int floor, size_t rows, size_t cols,
		float* pValues);
	// 更新标记数据
	bool UpdateFloorMarker(int floor, size_t rows, size_t cols,
		unsigned char* pValues);

	// 创建
	bool BuildFloorHeight(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);
	bool BuildSpaceHeight(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);
	bool BuildFloorMarker(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);

	// 加载碰撞数据
	bool Load();
	// 卸载碰撞数据
	bool Unload();

	// 添加动态物件
	bool AddDynamicWalkable(IVisBase* pVisBase);
	// 删除动态物件
	bool DeleteDynamicWalkable(IVisBase* pVisBase);

	bool CanInsert(IVisBase* pVisBase);
	bool InsertDynamicWalkable(IVisBase* pVisBase);
	bool RemoveDynamicWalkable(IVisBase* pVisBase);

	bool IsLoaded() const { return m_bLoaded; }

	// 动态物件数
	size_t GetCount() const { return m_DynamicVisualArr.size(); }

	// 动态物件角度
	float GetAngleY() const { return m_fAngleY; }
	void SetAngleY(float value) { m_fAngleY = value; }

	float GetHeight() const { return m_fHeight; }
	void SetHeight(float value) { m_fHeight = value; }

	// 外切圆半径
	float GetRadius() const { return m_fRadius; }
	void SetRadius(float value) { m_fRadius = value; }

	// 包围盒尺寸
	float GetBoxSizeX() const { return m_fBoxSizeX; }
	void SetBoxSizeX(float value) { m_fBoxSizeX = value; }

	float GetBoxSizeZ() const { return m_fBoxSizeZ; }
	void SetBoxSizeZ(float value) { m_fBoxSizeZ = value; }

	void GetEntryPoints(IVisBase* pVisBase, int floor, IVarList& result);
	int GetEntryPointsCount(IVisBase* pVisBase, int floor);

	bool UpdateEntryPoints();

    bool EndCreate(int category, bool succeed);

private:
	CDynamicWalkable();
	CDynamicWalkable(const CDynamicWalkable&);
	CDynamicWalkable& operator=(const CDynamicWalkable&);

private:
    Terrain* m_pTerrain;

    // 动态碰撞数据加载器
    CDynamicWalkLoader* m_pDynamicWalkLoader;

	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strModelFileName;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nFloorCount;
	bool m_bReady;
	bool m_bLoaded;
    bool m_bAsync;
    bool m_bLoading;

	float m_fHeight;
	float m_fAngleY;
	float m_fRadius;
	float m_fBoxSizeX;
	float m_fBoxSizeZ;

	CWalkHeightMap* m_pFloorHeights[MAX_FLOOR_NUM];		// 层高度
	CWalkHeightMap* m_pSpaceHeights[MAX_FLOOR_NUM];		// 无障碍空间高度
	//CWalkFlagMap* m_pWalkMarker;						// 可行走标记
	CWalkFlagMap* m_pFloorMarkers[MAX_FLOOR_NUM];		// 行走层标记

	TArrayPod<entry_point_t, 1, TCoreAlloc> m_EntryPoints[MAX_FLOOR_NUM];

	TArrayPod<PERSISTID, 1, TCoreAlloc> m_DynamicVisualArr;	// 记录动态物件
};
#endif // _DYNAMICWALKABLE_H
