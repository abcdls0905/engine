//--------------------------------------------------------------------
// 文件名:		module.h
// 内  容:		组件模块必须包含的头文件
// 说  明:		
// 创建日期:	2007年2月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_MODULE_H
#define _PUBLIC_MODULE_H

#include "macros.h"
#include "i_var.h"
#include "i_var_list.h"
#include "i_core.h"
#include "i_entity.h"
#include "i_interface.h"
#include "i_ent_creator.h"
#include "i_int_creator.h"
#include "i_ent_info.h"
#include "var_type.h"
#include "var_traits.h"
#include "var_getter.h"
#include "var_setter.h"
#include "core_mem.h"
#include <stdlib.h>
#include <stdio.h>

// 核心接口指针
extern ICore* g_pCore;
// 实体创建器链表
extern IEntCreator* g_pEntCreator;
// 功能接口创建器链表
extern IIntCreator* g_pIntCreator;

// 连接接口创建器
inline void LinkIntCreator(IIntCreator& creator)
{
	creator.SetNext(g_pIntCreator);
	g_pIntCreator = &creator;
}

// 连接实体创建器
inline void LinkEntCreator(IEntCreator& creator)
{
	creator.SetNext(g_pEntCreator);
	g_pEntCreator = &creator;
}

// 定义功能接口
// cls: 接口类名
#define DECLARE_FUNCTION_INTERFACE(cls) \
class cls##IntCreator: public IIntCreator \
{ public: \
cls##IntCreator() {} \
virtual ~cls##IntCreator() {} \
virtual const char* GetName() const { return #cls; } \
virtual IInterface* Create() { return CORE_NEW(cls); } \
virtual void Destroy(IInterface* p) { CORE_DELETE((cls*)p); } }; \
IIntCreator& cls##_IntCreator() \
{ \
	static cls##IntCreator s_init; \
	return s_init; \
}
//cls##IntCreator cls##_IntCreator(g_pIntCreator);

#define INNER_ENTITY_CREATOR(cls) \
IEntCreator& cls##_EntCreator() \
{ \
	static cls##EntCreator s_init; \
	return s_init; \
}

//#define INNER_ENTITY_CREATOR(cls) \
//cls##EntCreator& cls##_EntCreator(IEntCreator*& pCreator) \
//{ \
//	static cls##EntCreator s_init(pCreator); \
//	return s_init; \
//} \
//struct cls##EntCreator_Init \
//{ \
//	cls##EntCreator_Init(IEntCreator*& pCreator) \
//	{ \
//		cls##_EntCreator(pCreator); \
//	} \
//} cls##EntCreator_init_single(g_pEntCreator);

// 定义只能用于继承的抽象实体类
// cls: 实体类名
// cls_id: 实体标识符
// parent: 父类名
#define DECLARE_ABSTRACT_ENTITY(cls, cls_id, parent) \
class cls##EntCreator: public IEntCreator \
{ public: \
cls##EntCreator() {} \
virtual ~cls##EntCreator() {} \
virtual bool IsAbstract() const { return true; } \
virtual const char* GetParent() const \
{ parent::__validate_parent_exists(); return #parent; } \
virtual const char* GetName() const { return #cls; } \
virtual int GetClassID() const { return cls_id; } \
virtual IEntity* Create() { return NULL; } \
virtual void Destroy(IEntity* p) {} }; \
INNER_ENTITY_CREATOR(cls);

// 定义实体类
// space: 名字空间（为空则使用模块的缺省名字空间）
// cls: 实体类名
// cls_id: 实体标识符
// parent: 父类名
#define DECLARE_ENTITY(cls, cls_id, parent) \
class cls##EntCreator: public IEntCreator \
{ public: \
cls##EntCreator() {} \
virtual ~cls##EntCreator() {} \
virtual const char* GetParent() const \
{ parent::__validate_parent_exists(); return #parent; } \
virtual const char* GetName() const { return #cls; } \
virtual int GetClassID() const { return cls_id; } \
virtual IEntity* Create() { return CORE_NEW(cls); } \
virtual void Destroy(IEntity* p) { CORE_DELETE((cls*)p); } }; \
INNER_ENTITY_CREATOR(cls);

// 定义只读实体属性
// type: 属性类型
// cls: 实体类名
// prop: 属性名
// get_func: 读取属性的类成员函数
#define DECLARE_PROPERTY_GET(type, cls, prop, get_func) \
bool cls##prop##_getter(IEntity* ent, IVar& v) \
{ \
	TVarSetter<type>::Set(v, ((cls*)ent)->get_func()); \
	return true; \
} \
struct cls##prop##_prop_struct: public IEntProperty \
{ \
	cls##prop##_prop_struct() \
	{ \
		IEntCreator& creator = cls##_EntCreator(); \
		m_strName = #prop; \
		m_nType = TVarTraits<type>::Type; \
		m_pGetFunc = (void*)(cls##prop##_getter); \
		m_pSetFunc = NULL; \
		m_pNext = creator.GetPropertyLink(); \
		creator.SetPropertyLink(this); \
	} \
}; \
cls##prop##_prop_struct g_##cls##prop##_prop_struct;
		
// 定义可读可写实体属性
// type: 属性类型
// cls: 实体类名
// prop: 属性名
// get_func: 读取属性的类成员函数
// set_func: 设置属性的类成员函数
#define DECLARE_PROPERTY(type, cls, prop, get_func, set_func) \
bool cls##prop##_getter(IEntity* ent, IVar& v) \
{ \
	TVarSetter<type>::Set(v, ((cls*)ent)->get_func()); \
	return true; \
} \
bool cls##prop##_setter(IEntity* ent, const IVar& v) \
{ \
	((cls*)ent)->set_func(TVarGetter<type>::Get(v)); \
	return true; \
} \
struct cls##prop##_prop_struct: public IEntProperty \
{ \
	cls##prop##_prop_struct() \
	{ \
		IEntCreator& creator = cls##_EntCreator(); \
		m_strName = #prop; \
		m_nType = TVarTraits<type>::Type; \
		m_pGetFunc = (void*)(cls##prop##_getter); \
		m_pSetFunc = (void*)(cls##prop##_setter); \
		m_pNext = creator.GetPropertyLink(); \
		creator.SetPropertyLink(this); \
	} \
}; \
cls##prop##_prop_struct g_##cls##prop##_prop_struct;

#endif // _PUBLIC_MODULE_H
