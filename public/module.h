//--------------------------------------------------------------------
// �ļ���:		module.h
// ��  ��:		���ģ����������ͷ�ļ�
// ˵  ��:		
// ��������:	2007��2��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ���Ľӿ�ָ��
extern ICore* g_pCore;
// ʵ�崴��������
extern IEntCreator* g_pEntCreator;
// ���ܽӿڴ���������
extern IIntCreator* g_pIntCreator;

// ���ӽӿڴ�����
inline void LinkIntCreator(IIntCreator& creator)
{
	creator.SetNext(g_pIntCreator);
	g_pIntCreator = &creator;
}

// ����ʵ�崴����
inline void LinkEntCreator(IEntCreator& creator)
{
	creator.SetNext(g_pEntCreator);
	g_pEntCreator = &creator;
}

// ���幦�ܽӿ�
// cls: �ӿ�����
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

// ����ֻ�����ڼ̳еĳ���ʵ����
// cls: ʵ������
// cls_id: ʵ���ʶ��
// parent: ������
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

// ����ʵ����
// space: ���ֿռ䣨Ϊ����ʹ��ģ���ȱʡ���ֿռ䣩
// cls: ʵ������
// cls_id: ʵ���ʶ��
// parent: ������
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

// ����ֻ��ʵ������
// type: ��������
// cls: ʵ������
// prop: ������
// get_func: ��ȡ���Ե����Ա����
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
		
// ����ɶ���дʵ������
// type: ��������
// cls: ʵ������
// prop: ������
// get_func: ��ȡ���Ե����Ա����
// set_func: �������Ե����Ա����
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
