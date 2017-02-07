//--------------------------------------------------------------------
// 文件名:		list_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "list_box.h"
#include "gui_input.h"

/// \file list_box.cpp
/// \brief 列表框

/// entity: ListBox
/// \brief 列表框实体
DECLARE_ENTITY(ListBox, 0, IScrollable);

/// readonly: int ItemCount
/// \brief 表项的数量
DECLARE_PROPERTY_GET(int, ListBox, ItemCount, GetItemCount);

/// property: bool Sorted
/// \brief 是否排序
DECLARE_PROPERTY(bool, ListBox, Sorted, GetSorted, SetSorted);
/// property: bool MouseSelect
/// \brief 是否鼠标移动选择
DECLARE_PROPERTY(bool, ListBox, MouseSelect, GetMouseSelect, SetMouseSelect);
/// property: int ItemHeight
/// \brief 表项的高度
DECLARE_PROPERTY(int, ListBox, ItemHeight, GetItemHeight, SetItemHeight);
/// property: int TextOffsetX
/// \brief 文字起始位移
DECLARE_PROPERTY(int, ListBox, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, ListBox, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, ListBox, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string SelectBackImage
/// \brief 被选中时的背景图元
DECLARE_PROPERTY(const char*, ListBox, SelectBackImage, GetSelectBackImage, 
	SetSelectBackImage);
/// property: int SelectIndex
/// \brief 被选中项的索引值，未选中为-1
DECLARE_PROPERTY(int, ListBox, SelectIndex, GetSelectIndex, SetSelectIndex);
/// property: wstring SelectString
/// \brief 当前被选中的文本
DECLARE_PROPERTY(const wchar_t*, ListBox, SelectString, GetSelectString, 
	SetSelectString);

/// event: int on_select_changed(object self, int old_select_index)
/// \brief 当选项发生改变时的回调
/// \param self 本控件
/// \param old_select_index 以前被选中索引
DECLARE_EVENT(on_select_changed);
/// event: int on_select_click(object self, int select_index)
/// \brief 点击表项时的回调
/// \param self 本控件
/// \param select_index 当前被选中索引
DECLARE_EVENT(on_select_click);
/// event: int on_select_double_click(object self, int select_index)
/// \brief 双击表项时的回调
/// \param self 本控件
/// \param select_index 当前被选中索引
DECLARE_EVENT(on_select_double_click);
/// event: int on_amount_changed(object self)
/// \brief 当表项数量发生改变时的回调
/// \param self 本控件
/// \param amount 当前表项数量
DECLARE_EVENT(on_amount_changed);

// ListBox

ListBox::ListBox()
{
	m_bSorted = false;
	m_bMouseSelect = false;
	m_nItemHeight = 16;
	m_nTextOffsetX = 2;
	m_nScrollWidth = 0;
	m_nScrollLeft = 0;
	m_nTopIndex = 0;
	m_nSelectIndex = -1;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	SetCanFocus(true);
	SetTabStop(true);
}

ListBox::~ListBox()
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		CORE_DELETE(m_Items[i]);
	}
}

bool ListBox::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}

	// 创建垂直滚动条
	SetHasVScroll(true);
	
	return true;
}

bool ListBox::Shut()
{
	return IScrollable::Shut();
}

void ListBox::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);
	
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
		&v);
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AlwaysHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HasHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);
	
	di->AddPropInfo("Sorted", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("MouseSelect", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ItemHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(
                                            CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true,
		&v);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(
                                              CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true,
		&v );
	di->AddPropInfo("SelectBackImage", IDesignInfo::TYPE_IMAGE, true, true);
	
	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");

	di->AddEventInfo("on_select_changed");
	di->AddEventInfo("on_select_click");
	di->AddEventInfo("on_select_double_click");
	di->AddEventInfo("on_amount_changed");
}

bool ListBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_SelectBackImage.IsLoadCompelte())
	{
		return false;
	}
	
	return IScrollable::GetLoadFinish();
}

void ListBox::SetSorted(bool value)
{
	m_bSorted = value;
}

bool ListBox::GetSorted() const
{
	return m_bSorted;
}

void ListBox::SetMouseSelect(bool val)
{
	m_bMouseSelect = val;
}

bool ListBox::GetMouseSelect() const
{
	return m_bMouseSelect;
}

