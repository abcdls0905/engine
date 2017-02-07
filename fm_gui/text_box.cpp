//--------------------------------------------------------------------
// 文件名:		text_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "text_box.h"
#include "gui_input.h"
#include "../public/auto_mem.h"
#include "../public/converts.h"
#include "../public/core_file.h"

/// \file text_box.cpp
/// \brief 文本显示框

/// entity: TextBox
/// \brief 文本显示框实体
DECLARE_ENTITY(TextBox, 0, IScrollable);

/// readonly: int ItemCount
/// \brief 表项的数量
DECLARE_PROPERTY_GET(int, TextBox, ItemCount, GetItemCount);
/// readonly: int ChapterCount
/// \brief 段落的数量
DECLARE_PROPERTY_GET(int, TextBox, ChapterCount, GetChapterCount);
/// readonly: int ScrollHeight
/// \brief 总的平滑滚动高度
DECLARE_PROPERTY_GET(int, TextBox, ScrollHeight, GetScrollHeight);
/// readonly: int ScrollPage
/// \brief 平滑滚动每页的高度
DECLARE_PROPERTY_GET(int, TextBox, ScrollPage, GetScrollPage);

/// property: bool ShowSelect
/// \brief 是否显示被选中项
DECLARE_PROPERTY(bool, TextBox, ShowSelect, GetShowSelect, SetShowSelect);
/// property: int ItemHeight
/// \brief 表项的高度
DECLARE_PROPERTY(int, TextBox, ItemHeight, GetItemHeight, SetItemHeight);
/// property: int TextOffsetX
/// \brief 文字起始位移
DECLARE_PROPERTY(int, TextBox, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: int SelectIndex
/// \brief 被选中项的索引值，未选中为-1
DECLARE_PROPERTY(int, TextBox, SelectIndex, GetSelectIndex, SetSelectIndex);
/// property: int SelectChapter
/// \brief 被选中段落的索引值，未选中为-1
DECLARE_PROPERTY(int, TextBox, SelectChapter, GetSelectChapter, 
	SetSelectChapter);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, TextBox, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, TextBox, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string SelectBackImage
/// \brief 被选中时的背景图元
DECLARE_PROPERTY(const char*, TextBox, SelectBackImage, GetSelectBackImage, 
	SetSelectBackImage);
/// property: string RefCursor
/// \brief 引用项的鼠标形状
DECLARE_PROPERTY(const char*, TextBox, RefCursor, GetRefCursor, SetRefCursor);
/// property: bool ScrollSmooth
/// \brief 是否平滑滚动
DECLARE_PROPERTY(bool, TextBox, ScrollSmooth, GetScrollSmooth, 
	SetScrollSmooth);
/// property: int ScrollTop
/// \brief 平滑滚动的顶部位置
DECLARE_PROPERTY(int, TextBox, ScrollTop, GetScrollTop, SetScrollTop);

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
/// event: int on_click_ref(object self, wstring content, wstring ref)
/// \brief 鼠标点击引用项的回调
/// \param self 本控件
/// \param content 文本内容
/// \param ref 引用项内容
DECLARE_EVENT(on_click_ref);

// TextBox

TextBox::TextBox()
{
	m_bShowSelect = false;
	m_nItemHeight = 16;
	m_nTextOffsetX = 2;
	m_nTopIndex = 0;
	m_nSelectIndex = -1;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	m_bScrollSmooth = false;
	m_nScrollHeight = 0;
	m_nScrollTop = 0;
	
	SetCanFocus(true);
	SetTabStop(true);
}

TextBox::~TextBox()
{
	ClearItems();
	
	for (size_t i = 0; i < m_Chapters.size(); ++i)
	{
		CORE_DELETE(m_Chapters[i]);
	}
}

bool TextBox::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}

	m_strRefCursor = "WIN_HELP";
	
	// 创建垂直滚动条
	SetHasVScroll(true);
	
	return true;
}

bool TextBox::Shut()
{
	return IScrollable::Shut();
}

