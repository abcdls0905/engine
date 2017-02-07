//--------------------------------------------------------------------
// 文件名:		CacheLoader.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年9月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CACHELOADER_H
#define _CACHELOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../visual/i_res_loader.h"

// 缓冲文件加载器

class CCacheFile;

class CCacheLoader: public IResLoader
{
public:
	static CCacheLoader* NewInstance();

public:
	CCacheLoader();
	virtual ~CCacheLoader();

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
	
	// 所有者
	void SetOwner(CCacheFile* pOwner);
	// 文件名
	void SetFileName(const char* file_name);

private:
	CCacheLoader(const CCacheLoader&);
	CCacheLoader& operator=(const CCacheLoader&);
	
private:
	CCacheFile* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	unsigned char* m_pData;
	size_t m_nSize;
};

#endif // _CACHELOADER_H
