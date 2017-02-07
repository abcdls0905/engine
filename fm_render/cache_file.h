//--------------------------------------------------------------------
// 文件名:		CacheFile.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年9月19日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CACHEFILE_H
#define _CACHEFILE_H

#include "../public/macros.h"
#include "../visual/i_res_base.h"
#include "../visual/i_res_loader.h"

// 文件缓冲

class IResLoader;

class CCacheFile: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// 初始状态
		STATE_LOADING,	// 正在加载
		STATE_READY,	// 就绪
		STATE_FAILED,	// 加载失败
	};

	static CCacheFile* NewInstace();

public:
	CCacheFile();
	virtual ~CCacheFile();

	// 同步创建
	bool Create();
	// 开始异步创建 
	bool BeginCreate();
	// 结束异步创建 
	bool EndCreate(int category, bool succeed);

	// 是否就绪
	bool IsReady()
	{
		return (m_nState == STATE_READY);
	}

	// 是否加载结束（成功或失败）
	bool IsLoadComplete()
	{
		return (m_nState == STATE_READY) || (m_nState == STATE_FAILED);
	}

	// 释放
	virtual void Release();

	// 获得加载器
	IResLoader* GetLoader() const;

	// 创建完成
	bool Build(unsigned char* pdata, size_t size);
	
	// 获得文件数据
	unsigned char* GetData() const { return m_pData; }
	// 获得文件长度
	size_t GetSize() const { return m_nSize; }

private:
	CCacheFile(const CCacheFile&);
	CCacheFile& operator=(const CCacheFile&);

private:
	IResLoader* m_pLoader;
	int m_nState;
	unsigned char* m_pData;
	size_t m_nSize;
};

#endif // _CACHEFILE_H

