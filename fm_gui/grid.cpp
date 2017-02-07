//--------------------------------------------------------------------
// 文件名:		grid.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "grid.h"
#include "label.h"
#include "gui_input.h"
#include "gui_utils.h"
#include "../public/core_mem.h"

/// \file grid.cpp
/// \brief 表格控件

/// entity: Grid
/// \brief 表格控件实体
/// \see 继承自IScrollable
DECLARE_ENTITY(Grid, 0, IScrollable);

/// readonly: object CommentLabel
/// \brief 注释标签对象
DECLARE_PROPERTY_GET(PERSISTID, Grid, CommentLabel, GetCommentLabelID);
/// readonly: int VisColCount
/// \brief 可见列数量
DECLARE_PROPERTY_GET(int, Grid, VisColCount, GetVisColCount);
/// readonly: int VisRowCount
/// \brief 可见行数量
DECLARE_PROPERTY_GET(int, Grid, VisRowCount, GetVisRowCount);
/// readonly: int ColSelectIndex
/// \brief 当前选择列索引
DECLARE_PROPERTY_GET(int, Grid, ColSelectIndex, GetColSelectIndex);
/// readonly: int RowSelectIndex
/// \brief 当前选择行索引
DECLARE_PROPERTY_GET(int, Grid, RowSelectIndex, GetRowSelectIndex);

/// property: bool ShowComment
/// \brief 是否显示注释信息
DECLARE_PROPERTY(bool, Grid, ShowComment, GetShowComment, SetShowComment);
/// property: int HeaderRowHeight
/// \brief 表头行高度
DECLARE_PROPERTY(int, Grid, HeaderRowHeight, GetHeaderRowHeight, 
	SetHeaderRowHeight);
/// property: int HeaderColWidth
/// \brief 表头列宽度
DECLARE_PROPERTY(int, Grid, HeaderColWidth, GetHeaderColWidth, 
	SetHeaderColWidth);
/// property: string HeaderBackColor
/// \brief 表头行背景色
DECLARE_PROPERTY(result_string, Grid, HeaderBackColor, 
	GetHeaderBackColorString, SetHeaderBackColorString);
/// property: string HeaderForeColor
/// \brief 表头行前景色
DECLARE_PROPERTY(result_string, Grid, HeaderForeColor, 
	GetHeaderForeColorString, SetHeaderForeColorString);
/// property: bool RowHeaderVisible
/// \brief 是否显示表头行
DECLARE_PROPERTY(bool, Grid, RowHeaderVisible, GetRowHeaderVisible, 
	SetRowHeaderVisible);
/// property: bool ColHeaderVisible
/// \brief 是否显示表头列
DECLARE_PROPERTY(bool, Grid, ColHeaderVisible, GetColHeaderVisible, 
	SetColHeaderVisible);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, Grid, SelectForeColor, 
	 GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, Grid, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);

/// property: string SelectBackDraw
/// \brief 被选中文本的背景图
DECLARE_PROPERTY(const char*, Grid, SelectBackDraw, GetSelectBackDraw, 
	SetSelectBackDraw);

/// property: bool CanSelectCol
/// \brief 能是否能选择一列
DECLARE_PROPERTY(bool, Grid, CanSelectCol, GetCanSelectCol, SetCanSelectCol);
/// property: bool CanSelectRow
/// \brief 能是否能选择一行
DECLARE_PROPERTY(bool, Grid, CanSelectRow, GetCanSelectRow, SetCanSelectRow);
/// property: int ColWidth
/// \brief 列宽度
DECLARE_PROPERTY(int, Grid, ColWidth, GetColWidth, SetColWidth);

/// property: string ColWidths
/// \brief 配置所有列的各自宽度
DECLARE_PROPERTY(const char*, Grid, ColWidths, GetColWidths, SetColWidths);

/// property: int RowHeight
/// \brief 行高度
DECLARE_PROPERTY(int, Grid, RowHeight, GetRowHeight, SetRowHeight);
/// property: int ColCount
/// \brief 列数量
DECLARE_PROPERTY(int, Grid, ColCount, GetColCount, SetColCount);
/// property: int RowCount
/// \brief 行数量
DECLARE_PROPERTY(int, Grid, RowCount, GetRowCount, SetRowCount);

/// property: bool Transparent
/// \brief 是否判断透明
DECLARE_PROPERTY(bool, Grid, Transparent, GetTransparent, SetTransparent);

/// readonly: int VisRowCount
/// \brief 可见行数量
DECLARE_PROPERTY_GET(int, Grid, VisRow, GetVerticalValue);
/// readonly: int VisRowCount
/// \brief 可见行数量
DECLARE_PROPERTY_GET(int, Grid, VisCol, GetHorizonValue);

/// event: int on_select_row(object self, int row)
/// \brief 选择行时的回调
/// \param self 本控件
/// \param row 行索引
DECLARE_EVENT(on_select_row);
/// event: int on_select_col(object self, int col)
/// \brief 选择列时的回调
/// \param self 本控件
/// \param col 列索引
DECLARE_EVENT(on_select_col);
/// event: int on_select_grid(object self, int row, int col)
/// \brief 选择格子时的回调
/// \param self 本控件
/// \param row 行索引
/// \param col 列索引
DECLARE_EVENT(on_select_grid);
/// event: int on_double_click_grid(object self, int row, int col)
/// \brief 双击格子时的回调
/// \param self 本控件
/// \param row 行索引
/// \param col 列索引
DECLARE_EVENT(on_double_click_grid);

/// event: int on_mousein_row_changed(object self, int new_row, int old_row)
/// \brief 鼠标移入移出行的回调
/// \param self 本控件
/// \param new_row 新移入的行索引
/// \param old_row 移出的行索引
DECLARE_EVENT(on_mousein_row_changed);

/// event: int on_mousein_col_changed(object self, int new_col, int old_col)
/// \brief 鼠标移入移出列的回调
/// \param self 本控件
/// \param new_col 新移入的列索引
/// \param old_col 移出的列索引
DECLARE_EVENT(on_mousein_col_changed);

/// event: int on_right_select_grid(object self, int row, int col)
/// \brief 右键点击回调事件
/// \param self 本控件
/// \param row 点击的行索引
/// \param col 点击的列索引
DECLARE_EVENT(on_right_select_grid);

// Grid

