//--------------------------------------------------------------------
// 文件名:		block_area.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月2日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_AREA_H
#define _BLOCK_AREA_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "region_loader.h"

// 读取地区标记信息 

class CBlockAreaReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CRegionLoader::area_all_t* p, 
		size_t rows, size_t cols);
	
public:
	CBlockAreaReader(CRegionLoader::area_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockAreaReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockAreaReader();

private:
	CRegionLoader::area_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// 写入地区标记信息 

class CTerrainZone;

class CBlockAreaWriter: public IBlockWriter
{
public:
	CBlockAreaWriter(CTerrainZone* pZone);
	virtual ~CBlockAreaWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockAreaWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_AREA_H

