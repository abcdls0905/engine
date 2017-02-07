//--------------------------------------------------------------------
// 文件名:		zone_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ZONE_MANAGER_H
#define _ZONE_MANAGER_H

#include "../public/i_interface.h"
#include "../public/macros.h"
#include "../public/core_mem.h"


#include "../visual/i_render.h"
#include "../utils/array_pod.h"
#include "../utils/tick_timer.h"
#include "terrain_zone.h"


class TZoneManagerAlloc
{
public:
	TZoneManagerAlloc() {}
	~TZoneManagerAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TZoneManagerAlloc& src) {}
};

// 地形区域管理
/*
class Terrain;
class CTerrainZone;
class CTerrainFactory;
*/
class CZoneManager
{
public:
	enum { RANGE_ZONE_MAX = 256 };

private:
	// 最大区域范围
	enum { MAX_RANGE_NUM = 7 };
	// 中心及周围区域索引数量
	enum { MAX_NEAR_ZONES = 13 * 13 };

	// 加载进度
	enum
	{
		LOAD_BEGIN = 1,		// 开始加载	
		LOAD_ALLHEIGHT = 2,	// 已加载所有高度
		LOAD_COMPLETE = 3,	// 加载完毕
	};

	// 相邻的区域
	enum ZONE_SIBLING
	{
		SIBLING_LEFT,
		SIBLING_RIGHT,
		SIBLING_UP,
		SIBLING_DOWN,
		SIBLING_LEFT_UP,
		SIBLING_RIGHT_UP,
		SIBLING_LEFT_DOWN,
		SIBLING_RIGHT_DOWN,
		SIBLING_MAX,
	};
	
	// 周围区域索引
	struct near_zone_t
	{
		int	nRow;	// 相对行数
		int	nCol;	// 相对列数
	};

public:
	CZoneManager();
	~CZoneManager();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	
	// 区域行列数量
	size_t GetRows() const { return m_nZoneRows; }
	size_t GetCols() const { return m_nZoneCols; }

	// 原点所在区域
	size_t GetOriginRow() const { return m_nZoneOriginRow; }
	size_t GetOriginCol() const { return m_nZoneOriginCol; }
	// 获得视野内可以看到的区域范围
	size_t GetZoneRange() const { return m_nZoneRange; }

	// 当前摄像机所在的区域行列号
	size_t GetCurrentRow() const { return m_nZoneCurrentRow; }
	size_t GetCurrentCol() const { return m_nZoneCurrentCol; }

	// 设置视野内可以看到的区域范围
	bool SetZoneRange(size_t value);
	// 设置所在的中心区域
	bool SetZoneCurrent(size_t row, size_t col);
	
	// 是否所有可见区域全部加载完成
	bool GetLoadFinish() const { return (m_nLoadProgress == LOAD_COMPLETE); }
	// 返回加载百分比
	float GetLoadPercent();
	
	// 初始化
	bool Initialize();

	// 释放所有地块的加载器
	void ReleaseAllZoneLoader();
	// 释放所有可视对象
	void ReleaseAllVisuals();
	// 释放所有地块的常量缓冲
	void ReleaseAllChunkCB();

	// 定时检查加载
	void CheckLoading();

	// 获得相邻的区域
	CTerrainZone* GetSiblingZoneByIndex(CTerrainZone* zone, size_t index);

	CTerrainZone* GetLeftZone(CTerrainZone* zone)
	{
		Assert(zone != NULL);

		int col = (int)zone->GetCol();

		if ((col - 1) >= 0)
		{
			return m_pZones[zone->GetIndex() - 1];
		}

		return NULL;
	}

	CTerrainZone* GetRightZone(CTerrainZone* zone)
	{
		Assert(zone != NULL);

		size_t col = zone->GetCol();

		if ((col + 1) < m_nZoneCols)
		{
			return m_pZones[zone->GetIndex() + 1];
		}

		return NULL;
	}

	CTerrainZone* GetUpZone(CTerrainZone* zone)
	{
		Assert(zone != NULL);

		int row = (int)zone->GetRow();

		if ((row - 1) >= 0)
		{
			return m_pZones[zone->GetIndex() - m_nZoneCols];
		}

		return NULL;
	}

	CTerrainZone* GetDownZone(CTerrainZone* zone)
	{
		Assert(zone != NULL);

		size_t row = zone->GetRow();

		if ((row + 1) < m_nZoneRows)
		{
			return m_pZones[zone->GetIndex() + m_nZoneCols];
		}

		return NULL;
	}

	CTerrainZone* GetLeftUpZone(CTerrainZone* zone)
	{
		CTerrainZone* pZone = GetLeftZone(zone);

		if (NULL == pZone)
		{
			return NULL;
		}

		return GetUpZone(pZone);
	}

