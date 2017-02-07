//--------------------------------------------------------------------
// �ļ���:		light_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_SET_H
#define _LIGHT_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../visual/dx_header.h"
#include "light_types.h"

// ��Դ����

class LightInfo;

class CLightSet  
{
public:
	CLightSet();
	~CLightSet();

	// ����̫���ⷽ��
	bool SetSunDirection(const FmVec4& dir);
	// ���ӹ�Դ
	bool AddLight(LightInfo* info);
	// ���¹�Դ
	bool SetLight(LightInfo* info);

	// ��ù�Դ����
	int GetCount() const
	{
		return m_Lights.size();
	}

	// ���ָ��������Դ
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