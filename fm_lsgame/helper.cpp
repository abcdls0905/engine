
#include "helper.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/i_var_table.h"

// 实体内部属性或自定义属性

bool CHelper::QueryBool(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.BoolVal();
	}

	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return false;
	}
	
	return pVar->BoolVal();
}

int CHelper::QueryInt(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.IntVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return 0;
	}
	
	return pVar->IntVal();
}

int64_t CHelper::QueryInt64(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.Int64Val();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return 0;
	}
	
	return pVar->Int64Val();
}

float CHelper::QueryFloat(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.FloatVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return 0.0f;
	}
	
	return pVar->FloatVal();
}

double CHelper::QueryDouble(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.DoubleVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return 0.0;
	}
	
	return pVar->DoubleVal();
}

result_string CHelper::QueryString(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.StringVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return "";
	}
	
	return pVar->StringVal();
}

result_wstring CHelper::QueryWideStr(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.WideStrVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return L"";
	}
	
	return pVar->WideStrVal();
}

PERSISTID CHelper::QueryObject(IEntity* pEntity, const char* name)
{
	CVar prop;
	
	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.ObjectVal();
	}
	
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);
	
	if (NULL == pVar)
	{
		return PERSISTID();
	}
	
	return pVar->ObjectVal();
}

bool CHelper::SetBool(IEntity* pEntity, const char* name, bool value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_BOOL, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_BOOL, value));
	}
	
	return true;
}

bool CHelper::SetInt(IEntity* pEntity, const char* name, int value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_INT, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_INT, value));
	}
	
	return true;
}

bool CHelper::SetInt64(IEntity* pEntity, const char* name, int64_t value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_INT64, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_INT64, value));
	}
	
	return true;
}

bool CHelper::SetFloat(IEntity* pEntity, const char* name, float value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_FLOAT, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_FLOAT, value));
	}
	
	return true;
}

bool CHelper::SetDouble(IEntity* pEntity, const char* name, double value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_DOUBLE, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_DOUBLE, value));
	}
	
	return true;
}

bool CHelper::SetString(IEntity* pEntity, const char* name, 
	const char* value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_STRING, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_STRING, value));
	}
	
	return true;
}

bool CHelper::SetWideStr(IEntity* pEntity, const char* name, 
	const wchar_t* value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_WIDESTR, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_WIDESTR, value));
	}
	
	return true;
}

bool CHelper::SetObject(IEntity* pEntity, const char* name, 
	const PERSISTID& value)
{
	if (!pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_OBJECT, value)))
	{
		pEntity->GetCustoms()->Assign(name, CVar(VTYPE_OBJECT, value));
	}
	
	return true;
}

// 实体内部属性

bool CHelper::QueryPropertyBool(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.BoolVal();
	}

	return false;
}

int CHelper::QueryPropertyInt(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.IntVal();
	}

	return 0;
}

int64_t CHelper::QueryPropertyInt64(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.Int64Val();
	}

	return 0;
}

float CHelper::QueryPropertyFloat(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.FloatVal();
	}

	return 0.0f;
}

double CHelper::QueryPropertyDouble(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.DoubleVal();
	}

	return 0.0;
}

result_string CHelper::QueryPropertyString(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.StringVal();
	}

	return "";
}

result_wstring CHelper::QueryPropertyWideStr(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.WideStrVal();
	}

	return L"";
}

PERSISTID CHelper::QueryPropertyObject(IEntity* pEntity, const char* name)
{
	CVar prop;

	if (pEntity->GetCore()->GetProperty(pEntity, name, prop))
	{
		return prop.ObjectVal();
	}

	return PERSISTID();
}

bool CHelper::SetPropertyBool(IEntity* pEntity, const char* name, bool value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_BOOL, value));
}

bool CHelper::SetPropertyInt(IEntity* pEntity, const char* name, int value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_INT, value));
}

bool CHelper::SetPropertyInt64(IEntity* pEntity, const char* name, 
	int64_t value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_INT64, value));
}

bool CHelper::SetPropertyFloat(IEntity* pEntity, const char* name, float value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_FLOAT, value));
}

bool CHelper::SetPropertyDouble(IEntity* pEntity, const char* name, 
	double value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_DOUBLE, value));
}

bool CHelper::SetPropertyString(IEntity* pEntity, const char* name, 
	const char* value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_STRING, value));
}

bool CHelper::SetPropertyWideStr(IEntity* pEntity, const char* name, 
	const wchar_t* value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_WIDESTR, value));
}

bool CHelper::SetPropertyObject(IEntity* pEntity, const char* name, 
	const PERSISTID& value)
{
	return pEntity->GetCore()->SetProperty(pEntity, name, 
		CVar(VTYPE_OBJECT, value));
}