	CTerrainZone* GetRightUpZone(CTerrainZone* zone)
	{
		CTerrainZone* pZone = GetRightZone(zone);

		if (NULL == pZone)
		{
			return NULL;
		}

		return GetUpZone(pZone);
	}

	CTerrainZone* GetLeftDownZone(CTerrainZone* zone)
	{
		CTerrainZone* pZone = GetLeftZone(zone);

		if (NULL == pZone)
		{
			return NULL;
		}

		return GetDownZone(pZone);
	}

	CTerrainZone* GetRightDownZone(CTerrainZone* zone)
	{
		CTerrainZone* pZone = GetRightZone(zone);

		if (NULL == pZone)
		{
			return NULL;
		}

		return GetDownZone(pZone);
	}
		
	// 获得区域最大数量
	size_t GetZoneNum() const { return m_nZoneNum; }
	
	// 获得指定位置区域
	CTerrainZone* GetZone(size_t row, size_t col)
	{
		if ((row >= m_nZoneRows) || (col >= m_nZoneCols))
		{
			return NULL;
		}

		return m_pZones[row * m_nZoneCols + col];
	}
	
	// 获得指定索引区域
	CTerrainZone* GetZoneByIndex(size_t index)
	{
		Assert(index < m_nZoneNum);

		return m_pZones[index];
	}

	// 获得指定位置在哪个区域内
	CTerrainZone* GetInZone(float x, float z)
	{
		int col = FloatToInt((x - m_fGlobalLeft) * m_fInvZoneSize);
		int row = FloatToInt((z - m_fGlobalTop) * m_fInvZoneSize);
		size_t zone_cols = m_nZoneCols;

		if ((size_t(row) >= m_nZoneRows) || (size_t(col) >= zone_cols))
		{
			return NULL;
		}

		return m_pZones[row * zone_cols + col];
	}

	// 获得指定坐标所在的区域位置
	bool GetZonePos(float x, float z, int& row, int& col);
	// 设置区域
	bool SetZones(size_t rows, size_t cols);
	// 设置起始区域（0，0坐标所在）
	bool SetZoneOrigin(size_t row, size_t col);
	// 创建区域
	CTerrainZone* CreateZone(size_t row, size_t col);

	// 获得当前可见范围内的排序后的所有区域
	size_t GetRangeZoneList(CTerrainZone** zones, size_t max_num, 
		size_t zone_range);

	// 导出内存中的区域列表
	bool DumpZones(const char* file_name);

private:
	CZoneManager(const CZoneManager&);
	CZoneManager& operator=(const CZoneManager&);

	void ReleaseAll();

	// 连接高度图
	bool LoadConnectHeight(size_t range);

	// 异步加载
	bool AsyncLoadHeight(size_t range);
	bool AsyncLoadLight(size_t range);
	bool AsyncLoadAlpha(size_t range);
	bool AsyncLoadVisual(size_t range);
	bool AsyncLoadWalk(size_t range);
	bool AsyncLoadRegion(size_t range);
#ifdef PHYSICS_HAVOK_TERRAIN
	bool AsyncLoadRigid(size_t range);
#endif // PHYSICS_HAVOK_TERRAIN
	
	// 是否已加载所有高度
	bool GetLoadAllHeight(size_t range);
	// 是否已全部加载完成
	bool GetLoadComplete(size_t range);

	// 检查加载碰撞信息
	bool CheckLoadCollide(size_t range);
	// 检查可以卸载的区域
	bool CheckUnload(float seconds, size_t range);

private:
	Terrain* m_pTerrain;
	CTerrainFactory* m_pFactory;
	CTerrainZone** m_pZones;	// 地形区域
	size_t m_nZoneRows;			// 区域行数
	size_t m_nZoneCols;			// 区域列数
	size_t m_nZoneNum;			// 总数量
	size_t m_nZoneOriginRow;	// 中心区域的行列
	size_t m_nZoneOriginCol;
	size_t m_nZoneCurrentRow;	// 当前所在区域的行列
	size_t m_nZoneCurrentCol;	
	size_t m_nZoneRange;		// 可见区域的层次数(1-3x3,2-5x5,...)
	size_t m_nRangeZones[MAX_RANGE_NUM];
	near_zone_t m_NearZones[MAX_NEAR_ZONES];
	TArrayPod<CTerrainZone*, 64, TZoneManagerAlloc> m_ExistZones;
	float m_fGlobalLeft;
	float m_fGlobalTop;
	float m_fZoneWidth;
	float m_fZoneHeight;
	float m_fInvZoneSize;
	int m_nLoadProgress;
	CTickTimer m_UnloadTimer;
};

#endif // _ZONE_MANAGER_H