void TextBox::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);
	
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	CVar v = CVar(VTYPE_BOOL, true);
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
		&v);
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AlwaysHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HasHScroll", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);
	
	di->AddPropInfo("ShowSelect", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ItemHeight", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true, &v);
	di->AddPropInfo("SelectBackImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("RefCursor", IDesignInfo::TYPE_CURSOR, true, true);
	di->AddPropInfo("ScrollSmooth", IDesignInfo::TYPE_BOOL, true, true);
	
	di->AddSuborInfo("HScrollBar", "ScrollBar");
	di->AddSuborInfo("VScrollBar", "ScrollBar");
	di->AddSuborInfo("CornerLabel", "Label");
	
	di->AddEventInfo("on_select_changed");
	di->AddEventInfo("on_select_click");
	di->AddEventInfo("on_select_double_click");
	di->AddEventInfo("on_click_ref");
}

bool TextBox::GetLoadFinish()
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

void TextBox::SetShowSelect(bool value)
{
	m_bShowSelect = value;
}

bool TextBox::GetShowSelect() const
{
	return m_bShowSelect;
}

void TextBox::SetItemHeight(int value)
{
	if (value < MIN_ITEM_HEIGHT)
	{
		value = MIN_ITEM_HEIGHT;
	}

	m_nItemHeight = value;
}

int TextBox::GetItemHeight() const 
{ 
	return m_nItemHeight; 
}

void TextBox::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nTextOffsetX = value;
}

int TextBox::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

int TextBox::GetItemCount() const 
{ 
	return (int)m_Items.size(); 
}

int TextBox::GetCanDispCount() const
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
			top += m_Items[index].pMarkStr->GetHeight();
		}

		if (top > bottom)
		{
			break;
		}
		
		count++;
	}

	return count;
}

int TextBox::GetDispItemCount() const
{
	int num = (int)m_Items.size() - m_nTopIndex;
	int can_disp = GetCanDispCount();
	
	if (num > can_disp)
	{
		num = can_disp;
	}

	return num;
}

bool TextBox::ShowSelectItem()
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

void TextBox::SetSelectIndex(int value) 
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
			GuiUtil_RunCallback(this, 
				"on_select_changed", CVarList() << old_value);
		}
	}
}

int TextBox::GetSelectIndex() const 
{ 
	return m_nSelectIndex; 
}

void TextBox::SetSelectChapter(int value)
{
	if (size_t(value) >= m_Chapters.size())
	{
		m_nSelectIndex = -1;
	}
	else
	{
		SetSelectIndex((int)m_Chapters[value]->nStart);
	}
}

int TextBox::GetSelectChapter() const
{
	if (size_t(m_nSelectIndex) >= m_Items.size())
	{
		return -1;
	}

	return (int)m_Items[m_nSelectIndex].nChapter;
}

void TextBox::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string TextBox::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void TextBox::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string TextBox::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void TextBox::SetSelectBackImage(const char* value) 
{ 
	m_SelectBackImage.SetName(value);
	m_SelectBackImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* TextBox::GetSelectBackImage() const 
{ 
	return m_SelectBackImage.GetName();
}

void TextBox::SetRefCursor(const char* value)
{
	m_strRefCursor = value;
}

const char* TextBox::GetRefCursor() const
{
	return m_strRefCursor.c_str();
}

void TextBox::SetScrollSmooth(bool value)
{
	m_bScrollSmooth = value;
}

bool TextBox::GetScrollSmooth() const
{
	return m_bScrollSmooth;
}

int TextBox::GetScrollHeight() const
{
	return m_nScrollHeight;
}

int TextBox::GetScrollPage() const
{
	return GetContentHeight();
}

void TextBox::SetScrollTop(int value)
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

int TextBox::GetScrollTop() const
{
	return m_nScrollTop;
}

void TextBox::CalcScrollTotal()
{
	m_nScrollHeight = 0;
	
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		m_nScrollHeight += m_Items[i].pMarkStr->GetHeight();
	}
}

void TextBox::ClearItems()
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		m_Items[i].pMarkStr->Release();
	}
	
	m_Items.clear();
}

bool TextBox::GenChapter(size_t index)
{
	Assert(index < m_Chapters.size());
	
	chapter_t* chapter = m_Chapters[index];

	chapter->nStart = m_Items.size();
	chapter->nCount = 0;

	const wchar_t* text = chapter->wsText.c_str();
	int width = GetContentWidth() - m_nTextOffsetX * 2;
	CMarkStringStatus status;
	int total = (int)wcslen(text);
	int length = 0;
	
	do
	{
		m_Items.push_back(item_t());
		
		item_t& item = m_Items.back();
		
		item.nChapter = index;
		item.nForeColor = 0;
		item.pMarkStr = CMarkString::NewInstance(GetPainter());
		
		length += item.pMarkStr->CreateInWidth(GetGui(), 
			text + length, width, status);
		
		chapter->nCount++;
	} while (length < total);
	
	return true;
}

