//--------------------------------------------------------------------
// 文件名:		RichEdit.cpp
// 内  容:		多格式文本输入框 多行 (HTML部分支持)
// 说  明:		
// 创建日期:	2008年9月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "rich_edit.h"
#include "caret.h"
#include "gui_input.h"
#include "gui_utils.h"
#include "imageanimation_manager.h"
#include "hyperlinkstyle_manager.h"
#include "../tools/string_util.h"
#include "font.h"
#include "scroll_bar.h"
#include "../public/auto_mem.h"
#include "../public/Inlines.h"
#include "../public/converts.h"
#include "../utils/language.h"

// 换行标记
#define NEW_LINE_FLAG L"<br/>"
#define NEW_LINE_FLAG_WIDTH 0

/// \file RichEdit.cpp
/// \brief 文本输入框

/// entity: RichEdit
/// \brief 文本输入框实体
/// \see 继承自IScrollable
DECLARE_ENTITY(RichEdit, 0, IScrollable);

/// property: Rect ViewRect
/// \brief 文本显示内容
DECLARE_PROPERTY(result_string, RichEdit, ViewRect, GetViewRect, SetViewRect);

/// property: bool ReadOnly
/// \brief 是否只读
DECLARE_PROPERTY(bool, RichEdit, ReadOnly, GetReadOnly, SetReadOnly);

/// property: bool EditMode
/// \brief 编辑器模式, 在编辑器模式下，拷贝粘贴没有<>格式限制
DECLARE_PROPERTY(bool, RichEdit, EditMode, GetEditMode, SetEditMode);

// property: bool SupportHtml
/// \brief 是否支持HTML
DECLARE_PROPERTY(bool, RichEdit, SupportHtml, GetSupportHtml, SetSupportHtml);

/// property: bool DragEvent
/// \brief 是否产生拖动事件 
DECLARE_PROPERTY(bool, RichEdit, DragEvent, GetDragEvent, SetDragEvent);

/// property: bool ChangedEvent
/// \brief 是否产生变化事件 
DECLARE_PROPERTY(bool, RichEdit, ChangedEvent, GetChangedEvent, SetChangedEvent);

/// property: int MaxLength
/// \brief 文本的最大长度
DECLARE_PROPERTY(int, RichEdit, MaxLength, GetMaxLength, SetMaxLength);

/// property: int Size
/// \brief 获取SIZE
DECLARE_PROPERTY_GET(int, RichEdit, Size, GetSize);

/// property: int SelectText
/// \brief 选中部分的字符串
DECLARE_PROPERTY_GET(std::wstring, RichEdit, SelectText, GetSelectText);

/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(result_string, RichEdit, Align, GetAlignString, SetAlignString);
/// property: wstring PasswordChar
/// \brief 作为密码输入时显示的字符
DECLARE_PROPERTY(const wchar_t*, RichEdit, PasswordChar, 
				 GetPasswordChar, SetPasswordChar);
/// property: string SelectForeColor
/// \brief 被选中文本的前景色
DECLARE_PROPERTY(result_string, RichEdit, SelectForeColor, 
				 GetSelectForeColorString, SetSelectForeColorString);
/// property: string SelectBackColor
/// \brief 被选中文本的背景色
DECLARE_PROPERTY(result_string, RichEdit, SelectBackColor, 
				 GetSelectBackColorString, SetSelectBackColorString);

/// property: bool Remember
/// \brief 是否记录输入
DECLARE_PROPERTY(bool, RichEdit, Remember, GetRemember, SetRemember);

/// property: int MaxRemember
/// \brief 记录输入文本数量的最大值
DECLARE_PROPERTY(int, RichEdit, MaxRemember, GetMaxRemember, SetMaxRemember);

/// property: string Caret
/// \brief 输入光标资源名
DECLARE_PROPERTY(const char*, RichEdit, Caret, GetCaretName, SetCaretName);

/// property: int LineHeight
/// \brief 最小行高,如果字体或图片大于此值则会自动将该行设成实际高度
DECLARE_PROPERTY(int, RichEdit, LineHeight, GetLineHeight, SetLineHeight);

/// property: bool ReturnAllFormat
/// \brief 为TRUE时通过TEXT属性返回全部格式，FALSE时无格式化
DECLARE_PROPERTY(bool, RichEdit, ReturnAllFormat, GetReturnAllFormat, SetReturnAllFormat);

/// property: bool ReturnFontFormat
/// \brief 是否返回字体格式，为false时格式中的字体部分将被忽略
DECLARE_PROPERTY(bool, RichEdit, ReturnFontFormat, GetReturnFontFormat, SetReturnFontFormat)

#if 0	//DECLARE_METHOD
/// method: bool Cut()
/// \brief 剪切
DECLARE_METHOD_0(bool, RichEdit, Cut);
/// method: bool Copy()
/// \brief 复制
DECLARE_METHOD_0(bool, RichEdit, Copy);
/// method: bool Paste()
/// \brief 粘贴
DECLARE_METHOD_0(bool, RichEdit, Paste);
/// method: bool Append(wstring text, int mode)
/// \brief 添加文字
/// \param text 文本
/// \param mode 0 添加在后,1添加在前 -1添加在当前光标位置,当前无光标则添加在最后
DECLARE_METHOD_2(bool, RichEdit, Append, const wchar_t*, int);

/// method: void ChangeSelectColor(const char* color);
/// \brief 将选择项变色
/// \param color 文本"255,255,255,255"
DECLARE_METHOD_1(bool, RichEdit, ChangeSelectColor, const char*);

/// method: void ChangeSelectFont(const char*font);
/// \brief 将选择项字体
/// \param text 文本
DECLARE_METHOD_1(bool, RichEdit, ChangeSelectFont, const char*);
#endif

/// method: virtual int LineNum;
/// \brief 内容行数
DECLARE_PROPERTY_GET(int, RichEdit, LineNum, GetLineNum);

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

/// event: int on_changed(object self)
/// \brief Text改变的回调
/// \param self 本控件
DECLARE_EVENT(on_changed);

/// event: int on_enter(object self)
/// \brief 输入回车键的回调
/// \param self 本控件
DECLARE_EVENT(on_enter);

/// event: int on_space(object self)
/// \brief 输入空隔键的回调
/// \param self 本控件
DECLARE_EVENT(on_space);
// RichEdit

RichEdit::RichEdit()
{
    m_bOneLine = true;
    m_nTopIndex = 0;
    m_nRealTopIndex = 0;
    m_nRealEndIndex = 0;
    m_nCaretInLine = 0;
    m_nCaretLinePos = 0;
	m_bReadOnly = false;
    m_bEditMode = false;
	m_bSupportHtml = true;
	m_bOnlyDigit = false;
	m_bMarked = false;
	m_bDragEvent = false;
    m_bChangedEvent = false;
    m_bReturnAllFormat = true;
	m_bReturnFontFormat = true;
    m_nLineHeight = 12;
	m_nDragOldX = 0;
	m_nDragOldY = 0;
	m_nMaxLength = 0;
	m_nAlign = CGuiConst::ALIGN_LEFT;
	m_nScrollStep = 6;
	m_nCaretLinePos = 0;
	m_nSelectBeg = 0;
	m_nSelectEnd = 0;
	m_SelectForeColor.SetValue(CGuiConst::SELECT_FORE_COLOR);
	m_SelectBackColor.SetValue(CGuiConst::SELECT_BACK_COLOR);

    m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.right = 100;
	m_ViewRect.bottom = 200;
    m_TextWidth = m_ViewRect.right - m_ViewRect.left;

	m_bRemember = false;
	m_nMaxRemember = 10;
	m_nCurRemember = -1;

    m_CaretPos.x = 0;
    m_CaretPos.y = 0;

	SetDesignSize(80, 20);
	SetCanFocus(true);
	SetTabStop(true);
}

RichEdit::~RichEdit()
{
	ReleaseAll();
}

void RichEdit::ReleasePart(HtmlItem* part)
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

void RichEdit::ReleaseAll()
{
	for (size_t i = 0; i < m_Buffer.size(); ++i)
	{
		ReleasePart(m_Buffer[i]);
	}
	
	m_Buffer.clear();

    m_vSimpleText.clear();
}

bool RichEdit::Init(const IVarList& args)
{
    m_Html.SetDefColor(GetForeColor());
    m_Html.SetDefFont(GuiUtil_GetValidFontName(this));

	if (!IScrollable::Init(args))
	{
		return false;
	}
	
	SetCaretName("Default");
	SetCursorName("WIN_IBEAM");

    // 创建垂直滚动条// 改为配置
    //SetHasVScroll(true);

	return true;
}

