//--------------------------------------------------------------------
// 文件名:		terrain_region.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_REGION_H
#define _TERRAIN_REGION_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../utils/array_pod.h"



// 范围标记

class Terrain;
class CTerrainZone;
class CWalkBitMap;

class CTerrainRegion
{
private:
	struct region_t
	{
		char* pName;
		unsigned int nHash;
		CWalkBitMap* pMap;
	};

	struct area_t
	{
		char* pName;
		unsigned int nHash;
		CWalkBitMap* pMap;
	};

public:
	CTerrainRegion(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainRegion();

	// 行列数
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// 查找范围标记
	bool FindRegionIndex(const char* name, size_t& index) const;
	// 获得范围标记数量
	size_t GetRegionCount() const;
	// 获得范围标记名称
	const char* GetRegionName(size_t index) const;
	// 获得范围标记数据
	CWalkBitMap* GetRegionMap(size_t index) const;
	// 获得范围标记值
	bool GetRegionValue(size_t index, size_t row, size_t col);

	// 清除指定位置的范围标记
	bool ClearRegion(size_t row, size_t col);
	// 设置范围标记
	bool SetRegionEnable(const char* name, size_t row, size_t col, bool value);
	// 获得范围标记
	bool GetRegionEnable(const char* name, size_t row, size_t col);

	// 改变范围名称（编辑器用）
	bool ChangeRegionName(const char* ori_name, const char* new_name);

	// 创建范围标记
	bool BuildRegion(const char* name, size_t rows, size_t cols, 
		const unsigned char* pdata, size_t size);
	bool BuildRegionCompress(const char* name, size_t rows, size_t cols, 
		unsigned char* pdata, size_t size);

	// 查找地区标记
	bool FindAreaIndex(const char* name, size_t& index) const;
	// 获得地区标记数量
	size_t GetAreaCount() const;
	// 获得地区标记名称
	const char* GetAreaName(size_t index) const;
	// 获得地区标记数据
	CWalkBitMap* GetAreaMap(size_t index) const;
	// 获得地区标记值
	bool GetAreaValue(size_t index, size_t row, size_t col);

	// 清除指定位置的地区标记
	bool ClearArea(size_t row, size_t col);
	// 设置地区标记
	bool SetAreaEnable(const char* name, size_t row, size_t col, bool value);
	// 获得地区标记
	bool GetAreaEnable(const char* name, size_t row, size_t col);
	// 获得有效地区标记
	const char* GetValidArea(size_t row, size_t col);

	// 改变地区名称（编辑器用）
	bool ChangeAreaName(const char* ori_name, const char* new_name);

	// 创建地区标记
	bool BuildArea(const char* name, size_t rows, size_t cols, 
		const unsigned char* pdata, size_t size);
	bool BuildAreaCompress(const char* name, size_t rows, size_t cols, 
		unsigned char* pdata, size_t size);

private:
	CTerrainRegion();
	CTerrainRegion(const CTerrainRegion&);
	CTerrainRegion& operator=(const CTerrainRegion&);

	void ReleaseAll();
	// 创建范围
	CWalkBitMap* CreateRegion(const char* name);
	// 创建区域
	CWalkBitMap* CreateArea(const char* name);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	TArrayPod<region_t, 1, TCoreAlloc> m_Regions;
	TArrayPod<area_t, 1, TCoreAlloc> m_Areas;
};

#endif // _TERRAIN_REGION_H
