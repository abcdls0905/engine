//--------------------------------------------------------------------
// �ļ���:		terrain_walkable.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��8��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_WALKABLE_H
#define _TERRAIN_WALKABLE_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"

// ������߲���
#define MAX_FLOOR_NUM 15
// ��Ч�߶�ֵ
#define HEIGHT_NULL 1e8

// ����
#define FLOOR_MARKER_EXISTS 0x1
#define FLOOR_MARKER_MOVE	0x2
#define FLOOR_MARKER_STAND	0x4
#define FLOOR_MARKER_WALL	0x8

// �߶�ֵ�Ƿ���Ч
inline bool height_is_null(float height)
{
	return (height > 1e7);
}

// ���εĿ�������Ϣ

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

	// �����������
	void ReleaseAll();

	// ������
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

	// �����Ƿ����
	void SetReady(bool value) { m_bReady = value; }
	bool GetReady() const { return m_bReady; }

	// ���ˮ��߶�
	float GetWaterHeight(size_t row, size_t col);
	// ��������߲���
	unsigned int GetWalkMarker(size_t row, size_t col);
	// ����ܵ����߲�����
	int GetFloorCount()	{ return m_nFloorCount + ((m_nDynamicFloorCount > 1) ? (m_nDynamicFloorCount - 1) : m_nDynamicFloorCount); }
	// ������߲���
	unsigned int GetFloorMarker(size_t row, size_t col, int floor);
	// ������߲�߶�
	float GetFloorHeight(size_t row, size_t col, int floor);
	// ������߲�����ϰ��߶�
	float GetFloorSpace(size_t row, size_t col, int floor);
	// ���ָ���߶�λ���Ƿ����ǽ��
	bool GetWallExists(size_t row, size_t col, float y);
	// ������߲�ID
	IVisBase* GetFloorObject(size_t row, size_t col, int floor);

	// ��ö�̬�����߲�����
	int GetStaticFloorCount()	{ return m_nFloorCount; }
	// ��ö�̬�����߲�����
	int GetDynamicFloorCount()	{ return m_nDynamicFloorCount; }

	int GetApexStaticFloor(size_t row, size_t col);
	int GetApexDynamicFloor(size_t row, size_t col);
	// ������߾�̬����
	unsigned int GetStaticFloorMarker(size_t row, size_t col, int floor);
	// ������߾�̬��߶�
	float GetStaticFloorHeight(size_t row, size_t col, int floor);
	// ������߾�̬������ϰ��߶�
	float GetStaticFloorSpace(size_t row, size_t col, int floor);

	// ������߶�̬����
	unsigned int GetDynamicFloorMarker(size_t row, size_t col, int floor);
	// ������߶�̬��߶�
	float GetDynamicFloorHeight(size_t row, size_t col, int floor);
	// ������߶�̬������ϰ��߶�
	float GetDynamicFloorSpace(size_t row, size_t col, int floor);

	// ������������ͼ
	bool SaveWalkMap(const char* file_name);

	// ���¸߶�����
	bool UpdateGroundHeight(size_t rows, size_t cols, float* pValues);
	bool UpdateWaterHeight(size_t rows, size_t cols, float* pValues);
	bool UpdateFloorHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	bool UpdateSpaceHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	// ���±������
	bool UpdateWalkMarker(size_t rows, size_t cols, 
		unsigned char* pValues);
	bool UpdateFloorMarker(int floor, size_t rows, size_t cols, 
		unsigned char* pValues);

	// ���¸߶�����
	bool UpdateDynamicFloorHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	bool UpdateDynamicSpaceHeight(int floor, size_t rows, size_t cols, 
		float* pValues);
	// ���±������
	bool UpdateDynamicFloorMarker(int floor, size_t rows, size_t cols, 
		unsigned char* pValues);
	// ����ID����
	bool UpdateDynamicFloorID(int floor, size_t rows, size_t cols, 
		PERSISTID* pValues);

	// ����
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

	// ����߶�ͼ
	bool SaveHeightMap(CWalkHeightMap* pHeight, const char* file_name);
	// ������ͼ
	bool SaveFlagMap(CWalkFlagMap* pFlagMap, const char* file_name);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nFloorCount;
	size_t m_nDynamicFloorCount;
	bool m_bReady;

	CWalkHeightMap* m_pGroundHeight;				// �ر�߶�
	CWalkHeightMap* m_pWaterHeight;					// ˮ��߶�
	CWalkHeightMap* m_pFloorHeights[MAX_FLOOR_NUM];	// ��߶�
	CWalkHeightMap* m_pSpaceHeights[MAX_FLOOR_NUM];	// ���ϰ��ռ�߶�
	CWalkFlagMap* m_pWalkMarker;					// �����߱��
	CWalkFlagMap* m_pFloorMarkers[MAX_FLOOR_NUM];	// ���߲���

	CWalkHeightMap* m_pDynamicFloorHeights[MAX_FLOOR_NUM];	// ��̬��߶�
	CWalkHeightMap* m_pDynamicSpaceHeights[MAX_FLOOR_NUM];	// ��̬�����ϰ��ռ�߶�
	CWalkFlagMap* m_pDynamicFloorMarkers[MAX_FLOOR_NUM];	// ��̬���߲���
	CWalkIDMap* m_pDynamicFloorIDs[MAX_FLOOR_NUM];			// ���߲�ID
};

#endif // _TERRAIN_WALKABLE_H
