//--------------------------------------------------------------------
// 文件名:		block_base.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_BASE_H
#define _BLOCK_BASE_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "alpha_loader.h"

// 读取基础贴图信息 

class CBlockBaseReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CAlphaLoader::base_all_t* p, 
		size_t chunk_rows, size_t chunk_cols);
	
public:
	CBlockBaseReader(CAlphaLoader::base_all_t* p, size_t chunk_rows, 
		size_t chunk_cols);
	virtual ~CBlockBaseReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockBaseReader();

private:
	CAlphaLoader::base_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
};

// 写入基础贴图信息 

class CTerrainZone;

class CBlockBaseWriter: public IBlockWriter
{
public:
	CBlockBaseWriter(CTerrainZone* pZone);
	virtual ~CBlockBaseWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockBaseWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_BASE_H