Grid::Grid()
{
	m_bShowSuborControl = false;
	m_bShowComment = false;
	m_bRowHeaderVisible = true;
	m_bColHeaderVisible = false;
	m_nColWidth = 40;
	m_nRowHeight = 24;
	m_nColCount = 0;
	m_nVisColBeg = 0;
	m_nVisRowBeg = 0;
	m_nHeaderRowHeight = 0;
	m_nHeaderColWidth = 0;
	m_HeaderBackColor.SetValue(HEADER_BACK_COLOR);
	m_HeaderForeColor.SetValue(HEADER_FORE_COLOR);
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	m_bCanSelectCol = false;
	m_bCanSelectRow = true;
    m_bTransparent = false;
	m_nColSelectIndex = -1;
	m_nRowSelectIndex = -1;
    m_nRowMouseInIndex = -1;
    m_nColMouseInIndex = -1;
	m_pCommentLabel = NULL;
	SetDesignSize(80, 60);
	SetCanFocus(true);
	SetTabStop(true);
}

Grid::~Grid()
{
	for (size_t i = 0; i < m_Rows.size(); ++i)
	{
		for (int j = 0; j < m_nColCount; ++j)
		{
			m_Rows[i][j].~grid_data_t();
		}

		CORE_FREE(m_Rows[i], sizeof(grid_data_t) * m_nColCount);
        m_Rows[i] = NULL;
	}

	ClearRowTitles();
	ClearColTitles();
}

bool Grid::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}
	
	SetHasVScroll(true);
	SetHasHScroll(true);
	
	// 创建缺省浮动提示框
	Label* pLabel = (Label*)CreateSuborControl(this, "Label");
	
	pLabel->SetVisible(false);
	pLabel->SetSolid(true);
	pLabel->SetNoFrame(false);
	pLabel->SetAutoSize(true);
	pLabel->SetTransparent(true);
	pLabel->SetBackColor(CGuiConst::DEF_HINT_COLOR);
	pLabel->SetAlignString("Center");

	m_pCommentLabel = pLabel;
	
	return true;
}

bool Grid::Shut()
{
	ClearAllRow();
	m_SelectBackDraw.Release();
	SAFE_RELEASE(m_pCommentLabel);

	return IScrollable::Shut();
}

void Grid::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);
	
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true, &v);
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AlwaysHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HasHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);
	v = CVar(IDesignInfo::TYPE_BOOL, false);
    di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, &v);
	
	di->AddPropInfo("ShowComment", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HeaderRowHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("HeaderColWidth", IDesignInfo::TYPE_INT, true, true);
	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(HEADER_FORE_COLOR).c_str());
	di->AddPropInfo("HeaderForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);
	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(HEADER_BACK_COLOR).c_str());
	di->AddPropInfo("HeaderBackColor", IDesignInfo::TYPE_COLOR, true, true,	&v);
	v = CVar(VTYPE_BOOL, true);
	di->AddPropInfo("RowHeaderVisible", IDesignInfo::TYPE_BOOL, true, true, &v);

	v = CVar(VTYPE_BOOL, false);
	di->AddPropInfo("ColHeaderVisible", IDesignInfo::TYPE_BOOL, true, true, &v);

	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);
	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true, &v);
    di->AddPropInfo("SelectBackDraw", IDesignInfo::TYPE_IMAGE, true, true);

	di->AddPropInfo("CanSelectRow", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("CanSelectCol", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("RowHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("ColCount", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("ColWidth", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("ColWidths", IDesignInfo::TYPE_STRING, true, true);

	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");

	di->AddEventInfo("on_select_row");
	di->AddEventInfo("on_select_col");
	di->AddEventInfo("on_select_grid");
	di->AddEventInfo("on_double_click_grid");
	di->AddEventInfo("on_mousein_row_changed");
	di->AddEventInfo("on_mousein_col_changed");
    di->AddEventInfo("on_right_select_grid");
}

bool Grid::GetLoadFinish()
{
	if (!m_SelectBackDraw.IsLoadCompelte())
	{
		return false;
	}

	if (m_pCommentLabel && !m_pCommentLabel->GetLoadFinish())
	{
		return false;
	}
	
	return IScrollable::GetLoadFinish();
}

void Grid::ClearAllRow()
{
	for (size_t i = 0; i < m_Rows.size(); ++i)
	{
		for (int j = 0; j < m_nColCount; ++j)
		{
			SAFE_RELEASE(m_Rows[i][j].pControl);
			m_Rows[i][j].~grid_data_t();
		}

		CORE_FREE(m_Rows[i], sizeof(grid_data_t) * m_nColCount);
		m_Rows[i] = NULL;
	}
	
	m_Rows.clear();
	ClearRowTitles();
	//m_RowTitles.clear();
}

void Grid::ClearColTitles()
{
	for (size_t i = 0; i < m_ColTitles.size(); ++i)
	{
		CORE_DELETE(m_ColTitles[i]);
	}

	m_ColTitles.clear();
}

void Grid::ClearRowTitles()
{
	for (size_t i = 0; i < m_RowTitles.size(); ++i)
	{
		CORE_DELETE(m_RowTitles[i]);
	}

	m_RowTitles.clear();
}

bool Grid::IsValidGrid(int row, int col) const
{
	return (size_t(row) < size_t(GetRowCount()))
		&& (size_t(col) < size_t(GetColCount()));
}

bool Grid::IsSelectGrid(int row, int col) const
{
	if (GetCanSelectCol() && GetCanSelectRow())
	{
		if ((row == m_nRowSelectIndex) && (col == m_nColSelectIndex))
		{
			return true;
		}
	}
	else if (GetCanSelectRow())
	{
		if (row == m_nRowSelectIndex)
		{
			return true;
		}
	}
	else if (GetCanSelectCol())
	{
		if (col == m_nColSelectIndex)
		{
			return true;
		}
	}
	
	return false;
}

PERSISTID Grid::GetCommentLabelID() const
{
	if (NULL == m_pCommentLabel)
	{
		return PERSISTID();
	}

	return m_pCommentLabel->GetID();
}

void Grid::SetShowComment(bool value)
{
	m_bShowComment = value;
}

bool Grid::GetShowComment() const
{
	return m_bShowComment;
}

void Grid::SetHeaderRowHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nHeaderRowHeight = value;
}

int Grid::GetHeaderRowHeight() const
{
	return m_nHeaderRowHeight;
}

void Grid::SetHeaderColWidth(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nHeaderColWidth = value;
}

int Grid::GetHeaderColWidth() const
{
	return m_nHeaderColWidth;
}

void Grid::SetHeaderBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_HeaderBackColor.SetName(GetGui(), value);
}

result_string Grid::GetHeaderBackColorString() const
{
	return m_HeaderBackColor.GetName().c_str();
}

void Grid::SetHeaderForeColorString(const char* value)
{
	Assert(value != NULL);

	m_HeaderForeColor.SetName(GetGui(), value);
}

