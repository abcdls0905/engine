//--------------------------------------------------------------------
// 文件名:		BlockDynamicWalk.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月11日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_dynamic_walk.h"

#include "dynamic_walkable.h"
#include "terrain.h"
#include "walk_height_map.h"
#include "walk_height_compress.h"
#include "walk_flag_map.h"
#include "dynamic_walk_flag_compress.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 行走信息文件头
struct walk_file_header_t
{
	unsigned int nRows;			// 行数
	unsigned int nCols;			// 列数
	unsigned int nFloorCount;	// 行走层数量
	float fAngleY;				// 角度
	float fHeight;				// 高度
	float fRadius;				// 外切圆半径
	float fBoxSizeX;			// 包围盒X轴尺寸
	float fBoxSizeZ;			// 包围盒Z轴尺寸
	//unsigned int nWallCount;	// 墙面标记层数量
};

#pragma pack(pop)

// CBlockDynamicWalkReader

IBlockReader* CBlockDynamicWalkReader::NewInstance(CDynamicWalkLoader::walk_all_t* p,
	size_t rows, size_t cols)
{
	CBlockDynamicWalkReader* ptr = (CBlockDynamicWalkReader*)CORE_ALLOC(
		sizeof(CBlockDynamicWalkReader));
	
	new (ptr) CBlockDynamicWalkReader(p, rows, cols);

	return ptr;
}

CBlockDynamicWalkReader::CBlockDynamicWalkReader(CDynamicWalkLoader::walk_all_t* p, size_t rows, 
	size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockDynamicWalkReader::~CBlockDynamicWalkReader()
{
}

void CBlockDynamicWalkReader::Release()
{
	CORE_DELETE(this);
}

int CBlockDynamicWalkReader::GetSize() const
{
	return 0;
}
	
int CBlockDynamicWalkReader::Read(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	walk_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkReader::Read)read header failed");
		return -1;
	}
	
	if ((header.nRows != m_nRows) || (header.nCols != m_nCols))
	{
		CORE_TRACE("(CBlockWalkReader::Read)scale error");
		return -1;
	}

	if (header.nFloorCount > MAX_FLOOR_NUM)
	{
		CORE_TRACE("(CBlockWalkReader::Read)floor amount error");
		return -1;
	}
	
	//if (header.nWallCount > MAX_FLOOR_NUM)
	//{
	//	CORE_TRACE("(CBlockWalkReader::Read)wall amount error");
	//	return -1;
	//}

	count += sizeof(header);

	// 读取水面高度
	unsigned int height_size = 0;

	// 读取主行走层标记
	unsigned int marker_size = 0;

	// 读取行走层高度
	for (size_t k = 0; k < header.nFloorCount; ++k)
	{
		height_size = 0;

		if (core_file::fread(&height_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkReader::Read)read floor height size error");
			return -1;
		}

		if (height_size > 0x100000)
		{
			CORE_TRACE("(CBlockWalkReader::Read)floor height size error");
			return -1;
		}

		if (height_size > 0)
		{
			m_pData->pFloorHeights[k] = (unsigned char*)CORE_ALLOC(
				height_size);

			if (core_file::fread(m_pData->pFloorHeights[k], 
				sizeof(unsigned char), height_size, fp) != height_size)
			{
				CORE_TRACE("(CBlockWalkReader::Read)read floor height error");
				return -1;
			}
		}

		m_pData->nFloorHeightsSize[k] = height_size;
		count += sizeof(height_size) + height_size;
	}
	
	// 读取无障碍空间高度
	for (size_t j = 0; j < header.nFloorCount; ++j)
	{
		height_size = 0;

		if (core_file::fread(&height_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkReader::Read)read space height size error");
			return -1;
		}

		if (height_size > 0x100000)
		{
			CORE_TRACE("(CBlockWalkReader::Read)space height size error");
			return -1;
		}

		if (height_size > 0)
		{
			m_pData->pSpaceHeights[j] = (unsigned char*)CORE_ALLOC(
				height_size);

			if (core_file::fread(m_pData->pSpaceHeights[j], 
				sizeof(unsigned char), height_size, fp) != height_size)
			{
				CORE_TRACE("(CBlockWalkReader::Read)read space height error");
				return -1;
			}
		}

		m_pData->nSpaceHeightsSize[j] = height_size;
		count += sizeof(height_size) + height_size;
	}

	// 读取可行走标记
	for (size_t i = 0; i < header.nFloorCount; ++i)
	{
		unsigned int marker_size = 0;

		if (core_file::fread(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkReader::Read)read floor marker size error");
			return -1;
		}

		if (marker_size > 0x100000)
		{
			CORE_TRACE("(CBlockWalkReader::Read)floor marker size error");
			return -1;
		}

		if (marker_size > 0)
		{
			m_pData->pFloorMarkers[i] = (unsigned char*)CORE_ALLOC(
				marker_size);

			if (core_file::fread(m_pData->pFloorMarkers[i], 
				sizeof(unsigned char), marker_size, fp) != marker_size)
			{
				CORE_TRACE("(CBlockWalkReader::Read)read floor marker error");
				return -1;
			}
		}

		m_pData->nFloorMarkersSize[i] = marker_size;
		count += sizeof(marker_size) + marker_size;
	}

	m_pData->nFloorCount = header.nFloorCount;
	m_pData->fAngleY = header.fAngleY;
	m_pData->fHeight = header.fHeight;
	m_pData->fRadius = header.fRadius;
	m_pData->fBoxSizeX = header.fBoxSizeX;
	m_pData->fBoxSizeZ = header.fBoxSizeZ;
	//m_pData->nWallCount = header.nWallCount;

	return count;
}

