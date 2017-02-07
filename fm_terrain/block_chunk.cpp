//--------------------------------------------------------------------
// 文件名:		block_chunk.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_chunk.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 分块信息文件头
struct chunk_file_header_t
{
	unsigned int nChunkRows;	// 分块行数
	unsigned int nChunkCols;	// 分块列数
};

#pragma pack(pop)

// CBlockChunkReader

IBlockReader* CBlockChunkReader::NewInstance(CHeightLoader::chunk_all_t* p,
	size_t chunk_rows, size_t chunk_cols, float left, float top)
{
	CBlockChunkReader* ptr = (CBlockChunkReader*)CORE_ALLOC(
		sizeof(CBlockChunkReader));

	new (ptr) CBlockChunkReader(p, chunk_rows, chunk_cols, left, top);

	return ptr;
}

CBlockChunkReader::CBlockChunkReader(CHeightLoader::chunk_all_t* p,
	size_t chunk_rows, size_t chunk_cols, float left, float top)
{
	Assert(p != NULL);

	m_pData = p;
	m_nChunkRows = chunk_rows;
	m_nChunkCols = chunk_cols;
	m_fLeft = left;
	m_fTop = top;
}

CBlockChunkReader::~CBlockChunkReader()
{
}

void CBlockChunkReader::Release()
{
	CORE_DELETE(this);
}

int CBlockChunkReader::GetSize() const
{
	return 0;
}
	
int CBlockChunkReader::Read(void* handle)
{
	FILE* fp = (FILE*)handle;
	chunk_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockChunkReader::Read)read header failed");
		return -1;
	}
	
	if ((header.nChunkRows != m_nChunkRows)
		|| (header.nChunkCols != m_nChunkCols))
	{
		CORE_TRACE("(CBlockChunkReader::Read)scale error");
		return -1;
	}
	
	size_t chunk_num = header.nChunkRows * header.nChunkCols;
	CHeightLoader::chunk_info_t* chunks = (CHeightLoader::chunk_info_t*)
		CORE_ALLOC(sizeof(CHeightLoader::chunk_info_t) * chunk_num);
	
	if (core_file::fread(chunks, sizeof(CHeightLoader::chunk_info_t), 
		chunk_num, fp) != chunk_num)
	{
		CORE_TRACE("(CBlockChunkReader::Read)read data failed");
		CORE_FREE(chunks, sizeof(CHeightLoader::chunk_info_t) * chunk_num);
		return -1;
	}
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CHeightLoader::chunk_info_t* chunk = chunks + i;
		
		chunk->vMin.x += m_fLeft;
		chunk->vMin.z += m_fTop;
		chunk->vMax.x += m_fLeft;
		chunk->vMax.z += m_fTop;
	}
	
	m_pData->pChunks = chunks;
	
	return sizeof(header) + (int)chunk_num * sizeof(CHeightLoader::chunk_info_t);
}

// CBlockChunkWriter

CBlockChunkWriter::CBlockChunkWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockChunkWriter::~CBlockChunkWriter()
{
}

int CBlockChunkWriter::GetSize() const
{
	return 0;
}

int CBlockChunkWriter::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	size_t chunk_rows = m_pZone->GetChunkDims();
	size_t chunk_cols = m_pZone->GetChunkDims();
	chunk_file_header_t header;
	
	header.nChunkRows = (unsigned int)chunk_rows;
	header.nChunkCols = (unsigned int)chunk_cols;
	
	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockChunkWriter::Write)write header failed");
		return -1;
	}
	
	size_t chunk_num = chunk_rows * chunk_cols;
	CHeightLoader::chunk_info_t* chunks = (CHeightLoader::chunk_info_t*)
		CORE_ALLOC(sizeof(CHeightLoader::chunk_info_t) * chunk_num);
	
	memset(chunks, 0, sizeof(CHeightLoader::chunk_info_t) * chunk_num);
	
	size_t lod_levels = m_pZone->GetTerrain()->GetLodLevels();
	
	for (size_t r = 0; r < chunk_rows; ++r)
	{
		for (size_t c = 0; c < chunk_cols; ++c)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunk(r, c);
			CHeightLoader::chunk_info_t* info = chunks + r * chunk_cols + c;
			
			info->vMin = pChunk->GetMin();
			info->vMax = pChunk->GetMax();
			// 相对位置
			info->vMin.x -= m_pZone->GetLeft();
			info->vMin.z -= m_pZone->GetTop();
			info->vMax.x -= m_pZone->GetLeft();
			info->vMax.z -= m_pZone->GetTop();
			
			memcpy(info->fErrors, pChunk->GetErrors(),
				sizeof(float) * lod_levels);
		}
	}
	
	if (core_file::fwrite(chunks, sizeof(CHeightLoader::chunk_info_t), 
		chunk_num, fp) != chunk_num)
	{
		CORE_TRACE("(CBlockChunkWriter::Write)write values failed");
		CORE_FREE(chunks, sizeof(CHeightLoader::chunk_info_t) * chunk_num);
		return -1;
	}
	
	CORE_FREE(chunks, sizeof(CHeightLoader::chunk_info_t) * chunk_num);
	
	return sizeof(header) + (int)chunk_num * sizeof(CHeightLoader::chunk_info_t);
}

// CBlockChunkWriter2

CBlockChunkWriter2::CBlockChunkWriter2(CHeightLoader::chunk_all_t* p,
	size_t chunk_rows, size_t chunk_cols, float left, float top)
{
	Assert(p != NULL);

	m_pData = p;
	m_nChunkRows = chunk_rows;
	m_nChunkCols = chunk_cols;
	m_fLeft = left;
	m_fTop = top;
}

CBlockChunkWriter2::~CBlockChunkWriter2()
{
}

int CBlockChunkWriter2::GetSize() const
{
	return 0;
}

int CBlockChunkWriter2::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	chunk_file_header_t header;

	header.nChunkRows = (unsigned int)m_nChunkRows;
	header.nChunkCols = (unsigned int)m_nChunkCols;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockChunkWriter2::Write)write header failed");
		return -1;
	}

	size_t chunk_num = m_nChunkRows * m_nChunkCols;

	for (size_t r = 0; r < m_nChunkRows; ++r)
	{
		for (size_t c = 0; c < m_nChunkCols; ++c)
		{
			CHeightLoader::chunk_info_t* info = 
				m_pData->pChunks + r * m_nChunkCols + c;

			// 相对位置
			info->vMin.x -= m_fLeft;
			info->vMin.z -= m_fTop;
			info->vMax.x -= m_fLeft;
			info->vMax.z -= m_fTop;
		}
	}

	if (core_file::fwrite(m_pData->pChunks, 
		sizeof(CHeightLoader::chunk_info_t), chunk_num, fp) != chunk_num)
	{
		CORE_TRACE("(CBlockChunkWriter::Write)write values failed");
		return -1;
	}

	return sizeof(header) + (int)chunk_num * sizeof(CHeightLoader::chunk_info_t);
}

