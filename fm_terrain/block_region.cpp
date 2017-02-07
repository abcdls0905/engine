//--------------------------------------------------------------------
// 文件名:		block_region.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月23日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_region.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_region.h"
#include "walk_bit_map.h"
#include "walk_bit_compress.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 地区标记信息文件头
struct region_file_header_t
{
	unsigned int nRows;			// 行数
	unsigned int nCols;			// 列数
	unsigned int nRegionCount;	// 地区标记数量
};

#pragma pack(pop)

// CBlockRegionReader

IBlockReader* CBlockRegionReader::NewInstance(CRegionLoader::region_all_t* p,
	size_t rows, size_t cols)
{
	CBlockRegionReader* ptr = (CBlockRegionReader*)CORE_ALLOC(
		sizeof(CBlockRegionReader));
	
	new (ptr) CBlockRegionReader(p, rows, cols);

	return ptr;
}

CBlockRegionReader::CBlockRegionReader(CRegionLoader::region_all_t* p, 
	size_t rows, size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockRegionReader::~CBlockRegionReader()
{
}

void CBlockRegionReader::Release()
{
	CORE_DELETE(this);
}

int CBlockRegionReader::GetSize() const
{
	return 0;
}

int CBlockRegionReader::Read(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	region_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockRegionReader::Read)read header failed");
		return -1;
	}

	if ((header.nRows != m_nRows) || (header.nCols != m_nCols))
	{
		CORE_TRACE("(CBlockRegionReader::Read)scale error");
		return -1;
	}

	if (header.nRegionCount > 256)
	{
		CORE_TRACE("(CBlockRegionReader::Read)region amount error");
		return -1;
	}

	count += sizeof(header);

	// 读取地区标记
	for (size_t i = 0; i < header.nRegionCount; ++i)
	{
		unsigned int region_name_len;

		if (core_file::fread(&region_name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockRegionReader::Read)read region name len error");
			return -1;
		}

		if (region_name_len > 256)
		{
			CORE_TRACE("(CBlockRegionReader::Read)region name len error");
			return -1;
		}

		TAutoMem<char, 256, TCoreAlloc> auto_buf(region_name_len + 1);
		char* region_name = auto_buf.GetBuffer();

		if (core_file::fread(region_name, sizeof(char), region_name_len, fp) 
			!= region_name_len)
		{
			CORE_TRACE("(CBlockRegionReader::Read)read region name error");
			return -1;
		}

		region_name[region_name_len] = 0;

		unsigned int marker_size = 0;

		if (core_file::fread(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockRegionReader::Read)read region size error");
			return -1;
		}

		if (marker_size > 0x100000)
		{
			CORE_TRACE("(CBlockRegionReader::Read)region size error");
			return -1;
		}

		unsigned char* marker_data = (unsigned char*)CORE_ALLOC(marker_size);

		if (core_file::fread(marker_data, sizeof(unsigned char), marker_size, 
			fp) != marker_size)
		{
			CORE_FREE(marker_data, marker_size);
			CORE_TRACE("(CBlockRegionReader::Read)read region error");
			return -1;
		}

		CRegionLoader::region_data_t region;
		size_t name_size = strlen(region_name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);

		memcpy(pName, region_name, name_size);
		region.pName = pName;
		region.nSize = marker_size;
		region.pData = marker_data;

		m_pData->push_back(region);

		count += sizeof(region_name_len) + region_name_len 
			+ sizeof(marker_size) + marker_size;
	}

	return count;
}

// CBlockRegionWriter

CBlockRegionWriter::CBlockRegionWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);

	m_pZone = pZone;
}

CBlockRegionWriter::~CBlockRegionWriter()
{
}

int CBlockRegionWriter::GetSize() const
{
	return 0;
}

int CBlockRegionWriter::Write(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	CTerrainRegion* pRegion = m_pZone->GetRegion();
	size_t rows = pRegion->GetRows();
	size_t cols = pRegion->GetCols();
	size_t region_num = pRegion->GetRegionCount();
	// 真正有数据的地区标记数量
	size_t regions = 0;

	for (size_t k = 0; k < region_num; ++k)
	{
		if (!pRegion->GetRegionMap(k)->GetIsNull())
		{
			++regions;
		}
	}

	region_file_header_t header;

	header.nRows = (unsigned int)rows;
	header.nCols = (unsigned int)cols;
	header.nRegionCount = (unsigned int)regions;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockRegionWriter::Write)write header failed");
		return -1;
	}

	count += sizeof(header);

	CWalkBitCompress marker_compress;

	// 写入地区标记
	for (size_t i = 0; i < region_num; ++i)
	{
		CWalkBitMap* pMap = pRegion->GetRegionMap(i);

		if (pMap->GetIsNull())
		{
			// 没有数据的标记图
			continue;
		}

		// 地区标记名称
		const char* region_name = pRegion->GetRegionName(i);
		unsigned int region_name_len = (unsigned int)strlen(region_name) + 1;

		if (core_file::fwrite(&region_name_len, sizeof(unsigned int), 1, fp) 
			!= 1)
		{
			CORE_TRACE("(CBlockRegionWriter::Write)write region name len failed");
			return -1;
		}

		if (core_file::fwrite(region_name, sizeof(char), region_name_len, fp) 
			!= region_name_len)
		{
			CORE_TRACE("(CBlockRegionWriter::Write)write region name failed");
			return -1;
		}

		unsigned int marker_size = 0;

		if (marker_compress.Compress(rows, pMap->GetData(), pMap->GetSize()))
		{
			marker_size = (unsigned int)marker_compress.GetResultSize();
		}

		if (core_file::fwrite(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockRegionWriter::Write)write region size failed");
			return -1;
		}

		if (marker_size > 0)
		{
			if (core_file::fwrite(marker_compress.GetResultData(), 
				sizeof(unsigned char), marker_size, fp) != marker_size)
			{
				CORE_TRACE("(CBlockRegionWriter::Write)write region failed");
				return -1;
			}
		}

		count += sizeof(region_name_len) + region_name_len 
			+ sizeof(marker_size) + marker_size;
	}

	return count;
}

