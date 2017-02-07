//--------------------------------------------------------------------
// 文件名:		menu.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "menu.h"
#include "menu_item.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

/// \file menu.cpp
/// \brief 菜单

/// entity: Menu
/// \brief 菜单实体
DECLARE_ENTITY(Menu, 0, IControl);

/// property: int ItemHeight
/// \brief 菜单项高度
DECLARE_PROPERTY(int, Menu, ItemHeight, GetItemHeight, SetItemHeight);

/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, Menu, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, Menu, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string SelectBorderColor
/// \brief 选择框的边框色
DECLARE_PROPERTY(result_string, Menu, SelectBorderColor, 
	GetSelectBorderColorString, SetSelectBorderColorString);

/// property: string SelectBarDraw
/// \brief 被选中文本的背景图
DECLARE_PROPERTY(const char*, Menu, SelectBarDraw, GetSelectBarDraw, 
	SetSelectBarDraw);
/// property: string SplitColor
/// \brief 分隔线色
DECLARE_PROPERTY(result_string, Menu, SplitColor, GetSplitColorString, 
	SetSplitColorString);
/// property: string SplitDraw
/// \brief 分隔图元
DECLARE_PROPERTY(const char*, Menu, SplitDraw, GetSplitDraw, SetSplitDraw);
/// property: string SubFlagDraw
/// \brief 子界面标识图元
DECLARE_PROPERTY(const char*, Menu, SubFlagDraw, GetSubFlagDraw, 
	SetSubFlagDraw);
/// property: string CheckedDraw
/// \brief 菜单项Checked图元
DECLARE_PROPERTY(const char*, Menu, CheckedDraw, GetCheckedDraw, 
	SetCheckedDraw);
/// property: bool IsCenter
/// \brief 居中对齐
DECLARE_PROPERTY(bool, Menu, IsCenter, GetCenter, SetCenter);

/// property: int LeftBarWidth
/// \brief 左侧图标条的宽度
DECLARE_PROPERTY(int, Menu, LeftBarWidth, GetLeftBarWidth, SetLeftBarWidth);
/// property: bool LeftBar
/// \brief 左侧图标条
DECLARE_PROPERTY(bool, Menu, LeftBar, GetLeftBar, SetLeftBar);

DECLARE_PROPERTY(result_string, Menu, LeftBarBackColor, 
	GetLeftBarBackColorString, SetLeftBarBackColorString);
/// property: string SelectBarDraw
/// \brief 被选中文本的背景图
DECLARE_PROPERTY(const char*, Menu, LeftBarDraw, GetLeftBarDraw, 
	SetLeftBarDraw);

/// event: int on_click_<item_name>(object self, object item)
/// \brief 选择菜单时的回调
/// \param self 本控件
/// \param item 选中的菜单项
DECLARE_EVENT(on_click_<item_name>)
/// event: int on_show_submenu(object self, object item)
/// \brief 显示子菜单的回调
/// \param self 本控件
/// \param item 移入的拥有子的菜单项
DECLARE_EVENT(on_show_submenu)
/// event: int on_drag_enter(object self)
/// \brief 开始拖动的回调
/// \param self 本控件
DECLARE_EVENT(on_drag_enter);
/// event: int on_drag_move(object self, int drag_x, int drag_y)
/// \brief 鼠标按住拖动的回调
/// \param self 本控件
/// \param drag_x X方向的拖动距离
/// \param drag_y Y方向的拖动距离
DECLARE_EVENT(on_drag_move);

// Menu

Menu::Menu()
{
	m_nItemHeight = 20;
    m_nDragOldX = 0;
    m_nDragOldY = 0;
    m_IsCenter = false;
	m_bLeftBar = true;
	m_nLeftBarWidth = 20 + PADDING_WIDTH * 2;

	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(0xFFB0B0F0);
	m_SelectBorderColor.SetValue(0xFF0000F0);
	m_LeftBarBackColor.SetValue(0xFFB0B0B0);
	m_SplitColor.SetValue(0xFFC0C0C0);
	
	SetDesignSize(80, 20);
}

Menu::~Menu()
{
}

bool Menu::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	return true;
}

bool Menu::Shut()
{
	DeleteAllItem();
	
	return IControl::Shut();
}

void Menu::SetItemHeight(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nItemHeight = value;
}