bool RichEdit::Shut()
{
	return IScrollable::Shut();
}

// 垂直滚动总数量
int RichEdit::GetVerticalTotal() const
{
    return GetContentLines();
}

// 可以显示的数量
int RichEdit::GetVerticalCanDisp() const
{
    return (m_ViewRect.bottom - m_ViewRect.top) / m_nLineHeight;
}

// 设置当前值
void RichEdit::SetVerticalValue(int value)
{
    if (value >= 0 && value <= GetVerticalTotal())
    {
        m_nTopIndex = value;
    }
}

// 获取当前值
int RichEdit::GetVerticalValue() const
{
    return m_nTopIndex;
}

int RichEdit::GetContentLines() const
{
    int iHeight = 0;

    for(size_t i = 0; i < m_vLineItems.size(); ++i)
    {
        iHeight += m_vLineItems[i].nLineHeight;
    }

    int totalline = iHeight / m_nLineHeight;

    if ((iHeight % m_nLineHeight) > 0)
    {
        totalline++;
    }

    return  totalline;
}

bool RichEdit::UpdateScroll()
{
    // 修正最后一个字符，最后一个必为<br/>
    CorrectFinalFlag();

    // 处理显示文本
    CalcVisText();

    return IScrollable::UpdateScroll();
}

// 设置显示区域如:0,0,120,40
void RichEdit::SetViewRect(const char* value)
{
    GuiUtil_StringToRect(value, m_ViewRect);
    
	m_TextWidth = m_ViewRect.right - m_ViewRect.left;
}

result_string RichEdit::GetViewRect()
{
    return GuiUtil_RectToString(m_ViewRect).c_str();
}

void RichEdit::SetReadOnly(bool value) 
{ 
	m_bReadOnly = value; 
}

bool RichEdit::GetReadOnly() const 
{ 
	return m_bReadOnly; 
}

// 编辑器模式
void RichEdit::SetEditMode(bool value)
{
    m_bEditMode = value;
}

bool RichEdit::GetEditMode() const
{
    return m_bEditMode;
}

void RichEdit::SetSupportHtml(bool value) 
{ 
	m_bSupportHtml = value; 
}

bool RichEdit::GetSupportHtml() const 
{ 
	return m_bSupportHtml; 
}


void RichEdit::SetDragEvent(bool value)
{
	m_bDragEvent = value;
}

bool RichEdit::GetDragEvent() const
{
	return m_bDragEvent;
}

// 是否产生变化事件
void RichEdit::SetChangedEvent(bool value)
{
    m_bChangedEvent = value;
}

bool RichEdit::GetChangedEvent() const
{
	return m_bChangedEvent;
}

void RichEdit::SetMaxLength(int value) 
{ 
	if (value > GetMaxSize())
	{
		return;
	}

	m_nMaxLength = value; 
}
 
int RichEdit::GetMaxLength() const 
{ 
	return m_nMaxLength; 
}

void RichEdit::SetAlignString(const char* value)
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

result_string RichEdit::GetAlignString() const
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

void RichEdit::SetPasswordChar(const wchar_t* value)
{
	Assert(value != NULL);
	
	m_wsPswdChar = value;
}

const wchar_t* RichEdit::GetPasswordChar() const
{
	return m_wsPswdChar.c_str();
}

void RichEdit::SetSelectForeColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectForeColor.SetName(GetGui(), value);
}

result_string RichEdit::GetSelectForeColorString() const
{
	return m_SelectForeColor.GetName();
}

void RichEdit::SetSelectBackColorString(const char* value)
{
	Assert(value != NULL);
	
	m_SelectBackColor.SetName(GetGui(), value);
}

result_string RichEdit::GetSelectBackColorString() const
{
	return m_SelectBackColor.GetName().c_str();
}

// 记录输入
void RichEdit::SetRemember(bool bRemember)
{
	m_bRemember = bRemember;
}

bool RichEdit::GetRemember() const
{
	return m_bRemember;
}

// 设置记录输入文本最大条数
void RichEdit::SetMaxRemember(int max)
{
	m_nMaxRemember = max;
}

int RichEdit::GetMaxRemember() const
{
	return m_nMaxRemember;
}

// 行高
void RichEdit::SetLineHeight(int height)
{
    m_nLineHeight = height;
}


int RichEdit::GetLineHeight()
{
    return m_nLineHeight;
}

// 是否获取完整格式
void RichEdit::SetReturnAllFormat(bool bFull)
{
    m_bReturnAllFormat = bFull;
}

bool RichEdit::GetReturnAllFormat()
{
    return m_bReturnAllFormat;
}

// 是否返回字体格式
void RichEdit::SetReturnFontFormat(bool bValue)
{
	m_bReturnFontFormat = bValue;
}

bool RichEdit::GetReturnFontFormat()
{
	return m_bReturnFontFormat;
}

void RichEdit::SetText(const wchar_t* val)
{
    Assert(val != NULL);

    ReleaseAll();

	AppendText(-1, val, wcslen(val));

    m_wsText = val;

    m_nCaretInLine = 0;
    m_nCaretLinePos = 0;

    ClearSelect();

    UpdateScroll();
}

const wchar_t* RichEdit::GetText() const
{
	return m_wsText.c_str();
}

point_t RichEdit::GetCaretPos()
{
    return m_CaretPos;
}

void RichEdit::SetCaretName(const char* name)
{
	Assert(name != NULL);
	
	if (!SetCaretID(name))
	{
		return;
	}
	
	m_strCaretName = name;
}

const char* RichEdit::GetCaretName() const
{
	return m_strCaretName.c_str();
}

bool RichEdit::SetCaretID(const char* name)
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

int RichEdit::GetSize() const
{
	return m_Buffer.size();
}

int RichEdit::GetMaxSize() const
{
	return 0x10000;
}

int RichEdit::AppendText(int pos, const wchar_t* ws, int size)
{
	Assert(ws != NULL);
	Assert(size >= 0);

	wchar_t pswd_char = 0;
	
	if (!m_wsPswdChar.empty())
	{
		pswd_char = m_wsPswdChar[0];
	}

    int count = 0;

    std::vector< TextSegment >*pSegments = NULL;

    if (m_bSupportHtml)
    {
        m_Html.SetText(ws, GetForeColor(), GuiUtil_GetValidFontName(this), 12);

        pSegments = m_Html.GetSegments();
    }
    else
    {
        if (m_vSimpleText.size() == 0)
        {
            m_vSimpleText.resize(1);
            m_vSimpleText[0].m_processlist.push_back(true);
            HtmlTextItem item;
            item.m_wtext = ws;
            m_vSimpleText[0].m_TextItemList.push_back(item);
            pSegments = &m_vSimpleText;
        }
        else
        {
            m_vSimpleText[0].m_TextItemList[0].m_wtext += ws;
            pSegments = &m_vSimpleText;
        }
    }

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
                            int size = GetPainter()->GetNextLanguageCharacter(
								textItem.m_wtext.c_str(), t);

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
                                m_Buffer.insert(m_Buffer.begin() + pos, 
									pTextItem);
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
                            imageItem.m_Rect = MAKE_RECT(0, 0, pt.x, pt.y);
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

        // 加一个换行
        bool bNeedNewLineFlag = true;
        HtmlItem* pBackItem = m_Buffer.back();

        if (pBackItem->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pBackTextItem = (HtmlTextItem*)pBackItem;

            if (pBackTextItem->m_wtext == NEW_LINE_FLAG)
            {
                bNeedNewLineFlag = false;
            }
        }

        if (bNeedNewLineFlag)
        {
            HtmlTextItem* pTextItem = CORE_NEW(HtmlTextItem);

            if (pTextItem)
            {
                pTextItem->m_Type = PARTTYPE_CHAR;
                pTextItem->m_wtext = NEW_LINE_FLAG;
                pTextItem->m_Width = NEW_LINE_FLAG_WIDTH;

                m_Buffer.push_back(pTextItem);
            }
        }
	}

	return count;
}

