//--------------------------------------------------------------------
// 文件名:		BlockDynamicWalk.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月11日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCKDYNAMICWALK_H
#define _BLOCKDYNAMICWALK_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "dynamic_walk_loader.h"

// CBlockDynamicWalkReader
// 读取行走信息 

class CBlockDynamicWalkReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CDynamicWalkLoader::walk_all_t* p, size_t rows, 
		size_t cols);

public:
	CBlockDynamicWalkReader(CDynamicWalkLoader::walk_all_t* p, size_t rows, size_t cols);
	virtual ~CBlockDynamicWalkReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockDynamicWalkReader();

private:
	CDynamicWalkLoader::walk_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// CBlockWalkWriter
// 写入行走信息 

class CTerrainZone;

class CBlockDynamicWalkWriter: public IBlockWriter
{
public:
	CBlockDynamicWalkWriter(CDynamicWalkable* pWalk);
	virtual ~CBlockDynamicWalkWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);

private:
	CBlockDynamicWalkWriter();

private:
	CDynamicWalkable* m_pWalk;
};

#endif // _BLOCKDYNAMICWALK_H
