//--------------------------------------------------------------------
// 文件名:		edit.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "edit.h"
#include "caret.h"
#include "gui_input.h"
#include "gui_utils.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../visual/i_font.h"
/// \file edit.cpp
/// \brief 文本输入框

/// entity: Edit
/// \brief 文本输入框实体
/// \see 继承自IControl
DECLARE_ENTITY(Edit, 0, IControl);

/// property: bool ReadOnly
/// \brief 是否只读
DECLARE_PROPERTY(bool, Edit, ReadOnly, GetReadOnly, SetReadOnly);
/// property: bool OnlyDigit
/// \brief 是否只允许输入数字
DECLARE_PROPERTY(bool, Edit, OnlyDigit, GetOnlyDigit, SetOnlyDigit);
/// property: int MaxDigit
/// \brief 只允许输入数字时，允许输入的最大值
DECLARE_PROPERTY(int, Edit, MaxDigit, GetMaxDigit, SetMaxDigit);
/// property: bool Marked
/// \brief 是否包含转义符号
DECLARE_PROPERTY(bool, Edit, Marked, GetMarked, SetMarked);
/// property: bool DragEvent
/// \brief 是否产生拖动事件 
DECLARE_PROPERTY(bool, Edit, DragEvent, GetDragEvent, SetDragEvent);
/// property: bool ChangedEvent
/// \brief 是否产生变化事件 
DECLARE_PROPERTY(bool, Edit, ChangedEvent, GetChangedEvent, SetChangedEvent);
/// property: int MaxLength
/// \brief 文本的最大长度
DECLARE_PROPERTY(int, Edit, MaxLength, GetMaxLength, SetMaxLength);
/// property: int TextOffsetX
/// \brief 文字起始位移
DECLARE_PROPERTY(int, Edit, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(const char*, Edit, Align, GetAlignString, SetAlignString);
/// property: int InputPos
/// \brief 输入位置
DECLARE_PROPERTY(int, Edit, InputPos, GetInputPos, SetInputPos);
/// property: wstring PasswordChar
/// \brief 作为密码输入时显示的字符
DECLARE_PROPERTY(const wchar_t*, Edit, PasswordChar, GetPasswordChar, 
	SetPasswordChar);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, Edit, SelectForeColor, 
	GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, Edit, SelectBackColor, 
	GetSelectBackColorString, SetSelectBackColorString);
/// property: string Caret
/// \brief 输入光标资源名
DECLARE_PROPERTY(const char*, Edit, Caret, GetCaretName, SetCaretName);
/// property: bool IsSelectText
/// \brief 是否选择了文本
DECLARE_PROPERTY_GET(bool, Edit, IsSelectText, HasSelectText);

/// event: int on_get_focus(object self, object lost);
/// \brief 获得输入焦点的回调
/// \param self 本控件
/// \param lost 失去焦点的控件
DECLARE_EVENT(on_get_focus);
/// event: int on_lost_focus(object self, object get);
/// \brief 失去输入焦点的回调
/// \param self 本控件
/// \param get 获得焦点的控件
DECLARE_EVENT(on_lost_focus);
/// event: int on_drag(object self, int drag_x, int drag_y)
/// \brief 鼠标按住拖动的回调
/// \param self 本控件
/// \param drag_x X方向的拖动距离
/// \param drag_y Y方向的拖动距离
DECLARE_EVENT(on_drag);
/// event: int on_enter(object self)
/// \brief 输入回车键的回调
/// \param self 本控件

/// event: int on_changed(object self)
/// \brief Text改变的回调
/// \param self 本控件
DECLARE_EVENT(on_changed);
// Edit

Edit::Edit()
{
	m_bReadOnly = false;
	m_bOnlyDigit = false;
	m_bMarked = false;
	m_bDragEvent = false;
	m_bChangedEvent = false;
	m_nDragOldX = 0;
	m_nDragOldY = 0;
	m_nMaxLength = 0;
	m_nTextOffsetX = 2;
	m_nAlign = CGuiConst::ALIGN_LEFT;
	m_nScrollStep = 6;
	m_nInputPos = 0;
	m_nShowPos = 0;
	m_nShowCount = 0;
	m_nSelectBeg = 0;
	m_nSelectEnd = 0;
    m_nMaxDigit = 0;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	
    m_CaretPos.x = 0;
    m_CaretPos.y = 0;

	SetDesignSize(80, 20);
	SetCanFocus(true);
	SetTabStop(true);

}

Edit::~Edit()
{
	ReleaseAll();
}

void Edit::ReleasePart(part_base_t* part)
{
	int type = part->nType;
	
	if (type == PARTTYPE_CHAR)
	{
		CORE_DELETE((part_char_t*)part);
	}
	else if (type == PARTTYPE_IMAGE)
	{
		part_image_t* pImage = (part_image_t*)part;
		
		pImage->Image.Release();
		CORE_DELETE(pImage);
	}
}