int Menu::GetItemHeight() const
{
	return m_nItemHeight;
}

void Menu::SetLeftBarWidth(int value)
{
	if (value < 0)
	{
		return;
	}

	m_nLeftBarWidth = value;
}

int Menu::GetLeftBarWidth() const
{
	return m_nLeftBarWidth;
}

void Menu::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string Menu::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void Menu::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string Menu::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void Menu::SetSelectBorderColorString(const char* value)
{
	Assert(value != NULL);

	m_SelectBorderColor.SetName(GetGui(), value);
}

result_string Menu::GetSelectBorderColorString() const
{
	return m_SelectBorderColor.GetName().c_str();
}

void Menu::SetSelectBarDraw(const char* pDraw)
{
    Assert(pDraw != NULL);

    m_SelectBarDraw.SetName(pDraw);
    m_SelectBarDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Menu::GetSelectBarDraw()
{
    return m_SelectBarDraw.GetName();
}

void Menu::SetLeftBarBackColorString(const char* value)
{
	Assert(value != NULL);

	m_LeftBarBackColor.SetName(GetGui(), value);
}

result_string Menu::GetLeftBarBackColorString() const
{
	return m_LeftBarBackColor.GetName().c_str();
}

void Menu::SetLeftBarDraw(const char* pDraw)
{
	Assert(pDraw != NULL);

	m_LeftBarDraw.SetName(pDraw);
	m_LeftBarDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Menu::GetLeftBarDraw()
{
	return m_LeftBarDraw.GetName();
}

void Menu::SetSplitColorString(const char* value)
{
	Assert(value != NULL);

	m_SplitColor.SetName(GetGui(), value);
}

result_string Menu::GetSplitColorString() const
{
	return m_SplitColor.GetName().c_str();
}

void Menu::SetSplitDraw(const char* pDraw)
{
    Assert(pDraw != NULL);

    m_SplitDraw.SetName(pDraw);
    m_SplitDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Menu::GetSplitDraw()
{
    return m_SplitDraw.GetName();
}

void Menu::SetSubFlagDraw(const char* pDraw)
{
    Assert(pDraw != NULL);

    m_SubFlagDraw.SetName(pDraw);
    m_SubFlagDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Menu::GetSubFlagDraw()
{
    return m_SubFlagDraw.GetName();
}

void Menu::SetCheckedDraw(const char* pDraw)
{
	Assert(pDraw != NULL);

	m_CheckedDraw.SetName(pDraw);
	m_CheckedDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Menu::GetCheckedDraw()
{
	return m_CheckedDraw.GetName();
}

void Menu::SetCenter(bool IsCenter)
{
    m_IsCenter = IsCenter;
}

bool Menu::GetCenter()
{
    return m_IsCenter;
}

void Menu::SetLeftBar(bool bLeftBar)
{
	m_bLeftBar = bLeftBar;
}

bool Menu::GetLeftBar()
{
	return m_bLeftBar;
}

void Menu::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
    CVar v(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("ItemHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true, &v );
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBorderColor", IDesignInfo::TYPE_COLOR, true, true, &v );

    di->AddPropInfo("SelectBarDraw", IDesignInfo::TYPE_IMAGE, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(0xFFC0C0C0).c_str());
	di->AddPropInfo("SplitColor", IDesignInfo::TYPE_COLOR, true, true, &v );
    di->AddPropInfo("SplitDraw", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("SubFlagDraw", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("CheckedDraw", IDesignInfo::TYPE_IMAGE, true, true);

	di->AddPropInfo("IsCenter", IDesignInfo::TYPE_BOOL, true, false);

	di->AddPropInfo("LeftBar", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("LeftBarWidth", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(0xFFB0B0B0).c_str());
	di->AddPropInfo("LeftBarBackColor", IDesignInfo::TYPE_COLOR, true, true, &v );
	di->AddPropInfo("LeftBarDraw", IDesignInfo::TYPE_IMAGE, true, true);

    di->AddEventInfo("on_show_submenu");
    di->AddEventInfo("on_drag_enter");
    di->AddEventInfo("on_drag_move");
}

bool Menu::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_SelectBarDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_LeftBarDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_SplitDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_SubFlagDraw.IsLoadCompelte())
	{
		return false;
	}

	if (!m_CheckedDraw.IsLoadCompelte())
	{
		return false;
	}

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (!m_Items[i]->GetLoadFinish())
		{
			return false;
		}
	}
	
	return IControl::GetLoadFinish();
}

void Menu::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}

    RefreshInnerAbsPos();

	// 设置字体
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	// 重新计算菜单的尺寸
	int menu_height = CalcTotalHeight();
	int menu_width = CalcTotalWidth();

	SetHeight(menu_height);
	SetWidth(menu_width);

	if ((GetAbsLeft() + GetWidth()) > GetGui()->GetWidth())
	{
		m_SelectItem = PERSISTID();

		SetAbsLeft(GetGui()->GetWidth() - menu_width);
	}

	if ((GetAbsTop() + GetHeight()) > GetGui()->GetHeight() 
		&& InnerGetAbsTop() > menu_height)
	{
		m_SelectItem = PERSISTID();

		SetAbsTop(GetAbsTop() - menu_height);
	}

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	bool draw_image = false;
	
	if (HasBackImage())
	{
		draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
	}
	
	if (!draw_image)
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}

	int x_offset = 0;//菜单项文本的绘制需要偏移的距离

	if (m_bLeftBar)
	{
		//有左侧图标条
		if (m_LeftBarDraw.IsReady())
		{
			m_LeftBarDraw.Draw(GetDrawMode(), x1 + 1, y1 + 1, 
				x1 + m_nLeftBarWidth - 1, y2 - 1);
		}
		else
		{
			GetPainter()->FillRect(x1 + 1, y1 + 1, x1 + m_nLeftBarWidth - 1, 
				y2 - 1, m_LeftBarBackColor.GetValue());
		}

		x_offset = m_nLeftBarWidth;
	}

	if (!m_Items.empty())
	{
		int x3 = x1 + PADDING_WIDTH;
		int x4 = x2 - PADDING_WIDTH;
		int y3 = y1 + PADDING_WIDTH;
		
		for (size_t k = 0; k < m_Items.size(); ++k)
		{
			MenuItem* pItem = m_Items[k];
			
			if (IsSeperator(pItem))
			{
				if (!m_SplitDraw.IsReady())
                {
                    int y4 = y3 + SEPERATOR_HEIGHT / 2;

                    // 无分隔图元
                    GetPainter()->DrawLine(x1 + x_offset, y4, x4, y4, 
						m_SplitColor.GetValue());
                    y3 += SEPERATOR_HEIGHT;
                }
                else
                {
                    // 有分隔图元
                    int Height = m_SplitDraw.GetHeight();
                    int Width = m_SplitDraw.GetWidth();
                    int y4 = y3 + (SEPERATOR_HEIGHT - Height) / 2;

                    m_SplitDraw.Draw(CGuiConst::DRAWMODE_EXPANDH, 
						x1 + x_offset, y4, x4, y4 + Height);
                    y3 += SEPERATOR_HEIGHT;
                }
			}
			else
			{
				unsigned int color = GetForeColor();

				//绘制选择条
				if (GetCapture() && (pItem->GetID() == m_SelectItem))
				{
					if (!m_SelectBarDraw.IsReady())
					{
						// 无选条图元
						GetPainter()->FillRect(x3, y3, x4, y3 + m_nItemHeight, 
						    m_SelectBackColor.GetValue());
						GetPainter()->DrawRect(x3, y3, x4, y3 + m_nItemHeight, 
							m_SelectBorderColor.GetValue());
					}
					else
					{
						// 有选择条图元
						int Height = m_SelectBarDraw.GetHeight();
						int Width = m_SelectBarDraw.GetWidth();

						m_SelectBarDraw.Draw(CGuiConst::DRAWMODE_EXPAND, 
							x3, y3 - 1, x4, y3 + m_nItemHeight + 1);
					}
				}

				if (m_bLeftBar)
				{
					// 有左侧图标条
					if (pItem->GetChecked())
					{
						// 绘制Checked标志
						if (!m_CheckedDraw.IsReady())
						{
							int BOX_SIZE = 12;
							int xi1 = x3 + (m_nLeftBarWidth - BOX_SIZE) / 2;
							int yi1 = y3 + (m_nItemHeight - BOX_SIZE) / 2;
							int xi2 = xi1 + BOX_SIZE;
							int yi2 = yi1 + BOX_SIZE;

							//GetPainter()->FillRect(x3, y3, xi, xi, GetBackColor());
							GetPainter()->DrawRect(xi1, yi1, xi2, yi2, 
								0xFF000000);
							GetPainter()->DrawLine(xi1 + 1, yi1 + 1, xi2 - 1, 
								yi2 - 1, 0xFF000000);
							GetPainter()->DrawLine(xi1 + 1, yi2 - 1, xi2 - 1, 
								yi1 + 1, 0xFF000000);
						}
						else
						{
							int Height = m_CheckedDraw.GetHeight();
							int Width = m_CheckedDraw.GetWidth();
							int xi = x3 + Width;
							int yi = y3 + Height;

							m_CheckedDraw.Draw(CGuiConst::DRAWMODE_CENTER,
								x3 + 1, y3 + 1, xi - 1, yi - 1);
						}
					}
					else
					{
						// 绘制图标
						CImagePlayer * pImage = pItem->GetImagePlayer();
						
						if ((pImage != NULL) && pImage->IsReady())
						{
							int Height = pImage->GetHeight();
							int Width = pImage->GetWidth();
							int xi = x1 + Width;
							int yi = y3 + Height;

							pImage->Draw(CGuiConst::DRAWMODE_CENTER,
								x3 + 1, y3 + 1, xi - 1, yi - 1);
						}
					}
				}

				if (pItem->IsHasSubMenu())
                {
                    // 有子菜单
                    if (!m_SubFlagDraw.IsReady())
					{
						GetPainter()->DrawTriangle(x4 - SUBFLAG_WIDTH, 
							y3 + (m_nItemHeight - 16) / 2,
							x4 - SUBFLAG_WIDTH, y3 + (m_nItemHeight + 16) / 2,
							x4, y3 + m_nItemHeight / 2, 0xFF000000);
					}
					else
                    {
                        int Height = m_SubFlagDraw.GetHeight();
                        int Width = m_SubFlagDraw.GetWidth();
                        int xi = x4 - SUBFLAG_WIDTH;
                        int yi = y3 + (m_nItemHeight - Height) / 2;

                        m_SubFlagDraw.Draw(CGuiConst::DRAWMODE_CENTER, 
							xi, yi, xi + Width, yi + Height);
                    }
                }

				// 开始绘制文字
				if (pItem->GetDisable())
				{
					color = CGuiConst::DISABLE_COLOR;
				}

				const wchar_t* text = pItem->GetText();

				if (wcslen(text) > 0)
				{
					int h = GetPainter()->GetTextHeight(text);
					int w = GetPainter()->GetTextWidth(text);
					// 垂直居中
					int yi = y3 + (m_nItemHeight - h) / 2;
					int xi = x3 + x_offset + 8;
					
					if (m_IsCenter)
					{
						// 文字居中
						int output_width = 
							menu_width - x_offset - 8 - PADDING_WIDTH * 2;

						if (output_width > w)
						{
							xi =  xi + (output_width - w) / 2;
						}
					}

					GetPainter()->WriteText(xi, yi, color, text);
				}

				y3 += m_nItemHeight;
			}
		}
	}
}

MenuItem* Menu::GetMenuItem(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("MenuItem"))
	{
		return NULL;
	}

	return (MenuItem*)pEntity;
}

bool Menu::IsSeperator(const MenuItem* pItem) const
{
	Assert(pItem != NULL);

	return (wcscmp(pItem->GetText(), L"-") == 0);
}

int Menu::CalcTotalHeight() const
{
	int h = PADDING_WIDTH * 2;

	for (size_t k = 0; k < m_Items.size(); ++k)
	{
		if (IsSeperator(m_Items[k]))
		{
			h += SEPERATOR_HEIGHT;
		}
		else
		{
			h += m_nItemHeight;
		}
	}
	
	return h;
}

int Menu::CalcTotalWidth() const
{
	int max_w = 0;//文字的宽度

	for (size_t k = 0; k < m_Items.size(); ++k)
	{
		const wchar_t* text = m_Items[k]->GetText();

		if (wcslen(text) > 0)
		{
			int text_w = GetPainter()->GetTextWidth(text) + 8;

			if (text_w > max_w)
			{
				max_w = text_w;
			}
		}
	}

	int w = PADDING_WIDTH * 2 + SUBFLAG_WIDTH + max_w;

	if (m_bLeftBar)
	{
		w += m_nLeftBarWidth;
	}

	return w;
}

MenuItem* Menu::GetInItem(int x, int y, int& itemRight, int& itemTop) const
{
	int left = InnerGetAbsLeft();
	int top = InnerGetAbsTop();
	int x1 = left + PADDING_WIDTH;
	int y1 = top + PADDING_WIDTH;
	int x2 = left + GetWidth() - PADDING_WIDTH;
	
	for (size_t k = 0; k < m_Items.size(); ++k)
	{
		if (IsSeperator(m_Items[k]))
		{
            y1 += SEPERATOR_HEIGHT;
		}
		else
		{
			if (GuiUtil_PointInRect(x, y, x1, y1, x2, y1 + m_nItemHeight))
			{
                itemRight = left + GetWidth();
                itemTop = y1;
				return m_Items[k];
			}
			
			y1 += m_nItemHeight;
		}
	}
	
	return NULL;
}

bool Menu::FindItemIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if ((m_Items[i]->GetHash() == hash)
			&& (strcmp(m_Items[i]->GetName(), name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

PERSISTID Menu::NewItem(const char* name, const wchar_t* text)
{
	Assert(name != NULL);
	Assert(text != NULL);

	MenuItem* pItem = (MenuItem*)GetCore()->CreateEntity("MenuItem");

	pItem->SetName(name);
	pItem->SetText(text);
	pItem->SetGui(GetGui());
	pItem->SetPainter(GetPainter());

	return pItem->GetID();
}

PERSISTID Menu::CreateItem(const char* name, const wchar_t* text)
{
	Assert(name != NULL);
	Assert(text != NULL);
	
	size_t index;

	if (FindItemIndex(name, index))
	{
		CORE_TRACE("(Menu::CreateItem)item name exists");
		CORE_TRACE(name);
		return PERSISTID();
	}
	
	MenuItem* pItem = (MenuItem*)GetCore()->CreateEntity("MenuItem");
	
	pItem->SetName(name);
	pItem->SetText(text);
	pItem->SetMenuID(GetID());
	pItem->SetGui(GetGui());
	pItem->SetPainter(GetPainter());
	
	m_Items.push_back(pItem);

	return pItem->GetID();
}

bool Menu::AddItem(const PERSISTID& id)
{
	MenuItem* pItem = GetMenuItem(id);

	if (NULL == pItem)
	{
		return false;
	}

	size_t index;

	if (FindItemIndex(pItem->GetName(), index))
	{
		CORE_TRACE("(Menu::AddItem)item name exists");
		CORE_TRACE(pItem->GetName());
		return false;
	}

	pItem->SetMenuID(GetID());

	m_Items.push_back(pItem);

	return true;
}

bool Menu::InsertItem(const PERSISTID& id, const char* before)
{
	size_t before_index;
	
	if (!FindItemIndex(before, before_index))
	{
		// 添加到最后
		return AddItem(id);
	}
	
	MenuItem* pItem = GetMenuItem(id);
	
	if (NULL == pItem)
	{
		return false;
	}
	
	size_t index;

	if (FindItemIndex(pItem->GetName(), index))
	{
		CORE_TRACE("(Menu::InsertItem)item name exists");
		CORE_TRACE(pItem->GetName());
		return false;
	}

	pItem->SetMenuID(GetID());
	
	m_Items.insert(before_index, pItem);

	return true;
}

bool Menu::RemoveItem(const char* name)
{
	size_t index;

	if (!FindItemIndex(name, index))
	{
		return false;
	}

	m_Items[index]->SetMenuID(PERSISTID());
	m_Items.remove(index);

	return true;
}

bool Menu::DeleteItem(const char* name)
{
	size_t index;
	
	if (!FindItemIndex(name, index))
	{
		return false;
	}

	m_Items[index]->Release();
	m_Items.remove(index);
	
	return true;
}

bool Menu::ClearItem()
{
	for (size_t k = 0; k < m_Items.size(); ++k)
	{
		m_Items[k]->SetMenuID(PERSISTID());
	}
	
	m_Items.clear();

	return true;
}

bool Menu::DeleteAllItem()
{
	for (size_t k = 0; k < m_Items.size(); ++k)
	{
		m_Items[k]->Release();
	}

	m_Items.clear();

	return true;
}

PERSISTID Menu::FindItem(const char* name)
{
	size_t index;

	if (!FindItemIndex(name, index))
	{
		return PERSISTID();
	}
	
	return m_Items[index]->GetID();
}

int Menu::GetItemCount() const
{
	return (int)m_Items.size();
}

PERSISTID Menu::GetItem(size_t index)
{
	if (index >= m_Items.size())
	{
		return PERSISTID();
	}

	return m_Items[index]->GetID();
}

void Menu::SetVisible(bool val)
{
    IControl::SetVisible(val);

    if (!val)
    {
        // 关闭之前的子菜单
        if (!m_ShowSubMenu.IsNull())
        {
            Menu* pLastSubMenu = (Menu*)GetCore()->GetEntity(m_ShowSubMenu);

            if (pLastSubMenu)
            {
                pLastSubMenu->SetVisible(false);
            }

            m_ShowSubMenu.nIdent = -1;
            m_ShowSubMenu.nSerial = -1;
        }

		IControl * pControl = GetRootControl();

		if (pControl == NULL)
		{
			pControl = GetGui()->GetDesktopForm();
		}

		if (pControl != NULL)
		{
			pControl->RemoveTopLayer(GetID());
		}
    }
	else
	{
		IControl * pControl = GetRootControl();

		if (pControl == NULL)
		{
			pControl = GetGui()->GetDesktopForm();
		}

		if (pControl != NULL)
		{
			pControl->AddTopLayer(GetID());
		}
	}
}

int Menu::OnLButtonUp(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
    int itemRight = 0, itemTop = 0;

	MenuItem* pItem = GetInItem(x, y, itemRight, itemTop);

	if (NULL == pItem)
	{
		return 0;
	}

	if (pItem->GetDisable())
	{
		return 0;
	}
	
	result_string event = result_string("on_") + pItem->GetName() + "_click";

	return GuiUtil_RunCallback(this, event.c_str(), 
		CVarList() << pItem->GetID());
}

int Menu::OnMouseMove(int x, int y)
{
	if (!GetVisible())
	{
		return 0;
	}

    int itemRight = 0;
	int itemTop = 0;
	MenuItem* pItem = GetInItem(x, y, itemRight, itemTop);

	if (NULL == pItem)
	{
		m_SelectItem = PERSISTID();
	}
	else
	{	
        // 关闭之前的子菜单
        if (!m_ShowSubMenu.IsNull())
        {
            Menu* pLastSubMenu = (Menu*)GetCore()->GetEntity(m_ShowSubMenu);

            if (pLastSubMenu)
            {
                pLastSubMenu->SetVisible(false);
            }

            m_ShowSubMenu.nIdent = -1;
            m_ShowSubMenu.nSerial = -1;
        }

		 if (pItem->GetDisable())
		 {
			m_SelectItem = PERSISTID();
		 }
		 else
		 {
			 //当前选择的菜单项
			 m_SelectItem = pItem->GetID();

			if (pItem->IsHasSubMenu())
			{
				//return GuiUtil_RunCallback(this, 
				//    "on_show_submenu", CVarList() << pItem->GetID());

				// 打开当前的子菜单
				Menu* pSubMenu = (Menu*)GetCore()->GetEntity(
					pItem->GetSubMenuID());

				if (pSubMenu)
				{
					m_ShowSubMenu = pSubMenu->GetID();

					pSubMenu->SetAbsLeft(itemRight);
					pSubMenu->SetAbsTop(itemTop);
					pSubMenu->SetVisible(true);
				}
			}
		}
	}
	
	return 1;
}

int Menu::OnLButtonDown(int x, int y)
{
    if (!GetVisible())
	{
        return 0;
	}

    if (m_SelectItem == PERSISTID())
    {
        return 0;
    }

    m_nDragOldX = x;
    m_nDragOldY = y;

    return GuiUtil_RunCallback(this, "on_drag_enter", 
        CVarList() << m_SelectItem);
}

// 拖动结束
int Menu::OnDragMove(int x, int y)
{
    if (!GetVisible())
	{
        return 0;
	}

    int drag_x = x - m_nDragOldX;
    int drag_y = y - m_nDragOldY;

    m_nDragOldX = x;
    m_nDragOldY = y;

    return GuiUtil_RunCallback(this, "on_drag_move", 
        CVarList() << m_SelectItem << drag_x << drag_y);
}
