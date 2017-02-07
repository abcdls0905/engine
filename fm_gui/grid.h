//--------------------------------------------------------------------
// 文件名:		grid.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GRID_H
#define _GRID_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

class TGridAlloc
{
public:
	TGridAlloc() {}
	~TGridAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TGridAlloc& src) {}
};

// 表格

class Label;

class Grid: public IScrollable
{
private:
	enum { HEADER_FORE_COLOR = 0xFF000000 };
	enum { HEADER_BACK_COLOR = 0xFFFFFFFF };
	
	// 列表头数据
	struct col_data_t
	{
		core_wstring wsTitle;
		int nWidth;
		int nAlign;	

		col_data_t()
		{
			nWidth = 0;
			nAlign = 1;
		}
	};

	// 行表头数据
	struct row_data_t
	{
		core_wstring wsTitle;
	};
	
	// 格子数据
	struct grid_data_t
	{
		core_wstring wsValue;
		unsigned int nForeColor;
		unsigned int nBackColor;
		PERSISTID TagID;
		IControl* pControl;
		bool bNeedComment;

		grid_data_t()
		{
			nForeColor = 0;
			nBackColor = 0;
			pControl = NULL;
			bNeedComment = false;
		}
	};

public:
	Grid();
	virtual ~Grid();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
    virtual int OnRButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnMouseMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual IControl* OnGetInWhat(int x, int y);

	virtual bool RemoveTopLayer(const PERSISTID& id);

	// 获得注释标签对象
	PERSISTID GetCommentLabelID() const;

	// 是否显示注释信息
	void SetShowComment(bool value);
	bool GetShowComment() const;
	
	// 行表头高度
	void SetHeaderRowHeight(int value);
	int GetHeaderRowHeight() const;
	
	// 行表头宽度
	void SetHeaderColWidth(int value);
	int GetHeaderColWidth() const;
	
	// 表头背景颜色
	void SetHeaderBackColorString(const char* value);
	result_string GetHeaderBackColorString() const;
	
	// 表头前景颜色
	void SetHeaderForeColorString(const char* value);
	result_string GetHeaderForeColorString() const;
	
	// 是否显示行表头
	void SetRowHeaderVisible(bool value);
	bool GetRowHeaderVisible() const;
	
	// 是否显示列表头
	void SetColHeaderVisible(bool value);
	bool GetColHeaderVisible() const;
	
	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
    // 表格的选择背景图
    void SetSelectBackDraw(const char* value);
    const char* GetSelectBackDraw() const;

	// 能否选择列
	void SetCanSelectCol(bool value);
	bool GetCanSelectCol() const;
	
	// 能否选择行
	void SetCanSelectRow(bool value);
	bool GetCanSelectRow() const;
	
	// 列宽度
	void SetColWidth(int value);
	int GetColWidth() const;
	
	// 行高度
	void SetRowHeight(int value);
	int GetRowHeight() const;
	
	// 表列数
	void SetColCount(int value);
	int GetColCount() const;
	
	// 表行数
	void SetRowCount(int value);
	int GetRowCount() const;
	
    // 透明
    void SetTransparent(bool bTrans);
    const bool GetTransparent();

	// 当前在显示区的行列数
	int GetVisColCount() const;
	int GetVisRowCount() const;
	
	// 获得被选中的行列号
	int GetColSelectIndex() const;
	int GetRowSelectIndex() const;

	// 行表头
	bool SetRowTitle(int row, const wchar_t* title);
	const wchar_t* GetRowTitle(int row);

	// 列表头
	bool SetColTitle(int col, const wchar_t* title);
	const wchar_t* GetColTitle(int col);

	// 设置指定列的宽度
	bool SetColWidth(int col, int width);
	int GetColWidth(int col);

    // 设置所有列的宽度
    bool SetColWidths(const char* value);
    const char* GetColWidths();

	// 列的对齐方式
	bool SetColAlign(int col, const char* align);
	const char* GetColAlign(int col);
	
	// 指定格子的文字
	bool SetGridText(int row, int col, const wchar_t* text);
	const wchar_t* GetGridText(int row, int col) const;
	
	// 指定格子的前景色
	bool SetGridForeColor(int row, int col, const char* value);
	result_string GetGridForeColor(int row, int col);
	
	// 指定格子的背景色
	bool SetGridBackColor(int row, int col, const char* value);
	result_string GetGridBackColor(int row, int col);