void Edit::ReleaseAll()
{
	for (size_t i = 0; i < m_Buffer.size(); ++i)
	{
		ReleasePart(m_Buffer[i]);
        m_Buffer[i] = NULL;
	}
	
	m_Buffer.clear();
}

bool Edit::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}
	
	SetCaretName("Default");
	SetCursorName("WIN_IBEAM");
	
	return true;
}

bool Edit::Shut()
{
	return IControl::Shut();
}

void Edit::SetReadOnly(bool value) 
{ 
	m_bReadOnly = value; 
}

bool Edit::GetReadOnly() const 
{ 
	return m_bReadOnly; 
}

void Edit::SetOnlyDigit(bool value)
{
	m_bOnlyDigit = value;
}

bool Edit::GetOnlyDigit() const
{
	return m_bOnlyDigit;
}

void Edit::SetMaxDigit(int value)
{
    if (m_nMaxDigit != value)
    {
        m_nMaxDigit = value;

        if (m_bOnlyDigit)
        {
            int nCurValue = GuiUtil_WideStrToInt(GetText());

            if (nCurValue > m_nMaxDigit)
            {
				result_wstring wcsValue = 
					GuiUtil_IntToWideStr(m_nMaxDigit).c_str();

                SetText(wcsValue.c_str());
            }
        }
    }
}

int Edit::GetMaxDigit() const 
{
    return m_nMaxDigit;
}

void Edit::SetMarked(bool value)
{
	m_bMarked = value;
	
	SetText(GetText());
}

bool Edit::GetMarked() const
{
	return m_bMarked;
}

void Edit::SetDragEvent(bool value)
{
	m_bDragEvent = value;
}

bool Edit::GetDragEvent() const
{
	return m_bDragEvent;
}

void Edit::SetChangedEvent(bool value)
{
	m_bChangedEvent = value;
}

bool Edit::GetChangedEvent() const
{
	return m_bChangedEvent;
}

void Edit::SetMaxLength(int value) 
{ 
	if (value > GetMaxSize())
	{
		return;
	}

	m_nMaxLength = value; 
}
 
int Edit::GetMaxLength() const 
{ 
	return m_nMaxLength; 
}

void Edit::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nTextOffsetX = value;
}

int Edit::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

void Edit::SetAlignString(const char* value)
{
	if (stricmp(value, "Left") == 0)
	{
		m_nAlign = CGuiConst::ALIGN_LEFT;
	}
	else if (stricmp(value, "Center") == 0)
	{
		m_nAlign = CGuiConst::ALIGN_CENTER;
	}
	else if (stricmp(value, "Right") == 0)
	{
		m_nAlign = CGuiConst::ALIGN_RIGHT;
	}
}

const char* Edit::GetAlignString() const
{
	switch (m_nAlign)
	{
	case CGuiConst::ALIGN_LEFT:
		return "Left";
	case CGuiConst::ALIGN_CENTER:
		return "Center";
	case CGuiConst::ALIGN_RIGHT:
		return "Right";
	default:
		Assert(0);
		break;
	}
	
	return "";
}

void Edit::SetPasswordChar(const wchar_t* value)
{
	Assert(value != NULL);
	
	m_wsPswdChar = value;
}

const wchar_t* Edit::GetPasswordChar() const
{
	return m_wsPswdChar.c_str();
}

// �输入位置
const int Edit::GetInputPos() const
{
    return m_nInputPos;
}

void Edit::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string Edit::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void Edit::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string Edit::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

void Edit::SetText(const wchar_t* val)
{
	Assert(val != NULL);
	
	ReleaseAll();
	AppendText(-1, val, (int)wcslen(val));
	m_wsOldText = m_wsText;
	m_wsText = val;
	m_nInputPos = 0;
	m_nShowPos = 0;
	m_nShowCount = 0;
	ClearSelect();
	CalcVisText();
	OnChanged(this->GetID());
}

void Edit::SetOldText(const wchar_t* val)
{
	Assert(val != NULL);

	m_wsOldText = val;
}

const wchar_t* Edit::GetText() const
{
	return m_wsText.c_str();
}

const wchar_t* Edit::GetOldText() const
{
	return m_wsOldText.c_str();
}

point_t Edit::GetCaretPos()
{
	return m_CaretPos;
}

void Edit::SetCaretName(const char* name)
{
	Assert(name != NULL);
	
	if (!SetCaretID(name))
	{
		return;
	}
	
	m_strCaretName = name;
}

const char* Edit::GetCaretName() const
{
	return m_strCaretName.c_str();
}

bool Edit::SetCaretID(const char* name)
{
	Assert(name != NULL);
	
	PERSISTID id = GetGui()->GetCaret(name);
	
	if (id.IsNull())
	{
		return false;
	}
	
	m_CaretID = id;
	
	return true;
}

