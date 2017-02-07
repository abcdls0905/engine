//--------------------------------------------------------------------
// 文件名:		game_record.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_record.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"
#include "../public/var.h"

// CGameRecord

CGameRecord::CGameRecord()
{
}

CGameRecord::~CGameRecord()
{
	ReleaseAll();
}

void CGameRecord::ReleaseRow(row_value_t* row_value, size_t col_num)
{
	for (size_t i = 0; i < col_num; ++i)
	{
		row_value[i].~row_value_t();
	}

	CORE_FREE(row_value, sizeof(row_value_t) * col_num);
}

void CGameRecord::ReleaseAll()
{
	size_t col_num = m_ColTypes.size();
	
	for (size_t i = 0; i < m_RowValues.size(); ++i)
	{
		if (m_RowValues[i])
		{
			ReleaseRow(m_RowValues[i], col_num);
		}
	}

	m_RowValues.clear();
}

void CGameRecord::SetName(const char* value)
{
	Assert(value != NULL);

	m_strName = value;
}

const char* CGameRecord::GetName() const
{
	return m_strName.c_str();
}

void CGameRecord::SetColCount(size_t value)
{
	Assert(value > 0);

	if (m_ColTypes.size() > 0)
	{
		ReleaseAll();
	}

	m_ColTypes.resize(value);
}

size_t CGameRecord::GetColCount() const
{
	return m_ColTypes.size();
}

bool CGameRecord::SetColType(size_t index, int type)
{
	Assert(index < m_ColTypes.size());

	m_ColTypes[index] = type;

	return true;
}

int CGameRecord::GetColType(size_t index) const
{
	Assert(index < m_ColTypes.size());
	
	return m_ColTypes[index];
}

size_t CGameRecord::GetRowCount() const
{
	return m_RowValues.size();
}

bool CGameRecord::InsertRow(size_t row)
{
	size_t col_num = m_ColTypes.size();
	row_value_t* row_value = (row_value_t*)CORE_ALLOC(
		sizeof(row_value_t) * col_num);
	
	for (size_t i = 0; i < col_num; ++i)
	{
		new (&row_value[i]) row_value_t;
	}
	
	if (row >= m_RowValues.size())
	{
		m_RowValues.push_back(row_value);
	}
	else
	{
		m_RowValues.insert(row, row_value);
	}
	
	return true;
}

bool CGameRecord::DeleteRow(size_t row)
{
	if (row >= m_RowValues.size())
	{
		return false;
	}

	ReleaseRow(m_RowValues[row], m_ColTypes.size());
	m_RowValues.remove(row);
	
	return true;
}

void CGameRecord::ClearRow()
{
	ReleaseAll();
}

int CGameRecord::FindRow(size_t col, const IVar& key, size_t beg_row)
{
	if (col >= GetColCount())
	{
		return -1;
	}

	size_t row_num = GetRowCount();

	if (beg_row >= row_num)
	{
		return -1;
	}

	for (size_t r = beg_row; r < row_num; ++r)
	{
		row_value_t* row_value = m_RowValues[r];
		
		switch (m_ColTypes[col])
		{
		case VTYPE_BOOL:
			if (row_value[col].BoolVal() == key.BoolVal())
			{
				return (int)r;
			}
			break;
		case VTYPE_INT:
			if (row_value[col].IntVal() == key.IntVal())
			{
				return (int)r;
			}
			break;
		case VTYPE_INT64:
			if (row_value[col].Int64Val() == key.Int64Val())
			{
				return (int)r;
			}
			break;
		case VTYPE_FLOAT:
			if (FloatEqual(row_value[col].FloatVal(), key.FloatVal()))
			{
				return (int)r;
			}
			break;
		case VTYPE_DOUBLE:
			if (DoubleEqual(row_value[col].DoubleVal(), key.DoubleVal()))
			{
				return (int)r;
			}
			break;
		case VTYPE_STRING:
			if (strcmp(row_value[col].StringVal(), key.StringVal()) == 0)
			{
				return (int)r;
			}
			break;
		case VTYPE_WIDESTR:
			if (wcscmp(row_value[col].WideStrVal(), key.WideStrVal()) == 0)
			{
				return (int)r;
			}
			break;
		case VTYPE_OBJECT:
			if (row_value[col].ObjectVal() == key.ObjectVal())
			{
				return (int)r;
			}
			break;
		default:
			break;
		}
	}

	return -1;
}

int CGameRecord::FindRowCI(size_t col, const IVar& key, size_t beg_row)
{
	if (col >= GetColCount())
	{
		return -1;
	}
	
	size_t row_num = GetRowCount();
	
	if (beg_row >= row_num)
	{
		return -1;
	}
	
	for (size_t r = beg_row; r < row_num; ++r)
	{
		row_value_t* row_value = m_RowValues[r];
		
		switch (m_ColTypes[col])
		{
		case VTYPE_BOOL:
			if (row_value[col].BoolVal() == key.BoolVal())
			{
				return (int)r;
			}
			break;
		case VTYPE_INT:
			if (row_value[col].IntVal() == key.IntVal())
			{
				return (int)r;
			}
			break;
		case VTYPE_INT64:
			if (row_value[col].Int64Val() == key.Int64Val())
			{
				return (int)r;
			}
			break;
		case VTYPE_FLOAT:
			if (FloatEqual(row_value[col].FloatVal(), key.FloatVal()))
			{
				return (int)r;
			}
			break;
		case VTYPE_DOUBLE:
			if (DoubleEqual(row_value[col].DoubleVal(), key.DoubleVal()))
			{
				return (int)r;
			}
			break;
		case VTYPE_STRING:
			if (stricmp(row_value[col].StringVal(), key.StringVal()) == 0)
			{
				return (int)r;
			}
			break;
		case VTYPE_WIDESTR:
			if (wcsicmp(row_value[col].WideStrVal(), key.WideStrVal()) == 0)
			{
				return (int)r;
			}
			break;
		case VTYPE_OBJECT:
			if (row_value[col].ObjectVal() == key.ObjectVal())
			{
				return (int)r;
			}
			break;
		default:
			break;
		}
	}
	
	return -1;
}

bool CGameRecord::SetValue(size_t row, size_t col, const IVar& value)
{
	if ((row >= GetRowCount()) || (col >= GetColCount()))
	{
		return false;
	}

	row_value_t* row_value = m_RowValues[row];
	
	row_value[col].Assign(value);
	
	return true;
}

bool CGameRecord::GetValue(size_t row, size_t col, IVar& value)
{
	if ((row >= GetRowCount()) || (col >= GetColCount()))
	{
		return false;
	}
	
	row_value_t* row_value = m_RowValues[row];
	
	value.Assign(row_value[col]);
	
	return true;
}

const char* CGameRecord::GetString(size_t row, size_t col)
{
	if ((row >= GetRowCount()) || (col >= GetColCount()))
	{
		return "";
	}

	row_value_t* row_value = m_RowValues[row];

	if (row_value[col].GetType() != VTYPE_STRING)
	{
		return "";
	}

	return row_value[col].StringVal();
}

const wchar_t* CGameRecord::GetWideStr(size_t row, size_t col)
{
	if ((row >= GetRowCount()) || (col >= GetColCount()))
	{
		return L"";
	}

	row_value_t* row_value = m_RowValues[row];

	if (row_value[col].GetType() != VTYPE_WIDESTR)
	{
		return L"";
	}

	return row_value[col].WideStrVal();
}

