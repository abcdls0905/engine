//--------------------------------------------------------------------
// �ļ���:		game_prop_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_PROP_SET_H
#define _GAME_PROP_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/var.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"

// ��Ϸ�������ݼ���

class CGamePropSet
{
private:
	struct prop_data_t
	{
		core_string strName;
		TVar<4, TCoreAlloc> Value;
	};

public:
	CGamePropSet();
	~CGamePropSet();

	// ����
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// ���
	bool Add(const char* name, const IVar& value);
	// ���
	void Clear();
	// �����ֵ
	IVar* Get(const char* name);
	// �������
	size_t GetCount() const;
	// ���������
	const char* GetName(size_t index) const;
	// �������ֵ
	const IVar& GetValue(size_t index) const;
	// ��������ֵ
	bool SetValue(size_t index, const IVar& value);

private:
	TArrayPod<prop_data_t*, 1, TCoreAlloc> m_Props;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_Indices;
};

#endif // _GAME_PROP_SET_H

