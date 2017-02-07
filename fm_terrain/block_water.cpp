//--------------------------------------------------------------------
// 文件名:		block_water.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_water.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "terrain_water.h"
#include "sea_water.h"
#include "name_list.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 水面类型信息文件头
struct water_file_header_t
{
	unsigned int nChunkRows;		// 分块行数
	unsigned int nChunkCols;		// 分块列数
	unsigned int nWaterNum;			// 水面类型数量
	unsigned int nWaterNameSize;	// 水面类型名字缓冲的总长度
};

#pragma pack(pop)

// CBlockWaterReader

IBlockReader* CBlockWaterReader::NewInstance(CHeightLoader::water_all_t* p,
	size_t chunk_rows, size_t chunk_cols)
{
	CBlockWaterReader* ptr = (CBlockWaterReader*)CORE_ALLOC(
		sizeof(CBlockWaterReader));
	
	new (ptr) CBlockWaterReader(p, chunk_rows, chunk_cols);

	return ptr;
}

CBlockWaterReader::CBlockWaterReader(CHeightLoader::water_all_t* p, 
	size_t chunk_rows, size_t chunk_cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nChunkRows = chunk_rows;
	m_nChunkCols = chunk_cols;
}

CBlockWaterReader::~CBlockWaterReader()
{
}

void CBlockWaterReader::Release()
{
	CORE_DELETE(this);
}

int CBlockWaterReader::GetSize() const
{
	return 0;
}
	
static size_t get_buffer_names(const char* buffer, size_t size, 
	CNameList& names)
{
	size_t count = 0;
	const char* s = NULL;
	
	for (size_t i = 0; i < size; ++i)
	{
		if (buffer[i] == 0)
		{
			if (s != NULL)
			{
				names.AddName(s);
				s = NULL;
				count++;
			}
		}
		else if (NULL == s)
		{
			s = buffer + i;
		}
	}
	
	return count;
}

int CBlockWaterReader::Read(void* handle)
{
	FILE* fp = (FILE*)handle;
	water_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWaterReader::Read)read header failed");
		return -1;
	}
	
	if ((header.nChunkRows != m_nChunkRows)
		|| (header.nChunkCols != m_nChunkCols))
	{
		CORE_TRACE("(CBlockWaterReader::Read)chunk size error");
		return -1;
	}
	
	if (header.nWaterNum > 255)
	{
		CORE_TRACE("(CBlockWaterReader::Read)water number error");
		return -1;
	}
	
	if (header.nWaterNameSize > 0x10000)
	{
		CORE_TRACE("(CBlockWaterReader::Read)water name size error");
		return -1;
	}
	
	// 所有水面的名字
	size_t name_size = header.nWaterNameSize;
	TAutoMem<char, 256, TCoreAlloc> name_auto_buf(name_size + 1);
	char* name_buffer = name_auto_buf.GetBuffer();
	
	if (core_file::fread(name_buffer, sizeof(char), name_size, fp) 
		!= name_size)
	{
		CORE_TRACE("(CBlockWaterReader::Read)read water names failed");
		return -1;
	}
	
	// 防止没有结束符
	name_buffer[name_size] = 0;
	
	if (get_buffer_names(name_buffer, name_size, m_pData->WaterNames) 
		!= header.nWaterNum)
	{
		CORE_TRACE("(CBlockWaterReader::Read)get water names failed");
		return -1;
	}
	
	// 分块的水面索引
	size_t chunk_num = header.nChunkRows * header.nChunkCols;
	unsigned char* water_index = (unsigned char*)CORE_ALLOC(chunk_num);
	
	if (core_file::fread(water_index, sizeof(unsigned char), chunk_num, fp) 
		!= chunk_num)
	{
		CORE_TRACE("(CBlockWaterReader::Read)read water index failed");
		CORE_FREE(water_index, chunk_num);
		return -1;
	}
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		if (water_index[i] == 0xFF)
		{
			continue;
		}
		
		if (water_index[i] >= header.nWaterNum)
		{
			CORE_TRACE("(CBlockWaterReader::Read)water index error");
			CORE_FREE(water_index, chunk_num);
			return -1;
		}
	}
	
	m_pData->nWaterNum = header.nWaterNum;
	m_pData->pWaterIndex = water_index;
	
	return sizeof(header) + header.nWaterNameSize 
		+ (int)chunk_num * sizeof(unsigned char);
}

