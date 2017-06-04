
#ifndef _VISUAL_I_RES_LOADER_H
#define _VISUAL_I_RES_LOADER_H

#include "../public/macros.h"
#include "../utils/lock_util.h"

// 资源加载器接口

class IResLoader
{
public:
	IResLoader()
	{
		m_Refs.Set(1);
	}
	
	virtual ~IResLoader() = 0;

	// 释放
	virtual void Release() = 0;
	// 获得名字
	virtual const char* GetName() = 0;
	// 加载资源数据
	virtual bool Load(bool async) = 0;
	// 创建资源
	virtual bool Create(bool async) = 0;
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed) = 0;
	
	// 增加引用
	int IncRefs()
	{
		return m_Refs.Inc();
	}

	// 减少引用
	int DecRefs()
	{
		return m_Refs.Dec();
	}

	// 获得引用数
	int GetRefs() const
	{
		return m_Refs.Get();
	}

private:
	CLockVal m_Refs;
};

inline IResLoader::~IResLoader() {}

#endif // _VISUAL_I_RES_LOADER_H
