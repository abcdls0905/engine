//--------------------------------------------------------------------
// 文件名:        button.cpp
// 内  容:        
// 说  明:        
// 创建日期:    2007年3月14日
// 创建人:        陆利民
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "button.h"
#include "gui_input.h"

/// \file button.cpp
/// \brief 按钮

/// entity: Button
/// \brief 按钮控件
/// \see 继承自IControl
DECLARE_ENTITY(Button, 0, IControl);

/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(const char*, Button, Align, GetAlignString, SetAlignString);

/// property: bool HoverRepeat
/// \brief 当鼠标持续按住时是否产生多个回调
DECLARE_PROPERTY(bool, Button, HoverRepeat, GetHoverRepeat, SetHoverRepeat);
/// property: bool PushEvent
/// \brief 鼠标按下时是否产生事件
DECLARE_PROPERTY(bool, Button, PushEvent, GetPushEvent, SetPushEvent);

/// property: bool OutUpEvent
/// \brief 鼠标在控件上按下并移出控件后放开时是否产生事件
DECLARE_PROPERTY(bool, Button, OutUpEvent, GetOutUpEvent, SetOutUpEvent);

/// property: bool DragEvent
/// \brief 鼠标拖拽，是否产生点击事件
DECLARE_PROPERTY(bool, Button, DragEvent, GetDragEvent, SetDragEvent);

/// property: string Style
/// \brief 显示风格
DECLARE_PROPERTY(const char*, Button, Style, GetStyleString, SetStyleString);
/// property: string NormalImage
/// \brief 正常状态的图元
DECLARE_PROPERTY(const char*, Button, NormalImage, GetNormalImage, 
    SetNormalImage);
/// property: string FocusImage
/// \brief 鼠标选中状态的图元
DECLARE_PROPERTY(const char*, Button, FocusImage, GetFocusImage, 
    SetFocusImage);
/// property: string PushImage
/// \brief 被按下状态的图元
DECLARE_PROPERTY(const char*, Button, PushImage, GetPushImage, SetPushImage);
/// property: string DisableImage
/// \brief 被禁止操作状态的图元
DECLARE_PROPERTY(const char*, Button, DisableImage, GetDisableImage, 
    SetDisableImage);

/// property: string FocusBlendColor
/// \brief 鼠标移入状态的混合颜色
DECLARE_PROPERTY(result_string, Button, FocusBlendColor, 
	GetFocusBlendColorString, SetFocusBlendColorString);
/// property: string PushBlendColor
/// \brief 被按下状态的混合颜色
DECLARE_PROPERTY(result_string, Button, PushBlendColor, 
	GetPushBlendColorString, SetPushBlendColorString);
/// property: string DisableBlendColor
/// \brief 被禁止操作状态的混合颜色
DECLARE_PROPERTY(result_string, Button, DisableBlendColor, 
	GetDisableBlendColorString, SetDisableBlendColorString);

/// property: string NormalColor
/// \brief 正常状态的颜色
DECLARE_PROPERTY(result_string, Button, NormalColor, GetNormalColorString, 
    SetNormalColorString);
/// property: string FocusColor
/// \brief 鼠标选中状态的颜色
DECLARE_PROPERTY(result_string, Button, FocusColor, GetFocusColorString, 
    SetFocusColorString);
/// property: string PushColor
/// \brief 被按下状态的颜色
DECLARE_PROPERTY(result_string, Button, PushColor, GetPushColorString, 
    SetPushColorString);
/// property: string DisableColor
/// \brief 被禁止操作状态的颜色
DECLARE_PROPERTY(result_string, Button, DisableColor, GetDisableColorString, 
    SetDisableColorString);

/// property: string NormalFont
/// \brief 正常状态的字体
DECLARE_PROPERTY(const char*, Button, NormalFont, GetNormalFontName, 
	SetNormalFontName);
/// property: string FocusFont
/// \brief 鼠标选中状态的字体
DECLARE_PROPERTY(const char*, Button, FocusFont, GetFocusFontName, 
	SetFocusFontName);
