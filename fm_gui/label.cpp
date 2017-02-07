//--------------------------------------------------------------------
// 文件名:		label.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "label.h"
#include "mark_string.h"
#include "../visual/vis_utils.h"

/// \file label.cpp
/// \brief 文本或图片标签控件

/// entity: Label
/// \brief 文本或图片标签控件实体
/// \see 继承自IControl
DECLARE_ENTITY(Label, 0, IControl);

/// property: string Align
/// \brief 对齐方式
DECLARE_PROPERTY(const char*, Label, Align, GetAlignString, SetAlignString);
/// property: bool Solid
/// \brief 是否用背景色填充
DECLARE_PROPERTY(bool, Label, Solid, GetSolid, SetSolid);
/// property: bool Marked
/// \brief 是否包含转义符号
DECLARE_PROPERTY(bool, Label, Marked, GetMarked, SetMarked);
/// property: bool Transparent
/// \brief 是否判断透明
DECLARE_PROPERTY(bool, Label, Transparent, GetTransparent, SetTransparent);
/// property: bool ClickEvent
/// \brief 是否产生点击事件
DECLARE_PROPERTY(bool, Label, ClickEvent, GetClickEvent, SetClickEvent);
/// property: bool Rotate
/// \brief 是否允许旋转
DECLARE_PROPERTY(bool, Label, Rotate, GetRotate, SetRotate);
/// property: float AngleX
/// \brief 平面旋转角度X
DECLARE_PROPERTY(float, Label, AngleX, GetAngleX, SetAngleX);
/// property: float AngleY
/// \brief 平面旋转角度Y
DECLARE_PROPERTY(float, Label, AngleY, GetAngleY, SetAngleY);
/// property: float AngleZ
/// \brief 平面旋转角度Z
DECLARE_PROPERTY(float, Label, AngleZ, GetAngleZ, SetAngleZ);
/// property: string RefCursor
/// \brief 引用项的鼠标形状
DECLARE_PROPERTY(const char*, Label, RefCursor, GetRefCursor, SetRefCursor);
/// property: bool IsLimitWidth
/// \brief label显示宽度限制,当前对Marked为true时无效/add by ylp 2008_09_03
DECLARE_PROPERTY(bool, Label, IsLimitWidth, GetIsLimitWidth, SetIsLimitWidth);

/// property: int TextOffsetX
/// \brief label显示宽度限制,当前对Marked为true时无效/add by ylp 2008_09_28
DECLARE_PROPERTY(int, Label, TextOffsetX, GetTextOffsetX, SetTextOffsetX);

/// property: int TextOffsetY
/// \brief label显示宽度限制,当前对Marked为true时无效/add by ylp 2008_09_28
DECLARE_PROPERTY(int, Label, TextOffsetY, GetTextOffsetY, SetTextOffsetY);

/// property: int TextWidth
/// \brief 获取文本宽度
DECLARE_PROPERTY_GET(int, Label, TextWidth, GetTextWidth);

/// event: int on_click(object self, float offset_x, float offset_y)
/// \brief 鼠标点击图片的回调
/// \param self 本控件
/// \param offset_x X偏移
/// \param offset_y Y偏移
DECLARE_EVENT(on_click);
/// event: int on_click_ref(object self, wstring content, wstring ref)
/// \brief 鼠标点击引用项的回调
/// \param self 本控件
/// \param content 文本内容
/// \param ref 引用项内容
DECLARE_EVENT(on_click_ref);

// Label

Label::Label()
{
	m_nAlign = CGuiConst::ALIGN_LEFT;
	m_bSolid = false;
	m_bMarked = false;
	// 缺省为判断透明
	m_bTransparent = true;
	m_bClickEvent = false;
	m_bRotate = false;
	m_vAngle = FmVec3(0.0F, 0.0F, 0.0F);
	m_pMarkStr = NULL;
    m_bLimitWidth = false;
    m_nTextOffsetX = 5;
    m_nTextOffsetY = 5;

	SetDesignSize(80, 16);
	SetCanSelect(false);
	SetNoFrame(true);
}

