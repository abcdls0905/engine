//--------------------------------------------------------------------
// �ļ���:		i_res_base.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_RES_BASE_H
#define _VISUAL_I_RES_BASE_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../utils/lock_util.h"
#include "../public/core_mem.h"

// �����첽���ص���Դ����

class IResBase
{
public:
	IResBase()
	{
		m_Refs.Set(1);
	}
	
	virtual ~IResBase() = 0;

	// �ͷ�
	virtual void Release() = 0;
	

	// ��������
	void SetName(const char* value)
	{ 
		m_strName = value; 
	}

	// �������
	const char* GetName() const
	{
		return m_strName.c_str();
	}
	// ��������
	int IncRefs()
	{
		return m_Refs.Inc();
	}

	// ��������
	int DecRefs()
	{
		return m_Refs.Dec();
	}

	// ���������
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
