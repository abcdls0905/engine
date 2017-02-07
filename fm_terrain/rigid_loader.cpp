//--------------------------------------------------------------------
// �ļ���:		rigid_loader.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��8��
// ������:		��Ң˳
// ��Ȩ����:	������ţ�������޹�˾
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

	// �������ݼ������
	m_pOwner->SetRigidLoaded(true);
	
	return true;
}

bool CRigidLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}
#endif // PHYSICS_HAVOK_TERRAIN