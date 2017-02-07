//--------------------------------------------------------------------
// �ļ���:		i_ent_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��7��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_ENT_INFO_H
#define _PUBLIC_I_ENT_INFO_H

#include "macros.h"
#include "i_var_list.h"

// ʵ��������Ϣ

class IPropInfo
{
public:
	virtual ~IPropInfo() = 0;

	virtual const char* GetName() const = 0;
	virtual int GetType() const = 0;
	virtual void* GetGetFunc() const = 0;
	virtual void* GetSetFunc() const = 0;
};

inline IPropInfo::~IPropInfo() {}

// ʵ������Ϣ

class IEntCreator;
class IEntity;

class IEntInfo
{
public:
	virtual ~IEntInfo() = 0;

	// ��ô�����
	virtual IEntCreator* GetCreator() const = 0;
	// ��ø�������
	virtual const char* GetParentName() const = 0;
	// ��������
	virtual const char* GetEntityName() const = 0;
	// ����ʵ�����ʶ��
	virtual int GetEntityClassID() const = 0;
	// ��ø�����Ϣ
	virtual const IEntInfo* GetParent() const = 0;
	// �Ƿ�����ĳ�����������
	virtual bool IsKindOf(const char* name) const = 0;

	// ʵ������
	virtual size_t GetPropertyCount() const = 0;
	// ������������б�
	virtual size_t GetPropertyList(IVarList& result) const = 0;
	// �ڱ����л��������Ϣ
	virtual const IPropInfo* GetPropertyInfo(const char* name) const = 0;
	// �ӱ���͸������������Ϣ
	virtual const IPropInfo* FindPropertyInfo(const char* name) const = 0;
	// ��ñ���͸�������������б�
	virtual size_t GetPropertyAll(IVarList& result) const = 0;
};

inline IEntInfo::~IEntInfo() {}

#endif // _PUBLIC_I_ENT_INFO_H
