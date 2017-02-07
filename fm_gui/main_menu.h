//--------------------------------------------------------------------
// 文件名:		main_menu.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// 主菜单条

class Menu;

class MainMenu: public IControl
{
private:
	// 边距
	enum { PADDING_WIDTH = 2 };
	// 缺省菜单标题宽度
	enum { DEF_ITEM_WIDTH = 40 };
	
public:
	MainMenu();
	virtual ~MainMenu();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnMouseMove(int x, int y);

	// 菜单标题宽度
	void SetItemWidth(int value);
	int GetItemWidth() const;

	// 新建并加入菜单
	PERSISTID CreateMenu(const char* name, const wchar_t* text);
	// 查找菜单
	PERSISTID FindMenu(const char* name);
	// 获得菜单数量
	int GetMenuCount() const;
	// 获得指定位置菜单
	PERSISTID GetMenu(size_t index);
	// 复位菜单
	bool ResetMenu();
	
private:
	Menu* GetMenuControl(const PERSISTID& id) const;
	// 处在哪个菜单中
	Menu* GetInMenu(int x, int y) const;
	bool FindMenuIndex(const char* name, size_t& index) const;
	// 显示菜单
	bool ShowMenu(Menu* pMenu);
	// 隐藏菜单
	bool HideMenu(Menu* pMenu);
	
private:
	int m_nItemWidth;
	bool m_bPush;
	PERSISTID m_SelectMenu;
	TArrayPod<Menu*, 1, TCoreAlloc> m_Menus;
};

#endif // _MAIN_MENU_H
