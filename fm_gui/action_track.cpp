//--------------------------------------------------------------------
// 文件名:		ActionTrack.cpp
// 内  容:		动作融合编辑器中的滑块
// 说  明:		
// 创建日期:	2010年6月22日
// 创建人:		刘建国
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "action_track.h"
#include "gui_input.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

/// \file ActionTrack.cpp
/// \brief 滑块

/// entity: ActionTrack
/// \brief 滑块控件
/// \see 继承自IControl
DECLARE_ENTITY(ActionTrack, 0, IControl);

/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(const char*, ActionTrack, Align, GetAlignString, 
	SetAlignString);

/// property:int StartFrame
/// \brief 开始帧
DECLARE_PROPERTY(int, ActionTrack, StartFrame, GetStartFrame, SetStartFrame);

/// property:int MaxFrame
/// \brief 开始帧
DECLARE_PROPERTY(int, ActionTrack, MaxFrame, GetMaxFrame, SetMaxFrame);

/// property:int EndFrame
/// \brief 结束帧
DECLARE_PROPERTY(int, ActionTrack, EndFrame, GetEndFrame, SetEndFrame);

/// property:int Scale
/// \brief 缩放系数
DECLARE_PROPERTY(float, ActionTrack, Scale, GetScale, SetScale);

/// property:int FadeIn
/// \brief 融入时间
DECLARE_PROPERTY(float, ActionTrack, FadeIn, GetFadeIn, SetFadeIn);

/// property:int FadeOut
/// \brief 融出时间
DECLARE_PROPERTY(float, ActionTrack, FadeOut, GetFadeOut, SetFadeOut);

/// property: bool IsLock
/// \brief 锁定否
DECLARE_PROPERTY(bool, ActionTrack, IsLock, GetIsLock, SetIsLock);

/// property: bool HoverRepeat
/// \brief 当鼠标持续按住时是否产生多个回调
DECLARE_PROPERTY(bool, ActionTrack, HoverRepeat, GetHoverRepeat, SetHoverRepeat);
/// property: bool PushEvent
/// \brief 鼠标按下时是否产生事件
DECLARE_PROPERTY(bool, ActionTrack, PushEvent, GetPushEvent, SetPushEvent);

/// property: bool OutUpEvent
/// \brief 鼠标在控件上按下并移出控件后放开时是否产生事件
DECLARE_PROPERTY(bool, ActionTrack, OutUpEvent, GetOutUpEvent, SetOutUpEvent);

/// property: bool DragEvent
/// \brief 鼠标拖拽，是否产生点击事件
DECLARE_PROPERTY(bool, ActionTrack, DragEvent, GetDragEvent, SetDragEvent);

/// property: int EditMode
/// \brief 编辑模式
DECLARE_PROPERTY(int, ActionTrack, EditMode, GetEditMode, SetEditMode);

/// property: string Style
/// \brief 显示风格
DECLARE_PROPERTY(const char*, ActionTrack, Style, GetStyleString, 
	SetStyleString);
/// property: string NormalImage
/// \brief 正常状态的图元
DECLARE_PROPERTY(const char*, ActionTrack, NormalImage, GetNormalImage, 
	SetNormalImage);
/// property: string FocusImage
/// \brief 鼠标选中状态的图元
DECLARE_PROPERTY(const char*, ActionTrack, FocusImage, GetFocusImage, 
	SetFocusImage);
/// property: string PushImage
/// \brief 被按下状态的图元
DECLARE_PROPERTY(const char*, ActionTrack, PushImage, GetPushImage, 
	SetPushImage);
/// property: string DisableImage
/// \brief 被禁止操作状态的图元
DECLARE_PROPERTY(const char*, ActionTrack, DisableImage, GetDisableImage, 
	SetDisableImage);

/// property: string Icon
/// \brief 图标
DECLARE_PROPERTY(const char*, ActionTrack, Icon, GetIconName, SetIconName);

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

/// event: int on_drag_move(object self, int drag_x, int drag_y)
/// \brief 鼠标按住拖动的回调
/// \param self 本控件
/// \param drag_x X方向的拖动距离
/// \param drag_y Y方向的拖动距离
DECLARE_EVENT(on_drag_move);

/// event: int on_mouse_move(object self, int x, int y)
/// \brief 鼠标移动
/// \param self 本控件
/// \param x X坐标
/// \param y Y坐标
DECLARE_EVENT(on_mouse_move);

// ActionTrack

