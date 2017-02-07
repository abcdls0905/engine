//--------------------------------------------------------------------
// 文件名:		i_ent_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_ENT_INFO_H
#define _PUBLIC_I_ENT_INFO_H

#include "macros.h"
#include "i_var_list.h"

// 实体属性信息

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

// 实体类信息

class IEntCreator;
class IEntity;

class IEntInfo
{
public:
	virtual ~IEntInfo() = 0;

	// 获得创建器
	virtual IEntCreator* GetCreator() const = 0;
	// 获得父类名称
	virtual const char* GetParentName() const = 0;
	// 返回类名
	virtual const char* GetEntityName() const = 0;
	// 返回实体类标识符
	virtual int GetEntityClassID() const = 0;
	// 获得父类信息
	virtual const IEntInfo* GetParent() const = 0;
	// 是否属于某类或是其子类
	virtual bool IsKindOf(const char* name) const = 0;

	// 实体属性
	virtual size_t GetPropertyCount() const = 0;
	// 获得属性名字列表
	virtual size_t GetPropertyList(IVarList& result) const = 0;
	// 在本类中获得属性信息
	virtual const IPropInfo* GetPropertyInfo(const char* name) const = 0;
	// 从本类和父类查找属性信息
	virtual const IPropInfo* FindPropertyInfo(const char* name) const = 0;
	// 获得本类和父类的属性名字列表
	virtual size_t GetPropertyAll(IVarList& result) const = 0;
};

inline IEntInfo::~IEntInfo() {}

#endif // _PUBLIC_I_ENT_INFO_H