int Edit::GetSize() const
{
	return (int)m_Buffer.size();
}

int Edit::GetMaxSize() const
{
	return 0x10000;
}

int Edit::AppendText(int pos, const wchar_t* ws, int size)
{
	Assert(ws != NULL);
	Assert(size >= 0);

	wchar_t pswd_char = 0;
	
	if (!m_wsPswdChar.empty())
	{
		pswd_char = m_wsPswdChar.c_str()[0];
	}
	
	int count = 0;
	
	// 设置字体以获得正确的宽度
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

	const wchar_t* end = ws + size;
	
	const wchar_t* p = ws;
	
	while (p < end)
	{
		if (m_nMaxLength > 0)
		{
			// 限制最大长度
			if (m_Buffer.size() >= size_t(m_nMaxLength))
			{
				break;
			}
		}

		if (m_bMarked && (*p == L'{'))
		{
			wchar_t flag = p[1];
			
			if ((flag == L'i') || (flag == L'I'))
			{
				// 图元
				const wchar_t* beg = p + 2;
				const wchar_t* end = wcschr(beg, L'}');
				
				if (end != NULL)
				{
					result_wstring ws(beg, end - beg);
					char image[128];
					
					//ToStringLen(beg, end - beg, image, sizeof(image));
					ToString(ws.c_str(), image, sizeof(image));
					
					part_image_t* pImage = CORE_NEW(part_image_t);
					
					pImage->nType = PARTTYPE_IMAGE;
					pImage->wsText = result_wstring(beg, end - beg).c_str();
					pImage->Image.SetName(image);
					pImage->Image.Create(GetPainter(), GetGui(), false);
					
					if (pImage->Image.IsReady())
					{
						// 将图元显示为字体的高度的正方形
						pImage->nWidth = GetPainter()->GetTextHeight(L" ") 
							+ GetPainter()->GetTextInterval();
					}
					else
					{
						pImage->nWidth = 0;
					}
					
					if (pos < 0)
					{
						m_Buffer.push_back(pImage);
					}
					else
					{
						m_Buffer.insert(pos, pImage);
						pos++;
					}
					
					count++;
					p = end + 1;
					continue;
				}
			}
		}
		
		part_char_t* pChar = CORE_NEW(part_char_t);
		
		pChar->nType = PARTTYPE_CHAR;
		pChar->nOrigin = *p;

		if (pswd_char != 0)
		{
			pChar->nVisual = pswd_char;
		}
		else
		{
			pChar->nVisual = *p;
		}

		wchar_t wcsBuffer[3];

		wcsBuffer[0] = pChar->nVisual;
		wcsBuffer[1] = 0;
		char_info_t info;
		if(GetPainter()->GetFontPtr()->GetCharDataInfo(pChar->nVisual, info))
		{
			pChar->nAdvance = info.nXAdvance;
			pChar->nOffset = info.nXOffset + info.nUserYOffset;
			pChar->nWidth = info.nCharWidth;
		}
		else
		{
			Assert(0);
		}
		
		if (pos < 0)
		{
			m_Buffer.push_back(pChar);
		}
		else
		{
			m_Buffer.insert(pos, pChar);
			pos++;
		}

		count++;
		p++;
	}

	return count;
}

void Edit::GenerateText(part_container_t& parts, int pos, int num, 
	core_wstring& ws)
{
	ws = L"";
	
	int end = pos + num;

	if (size_t(end) > parts.size())
	{
		end = (int)parts.size();
	}

	for (int i = pos; i < end; ++i)
	{
		int type = parts[i]->nType;

		if (type == PARTTYPE_CHAR)
		{
			part_char_t* pChar = (part_char_t*)parts[i];

			ws += pChar->nOrigin;
		}
		else if (type == PARTTYPE_IMAGE)
		{
			part_image_t* pImage = (part_image_t*)parts[i];

			ws += pImage->wsText.c_str();
		}
	}
}

int Edit::InsertText(int pos, const wchar_t* ws, int size)
{
	Assert( (pos >= 0) && (pos <= GetSize()) );
	Assert(ws != NULL);
	Assert(size >= 0);
	
	if (m_bReadOnly)
	{
		return 0;
	}
	
	if ((GetSize() + size) > GetMaxSize())
	{
		return 0;
	}
	
	int count = AppendText(pos, ws, size);

	if (count > 0)
	{
		m_wsOldText = m_wsText;
		// 同步
		GenerateText(m_Buffer, 0, (int)m_Buffer.size(), m_wsText);
	}
	
	return count;
}

