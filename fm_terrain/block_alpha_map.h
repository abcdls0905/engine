//--------------------------------------------------------------------
// 文件名:		block_alpha_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_ALPHA_MAP_H
#define _BLOCK_ALPHA_MAP_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "alpha_loader.h"

// 读取混合贴图信息 

class CBlockAlphaMapReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CAlphaLoader::map_all_t* p, 
		size_t chunk_rows, size_t chunk_cols, size_t alpha_map_scale);
	
public:
	CBlockAlphaMapReader(CAlphaLoader::map_all_t* p, size_t chunk_rows, 
		size_t chunk_cols, size_t alpha_map_scale);
	virtual ~CBlockAlphaMapReader();

	// 释放
	virtual void Release();
	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Read(void* handle);

private:
	CBlockAlphaMapReader();

private:
	CAlphaLoader::map_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	size_t m_nAlphaMapScale;
};

// 写入混合贴图信息 

class CTerrainZone;

class CBlockAlphaMapWriter: public IBlockWriter
{
public:
	CBlockAlphaMapWriter(CTerrainZone* pZone);
	virtual ~CBlockAlphaMapWriter();

	// 块尺寸
	virtual int GetSize() const;
	// 读取
	virtual int Write(void* handle);
	
private:
	CBlockAlphaMapWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_ALPHA_MAP_H
