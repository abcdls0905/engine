//--------------------------------------------------------------------
// 文件名:		text_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TEXT_BOX_H
#define _TEXT_BOX_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_scrollable.h"
#include "mark_string.h"

// 文本显示框

class CMarkString;

class TextBox: public IScrollable
{
private:
	enum { MIN_ITEM_HEIGHT = 4 };

	// 文字段落
	struct chapter_t
	{
		core_wstring wsText;
		PERSISTID TagID;
		size_t nStart;
		size_t nCount;
	};

	struct item_t
	{
		CMarkString* pMarkStr;
		size_t nChapter;
		unsigned int nForeColor;
	};
	
public:
	TextBox();
	virtual ~TextBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	// 设置鼠标形状
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnKeyDown(unsigned int vk);

	// 是否显示被选中项
	void SetShowSelect(bool value);
	bool GetShowSelect() const;

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

	// 选中的段落
	void SetSelectChapter(int value);
	int GetSelectChapter() const;
	
	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
	// 选中时的背景贴图
	void SetSelectBackImage(const char* value);
	const char* GetSelectBackImage() const;
	
	// 引用项的鼠标形状
	void SetRefCursor(const char* value);
	const char* GetRefCursor() const;

	// 是否平滑滚动
	void SetScrollSmooth(bool value);
	bool GetScrollSmooth() const;

	// 总的平滑滚动高度
	int GetScrollHeight() const;
	// 平滑滚动每页的高度
	int GetScrollPage() const;

	// 平滑滚动的顶部位置
	void SetScrollTop(int value);
	int GetScrollTop() const;
	
	// 添加文字段落
	int AddChapter(const wchar_t* text);
	// 删除指定段落
	bool RemoveChapter(size_t index);
	bool RemoveChapterRange(size_t beg, size_t end);
	// 清除所有段落
	bool ClearChapter();
	// 应用新的控件宽度刷新段落排列
	bool RefreshChapter();
	// 获得段落数量
	int GetChapterCount() const;
	// 获得指定段落文字
	const wchar_t* GetChapter(size_t index) const;
	// 段落的关联对象
	bool SetChapterTag(size_t index, const PERSISTID& id);
	PERSISTID GetChapterTag(size_t index) const;
	// 设置段落文字颜色
	bool SetChapterForeColor(size_t index, const char* color);

	// 加载文字
	bool LoadText(const wchar_t* text);
	// 加载文本文件
	bool LoadFile(const char* file);
	// 保存到文本文件
	bool SaveFile(const char* file);
	
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

	// 清空显示项
	void ClearItems();
	// 生成段落
	bool GenChapter(size_t index);

	// 计算总的平滑滚动尺寸
	void CalcScrollTotal();

	// 单击项
	int ClickItem(int select_index);
	// 双击项
	int DoubleClickItem(int select_index);

private:
	bool m_bShowSelect;
	int m_nItemHeight;
	int m_nTextOffsetX;
	int m_nTopIndex;
	int m_nSelectIndex;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	CImagePlayer m_SelectBackImage;
	core_string m_strRefCursor;
	TArrayPod<item_t, 1, TCoreAlloc> m_Items;
	TArrayPod<chapter_t*, 1, TCoreAlloc> m_Chapters;
	bool m_bScrollSmooth;
	int m_nScrollHeight;
	int m_nScrollTop;
};

#endif // _TEXT_BOX_H
