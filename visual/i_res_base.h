//--------------------------------------------------------------------
// 文件名:		i_res_base.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_RES_BASE_H
#define _VISUAL_I_RES_BASE_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../utils/lock_util.h"
#include "../public/core_mem.h"

// 可以异步加载的资源基类

class IResBase
{
public:
	IResBase()
	{
		m_Refs.Set(1);
	}
	
	virtual ~IResBase() = 0;

	// 释放
	virtual void Release() = 0;
	

	// 设置名称
	void SetName(const char* value)
	{ 
		m_strName = value; 
	}

	// 获得名称
	const char* GetName() const
	{
		return m_strName.c_str();
	}
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
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strName;
	CLockVal m_Refs;
};

inline IResBase::~IResBase() {}

#endif // _VISUAL_I_RES_BASE_H
