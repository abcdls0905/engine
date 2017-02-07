//--------------------------------------------------------------------
// 文件名:		block_region.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_REGION_H
#define _BLOCK_REGION_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "region_loader.h"

// 读取范围标记信息 

class CBlockRegionReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CRegionLoader::region_all_t* p, 
		size_t rows, size_t cols);

public:
	CBlockRegionReader(CRegionLoader::region_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockRegionReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockRegionReader();

private:
	CRegionLoader::region_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// 写入范围标记信息 

class CTerrainZone;

class CBlockRegionWriter: public IBlockWriter
{
public:
	CBlockRegionWriter(CTerrainZone* pZone);
	virtual ~CBlockRegionWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);

private:
	CBlockRegionWriter();

private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_REGION_H
