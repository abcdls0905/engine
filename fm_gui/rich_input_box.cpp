//--------------------------------------------------------------------
// 文件名:		RichInputBox.h
// 内  容:		多格式文本输入框单行(HTML部分支持)
// 说  明:		
// 创建日期:	2008年9月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "rich_input_box.h"
#include "caret.h"
#include "gui_input.h"
#include "../tools/string_util.h"
#include "imageanimation_manager.h"
#include "hyperlinkstyle_manager.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../utils/language.h"
/// \file RichInputBox.cpp
/// \brief 文本输入框

/// entity: RichInputBox
/// \brief 文本输入框实体
/// \see 继承自IControl
DECLARE_ENTITY(RichInputBox, 0, IControl);

/// property: bool ReadOnly
/// \brief 是否只读
DECLARE_PROPERTY(bool, RichInputBox, ReadOnly, GetReadOnly, SetReadOnly);

/// property: bool EditMode
/// \brief 编辑器模式, 在编辑器模式下，拷贝粘贴没有<>格式限制
DECLARE_PROPERTY(bool, RichInputBox, EditMode, GetEditMode, SetEditMode);

/// property: bool OnlyDigit
/// \brief 是否只允许输入数字
DECLARE_PROPERTY(bool, RichInputBox, OnlyDigit, GetOnlyDigit, SetOnlyDigit);

/// property: bool SupportHtml
/// \brief 是否包函HTML标签
DECLARE_PROPERTY(bool, RichInputBox, SupportHtml, GetSupportHtml, SetSupportHtml);

/// property: bool DragEvent
/// \brief 是否产生拖动事件 
DECLARE_PROPERTY(bool, RichInputBox, DragEvent, GetDragEvent, SetDragEvent);

/// property: bool ChangedEvent
/// \brief 是否产生变化事件 
DECLARE_PROPERTY(bool, RichInputBox, ChangedEvent, GetChangedEvent, SetChangedEvent);

/// property: int MaxLength
/// \brief 文本的最大长度
DECLARE_PROPERTY(int, RichInputBox, MaxLength, GetMaxLength, SetMaxLength);
/// property: int TextOffsetX
/// \brief 文字起始位移
DECLARE_PROPERTY(int, RichInputBox, TextOffsetX, GetTextOffsetX, SetTextOffsetX);
/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(result_string, RichInputBox, Align, GetAlignString, SetAlignString);
/// property: wstring PasswordChar
/// \brief 作为密码输入时显示的字符
DECLARE_PROPERTY(const wchar_t*, RichInputBox, PasswordChar, 
				 GetPasswordChar, SetPasswordChar);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, RichInputBox, SelectForeColor, 
				 GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, RichInputBox, SelectBackColor, 
				 GetSelectBackColorString, SetSelectBackColorString);
/// property: string Caret
/// \brief 输入光标资源名
DECLARE_PROPERTY(const char*, RichInputBox, Caret, GetCaretName, SetCaretName);

/// property: bool Remember
/// \brief 是否记录输入
DECLARE_PROPERTY(bool, RichInputBox, Remember, GetRemember, SetRemember);

/// property: int MaxRemember
/// \brief 记录输入文本数量的最大值
DECLARE_PROPERTY(int, RichInputBox, MaxRemember, GetMaxRemember, SetMaxRemember)

/// property: bool ReturnFontFormat
/// \brief 是否返回字体格式，为false时格式中的字体部分将被忽略
DECLARE_PROPERTY(bool, RichInputBox, ReturnFontFormat, GetReturnFontFormat, SetReturnFontFormat)

#if 0
/// method: bool Cut()
/// \brief 剪切
DECLARE_METHOD_0(bool, RichInputBox, Cut);
/// method: bool Copy()
/// \brief 复制
DECLARE_METHOD_0(bool, RichInputBox, Copy);
/// method: bool Paste()
/// \brief 粘贴
DECLARE_METHOD_0(bool, RichInputBox, Paste);
/// method: bool Append(wstring text, int mode)
/// \brief 添加文字
/// \param text 文本
/// \param mode 0 添加在后,1添加在前 -1添加在当前光标位置,当前无光标则添加在最后
DECLARE_METHOD_2(bool, RichInputBox, Append, const wchar_t*, int);
#endif

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
DECLARE_EVENT(on_enter);

/// event: int on_space(object self)
/// \brief 输入空隔键的回调
/// \param self 本控件
DECLARE_EVENT(on_space);

/// event: int on_ctrl_keydown(object self, key)
/// \brief 按ctrl后的控制键回调
/// \param self 本控件
/// \param 按键名
DECLARE_EVENT(on_ctrl_keydown);

/// event: int on_shift_key_down(object self, key)
/// \brief 按shift后的控制键回调
/// \param self 本控件
/// \param 按键名
DECLARE_EVENT(on_shift_key_down);

// RichInputBox

RichInputBox::RichInputBox()
{
	m_bReadOnly = false;
    m_bEditMode = false;
	m_bOnlyDigit = false;
	m_bSupportHtml = true;
	m_bDragEvent = false;
    m_bChangedEvent = false;
    m_bReturnFontFormat = true;
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
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);
	
    m_bRemember = false;
    m_nMaxRemember = 10;
    m_nCurRemember = -1;

    m_CaretPos.x = 0;
    m_CaretPos.y = 0;

	SetDesignSize(80, 20);
	SetCanFocus(true);
	SetTabStop(true);
}

