//--------------------------------------------------------------------
// 文件名:		block_area.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月2日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_area.h"
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
struct area_file_header_t
{
	unsigned int nRows;			// 行数
	unsigned int nCols;			// 列数
	unsigned int nAreaCount;	// 地区标记数量
};

#pragma pack(pop)

// CBlockAreaReader

IBlockReader* CBlockAreaReader::NewInstance(CRegionLoader::area_all_t* p,
	size_t rows, size_t cols)
{
	CBlockAreaReader* ptr = (CBlockAreaReader*)CORE_ALLOC(
		sizeof(CBlockAreaReader));

	new (ptr) CBlockAreaReader(p, rows, cols);

	return ptr;
}

CBlockAreaReader::CBlockAreaReader(CRegionLoader::area_all_t* p, 
	size_t rows, size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockAreaReader::~CBlockAreaReader()
{
}

void CBlockAreaReader::Release()
{
	CORE_DELETE(this);
}

int CBlockAreaReader::GetSize() const
{
	return 0;
}
	
int CBlockAreaReader::Read(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	area_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockAreaReader::Read)read header failed");
		return -1;
	}
	
	if ((header.nRows != m_nRows) || (header.nCols != m_nCols))
	{
		CORE_TRACE("(CBlockAreaReader::Read)scale error");
		return -1;
	}

	if (header.nAreaCount > 256)
	{
		CORE_TRACE("(CBlockAreaReader::Read)area amount error");
		return -1;
	}
	
	count += sizeof(header);

	// 读取地区标记
	for (size_t i = 0; i < header.nAreaCount; ++i)
	{
		unsigned int area_name_len;

		if (core_file::fread(&area_name_len, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockAreaReader::Read)read area name len error");
			return -1;
		}

		if (area_name_len > 256)
		{
			CORE_TRACE("(CBlockAreaReader::Read)area name len error");
			return -1;
		}

		TAutoMem<char, 256, TCoreAlloc> auto_buf(area_name_len + 1);
		char* area_name = auto_buf.GetBuffer();

		if (core_file::fread(area_name, sizeof(char), area_name_len, fp) 
			!= area_name_len)
		{
			CORE_TRACE("(CBlockAreaReader::Read)read area name error");
			return -1;
		}

		area_name[area_name_len] = 0;

		unsigned int marker_size = 0;

		if (core_file::fread(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockAreaReader::Read)read region size error");
			return -1;
		}

		if (marker_size > 0x100000)
		{
			CORE_TRACE("(CBlockAreaReader::Read)region size error");
			return -1;
		}

		unsigned char* marker_data = (unsigned char*)CORE_ALLOC(marker_size);

		if (core_file::fread(marker_data, sizeof(unsigned char), marker_size, 
			fp) != marker_size)
		{
			CORE_FREE(marker_data, marker_size);
			CORE_TRACE("(CBlockAreaReader::Read)read area error");
			return -1;
		}

		CRegionLoader::area_data_t area;
		size_t name_size = strlen(area_name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);

		memcpy(pName, area_name, name_size);
		area.pName = pName;
		area.nSize = marker_size;
		area.pData = marker_data;

		m_pData->push_back(area);
		
		count += sizeof(area_name_len) + area_name_len + sizeof(marker_size) 
			+ marker_size;
	}

	return count;
}

// CBlockAreaWriter

CBlockAreaWriter::CBlockAreaWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockAreaWriter::~CBlockAreaWriter()
{
}

int CBlockAreaWriter::GetSize() const
{
	return 0;
}

int CBlockAreaWriter::Write(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	CTerrainRegion* pRegion = m_pZone->GetRegion();
	size_t rows = pRegion->GetRows();
	size_t cols = pRegion->GetCols();
	size_t area_num = pRegion->GetAreaCount();
	// 真正有数据的地区标记数量
	size_t areas = 0;

	for (size_t k = 0; k < area_num; ++k)
	{
		if (!pRegion->GetAreaMap(k)->GetIsNull())
		{
			++areas;
		}
	}

	area_file_header_t header;

	header.nRows = (unsigned int)rows;
	header.nCols = (unsigned int)cols;
	header.nAreaCount = (unsigned int)areas;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockAreaWriter::Write)write header failed");
		return -1;
	}

	count += sizeof(header);
	
	CWalkBitCompress marker_compress;

	// 写入地区标记
	for (size_t i = 0; i < area_num; ++i)
	{
		CWalkBitMap* pMap = pRegion->GetAreaMap(i);

		if (pMap->GetIsNull())
		{
			// 没有数据的标记图
			continue;
		}

		// 地区标记名称
		const char* area_name = pRegion->GetAreaName(i);
		unsigned int area_name_len = (unsigned int)strlen(area_name) + 1;

		if (core_file::fwrite(&area_name_len, sizeof(unsigned int), 1, fp) 
			!= 1)
		{
			CORE_TRACE("(CBlockAreaWriter::Write)write area name len failed");
			return -1;
		}

		if (core_file::fwrite(area_name, sizeof(char), area_name_len, fp) 
			!= area_name_len)
		{
			CORE_TRACE("(CBlockAreaWriter::Write)write area name failed");
			return -1;
		}
		
		unsigned int marker_size = 0;

		if (marker_compress.Compress(rows, pMap->GetData(), pMap->GetSize()))
		{
			marker_size = (unsigned int)marker_compress.GetResultSize();
		}

		if (core_file::fwrite(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockAreaWriter::Write)write area size failed");
			return -1;
		}

		if (marker_size > 0)
		{
			if (core_file::fwrite(marker_compress.GetResultData(), 
				sizeof(unsigned char), marker_size, fp) != marker_size)
			{
				CORE_TRACE("(CBlockAreaWriter::Write)write area failed");
				return -1;
			}
		}

		count += sizeof(area_name_len) + area_name_len + sizeof(marker_size) 
			+ marker_size;
	}

	return count;
}

