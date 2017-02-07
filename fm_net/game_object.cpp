//--------------------------------------------------------------------
// �ļ���:		game_object.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_object.h"
#include "game_prop_set.h"
#include "game_record_set.h"
#include "game_record.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"

/// \file game_object.cpp
/// \brief ��Ϸ�������

/// entity: GameObject
/// \brief ��Ϸ�������ʵ��
DECLARE_ENTITY(GameObject, NULL, IEntity);

/// readonly: string Ident
/// \brief �����ʶ
DECLARE_PROPERTY_GET(const char*, GameObject, Ident, GetIdent);

/// property: object Data
/// \brief �������ݶ���һ�������洢�������ݵ�ʵ�����
DECLARE_PROPERTY(PERSISTID, GameObject, Data, GetDataID, SetDataID);

/*
/// method: bool FindProp(string name)
/// \brief ��������
/// \param name ������
DECLARE_METHOD_1(bool, GameObject, FindProp, const char*);
/// method: int GetPropType(string name)
/// \brief �����������
/// \param name ������
DECLARE_METHOD_1(int, GameObject, GetPropType, const char*);
/// method: table GetPropList()
/// \brief ��������б�
DECLARE_METHOD_T(GameObject, GetPropList);
/// method: var QueryProp(string name)
/// \brief ��ѯ����ֵ
/// \param name ������
DECLARE_METHOD_N(GameObject, QueryProp);

/// method: bool FindRecord(string name)
/// \brief ���ұ��
/// \param name �����
DECLARE_METHOD_1(bool, GameObject, FindRecord, const char*);
/// method: int GetRecordRows(string name)
/// \brief ��ñ������
/// \param name �����
DECLARE_METHOD_1(int, GameObject, GetRecordRows, const char*);
/// method: int GetRecordCols(string name)
/// \brief ��ñ������
/// \param name �����
DECLARE_METHOD_1(int, GameObject, GetRecordCols, const char*);
/// method: int GetRecordColType(string name, int col)
/// \brief ��ñ������������
/// \param name �����
/// \param col ������
DECLARE_METHOD_2(int, GameObject, GetRecordColType, const char*, size_t);
/// method: table GetRecordList()
/// \brief ��ñ���б�
DECLARE_METHOD_T(GameObject, GetRecordList);
/// method: var QueryRecord(string name, int row, int col)
/// \brief ��ñ����ֵ
/// \param name �����
/// \param row �к�
/// \param col �к�
DECLARE_METHOD_N(GameObject, QueryRecord);
/// method: int FindRecordRow(string name, int col, var key, int beg_row)
/// \brief ������
/// \param name �����
/// \param col �к�
/// \param key ��ֵ
/// \param beg_row ��ʼ��
DECLARE_METHOD_N(GameObject, FindRecordRow);
/// method: int FindRecordRowCI(string name, int col, var key, int beg_row)
/// \brief ��Сд�����в�����
/// \param name �����
/// \param col �к�
/// \param key ��ֵ
/// \param beg_row ��ʼ��
DECLARE_METHOD_N(GameObject, FindRecordRowCI);*/

// GameObject

GameObject::GameObject()
{
	m_pData = NULL;
	m_nHash = 0;
	m_pPropSet = CORE_NEW(CGamePropSet);
	m_pRecordSet = NULL;
}

GameObject::~GameObject()
{
	CORE_DELETE(m_pPropSet);
	
	if (m_pRecordSet)
	{
		CORE_DELETE(m_pRecordSet);
	}
}

bool GameObject::Init(const IVarList& args)
{
	return true;
}

bool GameObject::Shut()
{
	SAFE_RELEASE(m_pData);
	
	return true;
}

void GameObject::SetDataID(const PERSISTID& value)
{
	SAFE_RELEASE(m_pData);
	
	m_pData = GetCore()->GetEntity(value);
}

PERSISTID GameObject::GetDataID() const
{
	if (NULL == m_pData)
	{
		return PERSISTID();
	}

	return m_pData->GetID();
}

void GameObject::SetIdent(const char* value)
{
	Assert(value != NULL);

	m_strIdent = value;
}

const char* GameObject::GetIdent() const
{
	return m_strIdent.c_str();
}

bool GameObject::FindProp(const char* name)
{
	return m_pPropSet->Find(name);
}

int GameObject::GetPropType(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return 0;
	}

	return pValue->GetType();
}

