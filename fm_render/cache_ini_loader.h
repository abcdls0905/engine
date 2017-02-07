//--------------------------------------------------------------------
// 文件名:		CacheIniLoader.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年4月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CACHEINILOADER_H
#define _CACHEINILOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../visual/i_res_loader.h"

// 缓冲配置文件加载器

class CCacheIni;
class IIniFile;

class CCacheIniLoader: public IResLoader
{
public:
	static CCacheIniLoader* NewInstance();

public:
	CCacheIniLoader();
	virtual ~CCacheIniLoader();

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
	void SetOwner(CCacheIni* pOwner);
	// 文件名
	void SetFileName(const char* file_name);
	// 是否创建哈希索引
	void SetCreateHash(bool value);

private:
	CCacheIniLoader(const CCacheIniLoader&);
	CCacheIniLoader& operator=(const CCacheIniLoader&);
	
private:
	CCacheIni* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	bool m_bCreateHash;
	IIniFile* m_pIni;
};

#endif // _CACHEINILOADER_H
