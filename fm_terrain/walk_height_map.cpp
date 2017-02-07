//--------------------------------------------------------------------
// 文件名:		walk_height_map.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月13日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "walk_height_map.h"
#include "walk_height_compress.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// 无效高度值
#ifndef HEIGHT_NULL
#define HEIGHT_NULL 1e8
#endif

TArrayPod<float, 1> g_pHeightNullValues;


// CWalkHeightMap

CWalkHeightMap::CWalkHeightMap()
{
	m_nRows = 0;
	m_nCols = 0;
	m_pValues = NULL;
	m_pCompressData = NULL;
	m_nCompressSize = 0;
}

CWalkHeightMap::~CWalkHeightMap()
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, sizeof(float) * m_nRows * m_nCols);
	}

	if (m_pCompressData)
	{
		CORE_FREE(m_pCompressData, m_nCompressSize);
	}

	g_pHeightNullValues.resize(0);
}

bool CWalkHeightMap::BuildCompress(size_t rows, size_t cols,
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


	// 初始化不合法的高度值数组，为优化reset value函数实现做准备
	if (m_nRows * m_nCols > g_pHeightNullValues.size())
	{
		g_pHeightNullValues.resize(m_nRows * m_nCols, HEIGHT_NULL);
	}

	return true;
}

bool CWalkHeightMap::Create(size_t rows, size_t cols, float* pValues)
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, sizeof(float) * m_nRows * m_nCols);
	}

	m_nRows = rows;
	m_nCols = cols;
	m_pValues = (float*)CORE_ALLOC(sizeof(float) * rows * cols);

	if (pValues)
	{
		memcpy(m_pValues, pValues, rows * cols * sizeof(float));
	}
	
	return true;
}

float CWalkHeightMap::GetValue(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	
	if (m_pValues)
	{
		return m_pValues[row * m_nCols + col];
	}
	else if (m_pCompressData)
	{
		float value;

		if (!CWalkHeightCompress::GetHeight(m_nRows, row, col, 
			m_pCompressData, m_nCompressSize, value))
		{
			return 0.0F;
		}

		return value;
	}
	else
	{
		return 0.0F;
	}
}

// 将高度设置为统一值
void CWalkHeightMap::ResetValue()
{
    size_t count = m_nCols * m_nRows;
	memcpy(&m_pValues[0], &g_pHeightNullValues[0], count * 4);
}

