
#ifndef _VISUAL_I_RES_LOADER_H
#define _VISUAL_I_RES_LOADER_H

#include "../public/macros.h"
#include "../utils/lock_util.h"

// ��Դ�������ӿ�

class IResLoader
{
public:
	IResLoader()
	{
		m_Refs.Set(1);
	}
	
	virtual ~IResLoader() = 0;

	// �ͷ�
	virtual void Release() = 0;
	// �������
	virtual const char* GetName() = 0;
	// ������Դ����
	virtual bool Load(bool async) = 0;
	// ������Դ
	virtual bool Create(bool async) = 0;
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed) = 0;
	
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
	CLockVal m_Refs;
};

inline IResLoader::~IResLoader() {}

#endif // _VISUAL_I_RES_LOADER_H
