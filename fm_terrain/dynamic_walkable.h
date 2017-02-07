//--------------------------------------------------------------------
// �ļ���:		DynamicWalkable.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��2��28��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKABLE_H
#define _DYNAMICWALKABLE_H

//#include "TerrainWalkable.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

#define MAX_FLOOR_NUM 15
// ��Ч�߶�ֵ
#define HEIGHT_NULL 1e8

// ����
#define FLOOR_MARKER_EXISTS 0x1
#define FLOOR_MARKER_MOVE	0x2
#define FLOOR_MARKER_STAND	0x4
#define FLOOR_MARKER_WALL	0x8
#define FLOOR_MARKER_EDGE	0x10

// �߶�ֵ�Ƿ���Ч
// inline bool height_is_null(float height)
// {
// 	return (height > 1e7);
// }

// ��̬�����������Ϣ

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
	
	// �����������
	void ReleaseAll();

	// �ļ���
	const char* GetFileName() const { return m_strFileName.c_str(); }
	const char* GetModelFileName() const { return m_strModelFileName.c_str(); }

	result_string GetWalkFileName() const;

	// ������
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	CWalkHeightMap* GetFloorHeightMap(size_t i) { return m_pFloorHeights[i]; }
	CWalkHeightMap* GetSpaceHeightMap(size_t i) { return m_pSpaceHeights[i]; }
	CWalkFlagMap* GetFloorMarkerMap(size_t i) { return m_pFloorMarkers[i]; }

	// �����Ƿ����
	void SetReady(bool value) { m_bReady = value; }
	bool GetReady() const { return m_bReady; }

	// ����ܵ����߲�����
	size_t GetFloorCount() const { return m_nFloorCount; }
	// ���ָ��λ�õ����߶�
	float GetApexHeight(float height, size_t row, size_t col) const;
	// ���ָ��λ�õ����߶����ڵĲ�
	int GetApexFloor(size_t row, size_t col);
	// ���ָ��λ�õ���С�߶�
	float GetBottomHeight(float height, size_t row, size_t col) const;
	// ���ָ��λ�õ���С�߶����ڵĲ�
	int GetBottomFloor(size_t row, size_t col);
	// ������߲�߶�
	float GetFloorHeight(float height, size_t row, size_t col, int floor) const;
	// ������߲�����ϰ��߶�
	float GetFloorSpace(size_t row, size_t col, int floor) const;
	// ������߲���
	unsigned int GetFloorMarker(size_t row, size_t col, int floor) const;
	// ���ָ���߶�λ���Ƿ����ǽ��
	bool GetWallExists(float height, size_t row, size_t col, float y) const;

	// ���¸߶�����
	bool UpdateFloorHeight(int floor, size_t rows, size_t cols,
		float* pValues);
	bool UpdateSpaceHeight(int floor, size_t rows, size_t cols,
		float* pValues);
	// ���±������
	bool UpdateFloorMarker(int floor, size_t rows, size_t cols,
		unsigned char* pValues);

	// ����
	bool BuildFloorHeight(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);
	bool BuildSpaceHeight(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);
	bool BuildFloorMarker(size_t rows, size_t cols, unsigned char* pData,
		size_t size, int floor);

	// ������ײ����
	bool Load();
	// ж����ײ����
	bool Unload();

	// ��Ӷ�̬���
	bool AddDynamicWalkable(IVisBase* pVisBase);
	// ɾ����̬���
	bool DeleteDynamicWalkable(IVisBase* pVisBase);

	bool CanInsert(IVisBase* pVisBase);
	bool InsertDynamicWalkable(IVisBase* pVisBase);
	bool RemoveDynamicWalkable(IVisBase* pVisBase);

	bool IsLoaded() const { return m_bLoaded; }

	// ��̬�����
	size_t GetCount() const { return m_DynamicVisualArr.size(); }

	// ��̬����Ƕ�
	float GetAngleY() const { return m_fAngleY; }
	void SetAngleY(float value) { m_fAngleY = value; }

	float GetHeight() const { return m_fHeight; }
	void SetHeight(float value) { m_fHeight = value; }

	// ����Բ�뾶
	float GetRadius() const { return m_fRadius; }
	void SetRadius(float value) { m_fRadius = value; }

	// ��Χ�гߴ�
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

    // ��̬��ײ���ݼ�����
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

	CWalkHeightMap* m_pFloorHeights[MAX_FLOOR_NUM];		// ��߶�
	CWalkHeightMap* m_pSpaceHeights[MAX_FLOOR_NUM];		// ���ϰ��ռ�߶�
	//CWalkFlagMap* m_pWalkMarker;						// �����߱��
	CWalkFlagMap* m_pFloorMarkers[MAX_FLOOR_NUM];		// ���߲���

	TArrayPod<entry_point_t, 1, TCoreAlloc> m_EntryPoints[MAX_FLOOR_NUM];

	TArrayPod<PERSISTID, 1, TCoreAlloc> m_DynamicVisualArr;	// ��¼��̬���
};
#endif // _DYNAMICWALKABLE_H
