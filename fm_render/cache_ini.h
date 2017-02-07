//--------------------------------------------------------------------
// 文件名:		CacheIni.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年4月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CACHEINI_H
#define _CACHEINI_H

#include "../public/macros.h"
#include "../visual/i_res_base.h"

// 配置文件缓冲

class IResLoader;
//class CReadIni;
class IIniFile;

class CCacheIni: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// 初始状态
		STATE_LOADING,	// 正在加载
		STATE_READY,	// 就绪
		STATE_FAILED,	// 加载失败
	};

	static CCacheIni* NewInstace();

public:
	CCacheIni();
	virtual ~CCacheIni();

	// 同步创建
	bool Create(bool create_hash);
	// 开始异步创建 
	bool BeginCreate(bool create_hash);
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
	bool Build(IIniFile* pIni);
	
	// 获得文件数据
	IIniFile* GetIni() const { return m_pIni; }

private:
	CCacheIni(const CCacheIni&);
	CCacheIni& operator=(const CCacheIni&);

private:
	IResLoader* m_pLoader;
	int m_nState;
	IIniFile* m_pIni;
};

#endif // _CACHEINI_H