int TextBox::AddChapter(const wchar_t* text)
{
	Assert(text != NULL);
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

	chapter_t* pChapter = CORE_NEW(chapter_t);

	pChapter->wsText = text;
	pChapter->nStart = 0;
	pChapter->nCount = 0;
	
	m_Chapters.push_back(pChapter);
	GenChapter(m_Chapters.size() - 1);

	if (m_bScrollSmooth)
	{
		CalcScrollTotal();
	}

	UpdateScroll();

	return (int)m_Chapters.size() - 1;
}

bool TextBox::RemoveChapter(size_t index)
{
	return RemoveChapterRange(index, index);
}

bool TextBox::RemoveChapterRange(size_t beg, size_t end)
{
	if (beg >= m_Chapters.size())
	{
		return false;
	}

	if (end >= m_Chapters.size())
	{
		return false;
	}
	
	size_t item_beg = m_Chapters[beg]->nStart;
	size_t item_end = item_beg;

	for (size_t k = beg; k <= end; ++k)
	{
		item_end += m_Chapters[k]->nCount;
	}

	for (size_t i = item_beg; i < item_end; ++i)
	{
		m_Items[i].pMarkStr->Release();
        m_Items[i].pMarkStr = NULL;

		if (m_nSelectIndex == i)
		{
			m_nSelectIndex = -1;
		}
	}
	
	//m_Items.erase(m_Items.begin() + item_beg, m_Items.begin() + item_end);
	m_Items.remove_from(item_beg, item_end - item_beg);
	
	for (size_t c = beg; c < (end + 1); ++c)
	{
		CORE_DELETE(m_Chapters[c]);
	}

	m_Chapters.remove_from(beg, end + 1 - beg);
	//m_Chapters.erase(m_Chapters.begin() + beg, m_Chapters.begin() + end + 1);

	// 调整后面的段落数据
	size_t item_count = item_end - item_beg;

	for (size_t c = beg; c < m_Chapters.size(); ++c)
	{
		m_Chapters[c]->nStart -= item_count;
	}
	
	if (m_bScrollSmooth)
	{
		CalcScrollTotal();
	}
	
	UpdateScroll();
	
	return true;
}

bool TextBox::ClearChapter()
{
	ClearItems();

	for (size_t i = 0; i < m_Chapters.size(); ++i)
	{
		CORE_DELETE(m_Chapters[i]);
	}

	m_Chapters.clear();
	m_nTopIndex = 0;
	m_nSelectIndex = -1;

	if (m_bScrollSmooth)
	{
		m_nScrollTop = 0;
		CalcScrollTotal();
	}
	
	UpdateScroll();

	return true;
}

bool TextBox::RefreshChapter()
{
	GetGui()->SetFont(GuiUtil_GetValidFontName(this));
	
	// 记录当前顶部段落的索引
	int chapter_index = -1;
	float percent = 0.0F;

	if (size_t(m_nTopIndex) < m_Items.size())
	{
		chapter_index = (int)m_Items[m_nTopIndex].nChapter;
		percent = float(m_nTopIndex - m_Chapters[chapter_index]->nStart) 
			/ float(m_Chapters[chapter_index]->nCount);

		if (percent > 0.1F)
		{
			percent -= 0.01F;
		}
	}
	
	BeginUpdate();

	ClearItems();
	
	for (size_t i = 0; i < m_Chapters.size(); ++i)
	{
		GenChapter(i);
	}

	// 恢复当前顶部段落位置
	if (chapter_index >= 0)
	{
		m_nTopIndex = int(m_Chapters[chapter_index]->nStart
			+ float(m_Chapters[chapter_index]->nCount) * percent);
	}

	if (m_bScrollSmooth)
	{
		CalcScrollTotal();
	}
	
	EndUpdate();
	
	return true;
}

int TextBox::GetChapterCount() const
{
	return (int)m_Chapters.size();
}

const wchar_t* TextBox::GetChapter(size_t index) const
{
	if (index >= m_Chapters.size())
	{
		return L"";
	}

	return m_Chapters[index]->wsText.c_str();
}

bool TextBox::SetChapterTag(size_t index, const PERSISTID& id)
{
	if (index >= m_Chapters.size())
	{
		return false;
	}

	m_Chapters[index]->TagID = id;

	return true;
}

PERSISTID TextBox::GetChapterTag(size_t index) const
{
	if (index >= m_Chapters.size())
	{
		return PERSISTID();
	}

	return m_Chapters[index]->TagID;
}

bool TextBox::SetChapterForeColor(size_t index, const char* color)
{
	if (index >= m_Chapters.size())
	{
		return false;
	}

	size_t start = m_Chapters[index]->nStart;
	size_t count = m_Chapters[index]->nCount;

	for (size_t i = start; i < (start + count); ++i)
	{
		m_Items[i].nForeColor = GuiUtil_ColorToInt(color);
	}

	return true;
}

