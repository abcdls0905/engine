

#ifndef _HELPER_H
#define _HELPER_H

#include <string>
#include "../public/Module.h"

// 实体辅助函数类
class IVisBase;
class IActor;
class ITerrain;
class StateMachine;

class CHelper
{
public:
	// 查询属性(内部属性或自定义属性)
	static bool QueryBool(IEntity* pEntity, const char* name);
	static int QueryInt(IEntity* pEntity, const char* name);
	static int64_t QueryInt64(IEntity* pEntity, const char* name);
	static float QueryFloat(IEntity* pEntity, const char* name);
	static double QueryDouble(IEntity* pEntity, const char* name);
	static result_string QueryString(IEntity* pEntity, const char* name);
	static result_wstring QueryWideStr(IEntity* pEntity, 
		const char* name);
	static PERSISTID QueryObject(IEntity* pEntity, const char* name);

	// 设置属性(内部属性或自定义属性)
	static bool SetBool(IEntity* pEntity, const char* name, bool value);
	static bool SetInt(IEntity* pEntity, const char* name, int value);
	static bool SetInt64(IEntity* pEntity, const char* name, int64_t value);
	static bool SetFloat(IEntity* pEntity, const char* name, float value);
	static bool SetDouble(IEntity* pEntity, const char* name, 
		double value);
	static bool SetString(IEntity* pEntity, const char* name, 
		const char* value);
	static bool SetWideStr(IEntity* pEntity, const char* name, 
		const wchar_t* value);
	static bool SetObject(IEntity* pEntity, const char* name, 
		const PERSISTID& value);

	// 查询内部属性
	static bool QueryPropertyBool(IEntity* pEntity, const char* name);
	static int QueryPropertyInt(IEntity* pEntity, const char* name);
	static int64_t QueryPropertyInt64(IEntity* pEntity, const char* name);
	static float QueryPropertyFloat(IEntity* pEntity, const char* name);
	static double QueryPropertyDouble(IEntity* pEntity, const char* name);
	static result_string QueryPropertyString(IEntity* pEntity, const char* name);
	static result_wstring QueryPropertyWideStr(IEntity* pEntity, 
		const char* name);
	static PERSISTID QueryPropertyObject(IEntity* pEntity, const char* name);

	// 设置内部属性
	static bool SetPropertyBool(IEntity* pEntity, const char* name, 
		bool value);
	static bool SetPropertyInt(IEntity* pEntity, const char* name, 
		int value);
	static bool SetPropertyInt64(IEntity* pEntity, const char* name, 
		int64_t value);
	static bool SetPropertyFloat(IEntity* pEntity, const char* name, 
		float value);
	static bool SetPropertyDouble(IEntity* pEntity, const char* name, 
		double value);
	static bool SetPropertyString(IEntity* pEntity, const char* name, 
		const char* value);
	static bool SetPropertyWideStr(IEntity* pEntity, const char* name, 
		const wchar_t* value);
	static bool SetPropertyObject(IEntity* pEntity, const char* name, 
		const PERSISTID& value);

	// 查询自定义属性
	static bool QueryCustomBool(IEntity* pEntity, const char* name);
	static int QueryCustomInt(IEntity* pEntity, const char* name);
	static int64_t QueryCustomInt64(IEntity* pEntity, const char* name);
	static float QueryCustomFloat(IEntity* pEntity, const char* name);
	static double QueryCustomDouble(IEntity* pEntity, const char* name);
	static result_string QueryCustomString(IEntity* pEntity, const char* name);
	static result_wstring QueryCustomWideStr(IEntity* pEntity, 
		const char* name);
	static PERSISTID QueryCustomObject(IEntity* pEntity, const char* name);

	// 设置自定义属性
	static bool SetCustomBool(IEntity* pEntity, const char* name, bool value);
	static bool SetCustomInt(IEntity* pEntity, const char* name, int value);
	static bool SetCustomInt64(IEntity* pEntity, const char* name, 
		int64_t value);
	static bool SetCustomFloat(IEntity* pEntity, const char* name, 
		float value);
	static bool SetCustomDouble(IEntity* pEntity, const char* name, 
		double value);
	static bool SetCustomString(IEntity* pEntity, const char* name, 
		const char* value);
	static bool SetCustomWideStr(IEntity* pEntity, const char* name, 
		const wchar_t* value);
	static bool SetCustomObject(IEntity* pEntity, const char* name, 
		const PERSISTID& value);

	// 获得实体
  static IEntity*	GetEntity(const PERSISTID& id);
  static IEntity*	GetGlobalEntity(const char* name);
  static IVisBase* GetPlayer();
  static bool IsActor(IVisBase* pRole);
  static IActor* GetActor(IVisBase* pRole);

  //设置全局
  static bool SetGlobalEntity(const char* name, IEntity* pEntity);
  static IEntity* CreateEntity(const char* name,  std::string szGlobalName = "");  // 创建实体

  //获取逻辑对象
  static StateMachine*	GetStateMachine();

	/*
	// 调用方法
	static bool ReturnBool(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static int ReturnInt(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static int64_t ReturnInt64(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static float ReturnFloat(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static double ReturnDouble(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static result_string ReturnString(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static result_wstring ReturnWideStr(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static PERSISTID ReturnObject(IEntity* pEntity, const char* func, 
		const IVarList& args);
	static bool ReturnVarList(IEntity* pEntity, const char* func, 
		const IVarList& args, IVarList& result);
    */
};

#endif // _HELPER_H

