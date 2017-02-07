//--------------------------------------------------------------------
// �ļ���:		i_control.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "i_control.h"
#include "../visual/i_gui_hook.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

/// \file i_control.cpp
/// \brief ����ؼ�

/// entity: IControl
/// \brief ����ؼ�����
DECLARE_ABSTRACT_ENTITY(IControl, 0, IEntity);

// ��������չ�ؼ��ﶨ��Ļ�������

/// readonly: object Root
/// \brief ���ؼ���һ���Ǵ��ڣ�
DECLARE_PROPERTY_GET(PERSISTID, IControl, Root, GetRootID);
/// readonly: object ParentForm
/// \brief ��ȡ�˿ؼ���ֱ�Ӹ����洰��Form
DECLARE_PROPERTY_GET(PERSISTID, IControl, ParentForm, GetParentFormID);
/// readonly: object Parent
/// \brief ���ؼ�
DECLARE_PROPERTY_GET(PERSISTID, IControl, Parent, GetParentID);
/// readonly: object Master
/// \brief ���������ؼ�
DECLARE_PROPERTY_GET(PERSISTID, IControl, Master, GetMasterID);
/// readonly: object Delegate
/// \brief �����ؼ�
DECLARE_PROPERTY_GET(PERSISTID, IControl, Delegate, GetDelegateID);
/// readonly: bool IsContainer
/// \brief �Ƿ�������
DECLARE_PROPERTY_GET(bool, IControl, IsContainer, IsContainer);
/// readonly: bool Focused
/// \brief �Ƿ�ӵ�����뽹��
DECLARE_PROPERTY_GET(bool, IControl, Focused, GetFocused);
/// readonly: bool Capture
/// \brief �Ƿ񲶻����
DECLARE_PROPERTY_GET(bool, IControl, Capture, GetCapture);
/// readonly: bool LoadFinish
/// \brief �Ƿ�������
DECLARE_PROPERTY_GET(bool, IControl, LoadFinish, GetLoadFinish);

/// property: int AbsLeft
/// \brief ����X����
DECLARE_PROPERTY(int, IControl, AbsLeft, GetAbsLeft, SetAbsLeft);
/// property: int AbsTop
/// \brief ����Y����
DECLARE_PROPERTY(int, IControl, AbsTop, GetAbsTop, SetAbsTop);