/// property: string PushFont
/// \brief 被按下状态的字体
DECLARE_PROPERTY(const char*, Button, PushFont, GetPushFontName, 
	SetPushFontName);
/// property: string DisableFont
/// \brief 被禁止操作状态的字体
DECLARE_PROPERTY(const char*, Button, DisableFont, GetDisableFontName, 
	SetDisableFontName);

/// event: int on_click(object self, int mouse_button)
/// \brief 鼠标点击的回调
/// \param self 本控件
/// \param mouse_button 鼠标按钮（0-左键，1-中键，2-右键）
DECLARE_EVENT(on_click);
/// event: int on_push(object self, int offset_x, int offset_y)
/// \brief 鼠标按下的回调
/// \param self 本控件
/// \param offset_x X偏移
/// \param offset_y Y偏移
DECLARE_EVENT(on_push);

/// event: int on_drag(object self, int drag_x, int drag_y)
/// \brief 鼠标按住拖动的回调
/// \param self 本控件
/// \param drag_x X方向的拖动距离
/// \param drag_y Y方向的拖动距离
DECLARE_EVENT(on_drag_move);

// Button

Button::Button()
{
    m_nAlign = CGuiConst::ALIGN_CENTER;
    m_bPush = false;
    m_bHoverRepeat = false;
    m_bPushEvent = false;
    m_bDragEvent = false;
    m_bOutUpEvent = false;
    m_nStyle = 0;
    m_nClickX = 0;
    m_nClickY = 0;

    m_nDragOldX = 0;
    m_nDragOldY = 0;
    
    SetDesignSize(80, 24);
    SetCanFocus(true);
    SetTabStop(false);
    SetBackColor(CGuiConst::BUTTON_COLOR);

	m_FocusBlendColor.SetValue(CGuiConst::WHITE_COLOR);
	m_PushBlendColor.SetValue(CGuiConst::WHITE_COLOR);
	m_DisableBlendColor.SetValue(CGuiConst::WHITE_COLOR);

    m_NormalColor.SetValue(0);
    m_FocusColor.SetValue(0);
    m_PushColor.SetValue(0);
    m_DisableColor.SetValue(0);

    m_strNormalFontName = "";
    m_strFocusFontName = "";
    m_strPushFontName = "";
    m_strDisableFontName = "";
}

Button::~Button()
{
}

bool Button::Init(const IVarList& args)
{
    if (!IControl::Init(args))
    {
        return false;
    }

    return true;
}

bool Button::Shut()
{
    m_FocusImage.Release();
    m_PushImage.Release();
    m_DisableImage.Release();

    return IControl::Shut();
}

// 对齐方式
void Button::SetAlignString(const char* value)
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

const char* Button::GetAlignString() const
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

void Button::SetHoverRepeat(bool value)
{
    m_bHoverRepeat = value;
}

bool Button::GetHoverRepeat() const
{
    return m_bHoverRepeat;
}

void Button::SetPushEvent(bool value)
{
    m_bPushEvent = value;
}

bool Button::GetPushEvent() const
{
    return m_bPushEvent;
}

void Button::SetOutUpEvent(bool value)
{
    m_bOutUpEvent = value;
}

bool Button::GetOutUpEvent() const
{
    return m_bOutUpEvent;
}

void Button::SetDragEvent(bool value)
{
    m_bDragEvent = value;
}

bool Button::GetDragEvent() const
{
    return m_bDragEvent;
}

void Button::SetStyleString(const char* value)
{
    Assert(value != NULL);

    if (stricmp(value, "UpArrow") == 0)
    {
        m_nStyle = CGuiConst::ARROW_UP;
    }
    else if (stricmp(value, "DownArrow") == 0)
    {
        m_nStyle = CGuiConst::ARROW_DOWN;
    }
    else if (stricmp(value, "LeftArrow") == 0)
    {
        m_nStyle = CGuiConst::ARROW_LEFT;
    }
    else if (stricmp(value, "RightArrow") == 0)
    {
        m_nStyle = CGuiConst::ARROW_RIGHT;
    }
    else
    {
        m_nStyle = 0;
    }
}