ActionTrack::ActionTrack()
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
	m_bIconLoaded = false;

	m_nDragOldX = 0;
	m_nDragOldY = 0;
	m_nStartFrame = 0;
	m_nFrameChange = 0;
	m_bSlide = false;
	m_nEditMode = 0;
	m_nEndFrame = 0;
	m_fScale = 1.0f;

	m_nFadeIn = 0;
	m_nFadeOut = 0;
	m_nFadeOutEnd = 0;
	m_btnColor = CGuiConst::BUTTON_COLOR;
	m_btnColorWhite = CGuiConst::WHITE_COLOR;
	m_bLock = false;
	m_bControl = false;
	
	SetDesignSize(80, 24);
	SetCanFocus(true);
	SetTabStop(true);
}

ActionTrack::~ActionTrack()
{
}

bool ActionTrack::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	//SetVAnchorString("Top");
	//SetHAnchorString("Left");

	return true;
}

bool ActionTrack::Shut()
{
	m_FocusImage.Release();
	m_PushImage.Release();
	m_DisableImage.Release();

	if (!m_IconId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_IconId);
		m_IconId = PERSISTID();
	}
	
	return IControl::Shut();
}

void ActionTrack::SetScale(float value)
{
	m_fScale = value;
	//SetWidth(m_fScale * 6.0f * m_nMaxFrame );
}

float ActionTrack::GetScale() const
{
	return m_fScale;
}

// 对齐方式
void ActionTrack::SetAlignString(const char* value)
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

const char* ActionTrack::GetAlignString() const
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

void ActionTrack::SetHoverRepeat(bool value)
{
	m_bHoverRepeat = value;
}

bool ActionTrack::GetHoverRepeat() const
{
	return m_bHoverRepeat;
}

void ActionTrack::SetPushEvent(bool value)
{
	m_bPushEvent = value;
}

bool ActionTrack::GetPushEvent() const
{
	return m_bPushEvent;
}

void ActionTrack::SetOutUpEvent(bool value)
{
	m_bOutUpEvent = value;
}

bool ActionTrack::GetOutUpEvent() const
{
	return m_bOutUpEvent;
}

void ActionTrack::SetDragEvent(bool value)
{
	m_bDragEvent = value;
}

bool ActionTrack::GetDragEvent() const
{
	return m_bDragEvent;
}

void ActionTrack::SetEditMode(int value)
{
	m_nEditMode = value;
}
int ActionTrack::GetEditMode() const
{
	return m_nEditMode;
}

void ActionTrack::SetIsSlide(bool value)
{
	m_bSlide = value;
}

bool ActionTrack::GetIsSlide() const
{
	return m_bSlide;
}
 
void ActionTrack::SetIsLock(bool value)
{
	m_bLock = value;
}

bool ActionTrack::GetIsLock() const
{
	return m_bLock;
}

void ActionTrack::SetStartFrame(int value)
{
	m_nStartFrame = value;
}

int ActionTrack::GetStartFrame() const
{
	return m_nStartFrame;
}

void ActionTrack::SetEndFrame(int value)
{
	m_nEndFrame = value;
}

int ActionTrack::GetEndFrame() const
{
	return m_nEndFrame;
}

void ActionTrack::SetMaxFrame(int value)
{
	m_nMaxFrame = value;
}

int ActionTrack::GetMaxFrame() const
{
	return m_nMaxFrame;
}

void ActionTrack::SetFrameChange(int value)
{
	m_nFrameChange = value;
}

int ActionTrack::GetFrameChange() const
{
 return m_nFrameChange;
}

void ActionTrack::SetFadeIn(float value)
{
	m_nFadeIn = int(value * GetWidth());
}

float ActionTrack::GetFadeIn() const
{
	//return m_nFadeIn * 1.0f / GetWidth();
	return 0.0f;
}

void ActionTrack::SetFadeOut(float value)
{
	m_nFadeOutEnd = int((value / 0.03f) * (4.0f / m_fScale));
}
float ActionTrack::GetFadeOut() const
{
	 return m_nFadeOutEnd / (4.0f / m_fScale) * 0.03f;
}

void ActionTrack::SetStyleString(const char* value)
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

const char* ActionTrack::GetStyleString() const
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

void ActionTrack::SetNormalImage(const char* value) 
{ 
	SetBackImage(value);
}

const char* ActionTrack::GetNormalImage() const 
{ 
	return GetBackImage();
}

