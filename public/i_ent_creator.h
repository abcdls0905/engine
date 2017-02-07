//--------------------------------------------------------------------
// 文件名:		i_ent_creator.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年1月31日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_ENT_CREATOR_H
#define _PUBLIC_I_ENT_CREATOR_H

#include "macros.h"

// 实体创建器

class IEntity;

// 实体属性描述
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

	// 是否为纯虚类（只能用于继承）
	virtual bool IsAbstract() const { return false; }
	// 获得父类名称
	virtual const char* GetParent() const = 0;
	// 返回名称
	virtual const char* GetName() const = 0;
	// 返回实体标识符
	virtual int GetClassID() const = 0;
	// 创建
	virtual IEntity* Create() = 0;
	// 删除
	virtual void Destroy(IEntity* p) = 0;
	
	// 设置下一个
	void SetNext(IEntCreator* value) { m_pNext = value; }
	// 获得下一个
	IEntCreator* GetNext()	const { return m_pNext; }
	// 获得属性链表
	IEntProperty* GetPropertyLink() const { return m_pProperty; }
	// 设置属性链表
	void SetPropertyLink(IEntProperty* value) { m_pProperty = value; }

//private:
//	IEntCreator();

private:
	IEntCreator* m_pNext;
	IEntProperty* m_pProperty;
};

inline IEntCreator::~IEntCreator() {}

#endif // _PUBLIC_I_ENT_CREATOR_H
