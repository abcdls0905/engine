//--------------------------------------------------------------------
// 文件名:		DynamicWalkableManager.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月1日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKABLEMANAGER_H
#define _DYNAMICWALKABLEMANAGER_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../utils/array_pod.h"

class CDynamicWalkable;
class IVisBase;
class Terrain;

class CDynamicWalkableManager
{
private:
	typedef TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> FileName;
public:
	CDynamicWalkableManager();
	~CDynamicWalkableManager();

	void SetTerrain(Terrain* pTerrain);

	// 获取模型名
	result_string GetVisualName(IVisBase* pVisBase) const;
	// 获取模型扩展名
	result_string GetVisualExtName(IVisBase* pVisBase) const;

	// 添加动态物件
	bool AddDynamicWalkable(IVisBase* pVisBase);
	// 添加动态物件（不加载)
	bool AddDynamicWalkable2(IVisBase* pVisBase);
	// 删除动态物件
	bool DeleteDynamicWalkable(IVisBase* pVisBase);

	// 加载动态物件碰撞
	bool LoadDynamicWalkable(IVisBase* pVisBase);
	// 卸载动态物件碰撞
	bool UnloadDynamicWalkable(IVisBase* pVisBase);

	// 获取物件可行走信息
	CDynamicWalkable* GetDynamicWalkable(IVisBase* pVisBase) const;
private:
	Terrain* m_pTerrain;
	TArrayPod<CDynamicWalkable*, 1, TCoreAlloc> m_VisualWalkableArr;
};

#endif // _DYNAMICWALKABLEMANAGER_H
