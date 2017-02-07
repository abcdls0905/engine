//--------------------------------------------------------------------
// �ļ���:		grid.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TGridAlloc& src) {}
};

// ���

class Label;

class Grid: public IScrollable
{
private:
	enum { HEADER_FORE_COLOR = 0xFF000000 };
	enum { HEADER_BACK_COLOR = 0xFFFFFFFF };
	
	// �б�ͷ����
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

	// �б�ͷ����
	struct row_data_t
	{
		core_wstring wsTitle;
	};
	
	// ��������
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
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
    virtual int OnRButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnMouseMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual IControl* OnGetInWhat(int x, int y);

	virtual bool RemoveTopLayer(const PERSISTID& id);

	// ���ע�ͱ�ǩ����
	PERSISTID GetCommentLabelID() const;

	// �Ƿ���ʾע����Ϣ
	void SetShowComment(bool value);
	bool GetShowComment() const;
	
	// �б�ͷ�߶�
	void SetHeaderRowHeight(int value);
	int GetHeaderRowHeight() const;
	
	// �б�ͷ���
	void SetHeaderColWidth(int value);
	int GetHeaderColWidth() const;
	
	// ��ͷ������ɫ
	void SetHeaderBackColorString(const char* value);
	result_string GetHeaderBackColorString() const;
	
	// ��ͷǰ����ɫ
	void SetHeaderForeColorString(const char* value);
	result_string GetHeaderForeColorString() const;
	
	// �Ƿ���ʾ�б�ͷ
	void SetRowHeaderVisible(bool value);
	bool GetRowHeaderVisible() const;
	
	// �Ƿ���ʾ�б�ͷ
	void SetColHeaderVisible(bool value);
	bool GetColHeaderVisible() const;
	
	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
    // ����ѡ�񱳾�ͼ
    void SetSelectBackDraw(const char* value);
    const char* GetSelectBackDraw() const;

	// �ܷ�ѡ����
	void SetCanSelectCol(bool value);
	bool GetCanSelectCol() const;
	
	// �ܷ�ѡ����
	void SetCanSelectRow(bool value);
	bool GetCanSelectRow() const;
	
	// �п��
	void SetColWidth(int value);
	int GetColWidth() const;
	
	// �и߶�
	void SetRowHeight(int value);
	int GetRowHeight() const;
	
	// ������
	void SetColCount(int value);
	int GetColCount() const;
	
	// ������
	void SetRowCount(int value);
	int GetRowCount() const;
	
    // ͸��
    void SetTransparent(bool bTrans);
    const bool GetTransparent();

	// ��ǰ����ʾ����������
	int GetVisColCount() const;
	int GetVisRowCount() const;
	
	// ��ñ�ѡ�е����к�
	int GetColSelectIndex() const;
	int GetRowSelectIndex() const;

	// �б�ͷ
	bool SetRowTitle(int row, const wchar_t* title);
	const wchar_t* GetRowTitle(int row);

	// �б�ͷ
	bool SetColTitle(int col, const wchar_t* title);
	const wchar_t* GetColTitle(int col);

	// ����ָ���еĿ��
	bool SetColWidth(int col, int width);
	int GetColWidth(int col);

    // ���������еĿ��
    bool SetColWidths(const char* value);
    const char* GetColWidths();

	// �еĶ��뷽ʽ
	bool SetColAlign(int col, const char* align);
	const char* GetColAlign(int col);
	
	// ָ�����ӵ�����
	bool SetGridText(int row, int col, const wchar_t* text);
	const wchar_t* GetGridText(int row, int col) const;
	
	// ָ�����ӵ�ǰ��ɫ
	bool SetGridForeColor(int row, int col, const char* value);
	result_string GetGridForeColor(int row, int col);
	
	// ָ�����ӵı���ɫ
	bool SetGridBackColor(int row, int col, const char* value);
	result_string GetGridBackColor(int row, int col);

	// ����ָ������Ϊ��ʾ�ؼ�
	bool SetGridControl(int row, int col, const PERSISTID& id);
	// ���ָ�����ӵĿؼ�
	bool ClearGridControl(int row, int col);
	// ���ָ�����ӵĿؼ�
	PERSISTID GetGridControl(int row, int col) const;

	// ���ӵĹ�������
	bool SetGridTag(int row, int col, const PERSISTID& id);
	PERSISTID GetGridTag(int row, int col);

	// ѡ�����
	bool SelectGrid(int row, int col);
	// ѡ����
	bool SelectRow(int row);
	// ѡ����
	bool SelectCol(int col);
	// ���ѡ��
	bool ClearSelect();

	// ������
	int InsertRow(int row);
	// ɾ����
	bool DeleteRow(int row);
	// ���������
	bool ClearRow();

	// ָ��λ�����ĸ�������
	void InGrid(const IVarList& args, IVarList& result);

	// ����(�ַ�����)
    bool SortRows(int clomn, bool bGreater);

    // ����(�ַ�����)����������
    bool SortRowsRang(int clomn, int row_begin, int row_end, bool bGreater);

	// ����(��INT��������)
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

	// ������б������
	void ClearAllRow();
	void ClearColTitles();
	void ClearRowTitles();

	bool IsValidGrid(int row, int col) const;
	bool IsSelectGrid(int row, int col) const;
	
	// ��ʾѡ����
	bool ShowSelectRow();
	// ��ʾѡ����
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
