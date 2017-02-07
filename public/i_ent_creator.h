//--------------------------------------------------------------------
// �ļ���:		i_ent_creator.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_ENT_CREATOR_H
#define _PUBLIC_I_ENT_CREATOR_H

#include "macros.h"

// ʵ�崴����

class IEntity;

// ʵ����������
struct IEntProperty
{
	const char* m_strName;
	int m_nType;
	void* m_pGetFunc;
	void* m_pSetFunc;
	IEntProperty* m_pNext;
};

class IEntCreator
{
public:
	//IEntCreator(IEntCreator* pNext)
	//{  
	//	m_pNext = pNext; 
	//	m_pProperty = NULL;
	//}
	IEntCreator()
	{  
		m_pNext = NULL; 
		m_pProperty = NULL;
	}

	virtual ~IEntCreator() = 0;

	// �Ƿ�Ϊ�����ֻࣨ�����ڼ̳У�
	virtual bool IsAbstract() const { return false; }
	// ��ø�������
	virtual const char* GetParent() const = 0;
	// ��������
	virtual const char* GetName() const = 0;
	// ����ʵ���ʶ��
	virtual int GetClassID() const = 0;
	// ����
	virtual IEntity* Create() = 0;
	// ɾ��
	virtual void Destroy(IEntity* p) = 0;
	
	// ������һ��
	void SetNext(IEntCreator* value) { m_pNext = value; }
	// �����һ��
	IEntCreator* GetNext()	const { return m_pNext; }
	// �����������
	IEntProperty* GetPropertyLink() const { return m_pProperty; }
	// ������������
	void SetPropertyLink(IEntProperty* value) { m_pProperty = value; }

//private:
//	IEntCreator();

private:
	IEntCreator* m_pNext;
	IEntProperty* m_pProperty;
};

inline IEntCreator::~IEntCreator() {}

#endif // _PUBLIC_I_ENT_CREATOR_H
