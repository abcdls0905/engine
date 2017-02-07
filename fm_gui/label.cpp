//--------------------------------------------------------------------
// �ļ���:		label.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "label.h"
#include "mark_string.h"
#include "../visual/vis_utils.h"

/// \file label.cpp
/// \brief �ı���ͼƬ��ǩ�ؼ�

/// entity: Label
/// \brief �ı���ͼƬ��ǩ�ؼ�ʵ��
/// \see �̳���IControl
DECLARE_ENTITY(Label, 0, IControl);

/// property: string Align
/// \brief ���뷽ʽ
DECLARE_PROPERTY(const char*, Label, Align, GetAlignString, SetAlignString);
/// property: bool Solid
/// \brief �Ƿ��ñ���ɫ���
DECLARE_PROPERTY(bool, Label, Solid, GetSolid, SetSolid);
/// property: bool Marked
/// \brief �Ƿ����ת�����
DECLARE_PROPERTY(bool, Label, Marked, GetMarked, SetMarked);
/// property: bool Transparent
/// \brief �Ƿ��ж�͸��
DECLARE_PROPERTY(bool, Label, Transparent, GetTransparent, SetTransparent);
/// property: bool ClickEvent
/// \brief �Ƿ��������¼�
DECLARE_PROPERTY(bool, Label, ClickEvent, GetClickEvent, SetClickEvent);
/// property: bool Rotate
/// \brief �Ƿ�������ת
DECLARE_PROPERTY(bool, Label, Rotate, GetRotate, SetRotate);
/// property: float AngleX
/// \brief ƽ����ת�Ƕ�X
DECLARE_PROPERTY(float, Label, AngleX, GetAngleX, SetAngleX);
/// property: float AngleY
/// \brief ƽ����ת�Ƕ�Y
DECLARE_PROPERTY(float, Label, AngleY, GetAngleY, SetAngleY);
/// property: float AngleZ
/// \brief ƽ����ת�Ƕ�Z
DECLARE_PROPERTY(float, Label, AngleZ, GetAngleZ, SetAngleZ);
/// property: string RefCursor
/// \brief ������������״
DECLARE_PROPERTY(const char*, Label, RefCursor, GetRefCursor, SetRefCursor);
/// property: bool IsLimitWidth
/// \brief label��ʾ�������,��ǰ��MarkedΪtrueʱ��Ч/add by ylp 2008_09_03
DECLARE_PROPERTY(bool, Label, IsLimitWidth, GetIsLimitWidth, SetIsLimitWidth);

/// property: int TextOffsetX
/// \brief label��ʾ�������,��ǰ��MarkedΪtrueʱ��Ч/add by ylp 2008_09_28
DECLARE_PROPERTY(int, Label, TextOffsetX, GetTextOffsetX, SetTextOffsetX);

/// property: int TextOffsetY
/// \brief label��ʾ�������,��ǰ��MarkedΪtrueʱ��Ч/add by ylp 2008_09_28
DECLARE_PROPERTY(int, Label, TextOffsetY, GetTextOffsetY, SetTextOffsetY);

/// property: int TextWidth
/// \brief ��ȡ�ı����
DECLARE_PROPERTY_GET(int, Label, TextWidth, GetTextWidth);

/// event: int on_click(object self, float offset_x, float offset_y)
/// \brief �����ͼƬ�Ļص�
/// \param self ���ؼ�
/// \param offset_x Xƫ��
/// \param offset_y Yƫ��
DECLARE_EVENT(on_click);
/// event: int on_click_ref(object self, wstring content, wstring ref)
/// \brief �����������Ļص�
/// \param self ���ؼ�
/// \param content �ı�����
/// \param ref ����������
DECLARE_EVENT(on_click_ref);

// Label

Label::Label()
{
	m_nAlign = CGuiConst::ALIGN_LEFT;
	m_bSolid = false;
	m_bMarked = false;
	// ȱʡΪ�ж�͸��
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
		// ��ת
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
		// ��������󷽿�ȡ����ȷ�ĳߴ�
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
        // ���,��������˱����Զ�������Ҳ�����Զ�����
        if (HasBackImage() && GetDrawModeString() != "Expand")
        {
            // ����������ĸı�,��ֱ�ӷ���,���ٵ�����С
            return;
        }

		// ��������󷽿�ȡ����ȷ�ĳߴ�
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

	// ͬʱҪ�ı��ܷ���״̬
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
        // ͸���Ļ���ȫ͸�����ÿ����κ�Ԫ��
        return true;
    }

    // ��ȫ͸������ж��Ƿ�Ҫ����͸������
	if (!GetTestTrans())
	{
		// ������͸�����ԣ�����ȫ��͸��
		return false;
	}
	
    // ����͸������
	if (!HasBackImage())
	{
		return true;
	}

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();
	
    // ��ȡ����ڿؼ����ϵ�δ������ƫ��
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
		// ���״̬
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
			// ͸��
			return NULL;
		}
	//}
	
	return this;
}