/// property: string Name
/// \brief �ؼ���
DECLARE_PROPERTY(const char*, IControl, Name, GetName, SetName);
/// property: int Left
/// \brief ����ڸ��ؼ���X����
DECLARE_PROPERTY(int, IControl, Left, GetLeft, SetLeft);
/// property: int Top
/// \brief ����ڸ��ؼ���Y����
DECLARE_PROPERTY(int, IControl, Top, GetTop, SetTop);
/// property: int Width
/// \brief ���
DECLARE_PROPERTY(int, IControl, Width, GetWidth, SetWidth);
/// property: int Height
/// \brief �߶�
DECLARE_PROPERTY(int, IControl, Height, GetHeight, SetHeight);
/// property: bool AsyncLoad
/// \brief �Ƿ��첽����
DECLARE_PROPERTY(bool, IControl, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool DesignMode
/// \brief �Ƿ������״̬
DECLARE_PROPERTY(bool, IControl, DesignMode, GetDesignMode, SetDesignMode);

/// property: string ForeColor
/// \brief ǰ��ɫ
DECLARE_PROPERTY(result_string, IControl, ForeColor, GetForeColorString, 
	SetForeColorString);
/// property: string BackColor
/// \brief ����ɫ
DECLARE_PROPERTY(result_string, IControl, BackColor, GetBackColorString, 
	SetBackColorString);
/// property: string LineColor
/// \brief �ߵ���ɫ
DECLARE_PROPERTY(result_string, IControl, LineColor, GetLineColorString, 
	SetLineColorString);

/// property: string BlendColor
/// \brief �����ɫ
DECLARE_PROPERTY(result_string, IControl, BlendColor, GetBlendColorString, 
	SetBlendColorString);

/// property: string ShadowColor
/// \brief ������Ӱ/���ɫ
DECLARE_PROPERTY(result_string, IControl, ShadowColor, GetShadowColorString, 
	SetShadowColorString);
	
/// property: string VAnchor
/// \brief ��ֱê����ʽ
DECLARE_PROPERTY(const char*, IControl, VAnchor, GetVAnchorString, 
	SetVAnchorString);
/// property: string HAnchor
/// \brief ˮƽê����ʽ
DECLARE_PROPERTY(const char*, IControl, HAnchor, GetHAnchorString, 
	SetHAnchorString);

/// property: bool Enabled
/// \brief �Ƿ��������
DECLARE_PROPERTY(bool, IControl, Enabled, GetEnabled, SetEnabled);
/// property: bool Visible
/// \brief �Ƿ���ʾ
DECLARE_PROPERTY(bool, IControl, Visible, GetVisible, SetVisible);

/// property: bool TestTrans
/// \brief ͸������,����ĳЩ�ؼ�͸������Ҳ��Ӧ��Ϣ,Ĭ��Ϊfalse
DECLARE_PROPERTY(bool, IControl, TestTrans, GetTestTrans, SetTestTrans);

/// property: wstring Text
/// \brief �ؼ�����
DECLARE_PROPERTY(const wchar_t*, IControl, Text, GetText, SetText);

/// property: string DataSource
/// \brief �ؼ�����Դ
DECLARE_PROPERTY(const char*, IControl, DataSource, GetDataSource, 
	SetDataSource);

/// property: string Font
/// \brief ����
DECLARE_PROPERTY(const char*, IControl, Font, GetFontName, SetFontName);
/// property: string Cursor
/// \brief �����״
DECLARE_PROPERTY(const char*, IControl, Cursor, GetCursorName, SetCursorName);
/// property: wstring HintText
/// \brief ������ʾ����
DECLARE_PROPERTY(const wchar_t*, IControl, HintText, GetHintText, 
	SetHintText);
/// property: string HintType
/// \brief ������ʾ����
DECLARE_PROPERTY(const char*, IControl, HintType, GetHintType, SetHintType);
/// property: string Context
/// \brief �����Ĳ˵�
DECLARE_PROPERTY(const char*, IControl, Context, GetContext, SetContext);
/// property: object Data
/// \brief �������ݶ���
DECLARE_PROPERTY(PERSISTID, IControl, Data, GetDataID, SetDataID);

/// property: int TabIndex
/// \brief TAB���ƶ���˳��
DECLARE_PROPERTY(int, IControl, TabIndex, GetTabIndex, SetTabIndex);
/// property: bool TabStop
/// \brief �Ƿ����TAB�����뽹���ƶ����˿ؼ�
DECLARE_PROPERTY(bool, IControl, TabStop, GetTabStop, SetTabStop);

/// property: bool NoFrame
/// \brief �Ƿ񲻻��߿���
DECLARE_PROPERTY(bool, IControl, NoFrame, GetNoFrame, SetNoFrame);
/// property: bool AutoSize
/// \brief �Ƿ��Զ������ؼ���С
DECLARE_PROPERTY(bool, IControl, AutoSize, GetAutoSize, SetAutoSize);
/// property: string DrawMode
/// \brief ����ͼԪ����ʾ��ʽ
DECLARE_PROPERTY(const char*, IControl, DrawMode, GetDrawModeString, 
	SetDrawModeString);
/// property: string BackImage
/// \brief ����ͼԪ����
DECLARE_PROPERTY(const char*, IControl, BackImage, GetBackImage, 
	SetBackImage);

/// property: string MouseInSound
/// \brief �������ʱ����Ч
DECLARE_PROPERTY(const char*, IControl, InSound, GetInSound, SetInSound);
/// property: string MouseOutSound
/// \brief ����Ƴ�ʱ����Ч
DECLARE_PROPERTY(const char*, IControl, OutSound, GetOutSound, SetOutSound);
/// property: string MouseClickSound
/// \brief ���ʱ����Ч
DECLARE_PROPERTY(const char*, IControl, ClickSound, GetClickSound, 
	SetClickSound);

/// property: bool DisableEnter
/// \brief �Ƿ���öԻس�������Ӧ
DECLARE_PROPERTY(bool, IControl, DisableEnter, GetDisableEnter, SetDisableEnter);

/// event: int on_set_hint(object self, object hint, int x, int y)
/// \brief ��ʾ������ʾʱ�Ļص�
/// \param self ���ؼ�
/// \param hint ������ʾ�ؼ�
/// \param x ������λ��x
/// \param y ������λ��y
DECLARE_EVENT(on_set_hint);
/// event: int on_set_context(object self, object context)
/// \brief ��ʾ�����Ĳ˵�ǰ�Ļص�
/// \param self ���ؼ�
/// \param context �˵��ؼ�
DECLARE_EVENT(on_set_context);
/// event: int on_lost_capture(object self, object get)
/// \brief �������
DECLARE_EVENT(on_get_capture);
/// event: int on_lost_capture(object self, object lost)
/// \brief ����Ƴ�
DECLARE_EVENT(on_lost_capture);
/// event: int on_right_click(object self, int x, int y)
/// \brief ���Ҽ����ʱ�Ļص�
/// \param self ���ؼ�
/// \param x Xƫ��
/// \param y Yƫ��
DECLARE_EVENT(on_right_click);

/// event: int on_left_click(object self, int x, int y)
/// \brief ��������ʱ�Ļص�
/// \param self ���ؼ�
/// \param x Xƫ��
/// \param y Yƫ��
DECLARE_EVENT(on_left_click);

/// event: int on_left_double_click(object self, int x, int y)
/// \brief �����˫��ʱ�Ļص�
/// \param self ���ؼ�
/// \param x Xƫ��
/// \param y Yƫ��
DECLARE_EVENT(on_left_double_click);

// IControl

IControl::IControl()
{
	m_pGui = NULL;
	m_pPainter = NULL;
	m_pGuiHook = NULL;
	m_pParent = NULL;
	m_pDelegate = NULL;
	m_pData = NULL;

	m_nHash = 0;

	m_nLeft = 0;
	m_nTop = 0;
	m_nInnerAbsLeft = 0;
	m_nInnerAbsTop = 0;
	m_nWidth = 32;
	m_nHeight = 32;
    m_nAlpha = 255;
	m_nLoadMode = LOADMODE_DEFAULT;

	m_ForeColor.SetValue(CGuiConst::DEF_FORE_COLOR);
	m_BackColor.SetValue(CGuiConst::DEF_BACK_COLOR);
	m_LineColor.SetValue(CGuiConst::DEF_LINE_COLOR);
	m_BlendColor.SetValue(CGuiConst::WHITE_COLOR);
	m_ShadowColor.SetValue(0);

	m_nVAnchor = CGuiConst::V_ANCHOR_TOP;
	m_nHAnchor = CGuiConst::H_ANCHOR_LEFT;

    m_bRefreshInnerAbsPos = true;
	m_bDesignMode = false;
	m_bEnabled = true;
	m_bVisible = true;
	m_bFocused = false;
	m_bCapture = false;
	m_bCanFocus = false;
	m_bCanSelect = true;
	m_bTestTrans = false;
    m_bLoadFinish = false;
    m_bIsDisableEnter = false;

	m_nTabIndex = 0;
	m_bTabStop = false;
	
	m_bNoFrame = false;
	m_bAutoSize = false;
	m_nDrawMode = 0;

	//m_pCallbacks = NULL;
	m_bTransparentDefault = false;
	m_bTransparent = false;
}

IControl::~IControl()
{
	//if (m_pCallbacks)
	//{
	//	CORE_DELETE(m_pCallbacks);
	//}

	SAFE_RELEASE(m_pGuiHook);
}

bool IControl::Init(const IVarList& args)
{
	// �����Ⱦ��
	const char* painter_name = args.StringVal(0);
	
	m_pPainter = (IPainter*)GetCore()->GetInterface(painter_name);
	
	if (NULL == m_pPainter)
	{
		CORE_TRACE("(IControl::Init)no painter");
		CORE_TRACE(painter_name);
		return false;
	}

	// ����ϵͳָ��
	m_pGui = (Gui*)GetCore()->GetEntity(args.ObjectVal(1));

	if (NULL == m_pGui)
	{
		CORE_TRACE("(IControl::Init)no Gui");
		return false;
	}
	
	return true;
}

bool IControl::Shut()
{
    // Ϊ�˱���ؼ���ɾ����GUI������BUG
    if (m_pGui->GetCaptureID() == this->GetID())
    {
        m_pGui->ClearCapture();
    }

	m_BackImage.Release();

	SAFE_RELEASE(m_pData);
	SAFE_RELEASE(m_pPainter);

    IControl* parent = this->GetParentControl();

    if (parent != NULL)
    {
        parent->RemoveChildNoEvent(this);
    }
	return true;
}

IControl* IControl::GetRootControl()
{
	IControl* pControl = this;

	while (pControl->GetParentControl() != NULL)
	{
		pControl = pControl->GetParentControl();
	}

	if (!pControl->GetEntInfo()->IsKindOf("Form"))
	{
		return NULL;
	}

	return pControl;
}

PERSISTID IControl::GetRootID()
{
	IControl* root = GetRootControl();
	
	if (NULL == root)
	{
		return PERSISTID();
	}
	else
	{
		return root->GetID();
	}
}

PERSISTID IControl::GetParentFormID()
{
    IControl* pControl = this;

    while (pControl)
    {
        if (pControl->GetEntInfo()->IsKindOf("Form"))
        {
            return pControl->GetID();
        }

        pControl = pControl->GetParentControl();
    }

    return PERSISTID();

}

//PERSISTID IControl::GetParentID() const
//{
//    if (NULL == m_pParent)
//	{
//		return PERSISTID();
//	}
//	else
//	{
//		return m_pParent->GetID();
//	}
//}

IControl* IControl::GetMasterControl()
{
	IControl* pControl = this;
	
	while (pControl->GetDelegateControl() != NULL)
	{
		pControl = pControl->GetDelegateControl();
	}
	
	return pControl;
}

PERSISTID IControl::GetMasterID()
{
	return GetMasterControl()->GetID();
}

PERSISTID IControl::GetDelegateID()
{
	if (NULL == m_pDelegate)
	{
		return PERSISTID();
	}
	
	return m_pDelegate->GetID();
}

void IControl::SetName(const char* val)
{
	Assert(val != NULL);

	m_strName = val;
	m_nHash = GetHashValueCase(val);
}

//const char* IControl::GetName() const
//{
//	return m_strName.c_str();
//}

//unsigned int IControl::GetHash() const
//{
//	return m_nHash;
//}

void IControl::InnerSetLeft(int value)
{
	int old = m_nLeft;

	m_nLeft = value;

    m_bRefreshInnerAbsPos = true;

	OnLeftChanged(old);
}

void IControl::InnerSetTop(int value)
{
	int old = m_nTop;

	m_nTop = value;

    m_bRefreshInnerAbsPos = true;

	OnTopChanged(old);
}

void IControl::InnerSetWidth(int value)
{
	int old = m_nWidth;

	m_nWidth = value;
	
	OnWidthChanged(old);
}

void IControl::InnerSetHeight(int value)
{
	int old = m_nHeight;

	m_nHeight = value;
	
	OnHeightChanged(old);
}

void IControl::SetLeft(int val)
{
	InnerSetLeft(val);
}

void IControl::SetTop(int val)
{
	InnerSetTop(val);
}

void IControl::SetWidth(int val)
{
    if (m_bAutoSize && m_BackImage.IsReady())
    {
        m_nWidth = m_BackImage.GetWidth();
        return;
    }

    // ���밴�з������Ƚ�
    if (val < GetMinWidth())
    {
        val = GetMinWidth();
    }

    InnerSetWidth(val);
}

//int IControl::GetWidth() const
//{
//	return m_nWidth;
//}

void IControl::SetHeight(int val)
{
    if (m_bAutoSize && m_BackImage.IsReady())
    {
        m_nHeight = m_BackImage.GetHeight();
        return;
    }

    // ���밴�з������Ƚ�
    if (val < GetMinHeight())
    {
        val = GetMinHeight();
    }

    InnerSetHeight(val);
}

//int IControl::GetHeight() const
//{
//	return m_nHeight;
//}

void IControl::SetAbsLeft(int val)
{
	IControl* parent = m_pParent;
	
	if (NULL == parent)
	{
		SetLeft(val);
		return;
	}
	
	switch (m_nHAnchor)
	{
	case CGuiConst::H_ANCHOR_LEFT:
		SetLeft(val - parent->GetAbsLeft());
		break;
	case CGuiConst::H_ANCHOR_CENTER:
		SetLeft(val - (parent->GetAbsLeft() + parent->GetWidth() / 2));
		break;
	case CGuiConst::H_ANCHOR_RIGHT:
		SetLeft(val - (parent->GetAbsLeft() + parent->GetWidth()));
		break;
	default:
		Assert(0);
		break;
	}
}

int IControl::GetAbsLeft() const
{
	IControl* parent = m_pParent;
	
	if (NULL == parent)
	{
		return m_nLeft;
	}

	switch (m_nHAnchor)
	{
	case CGuiConst::H_ANCHOR_LEFT:
		return parent->GetAbsLeft() + m_nLeft;
	case CGuiConst::H_ANCHOR_CENTER:
		return parent->GetAbsLeft() + int(parent->GetWidth() * 0.5F) + m_nLeft;
	case CGuiConst::H_ANCHOR_RIGHT:
		return parent->GetAbsLeft() + parent->GetWidth() + m_nLeft;
	default:
		Assert(0);
		break;
	}

	return 0;
}

void IControl::SetAbsTop(int val)
{
	IControl* parent = m_pParent;
	
	if (NULL == parent)
	{
		SetTop(val);
		return;
	}
	
	switch (m_nVAnchor)
	{
	case CGuiConst::V_ANCHOR_TOP:
		SetTop(val - parent->GetAbsTop());
		break;
	case CGuiConst::V_ANCHOR_CENTER:
		SetTop(val - (parent->GetAbsTop() + int(parent->GetHeight() / 2)));
		break;
	case CGuiConst::V_ANCHOR_BOTTOM:
		SetTop(val - (parent->GetAbsTop() + int(parent->GetHeight())));
		break;
	default:
		Assert(0);
		break;
	}
}

int IControl::GetAbsTop() const
{
	IControl* parent = m_pParent;
	
	if (NULL == parent)
	{
		return m_nTop;
	}

	switch (m_nVAnchor)
	{
	case CGuiConst::V_ANCHOR_TOP:
		return parent->GetAbsTop() + m_nTop;
	case CGuiConst::V_ANCHOR_CENTER:
		return parent->GetAbsTop() + parent->GetHeight() / 2 + m_nTop;
	case CGuiConst::V_ANCHOR_BOTTOM:
		return parent->GetAbsTop() + parent->GetHeight() + m_nTop;
	default:
		Assert(0);
		break;
	}

	return 0;
}

//int IControl::InnerGetAbsLeft() const
//{
//    return m_nInnerAbsLeft;
//}

//int IControl::InnerGetAbsTop() const
//{
//    return m_nInnerAbsTop;
//}

void IControl::SetForeColor(unsigned int val)
{
	m_ForeColor.SetValue(val);
}

//unsigned int IControl::GetForeColor() const
//{
//	return m_ForeColor.GetValue();
//}

void IControl::SetBackColor(unsigned int val)
{
	m_BackColor.SetValue(val);
}

//unsigned int IControl::GetBackColor() const
//{
//	return m_BackColor.GetValue();
//}

void IControl::SetLineColor(unsigned int val)
{
	m_LineColor.SetValue(val);
}

//unsigned int IControl::GetLineColor() const
//{
//	return m_LineColor.GetValue();
//}

// ��ɫ
void IControl::SetBlendColor(unsigned int val)
{
	m_BlendColor.SetValue(val);
}

//unsigned int IControl::GetBlendColor() const
//{
//	return m_BlendColor.GetValue();
//}

void IControl::SetForeColorString(const char* val)
{
	Assert(val != NULL);

	m_ForeColor.SetName(m_pGui, val);
}

result_string IControl::GetForeColorString() const
{
	return m_ForeColor.GetName().c_str();
}

void IControl::SetBackColorString(const char* val)
{
	Assert(val != NULL);

	m_BackColor.SetName(m_pGui, val);
}

result_string IControl::GetBackColorString() const
{
	return m_BackColor.GetName().c_str();
}

// ����ɫ�ַ���
void IControl::SetBlendColorString(const char* val)
{
	Assert(val != NULL);

	m_BlendColor.SetName(m_pGui, val);
}

result_string IControl::GetBlendColorString() const
{
	return m_BlendColor.GetName().c_str();
}

// ������Ӱɫ/���ɫ
void IControl::SetShadowColor(unsigned int val)
{
	m_ShadowColor.SetValue(val);
}

//unsigned int IControl::GetShadowColor() const
//{
//	return m_ShadowColor.GetValue();
//}

// ������Ӱɫ/���ɫ
void IControl::SetShadowColorString(const char* val)
{
	Assert(val != NULL);

	m_ShadowColor.SetName(m_pGui, val);
}

result_string IControl::GetShadowColorString() const
{
	return m_ShadowColor.GetName().c_str();
}

// �����ؼ���͸����
void IControl::SetAlpha(int val)
{
    if (val > 255)
    {
        val = 255;
    }
    else if (val < 0)
    {
        val = 0;
    }
    else
    {
        m_nAlpha = val;
    }
}

//int IControl::GetAlpha() const
//{
//    return m_nAlpha;
//}

void IControl::SetLineColorString(const char* val)
{
	Assert(val != NULL);

	m_LineColor.SetName(m_pGui, val);
}

result_string IControl::GetLineColorString() const
{
	return m_LineColor.GetName().c_str();
}

void IControl::InnerSetVAnchor(int value)
{
	if (value == m_nVAnchor)
	{
		return;
	}

	int top = GetAbsTop();

	m_nVAnchor = value;

	SetAbsTop(top);
}

void IControl::InnerSetHAnchor(int value)
{
	if (value == m_nHAnchor)
	{
		return;
	}

	int left = GetAbsLeft();
	
	m_nHAnchor = value;

	SetAbsLeft(left);
}

void IControl::SetVAnchorString(const char* val)
{
	if (stricmp(val, "Top") == 0)
	{
		InnerSetVAnchor(CGuiConst::V_ANCHOR_TOP);
	}
	else if (stricmp(val, "Center") == 0)
	{
		InnerSetVAnchor(CGuiConst::V_ANCHOR_CENTER);
	}
	else if (stricmp(val, "Bottom") == 0)
	{
		InnerSetVAnchor(CGuiConst::V_ANCHOR_BOTTOM);
	}
}

const char* IControl::GetVAnchorString() const
{
	switch (m_nVAnchor)
	{
	case CGuiConst::V_ANCHOR_TOP:
		return "Top";
	case CGuiConst::V_ANCHOR_CENTER:
		return "Center";
	case CGuiConst::V_ANCHOR_BOTTOM:
		return "Bottom";
	default:
		Assert(0);
		break;
	}

	return "";
}

void IControl::SetHAnchorString(const char* val)
{
	if (stricmp(val, "Left") == 0)
	{
		InnerSetHAnchor(CGuiConst::H_ANCHOR_LEFT);
	}
	else if (stricmp(val, "Center") == 0)
	{
		InnerSetHAnchor(CGuiConst::H_ANCHOR_CENTER);
	}
	else if (stricmp(val, "Right") == 0)
	{
		InnerSetHAnchor(CGuiConst::H_ANCHOR_RIGHT);
	}
}

const char* IControl::GetHAnchorString() const
{
	switch (m_nHAnchor)
	{
	case CGuiConst::H_ANCHOR_LEFT:
		return "Left";
	case CGuiConst::H_ANCHOR_CENTER:
		return "Center";
	case CGuiConst::H_ANCHOR_RIGHT:
		return "Right";
	default:
		Assert(0);
		break;
	}
	
	return "";
}

// ͸������,����ĳЩ�ؼ�͸������Ҳ��Ӧ��Ϣ
//void IControl::SetTestTrans(bool bTest)
//{
//    m_bTestTrans = bTest;
//}

//bool IControl::GetTestTrans()
//{
//    return m_bTestTrans;
//}

void IControl::SetText(const wchar_t* val)
{
    Assert(val != NULL);

    m_wsText = val;

    if (val[0] == L'@' && (m_wsText.size() > 1))
    {
		result_wstring real_text;
		
		if (m_pGui->ConvertText(val, real_text))
		{
			m_wsShowText = real_text.c_str();
			return;
		}
    }

    m_wsShowText = val;
}

const wchar_t* IControl::GetText() const
{
    return m_wsText.c_str();
}

point_t IControl::GetCaretPos()
{
	point_t pt = { InnerGetAbsLeft(), InnerGetAbsTop() + GetHeight() };

	return pt;
}

void IControl::SetDataSource(const char* source)
{
    if (source == NULL || source[0] == 0)
    {
        m_strDataSource = "";
    }
    else
    {
        m_strDataSource = source;
    }
}

const char* IControl::GetDataSource() const 
{ 
	return m_strDataSource.c_str();
}

void IControl::SetFontName(const char* val)
{
	Assert(val != NULL);

	m_strFontName = val;
}

//const char* IControl::GetFontName() const
//{
//	return m_strFontName.c_str();
//}

void IControl::SetCursorName(const char* val)
{
	Assert(val != NULL);

	m_strCursorName = val;
}

//const char* IControl::GetCursorName() const
//{
//	return m_strCursorName.c_str();
//}

void IControl::SetHintText(const wchar_t* value)
{
	Assert(value != NULL);
	
    m_wsHintText = value;

    // �ȼ���Ƿ���IDNAME
    if (m_pGui)
    {
		result_wstring real_text;
		
		if (m_pGui->ConvertText(value, real_text))
		{
			m_wsRealHintText = real_text.c_str();
			return;
		}
        else
        {
            m_wsRealHintText = value;
        }
    }
}

//const wchar_t* IControl::GetHintText() const
//{
//	return m_wsHintText.c_str();
//}

//const wchar_t* IControl::GetRealHintText() const
//{
//    return m_wsRealHintText.c_str();
//}

void IControl::SetHintType(const char* value)
{
	Assert(value != NULL);

	m_strHintType = value;
}

//const char* IControl::GetHintType() const
//{
//	return m_strHintType.c_str();
//}

void IControl::SetContext(const char* value)
{
	Assert(value != NULL);

	m_strContext = value;
}

//const char* IControl::GetContext() const
//{
//	return m_strContext.c_str();
//}

void IControl::SetDataID(const PERSISTID& value)
{
	SAFE_RELEASE(m_pData);
	
	m_pData = GetCore()->GetEntity(value);
}

PERSISTID IControl::GetDataID() const
{
	if (NULL == m_pData)
	{
		return PERSISTID();
	}

	return m_pData->GetID();
}

void IControl::SetAsyncLoad(bool value)
{
	if (value)
	{
		m_nLoadMode = LOADMODE_ASYNC;
	}
	else
	{
		m_nLoadMode = LOADMODE_SYNC;
	}
}

bool IControl::GetAsyncLoad() const
{
	if (LOADMODE_DEFAULT == m_nLoadMode)
	{
		if (m_pParent)
		{
			return m_pParent->GetAsyncLoad();
		}

		return false;
	}

	return (m_nLoadMode == LOADMODE_ASYNC);
}

void IControl::SetAutoSize(bool value)
{
	m_bAutoSize = value;

	if (m_bAutoSize && m_BackImage.IsReady())
	{
		InnerSetWidth(m_BackImage.GetWidth());
		InnerSetHeight(m_BackImage.GetHeight());
	}
}

bool IControl::GetAutoSize() const
{
	return m_bAutoSize;
}

void IControl::SetDrawModeString(const char* value)
{
	m_nDrawMode = GetDrawModeByString(value);
}

int IControl::GetDrawModeByString(const char* value,int nDefauleMode)const
{
	int nRes(nDefauleMode);
	Assert(value != NULL);

	if (stricmp(value, "Tile") == 0)
	{
		nRes = CGuiConst::DRAWMODE_TILE;
	}
	else if (stricmp(value, "Center") == 0)
	{
		nRes = CGuiConst::DRAWMODE_CENTER;
	}
	else if (stricmp(value, "FitWindow") == 0)
	{
		nRes = CGuiConst::DRAWMODE_FITWINDOW;
	}
	else if (stricmp(value, "Expand") == 0)
	{
		nRes = CGuiConst::DRAWMODE_EXPAND;
	}
	else if (stricmp(value, "ExpandH") == 0)
	{
		nRes = CGuiConst::DRAWMODE_EXPANDH;
	}
	else if (stricmp(value, "ExpandV") == 0)
	{
		nRes = CGuiConst::DRAWMODE_EXPANDV;
	}
	return nRes;
}

const char* IControl::GetStringByDrawMode(int nDrawMode)const
{
	switch (nDrawMode)
	{
	case CGuiConst::DRAWMODE_TILE:
		return "Tile";
	case CGuiConst::DRAWMODE_CENTER:
		return "Center";
	case CGuiConst::DRAWMODE_FITWINDOW:
		return "FitWindow";
	case CGuiConst::DRAWMODE_EXPAND:
		return "Expand";
	case CGuiConst::DRAWMODE_EXPANDH:
		return "ExpandH";
	case CGuiConst::DRAWMODE_EXPANDV:
		return "ExpandV";
	default:
		Assert(0);
		break;
	}

	return "";
}

const char* IControl::GetDrawModeString() const
{
	return GetStringByDrawMode(m_nDrawMode);
}

int IControl::GetDrawMode() const
{
	return m_nDrawMode;
}

void IControl::SetBackImage(const char* value)
{
	Assert(value != NULL);

    if (value[0] == 0)
    {
        //m_BackImage.Release();
		RelaseBackImage();
        return;
    }

	m_BackImage.SetName(value);
	m_BackImage.Create(m_pPainter, m_pGui, GetAsyncLoad());
	
	if (m_bAutoSize && m_BackImage.IsReady())
	{
		InnerSetWidth(m_BackImage.GetWidth());
		InnerSetHeight(m_BackImage.GetHeight());
	}
}

void IControl::RelaseBackImage()
{
	m_BackImage.Release();
	m_BackImage.SetName("");
}

bool IControl::DrawBackImage(float seconds, int x1, int y1, int x2, int y2)
{
    if (m_BackImage.IsReady())
    {
	    return m_BackImage.Draw(m_nDrawMode, x1, y1, x2, y2, GetBlendColor());
    }

    return false;
}

IControl* IControl::CreateSuborControl(IControl* parent, const char* name)
{
	IControl* pControl = (IControl*)GetCore()->CreateEntityArgs(
		name, CVarList() << m_pGui->GetPainterName() << m_pGui->GetID());

	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		pControl->Release();
		return NULL;
	}

	pControl->SetParentControl(parent);
	pControl->SetDelegateControl(parent);

	m_Subors.push_back(pControl);
	
	return pControl;
}

