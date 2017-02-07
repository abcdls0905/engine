//--------------------------------------------------------------------
// 文件名:		block_height.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_HEIGHT_H
#define _BLOCK_HEIGHT_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// 读取高度信息 

class CBlockHeightReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::height_all_t* p,
		size_t rows, size_t cols);
	
public:
	CBlockHeightReader(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeightReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockHeightReader();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// 写入高度信息 

class CTerrainZone;

class CBlockHeightWriter: public IBlockWriter
{
public:
	CBlockHeightWriter(CTerrainZone* pZone);
	virtual ~CBlockHeightWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockHeightWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_HEIGHT_H
