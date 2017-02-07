//--------------------------------------------------------------------
// 文件名:		control_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "control_box.h"
#include "gui_input.h"

/// \file control_box.cpp
/// \brief 控件列表框

/// entity: ControlBox
/// \brief 控件列表实体
DECLARE_ENTITY(ControlBox, 0, IScrollable);

/// readonly: int ItemCount
/// \brief 表项的数量
DECLARE_PROPERTY_GET(int, ControlBox, ItemCount, GetItemCount);
/// readonly: int ScrollHeight
/// \brief 总的平滑滚动高度
DECLARE_PROPERTY_GET(int, ControlBox, ScrollHeight, GetScrollHeight);
/// readonly: int ScrollPage
/// \brief 平滑滚动每页的高度
DECLARE_PROPERTY_GET(int, ControlBox, ScrollPage, GetScrollPage);

/// property: bool ShowSelect
/// \brief 是否显示被选中项
DECLARE_PROPERTY(bool, ControlBox, ShowSelect, GetShowSelect, SetShowSelect);
/// property: int ItemHeight
/// \brief 表项的高度
DECLARE_PROPERTY(int, ControlBox, ItemHeight, GetItemHeight, SetItemHeight);
/// property: int OffsetX
/// \brief X方向起始位移
DECLARE_PROPERTY(int, ControlBox, OffsetX, GetOffsetX, SetOffsetX);
/// property: int OffsetY
/// \brief Y方向起始位移
DECLARE_PROPERTY(int, ControlBox, OffsetY, GetOffsetY, SetOffsetY);
/// property: int SelectIndex
/// \brief 被选中项的索引值，未选中为-1
DECLARE_PROPERTY(int, ControlBox, SelectIndex, GetSelectIndex, SetSelectIndex);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, ControlBox, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, ControlBox, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string SelectBackImage
/// \brief 被选中时的背景图元
DECLARE_PROPERTY(const char*, ControlBox, SelectBackImage, 
	GetSelectBackImage, SetSelectBackImage);
/// property: bool ScrollSmooth
/// \brief 是否平滑滚动
DECLARE_PROPERTY(bool, ControlBox, ScrollSmooth, GetScrollSmooth, 
	SetScrollSmooth);
/// property: int ScrollTop
/// \brief 平滑滚动的顶部位置
DECLARE_PROPERTY(int, ControlBox, ScrollTop, GetScrollTop, SetScrollTop);

/// event: int on_select_click(object self, int select_index, int x, int y)
/// \brief 点击表项时的回调
/// \param self 本控件
/// \param select_index 当前被选中索引
/// \param x X偏移
/// \param y Y偏移
DECLARE_EVENT(on_select_click);

/// event: int on_select_changed(object self, int old_select_index)
/// \brief 当选项发生改变时的回调
/// \param self 本控件
/// \param old_select_index 以前被选中索引
DECLARE_EVENT(on_select_changed);

// ControlBox

ControlBox::ControlBox()
{
	m_bShowSelect = false;
	m_bScrollSmooth = false;
	m_nScrollHeight = 0;
	m_nScrollTop = 0;
	m_nScrollWidth = 0;
	m_nScrollLeft = 0;
	m_nItemHeight = 16;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_nTopIndex = 0;
	m_nSelectIndex = -1;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);

	SetCanFocus(true);
	SetTabStop(true);
}

ControlBox::~ControlBox()
{
	ClearItems();
}

bool ControlBox::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}

	// 创建垂直滚动条
	SetHasVScroll(true);
	
	return true;
}

bool ControlBox::Shut()
{
	ClearItems();
	
	return IScrollable::Shut();
}