bool IControl::RemoveSuborControl(IControl* pControl)
{
	size_t k = 0; 

	while (k < m_Subors.size())
    {
        if (m_Subors[k] == pControl)
        {
            m_Subors.remove(k);
            return true;
        }
		else
		{
			++k;
		}
    }

    return false;
}

void IControl::Paint(float seconds)
{
}

void IControl::PrepareDesignInfo(IDesignInfo* di)
{
	Assert(di != NULL);

	di->AddPropInfo("Left", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("Top", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, false);
	
	CVar v(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::DEF_FORE_COLOR).c_str());
	di->AddPropInfo("ForeColor", IDesignInfo::TYPE_COLOR, true, true, &v);

	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::DEF_BACK_COLOR).c_str());
	di->AddPropInfo("BackColor", IDesignInfo::TYPE_COLOR, true, true, &v);

	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::DEF_LINE_COLOR).c_str());
	di->AddPropInfo("LineColor", IDesignInfo::TYPE_COLOR, true, true, &v);

	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::WHITE_COLOR).c_str());
	di->AddPropInfo("BlendColor", IDesignInfo::TYPE_COLOR, true, true, &v);


	v = CVar(VTYPE_STRING, GuiUtil_IntToColor(CGuiConst::BLACK_COLOR).c_str());
	di->AddPropInfo("ShadowColor", IDesignInfo::TYPE_COLOR, true, true, &v);

	v = CVar(VTYPE_STRING, "Top");
	di->AddPropInfo("VAnchor", IDesignInfo::TYPE_STRING, true, false, &v);

	v = CVar(VTYPE_STRING, "Left");
	di->AddPropInfo("HAnchor", IDesignInfo::TYPE_STRING, true, false, &v);

	v = CVar(VTYPE_BOOL, true);
	di->AddPropInfo("Enabled", IDesignInfo::TYPE_BOOL, true, false, &v);
    di->AddPropInfo("TestTrans", IDesignInfo::TYPE_BOOL, true, false);
	
	di->AddPropInfo("Text", IDesignInfo::TYPE_WIDESTR, true, true);

	v = CVar(VTYPE_STRING, "");
    di->AddPropInfo("DataSource", IDesignInfo::TYPE_STRING, true, true, &v);
	di->AddPropInfo("Font", IDesignInfo::TYPE_FONT, true, true);
	di->AddPropInfo("Cursor", IDesignInfo::TYPE_CURSOR, true, true);
	di->AddPropInfo("HintText", IDesignInfo::TYPE_WIDESTR, true, true);
	di->AddPropInfo("HintType", IDesignInfo::TYPE_STRING, true, true);
	di->AddPropInfo("Context", IDesignInfo::TYPE_STRING, true, true);
	
	di->AddPropInfo("NoFrame", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("AutoSize", IDesignInfo::TYPE_BOOL, true, true);
	v = CVar(VTYPE_STRING, "Tile");
	di->AddPropInfo("DrawMode", IDesignInfo::TYPE_STRING, true, true, &v);
	di->AddPropInfo("BackImage", IDesignInfo::TYPE_IMAGE, true, true);

    //��Ч����
    di->AddPropInfo("InSound", IDesignInfo::TYPE_SOUND, true, true);
    di->AddPropInfo("OutSound", IDesignInfo::TYPE_SOUND, true, true);
    di->AddPropInfo("ClickSound", IDesignInfo::TYPE_SOUND, true, true);
    di->AddPropInfo("DisableEnter", IDesignInfo::TYPE_BOOL, true, false);

	di->AddEventInfo("on_set_hint");
	di->AddEventInfo("on_set_context");
	di->AddEventInfo("on_get_capture");
    di->AddEventInfo("on_lost_capture");
    di->AddEventInfo("on_right_click");
    di->AddEventInfo("on_left_double_click");
}