const char* Button::GetStyleString() const
{
    switch (m_nStyle)
    {
    case CGuiConst::ARROW_UP:
        return "UpArrow";
    case CGuiConst::ARROW_DOWN:
        return "DownArrow";
    case CGuiConst::ARROW_LEFT:
        return "LeftArrow";
    case CGuiConst::ARROW_RIGHT:
        return "RightArrow";
    default:
        break;
    }

    return "";
}

void Button::SetNormalImage(const char* value) 
{ 
    SetBackImage(value);
}

const char* Button::GetNormalImage() const 
{ 
    return GetBackImage();
}

void Button::SetFocusImage(const char* value) 
{ 
    m_FocusImage.SetName(value);
    m_FocusImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Button::GetFocusImage() const 
{ 
    return m_FocusImage.GetName();
}

void Button::SetPushImage(const char* value) 
{ 
    m_PushImage.SetName(value);
    m_PushImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Button::GetPushImage() const 
{ 
    return m_PushImage.GetName();
}

void Button::SetDisableImage(const char* value) 
{ 
    m_DisableImage.SetName(value);
    m_DisableImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* Button::GetDisableImage() const 
{ 
    return m_DisableImage.GetName();
}

result_string Button::GetNormalColorString() const
{
    return m_NormalColor.GetName().c_str();
}

void Button::SetNormalColorString(const char* val)
{
    Assert(val != NULL);

    Gui* pGui = GetGui();
    Assert(pGui != NULL);

    m_NormalColor.SetName(pGui, val);
}

result_string Button::GetFocusColorString() const
{
    return m_FocusColor.GetName().c_str();
}

void Button::SetFocusColorString(const char* val)
{
    Assert(val != NULL);

    Gui* pGui = GetGui();
    Assert(pGui != NULL);

    m_FocusColor.SetName(pGui, val);
}

result_string Button::GetPushColorString() const
{
    return m_PushColor.GetName().c_str();
}

void Button::SetPushColorString(const char* val)
{
    Assert(val != NULL);

    Gui* pGui = GetGui();
    Assert(pGui != NULL);

    m_PushColor.SetName(pGui, val);
}

result_string Button::GetDisableColorString() const
{
    return m_DisableColor.GetName().c_str();
}

void Button::SetDisableColorString(const char* val)
{
    Assert(val != NULL);

    Gui* pGui = GetGui();
    Assert(pGui != NULL);

    m_DisableColor.SetName(pGui, val);
}

void Button::SetNormalFontName(const char* val)
{
    Assert(val != NULL);

    m_strNormalFontName = val;
}

const char* Button::GetNormalFontName() const
{
    return m_strNormalFontName.c_str();
}

void Button::SetFocusFontName(const char* val)
{
    Assert(val != NULL);

    m_strFocusFontName = val;
}

const char* Button::GetFocusFontName() const
{
    return m_strFocusFontName.c_str();
}

void Button::SetPushFontName(const char* val)
{
    Assert(val != NULL);

    m_strPushFontName = val;
}

const char* Button::GetPushFontName() const
{
    return m_strPushFontName.c_str();
}

void Button::SetDisableFontName(const char* val)
{
    Assert(val != NULL);

    m_strDisableFontName = val;
}

const char* Button::GetDisableFontName() const
{
    return m_strDisableFontName.c_str();
}

void Button::SetPressed(bool bValue)
{
    m_bPush = bValue;
}

// 鼠标选中状态的颜色字符串
void Button::SetFocusBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_FocusBlendColor.SetName(pGui, val);
}

result_string Button::GetFocusBlendColorString() const
{
	return m_FocusBlendColor.GetName().c_str();
}

void Button::SetPushBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_PushBlendColor.SetName(pGui, val);
}

result_string Button::GetPushBlendColorString() const
{
	return m_PushBlendColor.GetName().c_str();
}

void Button::SetDisableBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_DisableBlendColor.SetName(pGui, val);
}