void ControlBox::PrepareDesignInfo(IDesignInfo* di)
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
	
	di->AddPropInfo("ShowSelect", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ItemHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("OffsetX", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("OffsetY", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(
                                            CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true,
		&v );
    
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(
                                              CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true,
		&v );
	di->AddPropInfo("SelectBackImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("ScrollSmooth", IDesignInfo::TYPE_BOOL, true, true);
	
	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");

	di->AddEventInfo("on_select_click");
	di->AddEventInfo("on_select_changed");
}

bool ControlBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_SelectBackImage.IsLoadCompelte())
	{
		return false;
	}

	return IControl::GetLoadFinish();
}

void ControlBox::SetShowSelect(bool value)
{
	m_bShowSelect = value;
}

bool ControlBox::GetShowSelect() const
{
	return m_bShowSelect;
}

void ControlBox::SetItemHeight(int value)
{
	if (value < MIN_ITEM_HEIGHT)
	{
		value = MIN_ITEM_HEIGHT;
	}

	m_nItemHeight = value;
}

int ControlBox::GetItemHeight() const 
{ 
	return m_nItemHeight; 
}

void ControlBox::SetOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nOffsetX = value;
}

int ControlBox::GetOffsetX() const
{
	return m_nOffsetX;
}

void ControlBox::SetOffsetY(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nOffsetY = value;
}

int ControlBox::GetOffsetY() const
{
	return m_nOffsetY;
}

int ControlBox::GetItemCount() const 
{ 
	return (int)m_Items.size(); 
}

int ControlBox::GetCanDispCount() const
{
	int height = GetContentHeight();
	
	int num = height / m_nItemHeight;
	
	int top = GetContentTop();
	int bottom = top + height;

	int count = 0;
	
	for (int i = 0; i < num; i++)
	{
		int index = m_nTopIndex + i;
		
		if (size_t(index) >= m_Items.size())
		{
			top += m_nItemHeight;
		}
		else
		{
			top += m_Items[index].pControl->GetHeight() + m_nOffsetY;
		}

		if (top > bottom)
		{
			break;
		}
		
		count++;
	}

	return count;
}

int ControlBox::GetDispItemCount() const
{
	int num = (int)m_Items.size() - m_nTopIndex;
	int can_disp = GetCanDispCount();
	
	if (num > can_disp)
	{
		num = can_disp;
	}

	return num;
}

bool ControlBox::ShowSelectItem()
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

void ControlBox::SetSelectIndex(int value) 
{ 
	int old_value = m_nSelectIndex;

	m_nSelectIndex = value; 

	if (!m_bScrollSmooth)
	{
		ShowSelectItem();
		
		UpdateScroll();
	}

	if (old_value != value)
	{
		if (GetEnabled())
		{
			GuiUtil_RunCallback(this, "on_select_changed", 
				CVarList() << old_value);
		}
	}
}

int ControlBox::GetSelectIndex() const 
{ 
	return m_nSelectIndex; 
}

void ControlBox::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string ControlBox::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void ControlBox::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string ControlBox::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void ControlBox::SetSelectBackImage(const char* value) 
{ 
	m_SelectBackImage.SetName(value);
	m_SelectBackImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ControlBox::GetSelectBackImage() const 
{ 
	return m_SelectBackImage.GetName();
}

void ControlBox::SetScrollSmooth(bool value)
{
	m_bScrollSmooth = value;
}

bool ControlBox::GetScrollSmooth() const
{
	return m_bScrollSmooth;
}

int ControlBox::GetScrollHeight() const
{
	return m_nScrollHeight;
}

int ControlBox::GetScrollPage() const
{
	return GetContentHeight();
}

void ControlBox::SetScrollTop(int value)
{
	if ((value < 0) || (value >= m_nScrollHeight))
	{
		return;
	}
	
	m_nScrollTop = value;

	if (m_bScrollSmooth)
	{
		UpdateScroll();
	}
}

int ControlBox::GetScrollTop() const
{
	return m_nScrollTop;
}

void ControlBox::CalcScrollTotal()
{
	m_nScrollHeight = 0;
	m_nScrollWidth = 0;
	
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		m_nScrollHeight += m_Items[i].pControl->GetHeight() + m_nOffsetY;

		int width = m_Items[i].pControl->GetWidth();

		if (width > m_nScrollWidth)
		{
			m_nScrollWidth = width;
		}
	}
}

void ControlBox::ClearItems()
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		m_Items[i].pControl->Release();
	}
	
	m_Items.clear();
}

int ControlBox::FindControl(const PERSISTID& id) const
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i].pControl->GetID() == id)
		{
			return int(i);
		}
	}
	
	return -1;
}