bool TextBox::LoadText(const wchar_t* text)
{
	Assert(text != NULL);
	
	// 必须设置字体，否则无法得知行的高度
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	BeginUpdate();
	ClearChapter();
	
	size_t len = wcslen(text);
	TAutoMem<wchar_t, 256, TCoreAlloc> auto_buf(len + 1);
	wchar_t* buf = auto_buf.GetBuffer();
	size_t count = 0;

	for (const wchar_t* p = text; *p; ++p)
	{
		if (*p == '\n')
		{
			buf[count] = 0;
			AddChapter(buf);
			count = 0;
		}
		else if (*p == '\r')
		{
		}
		else if (*p == '\t')
		{
			// Tab键转换为最多4个空格
			size_t k = (count + 4) / 4 * 4;

			while (count < k)
			{
				buf[count++] = ' ';
			}
		}
		else
		{
			buf[count++] = *p;
		}
	}

	if (count > 0)
	{
		buf[count] = 0;

		AddChapter(buf);
	}

	EndUpdate();
	
	return true;
}

bool TextBox::LoadFile(const char* file)
{
	Assert(file != NULL);

	FILE* fp = core_file::fopen(file, "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buf = auto_buf.GetBuffer();

	if (core_file::fread(buf, sizeof(char), file_size, fp) != file_size)
	{
		return false;
	}
	
	core_file::fclose(fp);
	buf[file_size] = 0;

	size_t wide_size = GetToWideStrLen(buf);
	TAutoMem<wchar_t, 256, TCoreAlloc> auto_buf1(wide_size);
	wchar_t* buf1 = auto_buf1.GetBuffer();

	ToWideStr(buf, buf1, wide_size * sizeof(wchar_t));

	return LoadText(buf1);
}

bool TextBox::SaveFile(const char* file)
{
	Assert(file != NULL);

	FILE* fp = core_file::fopen(file, "wb");

	if (NULL == fp)
	{
		return false;
	}

	for (size_t i = 0; i < m_Chapters.size(); ++i)
	{
		const wchar_t* text = m_Chapters[i]->wsText.c_str();
		size_t size = GetToStringLen(text);
		TAutoMem<char, 256, TCoreAlloc> auto_buf(size);
		char* buf = auto_buf.GetBuffer();

		ToString(text, buf, size);
		core_file::fprintf(fp, "%s\r\n", buf);
	}

	core_file::fclose(fp);

	return true;
}

// 重载的模板方法

int TextBox::GetVerticalTotal() const 
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

int TextBox::GetVerticalCanDisp() const 
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

void TextBox::SetVerticalValue(int value) 
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

int TextBox::GetVerticalValue() const 
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

int TextBox::GetHorizonTotal() const 
{ 
	return 10; 
}

int TextBox::GetHorizonCanDisp() const 
{ 
	return 10; 
}

void TextBox::SetHorizonValue(int value) 
{
}

int TextBox::GetHorizonValue() const 
{ 
	return 0; 
}

void TextBox::PaintScrollRect(float seconds)
{
	int x1 = GetContentLeft();
	int y1 = GetContentTop();
	int x2 = x1 + GetContentWidth();
	int y2 = y1 + GetContentHeight();
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	int top = 0;
	int start_index = 0;

	if (m_bScrollSmooth)
	{
		int count = 0;

		for (size_t k = 0; k < m_Items.size(); ++k)
		{
			int h = m_Items[k].pMarkStr->GetHeight();
			
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
		CMarkString* pMarkStr = m_Items[i].pMarkStr;
		
		int item_height = pMarkStr->GetHeight();
		
		if (m_bShowSelect && (i == m_nSelectIndex))
		{
			if (m_SelectBackImage.IsReady())
			{
				m_SelectBackImage.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
					x1, y1 + top, x2, y1 + top + m_nItemHeight);
			}
			else
			{
				GetPainter()->FillRect(x1, y1 + top, 
					x2, y1 + top + item_height, 
					m_SelectBackColor.GetValue());
			}
			
			unsigned int fore_color = m_Items[i].nForeColor;

			if (0 == fore_color)
			{
				fore_color = m_SelectForeColor.GetValue();
			}

			pMarkStr->Write(seconds, x1 + m_nTextOffsetX, y1 + top, 
				fore_color);
		}
		else
		{
			unsigned int fore_color = m_Items[i].nForeColor;

			if (0 == fore_color)
			{
				fore_color = GetForeColor();
			}
			
			pMarkStr->Write(seconds, x1 + m_nTextOffsetX, y1 + top, 
				fore_color);
		}
		
		top += pMarkStr->GetHeight();

		if (top >= y2)
		{
			break;
		}
	}
}

// 事件处理

int TextBox::ClickItem(int select_index)
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

int TextBox::DoubleClickItem(int select_index)
{
	if (select_index < 0)
	{
		return 0;
	}

	return GuiUtil_RunCallback(this, "on_select_double_click", 
		CVarList() << select_index);
}

int TextBox::OnSetCursor(int x, int y)
{
	if (m_bScrollSmooth)
	{
		return IScrollable::OnSetCursor(x, y);
	}

	int x1 = GetContentLeft();
	int x2 = x1 + GetContentWidth();
	
	int top = GetContentTop();
	int bottom = top + GetContentHeight();
	
	int y1 = top;

	for (int i = m_nTopIndex; i < int(m_Items.size()); i++)
	{
		CMarkString* pMarkStr = m_Items[i].pMarkStr;
		
		int y2 = y1 + pMarkStr->GetHeight();
		
		if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2))
		{
			int offset_x = x - x1;
			int offset_y = y - y1;
			
			int part = pMarkStr->GetInPart(offset_x, offset_y);
			
			if (part >= 0)
			{
				if (!WideStrEmpty(pMarkStr->GetPartRef(part)))
				{
					if (GetGui()->SetCurrentCursor(m_strRefCursor.c_str()))
					{
						return 1;
					}
				}
			}

			break;
		}
		
		y1 = y2;
		
		if (y1 >= bottom)
		{
			break;
		}
	}
	
	return IScrollable::OnSetCursor(x, y);
}

