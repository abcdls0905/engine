//--------------------------------------------------------------------
// 文件名:		block_height.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_height.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 高度信息文件头
struct height_file_header_t
{
	unsigned int nRows;	// 行数
	unsigned int nCols;	// 列数
};

#pragma pack(pop)

// CBlockHeightReader

IBlockReader* CBlockHeightReader::NewInstance(CHeightLoader::height_all_t* p,
	size_t rows, size_t cols)
{
	CBlockHeightReader* ptr = (CBlockHeightReader*)CORE_ALLOC(
		sizeof(CBlockHeightReader));

	new (ptr) CBlockHeightReader(p, rows, cols);

	return ptr;
}

CBlockHeightReader::CBlockHeightReader(CHeightLoader::height_all_t* p,
	size_t rows, size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockHeightReader::~CBlockHeightReader()
{
}

void CBlockHeightReader::Release()
{
	CORE_DELETE(this);
}

int CBlockHeightReader::GetSize() const
{
	return 0;
}
	
int CBlockHeightReader::Read(void* handle)
{
	FILE* fp = (FILE*)handle;
	height_file_header_t header;
	
	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeightReader::Read)read header failed");
		return -1;
	}
	
	if ((header.nRows != m_nRows) || (header.nCols != m_nCols))
	{
		CORE_TRACE("(CBlockHeightReader::Read)scale error");
		return -1;
	}
	
	size_t count = header.nRows * header.nCols;
	float* values = (float*)CORE_ALLOC(sizeof(float) * count);
	
	if (core_file::fread(values, sizeof(float), count, fp) != count)
	{
		CORE_TRACE("(CBlockHeightReader::Read)read data failed");
		CORE_FREE(values, sizeof(float) * count);
		return -1;
	}
	
	m_pData->pValues = values;

	return sizeof(header) + (int)count * sizeof(float);
}

// CBlockHeightWriter

CBlockHeightWriter::CBlockHeightWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockHeightWriter::~CBlockHeightWriter()
{
}

int CBlockHeightWriter::GetSize() const
{
	return 0;
}

int CBlockHeightWriter::Write(void* handle)
{
	FILE* fp = (FILE*)handle;
	CTerrainHeight* pHeight = m_pZone->GetHeight();
	size_t rows = pHeight->GetRows();
	size_t cols = pHeight->GetCols();
	height_file_header_t header;
	
	header.nRows = (unsigned int)rows;
	header.nCols = (unsigned int)cols;
	
	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeightWriter::Write)write header failed");
		return -1;
	}
	
	const float* values = pHeight->GetHeightValues();
	size_t size1 = rows * cols;
	
	if (core_file::fwrite(values, sizeof(float), size1, fp) != size1)
	{
		CORE_TRACE("(CBlockHeightWriter::Write)write values failed");
		return -1;
	}
	
	return sizeof(header) + (int)size1 * sizeof(float);
}

