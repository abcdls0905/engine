//--------------------------------------------------------------------
// �ļ���:		block_walk.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "block_walk.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_walkable.h"
#include "walk_height_map.h"
#include "walk_height_compress.h"
#include "walk_flag_map.h"
#include "walk_flag_compress.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// ������Ϣ�ļ�ͷ
struct walk_file_header_t
{
	unsigned int nRows;			// ����
	unsigned int nCols;			// ����
	unsigned int nFloorCount;	// ���߲�����
};

#pragma pack(pop)

// CBlockWalkReader

IBlockReader* CBlockWalkReader::NewInstance(CWalkLoader::walk_all_t* p,
	size_t rows, size_t cols)
{
	CBlockWalkReader* ptr = (CBlockWalkReader*)CORE_ALLOC(
		sizeof(CBlockWalkReader));
	
	new (ptr) CBlockWalkReader(p, rows, cols);

	return ptr;
}

CBlockWalkReader::CBlockWalkReader(CWalkLoader::walk_all_t* p, size_t rows, 
	size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockWalkReader::~CBlockWalkReader()
{
}

void CBlockWalkReader::Release()
{
	CORE_DELETE(this);
}

int CBlockWalkReader::GetSize() const
{
	return 0;
}
	
int CBlockWalkReader::Read(void* handle)
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
	
	count += sizeof(header);

	// ��ȡˮ��߶�
	unsigned int height_size = 0;

	if (core_file::fread(&height_size, sizeof(unsigned int), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkReader::Read)read water height size error");
		return -1;
	}

	if (height_size > 0x100000)
	{
		CORE_TRACE("(CBlockWalkReader::Read)water height size error");
		return -1;
	}

	if (height_size > 0)
	{
		m_pData->pWaterHeight = (unsigned char*)CORE_ALLOC(height_size);

		if (core_file::fread(m_pData->pWaterHeight, sizeof(unsigned char), 
			height_size, fp) != height_size)
		{
			CORE_TRACE("(CBlockWalkReader::Read)read water height error");
			return -1;
		}
	}

	m_pData->nWaterHeightSize = height_size;
	count += sizeof(height_size) + height_size;

	// ��ȡ�����߲���
	unsigned int marker_size = 0;

	if (core_file::fread(&marker_size, sizeof(unsigned int), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkReader::Read)read walk marker size error");
		return -1;
	}

	if (marker_size > 0x100000)
	{
		CORE_TRACE("(CBlockWalkReader::Read)walk marker size error");
		return -1;
	}

	if (marker_size > 0)
	{
		m_pData->pWalkMarker = (unsigned char*)CORE_ALLOC(marker_size);

		if (core_file::fread(m_pData->pWalkMarker, 
			sizeof(unsigned char), marker_size, fp) != marker_size)
		{
			CORE_TRACE("(CBlockWalkReader::Read)read walk marker error");
			return -1;
		}
	}

	m_pData->nWalkMarkerSize = marker_size;
	count += sizeof(marker_size) + marker_size;

	// ��ȡ���߲�߶�
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
	
	// ��ȡ���ϰ��ռ�߶�
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

	// ��ȡ�����߱��
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

	return count;
}

// CBlockWalkWriter

CBlockWalkWriter::CBlockWalkWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockWalkWriter::~CBlockWalkWriter()
{
}

int CBlockWalkWriter::GetSize() const
{
	return 0;
}

int CBlockWalkWriter::Write(void* handle)
{
	int count = 0;
	FILE* fp = (FILE*)handle;
	CTerrainWalkable* pWalkable = m_pZone->GetWalkable();
	size_t rows = pWalkable->GetRows();
	size_t cols = pWalkable->GetCols();
	size_t floors = pWalkable->GetFloorCount();
	walk_file_header_t header;

	header.nRows = (unsigned int)rows;
	header.nCols = (unsigned int)cols;
	header.nFloorCount = (unsigned int)floors;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkWriter::Write)write header failed");
		return -1;
	}

	count += sizeof(header);
	
	CWalkHeightCompress height_compress;
	CWalkFlagCompress marker_compress;
	// д��ˮ��߶�
	unsigned int height_size = 0;
	CWalkHeightMap* pHeightMap = pWalkable->GetWaterHeightMap();

	if (pHeightMap)
	{
		if (height_compress.Compress(rows, pHeightMap->GetData(), 
			pHeightMap->GetSize()))
		{
			height_size = (unsigned int)height_compress.GetResultSize();
		}
	}

	if (core_file::fwrite(&height_size, sizeof(unsigned int), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkWriter::Write)write water height size failed");
		return -1;
	}

	if (height_size > 0)
	{
		if (core_file::fwrite(height_compress.GetResultData(), 
			sizeof(unsigned char), height_size, fp) != height_size)
		{
			CORE_TRACE("(CBlockWalkWriter::Write)write water height failed");
			return -1;
		}
	}

	count += sizeof(height_size) + height_size;

	// д�������߲���
	unsigned int marker_size = 0;
	CWalkFlagMap* pMarkerMap = pWalkable->GetWalkMarkerMap();

	if (pMarkerMap)
	{
		if (marker_compress.Compress(rows, pMarkerMap->GetData(), 
			pMarkerMap->GetSize()))
		{
			marker_size = (unsigned int)marker_compress.GetResultSize();
		}
	}

	if (core_file::fwrite(&marker_size, sizeof(unsigned int), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockWalkWriter::Write)write walk marker size failed");
		return -1;
	}

	if (marker_size > 0)
	{
		if (core_file::fwrite(marker_compress.GetResultData(), 
			sizeof(unsigned char), marker_size, fp) != marker_size)
		{
			CORE_TRACE("(CBlockWalkWriter::Write)write walk marker failed");
			return -1;
		}
	}

	count += sizeof(marker_size) + marker_size;

	// д���߶�
	for (size_t k = 0; k < floors; ++k)
	{
		height_size = 0;
		pHeightMap = pWalkable->GetFloorHeightMap((int)k);

		if (pHeightMap)
		{
			if (height_compress.Compress(rows, pHeightMap->GetData(), 
				pHeightMap->GetSize()))
			{
				height_size = (unsigned int)height_compress.GetResultSize();
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
	
	// д�����ϰ��ռ�߶�
	for (size_t j = 0; j < floors; ++j)
	{
		height_size = 0;
		pHeightMap = pWalkable->GetSpaceHeightMap((int)j);

		if (pHeightMap)
		{
			if (height_compress.Compress(rows, pHeightMap->GetData(), 
				pHeightMap->GetSize()))
			{
				height_size = (unsigned int)height_compress.GetResultSize();
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

	// д������߱��
	for (size_t i = 0; i < floors; ++i)
	{
		unsigned int marker_size = 0;
		CWalkFlagMap* pMarkerMap = pWalkable->GetFloorMarkerMap((int)i);

		if (pMarkerMap)
		{
			if (marker_compress.Compress(rows, pMarkerMap->GetData(), 
				pMarkerMap->GetSize()))
			{
				marker_size = (unsigned int)marker_compress.GetResultSize();
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