// 实体自定义属性

bool CHelper::QueryCustomBool(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return false;
	}

	return pVar->BoolVal();
}

int CHelper::QueryCustomInt(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0;
	}

	return pVar->IntVal();
}

int64_t CHelper::QueryCustomInt64(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0;
	}

	return pVar->Int64Val();
}

float CHelper::QueryCustomFloat(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0.0f;
	}

	return pVar->FloatVal();
}

double CHelper::QueryCustomDouble(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0.0;
	}

	return pVar->DoubleVal();
}

result_string CHelper::QueryCustomString(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return "";
	}

	return pVar->StringVal();
}

result_wstring CHelper::QueryCustomWideStr(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return L"";
	}

	return pVar->WideStrVal();
}

PERSISTID CHelper::QueryCustomObject(IEntity* pEntity, const char* name)
{
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return PERSISTID();
	}

	return pVar->ObjectVal();
}

bool CHelper::SetCustomBool(IEntity* pEntity, const char* name, bool value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_BOOL, value));

	return true;
}

bool CHelper::SetCustomInt(IEntity* pEntity, const char* name, int value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_INT, value));

	return true;
}

bool CHelper::SetCustomInt64(IEntity* pEntity, const char* name, int64_t value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_INT64, value));

	return true;
}

bool CHelper::SetCustomFloat(IEntity* pEntity, const char* name, float value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_FLOAT, value));

	return true;
}

bool CHelper::SetCustomDouble(IEntity* pEntity, const char* name, double value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_DOUBLE, value));

	return true;
}

bool CHelper::SetCustomString(IEntity* pEntity, const char* name, 
	const char* value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_STRING, value));

	return true;
}

bool CHelper::SetCustomWideStr(IEntity* pEntity, const char* name, 
	const wchar_t* value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_WIDESTR, value));

	return true;
}

bool CHelper::SetCustomObject(IEntity* pEntity, const char* name, 
	const PERSISTID& value)
{
	pEntity->GetCustoms()->Assign(name, CVar(VTYPE_OBJECT, value));

	return true;
}

// 全局变量
IEntity* CHelper::GetGlobalEntity(const char* name)
{
	IVar& v = g_pCore->GetGlobalValue(name);
	if (v.GetType() != VTYPE_OBJECT)
	{
		return NULL;
	}

	return g_pCore->GetEntity(v.ObjectVal());
}

IEntity* CHelper::CreateEntity(const char* name,  std::string szGlobalName)
{
	IEntity* pEnt =  g_pCore->CreateEntity(name);
	if (pEnt != NULL)
	{
		PERSISTID id = pEnt->GetID();

		if (szGlobalName == "")
		{
			szGlobalName = name;
		}

		//SetGlobalObject(szGlobalName.c_str(), id);
		SetGlobalEntity(szGlobalName.c_str(), pEnt);
	}

	return pEnt;
}

bool CHelper::SetGlobalEntity(const char* name, IEntity* pEntity)
{
	if (pEntity == NULL)
	{
		g_pCore->SetGlobalValue(name, CVar(VTYPE_OBJECT, PERSISTID()));
		return false;
	}

	return g_pCore->SetGlobalValue(name, CVar(VTYPE_OBJECT, pEntity->GetID()));
}

/*
// 实体方法
bool CHelper::ReturnBool(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;

	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return false;
	}

	return result.BoolVal(0);
}

int CHelper::ReturnInt(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return 0;
	}
	
	return result.IntVal(0);
}

int64_t CHelper::ReturnInt64(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return 0;
	}
	
	return result.Int64Val(0);
}

float CHelper::ReturnFloat(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return 0.0f;
	}
	
	return result.FloatVal(0);
}

double CHelper::ReturnDouble(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return 0.0;
	}
	
	return result.DoubleVal(0);
}

result_string CHelper::ReturnString(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return "";
	}
	
	return result.StringVal(0);
}

result_wstring CHelper::ReturnWideStr(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return L"";
	}
	
	return result.WideStrVal(0);
}

PERSISTID CHelper::ReturnObject(IEntity* pEntity, const char* func, 
	const IVarList& args)
{
	CVarList result;
	
	if (!pEntity->GetCore()->InvokeMethod(pEntity, func, args, result))
	{
		return PERSISTID();
	}
	
	return result.ObjectVal(0);
}

bool CHelper::ReturnVarList(IEntity* pEntity, const char* func, 
	const IVarList& args, IVarList& result)
{
	return pEntity->GetCore()->InvokeMethod(pEntity, func, args, result);
}

*/