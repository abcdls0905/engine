//--------------------------------------------------------------------
// �ļ���:		i_var_table.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_VAR_TABLE_H
#define _PUBLIC_I_VAR_TABLE_H

#include "macros.h"
#include "i_var_list.h"

// �ɱ���������������ӿ�

class IVar;

class IVarTable
{
public:
	virtual ~IVarTable() = 0;

	// ��ñ�������
	virtual size_t GetCount() const = 0;
	// ��ü����б�
	virtual void GetKeyList(IVarList& result) const = 0;
	// ��ü�ֵ
	virtual IVar* GetValue(const char* key) const = 0;
	// ��ֵ�Ƿ����
	virtual bool Exists(const char* key) const = 0;
	// ��Ӽ�
	virtual IVar* Add(const char* key, const IVar& value) = 0;
	// ɾ����
	virtual bool Remove(const char* key) = 0;
	// ���
	virtual void Clear() = 0;
	// ���ü�ֵ������������ӣ�
	virtual void Assign(const char* key, const IVar& value) = 0;
};

inline IVarTable::~IVarTable() {}

#endif // _PUBLIC_I_VAR_TABLE_H

