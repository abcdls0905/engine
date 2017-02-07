//--------------------------------------------------------------------
// �ļ���:		i_int_creator.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_INT_CREATOR_H
#define _PUBLIC_I_INT_CREATOR_H

#include "macros.h"

// ���ܽӿڴ�����

class IInterface;

class IIntCreator
{
public:
	//IIntCreator(IIntCreator* pNext) { m_pNext = pNext; }
	IIntCreator() { m_pNext = NULL; }
	virtual ~IIntCreator() = 0;

	// ��������
	virtual const char* GetName() const = 0;
	// ����
	virtual IInterface* Create() = 0;
	// ɾ��
	virtual void Destroy(IInterface* p) = 0;
	
	// ������һ��
	void SetNext(IIntCreator* value) { m_pNext = value; }
	// �����һ��
	IIntCreator* GetNext() const { return m_pNext; }

//private:
//	IIntCreator();

private:
	IIntCreator* m_pNext;
};

inline IIntCreator::~IIntCreator() {}

#endif // _PUBLIC_I_INT_CREATOR_H