bool IControl::GetLoadFinish()
{
    if (m_bLoadFinish)
    {
        return true;
    }

	if (!m_BackImage.IsLoadCompelte())
	{
		return false;
	}
	
	if (m_bAutoSize && m_BackImage.IsReady())
	{
		InnerSetWidth(m_BackImage.GetWidth());
		InnerSetHeight(m_BackImage.GetHeight());
	}

    m_bLoadFinish = true;

	return true;
}

//bool IControl::GetLoadFinishValue()
//{
//  return m_bLoadFinish;
//}

bool IControl::IsAncestorOf(IControl* pControl)
{
	IControl* parent = pControl->GetParentControl();

	while (parent)
	{
		if (parent == this)
		{
			return true;
		}

		parent = parent->GetParentControl();
	}

	return false;
}

bool IControl::InRectangle(int x, int y) const
{
	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();
	
	return (x >= x1) && (x < x2) && (y >= y1) && (y < y2);
}

IControl* IControl::GetControl(const PERSISTID& id) const
{
	IEntity* pEntity = GetCore()->GetEntity(id);
	
	if (NULL == pEntity)
	{
		return NULL;
	}
	
	if (!pEntity->GetEntInfo()->IsKindOf("IControl"))
	{
		return NULL;
	}
	
	return (IControl*)pEntity;
}

