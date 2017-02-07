//--------------------------------------------------------------------
// �ļ���:		menu_item.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "image_player.h"

// �˵���

class Gui;
class IPainter;

class MenuItem: public IEntity
{
public:
	MenuItem();
	virtual ~MenuItem();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ƿ�������
	bool GetLoadFinish();

	// ����
	void SetName(const char* value);
	const char* GetName() const;
	unsigned int GetHash() const;

	// �ı�
	void SetText(const wchar_t* value);
	const wchar_t* GetText() const;

	// �Ƿ��ֹ
	void SetDisable(bool value);
	bool GetDisable() const;

	// �Ƿ�Checked
	void SetChecked(bool value);
	bool GetChecked() const;

	// ͼ��
	void SetIcon(const char* pDraw);
	const char* GetIcon();

	// �����˵�
	void SetMenuID(const PERSISTID& id);
	PERSISTID GetMenuID() const;

	// �Ӳ˵�
    void SetSubMenuID(const PERSISTID& id);
    PERSISTID GetSubMenuID() const;

	// ΪMenu���ṩ�Ľӿ�
	bool IsHasSubMenu();

	// ����Gui��Paiter����
	void SetPainter(IPainter* pPainter);
	void SetGui(Gui* pGui);

	// ���ͼ��
	CImagePlayer* GetImagePlayer();

private:
	core_string m_strName;
	unsigned int m_nHash;
	core_wstring m_wsText;
	
	bool m_bDisable;
	bool m_bChecked;

	CImagePlayer m_Icon;	// ͼ��

	PERSISTID m_MenuID;		// �����˵�
    PERSISTID m_SubMenuID;	// �Ӳ˵�

	char m_cHotKey;			// �ȼ�

	IPainter* m_pPainter;
	Gui* m_pGui;
};

#endif // _MENU_ITEM_H