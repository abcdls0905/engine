//--------------------------------------------------------------------
// �ļ���:		ent_info_list.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ENT_INFO_LIST_H
#define _ENT_INFO_LIST_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// ʵ����Ϣ����

class CEntInfo;

class CEntInfoList
{
public:
	CEntInfoList();
	~CEntInfoList();

	// ���
	bool Add(CEntInfo* pEntInfo);
	// �������
	size_t GetCount() const;
	// ���ָ��λ������
	CEntInfo* GetByIndex(size_t index) const;
	// ����
	CEntInfo* Get(const char* name) const;

private:
	CEntInfoList(const CEntInfoList&);
	CEntInfoList& operator=(const CEntInfoList&);

	void Expand();
	
private:
	TArrayPod<CEntInfo*, 8> m_EntInfos;
	CEntInfo** m_pBuckets;
	size_t m_nSize;
};

#endif // _ENT_INFO_LIST_H
