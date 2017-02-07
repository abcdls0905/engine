//--------------------------------------------------------------------
// 文件名:		block_water.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_WATER_H
#define _BLOCK_WATER_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// 读取水面信息 

class CBlockWaterReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::water_all_t* p, 
		size_t chunk_rows, size_t chunk_cols);
	
public:
	CBlockWaterReader(CHeightLoader::water_all_t* p, size_t chunk_rows, 
		size_t chunk_cols);
	virtual ~CBlockWaterReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockWaterReader();

private:
	CHeightLoader::water_all_t*	m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
};

// 写入水面信息 

class CTerrainZone;

class CBlockWaterWriter: public IBlockWriter
{
public:
	CBlockWaterWriter(CTerrainZone* pZone);
	virtual ~CBlockWaterWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockWaterWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_WATER_H
