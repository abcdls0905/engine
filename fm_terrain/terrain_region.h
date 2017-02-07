//--------------------------------------------------------------------
// �ļ���:		terrain_region.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��23��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_REGION_H
#define _TERRAIN_REGION_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../utils/array_pod.h"



// ��Χ���

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

	// ������
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// ���ҷ�Χ���
	bool FindRegionIndex(const char* name, size_t& index) const;
	// ��÷�Χ�������
	size_t GetRegionCount() const;
	// ��÷�Χ�������
	const char* GetRegionName(size_t index) const;
	// ��÷�Χ�������
	CWalkBitMap* GetRegionMap(size_t index) const;
	// ��÷�Χ���ֵ
	bool GetRegionValue(size_t index, size_t row, size_t col);

	// ���ָ��λ�õķ�Χ���
	bool ClearRegion(size_t row, size_t col);
	// ���÷�Χ���
	bool SetRegionEnable(const char* name, size_t row, size_t col, bool value);
	// ��÷�Χ���
	bool GetRegionEnable(const char* name, size_t row, size_t col);

	// �ı䷶Χ���ƣ��༭���ã�
	bool ChangeRegionName(const char* ori_name, const char* new_name);

	// ������Χ���
	bool BuildRegion(const char* name, size_t rows, size_t cols, 
		const unsigned char* pdata, size_t size);
	bool BuildRegionCompress(const char* name, size_t rows, size_t cols, 
		unsigned char* pdata, size_t size);

	// ���ҵ������
	bool FindAreaIndex(const char* name, size_t& index) const;
	// ��õ����������
	size_t GetAreaCount() const;
	// ��õ����������
	const char* GetAreaName(size_t index) const;
	// ��õ����������
	CWalkBitMap* GetAreaMap(size_t index) const;
	// ��õ������ֵ
	bool GetAreaValue(size_t index, size_t row, size_t col);

	// ���ָ��λ�õĵ������
	bool ClearArea(size_t row, size_t col);
	// ���õ������
	bool SetAreaEnable(const char* name, size_t row, size_t col, bool value);
	// ��õ������
	bool GetAreaEnable(const char* name, size_t row, size_t col);
	// �����Ч�������
	const char* GetValidArea(size_t row, size_t col);

	// �ı�������ƣ��༭���ã�
	bool ChangeAreaName(const char* ori_name, const char* new_name);

	// �����������
	bool BuildArea(const char* name, size_t rows, size_t cols, 
		const unsigned char* pdata, size_t size);
	bool BuildAreaCompress(const char* name, size_t rows, size_t cols, 
		unsigned char* pdata, size_t size);

private:
	CTerrainRegion();
	CTerrainRegion(const CTerrainRegion&);
	CTerrainRegion& operator=(const CTerrainRegion&);

	void ReleaseAll();
	// ������Χ
	CWalkBitMap* CreateRegion(const char* name);
	// ��������
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