int ControlBox::AddControl(const PERSISTID& id)
{
	IControl* pControl = IControl::GetControl(id);

	if (NULL == pControl)
	{
		return -1;
	}

	pControl->SetParentControl(this);
	pControl->SetDelegateControl(this);
	
	int pos = (int)m_Items.size();
	
	m_Items.push_back(item_t());
	m_Items.back().pControl = pControl;

	CalcScrollTotal();
	UpdateScroll();
	
	return pos;
}

int ControlBox::AddControlByIndex(const PERSISTID& id, int index)
{
	// 索引位置不应该大于当前容器内控件数目
	Assert(size_t(index) <= m_Items.size());

	IControl* pControl = IControl::GetControl(id);

	if (NULL == pControl)
	{
		return -1;
	}

	int pos = -1;

	if (size_t(index) < m_Items.size())
	{
		pos = index;
		m_Items.insert(index, item_t());
	}
	else
	{
		pos = (int)m_Items.size();
		m_Items.push_back(item_t());
	}

	pControl->SetParentControl(this);
	pControl->SetDelegateControl(this);

	m_Items[pos].pControl = pControl;

	if (m_nSelectIndex == pos)
	{
		SetSelectIndex(-1);
	}

	CalcScrollTotal();
	UpdateScroll();

	return pos;
}

bool ControlBox::RemoveControl(const PERSISTID& id)
{
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i].pControl->GetID() == id)
		{
			return RemoveByIndex((int)i);
		}
	}
	
	return false;
}

bool ControlBox::RemoveByIndex(int index)
{
	if (size_t(index) >= m_Items.size())
	{
		return false;
	}

	m_Items[index].pControl->Release();
	m_Items.remove(index);

	if (m_nSelectIndex == index)
	{
		SetSelectIndex(-1);
	}

	CalcScrollTotal();
	UpdateScroll();

	return true;
}

bool ControlBox::ClearControl()
{
	ClearItems();
	m_nScrollTop = 0;
	m_nScrollLeft = 0;
	CalcScrollTotal();
	UpdateScroll();

	return true;
}

PERSISTID ControlBox::GetControl(int index) const
{
	if (size_t(index) >= m_Items.size())
	{
		return PERSISTID();
	}

	return m_Items[index].pControl->GetID();
}

// 重载的模板方法

int ControlBox::GetVerticalTotal() const 
{ 
	if (m_bScrollSmooth)
	{
		return m_nScrollHeight;
	}
	else
	{
		return (int)m_Items.size();
	}
}

int ControlBox::GetVerticalCanDisp() const 
{ 
	if (m_bScrollSmooth)
	{
		return GetScrollPage();
	}
	else
	{
		return GetCanDispCount(); 
	}
}

void ControlBox::SetVerticalValue(int value) 
{
	if (m_bScrollSmooth)
	{
		m_nScrollTop = value;
	}
	else
	{
		m_nTopIndex = value;
	}
}

int ControlBox::GetVerticalValue() const 
{ 
	if (m_bScrollSmooth)
	{
		return m_nScrollTop;
	}
	else
	{
		return m_nTopIndex; 
	}
}

int ControlBox::GetHorizonTotal() const 
{ 
	return m_nScrollWidth;
}

int ControlBox::GetHorizonCanDisp() const 
{ 
	return GetContentWidth();
}

void ControlBox::SetHorizonValue(int value) 
{
	m_nScrollLeft = value;
}

int ControlBox::GetHorizonValue() const 
{ 
	return m_nScrollLeft;
}

