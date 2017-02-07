//--------------------------------------------------------------------
// 文件名:		menu_item.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "image_player.h"

// 菜单项

class Gui;
class IPainter;

class MenuItem: public IEntity
{
public:
	MenuItem();
	virtual ~MenuItem();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 是否加载完成
	bool GetLoadFinish();

	// 名字
	void SetName(const char* value);
	const char* GetName() const;
	unsigned int GetHash() const;

	// 文本
	void SetText(const wchar_t* value);
	const wchar_t* GetText() const;

	// 是否禁止
	void SetDisable(bool value);
	bool GetDisable() const;

	// 是否Checked
	void SetChecked(bool value);
	bool GetChecked() const;

	// 图标
	void SetIcon(const char* pDraw);
	const char* GetIcon();

	// 所属菜单
	void SetMenuID(const PERSISTID& id);
	PERSISTID GetMenuID() const;

	// 子菜单
    void SetSubMenuID(const PERSISTID& id);
    PERSISTID GetSubMenuID() const;

	// 为Menu类提供的接口
	bool IsHasSubMenu();

	// 设置Gui与Paiter对象
	void SetPainter(IPainter* pPainter);
	void SetGui(Gui* pGui);

	// 获得图标
	CImagePlayer* GetImagePlayer();

private:
	core_string m_strName;
	unsigned int m_nHash;
	core_wstring m_wsText;
	
	bool m_bDisable;
	bool m_bChecked;

	CImagePlayer m_Icon;	// 图标

	PERSISTID m_MenuID;		// 所属菜单
    PERSISTID m_SubMenuID;	// 子菜单

	char m_cHotKey;			// 热键

	IPainter* m_pPainter;
	Gui* m_pGui;
};

#endif // _MENU_ITEM_H