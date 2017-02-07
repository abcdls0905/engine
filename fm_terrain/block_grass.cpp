//--------------------------------------------------------------------
// 文件名:		block_grass.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_grass.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_grass.h"
#include "terrain_chunk.h"
#include "grass_type.h"
#include "name_list.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 花草文件头
struct grass_file_header_t
{
	unsigned int nGrassTypeNum;		// 花草类型数量
	unsigned int nGrassNameSize;	// 花草类型名字缓冲的总长度
	unsigned int nGrassDataNum;		// 花草信息数量
};

#pragma pack(pop)

// CBlockGrassReader

IBlockReader* CBlockGrassReader::NewInstance(CVisualLoader::grass_all_t* p)
{
	CBlockGrassReader* ptr = (CBlockGrassReader*)CORE_ALLOC(
		sizeof(CBlockGrassReader));

	new (ptr) CBlockGrassReader(p);

	return ptr;
}

CBlockGrassReader::CBlockGrassReader(CVisualLoader::grass_all_t* p)
{
	Assert(p != NULL);

	m_pData = p;
}

CBlockGrassReader::~CBlockGrassReader()
{
}

void CBlockGrassReader::Release()
{
	CORE_DELETE(this);
}

int CBlockGrassReader::GetSize() const
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

int CBlockGrassReader::Read(void* handle)
{
	FILE* fp = (FILE*)handle;
	grass_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockGrassReader::Read)read header failed");
		return -1;
	}
	
	if (header.nGrassTypeNum > 255)
	{
		CORE_TRACE("(CBlockGrassReader::Read)grass type number error");
		return -1;
	}
	
	if (header.nGrassNameSize > 0x10000)
	{
		CORE_TRACE("(CBlockGrassReader::Read)grass name size error");
		return -1;
	}
	
	if (header.nGrassDataNum >= 0x100000)
	{
		CORE_TRACE("(CBlockGrassReader::Read)grass data size error");
		return -1;
	}
	
	// 所有花草类型的名字
	size_t SIZE1 = header.nGrassNameSize;
	TAutoMem<char, 256, TCoreAlloc> name_auto_buf(SIZE1 + 1);
	char* name_buffer = name_auto_buf.GetBuffer();
	
	if (core_file::fread(name_buffer, sizeof(char), SIZE1, fp) != SIZE1)
	{
		CORE_TRACE("(CBlockGrassReader::Read)read grass names failed");
		return -1;
	}
	
	// 防止没有结束符
	name_buffer[SIZE1] = 0;
	
	if (get_buffer_names(name_buffer, SIZE1, m_pData->GrassNames) 
		!= header.nGrassTypeNum)
	{
		CORE_TRACE("(CBlockGrassReader::Read)get grass names failed");
		return -1;
	}
	
	// 数据
	size_t grass_data_num = header.nGrassDataNum;
	CVisualLoader::grass_info_t* grass_data = 
		(CVisualLoader::grass_info_t*)CORE_ALLOC(
		sizeof(CVisualLoader::grass_info_t) * grass_data_num);
	
	if (core_file::fread(grass_data, sizeof(CVisualLoader::grass_info_t), 
		grass_data_num, fp) != grass_data_num)
	{
		CORE_TRACE("(CBlockGrassReader::Read)read grass data failed");
		CORE_FREE(grass_data, 
			sizeof(CVisualLoader::grass_info_t) * grass_data_num);
		return -1;
	}
	
	for (size_t i = 0; i < grass_data_num; ++i)
	{
		if (grass_data[i].nGrassIndex >= header.nGrassTypeNum)
		{
			CORE_TRACE("(CBlockGrassReader::Read)grass index error");
			CORE_FREE(grass_data, 
				sizeof(CVisualLoader::grass_info_t) * grass_data_num);
			return -1;
		}
	}
	
	m_pData->nGrassTypeNum = header.nGrassTypeNum;
	m_pData->nGrassDataNum = header.nGrassDataNum;
	m_pData->pGrassData = grass_data;
	
	return sizeof(header) + header.nGrassNameSize 
		+ header.nGrassDataNum * sizeof(CVisualLoader::grass_info_t);
}

// CBlockGrassWriter

CBlockGrassWriter::CBlockGrassWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockGrassWriter::~CBlockGrassWriter()
{
}

int CBlockGrassWriter::GetSize() const
{
	return 0;
}

int CBlockGrassWriter::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	CTerrainGrass* pGrass = m_pZone->GetTerrain()->GetGrass();
	size_t grass_num = pGrass->GetCount();
	size_t data_num = 0;
	size_t chunk_num = m_pZone->GetChunkCount();
	size_t i;
	
	for (i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = m_pZone->GetChunkByIndex(i);
			
		for (size_t k = 0; k < grass_num; ++k)
		{
			data_num += pChunk->GetGrassCount((int)k);
		}
	}
	
	size_t name_size = 0;
	
	for (i = 0; i < grass_num; ++i)
	{
		name_size += strlen(pGrass->GetByIndex(i)->GetName()) + 1;
	}
	
	grass_file_header_t header;
	
	header.nGrassTypeNum = (unsigned int)grass_num;
	header.nGrassNameSize = (unsigned int)name_size;
	header.nGrassDataNum = (unsigned int)data_num;
	
	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockGrassWriter::Write)write header failed");
		return -1;
	}
	
	for (i = 0; i < grass_num; ++i)
	{
		const char* grass_name = pGrass->GetByIndex(i)->GetName();
		
		size_t name_len = strlen(grass_name) + 1;
		
		if (core_file::fwrite(grass_name, sizeof(char), name_len, fp) 
			!= name_len)
		{
			CORE_TRACE("(CBlockGrassWriter::Write)write grass name failed");
			return -1;
		}
	}

	float unit_size = m_pZone->GetTerrain()->GetUnitSize();
	CVisualLoader::grass_info_t* grass_data = 
		(CVisualLoader::grass_info_t*)CORE_ALLOC(
		sizeof(CVisualLoader::grass_info_t) * data_num);
	CVisualLoader::grass_info_t* p = grass_data;
	
	for (i = 0; i < chunk_num; ++i)
	{
		for (size_t k = 0; k < grass_num; ++k)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunkByIndex(i);
			size_t num = pChunk->GetGrassCount((int)k);

			if (0 == num)
			{
				continue;
			}

			const grass_data_t* data = pChunk->GetGrasses((int)k);

			for (size_t j = 0; j < num; ++j)
			{
				float pos_x = data[j].fPosiX - m_pZone->GetLeft();
				float pos_z = data[j].fPosiZ - m_pZone->GetTop();
				
				pos_x /= unit_size;
				pos_z /= unit_size;
				p->nPosiX = int(pos_x * 10.0F);
				p->nPosiZ = int(pos_z * 10.0F);
				p->nGrassIndex = data[j].nGrassIndex;
				p->nScaleAmount = (data[j].nScale << 4) + data[j].nAmount;
				p->nSeed = data[j].nSeed;
				p++;
			}
		}
	}
	
	if (core_file::fwrite(grass_data, sizeof(CVisualLoader::grass_info_t), 
		data_num, fp) != data_num)
	{
		CORE_TRACE("(CBlockGrassWriter::Write)write grass data failed");
		CORE_FREE(grass_data, sizeof(CVisualLoader::grass_info_t) * data_num);
		return -1;
	}
	
	CORE_FREE(grass_data, sizeof(CVisualLoader::grass_info_t) * data_num);
	
	return sizeof(grass_file_header_t) + (int)name_size 
		+ sizeof(CVisualLoader::grass_info_t) * (int)data_num;
}
