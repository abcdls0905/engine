//--------------------------------------------------------------------
// 文件名:		block_walk.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_WALK_H
#define _BLOCK_WALK_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "walk_loader.h"

// 读取行走信息 

class CBlockWalkReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CWalkLoader::walk_all_t* p, size_t rows, 
		size_t cols);
	
public:
	CBlockWalkReader(CWalkLoader::walk_all_t* p, size_t rows, size_t cols);
	virtual ~CBlockWalkReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockWalkReader();

private:
	CWalkLoader::walk_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// 写入行走信息 

class CTerrainZone;

class CBlockWalkWriter: public IBlockWriter
{
public:
	CBlockWalkWriter(CTerrainZone* pZone);
	virtual ~CBlockWalkWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockWalkWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_WALK_H