bool Edit::DeleteText(int pos, int num)
{
	Assert((pos >= 0) && (pos <= GetSize()));
	
	if (m_bReadOnly)
	{
		return false;
	}
	
	if ((pos + num) > GetSize())
	{
		return false;
	}

	for (int i = pos; i < (pos + num); ++i)
	{
		ReleasePart(m_Buffer[i]);
	}
	
	//m_Buffer.erase(m_Buffer.begin() + pos, m_Buffer.begin() + pos + num);
	m_Buffer.remove_from(pos, num);
	
	if (num > 0)
	{
		m_wsOldText = m_wsText;
		// 同步
		GenerateText(m_Buffer, 0, (int)m_Buffer.size(), m_wsText);
	}
	
	return true;
}

result_wstring Edit::GetSelectText()
{
	if (!HasSelectText())
	{
		return L"";
	}
	
	int beg = GetSelectBeg();
	int end = GetSelectEnd();
	
	core_wstring ws;

	GenerateText(m_Buffer, beg, end - beg, ws);

	return ws.c_str();
}

bool Edit::HasSelectText() const
{
	if (m_nSelectBeg == m_nSelectEnd)
	{
		return false;
	}
	
	if ((m_nSelectBeg > GetSize()) || (m_nSelectEnd > GetSize()))
	{
		return false;
	}
	
	return true;
}

bool Edit::ClearSelect()
{
	m_nSelectBeg = 0;
	m_nSelectEnd = 0;
	return true;
}

bool Edit::Cut()
{
	if (!HasSelectText())
	{
		return false;
	}
	
	// 删除的同时复制到粘帖板
	if (!Copy())
	{
		return false;
	}
	
	if (!DeleteSelectText())
	{
		return false;
	}

	return true;
}

bool Edit::Copy()
{
	// 如果是密码状态不允许拷贝
	if (GetPasswordStage())
	{
		return false;
	}

	result_wstring ws = GetSelectText();
	
	if (ws.empty())
	{
		return false;
	}
	
	return GuiUtil_CopyToClipboard(ws.c_str());
}

bool Edit::Paste()
{
	result_wstring ws;
	
	if (!GuiUtil_PasteFromClipboard(ws))
	{
		return false;
	}
	
	if (ws.empty())
	{
		return false;
	}
	
	if (HasSelectText())
	{
		DeleteSelectText();
	}
	
	if (m_nMaxLength > 0)
	{
		// 限制最大长度
		int remain_length = m_nMaxLength - (int)m_Buffer.size();

		if (remain_length <= 0)
		{
			return false;
		}

		if (remain_length < (int)ws.length())
		{
			ws = ws.substr(0, remain_length);
		}
	}

	int count = InsertText(m_nInputPos, ws.c_str(), (int)ws.length());

	if (0 == count)
	{
        CalcVisText();
		return false;
	}
	
	SetInputPos(m_nInputPos + count);
	
	return true;
}

bool Edit::Append(const wchar_t* text)
{
    if (text == NULL)
    {
        return false;
    }

    //std::wstring wcsBuffer = text;
	size_t len = wcslen(text);
    
    for (size_t i = 0; i < len; ++i)
    {
        OnChar(text[i]);
    }

	return true;
}

int Edit::GetSelectBeg() const
{
	if (m_nSelectEnd >= m_nSelectBeg)
	{
		return m_nSelectBeg;
	}
	else
	{
		return m_nSelectEnd;
	}
}

int Edit::GetSelectEnd() const
{
	if (m_nSelectEnd >= m_nSelectBeg)
	{
		return m_nSelectEnd;
	}
	else
	{
		return m_nSelectBeg;
	}
}

bool Edit::IncSelect(int pos, int num)
{
	if (m_nSelectBeg == m_nSelectEnd)
	{
		m_nSelectBeg = pos;
		m_nSelectEnd = pos + num;
	}
	else
	{
		if (pos == m_nSelectEnd)
		{
			m_nSelectEnd += num;
		}
		else
		{
			ClearSelect();

			return IncSelect(pos, num);
		}
	}

	m_nSelectBeg = NormalizePos(m_nSelectBeg);
	m_nSelectEnd = NormalizePos(m_nSelectEnd);
	
	return true;
}

bool Edit::DeleteSelectText()
{
	Assert(HasSelectText());

	int beg = GetSelectBeg();
	int end = GetSelectEnd();

	if (!DeleteText(beg, end - beg))
	{
		return false;
	}

	ClearSelect();

	SetInputPos(beg);

	return true;
}

int Edit::NormalizePos(int pos) const
{
	if (pos < 0)
	{
		return 0;
	}

	int len = GetSize();
	
	if (pos > len)
	{
		return len;
	}

	return pos;
}

void Edit::SetShowPos(int pos)
{
	Assert( (pos >= 0) && (pos <= GetSize()) );

	if (m_nShowPos != pos)
	{
		m_nShowPos = pos;
		CalcVisText();
	}
}