int TextBox::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	if (m_bScrollSmooth)
	{
		return 0;
	}
	
	int x1 = GetContentLeft();
	int x2 = x1 + GetContentWidth();
	
	int top = GetContentTop();
	int bottom = top + GetContentHeight();
	
	int y1 = top;

	for (int i = m_nTopIndex; i < int(m_Items.size()); i++)
	{
		CMarkString* pMarkStr = m_Items[i].pMarkStr;
		
		int y2 = y1 + pMarkStr->GetHeight();
		
		if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2))
		{
			if (m_nSelectIndex != i)
			{
				SetSelectIndex(i);
			}
			
			int offset_x = x - x1;
			int offset_y = y - y1;
			
			int part = pMarkStr->GetInPart(offset_x, offset_y);
			
			if (part >= 0)
			{
				const wchar_t* ref = pMarkStr->GetPartRef(part);
				
				if (!WideStrEmpty(ref))
				{
					const wchar_t* content = pMarkStr->GetPartContent(part);

					if (GuiUtil_RunCallback(this, "on_click_ref", 
						CVarList() << content << ref))
					{
						return 1;
					}
				}
			}

			ClickItem(i);
			
			break;
		}
		
		y1 = y2;

		if (y1 >= bottom)
		{
			break;
		}
	}
	
	return 0;
}

int TextBox::OnLButtonDoubleClick(int x, int y) 
{ 
	if (!GetEnabled())
	{
		return 0;
	}

	if (m_bScrollSmooth)
	{
		return 0;
	}

	int x1 = GetContentLeft();
	int x2 = x1 + GetContentWidth();

	int top = GetContentTop();
	int bottom = top + GetContentHeight();

	int y1 = top;

	for (int i = m_nTopIndex; i < int(m_Items.size()); i++)
	{
		CMarkString* pMarkStr = m_Items[i].pMarkStr;

		int y2 = y1 + pMarkStr->GetHeight();

		if ((x >= x1) && (x <= x2) && (y >= y1) && (y <= y2))
		{
			if (m_nSelectIndex != i)
			{
				SetSelectIndex(i);
			}

			DoubleClickItem(i);

			break;
		}

		y1 = y2;

		if (y1 >= bottom)
		{
			break;
		}
	}

	return 0;
}

int TextBox::OnKeyDown(unsigned int vk)
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
				
				if (size_t(new_index) >= m_Items.size())
				{
					new_index = (int)m_Items.size() - 1;
				}
				
				SetSelectIndex(new_index);
			}
			else
			{
				int new_index = m_nTopIndex + page_disp;

				if (size_t(new_index + page_disp) > m_Items.size())
				{
					new_index = (int)m_Items.size() - page_disp;

					if (new_index < 0)
					{
						new_index = 0;
					}
				}

				if (size_t(new_index) < m_Items.size())
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