RichInputBox::~RichInputBox()
{
	ReleaseAll();
}

void RichInputBox::ReleasePart(HtmlItem* part)
{
	int type = part->m_Type;
	
	if (type == PARTTYPE_CHAR)
	{
		CORE_DELETE((HtmlTextItem*)part);
	}
	else if (type == PARTTYPE_IMAGE)
	{
		CORE_DELETE((HtmlImageItem*)part);
	}
}

void RichInputBox::ReleaseAll()
{
	for (size_t i = 0; i < m_Buffer.size(); ++i)
	{
		ReleasePart(m_Buffer[i]);
	}
	
	m_Buffer.clear();
}

bool RichInputBox::Init(const IVarList& args)
{
    m_Html.SetDefColor(GetForeColor());
    m_Html.SetDefFont(GuiUtil_GetValidFontName(this));

	if (!IControl::Init(args))
	{
		return false;
	}
	
	SetCaretName("Default");
	SetCursorName("WIN_IBEAM");

	return true;
}

bool RichInputBox::Shut()
{
	return IControl::Shut();
}

void RichInputBox::SetReadOnly(bool value) 
{ 
	m_bReadOnly = value; 
}

bool RichInputBox::GetReadOnly() const 
{ 
	return m_bReadOnly; 
}

// 编辑器模式
void RichInputBox::SetEditMode(bool value)
{
    m_bEditMode = value;
}

bool RichInputBox::GetEditMode() const
{
    return m_bEditMode;
}

void RichInputBox::SetOnlyDigit(bool value)
{
	m_bOnlyDigit = value;
}

bool RichInputBox::GetOnlyDigit() const
{
	return m_bOnlyDigit;
}

void RichInputBox::SetSupportHtml(bool value)
{
	m_bSupportHtml = value;
	
	SetText(GetText());
}

bool RichInputBox::GetSupportHtml() const
{
	return m_bSupportHtml;
}

void RichInputBox::SetDragEvent(bool value)
{
	m_bDragEvent = value;
}

bool RichInputBox::GetDragEvent() const
{
	return m_bDragEvent;
}

// 是否产生变化事件
void RichInputBox::SetChangedEvent(bool value)
{
    m_bChangedEvent = value;
}

bool RichInputBox::GetChangedEvent() const
{
    return m_bChangedEvent;
}


void RichInputBox::SetMaxLength(int value) 
{ 
	if (value > GetMaxSize())
	{
		return;
	}

	m_nMaxLength = value; 
}
 
int RichInputBox::GetMaxLength() const 
{ 
	return m_nMaxLength; 
}

void RichInputBox::SetTextOffsetX(int value)
{
	if (value < 0)
	{
		return;
	}
	
	m_nTextOffsetX = value;
}

int RichInputBox::GetTextOffsetX() const
{
	return m_nTextOffsetX;
}

void RichInputBox::SetAlignString(const char* value)
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

result_string RichInputBox::GetAlignString() const
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

void RichInputBox::SetPasswordChar(const wchar_t* value)
{
	Assert(value != NULL);
	
	m_wsPswdChar = value;
}

const wchar_t* RichInputBox::GetPasswordChar() const
{
	return m_wsPswdChar.c_str();
}

void RichInputBox::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string RichInputBox::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName().c_str();
}

void RichInputBox::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string RichInputBox::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

// 记录输入
void RichInputBox::SetRemember(bool bRemember)
{
    m_bRemember = bRemember;
}

bool RichInputBox::GetRemember() const
{
    return m_bRemember;
}

// 设置记录输入文本最大条数
void RichInputBox::SetMaxRemember(int max)
{
    m_nMaxRemember = max;
}

int RichInputBox::GetMaxRemember() const
{
    return m_nMaxRemember;
}

// 记录输入
void RichInputBox::SetReturnFontFormat(bool bValue)
{
    m_bReturnFontFormat = bValue;
}

bool RichInputBox::GetReturnFontFormat()
{
    return m_bReturnFontFormat;
}

void RichInputBox::SetText(const wchar_t* val)
{
	Assert(val != NULL);
	
	ReleaseAll();
	
	AppendText(-1, val, wcslen(val));
	
	m_wsText = val;
	m_nInputPos = 0;
	m_nShowPos = 0;
	m_nShowCount = 0;
	
	ClearSelect();
	
	CalcVisText();
}

const wchar_t* RichInputBox::GetText() const
{
	return m_wsText.c_str();
}

point_t RichInputBox::GetCaretPos()
{
    return m_CaretPos;
}

void RichInputBox::SetCaretName(const char* name)
{
	Assert(name != NULL);
	
	if (!SetCaretID(name))
	{
		return;
	}
	
	m_strCaretName = name;
}

const char* RichInputBox::GetCaretName() const
{
	return m_strCaretName.c_str();
}

bool RichInputBox::SetCaretID(const char* name)
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

int RichInputBox::GetSize() const
{
	return m_Buffer.size();
}

int RichInputBox::GetMaxSize() const
{
	return 0x10000;
}

