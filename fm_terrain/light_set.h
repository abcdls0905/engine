//--------------------------------------------------------------------
// 文件名:		light_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_SET_H
#define _LIGHT_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../visual/dx_header.h"
#include "light_types.h"

// 光源集合

class LightInfo;

class CLightSet  
{
public:
	CLightSet();
	~CLightSet();

	// 设置太阳光方向
	bool SetSunDirection(const FmVec4& dir);
	// 增加光源
	bool AddLight(LightInfo* info);
	// 更新光源
	bool SetLight(LightInfo* info);

	// 获得光源数量
	int GetCount() const
	{
		return m_Lights.size();
	}

	// 获得指定索引光源
	light_t* GetLight(size_t index) 
	{
		Assert(index < m_Lights.size());
	
		return m_Lights[index];
	}
	
private:
	void SetDefLightParam(int i);

private:
	TArrayPod<light_t*, 1, TCoreAlloc> m_Lights;
};

#endif // _LIGHT_SET_H
*/