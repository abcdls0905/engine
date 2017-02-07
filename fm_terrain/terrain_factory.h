//--------------------------------------------------------------------
// 文件名:		terrain_factory.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_FACTORY_H
#define _TERRAIN_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

class TTerrainFactoryAlloc
{
public:
	TTerrainFactoryAlloc() {}
	~TTerrainFactoryAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainFactoryAlloc& src) {}
};

// 资源工厂

class Terrain;
class CTerrainZone;

class CTerrainFactory
{
public:
	CTerrainFactory();
	~CTerrainFactory();

	// 创建资源
	CTerrainZone* Create(Terrain* pTerrain, size_t index, size_t row, 
		size_t col);
	// 销毁资源
	void Destroy(CTerrainZone* pResource);
	// 根据对象ID获得资源
	CTerrainZone* Get(const PERSISTID& id) const;
	// 全部删除
	void ReleaseAll();
	// 获得资源数量
	size_t GetCount() const { return m_nCount; }
	
private:
	CTerrainFactory(const CTerrainFactory&);
	CTerrainFactory& operator=(const CTerrainFactory&);

	// 生成序列号
	unsigned int GenSerial();

private:
	TArrayPod<CTerrainZone*, 1, TTerrainFactoryAlloc> m_Resources;
	TArrayPod<unsigned int, 1, TTerrainFactoryAlloc> m_FreeList;
	unsigned int m_nSerial;
	size_t m_nCount;
};

#endif // _TERRAIN_FACTORY_H
