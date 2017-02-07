//--------------------------------------------------------------------
// �ļ���:		zone_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TZoneManagerAlloc& src) {}
};

// �����������
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
	// �������Χ
	enum { MAX_RANGE_NUM = 7 };
	// ���ļ���Χ������������
	enum { MAX_NEAR_ZONES = 13 * 13 };

	// ���ؽ���
	enum
	{
		LOAD_BEGIN = 1,		// ��ʼ����	
		LOAD_ALLHEIGHT = 2,	// �Ѽ������и߶�
		LOAD_COMPLETE = 3,	// �������
	};

	// ���ڵ�����
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
	
	// ��Χ��������
	struct near_zone_t
	{
		int	nRow;	// �������
		int	nCol;	// �������
	};

public:
	CZoneManager();
	~CZoneManager();

	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	
	// ������������
	size_t GetRows() const { return m_nZoneRows; }
	size_t GetCols() const { return m_nZoneCols; }

	// ԭ����������
	size_t GetOriginRow() const { return m_nZoneOriginRow; }
	size_t GetOriginCol() const { return m_nZoneOriginCol; }
	// �����Ұ�ڿ��Կ���������Χ
	size_t GetZoneRange() const { return m_nZoneRange; }

	// ��ǰ��������ڵ��������к�
	size_t GetCurrentRow() const { return m_nZoneCurrentRow; }
	size_t GetCurrentCol() const { return m_nZoneCurrentCol; }

	// ������Ұ�ڿ��Կ���������Χ
	bool SetZoneRange(size_t value);
	// �������ڵ���������
	bool SetZoneCurrent(size_t row, size_t col);
	
	// �Ƿ����пɼ�����ȫ���������
	bool GetLoadFinish() const { return (m_nLoadProgress == LOAD_COMPLETE); }
	// ���ؼ��ذٷֱ�
	float GetLoadPercent();
	
	// ��ʼ��
	bool Initialize();

	// �ͷ����еؿ�ļ�����
	void ReleaseAllZoneLoader();
	// �ͷ����п��Ӷ���
	void ReleaseAllVisuals();
	// �ͷ����еؿ�ĳ�������
	void ReleaseAllChunkCB();

	// ��ʱ������
	void CheckLoading();

	// ������ڵ�����
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
		
	// ��������������
	size_t GetZoneNum() const { return m_nZoneNum; }
	
	// ���ָ��λ������
	CTerrainZone* GetZone(size_t row, size_t col)
	{
		if ((row >= m_nZoneRows) || (col >= m_nZoneCols))
		{
			return NULL;
		}

		return m_pZones[row * m_nZoneCols + col];
	}
	
	// ���ָ����������
	CTerrainZone* GetZoneByIndex(size_t index)
	{
		Assert(index < m_nZoneNum);

		return m_pZones[index];
	}

	// ���ָ��λ�����ĸ�������
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

	// ���ָ���������ڵ�����λ��
	bool GetZonePos(float x, float z, int& row, int& col);
	// ��������
	bool SetZones(size_t rows, size_t cols);
	// ������ʼ����0��0�������ڣ�
	bool SetZoneOrigin(size_t row, size_t col);
	// ��������
	CTerrainZone* CreateZone(size_t row, size_t col);

	// ��õ�ǰ�ɼ���Χ�ڵ���������������
	size_t GetRangeZoneList(CTerrainZone** zones, size_t max_num, 
		size_t zone_range);

	// �����ڴ��е������б�
	bool DumpZones(const char* file_name);

private:
	CZoneManager(const CZoneManager&);
	CZoneManager& operator=(const CZoneManager&);

	void ReleaseAll();

	// ���Ӹ߶�ͼ
	bool LoadConnectHeight(size_t range);

	// �첽����
	bool AsyncLoadHeight(size_t range);
	bool AsyncLoadLight(size_t range);
	bool AsyncLoadAlpha(size_t range);
	bool AsyncLoadVisual(size_t range);
	bool AsyncLoadWalk(size_t range);
	bool AsyncLoadRegion(size_t range);
#ifdef PHYSICS_HAVOK_TERRAIN
	bool AsyncLoadRigid(size_t range);
#endif // PHYSICS_HAVOK_TERRAIN
	
	// �Ƿ��Ѽ������и߶�
	bool GetLoadAllHeight(size_t range);
	// �Ƿ���ȫ���������
	bool GetLoadComplete(size_t range);

	// ��������ײ��Ϣ
	bool CheckLoadCollide(size_t range);
	// ������ж�ص�����
	bool CheckUnload(float seconds, size_t range);

private:
	Terrain* m_pTerrain;
	CTerrainFactory* m_pFactory;
	CTerrainZone** m_pZones;	// ��������
	size_t m_nZoneRows;			// ��������
	size_t m_nZoneCols;			// ��������
	size_t m_nZoneNum;			// ������
	size_t m_nZoneOriginRow;	// �������������
	size_t m_nZoneOriginCol;
	size_t m_nZoneCurrentRow;	// ��ǰ�������������
	size_t m_nZoneCurrentCol;	
	size_t m_nZoneRange;		// �ɼ�����Ĳ����(1-3x3,2-5x5,...)
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
