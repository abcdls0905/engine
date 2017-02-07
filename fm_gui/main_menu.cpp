//--------------------------------------------------------------------
// 文件名:		main_menu.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "main_menu.h"
#include "menu.h"
#include "../public/core_log.h"

/// \file main_menu.cpp
/// \brief 主菜单

/// entity: MainMenu
/// \brief 主菜单实体
/// \see 继承自IControl
DECLARE_ENTITY(MainMenu, 0, IControl);

/// property: int ItemWidth
/// \brief 菜单项的高度
DECLARE_PROPERTY(int, MainMenu, ItemWidth, GetItemWidth, SetItemWidth);

// MainMenu

MainMenu::MainMenu()
{
	m_nItemWidth = DEF_ITEM_WIDTH;
	m_bPush = false;
	
	SetDesignSize(80, 24);
	SetCanFocus(true);
}

MainMenu::~MainMenu()
{
}

bool MainMenu::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	return true;
}

bool MainMenu::Shut()
{
	for (size_t i = 0; i < m_Menus.size(); ++i)
	{
		m_Menus[i]->Release();
	}

	m_Menus.clear();
	
	return IControl::Shut();
}

void MainMenu::SetItemWidth(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nItemWidth = value;
}

int MainMenu::GetItemWidth() const
{
	return m_nItemWidth;
}

void MainMenu::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	di->RemovePropInfo("Text");
	CVar v(VTYPE_INT, DEF_ITEM_WIDTH);
	di->AddPropInfo("ItemWidth", IDesignInfo::TYPE_INT, true, true,
		&v);
}

bool MainMenu::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	for (size_t i = 0; i < m_Menus.size(); ++i)
	{
		if (!m_Menus[i]->GetLoadFinish())
		{
			return false;
		}
	}
	
	return IControl::GetLoadFinish();
}

void MainMenu::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
    RefreshInnerAbsPos();

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	bool draw_image = false;
	
	if (!draw_image)
	{
		if (HasBackImage())
		{
			draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
		}
	}
	
	if (!draw_image)
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}

	const wchar_t* text = GetShowText();
	
	if (!m_Menus.empty())
	{
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
		
		int x3 = x1 + PADDING_WIDTH;
		int y3 = y1 + PADDING_WIDTH;
		int y4 = y2 - PADDING_WIDTH;
		
		for (size_t k = 0; k < m_Menus.size(); ++k)
		{
			Menu* pMenu = m_Menus[k];
			unsigned int color = GetForeColor();
			
			if (GetCapture() && (pMenu->GetID() == m_SelectMenu))
			{
				GetPainter()->DrawRect(x3, y3, x3 + m_nItemWidth, y4, 
					GetLineColor());
			}
			
			const wchar_t* text = pMenu->GetText();
			int h = GetPainter()->GetTextHeight(text);
			int w = GetPainter()->GetTextWidth(text);
			// 垂直居中
			int offset_y = (GetHeight() - h) / 2;
			int offset_x = PADDING_WIDTH;
			
			GetPainter()->WriteText(
				x3 + offset_x, y3 + offset_y, color, text);
			x3 += m_nItemWidth;
		}
	}
}

Menu* MainMenu::GetMenuControl(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Menu"))
	{
		return NULL;
	}

	return (Menu*)pEntity;
}

Menu* MainMenu::GetInMenu(int x, int y) const
{
	int x1 = InnerGetAbsLeft() + PADDING_WIDTH;
	// 更快捷的操作
	int y1 = InnerGetAbsTop();
	int y2 = y1 + GetHeight();
	
	for (size_t k = 0; k < m_Menus.size(); ++k)
	{
		if (GuiUtil_PointInRect(x, y, x1, y1, x1 + m_nItemWidth, y2))
		{
			return m_Menus[k];
		}
		
		x1 += m_nItemWidth;
	}
	
	return NULL;
}

bool MainMenu::FindMenuIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Menus.size(); ++i)
	{
		if ((m_Menus[i]->GetHash() == hash)
			&& (strcmp(m_Menus[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

PERSISTID MainMenu::CreateMenu(const char* name, const wchar_t* text)
{
	Assert(name != NULL);
	Assert(text != NULL);
	
	size_t index;

	if (FindMenuIndex(name, index))
	{
		CORE_TRACE("(Menu::CreateMenu)item name exists");
		CORE_TRACE(name);
		return PERSISTID();
	}
	
	Menu* pMenu = (Menu*)CreateSuborControl(this, "Menu");
	
	pMenu->SetName(name);
	pMenu->SetText(text);
	pMenu->SetVisible(false);
	
	m_Menus.push_back(pMenu);

	return pMenu->GetID();
}

PERSISTID MainMenu::FindMenu(const char* name)
{
	size_t index;

	if (!FindMenuIndex(name, index))
	{
		return PERSISTID();
	}
	
	return m_Menus[index]->GetID();
}

int MainMenu::GetMenuCount() const
{
	return (int)m_Menus.size();
}

PERSISTID MainMenu::GetMenu(size_t index)
{
	if (index >= m_Menus.size())
	{
		return PERSISTID();
	}

	return m_Menus[index]->GetID();
}

bool MainMenu::ResetMenu()
{
	HideMenu(GetMenuControl(m_SelectMenu));

	m_SelectMenu = PERSISTID();
	m_bPush = false;
	
	return true;
}

bool MainMenu::ShowMenu(Menu* pMenu)
{
	for (size_t i = 0; i < m_Menus.size(); ++i)
	{
		if (m_Menus[i] == pMenu)
		{
			pMenu->SetLeft((int)i * m_nItemWidth + PADDING_WIDTH);
			pMenu->SetTop(GetHeight());
			pMenu->SetVisible(true);
			return true;
		}
	}
	
	return false;
}

bool MainMenu::HideMenu(Menu* pMenu)
{
	if (NULL == pMenu)
	{
		return false;
	}

	pMenu->SetVisible(false);

	return true;
}

int MainMenu::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (m_bPush)
	{
		HideMenu(GetMenuControl(m_SelectMenu));

		m_bPush = false;
		
		return 1;
	}

	Menu* pMenu = GetInMenu(x, y);

	if (NULL == pMenu)
	{
		return 0;
	}

	ShowMenu(GetMenuControl(m_SelectMenu));

	m_bPush = true;
	
	return 1; 
}

int MainMenu::OnMouseMove(int x, int y)
{
	Menu* pMenu = GetInMenu(x, y);
	
	if (NULL == pMenu)
	{
		return 0;
	}
	
	if (pMenu->GetID() == m_SelectMenu)
	{
		return 0;
	}
	
	if (m_bPush)
	{
		HideMenu(GetMenuControl(m_SelectMenu));
		
		ShowMenu(pMenu);
	}

	m_SelectMenu = pMenu->GetID();
	
	return 1;
}