// CBlockWaterWriter

CBlockWaterWriter::CBlockWaterWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockWaterWriter::~CBlockWaterWriter()
{
}

int CBlockWaterWriter::GetSize() const
{
	return 0;
}

// 获得指定值在数组中的索引
static int get_vector_index(TArrayPod<int, 1, TCoreAlloc>& vec, int value)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == value)
		{
			return int(i);
		}
	}

	return -1;
}

int CBlockWaterWriter::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	CTerrainWater* pWater = m_pZone->GetTerrain()->GetWater();
	TArrayPod<int, 1, TCoreAlloc> used_waters;
	size_t chunk_rows = m_pZone->GetChunkDims();
	size_t chunk_cols = m_pZone->GetChunkDims();
	size_t chunk_num = m_pZone->GetChunkCount();
	
	for (size_t k = 0; k < chunk_num; ++k)
	{
		CTerrainChunk* pChunk = m_pZone->GetChunkByIndex(k);
		int water_index = pChunk->GetWaterIndex();
		
		if (size_t(water_index) < pWater->GetCount())
		{
			if (get_vector_index(used_waters, water_index) < 0)
			{
				used_waters.push_back(water_index);
			}
		}
	}

	// 只保存使用过的水面信息
	size_t water_num = used_waters.size();

	if (water_num > 255)
	{
		water_num = 255;
	}

	size_t name_size = 0;
	size_t i;

	for (i = 0; i < water_num; ++i)
	{
		int water_index = used_waters[i];
		
		name_size += strlen(pWater->GetByIndex(water_index)->GetName()) + 1;
	}
	
	water_file_header_t header;

	header.nChunkRows = (unsigned int)chunk_rows;
	header.nChunkCols = (unsigned int)chunk_cols;
	header.nWaterNum = (unsigned int)water_num;
	header.nWaterNameSize = (unsigned int)name_size;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWaterWriter::Write)write header failed");
		return -1;
	}

	for (i = 0; i < water_num; ++i)
	{
		int water_index = used_waters[i];
		const char* water_name = pWater->GetByIndex(water_index)->GetName();
		size_t name_len = strlen(water_name) + 1;

		if (core_file::fwrite(water_name, sizeof(char), name_len, fp) 
			!= name_len)
		{
			CORE_TRACE("(CBlockWaterWriter::Write)write name failed");
			return -1;
		}
	}
	
	unsigned char* water_data = (unsigned char*)CORE_ALLOC(chunk_num);

	for (size_t r = 0; r < chunk_rows; ++r)
	{
		for (size_t c = 0; c < chunk_cols; ++c)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunk(r, c);
			int water_index = pChunk->GetWaterIndex();
			int used_index = get_vector_index(used_waters, water_index);

			if (used_index < 0)
			{
				water_data[r * chunk_cols + c] = 0xFF;
			}
			else if (size_t(used_index) < water_num)
			{
				water_data[r * chunk_cols + c] = used_index;
			}
			else
			{
				CORE_TRACE("(CBlockWaterWriter::Write)water index error");
				water_data[r * chunk_cols + c] = 0xFF;
			}
		}
	}

	if (core_file::fwrite(water_data, sizeof(unsigned char), chunk_num, fp) 
		!= chunk_num)
	{
		CORE_TRACE("(CBlockWaterWriter::Write)write water data failed");
		CORE_FREE(water_data, chunk_num);
		return -1;
	}

	CORE_FREE(water_data, chunk_num);

	return sizeof(header) + (int)name_size 
		+ (int)chunk_num * sizeof(unsigned char);
}