IControl* IControl::GetInWhat(int x, int y)
{
	if (!GetVisible())
	{
		return NULL;
	}
	
    // ����windows�Ĵ�������ʱ�ǲ�����Ϣ����ģ����ǲ���ʹ�ö���
    /*if (!GetEnabled() && !m_bDesignMode)
    {
        // ����ʱ,��������ʾ,��������Ϣ����
        return NULL;
    }*/

	// ���ж��Ƿ��ڱ��ؼ���Χ��
	if (!InRectangle(x, y))
	{
		return NULL;
	}
	
	return OnGetInWhat(x, y);
}

int IControl::OnKeyDown(unsigned int vk) 
{ 
	return 0; 
}

int IControl::OnKeyUp(unsigned int vk) 
{ 
	return 0; 
}

int IControl::OnLButtonDown(int x, int y) 
{ 
	return 0; 
}

int IControl::OnLButtonUp(int x, int y) 
{ 
    return GuiUtil_RunCallback(this, "on_left_click",  
		CVarList() << x << y);
}

int IControl::OnLButtonDoubleClick(int x, int y) 
{
    return GuiUtil_RunCallback(this, "on_left_double_click",  
		CVarList() << x << y);
}

int IControl::OnLButtonHover(int x, int y)
{
	return 0;
}