result_string Grid::GetHeaderForeColorString() const
{
	return m_HeaderForeColor.GetName().c_str();
}

void Grid::SetRowHeaderVisible(bool value) 
{ 
	m_bRowHeaderVisible = value; 
}

bool Grid::GetRowHeaderVisible() const 
{ 
	return m_bRowHeaderVisible; 
}

void Grid::SetColHeaderVisible(bool value) 
{ 
	m_bColHeaderVisible = value; 
}

bool Grid::GetColHeaderVisible() const 
{ 
	return m_bColHeaderVisible; 
}

void Grid::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string Grid::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void Grid::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string Grid::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

// 表格的选择背景图
void Grid::SetSelectBackDraw(const char* value)
{
    m_SelectBackDraw.SetName(value);
    m_SelectBackDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Grid::GetSelectBackDraw() const
{
    return m_SelectBackDraw.GetName();
}

void Grid::SetCanSelectCol(bool value)
{
	m_bCanSelectCol = value;
}

bool Grid::GetCanSelectCol() const
{
	return m_bCanSelectCol;
}

void Grid::SetCanSelectRow(bool value)
{
	m_bCanSelectRow = value;
}

bool Grid::GetCanSelectRow() const
{
	return m_bCanSelectRow;
}

void Grid::SetColWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nColWidth = value;
	
	UpdateScroll();
}

int Grid::GetColWidth() const
{
	return m_nColWidth;
}

void Grid::SetRowHeight(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nRowHeight = value;
	UpdateScroll();
}

int Grid::GetRowHeight() const
{
	return m_nRowHeight;
}

void Grid::SetColCount(int value)
{
	if (value < 0)
	{
		return;
	}

	if (value != m_nColCount)
	{
		ClearAllRow();
	}
	
	ClearColTitles();
	m_nColCount = value;
	m_ColTitles.resize(value, NULL);

	for (int k = 0; k < value; ++k)
	{
		m_ColTitles[k] = CORE_NEW(col_data_t);
	}

	if (m_szColWidths.empty())
	{
		for (int i = 0; i < m_nColCount; ++i)
		{
			SetColWidth(i, m_nColWidth);
		}
	}
	else
	{
		SetColWidths(m_szColWidths.c_str());
	}
}

int Grid::GetColCount() const
{
	return m_nColCount;
}

void Grid::SetRowCount(int value)
{
	if (value < 0)
	{
		return;
	}

	ClearAllRow();

	for (int i = 0; i < value; i++)
	{
		grid_data_t* pRow = (grid_data_t*)CORE_ALLOC(
			sizeof(grid_data_t) * m_nColCount);

		for (int k = 0; k < m_nColCount; ++k)
		{
			new (&pRow[k]) grid_data_t;
		}
		
		m_Rows.push_back(pRow);
		m_RowTitles.push_back(CORE_NEW(row_data_t));
	}
	
	UpdateScroll();
}

int Grid::GetRowCount() const
{
	return (int)m_Rows.size();
}

void Grid::SetTransparent(bool bTrans)
{
    m_bTransparent = bTrans;
}

const bool Grid::GetTransparent()
{
    return m_bTransparent;
}


int Grid::GetVisColCount() const
{
	int count = 0;
	int w = 0;
	
	if (m_bColHeaderVisible)
	{
		w += GetHeaderColWidthValue();
	}
	
	int cols = GetColCount();
	
	for (int i = m_nVisColBeg; i < cols; i++)
	{
		if (w >= GetContentWidth())
		{
			break;
		}
		
		count++;
		w += GetColWidthValue(i);
	}
	
	return count;
}

int Grid::GetVisRowCount() const
{
	int count = 0;
	int h = 0;
	
	if (m_bRowHeaderVisible)
	{
		h += GetHeaderRowHeightValue();
	}
	
	int rows = GetRowCount();
	
	for (int i = m_nVisRowBeg; i < rows; i++)
	{
		if (h >= GetContentHeight())
		{
			break;
		}
		
		count++;
		h += GetRowHeight();
	}
	
	return count;
}

int Grid::GetColSelectIndex() const
{
	return m_nColSelectIndex;
}

int Grid::GetRowSelectIndex() const
{
	return m_nRowSelectIndex;
}

bool Grid::SetRowTitle(int row, const wchar_t* title)
{
	Assert(title != NULL);
	
	if (size_t(row) >= m_RowTitles.size())
	{
		return false;
	}
	
	m_RowTitles[row]->wsTitle = title;
	
	return true;
}

const wchar_t* Grid::GetRowTitle(int row)
{
	if (size_t(row) >= m_RowTitles.size())
	{
		return L"";
	}
	
	return m_RowTitles[row]->wsTitle.c_str();
}

bool Grid::SetColTitle(int col, const wchar_t* title)
{
	Assert(title != NULL);

	if (size_t(col) >= m_ColTitles.size())
	{
		return false;
	}

	m_ColTitles[col]->wsTitle = title;

	return true;
}

const wchar_t* Grid::GetColTitle(int col)
{
	if (size_t(col) >= m_ColTitles.size())
	{
		return L"";
	}
	
	return m_ColTitles[col]->wsTitle.c_str();
}

bool Grid::SetColWidth(int col, int width)
{
	if (width < 0)
	{
		return false;
	}
	
	if (size_t(col) >= m_ColTitles.size())
	{
		return false;
	}
	
	m_ColTitles[col]->nWidth = width;
	UpdateScroll();
	
	return true;
}

int Grid::GetColWidth(int col)
{
	if (size_t(col) >= m_ColTitles.size())
	{
		return 0;
	}
	
	return m_ColTitles[col]->nWidth;
}

bool Grid::SetColWidths(const char* pColWidths)
{
    if (pColWidths == NULL)
    {
        return false;
    }

    m_szColWidths = pColWidths;

	CVarList vData;

	if (GuiUtil_StringToIntList(pColWidths, vData))
	{
		int nClomnWidth = 50;

		for (size_t i = 0; i < vData.GetCount() && i < (size_t)m_nColCount; ++i)
		{
			nClomnWidth = vData.IntVal(i);//atoi(vData.StringVal(i));

			// 允许0不显示
			if (nClomnWidth < 0)
			{
				nClomnWidth = m_nColWidth/*nClomnWidth*/;
			}

			m_ColTitles[i]->nWidth = nClomnWidth;
		}
	}

    UpdateScroll();

    return true;
}

const char* Grid::GetColWidths()
{
    return m_szColWidths.c_str();
}