void RichEdit::GenerateText(PART_CONTAINER& parts, int pos, int num, 
							std::wstring& ws)
{
	ws = L"";
	
    int end = pos + num;

    if (size_t(end) > parts.size())
    {
        end = parts.size();
    }

    // 无格式化
    if (!m_bReturnAllFormat)
    {
        HtmlTextItem textitem;

        for (int i = pos; i < end; ++i)
        {
            int type = parts[i]->m_Type;

            if (type == PARTTYPE_CHAR)
            {
                HtmlTextItem* pTextItem = (HtmlTextItem*)parts[i];

                if (pTextItem->m_wtext != NEW_LINE_FLAG)
                {
                    ws += pTextItem->m_wtext;
                }
            }
        }

        return;
    }

    // 格式化恢得
    std::string szDefaultFont = GuiUtil_GetValidFontName(this);
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

                        if (m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                        {
                            ws += buffer;
                        }

                        textitem.m_Face = szDefaultFont;
                        textitem.m_Color = GetForeColor();
                        textitem.m_wtext = L"";
                    }

                    std::wstring buffer = L"";

                    if (m_Html.RestoreToHtml(*(HtmlTextItem*)parts[i], buffer, m_bReturnFontFormat))
                    {
                        ws += buffer;
                    }
                }
                else if (pTextItem->m_wtext == NEW_LINE_FLAG)
                {
                    if (textitem.m_wtext.size() > 0)
                    {
                        std::wstring buffer = L"";

                        if (m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                        {
                            ws += buffer;
                        }

                        textitem.m_Face = szDefaultFont;
                        textitem.m_Color = GetForeColor();
                        textitem.m_wtext = L"";

                        ws += NEW_LINE_FLAG;
                    }
                }
                else if (pTextItem->m_Face != textitem.m_Face 
					|| pTextItem->m_Color != textitem.m_Color)
                {
                    if (textitem.m_wtext.size() > 0)
                    {
                        std::wstring buffer = L"";

                        if (m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                        {
                            ws += buffer;
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

                if (m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
                {
                    ws += buffer;
                }

                textitem.m_Face = szDefaultFont;
                textitem.m_Color = GetForeColor();
                textitem.m_wtext = L"";
            }

            std::wstring buffer = L"";

            if (m_Html.RestoreToHtml(*(HtmlImageItem*)parts[i], buffer))
            {
                ws += buffer;
            }
		}
	}

    if (textitem.m_wtext.size() > 0)
    {
        std::wstring buffer = L"";

        if (textitem.m_wtext == NEW_LINE_FLAG)
        {
            ws += textitem.m_wtext;
        }
        else if (m_Html.RestoreToHtml(textitem, buffer, m_bReturnFontFormat))
        {
            ws += buffer;
        }
    }
}

int RichEdit::InsertText(int pos, const wchar_t* ws, int size)
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

    UpdateScroll();

	return count;
}

void RichEdit::CorrectFinalFlag()
{
    int finalpos = GetSize() - 1;

    bool IsFindFinalFlag = false;

    if (finalpos >= 0)
    {
        if (m_Buffer[finalpos]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[finalpos];

            if (wcsicmp(pTextItem->m_wtext.c_str(), NEW_LINE_FLAG) == 0)
            {
               IsFindFinalFlag = true; 
            }
        }
    }

    // 如果没找到则添加
    if (!IsFindFinalFlag)
    {
        HtmlTextItem* pTextItem = CORE_NEW(HtmlTextItem);

        if (pTextItem)
        {
            pTextItem->m_Type = PARTTYPE_CHAR;
            pTextItem->m_wtext = NEW_LINE_FLAG;
            pTextItem->m_Width = NEW_LINE_FLAG_WIDTH;

            m_Buffer.push_back(pTextItem);
        }
    }

}

bool RichEdit::DeleteLeftText(int pos)
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
        if (!pItem->m_IsHyperLink && pItem->m_wtext != NEW_LINE_FLAG)
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

    UpdateScroll();
    return true;

}

bool RichEdit::DeleteText(int pos, int num)
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

std::wstring RichEdit::GetSelectText()
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

bool RichEdit::HasSelectText() const
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

void RichEdit::ClearSelect()
{
	m_nSelectBeg = 0;
	m_nSelectEnd = 0;
}

// 将选择项变色
bool RichEdit::ChangeSelectColor(const char* color)
{
    if (m_nSelectBeg == m_nSelectEnd)
    {
        return false;
    }

    unsigned int fcolor = GuiUtil_ColorToInt(color);

    for (int i = m_nSelectBeg; i < m_nSelectEnd; ++i)
    {
        if (m_Buffer[i]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[i];

            if (pTextItem->m_IsHyperLink)
            {
                continue;
            }

            pTextItem->m_Color = fcolor;
        }
    }
    
    GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);
    return true;
}

// 将选择项变字体
bool RichEdit::ChangeSelectFont(const char*font)
{
    if (m_nSelectBeg == m_nSelectEnd)
    {
        return false;
    }

    for (int i = m_nSelectBeg; i < m_nSelectEnd; ++i)
    {
        if (m_Buffer[i]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[i];

            if (pTextItem->m_IsHyperLink)
            {
                continue;
            }

            pTextItem->m_Face = font;

            GetGui()->SetFont(font, GetShadowColor());

            pTextItem->m_Width = GetPainter()->GetTextLenWidth(
				pTextItem->m_wtext.c_str(), pTextItem->m_wtext.size());
            
			UpdateScroll();
        }
    }

    return true;
}

bool RichEdit::Cut()
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

bool RichEdit::Copy()
{
	std::wstring ws = GetSelectText();
	
	if (ws.empty())
	{
		return false;
	}
	
    // 标签过滤
    if (!m_bEditMode)
    {
        std::wstring ws_buffer = L"";
        
        size_t begin = 0;

        size_t pos_begin = ws.find(L"<");
        size_t pos_end = ws.find(L">");

        while (pos_begin != std::wstring::npos && pos_end != std::wstring::npos && pos_end > pos_begin)
        {
            if (pos_begin > begin)
            {
                ws_buffer += ws.substr(begin, pos_begin - begin);
            }

            begin = pos_end + 1;

            pos_begin = ws.find(L"<", begin);
            pos_end = ws.find(L">", begin);
        }

        ws_buffer += ws.substr(begin, ws.length() - begin);

        ws = ws_buffer;
    }

	return GuiUtil_CopyToClipboard(ws.c_str());
}

int RichEdit::get_real_pos()
{
    // 获取光标真实位置
    int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

    if (real_pos >= 0 && real_pos <= GetSize())
    {
        //
    }
    else
    {
        real_pos = GetSize();
    }

    return real_pos;
}

bool RichEdit::Paste()
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

    // 长度限制，不能大于最大长度
    if (m_nMaxLength > 0)
    {
        int leave_length = m_nMaxLength - m_Buffer.size();

        if (leave_length <= 0)
        {
            return false;
        }

        if (leave_length < (int)ws.length())
        {
            ws = ws.substr(0, leave_length);
        }
    }

    int real_pos = get_real_pos();

    int count = 0;

    // 标签过滤
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

                count += InsertText(real_pos, insert_text.c_str(), insert_text.length());

                if (count > 0)
                {
                    SetInputPos(real_pos + count);
                }

                real_pos = get_real_pos();
            }

            begin = pos_end + 1;

            pos_begin = ws.find(L"<", begin);
            pos_end = ws.find(L">", begin);
        }

        std::wstring insert_text = ws.substr(begin, ws.length() - begin).c_str();

        count = InsertText(real_pos, insert_text.c_str(), insert_text.length());
    }
    else
    {
        count = InsertText(real_pos, ws.c_str(), ws.length());
    }

	if (0 == count)
	{
		return false;
	}
	
	SetInputPos(real_pos + count);
	
	return true;
}

bool RichEdit::Append(const wchar_t* text, int mode)
{
    int pos = 0;

    if (mode == 1)
    {
        pos = GetSize();
    }
    else
    {
        pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

        if (pos >= 0 && pos <= GetSize())
        {
            pos = pos;
        }
        else
        {
            pos = GetSize();
        }
    }

    int count = InsertText(pos, text, wcslen(text));
	
	SetInputPos(pos + count);

    RefreshScrollByCaret();

    UpdateScroll();

	if (count > 0)
	{
		OnChanged(GetID());
	}

	return true;
}

// 获取行数
int RichEdit::GetLineNum()
{
    return m_vLineItems.size();
}

int RichEdit::GetSelectBeg() const
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

int RichEdit::GetSelectEnd() const
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