int IControl::OnRButtonDown(int x, int y) 
{ 
	return 0; 
}

int IControl::OnRButtonUp(int x, int y) 
{ 
	if (m_pDelegate != NULL)
	{
		m_pDelegate->OnSuborEvent(this, CGuiConst::EVENT_RIGHT_CLICK, 
			CVarList() << x << y);
	}

    return GuiUtil_RunCallback(this, "on_right_click", CVarList() << x << y);
}

int IControl::OnRButtonDoubleClick(int x, int y) 
{ 
	return 0; 
}

int IControl::OnRButtonHover(int x, int y)
{
	return 0;
}

int IControl::OnMouseWheel(int x, int y, int delta)
{
	return 0;
}

int IControl::OnMouseMove(int x, int y) 
{ 
	return 0; 
}

int IControl::OnDragMove(int x, int y) 
{ 
	return 0; 
}

int IControl::OnDropIn(int x, int y) 
{ 
	return 0; 
}

int IControl::OnDragLeave(int x, int y) 
{ 
	return 0; 
}

int IControl::OnChar(unsigned int ch)	
{ 
	return 0; 
}

int IControl::OnGetFocus(const PERSISTID& lost_id) 
{ 
	if (!m_bCanFocus)
	{
		return 0;
	}
	
	if (m_pDelegate != NULL)
	{
		return m_pDelegate->OnSuborEvent(this, 
			CGuiConst::EVENT_GET_FOCUS, CVarList() << lost_id);
	}
	
	return 1; 
}

