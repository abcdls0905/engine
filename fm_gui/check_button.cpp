//--------------------------------------------------------------------
// 文件名:		check_button.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "check_button.h"
#include "gui_input.h"

/// \file check_button.cpp
/// \brief 复选框控件

/// entity: CheckButton
/// \brief 复选框控件实体
DECLARE_ENTITY(CheckButton, 0, IControl);

/// property: bool Checked
/// \brief 是否被选中
DECLARE_PROPERTY(bool, CheckButton, Checked, GetChecked, SetChecked);
/// property: bool HideBox
/// \brief 是否隐藏选择框
DECLARE_PROPERTY(bool, CheckButton, HideBox, GetHideBox, SetHideBox);
/// property: bool ClickEvent
/// \brief 是否产生点击事件
DECLARE_PROPERTY(bool, CheckButton, ClickEvent, GetClickEvent, SetClickEvent);
/// property: bool BoxSize
/// \brief 选择框的尺寸
DECLARE_PROPERTY(int, CheckButton, BoxSize, GetBoxSize, SetBoxSize);
/// property: string NormalImage
/// \brief 正常状态的图元
DECLARE_PROPERTY(const char*, CheckButton, NormalImage, GetNormalImage, 
	SetNormalImage);
/// property: string FocusImage
/// \brief 鼠标选中状态的图元
DECLARE_PROPERTY(const char*, CheckButton, FocusImage, GetFocusImage, 
	SetFocusImage);
/// property: string CheckedImage
/// \brief 被选中状态的图元
DECLARE_PROPERTY(const char*, CheckButton, CheckedImage, GetCheckedImage, 
	SetCheckedImage);
/// property: string DisableImage
/// \brief 被禁止操作状态的图元
DECLARE_PROPERTY(const char*, CheckButton, DisableImage, GetDisableImage, 
	SetDisableImage);

/// property: string NormalColor
/// \brief 正常状态的颜色
DECLARE_PROPERTY(result_string, CheckButton, NormalColor, GetNormalColorString, 
	SetNormalColorString);
/// property: string FocusColor
/// \brief 鼠标选中状态的颜色
DECLARE_PROPERTY(result_string, CheckButton, FocusColor, GetFocusColorString, 
	SetFocusColorString);
/// property: string PushColor
/// \brief 被按下状态的颜色
DECLARE_PROPERTY(result_string, CheckButton, PushColor, GetPushColorString, 
	SetPushColorString);
/// property: string DisableColor
/// \brief 被禁止操作状态的颜色
DECLARE_PROPERTY(result_string, CheckButton, DisableColor, GetDisableColorString, 
	SetDisableColorString);

/// property: string FocusBlendColor
/// \brief 鼠标移入状态的混合颜色
DECLARE_PROPERTY(result_string, CheckButton, FocusBlendColor, GetFocusBlendColorString, 
SetFocusBlendColorString);
/// property: string PushBlendColor
/// \brief 被按下状态的混合颜色
DECLARE_PROPERTY(result_string, CheckButton, PushBlendColor, GetPushBlendColorString, 
SetPushBlendColorString);
/// property: string DisableBlendColor
/// \brief 被禁止操作状态的混合颜色
DECLARE_PROPERTY(result_string, CheckButton, DisableBlendColor, GetDisableBlendColorString, 
SetDisableBlendColorString);

/// property: string NormalFont
/// \brief 正常状态的字体
DECLARE_PROPERTY(const char*, CheckButton, NormalFont, GetNormalFontName, SetNormalFontName);
/// property: string FocusFont
/// \brief 鼠标选中状态的字体
DECLARE_PROPERTY(const char*, CheckButton, FocusFont, GetFocusFontName, SetFocusFontName);
/// property: string PushFont
/// \brief 被按下状态的字体
DECLARE_PROPERTY(const char*, CheckButton, PushFont, GetPushFontName, SetPushFontName);
/// property: string DisableFont
/// \brief 被禁止操作状态的字体
DECLARE_PROPERTY(const char*, CheckButton, DisableFont, GetDisableFontName, SetDisableFontName);

/// event: int on_checked_changed(object self)
/// \brief 选择状态改变的回调
/// \param self 本控件
DECLARE_EVENT(on_checked_changed);
/// event: int on_click(object self)
/// \brief 被点击的回调
/// \param self 本控件
DECLARE_EVENT(on_click);

// CheckButton

CheckButton::CheckButton()
{
	m_bChecked = false;
	m_bHideBox = false;
	m_bClickEvent = false;
	m_bPush = false;
	m_nBoxSize = 12;
	m_nClickX = 0;
	m_nClickY = 0;
	
	SetDesignSize(80, 24);
	SetCanFocus(true);
	SetTabStop(true);
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

CheckButton::~CheckButton()
{
}

bool CheckButton::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	return true;
}