bool RichEdit::SetSelect(int begin,int end)
{
    if (begin > end)
    {
        ClearSelect();
        return true;
    }

    m_nSelectBeg = begin;
    m_nSelectEnd = end;

	m_nSelectBeg = NormalizePos(m_nSelectBeg);
	m_nSelectEnd = NormalizePos(m_nSelectEnd);
	
	return true;
}

bool RichEdit::DeleteSelectText()
{
	Assert(HasSelectText());

	int beg = GetSelectBeg();
	int end = GetSelectEnd();

	if (!DeleteText(beg, end - beg))
	{
		return false;
	}

	ClearSelect();

    UpdateScroll();

	SetInputPos(beg);

	return true;
}

int RichEdit::NormalizePos(int pos) const
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

int RichEdit::GetVisualCount(int pos, int width, int&height)
{
	int w = 0;
    height = 0;
	
	for (size_t i = pos; i < m_Buffer.size(); ++i)
	{
		w += m_Buffer[i]->m_Width;

        // 获取高度
        if (m_Buffer[i]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[i];

            Font* pFont = (Font* )GetCore()->GetEntity(
				GetGui()->GetFont(pTextItem->m_Face.c_str()));

            if (pFont == NULL)
            {
                // 如果配置不正确,则再取一次此控件默认的字体
                std::string szDefaultFont = GuiUtil_GetValidFontName(this);

                pFont = (Font* )GetCore()->GetEntity(
					GetGui()->GetFont(szDefaultFont.c_str()));
            }

            if (pFont)
            {
                if (height < pFont->GetHeight())
                {
                    height = pFont->GetHeight();
                }
            }

            if (wcsicmp(pTextItem->m_wtext.c_str() , NEW_LINE_FLAG)  == 0)
            {
                return (i - pos + 1);
            }
        }
        else if (m_Buffer[i]->m_Type == PARTTYPE_IMAGE)
        {
            HtmlImageItem* pImageItem = (HtmlImageItem*)m_Buffer[i];

            if (height < pImageItem->m_Rect.bottom - pImageItem->m_Rect.top)
            {
                height = pImageItem->m_Rect.bottom - pImageItem->m_Rect.top;
            }
        }

		if (w > width)
		{
            // 整词折行处理时遇到空格则换行
            if (GetGui()->GetWordChangeLine())
            {
                // 倒退处理折行
                for (int k = i; k > pos; --k)
                {
                    if (m_Buffer[k]->m_Type == PARTTYPE_IMAGE)
                    {
                        return (k - pos + 1);
                    }
                    else
                    {
                        HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[k];
                        if (pTextItem->m_IsHyperLink)
                        {
                            return (k - pos + 1);
                        }
                        else if (pTextItem->m_wtext == L" ")
                        {
                            return (k - pos + 1);
                        }
                    }
                }
            }

			// 修改RichEdit换行多一个item的bug
			// 2012-5-31 by liujie
			//return (i - pos + 1);
			return (i - pos);
		}
	}
	
	return (m_Buffer.size() - pos);
}

void RichEdit::SetInputPos(int pos)
{
    if (pos > 0)
    {
        if (pos > GetSize())
        {
            // 如果超范围,则定位到最后
            if (m_vLineItems.size() == 0)
            {
                m_nCaretLinePos = 0;
                m_nCaretInLine = 0;
                return;
            }

            m_nCaretInLine = m_vLineItems.size() - 1;
            m_nCaretLinePos = GetSize() 
				- m_vLineItems[m_nCaretInLine].nStartPos - 1;
            return;
        }

        // 确定行
        size_t i = 0;

        for (i = 0; i < m_vLineItems.size(); ++i)
        {
            if (pos <= m_vLineItems[i].nEndPos+1)
            {
                m_nCaretInLine = i;
                break;
            }
        }

        if (m_Buffer[pos - 1]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = (HtmlTextItem*)m_Buffer[pos - 1];

            if (wcsicmp(pTextItem->m_wtext.c_str(), NEW_LINE_FLAG) == 0)
            {
                //当前设置的是换行符之后,
                //1 后如果最后一项
                if (pos > GetSize() - 1)
                {
                    m_nCaretLinePos = 
						pos - m_vLineItems[m_nCaretInLine].nStartPos - 1;
                    return;
                }
                else
                {
                    m_nCaretInLine = m_nCaretInLine+1;
                    m_nCaretLinePos = 0;
                    return;
                }
            }
        }

        m_nCaretLinePos = pos - m_vLineItems[m_nCaretInLine].nStartPos;
    }
    else
    {
        m_nCaretInLine = 0;
        m_nCaretLinePos = 0;
    }
}

void RichEdit::SetInputPos(int line, int pos)
{
    if (line >= 0 && line < (int)m_vLineItems.size())
    {
        // 一行上的光标位置总共有文字个数+1
        if (m_vLineItems[line].nEndPos - m_vLineItems[line].nStartPos + 1 + 1 < pos)
        {
            // 下一行
            SetInputPos(line + 1, pos - (m_vLineItems[line].nEndPos 
                - m_vLineItems[line].nStartPos + 1));
        }
        else if (pos < 0 && line > 0)
        {
            // 上一行
            SetInputPos(line - 1, pos + (m_vLineItems[line - 1].nEndPos 
                - m_vLineItems[line - 1].nStartPos + 1));
        }
        else
        {
            // 检查当前设置的是否是换行符后
            int realpos = GetCareRealPos(line,pos);

            SetInputPos(realpos);
        }
    }
    else
    {
        line--;
        if (line >= 0 && line < (int)m_vLineItems.size())
        {
            int realpos = GetCareRealPos(line, m_vLineItems[line].nEndPos);
            SetInputPos(realpos);
        }
    }
}

int RichEdit::GetCaretWidth() const
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (NULL == caret)
	{
		return 0;
	}
	
	return caret->GetFixWidth();
}

int RichEdit::GetShowWidth() const
{
	return m_TextWidth;
}

void RichEdit::CalcVisText()
{
    m_vLineItems.clear();
    
    int show_start_pos = 0;

    while (show_start_pos < (int)m_Buffer.size())
    {
        LineItem lineitem;

        // 进行虚拟绘制
        int show_linecount = 0;
        int show_height = 0;

        // 获取一行的显示Item数量
        show_linecount = GetVisualCount(show_start_pos, m_TextWidth, 
			show_height);
        
        lineitem.nStartPos = show_start_pos;
        lineitem.nEndPos = show_start_pos + show_linecount - 1;

        if (show_height < m_nLineHeight)
        {
            lineitem.nLineHeight = m_nLineHeight;
        }
        else
        {
            lineitem.nLineHeight = show_height;
        }

        show_start_pos += show_linecount;

        m_vLineItems.push_back(lineitem);
    }

    //int itest = 0;
}