int RichInputBox::AppendText(int pos, const wchar_t* ws, int size)
{
	Assert(ws != NULL);
	Assert(size >= 0);

	wchar_t pswd_char = 0;
	
	if (!m_wsPswdChar.empty())
	{
		pswd_char = m_wsPswdChar[0];
	}

    int count = 0;

    m_Html.SetText(ws, GetForeColor(), GuiUtil_GetValidFontName(this), 12);

    std::vector< TextSegment >*pSegments = m_Html.GetSegments();

	for (size_t i = 0; i < pSegments->size(); ++i)
	{
		if (m_nMaxLength > 0)
		{
			// 限制最大长度
			if (m_Buffer.size() >= size_t(m_nMaxLength))
			{
				break;
			}
		}

        TextSegment &segment = (*pSegments)[i];

        size_t iText = 0;
        size_t iImage = 0;

        for (size_t j = 0; j < segment.m_processlist.size(); ++j)
        {
            if (segment.m_processlist[j])
            {
                // 文本
                HtmlTextItem &textItem = segment.m_TextItemList[iText];

                textItem.m_Type = PARTTYPE_CHAR;

                // 设置字体以获得正确的宽度
                GetGui()->SetFont(textItem.m_Face.c_str(), GetShadowColor());

                if (textItem.m_IsHyperLink || textItem.m_wtext.size() == 1)
                {
                    // 如果是超链接,或者只有一个字母时
                    textItem.m_Width = GetPainter()->GetTextLenWidth(
						textItem.m_wtext.c_str(), textItem.m_wtext.size());

                    HtmlTextItem* pTextItem = CORE_NEW(HtmlTextItem);

                    *pTextItem = textItem;

                    if (pos < 0)
                    {
                        m_Buffer.push_back(pTextItem);
                    }
                    else
                    {
                        m_Buffer.insert(m_Buffer.begin() + pos, pTextItem);

                        pos++;
                    }
                    count ++;
                }
                else
                {
                    // 如果不是超链接,且有多个文字

                    // 设置字体以获得正确的宽度
                    GetGui()->SetFont(textItem.m_Face.c_str(), GetShadowColor());

                    for (size_t t = 0; t < textItem.m_wtext.size();)
                    {
                        HtmlTextItem* pTextItem = CORE_NEW(HtmlTextItem);

                        if (pTextItem)
                        {
                            int size = GetPainter()->GetNextLanguageCharacter(textItem.m_wtext.c_str(), t);

							if (size == 0)
							{
								CORE_DELETE(pTextItem);
								return 0;
							}

                            *pTextItem = textItem;
                            pTextItem->m_wtext = textItem.m_wtext.substr(t, size);
                            pTextItem->m_Width = GetPainter()->GetTextLenWidth(
								pTextItem->m_wtext.c_str(), 
								pTextItem->m_wtext.size());

                            if (pos < 0)
                            {
                                m_Buffer.push_back(pTextItem);
                            }
                            else
                            {
                                m_Buffer.insert(m_Buffer.begin() + pos, pTextItem);

                                pos++;
                            }

                            t += size;
                            count ++;
                        }

                    }
                }

                iText++;
            }
            else
            {
                // 图片
                HtmlImageItem& imageItem = segment.m_ImageItemList[iImage];

                imageItem.m_Type = PARTTYPE_IMAGE;
                imageItem.m_Width = 12;

                if (imageItem.m_imageID.IsNull())
                {
                    ImageAnimationManager* pAniManager = InnerGetAnimation();
                    
					if (pAniManager)
                    {
                        std::string szImageName = StringUtil::WideStrAsString(
							imageItem.m_imagesrc.c_str());

                        if (pAniManager->IsAnimation(szImageName.c_str()))
                        {
                            imageItem.m_bIsAnimation = true;	

                            point_t pt = pAniManager->GetAnimationSize(
								szImageName.c_str());

                            imageItem.m_Width = pt.x;
                        }
                    }
                }
                else
                {
                    imageItem.m_Width = GetPainter()->GetGlyphWidth(
						imageItem.m_imageID);
                }

                HtmlImageItem* pImageItem = CORE_NEW(HtmlImageItem);

                if (!pImageItem)
				{
					return 0;
				}

               *pImageItem = imageItem;

                if (pos < 0)
                {
                    m_Buffer.push_back(pImageItem);
                }
                else
                {
                    m_Buffer.insert(m_Buffer.begin() + pos, pImageItem);
                    pos++;
                }

                imageItem.m_bNeedRelease = false;

                iImage++;
                count ++;
            }
        }
	}

	return count;
}

