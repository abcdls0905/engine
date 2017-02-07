//--------------------------------------------------------------------
// 文件名:		menu.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MENU_H
#define _MENU_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// 菜单

class MenuItem;

class Menu: public IControl
{
private:
	// 分隔线的高度
	enum { SEPERATOR_HEIGHT = 8 };
	// 边距
	enum { PADDING_WIDTH = 2 };
	// 子菜单标志的右边距
    enum { SUBFLAG_WIDTH = 8};

public:
	Menu();
	virtual ~Menu();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonUp(int x, int y);
	virtual int OnMouseMove(int x, int y);
    virtual int OnLButtonDown(int x, int y);
    virtual int OnDragMove(int x, int y);

	// 菜单项的高度
	void SetItemHeight(int value);
	int GetItemHeight() const;

	// 左边图标条的宽度
	void SetLeftBarWidth(int value);
	int GetLeftBarWidth() const;

	// 选中时的颜色
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// 选中时的背景颜色
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;

	// 选中时的边框颜色
	void SetSelectBorderColorString(const char* value);
	result_string GetSelectBorderColorString() const;

    // 
    void SetSelectBarDraw(const char* pDraw);
    const char* GetSelectBarDraw();

	// 左侧图标条的背景颜色
	void SetLeftBarBackColorString(const char* value);
	result_string GetLeftBarBackColorString() const;

	// 
	void SetLeftBarDraw(const char*pDraw);
	const char* GetLeftBarDraw();

	// 分隔条的颜色
	void SetSplitColorString(const char* value);
	result_string GetSplitColorString() const;

	//
    void SetSplitDraw(const char* pDraw);
    const char* GetSplitDraw();

    void SetSubFlagDraw(const char* pDraw);
    const char* GetSubFlagDraw();

	void SetCheckedDraw(const char* pDraw);
	const char* GetCheckedDraw();

    void SetCenter(bool IsCenter);
    bool GetCenter();

	// 左边图标条
	void SetLeftBar(bool bLeftBar);
	bool GetLeftBar();

	// 新建菜单项
	PERSISTID NewItem(const char* name, const wchar_t* text);
	// 新建并加入菜单项
	PERSISTID CreateItem(const char* name, const wchar_t* text);
	// 添加菜单项
	bool AddItem(const PERSISTID& id);
	// 插入菜单项
	bool InsertItem(const PERSISTID& id, const char* before);
	// 移除菜单项
	bool RemoveItem(const char* name);
	// 删除菜单项
	bool DeleteItem(const char* name);
	// 清空菜单项
	bool ClearItem();
	// 删除所有菜单项
	bool DeleteAllItem();
	// 查找菜单项
	PERSISTID FindItem(const char* name);
	// 获得菜单项数量
	int GetItemCount() const;
	// 获得指定位置菜单项
	PERSISTID GetItem(size_t index);

    void SetVisible(bool val);

private:
	MenuItem* GetMenuItem(const PERSISTID& id) const;

	// 是否是分隔线
	bool IsSeperator(const MenuItem* pItem) const;

	// 获得总的菜单高度
	int CalcTotalHeight() const;

	// 获得菜单宽度
	int CalcTotalWidth() const;

	// 处在哪个菜单项中
	MenuItem* GetInItem(int x, int y, int& itemRight, int& itemTop) const;
	
	bool FindItemIndex(const char* name, size_t& index) const;

private:
	int m_nItemHeight;				// 每个菜单项的高度
	bool m_IsCenter;				// 菜单项文字的水平对齐方式是否居中，默认为左对齐
	bool m_bLeftBar;				// 是否有左边图标条
	int m_nLeftBarWidth;			// 左边图标条的宽度

    int m_nDragOldX;
    int m_nDragOldY;

	CColorData m_SelectForeColor;	// 选择项的前景色
	CColorData m_SelectBackColor;	// 选择项的背景色
	CColorData m_SelectBorderColor;	// 选择项的边框色
	CImagePlayer m_SelectBarDraw;	// 选择条图元

	CColorData m_LeftBarBackColor;	// 图标条的背景色
	CImagePlayer m_LeftBarDraw;		// 图标条图元

	CColorData m_SplitColor;		// 分隔条的颜色
	CImagePlayer m_SplitDraw;		// 分隔条图元
	CImagePlayer m_SubFlagDraw;		// 子菜单箭头
	CImagePlayer m_CheckedDraw;		// 被Checked的菜单项的图标

	PERSISTID m_SelectItem;			// 当前被选择的菜单项
    PERSISTID m_ShowSubMenu;		// 当前显示的子菜单

	TArrayPod<MenuItem*, 1, TCoreAlloc> m_Items; // 所有子菜单
};

#endif // _MENU_H
