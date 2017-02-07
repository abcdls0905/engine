//--------------------------------------------------------------------
// 文件名:		terrain_rigid.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月8日
// 创建人:		柳尧顺
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_RIGID_H
#define _TERRAIN_RIGID_H

// 调试物理引擎需要的头文件
//#include "../physics/physics_macros.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "../public/macros.h"
#include "../visual/dx_header.h"

// 刚体地形

class Terrain;
class CTerrainZone;
class IPhysicsWorld;
class ICollisionTerrain;

class CTerrainRigid
{
public:
	CTerrainRigid(Terrain* pTerrain, CTerrainZone* pZone, int row, int col);
	~CTerrainRigid();

	// 创建刚体地形
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
	size_t m_nRow;		// 当前ZONE行
	size_t m_nCol;		// 当前ZONE列
};
#endif // PHYSICS_HAVOK_TERRAIN

#endif // _TERRAIN_RIGID_H
