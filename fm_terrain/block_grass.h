//--------------------------------------------------------------------
// 文件名:		block_grass.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_GRASS_H
#define _BLOCK_GRASS_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "visual_loader.h"

// 读取花草信息 

class CBlockGrassReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CVisualLoader::grass_all_t* p);
	
public:
	CBlockGrassReader(CVisualLoader::grass_all_t* p);
	virtual ~CBlockGrassReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockGrassReader();

private:
	CVisualLoader::grass_all_t* m_pData;
};

// 写入花草信息 

class CTerrainZone;

class CBlockGrassWriter: public IBlockWriter
{
public:
	CBlockGrassWriter(CTerrainZone* pZone);
	virtual ~CBlockGrassWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockGrassWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_GRASS_H