void RichInputBox::GenerateText(PART_CONTAINER& parts, int pos, int num, 
								std::wstring& ws)
{
	ws = L"";
	
	int end = pos + num;

	if (size_t(end) > parts.size())
	{
		end = parts.size();
	}

    std::string szCurFont = GuiUtil_GetValidFontName(this);
    dword dwCurColor = GetForeColor();
 
    HtmlTextItem textitem;

	for (int i = pos; i < end; ++i)
	{
        int type = parts[i]->m_Type;

        if (type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)parts[i];

            if (pTextItem)
            {
                if (pTextItem->m_IsHyperLink)
                {
                    if (textitem.m_wtext.size() > 0)
                    {
                        std::wstring buffer = L"";

                        if (m_bSupportHtml 
							&& m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                        {
                            ws += buffer;
                        }
                        else
                        {
                            ws += textitem.m_wtext;
                        }

                        textitem.m_Face = szCurFont;
                        textitem.m_Color = dwCurColor;
                        textitem.m_wtext = L"";
                    }

                    std::wstring buffer = L"";

                    if (m_bSupportHtml 
						&& m_Html.RestoreToHtml(*(HtmlTextItem*)parts[i], buffer, m_bReturnFontFormat))
                    {
                        ws += buffer;
                    }
                    else
                    {
                        ws += ((HtmlTextItem*)parts[i])->m_wtext;
                    }
                }
                else if (pTextItem->m_Face != textitem.m_Face 
					|| pTextItem->m_Color != textitem.m_Color)
                {
                    if (textitem.m_wtext.size() > 0)
                    {
                        std::wstring buffer = L"";

                        if (m_bSupportHtml 
							&& m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                        {
                            ws += buffer;
                        }
                        else
                        {
                            ws += textitem.m_wtext;
                        }
                    }

                    textitem.m_Face = pTextItem->m_Face;
                    textitem.m_Color = pTextItem->m_Color;
                    textitem.m_wtext = pTextItem->m_wtext;
                }
                else
                {
                    textitem.m_wtext += pTextItem->m_wtext;
                }
            }

        }
        else if (type == PARTTYPE_IMAGE)
        {
            if (textitem.m_wtext.size() > 0)
            {
                std::wstring buffer = L"";

                if (m_bSupportHtml && m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                {
                    ws += buffer;
                }
                else
                {
                    ws += textitem.m_wtext;
                }

                textitem.m_Face = szCurFont;
                textitem.m_Color = dwCurColor;
                textitem.m_wtext = L"";
            }

            std::wstring buffer = L"";

            if (m_bSupportHtml 
				&& m_Html.RestoreToHtml(*(HtmlImageItem*)parts[i], buffer))
            {
                ws += buffer;
            }
        }
	}

    if (textitem.m_wtext.size() > 0)
    {
        std::wstring buffer = L"";

        if (m_bSupportHtml && m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
        {
            ws += buffer;
        }
        else
        {
            ws += textitem.m_wtext;
        }
    }
}

int RichInputBox::InsertText(int pos, const wchar_t* ws, int size)
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
		// 同步
		GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);
	}
	
	return count;
}

bool RichInputBox::DeleteText(int pos, int num)
{
	Assert( (pos >= 0) && (pos <= GetSize()) );
	
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
	
	m_Buffer.erase(m_Buffer.begin() + pos, m_Buffer.begin() + pos + num);
	
	if (num > 0)
	{
		// 同步
		GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);
	}
	
	return true;
}

std::wstring RichInputBox::GetSelectText()
{
	if (!HasSelectText())
	{
		return std::wstring();
	}
	
	int beg = GetSelectBeg();
	int end = GetSelectEnd();
	
	std::wstring ws;

	GenerateText(m_Buffer, beg, end - beg, ws);

	return ws;
}

bool RichInputBox::HasSelectText() const
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

void RichInputBox::ClearSelect()
{
	m_nSelectBeg = 0;
	m_nSelectEnd = 0;
}

bool RichInputBox::Cut()
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

bool RichInputBox::Copy()
{
	std::wstring ws = GetSelectText();
	
	if (ws.empty())
	{
		return false;
	}
	
	return GuiUtil_CopyToClipboard(ws.c_str());
}

bool RichInputBox::Paste()
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
		int remain_length = m_nMaxLength - m_Buffer.size();

		if (remain_length <= 0)
		{
			return false;
		}

		if (remain_length < (int)ws.length())
		{
			ws = ws.substr(0, remain_length);
		}
	}

    int count = 0;

    if (!m_bEditMode)
    {
        size_t begin = 0;

        size_t pos_begin = ws.find(L"<");
        size_t pos_end = ws.find(L">");

        while (pos_begin != std::wstring::npos && pos_end != std::wstring::npos && pos_end > pos_begin)
        {
            if (pos_begin > begin)
            {
                std::wstring insert_text = ws.substr(begin, pos_begin - begin).c_str();

                count = InsertText(m_nInputPos, insert_text.c_str(), insert_text.length());

                if (count > 0)
                {
                    SetInputPos(m_nInputPos + count);
                }
            }

            begin = pos_end + 1;

            pos_begin = ws.find(L"<", begin);
            pos_end = ws.find(L">", begin);
        }

        std::wstring insert_text = ws.substr(begin, ws.length() - begin).c_str();

        count = InsertText(m_nInputPos, insert_text.c_str(), insert_text.length());
    }
    else
    {
        count = InsertText(m_nInputPos, ws.c_str(), ws.length());
    }

	if (0 == count)
	{
        CalcVisText();
		return false;
	}
	
	SetInputPos(m_nInputPos + count);
    CalcVisText();
	
	return true;
}

bool RichInputBox::Append(const wchar_t* text,int mode)
{
    int pos = 0;
    if (mode == 1)
    {
        pos = GetSize();
    }
    else
    {
        if (m_nInputPos >= 0 && m_nInputPos <= GetSize())
        {
            pos = m_nInputPos;
        }
        else
        {
            pos = GetSize();
        }
    }
	int count = InsertText(pos, text, wcslen(text));
	
	SetInputPos(m_nInputPos + count);

    CalcVisText();
	return true;
}

int RichInputBox::GetSelectBeg() const
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

int RichInputBox::GetSelectEnd() const
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