void ListBox::SetItemHeight(int value)
{
	if (value < MIN_ITEM_HEIGHT)
	{
		value = MIN_ITEM_HEIGHT;
	}

	m_nItemHeight = value;
}

int ListBox::GetItemHeight() const 
{ 
	return m_nItemHeight; 
}

void ListBox::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nTextOffsetX = value;
}

int ListBox::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

int ListBox::GetItemCount() const 
{ 
	return (int)m_Items.size(); 
}

bool ListBox::ShowSelectItem()
{
	if (m_nSelectIndex == -1)
	{
		return false;
	}
	
	if (m_nSelectIndex == m_nTopIndex)
	{
		return true;
	}
	
	if (m_nSelectIndex < m_nTopIndex)
	{
		SetVerticalValue(m_nSelectIndex);
		return true;
	}

	int page_disp = GetCanDispCount();

	if (m_nSelectIndex < (m_nTopIndex + page_disp))
	{
		return true;
	}

	int new_index = m_nSelectIndex - (page_disp - 1);

	if (new_index < 0)
	{
		new_index = 0;
	}
	
	SetVerticalValue(new_index);
	
	return true;
}

void ListBox::SetSelectIndex(int value) 
{ 
	int old_value = m_nSelectIndex;
	
	m_nSelectIndex = value; 

	ShowSelectItem();

	UpdateScroll();

	if (old_value != value)
	{
		if (GetEnabled())
		{
			GuiUtil_RunCallback(this, "on_select_changed", 
				CVarList() << old_value);
		}
	}
}

int ListBox::GetSelectIndex() const 
{ 
	return m_nSelectIndex; 
}

void ListBox::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string ListBox::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void ListBox::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string ListBox::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void ListBox::SetSelectBackImage(const char* value) 
{ 
	m_SelectBackImage.SetName(value);
	m_SelectBackImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ListBox::GetSelectBackImage() const 
{ 
	return m_SelectBackImage.GetName();
}

int ListBox::GetCanDispCount() const
{
	return GetContentHeight() / m_nItemHeight;
}

int ListBox::GetDispItemCount() const
{
	int num = (int)m_Items.size() - m_nTopIndex;
	int can_disp = GetCanDispCount();
	
	if (num > can_disp)
	{
		num = can_disp;
	}

	return num;
}

void ListBox::CalcScrollTotal()
{
	m_nScrollWidth = 0;
	
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		int width = m_Items[i]->nWidth;
		
		if (width > m_nScrollWidth)
		{
			m_nScrollWidth = width;
		}
	}
}

int ListBox::FindString(const wchar_t* item) const
{
	Assert(item != NULL);
	
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i]->wsValue == item)
		{
			return int(i);
		}
	}
	
	return -1;
}

int ListBox::AddString(const wchar_t* item)
{
	Assert(item != NULL);

	int pos;
	
	if (m_bSorted)
	{
		// 返回插入的位置
		pos = AddStringSort(item, 0, (int)m_Items.size());

		// 调整被选项的索引
		//if (pos < m_nSelectIndex)
		//{
		//	SetSelectIndex(m_nSelectIndex + 1);
		//}
	}
	else
	{
		pos = (int)m_Items.size();

		item_t* pItem = CORE_NEW(item_t);

		pItem->wsValue = item;
		
		m_Items.push_back(pItem);
	}

	if (GetHasHScroll())
	{
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
		m_Items[pos]->nWidth = GetPainter()->GetTextWidth(item) 
			+ m_nTextOffsetX * 2;
		CalcScrollTotal();
	}
	else
	{
		m_Items[pos]->nWidth = 0;
	}

	m_Items[pos]->nColor = 0;
	UpdateScroll();
	GuiUtil_RunCallback(this, "on_amount_changed", CVarList());

	return pos;
}

int ListBox::AddStringSort(const wchar_t* item, int beg, int end)
{
	Assert(size_t(beg) <= m_Items.size());
	Assert(size_t(end) <= m_Items.size());
	
	if (beg == end)
	{
		item_t* pItem = CORE_NEW(item_t);

		pItem->wsValue = item;
		
		m_Items.insert(beg, pItem);
		return beg;
	}

	// 折半查找
	int pos = beg + (end - beg) / 2;
	
	// 大小写不敏感
	if (wcsicmp(m_Items[pos]->wsValue.c_str(), item) >= 0)
	{
		return AddStringSort(item, beg, pos);
	}
	else
	{
		return AddStringSort(item, pos + 1, end);
	}
}