bool Grid::SetColAlign(int index, const char* align)
{
	Assert(align != NULL);
	
	if (size_t(index) >= m_ColTitles.size())
	{
		return false;
	}
	
	m_ColTitles[index]->nAlign = GuiUtil_AlignToInt(align);
	
	return true;
}

const char* Grid::GetColAlign(int col)
{
	if (size_t(col) >= m_ColTitles.size())
	{
		return "";
	}

	return GuiUtil_IntToAlign(m_ColTitles[col]->nAlign);
}

bool Grid::SetGridText(int row, int col, const wchar_t* text)
{
	Assert(text != NULL);

	if (!IsValidGrid(row, col))
	{
		return false;
	}

	m_Rows[row][col].wsValue = text;

	return true;
}

const wchar_t* Grid::GetGridText(int row, int col) const
{
	if (!IsValidGrid(row, col))
	{
		return L"";
	}
	
    return m_Rows[row][col].wsValue.c_str();
}

bool Grid::SetGridForeColor(int row, int col, const char* value)
{
	Assert(value != NULL);
	
	if (!IsValidGrid(row, col))
	{
		return false;
	}
	
	m_Rows[row][col].nForeColor = GuiUtil_ColorToInt(value);
	
	return true;
}

result_string Grid::GetGridForeColor(int row, int col)
{
	if (!IsValidGrid(row, col))
	{
		return "";
	}
	
	return GuiUtil_IntToColor(m_Rows[row][col].nForeColor).c_str();
}

bool Grid::SetGridBackColor(int row, int col, const char* value)
{
	Assert(value != NULL);
	
	if (!IsValidGrid(row, col))
	{
		return false;
	}
	
	m_Rows[row][col].nBackColor = GuiUtil_ColorToInt(value);
	
	return true;
}

result_string Grid::GetGridBackColor(int row, int col)
{
	if (!IsValidGrid(row, col))
	{
		return "";
	}
	
	return GuiUtil_IntToColor(m_Rows[row][col].nBackColor).c_str();
}

bool Grid::SetGridControl(int row, int col, const PERSISTID& id)
{
	if (!IsValidGrid(row, col))
	{
		return false;
	}

	IControl* control = GetControl(id);

	if (NULL == control)
	{
		return false;
	}

	ClearGridControl(row, col);

	control->SetParentControl(this);
	control->SetDelegateControl(this);

	m_Rows[row][col].pControl = control;

	return true;
}						  

bool Grid::ClearGridControl(int row, int col)
{
	if (!IsValidGrid(row, col))
	{
		return false;
	}

	if (m_Rows[row][col].pControl)
	{
		m_Rows[row][col].pControl->Release();
		m_Rows[row][col].pControl = NULL;
	}
	
	return true;
}

PERSISTID Grid::GetGridControl(int row, int col) const
{
	if (!IsValidGrid(row, col))
	{
		return PERSISTID();
	}

	if (m_Rows[row][col].pControl == NULL)
	{
		return PERSISTID();
	}

	return m_Rows[row][col].pControl->GetID();
}

bool Grid::SetGridTag(int row, int col, const PERSISTID& id)
{
	if (!IsValidGrid(row, col))
	{
		return false;
	}
	
	m_Rows[row][col].TagID = id;

	return true;
}

PERSISTID Grid::GetGridTag(int row, int col)
{
	if (!IsValidGrid(row, col))
	{
		return PERSISTID();
	}

	return m_Rows[row][col].TagID;
}

bool Grid::SelectGrid(int row, int col)
{
	if ((row < 0) || (row >= GetRowCount()))
	{
		return false;
	}
	
	if ((col < 0) || (col >= GetColCount()))
	{
		return false;
	}
	
	SelectCol(col);
	SelectRow(row);
	
	if (GetEnabled())
	{
		GuiUtil_RunCallback(this, "on_select_grid", 
			CVarList() << row << col);
	}
	
	return true;
}

int Grid::DoubleClickGrid(int row, int col)
{
	if ((row != -1) && (col != -1))
	{
		return GuiUtil_RunCallback(this, "on_double_click_grid", 
			CVarList() << row << col);
	}
	
	return 0;
}

bool Grid::ShowSelectRow()
{
	if (m_nRowSelectIndex == -1)
	{
		return false;
	}
	
	if (m_nRowSelectIndex == m_nVisRowBeg)
	{
		return true;
	}
	
	if (m_nRowSelectIndex < m_nVisRowBeg)
	{
		SetVerticalValue(m_nRowSelectIndex);
		return true;
	}
	
	int page_disp = GetCanDispRows();
	
	if (m_nRowSelectIndex < (m_nVisRowBeg + page_disp))
	{
		return true;
	}
	
	int new_index = m_nRowSelectIndex - (page_disp - 1);
	
	if (new_index < 0)
	{
		new_index = 0;
	}
	
	SetVerticalValue(new_index);
	
	return true;
}

bool Grid::SelectRow(int row)
{
	if ((row < 0) || (row >= GetRowCount()))
	{
		return false;
	}
	
	m_nRowSelectIndex = row;

	ShowSelectRow();

	UpdateScroll();
	
	if (GetEnabled())
	{
		GuiUtil_RunCallback(this, "on_select_row", CVarList() << row);
	}
	
	return true;
}

bool Grid::ShowSelectCol()
{
	if (m_nColSelectIndex == -1)
	{
		return false;
	}
	
	if (m_nColSelectIndex == m_nVisColBeg)
	{
		return true;
	}
	
	if (m_nColSelectIndex < m_nVisColBeg)
	{
		SetHorizonValue(m_nColSelectIndex);
		return true;
	}
	
	int page_disp = GetCanDispCols();
	
	if (m_nColSelectIndex < (m_nVisColBeg + page_disp))
	{
		return true;
	}
	
	int new_index = m_nColSelectIndex - (page_disp - 1);
	
	if (new_index < 0)
	{
		new_index = 0;
	}
	
	SetHorizonValue(new_index);
	
	return true;
}

bool Grid::SelectCol(int col)
{
	if ((col < 0) || (col >= GetColCount()))
	{
		return false;
	}
	
	m_nColSelectIndex = col;

	ShowSelectCol();

	UpdateScroll();
	
	if (GetEnabled())
	{
		GuiUtil_RunCallback(this, "on_select_col", CVarList() << col);
	}
	
	return true;
}

bool Grid::ClearSelect()
{
	m_nColSelectIndex = -1;
	m_nRowSelectIndex = -1;

	return true;
}