int Edit::GetVisualCount(int pos, int width)
{
	int w = 0;
	
	for (size_t i = pos; i < m_Buffer.size(); ++i)
	{
		part_base_t* part = m_Buffer[i];
		
		int cur_w = w + part->nWidth + part->nOffset;
		if (cur_w > width)
		{
			return ((int)i - pos);
		}
		w += m_Buffer[i]->nAdvance;
		w += GetPainter()->GetTextInterval();
	}
	
	return ((int)m_Buffer.size() - pos);
}

void Edit::SetInputPos(int pos)
{
	Assert( (pos >= 0) && (pos <= GetSize()) );

	int show_pos = m_nShowPos;

	if (show_pos > pos)
	{
		show_pos = pos;
	}
	
	int show_width = GetShowWidth();

	while (GetVisualCount(show_pos, show_width) < (pos - show_pos))
	{
		if (++show_pos >= pos)
		{
			show_pos = pos;
			break;
		}
	}
	
	if (pos == show_pos)
	{
		show_pos -= m_nScrollStep;
		
		if (show_pos < 0)
		{
			show_pos = 0;
		}
	}
	
	SetShowPos(show_pos);
	
	m_nInputPos = pos;
}

int Edit::GetCaretWidth() const
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (NULL == caret)
	{
		return 0;
	}
	
	return caret->GetFixWidth();
}

int Edit::GetShowWidth() const
{
	return GetWidth() - GetCaretWidth() - m_nTextOffsetX * 2;
}

void Edit::CalcVisText()
{
	Assert( (m_nShowPos >= 0) && (m_nShowPos <= GetSize()) );
	
	m_nShowCount = GetVisualCount(m_nShowPos, GetShowWidth());
}

void Edit::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	
	di->AddPropInfo("ReadOnly", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("OnlyDigit", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("MaxDigit", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Marked", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("DragEvent", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("ChangedEvent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("MaxLength", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true);
    CVar v = CVar(VTYPE_STRING, "Left");
	di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, true, true,	&v);
	di->AddPropInfo("PasswordChar", IDesignInfo::TYPE_WIDESTR, true, true);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str());
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);
    v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str());
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true, &v );
	di->AddPropInfo("Caret", IDesignInfo::TYPE_STRING, true, true);

	di->AddEventInfo("on_get_focus");
	di->AddEventInfo("on_lost_focus");
	di->AddEventInfo("on_drag");
    di->AddEventInfo("on_changed");
	di->AddEventInfo("on_enter");
}

void Edit::PaintSelectText(int x1, int y1)
{
	int beg = GetSelectBeg();
	int end = GetSelectEnd();
	
	if (beg < m_nShowPos)
	{
		beg = m_nShowPos;
	}

	if (beg >= (m_nShowPos + m_nShowCount))
	{
		return;
	}
	
	if (end <= m_nShowPos)
	{
		return;
	}
	
	if (end > (m_nShowPos + m_nShowCount))
	{
		end = m_nShowPos + m_nShowCount;
	}

	if (beg >= end)
	{
		return;
	}

	int pos = beg - m_nShowPos;
	int num = end - beg;

	// 起始位置
	int w_start = 0;

	for (int k = 0; k < pos; ++k)
	{
		if( k + 1 == pos)
		{
			w_start += m_Buffer[m_nShowPos + k]->nOffset;
			w_start += m_Buffer[m_nShowPos + k]->nWidth;
		}
		else
		{
			w_start += m_Buffer[m_nShowPos + k]->nAdvance;
			w_start += GetPainter()->GetTextInterval();
		}
	}
	
	unsigned int back_color = m_SelectBackColor.GetValue();
	unsigned int fore_color = m_SelectForeColor.GetValue();
	
	TAutoMem<wchar_t, 128, TCoreAlloc> auto_buf(m_nShowCount + 1);
	wchar_t* buf = auto_buf.GetBuffer();
	
	int count = 0;
	int h = GetPainter()->GetTextHeight(L" ");
	int w = 0;
	int x = x1 + w_start + m_nTextOffsetX;
	int y = y1 + (GetHeight() - h) / 2;
	
	for (int i = pos; i < (pos + num); ++i)
	{
		int type = m_Buffer[m_nShowPos + i]->nType;
		
		if (type == PARTTYPE_CHAR)
		{
			part_char_t* pChar = (part_char_t*)m_Buffer[m_nShowPos + i];
			
			buf[count++] = pChar->nVisual;
			w += pChar->nWidth;
		}
		else if (type == PARTTYPE_IMAGE)
		{
			if (count > 0)
			{
				buf[count] = 0;
				GetPainter()->FillRect(x, y, x + w, y + h, back_color);
				GetPainter()->WriteTextLeft(x, y, x + w, y + h, 
					fore_color, buf);
				x += w;
				w = 0;
				count = 0;
			}
			
			part_image_t* pImage = (part_image_t*)m_Buffer[m_nShowPos + i];
			
			if (pImage->Image.IsReady())
			{
				x += pImage->nWidth;
			}
		}
	}
	
	if (count > 0)
	{
		buf[count] = 0;
		GetPainter()->FillRect(x, y, x + w, y + h, back_color);
		GetPainter()->WriteTextLeft(x, y, x + w, y + h, fore_color, buf);
	}
}