void ActionTrack::SetFocusImage(const char* value) 
{ 
	m_FocusImage.SetName(value);
	m_FocusImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ActionTrack::GetFocusImage() const 
{ 
	return m_FocusImage.GetName();
}

void ActionTrack::SetPushImage(const char* value) 
{ 
	m_PushImage.SetName(value);
	m_PushImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ActionTrack::GetPushImage() const 
{ 
	return m_PushImage.GetName();
}

void ActionTrack::SetDisableImage(const char* value) 
{ 
	m_DisableImage.SetName(value);
	m_DisableImage.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* ActionTrack::GetDisableImage() const 
{ 
	return m_DisableImage.GetName();
}

void ActionTrack::SetFaceColor(unsigned int value)
{
	m_btnColor = value;
	m_btnColorWhite = value;
}

unsigned int ActionTrack::GetFaceColor() const
{
	return m_btnColor;
}

void ActionTrack::SetIconName(const char* value)
{
	Assert(value != NULL);

	m_strIcon = value;
	m_bIconLoaded = false;
	
	if (!m_IconId.IsNull())
	{
		GetPainter()->ReleaseGlyph(m_IconId);
		m_IconId = PERSISTID();
	}
}

const char* ActionTrack::GetIconName() const
{
	return m_strIcon.c_str();
}

void ActionTrack::SetPressed(bool bValue)
{
	m_bPush = bValue;
}

void ActionTrack::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	CVar varDefault;
	di->RemovePropInfo("BackImage");

	di->RemovePropInfo("Width");
	di->RemovePropInfo("Height");
	di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, 
		true, true,	&(varDefault.SetString("Center"),varDefault));
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
	
	di->AddEventInfo("on_click");
	di->AddEventInfo("on_push");
	di->AddEventInfo("on_drag_move");
	di->AddEventInfo("on_mouse_move");
}

bool ActionTrack::GetLoadFinish()
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

void ActionTrack::Paint(float seconds)
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
			draw_image = m_DisableImage.Draw(GetDrawMode(), x1, y1, x2, y2);
		}
	}
	else if (GetPressed())
	{
		if (m_PushImage.IsReady())
		{
			draw_image = m_PushImage.Draw(GetDrawMode(), x1, y1, x2, y2);
		}
	}
	else if (GetCapture())
	{
		if (m_FocusImage.IsReady())
		{
			draw_image = m_FocusImage.Draw(GetDrawMode(), x1, y1, x2, y2);
		}
	}
	
	if (!draw_image)
	{
		if (HasBackImage())
		{
			draw_image = DrawBackImage(seconds, x1, y1, x2, y2);
		}
	}

#if 0	//m_strIcon绘制
	if ((!draw_image) && (!m_strIcon.empty()))
	{
		if (!m_bIconLoaded)
		{
			size_t width = GetWidth() - 6;
			
			if (width < 20)
			{
				m_IconId = GetPainter()->CreateIconGlyph(m_strIcon.c_str(), 
					16);
			}
			else
			{
				m_IconId = GetPainter()->CreateIconGlyph(m_strIcon.c_str(), 
					32);
			}

			m_bIconLoaded = true;
		}
			
		if (!m_IconId.IsNull())
		{
			if (GetPressed())
			{
				GetPainter()->FillRect(x1, y1, x2, y2, m_btnColor);
				GetPainter()->DrawRect(x1, y1, x2, y2, CGuiConst::BLACK_COLOR);
			}
			else
			{
				/*if (m_bSlide)
				{
					GuiUtil_PaintButtonFaceDiv(GetPainter(), x1, y1, x2, y2, int(GetWidth() * 0.2f), int(GetWidth() * 0.2f));
				}*/
				//else
				{
					GuiUtil_PaintButtonFaceColor(GetPainter(), m_btnColor, 
						m_btnColorWhite, x1, y1, x2, y2);
				}
			}
			
			int tw = GetPainter()->GetGlyphWidth(m_IconId);
			int th = GetPainter()->GetGlyphHeight(m_IconId);
			
			draw_image = GetPainter()->DrawGlyph(x1 + 3, y1 + 3, x2 - 3, 
				y2 - 3, 0, 0, tw, th, m_IconId);
		}
	}
