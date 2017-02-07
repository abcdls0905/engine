//--------------------------------------------------------------------
// 文件名:		tex_alpha_map.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_alpha_map.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

// CTexAlphaMap

CTexAlphaMap::CTexAlphaMap()
{
	m_nScale = 16;
	m_pAlpha = NULL;
	m_nSize = 0;
}

CTexAlphaMap::~CTexAlphaMap()
{
	ReleaseAlpha();
}

void CTexAlphaMap::CreateAlpha()
{
	if (m_pAlpha == NULL)
	{
		m_nSize = m_nScale * m_nScale / 2;
		m_pAlpha = (unsigned char*)CORE_ALLOC(m_nSize);
		memset(m_pAlpha, 0, m_nSize);
	}
}

void CTexAlphaMap::ReleaseAlpha()
{
	if (m_pAlpha)
	{
		CORE_FREE(m_pAlpha, m_nSize);
		m_pAlpha = NULL;
	}
}

bool CTexAlphaMap::SetAlpha(size_t row, size_t col, size_t value)
{
	Assert(row < m_nScale);
	Assert(col < m_nScale);

	size_t pos = row * m_nScale + col;
	size_t index = pos >> 1;

	if ((pos & 0x1) == 0)
	{
		m_pAlpha[index] = 
			(m_pAlpha[index] & 0xF) + (((unsigned char)value & 0xF) << 4);
	}
	else
	{
		m_pAlpha[index] = 
			(m_pAlpha[index] & 0xF0) + ((unsigned char)value & 0xF);
	}
	
	return true;
}

size_t CTexAlphaMap::GetAlpha(size_t row, size_t col) const
{
	Assert(row < m_nScale);
	Assert(col < m_nScale);

	size_t pos = row * m_nScale + col;
	size_t index = pos >> 1;

	if ((pos & 0x1) == 0)
	{
		return (m_pAlpha[index] >> 4) & 0xF;
	}
	else
	{
		return m_pAlpha[index] & 0xF;
	}
}

bool CTexAlphaMap::IsEmpty() const
{
	for (size_t i = 0; i < m_nSize; ++i)
	{
		if (m_pAlpha[i] != 0)
		{
			return false;
		}
	}

	return true;
}

bool CTexAlphaMap::WriteMapData(const unsigned char* data, size_t size)
{
	Assert(data != NULL);
	Assert(size == (m_nScale * m_nScale / 2));
	
	// 保存贴图数据
	CreateAlpha();
	memcpy(m_pAlpha, data, size);

	return true;
}
