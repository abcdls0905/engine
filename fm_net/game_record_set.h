//--------------------------------------------------------------------
// �ļ���:		game_record_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_RECORD_SET_H
#define _GAME_RECORD_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"

// ��Ϸ������ݼ���

class CGameRecord;

class CGameRecordSet
{
public:
	CGameRecordSet();
	~CGameRecordSet();

	// ����
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// ���
	bool Add(CGameRecord* record);
	// ���
	void Clear();
	// ���
	CGameRecord* Get(const char* name);
	// �������
	size_t GetCount() const;
	// ��ñ��
	CGameRecord* GetRecord(size_t index) const;

private:
	void ReleaseAll();

private:
	TArrayPod<CGameRecord*, 1, TCoreAlloc> m_Records;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_Indices;
};

#endif // _GAME_RECORD_SET_H

