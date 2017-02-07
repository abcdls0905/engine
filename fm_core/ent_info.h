//--------------------------------------------------------------------
// �ļ���:		ent_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��7��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ENT_INFO_H
#define _ENT_INFO_H

#include "../public/macros.h"
#include "../public/i_ent_info.h"
#include "../public/i_ent_creator.h"
#include "../public/fast_str.h"
#include "../utils/literal_pod.h"

// ʵ��������Ϣ

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

// ʵ������Ϣ

class CEntInfo: public IEntInfo
{
public:
	CEntInfo();
	virtual ~CEntInfo();

	// ��ô�����
	virtual IEntCreator* GetCreator() const;
	// ��ø�������
	virtual const char* GetParentName() const;
	// ��������
	virtual const char* GetEntityName() const;
	// ����ʵ�����ʶ��
	virtual int GetEntityClassID() const;
	// ��ø�����Ϣ
	virtual const IEntInfo* GetParent() const;
	// �Ƿ�����ĳ�����������
	virtual bool IsKindOf(const char* name) const;
	
	// ����
	virtual size_t GetPropertyCount() const;
	// ������������б�
	virtual size_t GetPropertyList(IVarList& result) const;
	// �ڱ����л��������Ϣ
	virtual const IPropInfo* GetPropertyInfo(const char* name) const;
	// �ӱ���͸������������Ϣ
	virtual const IPropInfo* FindPropertyInfo(const char* name) const;
	// ��ñ���͸�������������б�
	virtual size_t GetPropertyAll(IVarList& result) const;
	
	void SetCreator(IEntCreator* value);
	void SetParentName(const char* value);
	void SetEntityName(const char* value);
	void SetEntityClassID(int value);
	void SetParent(IEntInfo* value);

	// �������������ת˳��
	size_t AddPropertyLink(IEntProperty* pPropertyLink);

	// ����
	void SetNext(CEntInfo* value) { m_pNext = value; }
	CEntInfo* GetNext() const { return m_pNext; }

	// ��ϣֵ
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

