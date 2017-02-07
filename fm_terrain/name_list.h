//--------------------------------------------------------------------
// �ļ���:		name_list.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��5��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _NAME_LIST_H
#define _NAME_LIST_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

// ���ּ���

class CNameList
{
private:
	struct name_t
	{
		char* pName;
		size_t nSize;
	};
	
public:
	CNameList()
	{
	}
	
	~CNameList()
	{
		Clear();
	}
	
	size_t GetCount() const
	{
		return m_Names.size();
	}
	
	const char* GetByIndex(size_t index) const
	{
		Assert(index < m_Names.size());
		
		return m_Names[index].pName;
	}
	
	bool AddName(const char* name)
	{
		Assert(name != NULL);
		
		size_t name_size = strlen(name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);
		name_t data;
		
		memcpy(pName, name, name_size);
		data.pName = pName;
		data.nSize = name_size;
		
		m_Names.push_back(data);
		
		return true;
	}
	
	void Clear()
	{
		for (size_t i = 0; i < m_Names.size(); ++i)
		{
			CORE_FREE(m_Names[i].pName, m_Names[i].nSize);
		}
		
		m_Names.clear();
	}

private:
	CNameList(const CNameList&);
	CNameList& operator=(const CNameList&);
	
private:	
	TArrayPod<name_t, 1, TCoreAlloc> m_Names;
};

#endif // _NAME_LIST_H