void RichEdit::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);
	
	CVar varDefault;
	
    di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
       &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("HasVScroll", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);

	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	
    //一定要设，可视范围,即窗户的大小,一般不要超过控件范围
    di->AddPropInfo("ViewRect", IDesignInfo::TYPE_STRING, true, false);
    //不设默认为14
    di->AddPropInfo("LineHeight", IDesignInfo::TYPE_INT, true, false, 
		&(varDefault.SetInt(12),varDefault));
    di->AddPropInfo("AutoScroll", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("ReadOnly", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("EditMode", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(false),varDefault));
	di->AddPropInfo("SupportHtml", IDesignInfo::TYPE_BOOL, true, true, 
        &(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("OnlyDigit", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("Marked", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("DragEvent", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("ChangedEvent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("MaxLength", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, 
		true, true,	&(varDefault.SetString("Left"),varDefault));
	di->AddPropInfo("PasswordChar", IDesignInfo::TYPE_WIDESTR, true, true);
	di->AddPropInfo("SelectForeColor", IDesignInfo::TYPE_COLOR, true, true,
		&(varDefault.SetString(GuiUtil_IntToColor(CGuiConst::SELECT_FORE_COLOR).c_str()),varDefault) );
	di->AddPropInfo("SelectBackColor", IDesignInfo::TYPE_COLOR, true, true,
		&(varDefault.SetString(GuiUtil_IntToColor(CGuiConst::SELECT_BACK_COLOR).c_str()),varDefault) );
	di->AddPropInfo("Caret", IDesignInfo::TYPE_STRING, true, true);
    di->AddPropInfo("ReturnAllFormat", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(true),varDefault) );

    di->AddSuborInfo("VScrollBar", "ScrollBar");

	di->AddEventInfo("on_get_focus");
	di->AddEventInfo("on_lost_focus");
	di->AddEventInfo("on_drag");
	di->AddEventInfo("on_changed");
	di->AddEventInfo("on_enter");
	di->AddEventInfo("on_space");
}

void RichEdit::PaintSelectText()
{
	int beg = GetSelectBeg();
	int end = GetSelectEnd();

    if (beg >= end)
    {
        return;
    }

    int beginline = 0;
    int endline = 0;
    bool IsBeginFound = false,IsEndFound = false;

    for (size_t i = 0; i < m_vLineItems.size(); ++i)
    {
        if (m_vLineItems[i].nStartPos <= beg 
			&& beg <= m_vLineItems[i].nEndPos)
        {
            beginline = i;
            IsBeginFound = true;
        }
        
        if (m_vLineItems[i].nStartPos <= end 
			&& end <= m_vLineItems[i].nEndPos)
        {
            endline = i;
            IsEndFound = true;
        }

        if (IsEndFound&&IsEndFound)
        {
            break;
        }
    }

    if (!IsBeginFound)
    {
        beginline = 0;
    }

    if (!IsEndFound)
    {
        endline = m_vLineItems.size();
    }

	if (beginline < m_nRealTopIndex)
    {
        beginline = m_nRealTopIndex;
    }

    if (endline < beginline)
    {
        return;
    }

    int y = InnerGetAbsTop() + m_ViewRect.top;

    for (int l = m_nRealTopIndex; l<beginline; ++l)
    {
        y += m_vLineItems[l].nLineHeight;
    }

    std::string szDefaultFont = GuiUtil_GetValidFontName(this);
    std::string szNowFont = szDefaultFont;

    GetGui()->SetFont(szDefaultFont.c_str(), GetShadowColor());

    for (int i = beginline; i <= endline; ++i)
    {
        if (i < m_nRealTopIndex)
        {
            continue;
        }

        if (i > m_nRealEndIndex)
        {
            break;
        }
        
		int x_start = 0;
        int x_end = m_ViewRect.right - m_ViewRect.left;
        int w_start = m_vLineItems[i].nStartPos;
        int w_end = m_vLineItems[i].nEndPos;

	    // 起始位置
        if (i == beginline && m_nSelectBeg > m_vLineItems[beginline].nStartPos)
        {
	        for (int k = m_vLineItems[beginline].nStartPos; k < m_nSelectBeg; ++k)
	        {
		        x_start += m_Buffer[k]->m_Width;
	        }

            w_start = m_nSelectBeg;
        }

        // 结束位置
        if (i == endline && m_nSelectEnd < m_vLineItems[endline].nEndPos)
        {
            x_end = 0;

            int k = 0;

            for (k = m_vLineItems[endline].nStartPos; k < m_nSelectEnd; ++k)
            {
               x_end += m_Buffer[k]->m_Width;
            }

            w_end = m_nSelectEnd-1;
        }

        if (x_end <= x_start)
        {
            continue;
        }
    	
	    unsigned int back_color = m_SelectBackColor.GetValue();
	    unsigned int fore_color = m_SelectForeColor.GetValue();
    	
	    TAutoMem<wchar_t, 128> auto_buf(w_end - w_start + 1 + 1);
    	
	    wchar_t* buf = auto_buf.GetBuffer();
    	
	    int count = 0;
    	
	    int h = GetPainter()->GetTextHeight(L" ");
	    int width = 0;
    	
	    int x = GetVisTextStart(m_vLineItems[i].nStartPos, 
			m_vLineItems[i].nEndPos) + x_start;

        for (int w = w_start; w <= w_end; ++w)
        {
            int type = m_Buffer[w]->m_Type;

            if (type == PARTTYPE_CHAR)
            {
                HtmlTextItem* pChar = (HtmlTextItem*)m_Buffer[w];

                if (wcsicmp(pChar->m_wtext.c_str(), NEW_LINE_FLAG) == 0)
                {
                    // 如果之前存在则绘之前的
                    if (count > 0)
                    {
                        buf[count] = 0;

                        GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                        int h = GetPainter()->GetTextHeight(L" ");

                        GetPainter()->FillRect(x, y, x + width, 
							y + m_vLineItems[i].nLineHeight, back_color);
                        GetPainter()->WriteText(x, 
							y + m_vLineItems[i].nLineHeight - h, fore_color, 
							buf);

						szNowFont = szDefaultFont;
                        
						x += width;

                        width = 0;
                        count = 0;
                    }
                    continue;
                }

                if (pChar->m_IsHyperLink)
                {
                    // 如果之前存在则绘之前的
                    if (count > 0)
                    {
                        buf[count] = 0;

                        GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                        int h = GetPainter()->GetTextHeight(L" ");

                        GetPainter()->FillRect(x, y, x + width, 
							y + m_vLineItems[i].nLineHeight, back_color);
                        GetPainter()->WriteText(x, 
							y + m_vLineItems[i].nLineHeight - h, fore_color, 
							buf);
                        
						szNowFont = szDefaultFont;
                        
						x += width;
                        
						width = 0;
                        count = 0;
                    }

                    // 获取超链接信息
                    HyperLinkStyleManager* pHLinkManager = 
						InnerGetHyperlinkMng();

                    if (NULL == pHLinkManager)
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

                        GetGui()->SetFont(pChar->m_Face.c_str(), GetShadowColor());
                    }

                    // 绘现在的
                    int h = GetPainter()->GetTextHeight(L" ");

                    GetPainter()->FillRect(x, y, x + width, 
						y + m_vLineItems[i].nLineHeight, back_color);
                    GetPainter()->WriteText(x, 
						y + m_vLineItems[i].nLineHeight - h, fore_color, 
						pChar->m_wtext.c_str());
                    
					x += pChar->m_Width;
                }
                else if (pChar->m_Color != GetForeColor() 
					|| stricmp(pChar->m_Face.c_str(), szNowFont.c_str()) != 0)
                {
                    // 如果是格式不同
                    // 如果之前存在则绘之前的
                    if (count > 0)
                    {
                        buf[count] = 0;
                        
						GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());
                        
						int h = GetPainter()->GetTextHeight(L" ");
                        
						GetPainter()->FillRect(x, y, x + width, 
							y + m_vLineItems[i].nLineHeight, back_color);
                        GetPainter()->WriteText(x, 
							y + m_vLineItems[i].nLineHeight - h, fore_color, 
							buf);
                        
						x += width;
                        
						width = 0;
                        count = 0;
                    }

                    szNowFont = pChar->m_Face;

                    // 重新开始记录
                    for (size_t c = 0; c < pChar->m_wtext.size(); ++c)
                    {
                        buf[count++] = pChar->m_wtext[c];
                    }

                    width += pChar->m_Width;
                }
                else
                {
                    // 继续记录
                    for (size_t c = 0; c < pChar->m_wtext.size(); ++c)
                    {
                        buf[count++] = pChar->m_wtext[c];
                    }

                    width += pChar->m_Width;
                }
            }
            else if (type == PARTTYPE_IMAGE)
            {
                int h = GetPainter()->GetTextHeight(L" ");

                if (count > 0)
                {
                    buf[count] = 0;

                    GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                    int h = GetPainter()->GetTextHeight(L" ");

                    GetPainter()->FillRect(x, y, x + width, 
						y + m_vLineItems[i].nLineHeight, back_color);
                    GetPainter()->WriteText(x, 
						y + m_vLineItems[i].nLineHeight - h, fore_color, buf);

                    x += width;

                    width = 0;
                    count = 0;
                }

                HtmlImageItem* pImage = (HtmlImageItem*)m_Buffer[w];

                x += pImage->m_Width;
            }
        }

        // 行末
        if (count > 0)
        {
            GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

            buf[count] = 0;

            int h = GetPainter()->GetTextHeight(L" ");

            GetPainter()->FillRect(x, y, x + width, 
				y + m_vLineItems[i].nLineHeight, back_color);
            GetPainter()->WriteText(x, y + m_vLineItems[i].nLineHeight - h, 
				fore_color, buf);

            szNowFont = szDefaultFont;
        }

        y += m_vLineItems[i].nLineHeight;
    }

}

void RichEdit::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
    RefreshInnerAbsPos();

    ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

    // 整个内容高度中的一部分,用于判断绘制内容
    long lContentTop = 0;

    if (pScrollbar && pScrollbar->GetVisible())
	{
		lContentTop = pScrollbar->GetCurValue() * m_nLineHeight;
	}

    long totaly = 0;

    for (size_t i = 0; i < m_vLineItems.size(); ++i)
    {
        if (totaly >= lContentTop)
        {
            m_nRealTopIndex = i;
            break;
        }

        totaly += m_vLineItems[i].nLineHeight;
    }

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
    std::string szNowFont = szDefaultFont;
    int nDefaultColor = GetForeColor();
    int nNowColor = nDefaultColor;

	GetGui()->SetFont(szDefaultFont.c_str(), GetShadowColor());

    // 光标所在Y位置
    int nCareYPos = y1 + m_ViewRect.top;
    // 获取可以显示的第一行行号
    int startindex = m_nRealTopIndex;
    int endindex = m_vLineItems.size() - 1;

    int y = y1 + m_ViewRect.top;

    for (size_t j = startindex; j < m_vLineItems.size(); ++j)
    {
        if (y + m_vLineItems[j].nLineHeight 
			> y1 + m_ViewRect.top + (m_ViewRect.bottom - m_ViewRect.top))
        {
            endindex = j-1;
            break;
        }
        
        if (m_vLineItems[j].nStartPos <=  m_vLineItems[j].nEndPos)
        {
            int x_start = GetVisTextStart(m_vLineItems[j].nStartPos, 
				m_vLineItems[j].nEndPos);
            int x = x_start;

            TAutoMem<wchar_t, 128> auto_buf(
				m_vLineItems[j].nEndPos - m_vLineItems[j].nStartPos + 1);

            wchar_t* buf = auto_buf.GetBuffer();

            int count = 0;

            int w = 0;

            for (int pos = m_vLineItems[j].nStartPos; 
				pos <= m_vLineItems[j].nEndPos; ++pos)
            {
                int type = m_Buffer[pos]->m_Type;

                if (type == PARTTYPE_CHAR)
                {
                    HtmlTextItem* pChar = (HtmlTextItem*)m_Buffer[pos];

                    if (wcsicmp(pChar->m_wtext.c_str(), NEW_LINE_FLAG) == 0)
                    {
                        // 如果之前存在则绘之前的
                        if (count > 0)
                        {
                            buf[count] = 0;

                            GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                            int h = GetPainter()->GetTextHeight(L" ");

                            GetPainter()->WriteText(x, 
								y + m_vLineItems[j].nLineHeight - h, 
								nNowColor, buf);
                            
							szNowFont = szDefaultFont;
                            nNowColor = nDefaultColor;

                            x += w;

                            w = 0;
                            count = 0;
                        }

                        continue;
                    }

                    if (pChar->m_IsHyperLink)
                    {
                        // 如果之前存在则绘之前的
                        if (count > 0)
                        {
                            buf[count] = 0;

                            GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                            int h = GetPainter()->GetTextHeight(L" ");

                            GetPainter()->WriteText(x, 
								y + m_vLineItems[j].nLineHeight - h, 
								nNowColor, buf);

                            szNowFont = szDefaultFont;
                            nNowColor = nDefaultColor;

                            x += w;

                            w = 0;
                            count = 0;
                        }
                
                        // 获取超链接信息
                        HyperLinkStyleManager* pHLinkManager = 
							InnerGetHyperlinkMng();
                        
						if (NULL == pHLinkManager)
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

                            GetGui()->SetFont(pChar->m_Face.c_str(), GetShadowColor());
                        }

                        // 绘现在的
                        int h = GetPainter()->GetTextHeight(L" ");

                        GetPainter()->WriteText(x, 
							y + m_vLineItems[j].nLineHeight - h, 
							pChar->m_Color, pChar->m_wtext.c_str());
                        
						x += pChar->m_Width;

                    }
                    else if (pChar->m_Color != nNowColor 
						|| stricmp(pChar->m_Face.c_str(), szNowFont.c_str()) != 0)
                    {
                        // 如果是格式不同
                        // 如果之前存在则绘之前的
                        if (count > 0)
                        {
                            buf[count] = 0;

                            GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                            int h = GetPainter()->GetTextHeight(L" ");

                            GetPainter()->WriteText(x, 
								y + m_vLineItems[j].nLineHeight - h, 
								nNowColor, buf);
                            
							x += w;
                            
							w = 0;
                            count = 0;
                        }
                        
						nNowColor = pChar->m_Color;
                        szNowFont = pChar->m_Face;

                        // 重新开始记录
                        for (size_t c = 0; c < pChar->m_wtext.size(); ++c)
                        {
                            buf[count++] = pChar->m_wtext[c];
                        }

                        w += pChar->m_Width;
                    }
                    else
                    {
                        // 继续记录
                        for(size_t c = 0; c < pChar->m_wtext.size(); ++c)
                        {
                            buf[count++] = pChar->m_wtext[c];
                        }

                        w += pChar->m_Width;
                    }
                }
                else if (type == PARTTYPE_IMAGE)
                {
                    if (count > 0)
                    {
                        buf[count] = 0;

                        GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                        int h = GetPainter()->GetTextHeight(L" ");

                        GetPainter()->WriteText(x, 
							y + m_vLineItems[j].nLineHeight - h, nNowColor, 
							buf);

                        x += w;

                        w = 0;
                        count = 0;
                    }

                    HtmlImageItem* pImage = (HtmlImageItem*)m_Buffer[pos];

                    if (!pImage->m_imageID.IsNull())
                    {
                        rect_t rect = { 0, 0, 
							GetPainter()->GetGlyphWidth(pImage->m_imageID),
							GetPainter()->GetGlyphHeight(pImage->m_imageID)
						};

                        int h = GetPainter()->GetTextHeight(L" ");

                        GetPainter()->DrawGlyph(x ,y + h - rect.bottom, 
							x + pImage->m_Width, y + h, rect.left, rect.top,
							rect.right, rect.bottom, pImage->m_imageID);
                    }
                    else
                    {
                        ImageAnimationManager* pAniManager = 
							InnerGetAnimation();

                        if (pAniManager)
                        {
                            std::string szValue = StringUtil::WideStrAsString(
								pImage->m_imagesrc.c_str());

                            point_t pt = pAniManager->GetAnimationSize(
								szValue.c_str());

                            rect_t rect = { 0, 0, pt.x, pt.y};

                            pAniManager->DrawAnimation(szValue.c_str(), x, 
								y + m_vLineItems[j].nLineHeight - pt.y, rect);
                        }
                    }

                    x += pImage->m_Width;
                }
            }
            
            // 行末
            if (count > 0)
            {
                
                GetGui()->SetFont(szNowFont.c_str(), GetShadowColor());

                buf[count] = 0;

                int h = GetPainter()->GetTextHeight(L" ");

                GetPainter()->WriteText(x, y + m_vLineItems[j].nLineHeight - h, 
					nNowColor, buf);

                szNowFont = szDefaultFont;
                nNowColor = nDefaultColor;
            }
        }

        if (j == m_nCaretInLine)
        {
            nCareYPos = y;
        }

        y += m_vLineItems[j].nLineHeight;
        
    }

    m_nRealEndIndex = endindex;

	if (HasSelectText())
	{
		PaintSelectText();
	}

	if (GetFocused() && GetEnabled() && (!m_bReadOnly) 
		&& m_nCaretInLine >= startindex && m_nCaretInLine <= endindex)
	{
        int x_start = GetVisTextStart(0, 0);

        if (m_nCaretInLine < (int)m_vLineItems.size())
        {
            x_start = GetVisTextStart(m_vLineItems[m_nCaretInLine].nStartPos, 
				m_vLineItems[m_nCaretInLine].nEndPos);
        }

		// 显示输入光标
		Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
		
		if (caret)
		{
			int h = GetPainter()->GetTextHeight(L" ");
			int w = 0;
            
            if (m_nCaretInLine < (int)m_vLineItems.size())
            {
                w = GetTotalWidth(m_Buffer, 
					m_vLineItems[m_nCaretInLine].nStartPos, m_nCaretLinePos);
            }
			
			int left = x_start + w;
			int bottom = nCareYPos + h;

            if (m_nCaretInLine < (int)m_vLineItems.size())
            {
                bottom = nCareYPos + m_vLineItems[m_nCaretInLine].nLineHeight;
            }

			int height = GetPainter()->GetTextHeight(L"_");
			int width = GetPainter()->GetTextWidth(L"_");


            m_CaretPos.x = left;
            m_CaretPos.y = bottom;

			caret->Draw(left, bottom, width, height, GetForeColor());
		}
	}

    IScrollable::Paint(seconds);
}