bool RichInputBox::IncSelect(int pos, int num)
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


bool RichInputBox::DeleteLeftText(int pos)
{
    Assert( (pos >= 0) && (pos <= GetSize()) );

    if (m_bReadOnly)
    {
        return false;
    }

    // 检查是否是文字item
    if (m_Buffer[pos]->m_Type == PARTTYPE_CHAR)
    {
        HtmlTextItem *pItem = (HtmlTextItem *)m_Buffer[pos];

        // 检查此文字Item是否为超链接
        if (!pItem->m_IsHyperLink)
        {
            if (pItem->m_wtext.size() > 1)
            {
                pItem->m_wtext.resize(pItem->m_wtext.size() - 1);

                GetGui()->SetFont(pItem->m_Face.c_str(), GetShadowColor());

                pItem->m_Width = GetPainter()->
                    GetTextLenWidth(pItem->m_wtext.c_str(), 
                    pItem->m_wtext.size());
                return false;
            }
        }
    }


    ReleasePart(m_Buffer[pos]);

    m_Buffer.erase(m_Buffer.begin() + pos, m_Buffer.begin() + pos + 1);

    // 同步
    GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);

    return true;

}

bool RichInputBox::DeleteSelectText()
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

int RichInputBox::NormalizePos(int pos) const
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

void RichInputBox::SetShowPos(int pos)
{
	Assert( (pos >= 0) && (pos <= GetSize()) );

	if (m_nShowPos != pos)
	{
		m_nShowPos = pos;

		CalcVisText();
	}
}

int RichInputBox::GetVisualCount(int pos, int width)
{
	int w = 0;
	
	for (size_t i = pos; i < m_Buffer.size(); ++i)
	{
		w += m_Buffer[i]->m_Width;

		if (w > width)
		{
			return (i - pos);
		}
	}
	
	return (m_Buffer.size() - pos);
}

void RichInputBox::SetInputPos(int pos)
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

int RichInputBox::GetCaretWidth() const
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (NULL == caret)
	{
		return 0;
	}
	
	return caret->GetFixWidth();
}

int RichInputBox::GetShowWidth() const
{
	return GetWidth() - GetCaretWidth() - m_nTextOffsetX * 2;
}

void RichInputBox::CalcVisText()
{
	Assert( (m_nShowPos >= 0) && (m_nShowPos <= GetSize()) );
	
	m_nShowCount = GetVisualCount(m_nShowPos, GetShowWidth());
}