Label::~Label()
{
	SAFE_RELEASE(m_pMarkStr);
}

bool Label::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	m_strRefCursor = "WIN_HELP";

	return true;
}

bool Label::Shut()
{
	return IControl::Shut();
}

void Label::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("NoFrame");
	CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("NoFrame", IDesignInfo::TYPE_BOOL, true, true, &v);
    v = CVar(VTYPE_STRING, "Left");
	di->AddPropInfo("Align", IDesignInfo::TYPE_STRING, true, true,	&v );
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("Marked", IDesignInfo::TYPE_BOOL, true, true);
    v = CVar(VTYPE_BOOL, true);
	di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, &v);
	di->AddPropInfo("ClickEvent", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("Rotate", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AngleX", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("AngleY", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("AngleZ", IDesignInfo::TYPE_FLOAT, true, true);
	di->AddPropInfo("RefCursor", IDesignInfo::TYPE_CURSOR, true, true);
    di->AddPropInfo("IsLimitWidth", IDesignInfo::TYPE_BOOL, true, true);
    v = CVar(VTYPE_INT, 5);
    di->AddPropInfo("TextOffsetX", IDesignInfo::TYPE_INT, true, true, &v);
    v = CVar(VTYPE_INT, 5);
    di->AddPropInfo("TextOffsetY", IDesignInfo::TYPE_INT, true, true, &v);

	di->AddEventInfo("on_click");
	di->AddEventInfo("on_click_ref");
}

void Label::Paint(float seconds)
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

	if (m_bRotate)
	{
		// 旋转
		float cx = float(x1 + x2) * 0.5F;
		float cy = float(y1 + y2) * 0.5F;

		if (GetPainter()->GetScaleEnable())
		{
			float cx1 = cx * GetPainter()->GetScaleRatio();
			float cy1 = cy * GetPainter()->GetScaleRatio();
			
			FmMat4 mat;
            FmVec3 v(cx1, -cy1, 0.0F);
			VisUtil_MatrixRotationTranslation(&mat, &m_vAngle,
				&v);
			GetPainter()->SetRotateTransform(&mat);
		}
		else
		{
			FmMat4 mat;
			FmVec3 v(cx, -cy, 0.0F);
			VisUtil_MatrixRotationTranslation(&mat, &m_vAngle, &v);
			GetPainter()->SetRotateTransform(&mat);
		}
			
		x1 -= int(cx);
		x2 -= int(cx);
		y1 -= int(cy);
		y2 -= int(cy);
	}

	if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		if (m_bSolid)
		{
			GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		}
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}

	//GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
	
	const wchar_t* text = GetShowText();
	
	if (!WideStrEmpty(text))
	{
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

		if (m_pMarkStr)
		{
			int start_x = 0;
			int start_y = 0;
			
			switch (m_nAlign)
			{
			case CGuiConst::ALIGN_LEFT:
				m_pMarkStr->GetLeftStart(x1, y1, x2, y2, start_x, start_y);
				break;
			case CGuiConst::ALIGN_CENTER:
				m_pMarkStr->GetCenterStart(x1, y1, x2, y2, start_x, start_y);
				break;
			case CGuiConst::ALIGN_RIGHT:
				m_pMarkStr->GetRightStart(x1, y1, x2, y2, start_x, start_y);
				break;
			default:
				Assert(0);
				break;
			}

			m_pMarkStr->Write(seconds, start_x, start_y, GetForeColor());
		}
		else
		{
			wchar_t buffer[256] = { 0 };

			if (m_bLimitWidth)
            {
				text = GuiUtil_GetCanShowText(GetPainter(),
					text, GetWidth(), L"...", buffer, sizeof(buffer));
            }

			switch (m_nAlign)
			{
			case CGuiConst::ALIGN_LEFT:
				GetPainter()->WriteTextLeft(x1, y1, x2, y2, GetForeColor(), 
					text);
				break;
			case CGuiConst::ALIGN_CENTER:
				GetPainter()->WriteTextCenter(x1, y1, x2, y2, GetForeColor(), 
					text);
				break;
			case CGuiConst::ALIGN_RIGHT:
				GetPainter()->WriteTextRight(x1, y1, x2, y2, GetForeColor(), 
					text);
				break;
			default:
				Assert(0);
				break;
			}
		}
	}
	
	if (m_bRotate)
	{
		GetPainter()->SetRotateTransform(NULL);
	}
}

