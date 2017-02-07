//--------------------------------------------------------------------
// �ļ���:		terrain_rigid.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��8��
// ������:		��Ң˳
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "terrain_rigid.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "terrain.h"
#include "terrain_zone.h"
#include "../visual/i_dx_context.h"
//#include "../physics/havok/i_collision_model.h"

// CTerrainRigid

CTerrainRigid::CTerrainRigid(Terrain* pTerrain, CTerrainZone* pZone, int row, 
	int col)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_pPhysicsWorld = 
		(IPhysicsWorld*)pTerrain->GetContext()->GetUnique("CPhysicsWorld");
	m_pCollisionTerrain = NULL;
	m_nRow = row;
	m_nCol = col;
}

CTerrainRigid::~CTerrainRigid()
{
	ReleaseAll();
}

void CTerrainRigid::ReleaseAll()
{
	// ����Ҫ����������Դ���ͷţ�����ϵͳ�Լ�����
	//if (m_pCollisionTerrain)
	//{
	//	ICore* pCore = m_pTerrain->GetCore();

	//	Assert(NULL != pCore);

	//	pCore->DeleteEntity(m_pCollisionTerrain->GetID());

	//	m_pCollisionTerrain = NULL;
	//}

	//SAFE_RELEASE(m_pPhysicsWorld);
}

bool CTerrainRigid::Build(float* heights)
{
	// ����ָ������ĸ������
	//bool bToBuild = false;

	//for (int i = 1; i < 2; ++i)
	//{
	//	for (int j = -i; j <= i; ++j)
	//	{
	//		for (int k = -i; k <= i; ++k)
	//		{
	//			if ((abs(50 + j) == m_nRow) && (abs(50 + k) == m_nCol))
	//			{
	//				bToBuild = true;
	//				break;
	//			}
	//		}
	//	}
	//}

	//if (!bToBuild)
	//{
	//	return true;
	//}

	Assert(heights != NULL);

	if (NULL == heights)
	{
		return false;
	}

	if (NULL == m_pPhysicsWorld)
	{
		return true;
	}

	// ���β���
	size_t zone_scale = m_pTerrain->GetZoneScale();
	float unit_size = m_pTerrain->GetUnitSize();
	int zone_origin_col = (int)m_pTerrain->GetZoneOriginCol();
	int zone_origin_row = (int)m_pTerrain->GetZoneOriginRow();

	// ����������
	size_t rows = size_t((zone_scale + 1) * unit_size);
	size_t cols = rows;

	// ���ĵ�λ��
	float x = zone_scale * unit_size * ((int)m_nCol - zone_origin_col);
	float z = zone_scale * unit_size * ((int)m_nRow - zone_origin_row);

	// y��ʾ����̧�߻򽵵͵ĸ߶�
	float y = 0.0F;

	ICore* pCore = m_pTerrain->GetCore();

	Assert(NULL != pCore);

	// �����������
	PERSISTID id = m_pPhysicsWorld->CreateTerrain();

	m_pCollisionTerrain = (ICollisionTerrain*)pCore->GetEntity(id);

	if (NULL == m_pCollisionTerrain)
	{
		return false;
	}

	return m_pCollisionTerrain->BuildTerrain(x, y, z, rows, cols, heights);
}
#endif // PHYSICS_HAVOK_TERRAIN