bool CheckButton::Shut()
{
	m_FocusImage.Release();
	m_CheckedImage.Release();
	m_DisableImage.Release();
	
	return IControl::Shut();
}

bool CheckButton::SetCheckValue(bool value)
{
	if (value == m_bChecked)
	{
		return false;
	}
	
	ApplyCheckValue(value);

	return true;
}

bool CheckButton::ApplyCheckValue(bool value)
{
	m_bChecked = value;
	
	if (GetEnabled())
	{
		GuiUtil_RunCallback(this, "on_checked_changed", CVarList());
	}

	return true;
}

void CheckButton::SetChecked(bool value) 
{ 
	SetCheckValue(value);
}

bool CheckButton::GetChecked() const 
{ 
	return m_bChecked; 
}

void CheckButton::SetHideBox(bool value)
{
	m_bHideBox = value;
}

bool CheckButton::GetHideBox() const
{
	return m_bHideBox;
}

void CheckButton::SetClickEvent(bool value)
{
	m_bClickEvent = value;
}

bool CheckButton::GetClickEvent() const
{
	return m_bClickEvent;
}

void CheckButton::SetBoxSize(int value)
{
	if (value < MIN_BOX_SIZE)
	{
		value = MIN_BOX_SIZE;
	}

	m_nBoxSize = value;
}

int CheckButton::GetBoxSize() const
{
	return m_nBoxSize;
}

void CheckButton::SetNormalImage(const char* value) 
{ 
	SetBackImage(value);
}

const char* CheckButton::GetNormalImage() const 
{ 
	return GetBackImage();
}

void CheckButton::SetFocusImage(const char* value) 
{ 
	m_FocusImage.SetName(value);
	m_FocusImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* CheckButton::GetFocusImage() const 
{ 
	return m_FocusImage.GetName();
}

void CheckButton::SetCheckedImage(const char* value) 
{ 
	m_CheckedImage.SetName(value);
	m_CheckedImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* CheckButton::GetCheckedImage() const 
{ 
	return m_CheckedImage.GetName();
}

void CheckButton::SetDisableImage(const char* value) 
{ 
	m_DisableImage.SetName(value);
	m_DisableImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* CheckButton::GetDisableImage() const 
{ 
	return m_DisableImage.GetName();
}

result_string CheckButton::GetNormalColorString() const
{
	return m_NormalColor.GetName().c_str();
}

void CheckButton::SetNormalColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_NormalColor.SetName(pGui, val);
}

result_string CheckButton::GetFocusColorString() const
{
	return m_FocusColor.GetName().c_str();
}

void CheckButton::SetFocusColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_FocusColor.SetName(pGui, val);
}

result_string CheckButton::GetPushColorString() const
{
	return m_PushColor.GetName().c_str();
}

void CheckButton::SetPushColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_PushColor.SetName(pGui, val);
}

result_string CheckButton::GetDisableColorString() const
{
	return m_DisableColor.GetName().c_str();
}

void CheckButton::SetDisableColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_DisableColor.SetName(pGui, val);
}

void CheckButton::SetNormalFontName(const char* val)
{
	Assert(val != NULL);

	m_strNormalFontName = val;
}

const char* CheckButton::GetNormalFontName() const
{
	return m_strNormalFontName.c_str();
}

void CheckButton::SetFocusFontName(const char* val)
{
	Assert(val != NULL);

	m_strFocusFontName = val;
}

const char* CheckButton::GetFocusFontName() const
{
	return m_strFocusFontName.c_str();
}

void CheckButton::SetPushFontName(const char* val)
{
	Assert(val != NULL);

	m_strPushFontName = val;
}

const char* CheckButton::GetPushFontName() const
{
	return m_strPushFontName.c_str();
}

void CheckButton::SetDisableFontName(const char* val)
{
	Assert(val != NULL);

	m_strDisableFontName = val;
}

const char* CheckButton::GetDisableFontName() const
{
	return m_strDisableFontName.c_str();
}

void CheckButton::SetFocusBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_FocusBlendColor.SetName(pGui, val);
}

result_string CheckButton::GetFocusBlendColorString() const
{
	return m_FocusBlendColor.GetName().c_str();
}

void CheckButton::SetPushBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_PushBlendColor.SetName(pGui, val);
}

result_string CheckButton::GetPushBlendColorString() const
{
	return m_PushBlendColor.GetName().c_str();
}

void CheckButton::SetDisableBlendColorString(const char* val)
{
	Assert(val != NULL);

	Gui* pGui = GetGui();
	Assert(pGui != NULL);

	m_DisableBlendColor.SetName(pGui, val);
}