void Edit::Paint(float seconds)
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

	if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}
	
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	int x_start = GetVisTextStart();

	if (m_nShowCount > 0)
	{
		TAutoMem<wchar_t, 128, TCoreAlloc> auto_buf(m_nShowCount + 1);
		wchar_t* buf = auto_buf.GetBuffer();

		int count = 0;
		int x = x_start + m_nTextOffsetX;
		int w = 0;

		for (int i = 0; i < m_nShowCount; ++i)
		{
			int type = m_Buffer[m_nShowPos + i]->nType;
			
			if (type == PARTTYPE_CHAR)
			{
				part_char_t* pChar = (part_char_t*)m_Buffer[m_nShowPos + i];
				
				buf[count++] = pChar->nVisual;
				if( i + 1 == m_nShowCount)
				{
					w += pChar->nOffset;
					w += pChar->nWidth;
				}
				else
				{
					w += pChar->nAdvance;
					w += GetPainter()->GetTextInterval();
				}
			}
			else if (type == PARTTYPE_IMAGE)
			{
				if (count > 0)
				{
					buf[count] = 0;
					GetPainter()->WriteTextLeft(x, y1, 
						x2 - m_nTextOffsetX, y2, GetForeColor(), buf);
					x += w;
					w = 0;
					count = 0;
				}

				part_image_t* pImage = (part_image_t*)m_Buffer[m_nShowPos + i];
				
				if (pImage->Image.IsReady())
				{
					int h = GetPainter()->GetTextHeight(L" ");
					int y = y1 + ((y2 - y1) - h) / 2;
					
					pImage->Image.Draw(CGuiConst::DRAWMODE_FITWINDOW, x, y, 
						x + h, y + h);
					x += pImage->nWidth;
				}
			}
		}

		if (count > 0)
		{
			buf[count] = 0;
			GetPainter()->WriteTextLeft(x, y1,
				x2 - m_nTextOffsetX, y2, GetForeColor(), buf);
		}
	}

	if (HasSelectText())
	{
		PaintSelectText(x_start, y1);
	}

	if (GetFocused() && GetEnabled() && (!m_bReadOnly))
	{
		// 显示输入光标
		Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
		
		if (caret)
		{
			Assert(m_nInputPos >= m_nShowPos);
			
			int h = GetPainter()->GetTextHeight(L" ");
			int w = GetTotalWidth(m_Buffer, m_nShowPos, 
				m_nInputPos - m_nShowPos);
			int left = x_start + w + m_nTextOffsetX;
			int bottom = y1 + (GetHeight() - h) / 2 + h;
			int height = GetPainter()->GetTextHeight(L"_");
			int width = GetPainter()->GetTextWidth(L"_");

            m_CaretPos.x = left;
            m_CaretPos.y = bottom;
			caret->Draw(left, bottom, width, height, GetForeColor());
		}
	}
}

int Edit::GetTotalWidth(part_container_t& parts, int pos, int num)
{
	int width = 0;
	int end = pos + num;

	if (size_t(end) > parts.size())
	{
		end = (int)parts.size();
	}

	for (int i = pos; i < end; ++i)
	{
		part_base_t* part = parts[i];
		if( i+1 == end)
		{
			width += (part->nOffset + part->nWidth);
		}
		else
		{
			width += part->nAdvance;
		}
	}

	return width;
}

int Edit::GetVisTextStart()
{
	int x1 = InnerGetAbsLeft();
	
	switch (m_nAlign)
	{
	case CGuiConst::ALIGN_LEFT:
		break;
	case CGuiConst::ALIGN_CENTER:
		return x1 + (GetWidth() - m_nTextOffsetX * 2
			- GetTotalWidth(m_Buffer, m_nShowPos, m_nShowCount)) / 2;
	case CGuiConst::ALIGN_RIGHT:
		return x1 + (GetWidth() - m_nTextOffsetX * 2
			- GetTotalWidth(m_Buffer, m_nShowPos, m_nShowCount));
	default:
		Assert(0);
		break;
	}
	
	return x1;
}

int Edit::GetCursorInPos(int x, int y)
{
	int sx = x - GetVisTextStart();
	
	if (sx < 0)
	{
		return -1;
	}
	
	int w = 0;

	for (int i = 0; i < m_nShowCount; ++i)
	{
		part_base_t* part = m_Buffer[m_nShowPos + i];
		
		int cur_w = w + part->nOffset + part->nWidth;
			
		if (sx <= cur_w)
		{
			return i;
		}
		w += part->nAdvance;
	}

	return m_nShowCount + 1;
}