void RichInputBox::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	CVar varDefault;
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	
	di->AddPropInfo("ReadOnly", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("EditMode", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
	di->AddPropInfo("OnlyDigit", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("SupportHtml", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("ReturnFontFormat", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("DragEvent", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("ChangedEvent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("MaxLength", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, 
		true, true,	&(varDefault.SetString("Left"),varDefault) );
	di->AddPropInfo("PasswordChar", IDesignInfo::TYPE_WIDESTR, true, true);
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true,
		&(varDefault.SetString(GuiUtil_IntToColor(
		CGuiConst::SELECT_FORE_COLOR).c_str()),varDefault) );
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true,
		&(varDefault.SetString(GuiUtil_IntToColor(
		CGuiConst::SELECT_BACK_COLOR).c_str()),varDefault) );
	di->AddPropInfo("Caret", IDesignInfo::TYPE_STRING, true, true);
	
    di->AddPropInfo("Remember", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("MaxRemember", IDesignInfo::TYPE_INT, true, true, 
		&(varDefault.SetInt(10),varDefault));

	di->AddEventInfo("on_get_focus");
	di->AddEventInfo("on_lost_focus");
	di->AddEventInfo("on_drag");
	di->AddEventInfo("on_changed");
	di->AddEventInfo("on_enter");
    di->AddEventInfo("on_space");
	di->AddEventInfo("on_ctrl_keydown");
	di->AddEventInfo("on_shift_key_down");
}

void RichInputBox::PaintSelectText(int x1, int y1)
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
		w_start += m_Buffer[m_nShowPos + k]->m_Width;
	}
	
	unsigned int back_color = m_SelectBackColor.GetValue();
	unsigned int fore_color = m_SelectForeColor.GetValue();
	
	TAutoMem<wchar_t, 128> auto_buf(m_nShowCount + 1);
	
	wchar_t* buf = auto_buf.GetBuffer();
	
	int count = 0;
	
	int h = GetPainter()->GetTextHeight(L" ");
	int w = 0;
	
	int x = x1 + w_start + m_nTextOffsetX;
	int y = y1 + (GetHeight() - h) / 2;
	
	for (int i = pos; i < (pos + num); ++i)
	{
		int type = m_Buffer[m_nShowPos + i]->m_Type;
		
		if (type == PARTTYPE_CHAR)
		{
			HtmlTextItem& itemChar = 
				*((HtmlTextItem*)m_Buffer[m_nShowPos + i]);
			
            for(size_t c = 0; c < itemChar.m_wtext.size(); ++c)
            {
			    buf[count++] = itemChar.m_wtext[c];
            }

			w += itemChar.m_Width;
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
			
			HtmlImageItem& itemImage = 
				*((HtmlImageItem*)m_Buffer[m_nShowPos + i]);
			
			x += itemImage.m_Width;
		}
	}
	
	if (count > 0)
	{
		buf[count] = 0;
		
		GetPainter()->FillRect(x, y, x + w, y + h, back_color);
		GetPainter()->WriteTextLeft(x, y, x + w, y + h, fore_color, buf);
	}
}

void RichInputBox::Paint(float seconds)
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
	
    std::string szDefaultFont = GuiUtil_GetValidFontName(this);

	GetGui()->SetFont(szDefaultFont.c_str(), GetShadowColor());
	
	int x_start = GetVisTextStart();

	if (m_nShowCount > 0)
	{
		TAutoMem<wchar_t, 128> auto_buf(m_nShowCount + 1);
		
		wchar_t* buf = auto_buf.GetBuffer();

		int count = 0;
		
		int x = x_start + m_nTextOffsetX;

		int w = 0;

		for (int i = 0; i < m_nShowCount; ++i)
		{
			int type = m_Buffer[m_nShowPos + i]->m_Type;
			
			if (type == PARTTYPE_CHAR)
			{
				HtmlTextItem* pChar = (HtmlTextItem*)m_Buffer[m_nShowPos + i];
				
                for(size_t c = 0; c < pChar->m_wtext.size(); ++c)
                {
				    buf[count++] = pChar->m_wtext[c];
                }

                if (pChar->m_IsHyperLink || pChar->m_Color != GetForeColor() 
					|| stricmp(pChar->m_Face.c_str(), szDefaultFont.c_str()) != 0)
                {
                    // 如果之前存在则绘制
                    if (count > 0)
                    {
                        buf[count] = 0;

                        GetPainter()->WriteTextLeft(x, y1, 
                            x2 - m_nTextOffsetX, y2, GetForeColor(), buf);

                        x += w;
                        w = 0;
                        count = 0;
                    }

                    if (pChar->m_IsHyperLink)
                    {
                        HyperLinkStyleManager* pHLinkManager = 
							InnerGetHyperlinkMng();

                        if (!pHLinkManager)
                        {
                            return;
                        }

                        StyleElement* pStyleElement = NULL;

                        if (pHLinkManager->GetStyleByState(
							pChar->m_szHPStyle.c_str(), HYPERLINK_NORMAL, 
							pStyleElement))
                        {
                            pChar->m_Color = pStyleElement->color;
                            pChar->m_Face = pStyleElement->szFont;
                            pChar->m_FontSize = pStyleElement->size;
                        }
                    }
                    
                    GetPainter()->WriteTextLeft(x, y1, x2 - m_nTextOffsetX, 
						y2, pChar->m_Color, pChar->m_wtext.c_str());

                    x += pChar->m_Width;
                }
				else
                {
                    w += pChar->m_Width;
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

				HtmlImageItem* pImage = 
					(HtmlImageItem*)m_Buffer[m_nShowPos + i];
                
                int h = GetPainter()->GetTextHeight(L" ");

                int y = y1 + ((y2 - y1) - h) / 2;

                if (!pImage->m_imageID.IsNull())
                {
                    rect_t rect = { 0, 0, 
						GetPainter()->GetGlyphWidth(pImage->m_imageID),
						GetPainter()->GetGlyphHeight(pImage->m_imageID)
					};

                    GetPainter()->DrawGlyph(x, y + h - rect.bottom, 
						x + pImage->m_Width, y + h, rect.left, rect.top,
						rect.right, rect.bottom, pImage->m_imageID);
                }
                else
                {
                    ImageAnimationManager* pAniManager = InnerGetAnimation();
                    
					if (pAniManager)
                    {
                        std::string szValue = StringUtil::WideStrAsString(
							pImage->m_imagesrc.c_str());

                        point_t pt = pAniManager->GetAnimationSize(
							szValue.c_str());

                        rect_t rect = { 0, 0, pt.x, pt.y};

                        pAniManager->DrawAnimation(szValue.c_str(), x, 
							y + h - rect.bottom, rect);
                    }
                }

                x += pImage->m_Width;
			}
		}

		if (count > 0)
		{
			buf[count] = 0;

			GetPainter()->WriteTextLeft(x, y1, x2 - m_nTextOffsetX, y2, 
				GetForeColor(), buf);
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

int RichInputBox::GetTotalWidth(PART_CONTAINER& parts, int pos, int num)
{
	int width = 0;

	int end = pos + num;

	if (size_t(end) > parts.size())
	{
		end = parts.size();
	}

	for (int i = pos; i < end; ++i)
	{
		width += parts[i]->m_Width;
	}

	return width;
}

int RichInputBox::GetVisTextStart()
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
		return x1 + GetTotalWidth(m_Buffer, m_nShowPos, m_nShowCount);
	default:
		Assert(0);
		break;
	}
	
	return x1;
}

int RichInputBox::GetCursorInPos(int x, int y)
{
	int sx = x - GetVisTextStart();
	
	if (sx < 0)
	{
		return -1;
	}
	
	int w = 0;

	for (int i = 0; i < m_nShowCount; ++i)
	{
		w += m_Buffer[m_nShowPos + i]->m_Width;
		
		if (sx <= w)
		{
			return i;
		}
	}

	return m_nShowCount + 1;
}

void RichInputBox::ResetCaret()
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (caret)
	{
		caret->Reset();
	}
}

int RichInputBox::InputKey(unsigned int ch)
{
	if (GuiUtil_ShiftPressed())
	{
		GuiUtil_RunCallback(this, "on_shift_key_down", CVarList()<<ch);

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
		GuiUtil_RunCallback(this, "on_ctrl_keydown", CVarList()<<ch);

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
			
			if (!DeleteLeftText(m_nInputPos - 1))
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


// 尝试越南文组合字符输入
bool RichInputBox::TryVietnamesInput(wchar_t ch)
{
    // 如果为越南文音调符，则检查当前位置的前面一个字符，如果是越南文单元单则添加，否则不处理
    if (FxLanguage::IsVietnameseVoiceLevel(ch))
    {
        // 如果当前输入框为空，则不处理
        if (m_Buffer.size() == 0)
        {
            return true;
        }

        int pos = m_nInputPos;

        if (pos > 0 && pos <= (int)m_Buffer.size())
        {
            HtmlItem* pHtmlItem = m_Buffer[pos-1];

            if (pHtmlItem == NULL)
            {
                return true;
            }

            // 如果之前的并不是文字输入则不处理
            if (pHtmlItem->m_Type != PARTTYPE_CHAR)
            {
                return true;
            }

            HtmlTextItem* pTextItem = (HtmlTextItem*)pHtmlItem;

            // 输入为空，或为超链接时直接返回
            if (pTextItem->m_wtext.size() == 0 || 
                pTextItem->m_IsHyperLink)
            {
                return true;
            }

            // 获取最后一个字符
            wchar_t end_ch = pTextItem->m_wtext[pTextItem->m_wtext.size() - 1];

            // 检查最后一个字符是否为单元音，是则进行注音组合
            if (FxLanguage::IsVietnameseSYan(end_ch))
            {
                pTextItem->m_wtext.resize(pTextItem->m_wtext.size() + 1);
                pTextItem->m_wtext[pTextItem->m_wtext.size() - 1] = ch;

                GetGui()->SetFont(pTextItem->m_Face.c_str(), GetShadowColor());

                pTextItem->m_Width = GetPainter()->GetTextLenWidth(
                    pTextItem->m_wtext.c_str(), pTextItem->m_wtext.size());
                // 这样应该不需调整显示所以返回0
                // 同步
                GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);

                if (!GuiUtil_ShiftPressed())
                {
                    ClearSelect();
                }

                return true;
            }
        }
    }

    return false;
}

// 尝试越南文组合字符输入
bool RichInputBox::TryThaiInput(wchar_t ch)
{
    int compose_flag = 0;
    // 如果为泰文第一类合成字符，则检查当前位置的前面一个字符，如果是CONS辅音则添加，否则不处理
    if (FxLanguage::IsThaiCompose1(ch))
    {
        compose_flag = 1;
    }
    // 如果为泰文第二类合成字符，则检查当前位置的前面一个字符，如果是AV/BV则添加，否则不处理
    else if (FxLanguage::IsThaiCompose2(ch))
    {
        compose_flag = 2;
    }
    else
    {
        return false;
    }

    // 如果当前输入框为空，则不处理
    if (m_Buffer.size() == 0)
    {
        return true;
    }

    int pos = m_nInputPos;

    if (pos > 0 && pos <= (int)m_Buffer.size())
    {
        HtmlItem* pHtmlItem = m_Buffer[pos-1];

        if (pHtmlItem == NULL)
        {
            return true;
        }

        // 如果之前的并不是文字输入则不处理
        if (pHtmlItem->m_Type != PARTTYPE_CHAR)
        {
            return true;
        }

        HtmlTextItem* pTextItem = (HtmlTextItem*)pHtmlItem;

        // 输入为空，或为超链接时直接返回
        if (pTextItem->m_wtext.size() == 0 || 
            pTextItem->m_IsHyperLink)
        {
            return true;
        }

        // 获取最后一个字符
        wchar_t end_ch = pTextItem->m_wtext[pTextItem->m_wtext.size() - 1];

        // 如果为泰文第一类合成字符，则检查当前位置的前面一个字符，如果是CONS辅音则添加，否则不处理
        if ((compose_flag == 1 && FxLanguage::IsThaiCONS(end_ch)) ||
            // 如果为泰文第二类合成字符，则检查当前位置的前面一个字符，如果是AV/BV则添加，否则不处理
            (compose_flag == 2 &&
            (   FxLanguage::IsThaiAV1(end_ch) || 
            FxLanguage::IsThaiAV2(end_ch) ||
            FxLanguage::IsThaiAV3(end_ch) ||
            FxLanguage::IsThaiBV1(end_ch) ||
            FxLanguage::IsThaiBV2(end_ch))))
        {
            pTextItem->m_wtext.resize(pTextItem->m_wtext.size() + 1);
            pTextItem->m_wtext[pTextItem->m_wtext.size() - 1] = ch;

            GetGui()->SetFont(pTextItem->m_Face.c_str(), GetShadowColor());

            pTextItem->m_Width = GetPainter()->GetTextLenWidth(
                pTextItem->m_wtext.c_str(), pTextItem->m_wtext.size());

            // 这样应该不需调整显示所以返回0
            // 同步
            GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);

            if (!GuiUtil_ShiftPressed())
            {
                ClearSelect();
            }

            return true;
        }
        // 如果为泰文第一类合成字符，如果前面的字符也是第一类合成字符，则将其替换
        else if ((compose_flag == 1 && FxLanguage::IsThaiCompose1(end_ch)) ||
            // 如果为泰文第二类合成字符，如果前面的字符也是第二类合成字符，则将其替换
            (compose_flag == 2 && FxLanguage::IsThaiCompose2(end_ch)))
        {
            pTextItem->m_wtext[pTextItem->m_wtext.size() - 1] = ch;

            GetGui()->SetFont(pTextItem->m_Face.c_str(), GetShadowColor());

            pTextItem->m_Width = GetPainter()->GetTextLenWidth(
                pTextItem->m_wtext.c_str(), pTextItem->m_wtext.size());
            // 这样应该不需调整显示所以返回0
            // 同步
            GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);

            if (!GuiUtil_ShiftPressed())
            {
                ClearSelect();
            }
            return true;
        }
    }

    return true;
}

int RichInputBox::InputChar(wchar_t ch)
{
	if (HasSelectText())
	{
		DeleteSelectText();
	}
	

    // 尝试越南文和泰文的组合字符输入，如果确实是这些组合字符，
    // 则尝试组合，即使组合失败也无需再处理，
    // 如果不是组合字符则照带处理即可
    if (TryVietnamesInput(ch) || TryThaiInput(ch))
    {
        return 1;
    }
    else
    {
        wchar_t buff[2];
        buff[0] = ch;
        buff[1] = 0;
	    int count = InsertText(m_nInputPos, buff, 1);

	    if (0 == count)
	    {
		    return 0;
	    }

	    SetInputPos(m_nInputPos + count);
    }
	
	if (!GuiUtil_ShiftPressed())
	{
		ClearSelect();
	}
	
	return 1;
}

// 事件

int RichInputBox::OnWidthChanged(int old)
{
	CalcVisText();
	
	return 1;
}

int RichInputBox::OnLButtonDown(int x, int y)
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

int RichInputBox::OnLButtonUp(int x, int y)
{ 
	return 1; 
}

int RichInputBox::OnDragMove(int x, int y)
{
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

int RichInputBox::OnKeyDown(unsigned int vk)
{
	if (vk == CGuiInput::KEY_TAB)
	{
		return 0;
	}
	
	if (vk == CGuiInput::KEY_RETURN)
	{
        if (m_bRemember)
        {
            if (m_vRemembers.size() == 0 || m_vRemembers.back() != m_wsText)
            {
                //已记录的最后一个文本和当前输入文本不一致时才能记录
                m_vRemembers.push_back(m_wsText);

                if ((int)m_vRemembers.size() > m_nMaxRemember)
                {
                    m_vRemembers.erase(m_vRemembers.begin());
                }
            }

            m_nCurRemember = -1;

        }

		return GuiUtil_RunCallback(this, "on_enter", CVarList());
	}

    if (vk == CGuiInput::KEY_UP)
    {
        if (m_bRemember)
        {
            if (m_vRemembers.size() > 0)
            {
                if (m_nCurRemember == -1)
                {
                    m_nCurRemember = m_vRemembers.size();
                }
            }

            if (m_nCurRemember != -1 && m_nCurRemember > 0)
            {
                m_nCurRemember--;

                SetText(m_vRemembers[m_nCurRemember].c_str());
            }
        }
    }
    else if (vk == CGuiInput::KEY_DOWN)
    {
        if (m_bRemember)
        {
            if (m_nCurRemember != -1 
				&& m_nCurRemember < (int)(m_vRemembers.size() - 1))
            {
                m_nCurRemember++;

                SetText(m_vRemembers[m_nCurRemember].c_str());
            }
        }
    }
    else
    {
        if (m_bRemember)
        {
            m_nCurRemember = -1;
        }
    }

	int res = InputKey(vk);
	
	if (res == 1)
	{
		ResetCaret();
		CalcVisText();

        OnChanged(GetID());
	}
	
	return 1;
}

int RichInputBox::OnKeyUp(unsigned int vk)
{
    // 吃掉UP消息
    return 1;
}

int RichInputBox::OnChar(unsigned int ch)
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

    //空隔键的回调
    if (ch == VK_SPACE)
    {
        GuiUtil_RunCallback(this, "on_space", CVarList());
    }

	return 1;
}

int RichInputBox::OnGetFocus(const PERSISTID& lost_id)
{
	ResetCaret();

	SetInputPos(GetSize());

	GuiUtil_RunCallback(this, "on_get_focus", CVarList() << lost_id);
	
	return IControl::OnGetFocus(lost_id);
}

int RichInputBox::OnLostFocus(const PERSISTID& get_id)
{
	ClearSelect();

	GuiUtil_RunCallback(this, "on_lost_focus", CVarList() << get_id);
	
	return IControl::OnLostFocus(get_id);
}

int RichInputBox::OnChanged(const PERSISTID& changed_id)
{
    if (!m_bChangedEvent)
    {
        return 0;
    }

    GuiUtil_RunCallback(this, "on_changed", CVarList() << changed_id);

    return 1;
}

ImageAnimationManager* RichInputBox::InnerGetAnimation()
{
	const IVar& var = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(var.ObjectVal());

	
}

HyperLinkStyleManager* RichInputBox::InnerGetHyperlinkMng()
{
	const IVar& var = GetCore()->GetGlobalValue("HyperLinkStyleManager");
	return (HyperLinkStyleManager*)GetCore()->GetEntity(var.ObjectVal());
}