int Grid::InsertRow(int row)
{
    if (m_nColCount <= 0)
    {
        return -1;
    }

	grid_data_t* pRow = (grid_data_t*)CORE_ALLOC(
		sizeof(grid_data_t) * m_nColCount);

	for (int k = 0; k < m_nColCount; ++k)
	{
		new (&pRow[k]) grid_data_t;
	}

	int res;
	
	if (size_t(row) < m_Rows.size())
	{
		m_Rows.insert(row, pRow);
		m_RowTitles.insert(row, CORE_NEW(row_data_t));
		res = row;
	}
	else
	{
		m_Rows.push_back(pRow);
		m_RowTitles.push_back(CORE_NEW(row_data_t));
		res = (int)m_Rows.size() - 1;
	}
	
	UpdateScroll();
	
	return res;
}

bool Grid::DeleteRow(int row)
{
	if (size_t(row) >= m_Rows.size())
	{
		return false;
	}

	for (int j = 0; j < m_nColCount; ++j)
	{
		SAFE_RELEASE(m_Rows[row][j].pControl);
		m_Rows[row][j].~grid_data_t();
	}

	CORE_FREE(m_Rows[row], sizeof(grid_data_t) * m_nColCount);
    m_Rows[row] = NULL;
	m_Rows.remove(row);
	CORE_DELETE(m_RowTitles[row]);
	m_RowTitles.remove(row);
	UpdateScroll();
	
	return true;
}

bool Grid::ClearRow()
{
	ClearAllRow();
	UpdateScroll();

	return true;
}

void Grid::InGrid(const IVarList& args, IVarList& result)
{
	int x = args.IntVal(0);
	int y = args.IntVal(1);

	int row;
	int col;
	int show_x;
	int show_y;

	GetInGrid(x, y, row, col, show_x, show_y);

	result.AddInt(row);
	result.AddInt(col);
}

int Grid::GetHeaderRowHeightValue() const
{
	if (m_nHeaderRowHeight == 0)
	{
		return m_nRowHeight;
	}

	return m_nHeaderRowHeight;
}

int Grid::GetHeaderColWidthValue() const
{
	if (m_nHeaderColWidth == 0)
	{
		return m_nColWidth;
	}
	
	return m_nHeaderColWidth;
}

unsigned int Grid::GetHeaderBackColorValue() const
{
	if (m_HeaderBackColor.GetValue() == 0)
	{
		return GetBackColor();
	}

	return m_HeaderBackColor.GetValue();
}

unsigned int Grid::GetHeaderForeColorValue() const
{
	if (m_HeaderForeColor.GetValue() == 0)
	{
		return GetForeColor();
	}

	return m_HeaderForeColor.GetValue();
}

int Grid::GetColWidthValue(int index) const
{
	Assert(size_t(index) < m_ColTitles.size());

	if (m_ColTitles[index]->nWidth < 0)
	{
		return m_nColWidth;
	}

	return m_ColTitles[index]->nWidth;
}

int Grid::GetCanDispRows() const
{
	int height = GetContentHeight();

	if (m_bRowHeaderVisible)
	{
		height -= int(GetHeaderRowHeightValue());
	}

	if (height <= 0)
	{
		return 0;
	}

	return (height / GetRowHeight());
}

int Grid::GetCanDispCols() const
{
	int width = GetContentWidth();
	
	if (m_bColHeaderVisible)
	{
		width -= int(GetHeaderColWidthValue());
	}
	
	int count = 0;
	
	int w = 0;
	
	int cols = GetColCount();
	
	for (int i = m_nVisColBeg; i < cols; i++)
	{
		w += GetColWidthValue(i);
		
		if (w >= width)
		{
			break;
		}
		
		count++;
	}
	
	return count;
}

// 重载的模板方法

int Grid::GetVerticalTotal() const 
{ 
	return GetRowCount();
}

int Grid::GetVerticalCanDisp() const 
{ 
	return GetCanDispRows(); 
}

void Grid::SetVerticalValue(int value) 
{
	m_nVisRowBeg = value;
}

int Grid::GetVerticalValue() const 
{ 
	return m_nVisRowBeg; 
}

int Grid::GetHorizonTotal() const 
{ 
	return GetColCount();
}

int Grid::GetHorizonCanDisp() const 
{ 
	return GetCanDispCols(); 
}

void Grid::SetHorizonValue(int value) 
{
	m_nVisColBeg = value;
}

int Grid::GetHorizonValue() const 
{ 
	return m_nVisColBeg; 
}

