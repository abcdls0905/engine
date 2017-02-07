//--------------------------------------------------------------------
// 文件名:		ent_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月7日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ENT_INFO_H
#define _ENT_INFO_H

#include "../public/macros.h"
#include "../public/i_ent_info.h"
#include "../public/i_ent_creator.h"
#include "../public/fast_str.h"
#include "../utils/literal_pod.h"

// 实体属性信息

class CPropInfo: public IPropInfo
{
public:
	CPropInfo()
	{
		m_nType = 0;
		m_pGetFunc = NULL;
		m_pSetFunc = NULL;
	}
	
	virtual ~CPropInfo()
	{
	}
	
	virtual const char* GetName() const
	{
		return m_strName.c_str();
	}

	virtual int GetType() const
	{
		return m_nType;
	}

	virtual void* GetGetFunc() const
	{
		return m_pGetFunc;
	}

	virtual void* GetSetFunc() const
	{
		return m_pSetFunc;
	}

	void SetName(const char* value)
	{
		m_strName = value;
	}

	void SetType(int value)
	{
		m_nType = value;
	}

	void SetGetFunc(void* value)
	{
		m_pGetFunc = value;
	}

	void SetSetFunc(void* value)
	{
		m_pSetFunc = value;
	}

private:
	TFastStr<char, 16> m_strName;
	int m_nType;
	void* m_pGetFunc;
	void* m_pSetFunc;
};

// 实体类信息

class CEntInfo: public IEntInfo
{
public:
	CEntInfo();
	virtual ~CEntInfo();

	// 获得创建器
	virtual IEntCreator* GetCreator() const;
	// 获得父类名称
	virtual const char* GetParentName() const;
	// 返回类名
	virtual const char* GetEntityName() const;
	// 返回实体类标识符
	virtual int GetEntityClassID() const;
	// 获得父类信息
	virtual const IEntInfo* GetParent() const;
	// 是否属于某类或是其子类
	virtual bool IsKindOf(const char* name) const;
	
	// 属性
	virtual size_t GetPropertyCount() const;
	// 获得属性名字列表
	virtual size_t GetPropertyList(IVarList& result) const;
	// 在本类中获得属性信息
	virtual const IPropInfo* GetPropertyInfo(const char* name) const;
	// 从本类和父类查找属性信息
	virtual const IPropInfo* FindPropertyInfo(const char* name) const;
	// 获得本类和父类的属性名字列表
	virtual size_t GetPropertyAll(IVarList& result) const;
	
	void SetCreator(IEntCreator* value);
	void SetParentName(const char* value);
	void SetEntityName(const char* value);
	void SetEntityClassID(int value);
	void SetParent(IEntInfo* value);

	// 添加属性链表（反转顺序）
	size_t AddPropertyLink(IEntProperty* pPropertyLink);

	// 链接
	void SetNext(CEntInfo* value) { m_pNext = value; }
	CEntInfo* GetNext() const { return m_pNext; }

	// 哈希值
	void SetHash(size_t value) { m_nHash = value; }
	size_t GetHash() const { return m_nHash; }

private:
	CEntInfo(const CEntInfo&);
	CEntInfo& operator=(const CEntInfo&);
	
	void InnerGetPropertyList(IVarList& result) const;

private:
	CEntInfo* m_pNext;
	size_t m_nHash;
	IEntCreator* m_pCreator;
	TFastStr<char, 16> m_strParentName;
	TFastStr<char, 16> m_strEntityName;
	int m_nClassID;
	IEntInfo* m_pParent;
	CPropInfo* m_pPropInfos;
	size_t m_nPropertyCount;
	TLiteralPod<char, size_t> m_PropIndices;
};

#endif // _ENT_INFO_H