void Edit::ResetCaret()
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (caret)
	{
		caret->Reset();
	}
}

int Edit::InputKey(unsigned int ch)
{
	if (GuiUtil_ShiftPressed())
	{
		switch (ch)
		{
		case CGuiInput::KEY_LEFT:
			if (m_nInputPos == 0)
			{
				return 0;
			}
			
			IncSelect(m_nInputPos, -1);
			SetInputPos(m_nInputPos - 1);
			break;
		case CGuiInput::KEY_RIGHT:
			if (m_nInputPos == GetSize())
			{
				return 0;
			}

			IncSelect(m_nInputPos, 1);
			SetInputPos(m_nInputPos + 1);
			break;
		case CGuiInput::KEY_HOME:
			if (m_nInputPos == 0)
			{
				return 0;
			}

			IncSelect(m_nInputPos, -GetSize());
			SetInputPos(0);
			break;
		case CGuiInput::KEY_END:
			if (m_nInputPos == GetSize())
			{
				return 0;
			}
			
			IncSelect(m_nInputPos, GetSize());
			
			SetInputPos(GetSize());
			break;
		case CGuiInput::KEY_DELETE:
			if (!HasSelectText())
			{
				return 0;
			}

			if (!DeleteSelectText())
			{
				return 0;
			}

			break;
		case CGuiInput::KEY_INSERT:
			if (!Paste())
			{
				return 0;
			}

			break;
		default:
			return 0;
			break;
		}
	}
	else if (GuiUtil_ControlPressed())
	{
		switch (ch)
		{
		case 'C':	// copy
		case CGuiInput::KEY_INSERT:
			{
				Copy();
				return 0;
			}
			break;
		case 'V':	// paste
			if (!Paste())
			{
				return 0;
			}
			break;
		case 'X':	// cut
			if (!Cut())
			{
				return 0;
			}
			break;
		case 'A':	// select all
			{
				IncSelect(0, GetSize());
				SetInputPos(GetSize());
				return 1;
			}
			break;
		case 'Z':	// undo
			return 0;
		default:
			return 0;
		}
	}
	else
	{
		switch (ch)
		{
		case CGuiInput::KEY_LEFT:
			if (m_nInputPos == 0)
			{
				return 0;
			}
			
			SetInputPos(m_nInputPos - 1);
			break;
		case CGuiInput::KEY_RIGHT:
			if (m_nInputPos == GetSize())
			{
				return 0;
			}

			SetInputPos(m_nInputPos + 1);
			break;
		case CGuiInput::KEY_HOME:
			if (m_nInputPos == 0)
			{
				return 0;
			}

			SetInputPos(0);
			break;
		case CGuiInput::KEY_END:
			if (m_nInputPos == GetSize())
			{
				return 0;
			}
			
			SetInputPos(GetSize());
			break;
		case CGuiInput::KEY_BACK:
			if (HasSelectText())
			{
				if (DeleteSelectText())
				{
					break;
				}
			}
			
			if (m_nInputPos == 0)
			{
				return 0;
			}
			
			if (!DeleteText(m_nInputPos - 1, 1))
			{
				return 0;
			}
			
			SetInputPos(m_nInputPos - 1);
			break;
		case CGuiInput::KEY_DELETE:
			if (HasSelectText())
			{
				if (DeleteSelectText())
				{
					break;
				}
			}
			
			if (!DeleteText(m_nInputPos, 1))
			{
				return 0;
			}
			break;
		default:
			return 0;
		}
		
		ClearSelect();
	}

	return 1;
}

int Edit::InputChar(wchar_t ch)
{
	if (HasSelectText())
	{
		DeleteSelectText();
	}
	
	int count = InsertText(m_nInputPos, &ch, 1);

	if (0 == count)
	{
		return 0;
	}

	SetInputPos(m_nInputPos + count);
	
	if (!GuiUtil_ShiftPressed())
	{
		ClearSelect();
	}
	
	return 1;
}

// 事件

int Edit::OnWidthChanged(int old)
{
	CalcVisText();
	
	return 1;
}

int Edit::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	int width = GetTotalWidth(m_Buffer, m_nShowPos, m_nShowCount);
	int sx = x - InnerGetAbsLeft();

	if (sx > width)
	{
		if (HasSelectText())
		{
			ClearSelect();
		}
		else
		{
			IncSelect(0, GetSize());
		}

		SetInputPos(GetSize());
	}
	else
	{
		if (HasSelectText())
		{
			ClearSelect();
		}
		
		int pos = GetCursorInPos(x, y);

		if ((pos >= 0) && (pos < m_nShowCount))
		{
			SetInputPos(m_nShowPos + pos);
		}
	}

	if (m_bDragEvent)
	{
		m_nDragOldX = x;
		m_nDragOldY = y;
	}

	return 1; 
}

int Edit::OnLButtonUp(int x, int y)
{ 
	return 1; 
}

