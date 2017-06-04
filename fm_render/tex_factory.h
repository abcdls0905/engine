
#ifndef _TEX_FACTORY_H
#define _TEX_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../public/i_var_list.h"
#include "../utils/array_pod.h"


class TTexFactoryAlloc
{
public:
	TTexFactoryAlloc() {}
	~TTexFactoryAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTexFactoryAlloc& src) {}
};

// 贴图资源工厂

class Render;
class CTexture;
class IStaticTex;

class CTexFactory
{
public:
	static CTexFactory* NewInstance(Render* pRender);

public:
	CTexFactory(Render* pRender);
	~CTexFactory();

	// 释放
	void Release();
	// 创建贴图资源
	CTexture* Create();
	// 销毁资源
	void Destroy(CTexture* pResource);
	// 根据对象ID获得资源
	CTexture* Get(const PERSISTID& id) const;
	// 全部删除
	void ReleaseAll();
	// 导出数据
	bool Dump(const char* filename);

	// 获得数量
	size_t GetCount() const { return m_nCount; }

private:
	CTexFactory();
	CTexFactory(const CTexFactory&);
	CTexFactory& operator=(const CTexFactory&);

	// 生成序列号
	unsigned int GenSerial();

	// 默认一个临时可用纹理(模拟D3D采样空纹理方式)
    IStaticTex* CreateTempStaticTex();
private:
	Render* m_pRender;
	IStaticTex* m_pTempTex;
	TArrayPod<CTexture*, 1, TTexFactoryAlloc> m_Resources;
	TArrayPod<unsigned int, 1, TTexFactoryAlloc> m_FreeList;
	unsigned int m_nSerial;
	size_t m_nCount;
};

#endif // _TEX_FACTORY_H
