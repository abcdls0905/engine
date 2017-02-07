//--------------------------------------------------------------------
// 文件名:		region_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _REGION_LOADER_H
#define _REGION_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"


#include "../visual/i_res_loader.h"
#include "terrain_region.h"

// 标记信息加载器

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
	// 范围标记文件名
	void SetFileName(const char* file_name);
	// 地区标记文件名
	void SetAreaFile(const char* file_name);
	
	// 保存范围标记信息文件
	static bool SaveBlockRegion(const char* file_name, 
		CTerrainZone* pZone);
	// 保存地区标记信息文件
	static bool SaveBlockArea(const char* file_name, 
		CTerrainZone* pZone);
	
private:
	CRegionLoader(const CRegionLoader&);
	CRegionLoader& operator=(const CRegionLoader&);
	
	// 加载范围标记信息
	bool LoadBlockRegion(const char* file_name);
	// 加载地区标记信息
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