void Label::SetText(const wchar_t* value)
{
	IControl::SetText(value);

	if (m_bMarked)
	{
		// 设置字体后方可取得正确的尺寸
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

		SAFE_RELEASE(m_pMarkStr);
			
		m_pMarkStr = CMarkString::NewInstance(GetPainter());
		m_pMarkStr->Create(GetGui(), value);
	}
	else
	{
		SAFE_RELEASE(m_pMarkStr);
	}

	if (GetAutoSize())
	{
        // 这边,如果设置了背景自动放缩则也可以自动放缩
        if (HasBackImage() && GetDrawModeString() != "Expand")
        {
            // 如果背景不改改变,则直接返回,不再调整大小
            return;
        }

		// 设置字体后方可取得正确的尺寸
		GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

		int h;
		int w;
		
		if (m_pMarkStr)
		{
			h = m_pMarkStr->GetHeight();
			w = m_pMarkStr->GetWidth();
		}
		else
		{
			h = GetPainter()->GetTextHeight(value);
			w = GetPainter()->GetTextWidth(value);
		}
		
		InnerSetWidth(w + m_nTextOffsetX * 2);
		InnerSetHeight(h + m_nTextOffsetY * 2);
	}
}

void Label::SetAlignString(const char* value)
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

const char* Label::GetAlignString() const
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

void Label::SetSolid(bool value) 
{ 
	m_bSolid = value; 
}

bool Label::GetSolid() const 
{ 
	return m_bSolid; 
}

void Label::SetMarked(bool value)
{
	m_bMarked = value;

	SetText(GetText());
}

bool Label::GetMarked() const
{
	return m_bMarked;
}

void Label::SetTransparent(bool value)
{
	m_bTransparent = value;
}

bool Label::GetTransparent() const
{
	return m_bTransparent;
}

void Label::SetClickEvent(bool value)
{
	m_bClickEvent = value;

	// 同时要改变能否点击状态
	SetCanSelect(value);
}

bool Label::GetClickEvent() const
{
	return m_bClickEvent;
}

void Label::SetRotate(bool value)
{
	m_bRotate = value;
}

bool Label::GetRotate() const
{
	return m_bRotate;
}

void Label::SetAngleX(float value)
{
	m_vAngle.x = value;
}

float Label::GetAngleX() const
{
	return m_vAngle.x;
}

void Label::SetAngleY(float value)
{
	m_vAngle.y = value;
}

float Label::GetAngleY() const
{
	return m_vAngle.y;
}

void Label::SetAngleZ(float value)
{
	m_vAngle.z = value;
}

float Label::GetAngleZ() const
{
	return m_vAngle.z;
}

void Label::SetRefCursor(const char* value)
{
	m_strRefCursor = value;
}

const char* Label::GetRefCursor() const
{
	return m_strRefCursor.c_str();
}

void Label::SetIsLimitWidth(bool IsLimit)
{
    m_bLimitWidth = IsLimit;
}

bool Label::GetIsLimitWidth()
{
    return m_bLimitWidth;
}

void Label::SetTextOffsetX(int offsetx)
{
    m_nTextOffsetX = offsetx;
}

int Label::GetTextOffsetX()
{
    return m_nTextOffsetX;
}

void Label::SetTextOffsetY(int offsety)
{
    m_nTextOffsetY = offsety;
}

int Label::GetTextOffsetY()
{
    return m_nTextOffsetY;
}

int Label::GetTextWidth()
{
    result_wstring wcsBuffer = GetText();
    
	GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
    
	return GetPainter()->GetTextWidth(wcsBuffer.c_str());
}

