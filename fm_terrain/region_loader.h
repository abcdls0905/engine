//--------------------------------------------------------------------
// �ļ���:		region_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��23��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _REGION_LOADER_H
#define _REGION_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"


#include "../visual/i_res_loader.h"
#include "terrain_region.h"

// �����Ϣ������

class CTerrainZone;
class IBlockReader;

class CRegionLoader: public IResLoader
{
public:
	static CRegionLoader* NewInstance();
	static IBlockReader* CreateBlockReader(int block_id, void* context);

	struct region_data_t
	{
		char* pName;
		unsigned char* pData;
		size_t nSize;
	};

	typedef TArrayPod<region_data_t, 1, TCoreAlloc> region_all_t;

	struct area_data_t
	{
		char* pName;
		unsigned char* pData;
		size_t nSize;
	};

	typedef TArrayPod<area_data_t, 1, TCoreAlloc> area_all_t;

public:
	CRegionLoader();
	virtual ~CRegionLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);
	
	// ��Դ����
	void SetOwner(CTerrainZone* pOwner);
	// ��Χ����ļ���
	void SetFileName(const char* file_name);
	// ��������ļ���
	void SetAreaFile(const char* file_name);
	
	// ���淶Χ�����Ϣ�ļ�
	static bool SaveBlockRegion(const char* file_name, 
		CTerrainZone* pZone);
	// ������������Ϣ�ļ�
	static bool SaveBlockArea(const char* file_name, 
		CTerrainZone* pZone);
	
private:
	CRegionLoader(const CRegionLoader&);
	CRegionLoader& operator=(const CRegionLoader&);
	
	// ���ط�Χ�����Ϣ
	bool LoadBlockRegion(const char* file_name);
	// ���ص��������Ϣ
	bool LoadBlockArea(const char* file_name);

private:
	CTerrainZone* m_pOwner;
	core_string m_strFileName;
	core_string m_strAreaFile;
	size_t m_nRows;
	size_t m_nCols;
	region_all_t m_Region;
	area_all_t m_Area;
};

#endif // _REGION_LOADER_H