void GameObject::GetPropList(const IVarList& args, IVarList& result)
{
	size_t num = m_pPropSet->GetCount();

	for (size_t i = 0; i < num; ++i)
	{
		result.AddString(m_pPropSet->GetName(i));
	}
}

void GameObject::QueryProp(const IVarList& args, IVarList& result)
{
	const char* name = args.StringVal(0);

	IVar* pValue = m_pPropSet->Get(name);
	
	if (pValue)
	{
		AddValue(result, *pValue);
	}
	else
	{
		result.AddInt(0);
	}
}

int GameObject::QueryPropInt(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return 0;
	}

	return pValue->IntVal();
}

int64_t GameObject::QueryPropInt64(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return 0;
	}

	return pValue->Int64Val();
}

float GameObject::QueryPropFloat(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return 0.0F;
	}

	return pValue->FloatVal();
}

double GameObject::QueryPropDouble(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return 0.0;
	}

	return pValue->DoubleVal();
}

const char* GameObject::QueryPropString(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return "";
	}

	return pValue->StringVal();
}

const wchar_t* GameObject::QueryPropWideStr(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return L"";
	}

	return pValue->WideStrVal();
}

PERSISTID GameObject::QueryPropObject(const char* name)
{
	IVar* pValue = m_pPropSet->Get(name);

	if (NULL == pValue)
	{
		return PERSISTID();
	}

	return pValue->ObjectVal();
}

CGameRecordSet* GameObject::GetRecordSet()
{ 
	if (NULL == m_pRecordSet)
	{
		m_pRecordSet = CORE_NEW(CGameRecordSet);
	}

	return m_pRecordSet; 
}

bool GameObject::FindRecord(const char* name)
{
	return GetRecordSet()->Find(name);
}

int GameObject::GetRecordRows(const char* name)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);
	
	if (NULL == pRecord)
	{
		return 0;
	}
	
	return (int)pRecord->GetRowCount();
}

int GameObject::GetRecordCols(const char* name)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return 0;
	}

	return (int)pRecord->GetColCount();
}

int GameObject::GetRecordColType(const char* name, size_t index)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);
	
	if (NULL == pRecord)
	{
		return 0;
	}
	
	if (index >= pRecord->GetColCount())
	{
		return 0;
	}
	
	return pRecord->GetColType(index);
}

void GameObject::GetRecordList(const IVarList& args, IVarList& result)
{
	size_t num = GetRecordSet()->GetCount();
	
	for (size_t i = 0; i < num; ++i)
	{
		result.AddString(GetRecordSet()->GetRecord(i)->GetName());
	}
}

void GameObject::QueryRecord(const IVarList& args, IVarList& result)
{
	const char* name = args.StringVal(0);
	size_t row = args.IntVal(1);
	size_t col = args.IntVal(2);

	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (pRecord)
	{
		TVar<4, TCoreAlloc> value;

		if (pRecord->GetValue(row, col, value))
		{
			AddValue(result, value);
		}
		else
		{
			result.AddInt(0);
		}
	}
}

int GameObject::QueryRecordInt(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return 0;
	}

	TVar<4, TCoreAlloc> value;

	if (!pRecord->GetValue(row, col, value))
	{
		return 0;
	}

	return value.IntVal();
}

int64_t GameObject::QueryRecordInt64(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return 0;
	}

	TVar<4, TCoreAlloc> value;

	if (!pRecord->GetValue(row, col, value))
	{
		return 0;
	}

	return value.Int64Val();
}

float GameObject::QueryRecordFloat(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return 0.0F;
	}

	CVar value;

	if (!pRecord->GetValue(row, col, value))
	{
		return 0.0F;
	}

	return value.FloatVal();
}

double GameObject::QueryRecordDouble(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return 0.0;
	}

	TVar<4, TCoreAlloc> value;

	if (!pRecord->GetValue(row, col, value))
	{
		return 0.0;
	}

	return value.DoubleVal();
}

const char* GameObject::QueryRecordString(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return "";
	}

	return pRecord->GetString(row, col);
}

const wchar_t* GameObject::QueryRecordWideStr(const char* name, int row, 
	int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return L"";
	}

	return pRecord->GetWideStr(row, col);
}

PERSISTID GameObject::QueryRecordObject(const char* name, int row, int col)
{
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		return PERSISTID();
	}

	TVar<4, TCoreAlloc> value;

	if (!pRecord->GetValue(row, col, value))
	{
		return PERSISTID();
	}

	return value.ObjectVal();
}

