//--------------------------------------------------------------------
// 文件名:		game_record.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 游戏表格数据

class CGameRecord
{
private:
	typedef TVar<4, TCoreAlloc> row_value_t;

public:
	CGameRecord();
	~CGameRecord();

	// 表格名称
	void SetName(const char* value);
	const char* GetName() const;

	// 列数
	void SetColCount(size_t value);
	size_t GetColCount() const;

	// 列类型
	bool SetColType(size_t index, int type);
	int GetColType(size_t index) const;
	
	// 行数
	size_t GetRowCount() const;
	// 插入行
	bool InsertRow(size_t row);
	// 删除行
	bool DeleteRow(size_t row);
	// 清空
	void ClearRow();
	// 查找行
	int FindRow(size_t col, const IVar& key, size_t beg_row = 0);
	// 查找行大小写不敏感
	int FindRowCI(size_t col, const IVar& key, size_t beg_row = 0);
	
	// 表格数值
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