void Grid::PaintScrollRect(float seconds)
{
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + GetContentHeight();
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	int row_count = GetVisRowCount();
	int col_count = GetVisColCount();

	int x_offset = x1;
	int y_offset = y1;

	if (m_bRowHeaderVisible)
	{
		y_offset += GetHeaderRowHeightValue(); 
	}

	if (m_bColHeaderVisible)
	{
		x_offset += GetHeaderColWidthValue();
	}	
	
	if (m_bRowHeaderVisible && m_bColHeaderVisible)
	{
		GetPainter()->FillRect(x1, y1, 
			x_offset + 1, y_offset + 1, GetHeaderBackColorValue());
		GetPainter()->DrawRect(x1, y1, 
			x_offset + 1, y_offset + 1, GetLineColor());
	}
	
	if (m_bRowHeaderVisible)
	{
		int x = x_offset;
		int y = y1;

		int h = GetHeaderRowHeightValue();

		for (int i = 0; i < col_count; i++)
		{
			int index = m_nVisColBeg + i;

			int w = GetColWidthValue(index);
			
			if (0 == w)
			{
				// 允许0不显示
				continue;
			}

			GetPainter()->FillRect(x, y, 
				x + w + 1, y + h + 1, GetHeaderBackColorValue());
			GetPainter()->DrawRect(x, y, 
				x + w + 1, y + h + 1, GetLineColor());

			x += w;
		}
	}

	if (m_bColHeaderVisible)
	{
		int x = x1;
		int y = y_offset;
		
		int w = GetHeaderColWidthValue();
		
		for (int i = 0; i < row_count; i++)
		{
			int index = m_nVisRowBeg + i;
			
			int h = GetRowHeight();
			
			GetPainter()->FillRect(x, y, 
				x + w + 1, y + h + 1, GetHeaderBackColorValue());
			GetPainter()->DrawRect(x, y, 
				x + w + 1, y + h + 1, GetLineColor());
			
			y += h;
		}
	}

	m_bShowSuborControl = false;
    bool IsSelectRow = false;

	int y = y_offset;

	int r;

	for (r = 0; r < row_count; r++)
	{
		int x = x_offset;
		
		int r_index = m_nVisRowBeg + r;

		int h = GetRowHeight();

		for (int c = 0; c < col_count; c++)
		{
			int c_index = m_nVisColBeg + c;
			
			int w = GetColWidthValue(c_index);

			if (0 == w)
			{
				// 允许0不显示
				continue;
			}

			IControl* control = m_Rows[r_index][c_index].pControl;
			
			if (control != NULL)
			{
				control->SetLeft(x - InnerGetAbsLeft());
				control->SetTop(y - InnerGetAbsTop());
				control->SetWidth(w + 1);
				control->SetHeight(h + 1);
				// 显示这个格子里的控件
				control->Paint(seconds);

				// 设置显示子控件标志
				m_bShowSuborControl = true;
			}
			else
			{
				// 画选中时的背景框
                if (IsSelectGrid(r_index, c_index) 
					&& m_SelectBackDraw.IsReady() && GetCanSelectRow())
                {
                    IsSelectRow = true;
                }
                else
                {
				    unsigned int back_color = 
						m_Rows[r_index][c_index].nBackColor;

				    if (IsSelectGrid(r_index, c_index))
				    {
					    back_color = m_SelectBackColor.GetValue();
				    }
				    else if (0 == back_color)
				    {
					    back_color = GetBackColor();
				    }
    				
				    GetPainter()->FillRect(x, y, 
					    x + w + 1, y + h + 1, back_color);
				    GetPainter()->DrawRect(x, y, 
					    x + w + 1, y + h + 1, GetLineColor());
                }
			}
			
			x += w;
		}

        if (IsSelectRow)
        {
            m_SelectBackDraw.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
                x_offset, y, x_offset + GetContentWidth() + 1, y + h + 1);

            IsSelectRow = false;
        }

		y += h;
	}
	
	// 将画线和文字分开以减少渲染状态的切换
	if (m_bRowHeaderVisible)
	{
		int x = x_offset;
		int y = y1;
		int h = GetHeaderRowHeightValue();
		
		for (int i = 0; i < col_count; i++)
		{
			int index = m_nVisColBeg + i;
			int w = GetColWidthValue(index);
			
			if (0 == w)
			{
				// 允许0不显示
				continue;
			}

			GetPainter()->WriteTextCenter(x, y, x + w + 1, y + h + 1, 
				GetForeColor(), m_ColTitles[index]->wsTitle.c_str());
			x += w;
		}
	}
	
	if (m_bColHeaderVisible)
	{
		int x = x1;
		int y = y_offset;
		int w = GetHeaderColWidthValue();
		
		for (int i = 0; i < row_count; i++)
		{
			int index = m_nVisRowBeg + i;
			int h = GetRowHeight();
			
			GetPainter()->WriteTextCenter(x, y, x + w + 1, y + h + 1, 
				GetForeColor(), m_RowTitles[index]->wsTitle.c_str());
			y += h;
		}
	}
	
	y = y_offset;

	for (r = 0; r < row_count; r++)
	{
		int x = x_offset;
		int r_index = m_nVisRowBeg + r;
		int h = GetRowHeight();
		
		for (int c = 0; c < col_count; c++)
		{
			int c_index = m_nVisColBeg + c;
			int w = GetColWidthValue(c_index);
			
			if (0 == w)
			{
				// 允许0不显示
				continue;
			}

			if (NULL == m_Rows[r_index][c_index].pControl)
			{
				unsigned int fore_color = m_Rows[r_index][c_index].nForeColor;
				
				if (IsSelectGrid(r_index, c_index) 
					&& !m_SelectBackDraw.IsReady())
				{
					fore_color = m_SelectForeColor.GetValue();
				}
				else if (0 == fore_color)
				{
					fore_color = GetForeColor();
				}
				
				wchar_t buffer[256] = { 0 };
				const wchar_t* val = GuiUtil_GetCanShowText(
					GetPainter(), m_Rows[r_index][c_index].wsValue.c_str(), 
					w, L"...", buffer, sizeof(buffer));

				if (m_bShowComment)
				{
					m_Rows[r_index][c_index].bNeedComment = 
						(m_Rows[r_index][c_index].wsValue != val);
				}
				
				if (m_ColTitles[c_index]->nAlign == CGuiConst::ALIGN_LEFT)
				{
					GetPainter()->WriteTextLeft(x, y, 
						x + w + 1, y + h + 1, fore_color, val);
				}
				else if (m_ColTitles[c_index]->nAlign == CGuiConst::ALIGN_RIGHT)
				{
					GetPainter()->WriteTextRight(x, y, 
						x + w + 1, y + h + 1, fore_color, val);
				}
				else
				{
					GetPainter()->WriteTextCenter(x, y, 
						x + w + 1, y + h + 1, fore_color, val);
				}
			}
			
			x += w;
		}
		
		y += h;
	}
}

bool Grid::GetInGrid(int px, int py, int& row_index, int& col_index,
					 int& show_x, int& show_y) const
{
	row_index = -1;
	col_index = -1;
	
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + GetContentHeight();
	int x3 = x1 + GetContentWidth();
	int y3 = y1 + GetContentHeight();
	int x_offset = x1;
	int y_offset = y1;
	
	if (m_bRowHeaderVisible)
	{
		y_offset += GetHeaderRowHeightValue(); 
	}
	
	if (m_bColHeaderVisible)
	{
		x_offset += GetHeaderColWidthValue();
	}	
	
	int col_count = GetVisColCount();
	
	if (m_bRowHeaderVisible)
	{
		int x = x_offset;
		int y = y1;
		int h = GetHeaderRowHeightValue();
		
		for (int i = 0; i < col_count; i++)
		{
			int index = m_nVisColBeg + i;
			int w = GetColWidthValue(index);
			
			if ((x + w) > x3)
			{
				w = x3 - x;
			}
			
			if ((y + h) > y3)
			{
				h = y3 - y;
			}
			
			if (GuiUtil_PointInRect(px, py, x, y, x + w, y + h))
			{
				show_x = x;
				show_y = y;
				col_index = index;
				return true;
			}
			
			x += w;
		}
	}
	
	int row_count = GetVisRowCount();
	
	if (m_bColHeaderVisible)
	{
		int x = x1;
		int y = y_offset;
		int w = GetHeaderColWidthValue();
		
		for (int i = 0; i < row_count; i++)
		{
			int index = m_nVisRowBeg + i;
			int h = GetRowHeight();
			
			if ((x + w) > x3)
			{
				w = x3 - x;
			}
			
			if ((y + h) > y3)
			{
				h = y3 - y;
			}
			
			if (GuiUtil_PointInRect(px, py, x, y, x + w, y + h))
			{
				show_x = x;
				show_y = y;
				row_index = index;
				return true;
			}
			
			y += h;
		}
	}
	
	int y = y_offset;
	
	for (int r = 0; r < row_count; r++)
	{
		int x = x_offset;
		int r_index = m_nVisRowBeg + r;
		int h = GetRowHeight();
		
		for (int c = 0; c < col_count; c++)
		{
			int c_index = m_nVisColBeg + c;
			int w = GetColWidthValue(c_index);
			
			if ((x + w) > x3)
			{
				w = x3 - x;
			}
			
			if ((y + h) > y3)
			{
				h = y3 - y;
			}
			
			if (GuiUtil_PointInRect(px, py, x, y, x + w, y + h))
			{
				show_x = x;
				show_y = y;
				row_index = r_index;
				col_index = c_index;
				return true;
			}
			
			x += w;
		}
		
		y += h;
	}
	
	return false; 
}

