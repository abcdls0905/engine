//--------------------------------------------------------------------
// 文件名:		block_height2.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_height2.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "height_compress.h"
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

// CBlockHeight2Reader

IBlockReader* CBlockHeight2Reader::NewInstance(CHeightLoader::height_all_t* p,
	size_t rows, size_t cols)
{
	CBlockHeight2Reader* ptr = (CBlockHeight2Reader*)CORE_ALLOC(
		sizeof(CBlockHeight2Reader));
	
	new (ptr) CBlockHeight2Reader(p, rows, cols);

	return ptr;
}

CBlockHeight2Reader::CBlockHeight2Reader(CHeightLoader::height_all_t* p,
	size_t rows, size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockHeight2Reader::~CBlockHeight2Reader()
{
}

void CBlockHeight2Reader::Release()
{
	CORE_DELETE(this);
}

int CBlockHeight2Reader::GetSize() const
{
	return 0;
}
	
int CBlockHeight2Reader::Read(void* handle)
{
	int read_size = 0;
	FILE* fp = (FILE*)handle;
	height_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)read header failed");
		return -1;
	}

	read_size += sizeof(header);

	if ((header.nRows != m_nRows) || (header.nCols != m_nCols))
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)scale error");
		return -1;
	}

	unsigned int data_size;

	if (core_file::fread(&data_size, sizeof(data_size), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)read compress size failed");
		return -1;
	}

	read_size += sizeof(data_size);

	if (data_size > 0x100000)
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)compress size error");
		return -1;
	}

	unsigned char* pdata = (unsigned char*)CORE_ALLOC(data_size);

	if (core_file::fread(pdata, sizeof(unsigned char), data_size, fp) 
		!= data_size)
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)read compress data error");
		CORE_FREE(pdata, data_size);
		return -1;
	}

	read_size += data_size;

	CHeightCompress compress;

	if (!compress.Decompress(m_nRows, pdata, data_size))
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)decompress failed");
		CORE_FREE(pdata, data_size);
		return -1;
	}

	CORE_FREE(pdata, data_size);

	size_t count = header.nRows * header.nCols;

	if (compress.GetResultSize() != (count * sizeof(float)))
	{
		CORE_TRACE("(CBlockHeight2Reader::Read)decompress size error");
		return -1;
	}

	float* values = (float*)CORE_ALLOC(sizeof(float) * count);

	memcpy(values, compress.GetResultData(), compress.GetResultSize());
	m_pData->pValues = values;

	return read_size;
}

// CBlockHeight2Writer

CBlockHeight2Writer::CBlockHeight2Writer(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockHeight2Writer::~CBlockHeight2Writer()
{
}

int CBlockHeight2Writer::GetSize() const
{
	return 0;
}

int CBlockHeight2Writer::Write(void* handle)
{
	int write_size = 0;
	FILE* fp = (FILE*)handle;
	CTerrainHeight* pHeight = m_pZone->GetHeight();
	size_t rows = pHeight->GetRows();
	size_t cols = pHeight->GetCols();
	height_file_header_t header;

	header.nRows = (unsigned int)rows;
	header.nCols = (unsigned int)cols;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write header failed");
		return -1;
	}

	write_size += sizeof(header);

	const float* values = pHeight->GetHeightValues();
	size_t count = rows * cols;
	CHeightCompress compress;

	if (!compress.Compress(rows, values, count))
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)compress failed");
		return -1;
	}

	unsigned int data_size = (unsigned int)compress.GetResultSize();

	if (core_file::fwrite(&data_size, sizeof(data_size), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write compress size failed");
		return -1;
	}

	write_size += sizeof(data_size);

	if (core_file::fwrite(compress.GetResultData(), sizeof(unsigned char), 
		data_size, fp) != data_size)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write compress data failed");
		return -1;
	}

	write_size += data_size;

	return write_size;
}

// CBlockHeight2Writer2

CBlockHeight2Writer2::CBlockHeight2Writer2(CHeightLoader::height_all_t* p, 
	size_t rows, size_t cols)
{
	Assert(p != NULL);

	m_pData = p;
	m_nRows = rows;
	m_nCols = cols;
}

CBlockHeight2Writer2::~CBlockHeight2Writer2()
{
}

int CBlockHeight2Writer2::GetSize() const
{
	return 0;
}

int CBlockHeight2Writer2::Write(void* handle)
{
	int write_size = 0;
	FILE* fp = (FILE*)handle;
	height_file_header_t header;

	header.nRows = (unsigned int)m_nRows;
	header.nCols = (unsigned int)m_nCols;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write header failed");
		return -1;
	}

	write_size += sizeof(header);

	const float* values = m_pData->pValues;
	size_t count = m_nRows * m_nCols;
	CHeightCompress compress;

	if (!compress.Compress(m_nRows, values, count))
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)compress failed");
		return -1;
	}

	unsigned int data_size = (unsigned int)compress.GetResultSize();

	if (core_file::fwrite(&data_size, sizeof(data_size), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write compress size failed");
		return -1;
	}

	write_size += sizeof(data_size);

	if (core_file::fwrite(compress.GetResultData(), sizeof(unsigned char), 
		data_size, fp) != data_size)
	{
		CORE_TRACE("(CBlockHeight2Writer::Write)write compress data failed");
		return -1;
	}

	write_size += data_size;

	return write_size;
}