int IControl::OnLostFocus(const PERSISTID& get_id) 
{ 
	if (!m_bCanFocus)
	{
		return 0;
	}
	
	if (m_pDelegate != NULL)
	{
		return m_pDelegate->OnSuborEvent(this, 
			CGuiConst::EVENT_LOST_FOCUS, CVarList() << get_id);
	}
	
	return 1; 
}

int IControl::OnAdd(const PERSISTID& child_id) 
{ 
	return 0; 
}

int IControl::OnRemove(const PERSISTID& child_id) 
{ 
	return 0; 
}

int IControl::OnEnter(const PERSISTID& parent_id)
{
	return 0;
}

int IControl::OnLeave(const PERSISTID& parent_id)
{
	return 0;
}

int IControl::OnGetCapture(const PERSISTID& lost_id) 
{ 
    if (!m_strInSound.empty())
    {
        // ������Ч
        m_pGui->PlayingSound(m_strInSound.c_str());
    }

    return GuiUtil_RunCallback(this, "on_get_capture", 
		CVarList() << lost_id);
}

int IControl::OnLostCapture(const PERSISTID& get_id) 
{
    if (!m_strOutSound.empty())
    {
        // ������Ч
        m_pGui->PlayingSound(m_strOutSound.c_str());
    }

    return GuiUtil_RunCallback(this, "on_lost_capture", 
        CVarList() << get_id);
}