int Label::OnSetCursor(int x, int y)
{
	if (m_pMarkStr)
	{
		int x1 = InnerGetAbsLeft();
		int y1 = InnerGetAbsTop();
		int x2 = x1 + GetWidth();
		int y2 = y1 + GetHeight();

		int start_x = 0;
		int start_y = 0;
		
		switch (m_nAlign)
		{
		case CGuiConst::ALIGN_LEFT:
			m_pMarkStr->GetLeftStart(x1, y1, x2, y2, start_x, start_y);
			break;
		case CGuiConst::ALIGN_CENTER:
			m_pMarkStr->GetCenterStart(x1, y1, x2, y2, start_x, start_y);
			break;
		case CGuiConst::ALIGN_RIGHT:
			m_pMarkStr->GetRightStart(x1, y1, x2, y2, start_x, start_y);
			break;
		default:
			Assert(0);
			break;
		}

		int offset_x = x - start_x;
		int offset_y = y - start_y;

		int part = m_pMarkStr->GetInPart(offset_x, offset_y);

		if (part >= 0)
		{
			if (!WideStrEmpty(m_pMarkStr->GetPartRef(part)))
			{
				if (GetGui()->SetCurrentCursor(m_strRefCursor.c_str()))
				{
					return 1;
				}
			}
		}
	}

	return IControl::OnSetCursor(x, y);
}

bool Label::TestTransparent(int x, int y)
{
    if (m_bTransparent)
    {
        // 透明的话就全透，不用考虑任何元素
        return true;
    }

    // 不全透的情况判断是否要进行透明测试
	if (!GetTestTrans())
	{
		// 不进行透明测试，则完全不透明
		return false;
	}
	
    // 进行透明测试
	if (!HasBackImage())
	{
		return true;
	}

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();
	
    // 获取相对于控件左上的未放缩的偏移
	int offset_x = x - x1;
	int offset_y = y - y1;

	int alpha;

	if (!InnerGetBackImage().GetAlpha(GetDrawMode(), 0, 0, 
		GetWidth(), GetHeight(), offset_x, offset_y, alpha))
	{
		return true;
	}
	
	return (alpha == 0);
}

int Label::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	int res = 1;

	if (TestTransparent(x, y))
	{
		res = 0;
	}
	
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();

	if (m_bClickEvent && HasBackImage() && (res == 1))
	{
		GuiUtil_RunCallback(this, "on_click", 
			CVarList() << (x - x1) << (y - y1));
	}
	
	if (NULL == m_pMarkStr)
	{
		return res;
	}
	
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	int start_x = 0;
	int start_y = 0;
	
	switch (m_nAlign)
	{
	case CGuiConst::ALIGN_LEFT:
		m_pMarkStr->GetLeftStart(x1, y1, x2, y2, start_x, start_y);
		break;
	case CGuiConst::ALIGN_CENTER:
		m_pMarkStr->GetCenterStart(x1, y1, x2, y2, start_x, start_y);
		break;
	case CGuiConst::ALIGN_RIGHT:
		m_pMarkStr->GetRightStart(x1, y1, x2, y2, start_x, start_y);
		break;
	default:
		Assert(0);
		break;
	}
	
	int offset_x = int(x) - start_x;
	int offset_y = int(y) - start_y;
	
	int part = m_pMarkStr->GetInPart(offset_x, offset_y);

	if (part < 0)
	{
		return res;
	}
	
	const wchar_t* ref = m_pMarkStr->GetPartRef(part);

	if (WideStrEmpty(ref))
	{
		return res;
	}

	const wchar_t* content = m_pMarkStr->GetPartContent(part);

	GuiUtil_RunCallback(this, "on_click_ref", 
		CVarList() << content << ref);

	return res;
}

IControl* Label::OnGetInWhat(int x, int y)
{
	if (GetDesignMode())
	{
		// 设计状态
		return this;
	}
	
	if (m_pMarkStr != NULL)
	{
		return this;
	}
	
	//if (HasBackImage())
	//{
		if (TestTransparent(x, y))
		{
			// 透明
			return NULL;
		}
	//}
	
	return this;
}

