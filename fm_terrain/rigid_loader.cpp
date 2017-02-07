//--------------------------------------------------------------------
// 文件名:		rigid_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月8日
// 创建人:		柳尧顺
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "rigid_loader.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "terrain_zone.h"
#include "terrain_rigid.h"
#include "terrain.h"
#include "../public/core_mem.h"

// CRigidLoader

CRigidLoader* CRigidLoader::NewInstance()
{
	return CORE_NEW(CRigidLoader);
}

CRigidLoader::CRigidLoader()
{
	m_pOwner = NULL;
	m_pHeights = NULL;
}

CRigidLoader::~CRigidLoader()
{
	m_pHeights = NULL;
}

void CRigidLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CRigidLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);
	
	m_pOwner = pOwner;
}

void CRigidLoader::SetHeights(float* heights)
{
	Assert(heights != NULL);

	m_pHeights = heights;
}

const char* CRigidLoader::GetName()
{
	return "TerrainRigid";
}

bool CRigidLoader::Load(bool async)
{
	return true;
}

bool CRigidLoader::Create(bool async)
{
	CTerrainRigid* pRigid = m_pOwner->GetRigid();

	Assert(pRigid != NULL);

	if (!pRigid->Build(m_pHeights))
	{
		return false;
	}

	// 设置数据加载完成
	m_pOwner->SetRigidLoaded(true);
	
	return true;
}

bool CRigidLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
#endif // PHYSICS_HAVOK_TERRAIN