int Grid::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}

    if (m_bTransparent)
    {
        return 0;
    }

	int row_index;
	int col_index;
	int show_x;
	int show_y;

	if (GetInGrid(x, y, row_index, col_index, show_x, show_y))
	{
		if ((row_index != -1) && (col_index != -1))
		{
			SelectGrid(row_index, col_index);
			return 1;
		}
		else if (col_index != -1)
		{
			SelectCol(col_index);
			return 1;
		}
	}

	return 1;
}

int Grid::OnLButtonDoubleClick(int x, int y)
{
    if (m_bTransparent)
    {
        return 0;
    }

	int row_index;
	int col_index;
	int show_x;
	int show_y;

	if (GetInGrid(x, y, row_index, col_index, show_x, show_y))
	{
		return DoubleClickGrid(row_index, col_index);
	}

	return 0;
}

int Grid::OnRButtonDown(int x, int y)
{
    if (m_bTransparent)
    {
        return 0;
    }

    GuiUtil_RunCallback(this, "on_right_select_grid", 
		CVarList() << m_nRowMouseInIndex << m_nColMouseInIndex);

    return 0;
}

int Grid::OnDragMove(int x, int y)
{
    if (m_bTransparent)
    {
        return 0;
    }

	return 0;
}

int Grid::OnMouseMove(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
    if (m_bTransparent)
    {
        return 0;
    }

	//if (!m_bShowComment)
	//{
	//	return 0;
	//}

	bool show = false;
	int r_index = -1;
	int c_index = -1;
	int show_x;
	int show_y;

	if (GetInGrid(x, y, r_index, c_index, show_x, show_y))
	{
		if (m_bShowComment && (r_index != -1) && (c_index != -1))
		{
			if (m_Rows[r_index][c_index].bNeedComment 
				&& (NULL == m_Rows[r_index][c_index].pControl))
			{
				m_pCommentLabel->SetVisible(true);
				m_pCommentLabel->SetText(
					m_Rows[r_index][c_index].wsValue.c_str());
				m_pCommentLabel->SetAbsLeft(show_x);
				m_pCommentLabel->SetAbsTop(show_y);

				GetRootControl()->AddTopLayer(m_pCommentLabel->GetID());
				show = true;
			}
		}
	}

	if (!show)
	{
		if (m_pCommentLabel->GetVisible())
		{
			GetRootControl()->RemoveTopLayer(m_pCommentLabel->GetID());
			m_pCommentLabel->SetVisible(false);
		}
	}

    if (r_index != m_nRowMouseInIndex)
    {
    	GuiUtil_RunCallback(this, "on_mousein_row_changed", 
			CVarList() << r_index << m_nRowMouseInIndex);
        m_nRowMouseInIndex = r_index;
    }

    if (c_index != m_nColMouseInIndex)
    {
        GuiUtil_RunCallback(this, "on_mousein_col_changed", 
			CVarList() << c_index << m_nColMouseInIndex);
        m_nColMouseInIndex = c_index;
    }

	return 1;
}

int Grid::OnKeyDown(unsigned int vk)
{
    if (m_bTransparent)
    {
        return 0;
    }

	switch (vk)
	{
	case CGuiInput::KEY_UP:
		{
			int new_index = GetRowSelectIndex() - 1;

			if (new_index < 0)
			{
				new_index = 0;
			}

			SelectRow(new_index);

			return 1;
		}
		break;
	case CGuiInput::KEY_DOWN:
		{
			int new_index = GetRowSelectIndex() + 1;

			if (new_index >= GetRowCount())
			{
				new_index = GetRowCount() - 1;
			}

			SelectRow(new_index);

			return 1;
		}
		break;
	case CGuiInput::KEY_PRIOR:
		{
			int page_disp = GetCanDispRows() - 1;

			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int new_index = GetRowSelectIndex() - page_disp;
			
			if (new_index < 0)
			{
				new_index = 0;
			}

			SelectRow(new_index);
			
			return 1;
		}
		break;
	case CGuiInput::KEY_NEXT:
		{
			int page_disp = GetCanDispRows() - 1;
			
			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int new_index = GetRowSelectIndex() + page_disp;
			
			if (new_index >= GetRowCount())
			{
				new_index = GetRowCount() - 1;
			}
			
			SelectRow(new_index);
			
			return 1;
		}
		break;
	case CGuiInput::KEY_LEFT:
		{
			int new_index = GetColSelectIndex() - 1;

			if (new_index < 0)
			{
				new_index = 0;
			}

			SelectCol(new_index);

			return 1;
		}
		break;
	case CGuiInput::KEY_RIGHT:
		{
			int new_index = GetColSelectIndex() + 1;

			if (new_index >= GetColCount())
			{
				new_index = GetColCount() - 1;
			}

			SelectCol(new_index);

			return 1;
		}
		break;
	case CGuiInput::KEY_HOME:
		{
			if (GuiUtil_ControlPressed())
			{
				int new_index = 0;
				
				SelectRow(new_index);
				
				return 1;
			}
			else
			{
				int new_index = 0;
				
				SelectCol(new_index);

				return 1;
			}
		}
		break;
	case CGuiInput::KEY_END:
		{
			if (GuiUtil_ControlPressed())
			{
				int new_index = GetRowCount() - 1;
				
				SelectRow(new_index);
				
				return 1;
			}
			else
			{
				int new_index = GetColCount() - 1;

				SelectCol(new_index);

				return 1;
			}
		}
		break;
	case CGuiInput::KEY_RETURN:
		{
			DoubleClickGrid(m_nRowSelectIndex, m_nColSelectIndex);

			return 1;
		}
		break;
	default:
		break;
	}
	
	return 0;
}