int Edit::OnDragMove(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}

	int pos = GetCursorInPos(x, y);

	if (pos < 0)
	{
		if (m_nInputPos > 0)
		{
			IncSelect(m_nInputPos, -1);
			SetInputPos(m_nInputPos - 1);
			CalcVisText();
		}
	}
	else if (pos > m_nShowCount)
	{
		if (m_nInputPos < GetSize())
		{
			IncSelect(m_nInputPos, 1);
			SetInputPos(m_nInputPos + 1);
			CalcVisText();
		}
	}
	else
	{
		IncSelect(m_nInputPos, m_nShowPos + pos - m_nInputPos);
		SetInputPos(m_nShowPos + pos);
		CalcVisText();
	}

	if (m_bDragEvent)
	{
		/*
		int x1 = InnerGetAbsLeft();
		int x2 = GetAbsRight();
		int y1 = InnerGetAbsTop();
		int y2 = GetAbsBottom();

		int drag_x = 0;
		int drag_y = 0;

		if (x > x2)
		{
			drag_x = x - x2;
		}
		else if (x < x1)
		{
			drag_x = x - x1;
		}

		if (y > y2)
		{
			drag_y = y - y2;
		}
		else if (y < y1)
		{
			drag_y = y - y1;
		}
		*/
		int drag_x = x - m_nDragOldX;
		int drag_y = y - m_nDragOldY;

		m_nDragOldX = x;
		m_nDragOldY = y;

		GuiUtil_RunCallback(this, "on_drag", 
			CVarList() << drag_x << drag_y);
	}
	
	return 1;
}

int Edit::OnKeyDown(unsigned int vk)
{
	if (vk == CGuiInput::KEY_TAB)
	{
		return 0;
	}
	
	if (vk == CGuiInput::KEY_RETURN)
	{
		return GuiUtil_RunCallback(this, "on_enter", CVarList());
	}
	
	if(InputKey(vk) == 1)
	{
		ResetEditInfo();
	}
		
	return 1;
}

int Edit::OnKeyUp(unsigned int vk)
{
    // 吃掉UP消息
	switch (vk)
	{
		case CGuiInput::KEY_CAPITAL:
		{
			if (m_bChangedEvent)
			{
				GuiUtil_RunCallback(this, "on_changed", CVarList() << GetID());
			}
		}
		break;
	}

    return 1;
}

//ch 为 unicode
int Edit::OnChar(unsigned int ch)
{
	if (m_bReadOnly)
	{
		return 0;
	}
	
	if (ch < 256)
	{
		unsigned int code_page = GetGui()->GetCodePage();

		if ((!isprint(ch)) && (!GuiUtil_IsLeadByteEx(code_page, ch)))
		{
			return 1;
		}
	}
	
	if (m_bOnlyDigit)
	{
		if (((ch < L'0') || (ch > L'9')) && (ch != L'-') && (ch != L'.'))
		{
			return 1;
		}
	}
	
	int res = 0;
    wchar_t wcsMultiByte[2] = {(wchar_t)ch, 0};
		
	if (!WideStrEmpty(wcsMultiByte))
	{
		res = InputChar(ch);
	}

	if (res == 1)
	{
		ResetCaret();

		CalcVisText();

		OnChanged(GetID());
	}

    if (m_bOnlyDigit && m_nMaxDigit != 0)
    {
        // 如果超出输入范围则自动设成最大值
        int nCurValue = GuiUtil_WideStrToInt(GetText());

        if (nCurValue > m_nMaxDigit)
        {
			result_wstring wcsValue = 
				GuiUtil_IntToWideStr(m_nMaxDigit).c_str();

            SetText(wcsValue.c_str());
        }
    }

	return 1;
}

int Edit::OnGetFocus(const PERSISTID& lost_id)
{
	ResetCaret();

	SetInputPos(GetSize());

	GuiUtil_RunCallback(this, "on_get_focus", CVarList() << lost_id);
	
	return IControl::OnGetFocus(lost_id);
}

int Edit::OnLostFocus(const PERSISTID& get_id)
{
	ClearSelect();

	GuiUtil_RunCallback(this, "on_lost_focus", CVarList() << get_id);
	
	return IControl::OnLostFocus(get_id);
}


int Edit::OnChanged(const PERSISTID& changed_id)
{
	if (!m_bChangedEvent)
	{
		return 0;
	}

	if (wcscmp(GetText(), GetOldText()) != 0)
	{
		GuiUtil_RunCallback(this, "on_changed", CVarList() << changed_id);
		m_wsOldText = m_wsText;
		return 1;
	}
	
	return 0;
}

bool Edit::GetPasswordStage() const
{
	return !m_wsPswdChar.empty();
}

bool Edit::ResetEditInfo()
{
	ResetCaret();
	CalcVisText();
	OnChanged(GetID());
	return true;
}
