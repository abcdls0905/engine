//--------------------------------------------------------------------
// 文件名:		block_height2.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_HEIGHT2_H
#define _BLOCK_HEIGHT2_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// 读取压缩高度信息 

class CBlockHeight2Reader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::height_all_t* p,
		size_t rows, size_t cols);
	
public:
	CBlockHeight2Reader(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeight2Reader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockHeight2Reader();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// 写入压缩高度信息 

class CTerrainZone;

class CBlockHeight2Writer: public IBlockWriter
{
public:
	CBlockHeight2Writer(CTerrainZone* pZone);
	virtual ~CBlockHeight2Writer();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockHeight2Writer();
	
private:
	CTerrainZone* m_pZone;
};

// 直接写入压缩高度信息 

class CBlockHeight2Writer2: public IBlockWriter
{
public:
	CBlockHeight2Writer2(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeight2Writer2();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);

private:
	CBlockHeight2Writer2();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

#endif // _BLOCK_HEIGHT2_H
