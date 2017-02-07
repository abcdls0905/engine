//--------------------------------------------------------------------
// �ļ���:		game_record.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_RECORD_H
#define _GAME_RECORD_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../public/var.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"

// ��Ϸ�������

class CGameRecord
{
private:
	typedef TVar<4, TCoreAlloc> row_value_t;

public:
	CGameRecord();
	~CGameRecord();

	// �������
	void SetName(const char* value);
	const char* GetName() const;

	// ����
	void SetColCount(size_t value);
	size_t GetColCount() const;

	// ������
	bool SetColType(size_t index, int type);
	int GetColType(size_t index) const;
	
	// ����
	size_t GetRowCount() const;
	// ������
	bool InsertRow(size_t row);
	// ɾ����
	bool DeleteRow(size_t row);
	// ���
	void ClearRow();
	// ������
	int FindRow(size_t col, const IVar& key, size_t beg_row = 0);
	// �����д�Сд������
	int FindRowCI(size_t col, const IVar& key, size_t beg_row = 0);
	
	// �����ֵ
	bool SetValue(size_t row, size_t col, const IVar& value);
	bool GetValue(size_t row, size_t col, IVar& value);
	const char* GetString(size_t row, size_t col);
	const wchar_t* GetWideStr(size_t row, size_t col);

private:
	void ReleaseRow(row_value_t* row_value, size_t col_num);
	void ReleaseAll();

private:
	core_string m_strName;
	TArrayPod<int, 1, TCoreAlloc> m_ColTypes;
	TArrayPod<row_value_t*, 1, TCoreAlloc> m_RowValues;
};

#endif // _GAME_RECORD_H

