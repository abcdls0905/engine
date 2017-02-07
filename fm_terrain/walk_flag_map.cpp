//--------------------------------------------------------------------
// 文件名:		walk_flag_map.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年9月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "walk_flag_map.h"
#include "walk_flag_compress.h"
#include "dynamic_walk_flag_compress.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// CWalkFlagMap

CWalkFlagMap::CWalkFlagMap()
{
	m_nRows = 0;
	m_nCols = 0;
	m_pValues = NULL;
	m_pCompressData = NULL;
	m_nCompressSize = 0;
}

CWalkFlagMap::~CWalkFlagMap()
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, m_nRows * m_nCols);
	}

	if (m_pCompressData)
	{
		CORE_FREE(m_pCompressData, m_nCompressSize);
	}
}

bool CWalkFlagMap::BuildCompress(size_t rows, size_t cols, 
	unsigned char* pdata, size_t size)
{
	Assert(pdata != NULL);

	if (m_pCompressData)
	{
		CORE_FREE(m_pCompressData, m_nCompressSize);
	}

	m_nRows = rows;
	m_nCols = cols;
	m_pCompressData = pdata;
	m_nCompressSize = size;

	return true;
}

bool CWalkFlagMap::Create(size_t rows, size_t cols, 
	const unsigned char* pValues)
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, m_nRows * m_nCols);
	}

	m_nRows = rows;
	m_nCols = cols;
	m_pValues = (unsigned char*)CORE_ALLOC(rows * cols);

	if (pValues)
	{
		memcpy(m_pValues, pValues, rows * cols * sizeof(unsigned char));
	}
	else
	{
		memset(m_pValues, 0, rows * cols * sizeof(unsigned char));
	}

	return true;
}

bool CWalkFlagMap::SetValue(size_t row, size_t col, unsigned int value)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	Assert(m_pValues != NULL);

	m_pValues[row * m_nCols + col] = value;

	return true;
}

unsigned int CWalkFlagMap::GetValue(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	
	if (m_pValues)
	{
		return m_pValues[row * m_nCols + col];
	}
	else if (m_pCompressData)
	{
		unsigned char value;

		if (!CWalkFlagCompress::GetFlag(m_nRows, row, col, m_pCompressData,
			m_nCompressSize, value))
		{
			return 0;
		}

		return value;
	}
	else
	{
		return 0;
	}
}

unsigned int CWalkFlagMap::GetDynamicValue(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	if (m_pValues)
	{
		return m_pValues[row * m_nCols + col];
	}
	else if (m_pCompressData)
	{
		unsigned char value;

		if (!CDynamicWalkFlagCompress::GetFlag(m_nRows, row, col, m_pCompressData,
			m_nCompressSize, value))
		{
			return 0;
		}

		return value;
	}
	else
	{
		return 0;
	}
}

bool CWalkFlagMap::GetIsNull() const
{
	if (m_pValues)
	{
		size_t size = m_nRows * m_nCols;

		for (size_t i = 0; i < size; ++i)
		{
			if (m_pValues[i] != 0)
			{
				return false;
			}
		}
	}
	else if (m_pCompressData)
	{
	}

	return true;
}

