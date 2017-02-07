//--------------------------------------------------------------------
// �ļ���:		terrain_rigid.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��8��
// ������:		��Ң˳
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_RIGID_H
#define _TERRAIN_RIGID_H

// ��������������Ҫ��ͷ�ļ�
//#include "../physics/physics_macros.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "../public/macros.h"
#include "../visual/dx_header.h"

// �������

class Terrain;
class CTerrainZone;
class IPhysicsWorld;
class ICollisionTerrain;

class CTerrainRigid
{
public:
	CTerrainRigid(Terrain* pTerrain, CTerrainZone* pZone, int row, int col);
	~CTerrainRigid();

	// �����������
	bool Build(float* heights);
	
private:
	CTerrainRigid();
	CTerrainRigid(const CTerrainRigid&);
	CTerrainRigid& operator=(const CTerrainRigid&);

	void ReleaseAll();

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	IPhysicsWorld* m_pPhysicsWorld;
	ICollisionTerrain* m_pCollisionTerrain;
	size_t m_nRow;		// ��ǰZONE��
	size_t m_nCol;		// ��ǰZONE��
};
#endif // PHYSICS_HAVOK_TERRAIN

#endif // _TERRAIN_RIGID_H
