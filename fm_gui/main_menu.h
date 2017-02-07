//--------------------------------------------------------------------
// �ļ���:		main_menu.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"

// ���˵���

class Menu;

class MainMenu: public IControl
{
private:
	// �߾�
	enum { PADDING_WIDTH = 2 };
	// ȱʡ�˵�������
	enum { DEF_ITEM_WIDTH = 40 };
	
public:
	MainMenu();
	virtual ~MainMenu();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnMouseMove(int x, int y);

	// �˵�������
	void SetItemWidth(int value);
	int GetItemWidth() const;

	// �½�������˵�
	PERSISTID CreateMenu(const char* name, const wchar_t* text);
	// ���Ҳ˵�
	PERSISTID FindMenu(const char* name);
	// ��ò˵�����
	int GetMenuCount() const;
	// ���ָ��λ�ò˵�
	PERSISTID GetMenu(size_t index);
	// ��λ�˵�
	bool ResetMenu();
	
private:
	Menu* GetMenuControl(const PERSISTID& id) const;
	// �����ĸ��˵���
	Menu* GetInMenu(int x, int y) const;
	bool FindMenuIndex(const char* name, size_t& index) const;
	// ��ʾ�˵�
	bool ShowMenu(Menu* pMenu);
	// ���ز˵�
	bool HideMenu(Menu* pMenu);
	
private:
	int m_nItemWidth;
	bool m_bPush;
	PERSISTID m_SelectMenu;
	TArrayPod<Menu*, 1, TCoreAlloc> m_Menus;
};

#endif // _MAIN_MENU_H
