//--------------------------------------------------------------------
// �ļ���:		WalkIDMap.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��21��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "walk_i_d_map.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"

// CWalkIDMap

CWalkIDMap::CWalkIDMap()
{
	m_nRows = 0;
	m_nCols = 0;
	m_pValues = NULL;
}

CWalkIDMap::~CWalkIDMap()
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, sizeof(PERSISTID) * m_nRows * m_nCols);
	}
}

bool CWalkIDMap::Create(size_t rows, size_t cols, PERSISTID* pValues)
{
	if (m_pValues)
	{
		CORE_FREE(m_pValues, sizeof(PERSISTID) * m_nRows * m_nCols);
	}

	m_nRows = rows;
	m_nCols = cols;
	m_pValues = (PERSISTID*)CORE_ALLOC(sizeof(PERSISTID) * rows * cols);

	if (pValues)
	{
		memcpy(m_pValues, pValues, rows * cols * sizeof(PERSISTID));
	}
	else
	{
		memset(m_pValues, 0, sizeof(PERSISTID) * rows * cols);
	}

	return true;
}

bool CWalkIDMap::SetValue(size_t row, size_t col, const PERSISTID& value)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);
	Assert(m_pValues != NULL);

	m_pValues[row * m_nCols + col] = value;

	return true;
}

PERSISTID CWalkIDMap::GetValue(size_t row, size_t col)
{
	Assert(row < m_nRows);
	Assert(col < m_nCols);

	if (m_pValues)
	{
		return m_pValues[row * m_nCols + col];
	}
	else
	{
		return PERSISTID();
	}
}