void GameObject::FindRecordRow(const IVarList& args, IVarList& result)
{
	const char* name = args.StringVal(0);
	size_t col = args.IntVal(1);
	TVar<4, TCoreAlloc> key;

	GetValue(args, 2, key);
	
	size_t beg_row = args.IntVal(3);
	CGameRecord* pRecord = GetRecordSet()->Get(name);

	if (NULL == pRecord)
	{
		result.AddInt(-1);
		return;
	}

	int row = pRecord->FindRow(col, key, beg_row);

	result.AddInt(row);
}

void GameObject::FindRecordRowCI(const IVarList& args, IVarList& result)
{
	const char* name = args.StringVal(0);
	size_t col = args.IntVal(1);
	TVar<4, TCoreAlloc> key;
	
	GetValue(args, 2, key);
	
	size_t beg_row = args.IntVal(3);
	CGameRecord* pRecord = GetRecordSet()->Get(name);
	
	if (NULL == pRecord)
	{
		result.AddInt(-1);
		return;
	}
	
	int row = pRecord->FindRowCI(col, key, beg_row);
	
	result.AddInt(row);
}

void GameObject::GetValue(const IVarList& vl, size_t index, IVar& value)
{
	switch (vl.GetType(index))
	{
	case VTYPE_BOOL:	// ����
		value.SetBool(vl.BoolVal(index));
		break;
	case VTYPE_INT:		// 32λ����
		value.SetInt(vl.IntVal(index));
		break;
	case VTYPE_INT64:	// 64λ����
		value.SetInt64(vl.Int64Val(index));
		break;
	case VTYPE_FLOAT:	// �����ȸ�����
		value.SetFloat(vl.FloatVal(index));
		break;
	case VTYPE_DOUBLE:	// ˫���ȸ�����
		value.SetDouble(vl.DoubleVal(index));
		break;
	case VTYPE_STRING:	// �ַ���
		value.SetString(vl.StringVal(index));
		break;
	case VTYPE_WIDESTR:	// ���ַ���
		value.SetWideStr(vl.WideStrVal(index));
		break;
	case VTYPE_OBJECT:	// �����
		value.SetObject(vl.ObjectVal(index));
		break;
	default:
		break;
	}
}

void GameObject::AddValue(IVarList& vl, const IVar& value)
{
	switch (value.GetType())
	{
	case VTYPE_BOOL:	// ����
		vl.AddBool(value.BoolVal());
		break;
	case VTYPE_INT:		// 32λ����
		vl.AddInt(value.IntVal());
		break;
	case VTYPE_INT64:	// 64λ����
		vl.AddInt64(value.Int64Val());
		break;
	case VTYPE_FLOAT:	// �����ȸ�����
		vl.AddFloat(value.FloatVal());
		break;
	case VTYPE_DOUBLE:	// ˫���ȸ�����
		vl.AddDouble(value.DoubleVal());
		break;
	case VTYPE_STRING:	// �ַ���
		vl.AddString(value.StringVal());
		break;
	case VTYPE_WIDESTR:	// ���ַ���
		vl.AddWideStr(value.WideStrVal());
		break;
	case VTYPE_OBJECT:	// �����
		vl.AddObject(value.ObjectVal());
		break;
	default:
		break;
	}
}

bool GameObject::PropertyChanged(const char* name)
{
	Assert(name != NULL);

	/*
	if (GetScript() == NULL)
	{
		return false;
	}

	GetCore()->ExecCallback(this, "on_change_property", CVarList() << name);

	char event[128];

	SafeSprintf(event, sizeof(event), "on_property_%s", name);
	
	if (GetCore()->FindCallback(this, event))
	{
		GetCore()->ExecCallback(this, event, CVarList());
	}
	*/
	return true;
}

bool GameObject::RecordChanged(const char* name, const char* op)
{
	/*
	if (GetScript() == NULL)
	{
		return false;
	}

	
	GetCore()->ExecCallback(this, "on_change_record", CVarList() << name);

	char event[128];

	SafeSprintf(event, sizeof(event), "on_record_%s", name);

	if (GetCore()->FindCallback(this, event))
	{
		GetCore()->ExecCallback(this, event, CVarList());
	}

	SafeSprintf(event, sizeof(event), "on_record_%s_%s", name, op);

	if (GetCore()->FindCallback(this, event))
	{
		GetCore()->ExecCallback(this, event, CVarList());
	}*/

	return true;
}