bool ListBox::RemoveString(const wchar_t* item)
{
	Assert(item != NULL);

	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i]->wsValue == item)
		{
			return RemoveByIndex((int)i);
		}
	}
	
	return false;
}

bool ListBox::RemoveByIndex(int index)
{
	if (size_t(index) >= m_Items.size())
	{
		return false;
	}

	CORE_DELETE(m_Items[index]);
	m_Items.remove(index);

	if (m_nSelectIndex == index)
	{
		SetSelectIndex(-1);
	}
	
	if (GetHasHScroll())
	{
		CalcScrollTotal();
	}

	UpdateScroll();
	GuiUtil_RunCallback(this, "on_amount_changed", CVarList());

	return true;
}

bool ListBox::ClearString()
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		CORE_DELETE(m_Items[i]);
	}

	m_Items.clear();

	if (GetHasHScroll())
	{
		CalcScrollTotal();
	}

	m_nScrollLeft = 0;
	UpdateScroll();
	GuiUtil_RunCallback(this, "on_amount_changed", CVarList());

	return true;
}

const wchar_t* ListBox::GetString(int index) const
{
	if (size_t(index) >= m_Items.size())
	{
		return L"";
	}

	return m_Items[index]->wsValue.c_str();
}

bool ListBox::ChangeString(int index, const wchar_t* value)
{
    if (size_t(index) >= m_Items.size())
    {
        return false;
    }

    m_Items[index]->wsValue = value;

    return true;
}

bool ListBox::SetItemColor(int index, const char* value)
{
	Assert(value != NULL);
	
	if (size_t(index) >= m_Items.size())
	{
		return false;
	}

	m_Items[index]->nColor = GuiUtil_ColorToInt(value);

	return true;
}

result_string ListBox::GetItemColor(int index) const
{
	if (size_t(index) >= m_Items.size())
	{
		return "";
	}

	return GuiUtil_IntToColor(m_Items[index]->nColor).c_str();
}

bool ListBox::SetTag(int index, const PERSISTID& id)
{
	if (size_t(index) >= m_Items.size())
	{
		return false;
	}
	
	m_Items[index]->TagID = id;
	
	return true;
}

PERSISTID ListBox::GetTag(int index) const
{
	if (size_t(index) >= m_Items.size())
	{
		return PERSISTID();
	}
	
	return m_Items[index]->TagID;
}

void ListBox::SetSelectString(const wchar_t* value)
{
	Assert(value != NULL);

	int index = FindString(value);

	SetSelectIndex(index);
}

const wchar_t* ListBox::GetSelectString() const
{
	return GetString(m_nSelectIndex);
}

int ListBox::GetInItem(int x, int y) const
{
	int num = GetDispItemCount() + 1;
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + m_nItemHeight;
	
	for (int i = 0; i < num; i++)
	{
		if (size_t(m_nTopIndex + i) >= m_Items.size())
		{
			break;
		}

		if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2))
		{
			return m_nTopIndex + i;
		}
		
		y1 += m_nItemHeight;
		y2 += m_nItemHeight;
	}

	return -1;
}

bool ListBox::ProcessSort()
{
	TArrayPod<item_t*, 1, TListBoxAlloc> temp;
	
	temp.swap(m_Items);
	//m_Items.clear();
	
	for (size_t i = 0; i < temp.size(); ++i)
	{
		AddStringSort(temp[i]->wsValue.c_str(), 0, (int)m_Items.size());
		CORE_DELETE(temp[i]);
	}

	UpdateScroll();
	
	return true;
}

// 重载的模板方法

int ListBox::GetVerticalTotal() const 
{ 
	return (int)m_Items.size();
}

int ListBox::GetVerticalCanDisp() const 
{ 
	return GetCanDispCount(); 
}

void ListBox::SetVerticalValue(int value) 
{
	m_nTopIndex = value;
}

int ListBox::GetVerticalValue() const 
{ 
	return m_nTopIndex; 
}

int ListBox::GetHorizonTotal() const 
{ 
	return m_nScrollWidth;
}

int ListBox::GetHorizonCanDisp() const 
{ 
	return GetContentWidth();
}

void ListBox::SetHorizonValue(int value) 
{
	m_nScrollLeft = value;
}

int ListBox::GetHorizonValue() const 
{ 
	return m_nScrollLeft;
}

