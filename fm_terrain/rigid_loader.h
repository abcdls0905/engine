//--------------------------------------------------------------------
// 文件名:		rigid_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年4月8日
// 创建人:		柳尧顺
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RIGID_LOADER_H
#define _RIGID_LOADER_H

// 调试物理引擎需要的头文件
//#include "../physics/physics_macros.h"

#ifdef PHYSICS_HAVOK_TERRAIN

#include "../public/macros.h"
#include "../visual/dx_header.h"
#include "../visual/i_res_loader.h"

// 地形刚体加载器

class CTerrainZone;

class CRigidLoader: public IResLoader
{
public:
	static CRigidLoader* NewInstance();

public:
	CRigidLoader();
	virtual ~CRigidLoader();

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);
	
	// 资源主体
	void SetOwner(CTerrainZone* pOwner);
	// 记录高度信息
	void SetHeights(float* heights);
	
private:
	CRigidLoader(const CRigidLoader&);
	CRigidLoader& operator=(const CRigidLoader&);

private:
	CTerrainZone* m_pOwner;
	float* m_pHeights;
};
#endif // PHYSICS_HAVOK_TERRAIN

#endif // _RIGID_LOADER_H