result_string CheckButton::GetDisableBlendColorString() const
{
	return m_DisableBlendColor.GetName().c_str();
}

void CheckButton::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);
	
	di->RemovePropInfo("BackImage");
	
	di->AddPropInfo("TabIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("TabStop", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("Checked", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("HideBox", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("ClickEvent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("BoxSize", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("NormalImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("FocusImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("CheckedImage", IDesignInfo::TYPE_IMAGE, true, true);
	di->AddPropInfo("DisableImage", IDesignInfo::TYPE_IMAGE, true, true);

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

	di->AddPropInfo("Icon", IDesignInfo::TYPE_STRING, true, true);
	
	di->AddEventInfo("on_checked_changed");
	di->AddEventInfo("on_click");
}

bool CheckButton::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_FocusImage.IsLoadCompelte())
	{
		return false;
	}

	if (!m_CheckedImage.IsLoadCompelte())
	{
		return false;
	}

	if (!m_DisableImage.IsLoadCompelte())
	{
		return false;
	}

	return IControl::GetLoadFinish();
}

void CheckButton::Paint(float seconds)
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
			draw_image = m_DisableImage.Draw(GetDrawMode(), x1, y1, x2, y2, m_DisableBlendColor.GetValue());
		}
	}
	else if (m_bPush || m_bChecked)
	{
		if (m_CheckedImage.IsReady())
		{
			draw_image = m_CheckedImage.Draw(GetDrawMode(), x1, y1, x2, y2, m_PushBlendColor.GetValue());
		}
	}
	else if (GetCapture())
	{
		if (m_FocusImage.IsReady())
		{
			draw_image = m_FocusImage.Draw(GetDrawMode(), x1, y1, x2, y2, m_FocusBlendColor.GetValue());
		}
	}
	
	if (!draw_image)
	{
		if (HasBackImage())
		{
			draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
		}
	}
	
	if (!draw_image)
	{
		if (m_bHideBox)
		{
			if (GetPressed())
			{
				GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
				GetPainter()->DrawRect(x1, y1, x2, y2, 
					CGuiConst::BLACK_COLOR);
			}
			else
			{
				GuiUtil_PaintButtonFaceColor(GetPainter(), GetBackColor(), 
					CGuiConst::WHITE_COLOR, x1, y1, x2, y2);
			}
		}
		else
		{
			const int BOX_SIZE = GetBoxSize();
			
			int x3 = x1;
			int y3 = y1 + (GetHeight() - BOX_SIZE) / 2;
			int x4 = x3 + BOX_SIZE;
			int y4 = y3 + BOX_SIZE;
			
			GetPainter()->FillRect(x3, y3, x4, y4, GetBackColor());
			GetPainter()->DrawRect(x3, y3, x4, y4, GetLineColor());

			if (GetPressed())
			{
				int x5 = x4 - 1;
				int y5 = y4 - 1;
				
				GetPainter()->DrawLine(x3, y3, x5, y5, GetLineColor());
				GetPainter()->DrawLine(x3, y5, x5, y3, GetLineColor());
			}

			x1 += BOX_SIZE + 2;
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
		
		if (draw_image || m_bHideBox)
		{
			GetPainter()->WriteTextCenter(x1, y1, x2, y2, color, text);
		}
		else
		{	
			GetPainter()->WriteTextLeft(x1, y1, x2, y2, color, text);
		}
	}
	
	if ((!draw_image) && GetFocused() && (GetDelegateControl() == NULL))
	{
		GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
	}
}

int CheckButton::OnKeyDown(unsigned int vk)
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
	
	SetCheckValue(!m_bChecked);

	if (GetDelegateControl() != NULL)
	{
		GetDelegateControl()->OnSuborEvent(this, CGuiConst::EVENT_CLICK, 
			CVarList());
	}

	return 1;
}

int CheckButton::OnLButtonDown(int x, int y)
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

        if (GetClickSound() != NULL)
        {
            // 播放音效
            GetGui()->PlayingSound(GetClickSound());
        }
	}
	
	return 1; 
}

int CheckButton::OnLButtonUp(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (m_bPush)
	{
		if (InRectangle(x, y))
		{
			SetCheckValue(!m_bChecked);

			if (GetDelegateControl() != NULL)
			{
				GetDelegateControl()->OnSuborEvent(this, 
					CGuiConst::EVENT_CLICK, 
					CVarList() << CGuiConst::MOUSE_LEFT);
			}

			if (m_bClickEvent)
			{
				// 每次点击都要产生事件
				GuiUtil_RunCallback(this, "on_click", 
					CVarList() << CGuiConst::MOUSE_LEFT);
			}
		}
		
		m_bPush = false;
	}
	
	return 1; 
}