// CBlockDynamicWalkWriter

CBlockDynamicWalkWriter::CBlockDynamicWalkWriter(CDynamicWalkable* pWalk)
{
	Assert(pWalk != NULL);
	
	m_pWalk = pWalk;
}

CBlockDynamicWalkWriter::~CBlockDynamicWalkWriter()
{
}

int CBlockDynamicWalkWriter::GetSize() const
{
	return 0;
}

int CBlockDynamicWalkWriter::Write(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	CDynamicWalkable* pWalkable = m_pWalk;
	size_t rows = pWalkable->GetRows();
	size_t cols = pWalkable->GetCols();
	size_t floors = pWalkable->GetFloorCount();
	float angleY = pWalkable->GetAngleY();
	float height = pWalkable->GetHeight();
	float radius = pWalkable->GetRadius();
	float boxSizeX = pWalkable->GetBoxSizeX();
	float boxSizeZ = pWalkable->GetBoxSizeZ();
	//size_t walls = pWalkable->GetWallCount();
	walk_file_header_t header;

	header.nRows = rows;
	header.nCols = cols;
	header.nFloorCount = floors;
	header.fAngleY = angleY;
	header.fHeight = height;
	header.fRadius = radius;
	header.fBoxSizeX = boxSizeX;
	header.fBoxSizeZ = boxSizeZ;
	//header.nWallCount = walls;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkWriter::Write)write header failed");
		return -1;
	}

	count += sizeof(header);
	
	CWalkHeightCompress height_compress;
	CDynamicWalkFlagCompress marker_compress;
	// 写入水面高度
	unsigned int height_size = 0;
	CWalkHeightMap* pHeightMap = NULL;

	// 写入主行走层标记
	unsigned int marker_size = 0;
	CWalkFlagMap* pMarkerMap = NULL;

	// 写入层高度
	for (size_t k = 0; k < floors; ++k)
	{
		height_size = 0;
		pHeightMap = pWalkable->GetFloorHeightMap(k);

		if (pHeightMap)
		{
			if (height_compress.Compress(rows, pHeightMap->GetData(), 
				pHeightMap->GetSize()))
			{
				height_size = height_compress.GetResultSize();
			}
		}

		if (core_file::fwrite(&height_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkWriter::Write)write floor height size failed");
			return -1;
		}

		if (height_size > 0)
		{
			if (core_file::fwrite(height_compress.GetResultData(), 
				sizeof(unsigned char), height_size, fp) != height_size)
			{
				CORE_TRACE("(CBlockWalkWriter::Write)write floor height failed");
				return -1;
			}
		}
		
		count += sizeof(height_size) + height_size;
	}
	
	// 写入无障碍空间高度
	for (size_t j = 0; j < floors; ++j)
	{
		height_size = 0;
		pHeightMap = pWalkable->GetSpaceHeightMap(j);

		if (pHeightMap)
		{
			if (height_compress.Compress(rows, pHeightMap->GetData(), 
				pHeightMap->GetSize()))
			{
				height_size = height_compress.GetResultSize();
			}
		}

		if (core_file::fwrite(&height_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkWriter::Write)write space height size failed");
			return -1;
		}

		if (height_size > 0)
		{
			if (core_file::fwrite(height_compress.GetResultData(), 
				sizeof(unsigned char), height_size, fp) != height_size)
			{
				CORE_TRACE("(CBlockWalkWriter::Write)write space height failed");
				return -1;
			}
		}

		count += sizeof(height_size) + height_size;
	}

	//CWalkBitCompress marker_compress;

	// 写入可行走标记
	for (size_t i = 0; i < floors; ++i)
	{
		unsigned int marker_size = 0;
		CWalkFlagMap* pMarkerMap = pWalkable->GetFloorMarkerMap(i);

		if (pMarkerMap)
		{
			if (marker_compress.Compress(rows, pMarkerMap->GetData(), 
				pMarkerMap->GetSize()))
			{
				marker_size = marker_compress.GetResultSize();
			}
		}

		if (core_file::fwrite(&marker_size, sizeof(unsigned int), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockWalkWriter::Write)write floor marker size failed");
			return -1;
		}

		if (marker_size > 0)
		{
			if (core_file::fwrite(marker_compress.GetResultData(), 
				sizeof(unsigned char), marker_size, fp) != marker_size)
			{
				CORE_TRACE("(CBlockWalkWriter::Write)write floor marker failed");
				return -1;
			}
		}

		count += sizeof(marker_size) + marker_size;
	}

	return count;
}
