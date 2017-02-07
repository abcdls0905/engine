//--------------------------------------------------------------------
// 文件名:		list_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LIST_BOX_H
#define _LIST_BOX_H

#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"

class TListBoxAlloc
{
public:
	TListBoxAlloc() {}
	~TListBoxAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TListBoxAlloc& src) {}
};

// 列表框

class ListBox: public IScrollable
{
private:
	enum { MIN_ITEM_HEIGHT = 4 };

	struct item_t
	{
		core_wstring wsValue;
		PERSISTID TagID;
		int nWidth;
		unsigned int nColor;
	};

public:
	ListBox();
	virtual ~ListBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnMouseMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);

	// 是否排序
	void SetSorted(bool value);
	bool GetSorted() const;
	
	// 设置鼠标移动能选取字符串功能
	void SetMouseSelect(bool val);
	bool GetMouseSelect() const;

	// 每行的高度
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// 文字起始位移
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;
	
	// 行数
	int GetItemCount() const;

	// 选中的行号
	void SetSelectIndex(int value);
	int GetSelectIndex() const;

	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// 选中时的背景贴图
	void SetSelectBackImage(const char* value);
	const char* GetSelectBackImage() const;
	
	// 选中的字符串
	void SetSelectString(const wchar_t* value);
	const wchar_t* GetSelectString() const;
	
	// 查找字符串
	int FindString(const wchar_t* item) const;
	// 添加字符串
	int AddString(const wchar_t* item);
	// 删除字符串
	bool RemoveString(const wchar_t* item);
	// 删除指定行
	bool RemoveByIndex(int index);
	// 清除所有行
	bool ClearString();
	// 获得指定行的字符串
	const wchar_t* GetString(int index) const;
    // 修改指定行的字符串
    bool ChangeString(int index, const wchar_t* value);
	// 列表项的颜色
	bool SetItemColor(int index, const char* value);
	result_string GetItemColor(int index) const;

	// 行数据的关联对象
	bool SetTag(int index, const PERSISTID& id);
	PERSISTID GetTag(int index) const;
	
private:
	virtual int GetVerticalTotal() const;
	virtual int GetVerticalCanDisp() const;
	virtual void SetVerticalValue(int value);
	virtual int GetVerticalValue() const;
	
	virtual int GetHorizonTotal() const;
	virtual int GetHorizonCanDisp() const;
	virtual void SetHorizonValue(int value);
	virtual int GetHorizonValue() const;
	
	virtual void PaintScrollRect(float seconds);
	
private:
	// 显示选择项
	bool ShowSelectItem();
	// 当前可以显示项数量
	int GetCanDispCount() const;
	// 获得当前需要显示的行数
	int GetDispItemCount() const;
	// 定位
	int GetInItem(int x, int y) const;

	// 计算总的平滑滚动尺寸
	void CalcScrollTotal();
	
	// 使用索引方式添加字符
	int AddStringSort(const wchar_t* item, int beg, int end);
	// 排序
	bool ProcessSort();

	// 单击项
	int ClickItem(int select_index);
	// 双击项
	int DoubleClickItem(int select_index);

private:
	bool m_bSorted;
	bool m_bMouseSelect;
	int m_nItemHeight;
	int m_nTextOffsetX;
	int m_nScrollWidth;
	int m_nScrollLeft;
	int m_nTopIndex;
	int m_nSelectIndex;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CImagePlayer m_SelectBackImage;
	TArrayPod<item_t*, 1, TListBoxAlloc> m_Items;
};

#endif // _LIST_BOX_H
