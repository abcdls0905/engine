//--------------------------------------------------------------------
// 文件名:		block_base.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_base.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "name_list.h"
#include "../utils/array_pod.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 基础贴图信息文件头
struct base_file_header_t
{
	unsigned int nChunkRows;	// 分块行数
	unsigned int nChunkCols;	// 分块列数
	unsigned int nBaseTexNum;	// 基础贴图数量
	unsigned int nBaseNameSize;	// 贴图名字缓冲的总长度
};

#pragma pack(pop)

// CBlockBaseReader

IBlockReader* CBlockBaseReader::NewInstance(CAlphaLoader::base_all_t* p,
	size_t chunk_rows, size_t chunk_cols)
{
	CBlockBaseReader* ptr = (CBlockBaseReader*)CORE_ALLOC(
		sizeof(CBlockBaseReader));

	new (ptr) CBlockBaseReader(p, chunk_rows, chunk_cols);

	return ptr;
}

CBlockBaseReader::CBlockBaseReader(CAlphaLoader::base_all_t* p,
	size_t chunk_rows, size_t chunk_cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nChunkRows = chunk_rows;
	m_nChunkCols = chunk_cols;
}

CBlockBaseReader::~CBlockBaseReader()
{
}

void CBlockBaseReader::Release()
{
	CORE_DELETE(this);
}

int CBlockBaseReader::GetSize() const
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

int CBlockBaseReader::Read(void* handle)
{
	FILE* fp = (FILE*)handle;
	base_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockBaseReader::Read)read header failed");
		return -1;
	}

	if ((header.nChunkRows != m_nChunkRows)
		|| (header.nChunkCols != m_nChunkCols))
	{
		CORE_TRACE("(CBlockBaseReader::Read)chunk size error");
		return -1;
	}

	if (header.nBaseTexNum > Terrain::MAX_TEX_NUM)
	{
		CORE_TRACE("(CBlockBaseReader::Read)base tex number error");
		return -1;
	}

	if (header.nBaseNameSize > 0x10000)
	{
		CORE_TRACE("(CBlockBaseReader::Read)base name size error");
		return -1;
	}

	// 所有贴图的名字
	size_t name_size = header.nBaseNameSize;
	TAutoMem<char, 256, TCoreAlloc> name_auto_buf(name_size + 1);
	char* name_buffer = name_auto_buf.GetBuffer();
	
	if (core_file::fread(name_buffer, sizeof(char), name_size, fp) 
		!= name_size)
	{
		CORE_TRACE("(CBlockBaseReader::Read)read base names failed");
		return -1;
	}
	
	// 防止没有结束符
	name_buffer[name_size] = 0;

	if (get_buffer_names(name_buffer, name_size, m_pData->BaseNames) 
		!= header.nBaseTexNum)
	{
		CORE_TRACE("(CBlockBaseReader::Read)get base names failed");
		return -1;
	}

	// 分块的贴图索引
	size_t chunk_num = header.nChunkRows* header.nChunkCols;
	unsigned char* base_tex_index = (unsigned char*)CORE_ALLOC(chunk_num);

	if (core_file::fread(base_tex_index, sizeof(unsigned char), chunk_num, fp) 
		!= chunk_num)
	{
		CORE_TRACE("(CBlockBaseReader::Read)read base index failed");
		CORE_FREE(base_tex_index, chunk_num);
		return -1;
	}
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		if (base_tex_index[i] >= header.nBaseTexNum)
		{
			CORE_TRACE("(CBlockBaseReader::Read)base tex index error");
			CORE_FREE(base_tex_index, chunk_num);
			return -1;
		}
	}

	m_pData->pBaseTexIndex = base_tex_index;
	m_pData->nBaseTexNum = header.nBaseTexNum;
	
	return sizeof(header) + header.nBaseNameSize 
		+ (int)chunk_num * sizeof(unsigned char); 
}

// CBlockBaseWriter

CBlockBaseWriter::CBlockBaseWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockBaseWriter::~CBlockBaseWriter()
{
}

int CBlockBaseWriter::GetSize() const
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

int CBlockBaseWriter::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	CBlendTexSet* pBaseTexSet = m_pZone->GetTerrain()->GetBaseTexSet();
	TArrayPod<int, 1, TCoreAlloc> used_bases;
	size_t chunk_rows = m_pZone->GetChunkDims();
	size_t chunk_cols = m_pZone->GetChunkDims();
	size_t chunk_num = m_pZone->GetChunkCount();

	for (size_t k = 0; k < chunk_num; ++k)
	{
		CTerrainChunk* pChunk = m_pZone->GetChunkByIndex(k);
		int base_index = pChunk->GetBaseTexIndex();

		if (size_t(base_index) < pBaseTexSet->GetCount())
		{
			if (get_vector_index(used_bases, base_index) < 0)
			{
				used_bases.push_back(base_index);
			}
		}
	}

	// 只保存使用过的基础贴图信息
	size_t tex_num = used_bases.size();
	size_t name_size = 0;

	for (size_t j = 0; j < tex_num; ++j)
	{
		int base_index = used_bases[j];
		
		name_size += 
			strlen(pBaseTexSet->GetByIndex(base_index)->GetName()) + 1;
	}

	base_file_header_t header;

	header.nChunkRows = (unsigned int)chunk_rows;
	header.nChunkCols = (unsigned int)chunk_cols;
	header.nBaseTexNum = (unsigned int)tex_num;
	header.nBaseNameSize = (unsigned int)name_size;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockBaseWriter::Write)write header failed");
		return -1;
	}

	for (size_t i = 0; i < tex_num; ++i)
	{
		int base_index = used_bases[i];
		const char* tex_name = pBaseTexSet->GetByIndex(base_index)->GetName();
		size_t name_len = strlen(tex_name) + 1;

		if (core_file::fwrite(tex_name, sizeof(char), name_len, fp) 
			!= name_len)
		{
			CORE_TRACE("(CBlockBaseWriter::Write)write tex name failed");
			return -1;
		}
	}
	
	unsigned char* tex_index = (unsigned char*)CORE_ALLOC(chunk_num);

	for (size_t r = 0; r < chunk_rows; ++r)
	{
		for (size_t c = 0; c < chunk_cols; ++c)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunk(r, c);
			int base_index = pChunk->GetBaseTexIndex();
			int used_index = get_vector_index(used_bases, base_index);

			if (used_index < 0)
			{
				tex_index[r * chunk_cols + c] = 0;
			}
			else
			{
				tex_index[r * chunk_cols + c] = used_index;
			}
		}
	}

	if (core_file::fwrite(tex_index, sizeof(unsigned char), chunk_num, fp) 
		!= chunk_num)
	{
		CORE_TRACE("(CBlockBaseWriter::Write)write tex index failed");
		CORE_FREE(tex_index, chunk_num);
		return -1;
	}

	CORE_FREE(tex_index, chunk_num);

	return sizeof(header) + (int)name_size 
		+ (int)chunk_num * sizeof(unsigned char); 
}