	// 设置指定格子为显示控件
	bool SetGridControl(int row, int col, const PERSISTID& id);
	// 清除指定格子的控件
	bool ClearGridControl(int row, int col);
	// 获得指定格子的控件
	PERSISTID GetGridControl(int row, int col) const;

	// 格子的关联对象
	bool SetGridTag(int row, int col, const PERSISTID& id);
	PERSISTID GetGridTag(int row, int col);

	// 选择格子
	bool SelectGrid(int row, int col);
	// 选择行
	bool SelectRow(int row);
	// 选择列
	bool SelectCol(int col);
	// 清除选择
	bool ClearSelect();

	// 插入行
	int InsertRow(int row);
	// 删除行
	bool DeleteRow(int row);
	// 清除所有行
	bool ClearRow();

	// 指定位置在哪个格子里
	void InGrid(const IVarList& args, IVarList& result);

	// 排序(字符排序)
    bool SortRows(int clomn, bool bGreater);

    // 排序(字符排序)有上下区间
    bool SortRowsRang(int clomn, int row_begin, int row_end, bool bGreater);

	// 排序(按INT类型排序)
	bool SortRowsByInt(int clomn, bool bGreater);
	virtual int GetVerticalValue() const;
	virtual int GetHorizonValue() const;

private:
	virtual int GetVerticalTotal() const;
	virtual void SetVerticalValue(int value);
	virtual int GetVerticalCanDisp() const;
	
	virtual int GetHorizonTotal() const;
	virtual int GetHorizonCanDisp() const;
	virtual void SetHorizonValue(int value);
	
	virtual void PaintScrollRect(float seconds);
	
private:
	int GetHeaderRowHeightValue() const;
	int GetHeaderColWidthValue() const;
	unsigned int GetHeaderBackColorValue() const;
	unsigned int GetHeaderForeColorValue() const;
	int GetColWidthValue(int index) const;

	// 清除所有表格数据
	void ClearAllRow();
	void ClearColTitles();
	void ClearRowTitles();

	bool IsValidGrid(int row, int col) const;
	bool IsSelectGrid(int row, int col) const;
	
	// 显示选择行
	bool ShowSelectRow();
	// 显示选择列
	bool ShowSelectCol();

	int GetCanDispRows() const;
	int GetCanDispCols() const;

	bool GetInGrid(int px, int py, int& row_index, int& col_index,
		int& show_x, int& show_y) const;

	int DoubleClickGrid(int row, int col);

    //static bool sort_greater(grid_data_t* elem1, grid_data_t* elem2);
    //static bool sort_less(grid_data_t* elem1, grid_data_t* elem2);
	static int __cdecl sort_greater(const void* elem1, const void* elem2);
	static int __cdecl sort_less(const void* elem1, const void* elem2);
	//static bool sort_greater_by_int(grid_data_t* elem1, grid_data_t* elem2);
	//static bool sort_less_by_int(grid_data_t* elem1, grid_data_t* elem2);
	static int __cdecl sort_greater_by_int(const void* elem1, 
		const void* elem2);
	static int __cdecl sort_less_by_int(const void* elem1, 
		const void* elem2);

private:
    static int m_nSortClomn;
	bool m_bShowSuborControl;
	bool m_bShowComment;
	bool m_bRowHeaderVisible;
	bool m_bColHeaderVisible;
    bool m_bTransparent;
	int m_nColWidth;
	int m_nRowHeight;
	int m_nColCount;
	int m_nVisColBeg;
	int m_nVisRowBeg;
	bool m_bCanSelectCol;
	bool m_bCanSelectRow;
	int m_nColSelectIndex;
	int m_nRowSelectIndex;
    int m_nRowMouseInIndex;
    int m_nColMouseInIndex;
	int m_nHeaderRowHeight;
	int m_nHeaderColWidth;
    core_string m_szColWidths;
	CColorData m_HeaderBackColor;
	CColorData m_HeaderForeColor;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	TArrayPod<col_data_t*, 1, TGridAlloc> m_ColTitles;
	TArrayPod<row_data_t*, 1, TGridAlloc> m_RowTitles;
	TArrayPod<grid_data_t*, 1, TGridAlloc> m_Rows;
	Label* m_pCommentLabel;
    CImagePlayer m_SelectBackDraw;
};

#endif // _GRID_H