result_string Button::GetDisableBlendColorString() const
{
	return m_DisableBlendColor.GetName().c_str();
}

void Button::PrepareDesignInfo(IDesignInfo* di)
{
    IControl::PrepareDesignInfo(di);

    CVar v(VTYPE_STRING, "Center");
    di->RemovePropInfo("BackImage");

    di->RemovePropInfo("Width");
    di->RemovePropInfo("Height");
    di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, 
        true, true,    &v);
    di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("HoverRepeat", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("PushEvent", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("OutUpEvent", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("DragEvent", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("Style", IDesignInfo::TYPE_STRING, true, false);
    di->AddPropInfo("NormalImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("FocusImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("PushImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("DisableImage", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("Icon", IDesignInfo::TYPE_STRING, true, true);

	di->AddPropInfo("FocusBlendColor", IDesignInfo::TYPE_COLOR, true, true);
	di->AddPropInfo("PushBlendColor", IDesignInfo::TYPE_COLOR, true, true);
	di->AddPropInfo("DisableBlendColor", IDesignInfo::TYPE_COLOR, true, true);

    di->AddPropInfo("NormalColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("FocusColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("PushColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("DisableColor", IDesignInfo::TYPE_COLOR, true, true);

    di->AddPropInfo("NormalFont", IDesignInfo::TYPE_FONT, true, true);
    di->AddPropInfo("FocusFont", IDesignInfo::TYPE_FONT, true, true);
    di->AddPropInfo("PushFont", IDesignInfo::TYPE_FONT, true, true);
    di->AddPropInfo("DisableFont", IDesignInfo::TYPE_FONT, true, true);
    
    di->AddEventInfo("on_click");
    di->AddEventInfo("on_push");
    di->AddEventInfo("on_drag_move");
}

bool Button::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

    if (!m_FocusImage.IsLoadCompelte())
    {
        return false;
    }

    if (!m_PushImage.IsLoadCompelte())
    {
        return false;
    }

    if (!m_DisableImage.IsLoadCompelte())
    {
        return false;
    }

    return IControl::GetLoadFinish();
}

void Button::Paint(float seconds)
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
    
    if (!GetEnabled())
    {
        if (m_DisableImage.IsReady())
        {
			draw_image = m_DisableImage.Draw(GetDrawMode(), x1, y1, x2, y2, 
				m_DisableBlendColor.GetValue());
        }
    }
    else if (GetPressed())
    {
        if (m_PushImage.IsReady())
        {
            draw_image = m_PushImage.Draw(GetDrawMode(), x1, y1, x2, y2, 
				m_PushBlendColor.GetValue());
        }
    }
    else if (GetCapture())
    {
        if (m_FocusImage.IsReady())
        {
            draw_image = m_FocusImage.Draw(GetDrawMode(), x1, y1, x2, y2, 
				m_FocusBlendColor.GetValue());
        }
    }
    
    if (!draw_image)
    {
        if (HasBackImage())
        {
            draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
        }
    }

    // 是否传统按钮状
    bool button_face = (GetLineColor() != 0);
    
    if ((!draw_image) && button_face)
    {
        if (GetPressed())
        {
            //GetPainter()->FillRect(x1, y1, x2, y2, CGuiConst::BUTTON_COLOR);
            GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
            GetPainter()->DrawRect(x1, y1, x2, y2, CGuiConst::BLACK_COLOR);
        }
        else
        {
            //GuiUtil_PaintButtonFace(GetPainter(), x1, y1, x2, y2);
            GuiUtil_PaintButtonFaceColor(GetPainter(), GetBackColor(), 
                CGuiConst::WHITE_COLOR, x1, y1, x2, y2);
        }
        
        if (m_nStyle != CGuiConst::ARROW_NONE)
        {
            GuiUtil_PaintArrow(GetPainter(), m_nStyle, x1, y1, 
                GetWidth(), GetHeight(), GetForeColor());
        }
    }

    const wchar_t* text = GetShowText();
    
    if (!WideStrEmpty(text))
    {
        // 根据按扭状态用不一样的颜色
        unsigned int color = GetForeColor();
        // 根据按扭状态使用不一样的字体
        result_string strFont = GuiUtil_GetValidFontName(this);

        if (!GetEnabled())
        {
            if (m_DisableColor.GetValue() > 0)
            {
                color = m_DisableColor.GetValue();
            }
            else
            {
                color = CGuiConst::DISABLE_COLOR;
            }

            if (m_strDisableFontName != "")
            {
                strFont = m_strDisableFontName.c_str();
            }
        }
        else if (GetPressed())
        {
            if (m_PushColor.GetValue() > 0)
            {
                color = m_PushColor.GetValue();
            }

            if (m_strPushFontName != "")
            {
                strFont = m_strPushFontName.c_str();
            }
        }
        else if (GetCapture())
        {
            if (m_FocusColor.GetValue() > 0)
            {
                color = m_FocusColor.GetValue();
            }
            else if (!button_face)
            {
                color = GetBackColor();
            }

            if (m_strFocusFontName != "")
            {
                strFont = m_strFocusFontName.c_str();
            }
        }
        else
        {
            if (m_NormalColor.GetValue() > 0)
            {
                color = m_NormalColor.GetValue();
            }

            if (m_strNormalFontName != "")
            {
                strFont = m_strNormalFontName.c_str();
            }
        }

        GetGui()->SetFont(strFont.c_str(), GetShadowColor());

        wchar_t buffer[256] = { 0 };
        const wchar_t* val = GuiUtil_GetCanShowText(GetPainter(),
            text, GetWidth(), L"...", buffer, sizeof(buffer));
        
        switch (m_nAlign)
        {
        case CGuiConst::ALIGN_LEFT:
            GetPainter()->WriteTextLeft(x1, y1, x2, y2, color, val);
            break;
        case CGuiConst::ALIGN_CENTER:
            GetPainter()->WriteTextCenter(x1, y1, x2, y2, color, val);
            break;
        case CGuiConst::ALIGN_RIGHT:
            GetPainter()->WriteTextRight(x1, y1, x2, y2, color, val);
            break;
        default:
            Assert(0);
            break;
        }
        //GetPainter()->WriteTextCenter(x1, y1, x2, y2, color, val);
    }
    
    if ((!draw_image) && GetFocused() && (GetDelegateControl() == NULL))
    {
        GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
    }
}

int Button::OnKeyDown(unsigned int vk)
{
    if (vk != CGuiInput::KEY_RETURN)
    {
        return 0;
    }
    
    if (!GetEnabled())
    {
        return 0;
    }

    if (!GetVisible())
    {
        return 0;
    }
    
    int res = GuiUtil_RunCallback(this, "on_click", CVarList());
    
    if (GetDelegateControl() != NULL)
    {
        GetDelegateControl()->OnSuborEvent(this, CGuiConst::EVENT_CLICK, 
            CVarList());
    }

    return res;
}

int Button::OnLButtonDown(int x, int y)
{ 
    if (!GetEnabled())
    {
        return 0;
    }
    
    m_nClickX = x - InnerGetAbsLeft();
    m_nClickY = y - InnerGetAbsTop();
    
    Assert( (m_nClickX >= 0) && (m_nClickX < GetWidth()) );
    Assert( (m_nClickY >= 0) && (m_nClickY < GetHeight()) );

    if (!m_bPush)
    {
        m_bPush = true;

        if (m_bPushEvent)
        {
            // 产生按下事件
            GuiUtil_RunCallback(this, "on_push", 
                CVarList() << m_nClickX << m_nClickY);
        }

        if (GetClickSound() != NULL)
        {
            // 播放音效
            GetGui()->PlayingSound(GetClickSound());
        }
    }

    if (m_bHoverRepeat)
    {
        // 产生持续按下消息
        //GetGui()->GenMouseHoverMessage(200);
    }
    
    if (m_bDragEvent)
    {
        m_nDragOldX = x;
        m_nDragOldY = y;
    }

    return 1; 
}

int Button::OnLButtonUp(int x, int y)
{ 
    if (!GetEnabled())
    {
        return 0;
    }
    
    if (GetPressed())
    {
        //这边如果不在按钮上,就表明是在按钮上按下后移开的,也应向应消息
        if (m_bOutUpEvent || InRectangle(x, y))
        {
            GuiUtil_RunCallback(this, "on_click", 
                CVarList() << CGuiConst::MOUSE_LEFT);
            
            if (GetDelegateControl() != NULL)
            {
                GetDelegateControl()->OnSuborEvent(this, 
                    CGuiConst::EVENT_CLICK, 
                    CVarList() << CGuiConst::MOUSE_LEFT);
            }
        }
        
        if (GetDelegateControl() != NULL)
        {
            GetDelegateControl()->OnSuborEvent(this, 
                CGuiConst::EVENT_DRAG_LEAVE, 
                CVarList() << m_nClickX << m_nClickY << x << y);
        }
    }

    if (m_bPush)
    {
        m_bPush = false;
    }
    
    return 1; 
}

int Button::OnLButtonDoubleClick(int x, int y)
{ 
    return IControl::OnLButtonDoubleClick(x, y);
    // return OnLButtonDown(x, y);
}

int Button::OnLButtonHover(int x, int y)
{
    if (!GetEnabled())
    {
        return 0;
    }
    
    if (GetPressed())
    {
        if (InRectangle(x, y))
        {
            GuiUtil_RunCallback(this, "on_click", 
                CVarList() << CGuiConst::MOUSE_LEFT);
            
            if (GetDelegateControl() != NULL)
            {
                GetDelegateControl()->OnSuborEvent(this, 
                    CGuiConst::EVENT_CLICK, 
                    CVarList() << CGuiConst::MOUSE_LEFT);
            }
        }

        
        if (m_bHoverRepeat)
        {
            // 产生下一个持续按下消息
            //GetGui()->GenMouseHoverMessage(50);
        }
    }
    
    return 1;
}

int Button::OnDragMove(int x, int y)
{
    if (GetDelegateControl() != NULL)
    {
        return GetDelegateControl()->OnSuborEvent(this, 
            CGuiConst::EVENT_DRAG_MOVE, 
            CVarList() << m_nClickX << m_nClickY << x << y);
    }
    
    if (m_bDragEvent)
    {

        int drag_x = x - m_nDragOldX;
        int drag_y = y - m_nDragOldY;

        m_nDragOldX = x;
        m_nDragOldY = y;

        GuiUtil_RunCallback(this, "on_drag_move", 
            CVarList() << drag_x << drag_y);
    }

    return 0;
}

// 判断指定位置是否透明
bool Button::TestTransparent(int x, int y)
{
    if (!GetTestTrans())
    {
        // 不进行透明测试，则返回不透明
        return false;
    }

    CImagePlayer* pImage = NULL;

    if (!GetEnabled())
    {
        if (m_DisableImage.IsReady())
        {
            pImage = &m_DisableImage;
        }
    }
    else if (GetPressed())
    {
        if (m_PushImage.IsReady())
        {
            pImage = &m_PushImage;
        }
    }
    else if (GetCapture())
    {
        if (m_FocusImage.IsReady())
        {
            pImage = &m_FocusImage;
        }
    }

    if (!pImage)
    {
        if (HasBackImage())
        {
            pImage = &InnerGetBackImage();
        }
    }

    if (pImage == NULL)
    {
        // 如果无图片，则认为不透明
        return false;
    }

    int x1 = InnerGetAbsLeft();
    int y1 = InnerGetAbsTop();
    // 算出不放缩情况下的相对控件的鼠标偏移
    int offset_x = x - x1;
    int offset_y = y - y1;
    int alpha;

    if (!pImage->GetAlpha(GetDrawMode(), 0, 0, GetWidth(), GetHeight(), 
        offset_x, offset_y, alpha))
    {
        return true;
    }

    return (alpha == 0);
}