IControl* Grid::OnGetInWhat(int x, int y)
{
    if (m_bTransparent)
    {
        return 0;
    }

	IControl* pScrollBar = IScrollable::OnGetScrollBar(x, y);

	if (pScrollBar)
	{
		// 选中的是滚动条，则直接返回
		return pScrollBar;
	}

	if (m_bShowSuborControl)
	{
		int row_index;
		int col_index;
		int show_x;
		int show_y;
		
		if (GetInGrid(x, y, row_index, col_index, show_x, show_y))
		{
			if ((row_index != -1) && (col_index != -1))
			{
				IControl* pChild = m_Rows[row_index][col_index].pControl;
				
				if (pChild != NULL)
				{
					IControl* pControl = pChild->GetInWhat(x, y);
					
					if (pControl != NULL)
					{
						return pControl;
					}
				}
			}
		}
	}

	return IScrollable::OnGetInWhat(x, y);
}

bool Grid::RemoveTopLayer(const PERSISTID& id)
{
	if (m_pCommentLabel->GetVisible())
	{
		GetRootControl()->RemoveTopLayer(m_pCommentLabel->GetID());

		m_pCommentLabel->SetVisible(false);
	}

	IControl* control = GetControl(GetGridControl(m_nRowSelectIndex, 
		m_nColSelectIndex));

	if (control)
	{
		control->RemoveTopLayer(this->GetID());
	}

	return true;
}

int Grid::m_nSortClomn = 0;

// Return whether first element is greater than the second
//bool Grid::sort_greater(grid_data_t* elem1, grid_data_t* elem2)
//{
//    return elem1[m_nSortClomn].wsValue > elem2[m_nSortClomn].wsValue;
//}

//bool Grid::sort_less(grid_data_t* elem1, grid_data_t* elem2)
//{
//    return elem1[m_nSortClomn].wsValue < elem2[m_nSortClomn].wsValue;
//}

int __cdecl Grid::sort_greater(const void* elem1, const void* elem2)
{
	grid_data_t* p1 = *(grid_data_t**)elem1 + m_nSortClomn;
	grid_data_t* p2 = *(grid_data_t**)elem2 + m_nSortClomn;
	int res = wcscmp(p1->wsValue.c_str(), p2->wsValue.c_str());

	if (res < 0)
	{
		return 1;
	}
	else if (res > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int __cdecl Grid::sort_less(const void* elem1, const void* elem2)
{
	grid_data_t* p1 = *(grid_data_t**)elem1 + m_nSortClomn;
	grid_data_t* p2 = *(grid_data_t**)elem2 + m_nSortClomn;

	return wcscmp(p1->wsValue.c_str(), p2->wsValue.c_str());
}

bool Grid::SortRows(int clomn, bool bGreater)
{
    if (clomn < 0 || clomn >= m_nColCount)
    {
        return false;
    }

    m_nSortClomn = clomn;

    if (bGreater)
    {
        // 降序
        //sort(m_Rows.begin(), m_Rows.end(), sort_greater);
		qsort(m_Rows.data(), m_Rows.size(), sizeof(grid_data_t*), 
			sort_greater);
    }
    else
    {
        // 升序
        //sort(m_Rows.begin(), m_Rows.end(), sort_less);
		qsort(m_Rows.data(), m_Rows.size(), sizeof(grid_data_t*), sort_less);
    }

    return true;
}

// Return whether first element is greater than the second
//bool Grid::sort_greater_by_int(grid_data_t* elem1, grid_data_t* elem2)
//{
//	int value1 = StringUtil::WideStrAsInt(elem1[m_nSortClomn].wsValue.c_str());
//	int value2 = StringUtil::WideStrAsInt(elem2[m_nSortClomn].wsValue.c_str());
//	return value1 > value2;
//}

//bool Grid::sort_less_by_int(grid_data_t* elem1, grid_data_t* elem2)
//{
//	int value1 = StringUtil::WideStrAsInt(elem1[m_nSortClomn].wsValue.c_str());
//	int value2 = StringUtil::WideStrAsInt(elem2[m_nSortClomn].wsValue.c_str());
//	return value1 < value2;
//}

int __cdecl Grid::sort_greater_by_int(const void* elem1, const void* elem2)
{
	grid_data_t* p1 = *(grid_data_t**)elem1 + m_nSortClomn;
	grid_data_t* p2 = *(grid_data_t**)elem2 + m_nSortClomn;
	int value1 = GuiUtil_WideStrToInt(p1->wsValue.c_str());
	int value2 = GuiUtil_WideStrToInt(p2->wsValue.c_str());

	if (value1 > value2)
	{
		return -1;
	}
	else if (value1 < value2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int __cdecl Grid::sort_less_by_int(const void* elem1, const void* elem2)
{
	grid_data_t* p1 = *(grid_data_t**)elem1 + m_nSortClomn;
	grid_data_t* p2 = *(grid_data_t**)elem2 + m_nSortClomn;
	int value1 = GuiUtil_WideStrToInt(p1->wsValue.c_str());
	int value2 = GuiUtil_WideStrToInt(p2->wsValue.c_str());

	if (value1 < value2)
	{
		return -1;
	}
	else if (value1 > value2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Grid::SortRowsByInt(int clomn, bool bGreater)
{
	if (clomn < 0 || clomn >= m_nColCount)
	{
		return false;
	}

	m_nSortClomn = clomn;

	if (bGreater)
	{
		// 降序
		//sort(m_Rows.begin(), m_Rows.end(), sort_greater_by_int);
		qsort(m_Rows.data(), m_Rows.size(), sizeof(grid_data_t*), 
			sort_greater_by_int);
	}
	else
	{
		// 升序
		//sort(m_Rows.begin(), m_Rows.end(), sort_less_by_int);
		qsort(m_Rows.data(), m_Rows.size(), sizeof(grid_data_t*), 
			sort_less_by_int);
	}

	return true;
}

// 排序(字符排序)有上下区间
bool Grid::SortRowsRang(int clomn, int row_begin, int row_end, bool bGreater)
{
    if (clomn < 0 || clomn >= m_nColCount)
    {
        return false;
    }

    if ((row_begin >= row_end) || (row_begin < 0) 
		|| (row_end > (int)m_Rows.size()))
    {
        return false;
    }

    size_t len = size_t(row_end - row_begin);

    m_nSortClomn = clomn;

    if (bGreater)
    {
        // 降序
        //sort(m_Rows.begin() + row_begin, m_Rows.begin() + row_end, sort_greater);
		qsort(m_Rows.data() + row_begin, len, sizeof(grid_data_t*), sort_greater);
    }
    else
    {
        // 升序
        //sort(m_Rows.begin() + row_begin, m_Rows.begin() + row_end, sort_less);
		qsort(m_Rows.data() + row_begin, len, sizeof(grid_data_t*), sort_less);
    }

    return true;
}