void ControlBox::PaintScrollRect(float seconds)
{
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + GetContentHeight();
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	int top = 0;
	int left = 0 - m_nScrollLeft;
	int start_index = 0;

	if (m_bScrollSmooth)
	{
		int count = 0;

		for (size_t k = 0; k < m_Items.size(); ++k)
		{
			int h = m_Items[k].pControl->GetHeight() + m_nOffsetY;
			
			count += h;

			if (count > m_nScrollTop)
			{
				top = count - m_nScrollTop - h;
				start_index = (int)k;
				break;
			}
		}
	}
	else
	{
		start_index = m_nTopIndex;
	}
	
	for (int i = start_index; i < int(m_Items.size()); ++i)
	{
		IControl* pControl = m_Items[i].pControl;
		int item_height = pControl->GetHeight() + m_nOffsetY;
		
		if (m_bShowSelect && (i == m_nSelectIndex))
		{
			if (m_SelectBackImage.IsReady())
			{
				m_SelectBackImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
					x1 + left, y1 + top, x2, y1 + top + m_nItemHeight);
			}
			else
			{
				GetPainter()->FillRect(x1 + left, y1 + top, x2, 
					y1 + top + item_height,	m_SelectBackColor.GetValue());
			}
		}
		
		pControl->SetAbsLeft(x1 + left + m_nOffsetX);
		pControl->SetAbsTop(y1 + top + m_nOffsetY);
		pControl->Paint(seconds);
		
		top += item_height;

		if (top >= y2)
		{
			break;
		}
	}
}

// 事件处理

int ControlBox::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	IControl* pControl = OnGetInWhat(x, y);

	if (NULL == pControl)
	{
		return 0;
	}

	int select_index = -1;

	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		if (m_Items[i].pControl == pControl)
		{
			select_index = (int)i;
			break;
		}
	}

	if (select_index < 0)
	{
		return 0;
	}

	m_nSelectIndex = select_index;

	return GuiUtil_RunCallback(this, "on_select_click", 
		CVarList() << m_nSelectIndex << x << y);
}