int RichEdit::GetTotalWidth(PART_CONTAINER& parts, int pos, int num)
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

int RichEdit::GetVisTextStart(int startpos, int endpos)
{
	int x1 = InnerGetAbsLeft();
	
	switch (m_nAlign)
	{
	case CGuiConst::ALIGN_LEFT:
		return x1 + m_ViewRect.left;
	case CGuiConst::ALIGN_CENTER:
		return x1 + m_ViewRect.left + (m_TextWidth - GetTotalWidth(
			m_Buffer, startpos, endpos - startpos + 1)) / 2;
	case CGuiConst::ALIGN_RIGHT:
		return x1 + m_ViewRect.left + (m_ViewRect.right - m_ViewRect.left) 
			- GetTotalWidth(m_Buffer, startpos, endpos - startpos + 1);
	default:
		Assert(0);
		break;
	}
	
	return x1;
}

int RichEdit::GetCursorInPos(int x, int y, int&line, int&linepos)
{
    // 通过y获取当前实际所有行
    // 获取相对Y
	int sy = y - InnerGetAbsTop() - m_ViewRect.top;

    line = 0;

    int totoay = 0;

    for (line = m_nRealTopIndex; line < (int)m_vLineItems.size(); ++line)
    {
        totoay += m_vLineItems[line].nLineHeight;

        if (sy < totoay)
        {
            break;
        }
    }

    if (line >= (int)m_vLineItems.size())
    {
        // 如果鼠标点击后移到了文本的下方则认为选中从光标开始的后面所有文本
        if (m_vLineItems.size() > 0)
        {
            line = m_vLineItems.size() - 1;
            linepos = m_vLineItems[line].nEndPos 
				- m_vLineItems[line].nStartPos + 1;
        }
        else
        {
            line = 0;
            linepos = 0;
        }
        
        return linepos;
    }

    int start = 0;
    int end = 0;

    if (line < (int)m_vLineItems.size())
    {
        start = m_vLineItems[line].nStartPos;
        end = m_vLineItems[line].nEndPos;
    }

    // 获取相对X
	int sx = x - GetVisTextStart(start, end);

	//sx = DivScaledValue(sx);

	if (sx < 0)
	{
		return -1;
	}
	
	int w = 0;

    int showcount = end - start + 1;

    if (end == start)
    {
        showcount = 0;
    }

	for (int i = 0; i < showcount; ++i)
	{
		w += m_Buffer[start + i]->m_Width;
		
		if (sx <= w)
		{
			linepos = i;
            return linepos;
		}
	}

    linepos = showcount;

	return linepos;
}