#endif
	// 是否传统按钮状
	bool button_face = (GetLineColor() != 0);
	
	if ((!draw_image) && button_face)
	{
		float fScale = 4.0f / m_fScale;
		int iDivIn = int(m_nStartFrame * fScale);
		int iDivOut = int((m_nEndFrame - 0) * fScale + InnerGetAbsLeft());

		if ((m_nEndFrame - 0) * fScale + m_nFadeOutEnd > GetWidth())
		{
			m_nFadeOutEnd = max(int(GetWidth() - (m_nEndFrame - 0) * fScale), 0);
		}

		int iFadeEnd = int((m_nEndFrame - 0) * fScale + m_nFadeOutEnd + InnerGetAbsLeft());

		if (GetPressed())
		{
			if (m_bSlide)
			{
				GuiUtil_PaintButtonFaceDiv(GetPainter(), x1, y1, x2, y2, 
					iDivIn, iDivOut, iFadeEnd);
			}
			else
			{
				GetPainter()->FillRect(x1, y1, x2, y2, m_btnColor);
			}

			GetPainter()->DrawRect(x1, y1, x2, y2, CGuiConst::BLACK_COLOR);
		}
		else
		{
			if (m_bSlide)
			{
				GuiUtil_PaintButtonFaceDiv(GetPainter(), x1, y1, x2, y2, 
					iDivIn, iDivOut, iFadeEnd);
			}
			else
			{
				GuiUtil_PaintButtonFaceColor(GetPainter(), m_btnColor, 
					m_btnColorWhite, x1, y1, x2, y2);
			}
		}
		
		if (m_nStyle != CGuiConst::ARROW_NONE)
		{
			GuiUtil_PaintArrow(GetPainter(), m_nStyle, x1, y1, GetWidth(), 
				GetHeight(), GetForeColor());
		}
	}

	const wchar_t* text = GetShowText();
	
	if (!WideStrEmpty(text))
	{
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
		unsigned int color;
		
		if (GetEnabled())
		{
			if ((!button_face) && GetCapture())
			{
				color = GetBackColor();
			}
			else
			{
				color = GetForeColor();
			}
		}
		else
		{
			color = CGuiConst::DISABLE_COLOR;
		}

		wchar_t buffer[256] = { 0 };

		const wchar_t* val = GuiUtil_GetCanShowText(GetPainter(), text, 
			GetWidth(), L"...", buffer, sizeof(buffer));
		
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

void ActionTrack::SetIsControl(bool value)
{
	m_bControl = value;
}

bool ActionTrack::GetIsControl() const
{
	return m_bControl;
}

int ActionTrack::OnKeyDown(unsigned int vk)
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

int ActionTrack::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();
	
	Assert((m_nClickX >= 0) && (m_nClickX < GetWidth()));
	Assert((m_nClickY >= 0) && (m_nClickY < GetHeight()));

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
		m_nEditMode = CheckEditMode(x, y);
	}

	return 1; 
}

int ActionTrack::OnLButtonUp(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (GetPressed())
	{
		// 这边如果不在按钮上,就表明是在按钮上按下后移开的,也应向应消息
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

	if (m_bLock)
	{
		m_bLock = false;
	}

	if (m_bPush)
	{
		m_bPush = false;
	}
	
	return 1; 
}

int ActionTrack::OnLButtonDoubleClick(int x, int y)
{ 
	if (GetDelegateControl() != NULL)
	{
		GetDelegateControl()->OnSuborEvent(this, 
			CGuiConst::EVENT_DOUBLE_CLICK, CVarList() << x << y);
	}

	return OnLButtonDown(x, y);
}

int ActionTrack::OnLButtonHover(int x, int y)
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

int ActionTrack::CheckEditMode(int x, int y)
{
	//默认为改变位置
	int iMode = 0;
	int iLeft = InnerGetAbsLeft();
	int iWidth = GetWidth();

	// 尺寸编辑
	if(m_bControl)
	{
		iMode = 1;
	}
	// 起始帧
	else if (x < iLeft + iWidth / 4)
	{
		iMode = 3;
	}
	// 位置
	else if(x < iLeft + iWidth / 4 * 3)
	{
		iMode = 0;
	}
	// 融出时间
	else if (x < iLeft + iWidth)
	{
		iMode = 4;
	}

	return iMode;
}

int ActionTrack::OnDragMove(int x, int y)
{
	if (m_bDragEvent)
	{
		int drag_x = x - m_nDragOldX;
		int drag_y = y - m_nDragOldY;

		m_nDragOldX = x;
		m_nDragOldY = y;

		GuiUtil_RunCallback(this, "on_drag_move", 
			CVarList() << drag_x << drag_y);

		GuiUtil_RunCallback(this, "on_mouse_move",
			CVarList() << x << y);

		int iLeft = GetLeft();
		int iWidth = GetWidth();

		if (iWidth < 10)
		{
			m_nEditMode = 0;
		}

		switch(m_nEditMode)
		{
			case 0:
				{
					// 改变位置
					SetLeft(iLeft + drag_x);
					m_nFrameChange = drag_x;
				}
				break;
			case 1:
				{	// 改变尺寸
					SetWidth(min(max(iWidth + drag_x, 24), 512));
					SetScale(m_nMaxFrame * 4.0f / GetWidth());
				}
				break;
			case 2:
				{
				}
				break;
			case 3:
				{	// 起始帧
					m_nStartFrame = int(max(min(int(iWidth * 0.5f), 
					m_nStartFrame * (4.0f / m_fScale) + drag_x * 1.0f), 0) / (4.0f / m_fScale));
				}
				break;
			case 4:
				{	// 融出时间
					m_nFadeOutEnd = max(min(iWidth, m_nFadeOutEnd + drag_x), 0);
				}
				break;
			default :
				break;
		}

	}

	if (GetDelegateControl() != NULL)
	{
		return GetDelegateControl()->OnSuborEvent(this, 
			CGuiConst::EVENT_DRAG_MOVE, 
			CVarList() << m_nClickX << m_nClickY << x << y);
	}
	
	return 0;
}

bool ActionTrack::TestTransparent(int x, int y)
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