int ControlBox::OnKeyDown(unsigned int vk)
{
	switch (vk)
	{
	case CGuiInput::KEY_UP:
		{
			if (m_bScrollSmooth)
			{
				if (m_nScrollTop > 0)
				{
					int new_value = m_nScrollTop - m_nItemHeight;

					if (new_value < 0)
					{
						new_value = 0;
					}

					SetScrollTop(new_value);

					return 1;
				}

				return 1;
			}
			
			if (m_bShowSelect)
			{
				if (m_nSelectIndex > 0)
				{
					SetSelectIndex(m_nSelectIndex - 1);

					return 1;
				}
			}
			else
			{
				if (m_nTopIndex > 0)
				{
					m_nTopIndex--;

					UpdateScroll();

					return 1;
				}
			}
		}
		break;
	case CGuiInput::KEY_DOWN:
		{
			if (m_bScrollSmooth)
			{
				int page = GetScrollPage();

				if (page > m_nItemHeight)
				{
					page -= m_nItemHeight;
				}
				
				if ((m_nScrollTop + page) < m_nScrollHeight)
				{
					int new_value = m_nScrollTop + m_nItemHeight;
					
					SetScrollTop(new_value);
					
					return 1;
				}

				return 1;
			}
			
			if (m_bShowSelect)
			{
				if (m_nSelectIndex < int(m_Items.size() - 1))
				{
					SetSelectIndex(m_nSelectIndex + 1);

					return 1;
				}
			}
			else
			{
				if ((m_nTopIndex + GetCanDispCount()) < int(m_Items.size()))
				{
					m_nTopIndex++;

					UpdateScroll();
					
					return 1;
				}
			}
		}
		break;
	case CGuiInput::KEY_PRIOR:
		{
			if (m_bScrollSmooth)
			{
				int page = GetScrollPage();
				
				if (page > m_nItemHeight)
				{
					page -= m_nItemHeight;
				}
				
				if (m_nScrollTop > 0)
				{
					int new_value = m_nScrollTop - page;

					if (new_value < 0)
					{
						new_value = 0;
					}
					
					SetScrollTop(new_value);
					
					return 1;
				}

				return 1;
			}
			
			int page_disp = GetCanDispCount() - 1;

			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			if (m_bShowSelect)
			{
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
			else
			{
				int new_index = m_nTopIndex;
				
				if (new_index > page_disp)
				{
					new_index -= page_disp;
				}
				else
				{
					new_index = 0;
				}
				
				m_nTopIndex = new_index;
				
				UpdateScroll();
				
				return 1;
			}
		}
		break;
	case CGuiInput::KEY_NEXT:
		{
			if (m_bScrollSmooth)
			{
				int page = GetScrollPage();
				
				if (page > m_nItemHeight)
				{
					page -= m_nItemHeight;
				}
				
				int new_value = m_nScrollTop + page;

				if (new_value < m_nScrollHeight)
				{
					if ((new_value + page) > m_nScrollHeight)
					{
						new_value = m_nScrollHeight - page;
					}

					SetScrollTop(new_value);

					return 1;
				}

				return 1;
			}
			
			int page_disp = GetCanDispCount() - 1;

			if (page_disp < 1)
			{
				page_disp = 1;
			}
			
			if (m_bShowSelect)
			{
				int new_index = m_nSelectIndex + page_disp;
				
				if (new_index >= int(m_Items.size()))
				{
					new_index = (int)m_Items.size() - 1;
				}
				
				SetSelectIndex(new_index);
			}
			else
			{
				int new_index = m_nTopIndex + page_disp;

				if ((new_index + page_disp) > int(m_Items.size()))
				{
					new_index = (int)m_Items.size() - page_disp;

					if (new_index < 0)
					{
						new_index = 0;
					}
				}

				if (new_index < int(m_Items.size()))
				{
					m_nTopIndex = new_index;

					UpdateScroll();

					return 1;
				}
			}

			return 1;
		}
		break;
	case CGuiInput::KEY_HOME:
		{
			if (m_bScrollSmooth)
			{
				SetScrollTop(0);
					
				return 1;
			}
			
			SetSelectIndex(0);

			return 1;
		}
		break;
	case CGuiInput::KEY_END:
		{
			if (m_bScrollSmooth)
			{
				int page = GetScrollPage();
				
				if (page > m_nItemHeight)
				{
					page -= m_nItemHeight;
				}

				int new_value = m_nScrollHeight - page;

				if (new_value < 0)
				{
					new_value = 0;
				}
				
				SetScrollTop(new_value);
				
				return 1;
			}

			SetSelectIndex((int)m_Items.size() - 1);

			return 1;
		}
		break;
	default:
		break;
	}

	return 0;
}

/*
IControl* ControlBox::OnGetInWhat(int x, int y)
{
	IControl* pScrollBar = IScrollable::OnGetScrollBar(x, y);

	if (pScrollBar)
	{
		// 选中的是滚动条，则直接返回
		return pScrollBar;
	}

	int x1 = GetContentLeft();
	int x2 = x1 + GetContentWidth();
	
	int top = GetContentTop();
	int bottom = top + GetContentHeight();
	
	int y1 = top;
	
	for (int i = m_nTopIndex; i < int(m_Items.size()); i++)
	{
		IControl* pControl = m_Items[i].pControl->GetInWhat(x, y);
		
		if (pControl != NULL)
		{
			return pControl;
		}

		y1 += m_Items[i].pControl->GetHeight() + m_nOffsetY;
		
		if (y1 >= bottom)
		{
			break;
		}
	}
	
	return IScrollable::OnGetInWhat(x, y);
}
*/

IControl* ControlBox::OnGetInWhat(int x, int y)
{
	if (!GetEnabled())
	{
		return NULL;
	}

	IControl* pScrollBar = IScrollable::OnGetScrollBar(x, y);

	if (pScrollBar)
	{
		// 选中的是滚动条，则直接返回
		return pScrollBar;
	}

	int x1 = GetContentLeft();
	int x2 = x1 + GetContentWidth();

	int top = GetContentTop();
	int bottom = top + GetContentHeight();

	int y1 = top;

	int start_index = m_nTopIndex;

	if (m_bScrollSmooth)
	{
		int count = 0;

		for (size_t k = 0; k < m_Items.size(); ++k)
		{
			int h = m_Items[k].pControl->GetHeight() + m_nOffsetY;

			count += h;

			if (count > m_nScrollTop)
			{
				y1 = count - m_nScrollTop - h;
				start_index = (int)k;
				break;
			}
		}
	}
	else
	{
		start_index = m_nTopIndex;
	}

	for (int i = start_index; i < int(m_Items.size()); i++)
	{
		IControl* pControl = m_Items[i].pControl->GetInWhat(x, y);

		if (pControl != NULL)
		{
			return pControl;
		}

		y1 += m_Items[i].pControl->GetHeight() + m_nOffsetY;

		if (y1 >= bottom)
		{
			break;
		}
	}

	return IScrollable::OnGetInWhat(x, y);
}

