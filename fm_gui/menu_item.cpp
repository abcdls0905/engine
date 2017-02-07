//--------------------------------------------------------------------
// �ļ���:		menu_item.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "menu_item.h"
#include "../public/inlines.h"
#include "../visual/i_painter.h"

/// \file menu_item.cpp
/// \brief �˵���

/// entity: MenuItem
/// \brief �˵���ʵ��
DECLARE_ENTITY(MenuItem, 0, IEntity);

/// readonly: object Menu
/// \brief �����ڵĲ˵�����
DECLARE_PROPERTY_GET(PERSISTID, MenuItem, Menu, GetMenuID);

/// property: object SubMenu
/// \brief �Ӳ˵�����
DECLARE_PROPERTY(PERSISTID, MenuItem, SubMenu, GetSubMenuID, SetSubMenuID);

/// property: string Name
/// \brief ����
DECLARE_PROPERTY(const char*, MenuItem, Name, GetName, SetName);
/// property: wstring Text
/// \brief ��ʾ���ı�
DECLARE_PROPERTY(const wchar_t*, MenuItem, Text, GetText, SetText);
/// property: bool Disable
/// \brief �Ƿ񱻽�ֹ
DECLARE_PROPERTY(bool, MenuItem, Disable, GetDisable, SetDisable);
/// property: bool Checked
/// \brief �Ƿ�Checked
DECLARE_PROPERTY(bool, MenuItem, Checked, GetChecked, SetChecked);
/// property: const char * icon
/// \brief ͼ��
DECLARE_PROPERTY(const char *, MenuItem, Icon, GetIcon, SetIcon);

// MenuItem

MenuItem::MenuItem()
{
	m_nHash = 0;
	m_bDisable = false;
	m_bChecked = false;
	m_pPainter = NULL;
	m_pGui = NULL;
}

MenuItem::~MenuItem()
{
	//if (!m_SubMenuID.IsNull() && GetCore())
    //{
    //    GetCore()->DeleteEntity(m_SubMenuID);
    //}

	//if (m_pPainter != NULL)
	//{
	//	m_Icon.Release();
	//}
}

bool MenuItem::Init(const IVarList& args)
{
	return true;
}

bool MenuItem::Shut()
{
	if (!m_SubMenuID.IsNull())
	{
		GetCore()->DeleteEntity(m_SubMenuID);
	}

	m_Icon.Release();

	return true;
}

bool MenuItem::GetLoadFinish()
{
	if (!m_Icon.IsLoadCompelte())
	{
		return false;
	}

	return true;
}

void MenuItem::SetName(const char* value)
{
	Assert(value != NULL);

	m_strName = value;
	m_nHash = GetHashValueCase(value);
}

const char* MenuItem::GetName() const
{
	return m_strName.c_str();
}

unsigned int MenuItem::GetHash() const
{
	return m_nHash;
}

void MenuItem::SetText(const wchar_t* value)
{
	Assert(value != NULL);

	m_wsText = value;
}

const wchar_t* MenuItem::GetText() const
{
	return m_wsText.c_str();
}

void MenuItem::SetDisable(bool value)
{
	m_bDisable = value;
}

bool MenuItem::GetDisable() const
{
	return m_bDisable;
}

void MenuItem::SetChecked(bool value)
{
	m_bChecked = value;
}

bool MenuItem::GetChecked() const
{
	return m_bChecked;
}

void MenuItem::SetIcon(const char* pDraw)
{
	Assert(pDraw != NULL);

	if (m_pPainter == NULL)
	{
		return;
	}

	if (m_pGui == NULL)
	{
		return;
	}

	m_Icon.SetName(pDraw);
	m_Icon.Create(m_pPainter, m_pGui, false);
}

const char* MenuItem::GetIcon()
{
	return m_Icon.GetName();
}

void MenuItem::SetMenuID(const PERSISTID& id)
{
	m_MenuID = id;
}

PERSISTID MenuItem::GetMenuID() const
{
	return m_MenuID;
}

void MenuItem::SetSubMenuID(const PERSISTID&id)
{
	m_SubMenuID = id;
}

PERSISTID MenuItem::GetSubMenuID() const
{
	return m_SubMenuID;
}

bool MenuItem::IsHasSubMenu()
{
    return !m_SubMenuID.IsNull();
}

void MenuItem::SetPainter(IPainter* pPainter)
{
	m_pPainter = pPainter;
}

void MenuItem::SetGui(Gui* pGui)
{
	m_pGui = pGui;
}

CImagePlayer* MenuItem::GetImagePlayer()
{
	return &m_Icon;
}
