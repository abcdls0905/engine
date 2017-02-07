//--------------------------------------------------------------------
// 文件名:		terrain_factory.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_factory.h"
#include "terrain_zone.h"
#include "../public/core_mem.h"

// CTerrainFactory

CTerrainFactory::CTerrainFactory()
{
	m_nSerial = 0;
	m_nCount = 0;
	m_Resources.push_back(NULL);	// 不使用位置0
}

CTerrainFactory::~CTerrainFactory()
{
	ReleaseAll();
}

void CTerrainFactory::ReleaseAll()
{
	for (size_t i = 0; i < m_Resources.size(); ++i)
	{
		if (m_Resources[i] != NULL)
		{
			Destroy(m_Resources[i]);
		}
	}
	
	m_Resources.clear();
	m_Resources.push_back(NULL);
	m_FreeList.clear();
	m_nCount = 0;
}

unsigned int CTerrainFactory::GenSerial()
{
	if (++m_nSerial == 0)
	{
		return ++m_nSerial & 0x7FFFFFFF;
	}
	else
	{
		return m_nSerial & 0x7FFFFFFF;
	}
}

CTerrainZone* CTerrainFactory::Create(Terrain* pTerrain, size_t index, 
	size_t row, size_t col)
{
	CTerrainZone* pZone = (CTerrainZone*)CORE_ALLOC(sizeof(CTerrainZone));
	
	new (pZone) CTerrainZone(pTerrain, this, index, row, col);

	unsigned int serial = GenSerial();
	unsigned int ident;

	if (m_FreeList.empty())
	{
		ident = (unsigned int)m_Resources.size();
		m_Resources.push_back(pZone);
	}
	else
	{
		ident = m_FreeList.back();
		m_FreeList.pop_back();
		m_Resources[ident] = pZone;
	}
	
	++m_nCount;
	pZone->SetID(PERSISTID(ident, serial));
	
	return pZone;
}

void CTerrainFactory::Destroy(CTerrainZone* pResource)
{
	Assert(pResource != NULL);

	size_t index = pResource->GetID().nIdent;
	
	Assert(index < m_Resources.size());
	Assert(m_Resources[index] == pResource);

	CORE_DELETE(pResource);
	m_Resources[index] = NULL;
	m_FreeList.push_back(index);
	--m_nCount;
}

CTerrainZone* CTerrainFactory::Get(const PERSISTID& id) const
{
	size_t index = id.nIdent;

	if (index >= m_Resources.size())
	{
		return NULL;
	}
	
	CTerrainZone* pResource = m_Resources[index];

	if (NULL == pResource)
	{
		return NULL;
	}

	if (pResource->GetID() != id)
	{
		return NULL;
	}

	return pResource;
}