void RichEdit::ResetCaret()
{
	Caret* caret = (Caret*)GetCore()->GetEntity(m_CaretID);
	
	if (caret)
	{
		caret->Reset();
	}
}

void RichEdit::RefreshScrollByCaret()
{
    if (m_nCaretInLine == 0)
    {
        SetVerticalValue(0);
    }
    else if (m_nCaretInLine == m_vLineItems.size() - 1)
    {
        SetVerticalValue(GetVerticalTotal());
    }
    else if (m_nCaretInLine > m_nRealEndIndex)
    {
        SetVerticalValue(m_nTopIndex + 1);
    }
    else if (m_nCaretInLine < m_nRealTopIndex)
    {
        SetVerticalValue(m_nTopIndex - 1);
    }
}

int RichEdit::InputKey(unsigned int ch)
{
	if (GuiUtil_ShiftPressed())
	{
		switch (ch)
		{
		case CGuiInput::KEY_LEFT:
			if (m_nCaretLinePos == 0)
			{
				return 0;
			}
            else
            {
                int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);
			    SetInputPos(real_pos - 1);
            }

            RefreshScrollByCaret();
			break;
		case CGuiInput::KEY_RIGHT:
			if (m_nCaretLinePos == GetSize())
			{
				return 0;
			}
            else
            {
                int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);
                SetInputPos(real_pos + 1);
            }

            RefreshScrollByCaret();
			break;
		case CGuiInput::KEY_HOME:
			if (m_nCaretLinePos == 0)
			{
				return 0;
			}
			
			SetInputPos(0);
            RefreshScrollByCaret();
			break;
		case CGuiInput::KEY_END:
			if (m_nCaretLinePos == GetSize())
			{
				return 0;
			}
			
			SetInputPos(GetSize());
            RefreshScrollByCaret();
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
				SetSelect(0, GetSize());
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
			if (m_nCaretInLine == 0 && m_nCaretLinePos == 0)
			{
				return 0;
			}
            else
            {
                int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);
                SetInputPos(real_pos - 1);
                RefreshScrollByCaret();
            }
			break;
		case CGuiInput::KEY_RIGHT:
			if (m_nCaretLinePos == GetSize())
			{
				return 0;
			}
            else
            {
                int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);
                SetInputPos(real_pos + 1);
                RefreshScrollByCaret();
            }

			break;
        case CGuiInput::KEY_UP:
            if (m_nCaretInLine <= 0)
            {
				// 返回值 改为1 因为有显示上条信息的功能处理
                return 1;
            }

            SetInputPos(m_nCaretInLine-1, m_nCaretLinePos);
            RefreshScrollByCaret();
            break;
        case CGuiInput::KEY_DOWN:
            if (m_nCaretInLine >= (int)(m_vLineItems.size()-1))
            {
				// 返回值 改为1 因为有显示下条信息的功能处理
                return 1;
            }

            SetInputPos(m_nCaretInLine+1, m_nCaretLinePos);
            RefreshScrollByCaret();
            break;
		case CGuiInput::KEY_HOME:
			if (m_nCaretLinePos == 0)
			{
				return 0;
			}

			SetInputPos(0);
            RefreshScrollByCaret();
			break;
		case CGuiInput::KEY_END:
			if (m_nCaretLinePos == GetSize())
			{
				return 0;
			}
			
			SetInputPos(GetSize());
            RefreshScrollByCaret();
			break;
		case CGuiInput::KEY_BACK:
            {
                if (HasSelectText())
			    {
				    if (!DeleteSelectText())
				    {
					    return 0;
				    }
			    }
                else
                {
                    if (m_nCaretInLine==0 && m_nCaretLinePos == 0)
                    {
                        return 0;
                    }

                    int pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

                    if (pos > 0)
                    {
                        if (!DeleteLeftText(pos - 1))
                        {
                            return 0;
                        }

                        SetInputPos(pos - 1);
                    }
                }


                RefreshScrollByCaret();
            }
			break;
		case CGuiInput::KEY_DELETE:
            {
                if (HasSelectText())
			    {
				    if (DeleteSelectText())
				    {
					    break;
				    }
			    }
    			
                int pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

			    if (!DeleteText(pos, 1))
			    {
				    return 0;
			    }
            }
			break;
		default:
			return 0;
		}
		
		ClearSelect();
	}

	return 1;
}

int RichEdit::GetCareRealPos(const int line, const int linepos)
{
    int pos = linepos;

    if (line >= 0 && line < (int)m_vLineItems.size())
    {
        pos = m_vLineItems[line].nStartPos + linepos;

        if (pos > GetSize())
        {
            return GetSize();
        }
    }
    else
    {
        return 0;
    }

    return pos;
}

// 尝试越南文组合字符输入
bool RichEdit::TryVietnamesInput(wchar_t ch)
{
    // 如果为越南文音调符，则检查当前位置的前面一个字符，如果是越南文单元单则添加，否则不处理
    if (FxLanguage::IsVietnameseVoiceLevel(ch))
    {
        // 如果当前输入框为空，则不处理
        if (m_Buffer.size() == 0)
        {
            return true;
        }

        int pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

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
                UpdateScroll();

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
bool RichEdit::TryThaiInput(wchar_t ch)
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

    int pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

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
            UpdateScroll();

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
            UpdateScroll();

            if (!GuiUtil_ShiftPressed())
            {
                ClearSelect();
            }
            return true;
        }
    }

    return true;
}

int RichEdit::InputChar(wchar_t ch)
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
        int real_pos = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

        wchar_t buff[2];
        buff[0] = ch;
        buff[1] = 0;

        int count = InsertText(real_pos, 
            buff, 1);

        if (0 == count)
        {
            return 0;
        }

        SetInputPos(real_pos + count);
    }
	
    if (!GuiUtil_ShiftPressed())
    {
        ClearSelect();
    }

	return 1;
}