int IControl::OnSuborEvent(IControl* subor, int event, 
	const IVarList& args) 
{ 
	return 0; 
}

int IControl::OnLeftChanged(int old)
{
	return 0; 
}

int IControl::OnTopChanged(int old)
{
	return 0; 
}

int IControl::OnWidthChanged(int old)
{
	return 0; 
}

int IControl::OnHeightChanged(int old)
{
	return 0; 
}

int IControl::OnSetHint(IControl* hint, int x, int y)
{
	if (m_wsHintText.empty())
	{
		return 0;
	}

	if (m_strHintType.empty())
	{
		return 0;
	}

	return GuiUtil_RunCallback(this, "on_set_hint", 
		CVarList() << hint->GetID() << x << y);
}

int IControl::OnSetContext(IControl* context)
{
	return GuiUtil_RunCallback(this, "on_set_context", 
		CVarList() << context->GetID());
}

int IControl::OnSetCursor(int x, int y)
{
	if (!m_pGui->SetCurrentCursor(GetCursorName()))
	{
		return 0;
	}

	return 1;
}

IControl* IControl::OnGetInWhat(int x, int y)
{
    // ���ж��ӿؼ������ӿؼ����򷵻��ӿؼ�
	if (!m_Subors.empty())
	{
		size_t subor_num = m_Subors.size();
		IControl** subors = &m_Subors[0];

		for (size_t i = 0; i < subor_num; ++i)
		{
			IControl* pControl = subors[i]->GetInWhat(x, y);

			if (pControl != NULL)
			{
				return pControl;
			}
		}
	}
	
    if (m_bDesignMode)
    {
        // ���״̬
        return this;
    }

    // �����ӿؼ����ж��Ƿ���������
    if (!TestTransparent(x, y))
    {
        // �˵㲻͸���ŷ���
        return this;
    }

	return NULL;
}

// �ж�ָ��λ���Ƿ�͸��
bool IControl::TestTransparent(int x, int y)
{
    if (!m_bTestTrans)
    {
        // ������͸�����ԣ��򷵻ز�͸��
        return false;
    }

    // Ĭ�Ͻ����Ա���ͼ����Ȼ����ؼ����������
    // �������Ҫ����TestTransparent����
    if (!HasBackImage())
    {
        // �����ͼƬ������Ϊ��͸��
        return false;
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

void IControl::SetInSound(const char* value)
{
    Assert(value != NULL);

    m_strInSound = value;
}

const char* IControl::GetInSound() const
{
    return m_strInSound.c_str();
}

void IControl::SetOutSound(const char* value)
{
    Assert(value != NULL);

    m_strOutSound = value;
}

const char* IControl::GetOutSound() const
{
    return m_strOutSound.c_str();
}

void IControl::SetClickSound(const char* value)
{
    Assert(value != NULL);

    m_strClickSound = value;
}

const char* IControl::GetClickSound() const
{
    return m_strClickSound.c_str();
}

bool IControl::RefreshInnerAbsPos()
{
    if (m_bRefreshInnerAbsPos)
    {
        m_nInnerAbsLeft = GetAbsLeft();
        m_nInnerAbsTop = GetAbsTop();

        m_bRefreshInnerAbsPos = false;
    }

    return true;
}

/*
bool IControl::AddCallback(const char* event, const char* func)
{
	Assert(event != NULL);
	Assert(func != NULL);

	if (NULL == m_pCallbacks)
	{
		m_pCallbacks = CORE_NEW(callback_container_t);
	}

	if (m_pCallbacks->Exists(event))
	{
		return false;
	}

	return m_pCallbacks->Add(event, func);
}

bool IControl::RemoveCallback(const char* event)
{
	Assert(event != NULL);

	if (NULL == m_pCallbacks)
	{
		return false;
	}

	return m_pCallbacks->Remove(event);
}

bool IControl::ClearCallback()
{
	if (NULL == m_pCallbacks)
	{
		return true;
	}

	m_pCallbacks->Clear();

	return true;
}

bool IControl::FindCallback(const char* event) const
{
	Assert(event != NULL);
	
	if (NULL == m_pCallbacks)
	{
		return false;
	}

	return m_pCallbacks->Exists(event);
}

const char* IControl::GetCallbackFunc(const char* event) const
{
	Assert(event != NULL);

	if (NULL == m_pCallbacks)
	{
		return "";
	}

	const char* func = m_pCallbacks->GetData(event);

	if (NULL == func)
	{
		return "";
	}

	return func;
}

size_t IControl::GetCallbackCount() const
{
	if (NULL == m_pCallbacks)
	{
		return 0;
	}

	return m_pCallbacks->GetCount();
}

size_t IControl::GetCallbackList(IVarList& result) const
{
	result.Clear();
	
	if (m_pCallbacks)
	{
		callback_container_t::iterator it = m_pCallbacks->Begin();
		callback_container_t::iterator end = m_pCallbacks->End();

		for (; it != end; ++it)
		{
			result.AddString(it.GetKey());
		}
	}

	return result.GetCount();
}
*/

void IControl::SetGuiHook(IGuiHook* value)
{
	SAFE_RELEASE(m_pGuiHook);
	m_pGuiHook = value;
}

IControl* IControl::QueryControl(const char* name) 
{
	unsigned int hash = GetHashValueCase(name);
	if ((GetHash() == hash) && (strcmp(GetName(), name) == 0))
	{
		return this;
	}
	return NULL;
}

// �Ƿ����Ϣ͸��
void IControl::SetTransparent(bool value)
{
	m_bTransparent = value;
}

bool IControl::GetTransparent() const
{
	return m_bTransparent;
}