void ListBox::PaintScrollRect(float seconds)
{
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + GetContentHeight();
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	int top = 0;
	int left = 0 - m_nScrollLeft;
	int num = GetDispItemCount() + 1;
	
	for (int i = m_nTopIndex; i < (num + m_nTopIndex); i++)
	{
		if (size_t(i) >= m_Items.size())
		{
			break;
		}

		Assert(i >= 0);
		
		if (i == m_nSelectIndex)
		{
			if (m_SelectBackImage.IsReady())
			{
				m_SelectBackImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
					x1 + left, y1 + top, x2, y1 + top + m_nItemHeight);
			}
			else
			{
				GetPainter()->FillRect(x1 + left, y1 + top, x2, 
					y1 + top + m_nItemHeight, m_SelectBackColor.GetValue());
			}
			
			GetPainter()->WriteText(x1 + left + m_nTextOffsetX, y1 + top, 
				m_SelectForeColor.GetValue(), m_Items[i]->wsValue.c_str());
		}
		else
		{
			unsigned int fore_color = GetForeColor();

			if (m_Items[i]->nColor != 0)
			{
				fore_color = m_Items[i]->nColor;
			}
			
			GetPainter()->WriteText(x1 + left + m_nTextOffsetX, y1 + top, 
				fore_color, m_Items[i]->wsValue.c_str());
		}
		
		top += m_nItemHeight;
	}
}

// 事件处理

int ListBox::ClickItem(int select_index)
{
	if (select_index < 0)
	{
		return 0;
	}
	
	if (GetDelegateControl() != NULL)
	{
		GetDelegateControl()->OnSuborEvent(this, 
			CGuiConst::EVENT_SELECT_CLICK, CVarList() << select_index);
	}

	return GuiUtil_RunCallback(this, "on_select_click", 
		CVarList() << select_index);
}

int ListBox::DoubleClickItem(int select_index)
{
	if (select_index < 0)
	{
		return 0;
	}
	
	return GuiUtil_RunCallback(this, "on_select_double_click", 
		CVarList() << select_index);
}

int ListBox::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	int res = GetInItem(x, y);

	if (res >= 0)
	{
		if (m_nSelectIndex != res)
		{
			SetSelectIndex(res);
		}
	}
	
	return ClickItem(res);
}

int ListBox::OnLButtonDoubleClick(int x, int y) 
{ 
	int res = GetInItem(x, y);

	return DoubleClickItem(res);
}

int ListBox::OnMouseMove(int x, int y)
{
	if (!m_bMouseSelect)
	{
		return 0;
	}
	
	int res = GetInItem(x, y);
	
	if (res >= 0)
	{
		if (m_nSelectIndex != res)
		{
			SetSelectIndex(res);
		}
	}
	
	return 1;
}

int ListBox::OnKeyDown(unsigned int vk)
{
	switch (vk)
	{
	case CGuiInput::KEY_UP:
		{
			if (m_nSelectIndex > 0)
			{
				SetSelectIndex(m_nSelectIndex - 1);
				return 1;
			}
		}
		break;
	case CGuiInput::KEY_DOWN:
		{
			if (m_nSelectIndex < int(m_Items.size() - 1))
			{
				SetSelectIndex(m_nSelectIndex + 1);
				return 1;
			}
		}
		break;
	case CGuiInput::KEY_PRIOR:
		{
			int page_disp = GetCanDispCount() - 1;

			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int new_index = m_nSelectIndex;

			if (new_index > page_disp)
			{
				new_index -= page_disp;
			}
			else
			{
				new_index = 0;
			}

			SetSelectIndex(new_index);

			return 1;
		}
		break;
	case CGuiInput::KEY_NEXT:
		{
			int page_disp = GetCanDispCount() - 1;

			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			int new_index = m_nSelectIndex + page_disp;
			
			if (size_t(new_index) >= m_Items.size())
			{
				new_index = (int)m_Items.size() - 1;
			}
			
			SetSelectIndex(new_index);
			return 1;
		}
		break;
	case CGuiInput::KEY_HOME:
		{
			SetSelectIndex(0);
			return 1;
		}
		break;
	case CGuiInput::KEY_END:
		{
			SetSelectIndex((int)m_Items.size() - 1);
			return 1;
		}
		break;
	case CGuiInput::KEY_RETURN:
		{
			// 等效点击
			ClickItem(m_nSelectIndex);
			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