// 事件
// 尺寸改变
int RichEdit::OnWidthChanged(int old)
{
	UpdateScroll();
	
    if (m_ViewRect.left == 0)
    {
        m_ViewRect.right = GetWidth();
    }

    return IScrollable::OnWidthChanged(old);
}

int RichEdit::OnHeightChanged(int old)
{
    if (m_ViewRect.top == 0)
    {
        m_ViewRect.bottom = GetHeight();
    }

    return IScrollable::OnHeightChanged(old);
}

int RichEdit::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
    // 通过y获取当前实际所有行
    int totoay = InnerGetAbsTop() + m_ViewRect.top;
    int line = 0;

    for (line = m_nRealTopIndex; line < (int)m_vLineItems.size(); ++line)
    {
        totoay += m_vLineItems[line].nLineHeight;

        if (y < totoay)
        {
            break;
        }
    }

    m_nCaretInLine = line;

    int start = 0;
    int end = 0;

    if (m_nCaretInLine < (int)m_vLineItems.size())
    {
        start = m_vLineItems[line].nStartPos;
        end = m_vLineItems[line].nEndPos;
    }
    else
    {
        SetInputPos(GetSize()-1);
        return 1;
    }

	int width = GetTotalWidth(m_Buffer, start, end - start + 1);
	
	int sx = x - (GetAbsLeft() + m_ViewRect.left);

	if (sx > width)
	{
		if (HasSelectText())
		{
			ClearSelect();
		}
		else
		{
			//SetSelect(GetSelectBeg(), end);
		}

        if (m_Buffer[m_vLineItems[line].nEndPos]->m_Type == PARTTYPE_CHAR)
        {
            HtmlTextItem* pTextItem = 
				(HtmlTextItem*)m_Buffer[m_vLineItems[line].nEndPos];
            
			if (wcsicmp(pTextItem->m_wtext.c_str(), NEW_LINE_FLAG) == 0)
            {
                // 如果行末是换行标志则移到标记前面
                if (m_vLineItems[line].nEndPos >= GetSize() - 1)
                {
                    SetInputPos(GetSize()-1);
                }
                else
                {
                    SetInputPos(m_vLineItems[line].nEndPos);
                }
            }
            else
            {
                SetInputPos(m_vLineItems[line].nEndPos+1);
            }
        }
		else
        {
            SetInputPos(m_vLineItems[line].nEndPos+1);
        }
	}
	else
	{
		if (HasSelectText())
		{
			ClearSelect();
		}
		
		int pos = GetCursorInPos(x, y, m_nCaretInLine, m_nCaretLinePos);

		if ((pos >= 0) && (pos < end - start + 1))
		{
			SetInputPos(start + pos);
		}
	}

	if (m_bDragEvent)
	{
		m_nDragOldX = x;
		m_nDragOldY = y;
	}

	return 1; 
}

int RichEdit::OnLButtonUp(int x, int y)
{ 
	return 1; 
}

int RichEdit::OnDragMove(int x, int y)
{
    int line,linepos;

	GetCursorInPos(x, y, line, linepos);

    int end = GetCareRealPos(line, linepos);

    int begin = GetCareRealPos(m_nCaretInLine, m_nCaretLinePos);

    if (end == begin)
    {
        return 0;
    }
    else if (end < begin)
    {
        int t = begin;
        begin = end;
        end = t;
    }

    SetSelect(begin,end);

	if (m_bDragEvent)
	{
	
		int drag_x = x - m_nDragOldX;
		int drag_y = y - m_nDragOldY;

		m_nDragOldX = x;
		m_nDragOldY = y;

		GuiUtil_RunCallback(this, "on_drag", 
			CVarList() << drag_x << drag_y);
	}
	
	return 1;
}

int RichEdit::OnKeyDown(unsigned int vk)
{
	if (vk == CGuiInput::KEY_TAB)
	{
		return 0;
	}
	
	if (vk == CGuiInput::KEY_RETURN)
	{
		// ctrl和enter同时按下有其他用处，所以不当做回调处理 by liujie 2011.12.6
		if (!GuiUtil_ControlPressed())
		{
			OnEnter();
			RememberInput();
		}
		return GuiUtil_RunCallback(this, "on_enter", CVarList());
	}

	if (vk == CGuiInput::KEY_UP)
	{
		if (m_nCaretInLine <= 0)
		{
			ShowLastText();
			//return 0;
		}
	}
	else if (vk == CGuiInput::KEY_DOWN)
	{
		if (m_nCaretInLine >= (int)(m_vLineItems.size()-1))
		{
			ShowNextText();
			//return 0;
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
        UpdateScroll();

        OnChanged(GetID());
	}
	
	return 1;
}

int RichEdit::OnKeyUp(unsigned int vk)
{
    // 吃掉UP消息
    return 1;
}

int RichEdit:: OnEnter()
{
    int cursorline = m_nCaretInLine;

    HtmlTextItem* pTextItem = CORE_NEW(HtmlTextItem);

    if (pTextItem)
    {
        pTextItem->m_Type = PARTTYPE_CHAR;
        pTextItem->m_wtext = NEW_LINE_FLAG;
        pTextItem->m_Width = NEW_LINE_FLAG_WIDTH;

        int insertpos = m_vLineItems[cursorline].nStartPos + m_nCaretLinePos;

        if (insertpos >= (int)m_Buffer.size())
        {
            m_Buffer.push_back(pTextItem);
        }
        else
        {
            m_Buffer.insert(m_Buffer.begin() + insertpos, pTextItem);
        }

        CalcVisText();

        // 行首
        SetInputPos(m_nCaretInLine + 1, 0);

        SetVerticalValue(m_nTopIndex + 1);
        UpdateScroll();
    }

    // 同步
    GenerateText(m_Buffer, 0, m_Buffer.size(), m_wsText);

    return 1;
}

//ch 为 Unicode
int RichEdit::OnChar(unsigned int ch)
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
        
        if (m_nCaretInLine > m_nRealEndIndex)
        {
            SetVerticalValue(m_nTopIndex + 1);
        }

		UpdateScroll();

        OnChanged(GetID());
	}

	//空隔键的回调
	if (ch == VK_SPACE)
	{
		GuiUtil_RunCallback(this, "on_space", CVarList());
	}

	return 1;
}


int RichEdit::OnGetFocus(const PERSISTID& lost_id)
{
	ResetCaret();

	SetInputPos(GetSize());

	GuiUtil_RunCallback(this, "on_get_focus", CVarList() << lost_id);
	
	return IScrollable::OnGetFocus(lost_id);
}

int RichEdit::OnLostFocus(const PERSISTID& get_id)
{
    if (get_id == GetVScrollBar())
    {
        GetGui()->SetFocused(this->GetID());
        return 0;
    }

	//ClearSelect();

	GuiUtil_RunCallback(this, "on_lost_focus", CVarList() << get_id);
	
	return IScrollable::OnLostFocus(get_id);
}

int RichEdit::OnChanged(const PERSISTID& changed_id)
{
    if (!m_bChangedEvent)
    {
        return 0;
    }

    GuiUtil_RunCallback(this, "on_changed", CVarList() << changed_id );

    return 1;
}

ImageAnimationManager* RichEdit::InnerGetAnimation()
{
	const IVar& var = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(var.ObjectVal());
	//return (ImageAnimationManager*)GetCore()->GetSingle(
	//    "ImageAnimationManager");
}

HyperLinkStyleManager* RichEdit::InnerGetHyperlinkMng()
{
	//return (HyperLinkStyleManager*)GetCore()->GetSingle(
	//     "HyperLinkStyleManager");
	const IVar& var = GetCore()->GetGlobalValue("HyperLinkStyleManager");
	return (HyperLinkStyleManager*)GetCore()->GetEntity(var.ObjectVal());
}



// 记录输入文字
bool RichEdit::RememberInput()
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
	return true;
}

bool RichEdit::ShowLastText()
{
	if (m_bRemember)
	{
		if (m_vRemembers.size() > 0)
		{
			if (m_nCurRemember == -1)
			{
				m_nCurRemember = m_vRemembers.size();
			}
		}else
		{
			return false;
		}

		if (m_nCurRemember > 0)
		{
			m_nCurRemember--;

			SetText(m_vRemembers[m_nCurRemember].c_str());
		}
	}
	return true;
}

bool RichEdit::ShowNextText()
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
	return true;
}