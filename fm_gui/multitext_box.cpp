//--------------------------------------------------------------------
// �ļ���:        MultiTextBox.cpp
// ��  ��:        ֧��HTML��ǩ��ͼ�Ļ��ſؼ�
// ˵  ��:        
// ��������:    2008��8��20��
// ������:        ����ƽ
// ��Ȩ����:    ������ţ�������޹�˾
//--------------------------------------------------------------------

#include "multitext_box.h"
#include "scroll_bar.h"
#include "imageanimation_manager.h"
#include "hyperlinkstyle_manager.h"
#include "../tools/string_util.h"
#include "gui_utils.h"
#include "../public/auto_mem.h"
#include "../public/converts.h"
#include "text.h"

/// entity: MultiTextBox
/// \brief �б��ʵ��
DECLARE_ENTITY(MultiTextBox, 0, IScrollable);
#if 1

/// property: string HAlign
/// \brief ˮƽ���뷽ʽ
DECLARE_PROPERTY(result_string, MultiTextBox, HAlign, GetHAlignString, 
    SetHAlignString);

/// property: string VAlign
/// \brief ��ֱ���뷽ʽ
DECLARE_PROPERTY(result_string, MultiTextBox, VAlign, GetVAlignString, 
    SetVAlignString);

/// property: string TextColor
/// \brief Ĭ���ı���ɫ ��:255.255.255.0
DECLARE_PROPERTY(result_string, MultiTextBox, TextColor, GetTextColor, 
    SetTextColor);
/// property: int Alpha
/// \brief �����ؼ���͸���� 
DECLARE_PROPERTY(int, MultiTextBox, Alpha, GetAlpha, SetAlpha);
///// property: bool Transparent
///// \brief �Ƿ�͸����͸���󽫲������κ���Ϣ 
//DECLARE_PROPERTY(bool, MultiTextBox, Transparent, GetTransparent, 
//    SetTransparent);
/// property: string SelectBarColor
/// \brief ѡ������ɫ
DECLARE_PROPERTY(result_string, MultiTextBox, SelectBarColor, 
    GetSelectBarColor, SetSelectBarColor);
/// property: string MouseInBarColor
/// \brief �����������ɫ
DECLARE_PROPERTY(result_string, MultiTextBox, MouseInBarColor, 
    GetMouseInBarColor, SetMouseInBarColor);

/// property: string SelectBarDraw
/// \brief ѡ����ͼƬ
DECLARE_PROPERTY(result_string, MultiTextBox, SelectBarDraw, GetSelectBarDraw, 
    SetSelectBarDraw);
/// property: string MouseInBarDraw
/// \brief ���������ͼƬ
DECLARE_PROPERTY(result_string, MultiTextBox, MouseInBarDraw, GetMouseInBarDraw, 
    SetMouseInBarDraw);

/// property: Rect ViewRect
/// \brief �ı���ʾ����
DECLARE_PROPERTY(result_string, MultiTextBox, ViewRect, GetViewRect, 
    SetViewRect);
/// property: int LineHeight
/// \brief �ı���ʾ����
DECLARE_PROPERTY(int, MultiTextBox, LineHeight, GetLineHeight, SetLineHeight);
/// property: bool AutoScroll
/// \brief �Զ�����,trueʱ�����һ����Ϣ���Զ�����������ڹ���
DECLARE_PROPERTY(bool, MultiTextBox, AutoScroll, GetAutoScroll, SetAutoScroll);
/// property: bool TipAutoSize
/// \brief �Զ��������ݶ��ٷ���
DECLARE_PROPERTY(bool, MultiTextBox, TipAutoSize, GetTipAutoSize, 
    SetTipAutoSize);
/// property: bool TipMaxWidth
/// \brief �Զ��������ݶ��ٷ���
DECLARE_PROPERTY(int, MultiTextBox, TipMaxWidth, GetTipMaxWidth, 
    SetTipMaxWidth);
/// property: wstring Text
/// \brief �ı���ʾ����
DECLARE_PROPERTY(const wchar_t*, MultiTextBox, HtmlText, GetHtmlText, 
    SetHtmlText);
/// property: string LineTextAlign
/// \brief �ı����и߷�Χ����ֱ�����ϵĶ��뷽ʽ
DECLARE_PROPERTY(result_string, MultiTextBox, LineTextAlign, GetLineTextAlign, 
    SetLineTextAlign);
/// property: int VerticalValue
/// \brief �ؼ���ǰ����ֵ
DECLARE_PROPERTY(int, MultiTextBox, VerticalValue, GetVerticalValue, 
    SetVerticalValue);
/// property: int VerticalMaxValue
/// \brief �ؼ���ǰ������ֵ
DECLARE_PROPERTY_GET(int, MultiTextBox, VerticalMaxValue, GetVerticalMaxValue);
/// property: bool TransNormalText
/// \brief ��ͨ�ı���Ϣ����
DECLARE_PROPERTY(bool, MultiTextBox, TransNormalText, GetTransNormalText, 
    SetTransNormalText);
/// property: bool TransImage
/// \brief ͼƬ��Ϣ����
DECLARE_PROPERTY(bool, MultiTextBox, TransImage, GetTransImage, SetTransImage);
/// property: bool TransHyperLink
/// \brief ��������Ϣ����
DECLARE_PROPERTY(bool, MultiTextBox, TransHyperLink, GetTransHyperLink, 
    SetTransHyperLink);

/// property: string DrawMode
/// \brief ����ͼԪ����ʾ��ʽ
DECLARE_PROPERTY(result_string, MultiTextBox, SelectBarDrawMode, GetSelectBarDrawModeString, 
    SetSelectBarDrawModeString);

/// property: string DrawMode
/// \brief ����ͼԪ����ʾ��ʽ
DECLARE_PROPERTY(result_string, MultiTextBox, MouseInBarDrawMode, GetMouseInBarDrawModeString, 
    SetMouseInBarDrawModeString);
    
/// property: int ItemCount
/// \brief �ı�item����
DECLARE_PROPERTY_GET(int, MultiTextBox, ItemCount, GetItemCount);
#endif
#if 0

/// \brief ����ı�
/// \method AddHtmlText(const wchar_t* szText, int nKey)
/// \param text �ı�
/// \param nkey ɸѡ��ֵ-1��Ϊ������ɸѡ��һֱ��ʾ
/// \return ƥ���������ֵ��-1��ʾδ�ҵ�
DECLARE_METHOD_2(int, MultiTextBox, AddHtmlText, const wchar_t*, int);
/// \brief ɾ��һ���ı�
/// \method DelHtmlItem(int index)
/// \param index �ı����
/// \return ɾ���ɹ�����true
DECLARE_METHOD_1(bool, MultiTextBox, DelHtmlItem, int);
/// \brief �ı�һ���ı�
/// \method ChangeHtmlText(int index, const wchar_t*pNewText)
/// \param index �ı����
/// \param pNewText �µĹ���
/// \return �ɹ�����true
DECLARE_METHOD_2(bool, MultiTextBox, ChangeHtmlText, int, const wchar_t*);
/// \brief �ı�һ���Ƿ��ѡ��
/// \method SetHtmlItemSelectable(int index, bool selectable)
/// \param index �ı����
/// \param selectable�Ƿ��ѡ��
/// \return �ɹ�����true
DECLARE_METHOD_2(bool, MultiTextBox, SetHtmlItemSelectable, int, bool);
/// \brief �ı�һ���Ƿ��ѡ��
/// \method SetHtmlItemSelected(int index, bool selectable)
/// \param index �ı����
/// \param selected�Ƿ�ѡ��
/// \return �ɹ�����true
DECLARE_METHOD_2(bool, MultiTextBox, SetHtmlItemSelected, int, bool);
/// \brief ��ȡһ���ı�
/// \method GetHtmlItemText(int index)
/// \param index �ı����
/// \return �ɹ������ı�, ʧ�ܷ���L""
DECLARE_METHOD_1(std::wstring, MultiTextBox, GetHtmlItemText, int);
/// \brief �����������
/// \method Clear()
DECLARE_METHOD_0(bool, MultiTextBox, Clear);
/// \brief �����ִ���ӿڴ�С������ʾ
/// \method Reset()
DECLARE_METHOD_0(bool, MultiTextBox, Reset);
/// \brief ���������������ֵ
/// \method GotoEnd()
DECLARE_METHOD_0(bool, MultiTextBox, GotoEnd);
/// \brief 
/// ��ʾ�˼�ֵ��Ӧ�ı���nKey == -1ʱ��ʾ��ʾ����
/// �˺������ظ���������ͬʱ��ʾ��ͬKeyֵ�����ݣ���Ҫ��ʾ��nKey�ᱻ��¼
/// ����-1ʱ��ʾ���У���ʱ��¼��Key���ᱻ���
/// \method bool ShowKeyItems(int nKey)
/// \param nKey �ؽ�ֵ
/// ����ֻ��ʾkeyΪ1������
/// ShowKeyItems(-1); // �����¼
/// ShowKeyItems(1);  // ������ʾ��ֵ
/// ���ͬʱҪ��ʾkeyΪ2��������ֻ���ټ���
/// ShowKeyItems(2)��
DECLARE_METHOD_1(bool, MultiTextBox, ShowKeyItems, int);
/// \brief ��ȡ���ѡ�е���������
/// \method GetSelectItemIndex()
DECLARE_METHOD_0(int, MultiTextBox, GetSelectItemIndex);
/// \brief ��ȡ�������������
/// \method GetMouseInItemIndex()
DECLARE_METHOD_0(int, MultiTextBox, GetMouseInItemIndex);
/// \brief ��ȡ���ѡ�е�ͼƬ���
/// \method GetSelectImageIndex()
DECLARE_METHOD_0(int, MultiTextBox, GetSelectImageIndex);
/// \brief ��ȡ��������ͼƬ���
/// \method GetMouseInImageIndex()
DECLARE_METHOD_0(int, MultiTextBox, GetMouseInImageIndex);
/// \brief ��ȡ��ǰ�ؼ�ĳ��������ʾ�ϱ�����ڿؼ�����ĸ߶�
/// \method GetItemTop()
DECLARE_METHOD_1(int, MultiTextBox, GetItemTop, int);
/// \brief ��ȡ��ǰ�ؼ�ĳ��������ʾ�߶�
/// \method GetItemHeight()
DECLARE_METHOD_1(int, MultiTextBox, GetItemHeight, int);
/// \brief ��ȡ��ǰ�ؼ����ݸ߶�
/// \method GetContentHeight()
DECLARE_METHOD_0(int, MultiTextBox, GetContentHeight);
/// \brief ��ȡ��ǰ�ؼ����ݿ��
/// \method GetContentWidth()
DECLARE_METHOD_0(int, MultiTextBox, GetContentWidth);
/// \brief ͨ����Ż�ȡkey
/// \method int GetItemKeyByIndex(int itemindex);
DECLARE_METHOD_1(int, MultiTextBox, GetItemKeyByIndex, int);
/// \brief ͨ��key��ȡ���
/// \method int GetItemIndexByKey(int key);
DECLARE_METHOD_1(int, MultiTextBox, GetItemIndexByKey, int);
/// \brief ��ȡͼƬ������Ϣ
/// \method int GetImageData(int itemindex, int imageindex);
/// \param itemindex ѡ�����
/// \param imageindex ѡ���е�ͼƬ���
DECLARE_METHOD_2(std::string, MultiTextBox, GetImageData, int, int);
/// \brief ��ȡͼƬ������Ϣ
/// \method left,top,width,height GetImagePlace(int itemindex, int imageindex);
/// \param itemindex ѡ�����
/// \param imageindex ѡ���е�ͼƬ���
DECLARE_METHOD_N(MultiTextBox, GetImagePlace);

/// \brief �޸�ͼƬ
/// \method bool ChangeImage(int itemindex, int imageindex, const char* image_res)
/// \param itemindex ѡ�����
/// \param imageindex ѡ���е�ͼƬ���
/// \param image_res �µ�ͼƬ
DECLARE_METHOD_3(bool, MultiTextBox, ChangeImage, int, int, const char*);

/// \brief ��ȡ�����Ӹ�����Ϣ
/// \method int GetHyperLinkData(int itemindex, int linkindex);
/// \param itemindex ѡ�����
/// \param linkindex ѡ���еĳ��������
DECLARE_METHOD_2(std::string, MultiTextBox, GetHyperLinkData, int, int);

/// \brief ��ȡ��������ʾ�ľ�������
/// \method bool,left,top,right,bottom GetHyperLinkRect(wstring key);
/// \param key ������NPC configid������������
DECLARE_METHOD_N(MultiTextBox, GetHyperLinkRect);
#endif

/// event: int on_select_item_change(object self, int newitemindex)
/// \brief ���ѡ����ı���Ϣ
/// \param self ���ؼ�
/// \param newitemindex ��ǰѡ�����
DECLARE_EVENT(on_select_item_change);
/// event: int on_mousemove(object self, string pos)
/// \brief ����ƶ�����Ϣ
/// \param self ���ؼ�
/// \param pos "x=%d y=%d"���ַ���
DECLARE_EVENT(on_mousemove);
/// event: int on_mouseout_image(object self, int itemindex, int old_select_index)
/// \brief ������Ƴ�ĳ��ͼƬʱ�Ļص�
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param old_select_index ��ǰѡ������ǰ��ѡ��ͼƬ����
DECLARE_EVENT(on_mouseout_image);
/// event: int on_mousein_image(object self, int itemindex, int new_index, int x, int y)
/// \brief �������ͼƬ
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param new_index �����ͼƬ����
DECLARE_EVENT(on_mousein_image);
/// event: int on_click_image(object self, int itemindex, int new_index)
/// \brief �����ͼƬ
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param new_index �����ͼƬ����
DECLARE_EVENT(on_click_image);
/// event: int on_doubleclick_image(object self, int itemindex, int new_index)
/// \brief ���˫��ͼƬ
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param new_index ˫����ͼƬ����
DECLARE_EVENT(on_doubleclick_image);
/// event: int on_mouseout_hyperlink(object self, int itemindex, string linkdata)
/// \brief ����Ƴ�������
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param linkdata ����������
DECLARE_EVENT(on_mouseout_hyperlink);
/// event: int on_mousein_hyperlink(object self, int itemindex, string linkdata, int x, int y)
/// \brief ������볬����
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param linkdata ����������
DECLARE_EVENT(on_mousein_hyperlink);
/// event: int on_click_hyperlink(object self, int itemindex, string linkdata)
/// \brief �����������
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param linkdata ����������
DECLARE_EVENT(on_click_hyperlink);
/// event: int on_doubleclick_hyperlink(object self, int itemindex, string linkdata)
/// \brief ���˫��������
/// \param self ���ؼ�
/// \param itemindex �ܵ�ѡ�����
/// \param linkdata ����������
DECLARE_EVENT(on_doubleclick_hyperlink);

/// event: int on_right_click_item(object self, int mousein_itemindex)
/// \brief �����������
/// \param self ���ؼ�
/// \param mousein_itemindex �ܵ�ѡ�����
DECLARE_EVENT(on_right_click_item);

/// event: int on_right_click_image(object self, int mousein_itemindex, int mousein_image)
/// \brief �����������
/// \param self ���ؼ�
/// \param mousein_itemindex �ܵ�ѡ�����
/// \param mousein_image 
DECLARE_EVENT(on_right_click_image);

/// event: int on_right_click_hyperlink(object self, int mousein_itemindex, string linkdata)
/// \brief �����������
/// \param self ���ؼ�
/// \param mousein_itemindex �ܵ�ѡ�����
/// \param linkdata ����������
DECLARE_EVENT(on_right_click_hyperlink);

/// event: int on_leftclick_down(object self, x, y)
/// \brief ����������ʱ����
/// \param self���ؼ�
DECLARE_EVENT(on_leftclick_down);
/// event: int on_leftclick_up(object self, x, y)
/// \brief ����������ʱ����
/// \param self���ؼ�
DECLARE_EVENT(on_leftclick_up);



MultiTextBox::MultiTextBox()
{
    //m_TextColor = GuiUtil_ColorToInt("255,255,0,255");
    m_TextColor = GuiUtil_ColorToInt("255,255,255,0");
    m_SelectBarColor = GuiUtil_ColorToInt("100,0,0,255");
    m_MouseInBarColor = GuiUtil_ColorToInt("100,255,255,0");

    // ������ͼƬ
    m_lCurMouseInImage = -1;
    m_lCurSelectImage = -1;
    // ��ǰ������г�����
    m_lCurMouseInHyperLink = -1;
    m_lCurSelectHyperLink = -1;
    // item
    m_lCurMouseInItem = -1;
    m_lCurSelectItem = -1;
    m_nTopIndex = 0;
    m_LineHeight = -1;

    GuiUtil_StringToRect("0,0,100,200", m_ViewRect);
    
    m_TextWidth = m_ViewRect.right - m_ViewRect.left;

    m_bAutoScroll = false;
    m_bTipAutoSize = false;

    m_nTipMaxWidth = 200;
    m_nLineTextAlign = 0;

    m_wcsHtmlText = L"";

    m_nAlpha = 256;
    m_nHAlign = CGuiConst::ALIGN_LEFT;
    m_nVAlign = CGuiConst::V_ANCHOR_TOP;
    //m_bTransparent = false;
	//SetTransparentDefault(false);
    m_bTransNormalText = false;
    m_bTransImage = false;
    m_bTransHyperlink = false;

    m_bIsMouseCaptured = false;
    m_bCanDelete = true;
    m_nSelectBarDrawMode = CGuiConst::DRAWMODE_FITWINDOW;
    m_nMouseInBarDrawMode = CGuiConst::DRAWMODE_FITWINDOW;
}

MultiTextBox::~MultiTextBox()
{
}

bool MultiTextBox::Shut()
{
    Clear();

    m_SelectBarDraw.Release();        // ���ѡ��
    m_MouseInBarDraw.Release();        // �������

    return IScrollable::Shut();
}

void MultiTextBox::PrepareDesignInfo(IDesignInfo* di)
{
    IScrollable::PrepareDesignInfo(di);

	CVar varProp;

    di->RemovePropInfo("Width");
    di->RemovePropInfo("Height");
    di->AddPropInfo("Width", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("Height", IDesignInfo::TYPE_INT, true, true);

	varProp.SetString("Left");
    di->AddPropInfo("HAlign", IDesignInfo::TYPE_STRING, true, true,    &varProp );

	varProp.SetString("Top");
	di->AddPropInfo("VAlign", IDesignInfo::TYPE_STRING, true, true,    &varProp );

    //�����Ƿ��й�����
    di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true);
    //di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("HasVScroll", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true);

	varProp.SetBool(false);
    di->AddPropInfo("AutoScroll", IDesignInfo::TYPE_BOOL, true, false, &varProp);

	varProp.SetBool(false);
	di->AddPropInfo("TipAutoSize", IDesignInfo::TYPE_BOOL, true, false, &varProp);

	varProp.SetInt(200);
    di->AddPropInfo("TipMaxWidth", IDesignInfo::TYPE_INT, true, false, &varProp);

    //�ı�Ĭ����ɫ
    //di->AddPropInfo("TextColor", IDesignInfo::TYPE_COLOR, true, true, 
    //    &CVar(VTYPE_STRING,"255,255,0,255"));
	varProp.SetString("255,255,255,0");
	di->AddPropInfo("TextColor", IDesignInfo::TYPE_COLOR, true, true, &varProp);

	varProp.SetString("100,0,0,255");
    di->AddPropInfo("SelectBarColor", IDesignInfo::TYPE_COLOR, true, true, &varProp);

	varProp.SetString("100,255,255,0");
    di->AddPropInfo("MouseInBarColor", IDesignInfo::TYPE_COLOR, true, true, &varProp);

    di->AddPropInfo("SelectBarDraw", IDesignInfo::TYPE_IMAGE, true, true);
    di->AddPropInfo("MouseInBarDraw", IDesignInfo::TYPE_IMAGE, true, true);
    //һ��Ҫ�裬���ӷ�Χ,�������Ĵ�С,һ�㲻Ҫ�����ؼ���Χ
    di->AddPropInfo("ViewRect", IDesignInfo::TYPE_STRING, true, true);

    //����ʱĬ��Ϊviewrect�Ŀ��
    //di->AddPropInfo("TextWidth", IDesignInfo::TYPE_INT, true, false);

    //����Ĭ��Ϊ14
	varProp.SetInt(16);
    di->AddPropInfo("LineHeight", IDesignInfo::TYPE_INT, true, true, &varProp);

	varProp.SetString("Bottom");
    di->AddPropInfo("LineTextAlign", IDesignInfo::TYPE_STRING, true, true, &varProp);

	varProp.SetBool(false);
    di->AddPropInfo("TransNormalText", IDesignInfo::TYPE_BOOL, true, false, &varProp);

	varProp.SetBool(false);
	di->AddPropInfo("TransImage", IDesignInfo::TYPE_BOOL, true, false, &varProp);

	varProp.SetBool(false);
	di->AddPropInfo("TransHyperLink", IDesignInfo::TYPE_BOOL, true, false, &varProp);

	varProp.SetString("FitWindow");
    di->AddPropInfo("SelectBarDrawMode", IDesignInfo::TYPE_STRING, true, true, &varProp);
    
	varProp.SetString("FitWindow");
	di->AddPropInfo("MouseInBarDrawMode", IDesignInfo::TYPE_STRING, true, true, &varProp);
    
    //���Բ���,�������跨
    //1���ļ�����text.html��ע������󼸸��ַ�Ϊ.htmlʱ�Żᱻ��Ϊ���ļ�
    //2ֱ����html�ű���<center><font color\"">fsfsf</font></center>// xml����ʱ��֧��
    //3��߲����ڳ����е�settext����
    di->AddPropInfo("HtmlText", IDesignInfo::TYPE_WIDESTR, true, true);

    di->AddSuborInfo("VScrollBar", "ScrollBar");

    di->AddEventInfo("on_select_item_change");
    di->AddEventInfo("on_mousemove");
    di->AddEventInfo("on_mouseout_image");
    di->AddEventInfo("on_mousein_image");
    di->AddEventInfo("on_click_image");
    di->AddEventInfo("on_doubleclick_image");

    di->AddEventInfo("on_mouseout_hyperlink");
    di->AddEventInfo("on_mousein_hyperlink");
    di->AddEventInfo("on_click_hyperlink");
    di->AddEventInfo("on_doubleclick_hyperlink");

    di->AddEventInfo("on_right_click_item");
    di->AddEventInfo("on_right_click_image");
    di->AddEventInfo("on_right_click_hyperlink");

	di->AddEventInfo("on_leftclick_down");
	di->AddEventInfo("on_leftclick_up");
}

bool MultiTextBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

    if (!m_SelectBarDraw.IsLoadCompelte())
    {
        return false;
    }

    if (!m_MouseInBarDraw.IsLoadCompelte())
    {
        return false;
    }
    
    return IScrollable::GetLoadFinish();
}

bool MultiTextBox::Init(const IVarList& args)
{
    if (!IScrollable::Init(args))
    {
        return false;
    }

    m_TextWidth = m_ViewRect.right - m_ViewRect.left;

    GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

    if (m_LineHeight < 0)
    {
        m_LineHeight = GetPainter()->GetTextHeight(NULL);
    }

    m_lCurLineHeight = m_LineHeight;

    SetSolid(false);

    return true;
}

// ���ô�ֱ���뷽ʽ
void MultiTextBox::SetVAlignString(const char* szText)
{
    if (szText == NULL)
    {
        return;
    }

    if (stricmp(szText, "Top") == 0)
    {
        m_nVAlign = CGuiConst::V_ANCHOR_TOP;
    }
    else if (stricmp(szText, "Center") == 0)
    {
        m_nVAlign = CGuiConst::V_ANCHOR_CENTER;
    }
    else if (stricmp(szText, "Bottom") == 0)
    {
        m_nVAlign = CGuiConst::V_ANCHOR_BOTTOM;
    }
}

result_string MultiTextBox::GetVAlignString() const
{
    switch (m_nVAlign)
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

// ����ˮƽ���뷽ʽ
void MultiTextBox::SetHAlignString(const char* szText)
{
    if (szText == NULL)
    {
        return;
    }

    if (stricmp(szText, "Left") == 0)
    {
        m_nHAlign = CGuiConst::ALIGN_LEFT;
    }
    else if (stricmp(szText, "Center") == 0)
    {
        m_nHAlign = CGuiConst::ALIGN_CENTER;
    }
    else if (stricmp(szText, "Right") == 0)
    {
        m_nHAlign = CGuiConst::ALIGN_RIGHT;
    }
}

result_string MultiTextBox::GetHAlignString() const
{
    switch (m_nHAlign)
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


void MultiTextBox::SetHtmlText(const wchar_t* wcsText)
{
    Assert(wcsText);

    if (!m_bCanDelete)
    {
        m_lCurMouseInItem = -1;
        m_lCurSelectItem = -1;
        // ������ͼƬ
        m_lCurMouseInImage = -1;
        m_lCurSelectImage = -1;
        // ��ǰ������г�����
        m_lCurSelectHyperLink = -1;
        m_lCurMouseInHyperLink = -1;
    }

    Clear();

    m_wcsHtmlText = wcsText;

    if (m_wcsHtmlText[0] == L'@' && m_wcsHtmlText.size() > 1)
    {
        result_wstring wcsBuffer;

        if (GetGui()->ConvertText(wcsText, wcsBuffer))
        {
            AddHtmlText(wcsBuffer.c_str(), -1);
        }
    }
    else
    {
        AddHtmlText(wcsText, -1);
    }
}

const wchar_t* MultiTextBox::GetHtmlText()
{
    return m_wcsHtmlText.c_str();
}

int MultiTextBox::AddHtmlText(const wchar_t* szText, int nKey)
{
    DrawHtmlItem htmlItem;

    if (szText == NULL || szText[0]==0)
    {
        htmlItem.m_ImageList.clear();
        htmlItem.m_TextList.clear();
        htmlItem.m_LinesLimits.clear();
        return -1;
    }

    if (szText != htmlItem.m_wcsText.c_str())
    {
        htmlItem.m_wcsText = szText;
        htmlItem.m_nKey = nKey;
    }

    //std::string szFace =     m_fontname.substr(0,4);
    //int        lFontSize =    atol(m_fontname.substr(4,2).c_str());

    //m_Html.Clean();
    if (!htmlItem.m_wcsText.empty())
    {
        std::wstring szTypeWords = L"";

        if (5 < htmlItem.m_wcsText.size()) 
        {
            szTypeWords = htmlItem.m_wcsText.substr(
                htmlItem.m_wcsText.size() - 5, 5);
        }

        if (wcscmp(szTypeWords.c_str(), L".html")==0)
        {
            size_t Size = htmlItem.m_wcsText.size() * 2 + 1;

            if (Size <= 1)
            {
                htmlItem.m_wcsText = L"";
                htmlItem.m_Html.SetText(L"");
                return -1;
            }

            TAutoMem<char, 256> auto_buf(Size);

            ToString(htmlItem.m_wcsText.c_str(), auto_buf.GetBuffer(), Size);

            htmlItem.m_Html.CreateFromHtml(auto_buf.GetBuffer(), m_TextColor, 
                GetFontName(), 12);
        }
        else
        {
            htmlItem.m_Html.SetText(htmlItem.m_wcsText.c_str(), m_TextColor, 
                GetFontName(),12);
        }
    }
    else
    {
        return -1;
    }

    VirtualDrawHTML(htmlItem,0,0);

    // ��ȡ��ǰitem�Ŀ�Ⱥ͸߶�
    htmlItem.m_ContentHeight = m_nMaxLines * m_LineHeight;

    if (m_nMaxLines > 1)
    {
        // ����ʱ���������������жϿ��
        int MaxWidth = 0;
        size_t text_num = htmlItem.m_TextList.size();

        if (text_num > 0)
        {
            for (size_t i = 0; i < text_num; ++i)
            {
                DRAWTEXTLINE& line = htmlItem.m_TextList[i];

                if (!line.m_TextLine.empty())
                {
                    size_t back = line.m_TextLine.size() - 1;
                    DRAWTEXTITEM textitem = line.m_TextLine[back];

                    if (MaxWidth < textitem.rect.right)
                    {
                        MaxWidth = textitem.rect.right;
                    }
                }
            }
        }

        size_t limit_num = htmlItem.m_LinesLimits.size();

        if (limit_num > 0)
        {
            for (size_t i = 0; i < limit_num; ++i)
            {
                DRAWLIMITLIST& limit = htmlItem.m_LinesLimits[i];

                if (!limit.m_OneLineLimits.empty())
                {
                    size_t back = limit.m_OneLineLimits.size() - 1;

                    if (MaxWidth < limit.m_OneLineLimits[back].m_Right)
                    {
                        MaxWidth = limit.m_OneLineLimits[back].m_Right;
                    }
                }
            }
        }

        htmlItem.m_ContentWidth = MaxWidth;
    }
    else
    {
        // ����ʱ�п�������ݵ�һ�е��ı���ͼƬ���������ȡ
        int MaxWidth = 0;

        if (!htmlItem.m_TextList.empty())
        {
            DRAWTEXTLINE& line = htmlItem.m_TextList[0];

            if (!line.m_TextLine.empty())
            {
                size_t back = line.m_TextLine.size() - 1;
                DRAWTEXTITEM textitem = line.m_TextLine[back];

                if (MaxWidth < textitem.rect.right)
                {
                    MaxWidth = textitem.rect.right;
                }
            }
        }

        if (!htmlItem.m_LinesLimits.empty())
        {
            DRAWLIMITLIST&limit = htmlItem.m_LinesLimits[0];

            if (!limit.m_OneLineLimits.empty())
            {
                size_t back = limit.m_OneLineLimits.size() - 1;
                
                if (MaxWidth < limit.m_OneLineLimits[back].m_Right)
                {
                    MaxWidth = limit.m_OneLineLimits[back].m_Right;
                }
            }
        }

        htmlItem.m_ContentWidth = MaxWidth;
    }

    m_HtmlItems.push_back(htmlItem);

    UpdateScroll();

    // ˢ����ʾλ��
    RefreshContentPos();

    return m_HtmlItems.size() - 1;
}

bool MultiTextBox::DelHtmlItem(size_t index)
{
    if (!m_bCanDelete)
    {
        return false;
    }

    if (index < m_HtmlItems.size())
    {
        ClearHtmlItem(m_HtmlItems[index]);
        m_HtmlItems.erase(m_HtmlItems.begin() + index);

        // ˢ����ʾλ��
        RefreshContentPos();

        // ˢ�¹�����
        UpdateScroll();
        return true;
    }

    return false;
}

bool MultiTextBox::ChangeHtmlText(size_t index, const wchar_t* szText)
{
    // �ص�����ô˺���ʱ
    if (!m_bCanDelete)
    {
        if (m_lCurMouseInItem == index)
        {
            m_lCurMouseInItem = -1;
            m_lCurSelectItem = -1;
            // ������ͼƬ
            m_lCurMouseInImage = -1;
            m_lCurSelectImage = -1;
            // ��ǰ������г�����
            m_lCurSelectHyperLink = -1;
            m_lCurMouseInHyperLink = -1;
        }
    }

    if (index < m_HtmlItems.size())
    {
        DrawHtmlItem& item = m_HtmlItems[index];

        ClearHtmlItem(item);

        item.m_Html.SetText(szText);

        VirtualDrawHTML(item, 0,0);

        item.m_ContentHeight = m_nMaxLines * m_LineHeight;

        return true;
    }

    return false;
}

bool MultiTextBox::ClearHtmlItem(DrawHtmlItem& htmlitem)
{
   std::vector<TextSegment>* pVSegments = htmlitem.m_Html.GetSegments();

    for (size_t i = 0; i < pVSegments->size(); ++i)
    {
        for (size_t k = 0; k < (*pVSegments)[i].m_ImageItemList.size(); ++k)
        {
            HtmlImageItem& imageItem = (*pVSegments)[i].m_ImageItemList[k];

            if (imageItem.m_bNeedRelease)
            {
                if (imageItem.m_nType == 0)
                {
                    //TGA
                    GetPainter()->ReleaseGlyph(imageItem.m_imageID);
                }
                else if (imageItem.m_nType == 1)
                {
                    //PNG
                    GetPainter()->ReleaseGlyph(imageItem.m_imageID);      
                }

                imageItem.m_bNeedRelease = false;
                imageItem.m_imageID = PERSISTID();
            }
        }
    }

    return true;
}

bool MultiTextBox::Clear()
{
    if (GetPainter() == NULL)
    {
        return true;
    }

    for (size_t j = 0; j < m_HtmlItems.size(); ++j)
    {
        ClearHtmlItem(m_HtmlItems[j]);
    }

    m_HtmlItems.clear();

    m_lCurMouseInItem = -1;
    m_lCurSelectItem = -1;
    // ������ͼƬ
    m_lCurMouseInImage = -1;
    m_lCurSelectImage = -1;
    // ��ǰ������г�����
    m_lCurSelectHyperLink = -1;
    m_lCurMouseInHyperLink = -1;

    SetVerticalValue(0);
    UpdateScroll();

    return true;
}

// ���ݵ�ǰ���������һ�°�,��Ҫ�ڿؼ���С�ı�ʱ����
bool MultiTextBox::Reset()
{
    size_t item_num = m_HtmlItems.size();
    
    for (size_t j = 0; j < item_num; ++j)
    {
        VirtualDrawHTML(m_HtmlItems[j], 0, 0);

        m_HtmlItems[j].m_ContentHeight = m_nMaxLines* m_LineHeight;
    }
    
    if (m_bAutoScroll)
    {
        GotoEnd();
    }
    else
    {
        SetVerticalValue(0);
    }

    return true;
}

bool MultiTextBox::GotoEnd()
{
    SetVerticalValue(GetVerticalMaxValue());

    if (GetVScrollBar() != PERSISTID())
    {
        IScrollable::UpdateScroll();
    }

    return true;
}

bool MultiTextBox::ShowKeyItems(int nKey)
{
    if (nKey == -1)
    {
        m_vShowKeys.clear();
    }
    else
    {
        m_vShowKeys.push_back(nKey);
    }

    UpdateScroll();

    return true;
}

const wchar_t* MultiTextBox::GetHtmlItemText(size_t index)
{
    if (index < m_HtmlItems.size())
    {
        return m_HtmlItems[index].m_wcsText.c_str();
    }

    return L"";
}

// ����ָ�����Ƿ��ѡ��
bool MultiTextBox::SetHtmlItemSelectable(size_t index, bool bSelectable)
{
    if (index < m_HtmlItems.size())
    {
        DrawHtmlItem& item = m_HtmlItems[index];

        item.m_bSelectable = bSelectable;

        return true;
    }

    return false;
}

bool MultiTextBox::SetHtmlItemSelected(size_t index, bool bSelect)
{
    if (index < 0)
    {
        m_lCurSelectItem = -1;
        m_lCurSelectHyperLink = -1;
        m_lCurSelectImage = -1;
        return true;
    }

    if (index >= m_HtmlItems.size())
    {
        return false;
    }

    if (index == m_lCurSelectItem)
    {
        if (bSelect)
        {
            return true;
        }
        else
        {
            m_lCurSelectItem = -1;
            m_lCurSelectHyperLink = -1;
            m_lCurSelectImage = -1;
            return true;
        }
    }
    else
    {
        m_lCurSelectItem = index;
        m_lCurSelectHyperLink = -1;
        m_lCurSelectImage = -1;
        return true;
    }

    return false;
}

// ������
void MultiTextBox::SetFontName(const char* val)
{
    IControl::SetFontName(val);

    GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

    int nLineHeight = GetPainter()->GetTextHeight(NULL);

    if (m_LineHeight < nLineHeight)
    {
        m_LineHeight = nLineHeight;
        m_lCurLineHeight = m_LineHeight;
    }
}

void MultiTextBox::SetTextColor(const char* szColor)
{
    m_TextColor = GuiUtil_ColorToInt(szColor);
}

result_string MultiTextBox::GetTextColor()
{
    return GuiUtil_IntToColor(m_TextColor);
}

// ����Ĭ����ɫ
void MultiTextBox::SetSelectBarColor(const char* szColor)
{
    m_SelectBarColor = GuiUtil_ColorToInt(szColor);
}

result_string MultiTextBox::GetSelectBarColor()
{
    return GuiUtil_IntToColor(m_SelectBarColor).c_str();
}

// ����Ĭ����ɫ
void MultiTextBox::SetMouseInBarColor(const char* szColor)
{
    m_MouseInBarColor = GuiUtil_ColorToInt(szColor);
}

result_string MultiTextBox::GetMouseInBarColor()
{
    return GuiUtil_IntToColor(m_MouseInBarColor).c_str();
}

void MultiTextBox::SetSelectBarDraw(const char* pDraw)
{
    Assert(pDraw != NULL);

    m_SelectBarDraw.SetName(pDraw);
    m_SelectBarDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* MultiTextBox::GetSelectBarDraw()
{
    return m_SelectBarDraw.GetName();
}

void MultiTextBox::SetMouseInBarDraw(const char* pDraw)
{
    Assert(pDraw != NULL);

    m_MouseInBarDraw.SetName(pDraw);
    m_MouseInBarDraw.Create(GetPainter(), GetGui(), GetAsyncLoad());
}

const char* MultiTextBox::GetMouseInBarDraw()
{
    return m_MouseInBarDraw.GetName();
}

// ������ʾ������:0,0,120,40
void MultiTextBox::SetViewRect(const char* RectStr)
{
    GuiUtil_StringToRect(RectStr, m_ViewRect);

    m_TextWidth = m_ViewRect.right - m_ViewRect.left;

    if (m_TextWidth < 0)
    {
        m_ViewRect.left = 0;
        m_ViewRect.right = GetWidth();
        m_ViewRect.top = 0;
        m_ViewRect.bottom = GetHeight();
    }
}

result_string MultiTextBox::GetViewRect()
{
    return GuiUtil_RectToString(m_ViewRect).c_str();
}

void MultiTextBox::SetLineHeight(int height)
{
    m_LineHeight = height;
    m_lCurLineHeight = m_LineHeight;

    GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

    int nLineHeight = GetPainter()->GetTextHeight(NULL);

    if (m_LineHeight < nLineHeight)
    {
        m_LineHeight = nLineHeight;
        m_lCurLineHeight = m_LineHeight;
    }
}

const int MultiTextBox::GetLineHeight()
{
    return m_LineHeight;
}

// ͸����
void MultiTextBox::SetAlpha(int alpha)
{
    if (alpha < 0 || alpha > 256)
    {
        return;
    }

    m_nAlpha = alpha;
}

const int MultiTextBox::GetAlpha()
{
    return m_nAlpha;
}

//// ͸��
//void MultiTextBox::SetTransparent(bool bTrans)
//{
//    m_bTransparent = bTrans;
//}
//
//const bool MultiTextBox::GetTransparent()
//{
//    return m_bTransparent;
//}

const char* MultiTextBox::GetLineTextAlign()
{
    if (m_nLineTextAlign == 0)
    {
        return "Bottom";
    }
    else if (m_nLineTextAlign == 1)
    {
        return "Center";
    }
    else
    {
        return "Top";
    }
}

void MultiTextBox::SetLineTextAlign(const char* pAlign)
{
    if (stricmp(pAlign, "Top") == 0)
    {
        m_nLineTextAlign = 2;
    }
    else if (stricmp(pAlign, "Center") == 0)
    {
        m_nLineTextAlign = 1;
    }
    else
    {
        m_nLineTextAlign = 0;
    }
}

int MultiTextBox::GetItemKeyByIndex(int index)
{
    if ((size_t)index <  m_HtmlItems.size())
    {
        return m_HtmlItems[index].m_nKey;
    }

    return -1;
}

int MultiTextBox::GetItemIndexByKey(int key)
{
    size_t item_num = m_HtmlItems.size();
    
    for (size_t i = 0; i < item_num; ++i)
    {
        if (m_HtmlItems[i].m_nKey == key)
        {
            return (int)i;
        }
    }

    return -1;
}

bool MultiTextBox::IsFound(const wchar_t* sub, const wchar_t* parent)
{
    return wcsstr(parent, sub) != NULL;
    //size_t nResult = parent.find(sub);

    //if (nResult >= 0 && nResult < parent.size())
    //{
    //    return true;
    //}
    
    //return false;
}

void MultiTextBox::ProcessY(bool IsLineChanged, DrawHtmlItem& htmlitem, 
    int& y, point_t& point, HtmlImageItem* pimage)
{
    rect_t rRect(pimage->m_Rect);

    if (IsLineChanged)
    {
        // ���е�ʱ��Ҫ������һ�еĸ߶�
        int addheight = ((m_lCurLineHeight % m_LineHeight) == 0) 
            ? m_lCurLineHeight : (m_lCurLineHeight/m_LineHeight + 1) * m_LineHeight;

        point.y = y + addheight;

        y = point.y;

        // �����и�
        if (pimage->m_nOnly == 2)
        {
            int imageheight = rRect.bottom - rRect.top;

            if (imageheight % m_LineHeight > 0)
            {
                m_lCurLineHeight = 
                    (imageheight / m_LineHeight + 1) * m_LineHeight;
            }
            else
            {
                m_lCurLineHeight = 
                    (imageheight / m_LineHeight) * m_LineHeight;
            }

            // �и߸ı䣬�����´���ͬ���ϵ�ͼƬ����
            ChangeMaxLineHeight(htmlitem, y, m_lCurLineHeight);
        }
    }
    else    // û����
    {             
        point.y = y;

        if (pimage->m_nOnly == 2)
        {
            int imageheight = rRect.bottom - rRect.top;
            // ��ͬ���ı�һ���ռ��
            if (m_lCurLineHeight < imageheight)
            {
                if (imageheight % m_LineHeight > 0)
                {
                    m_lCurLineHeight = (imageheight / m_LineHeight + 1) 
                        * m_LineHeight;
                }
                else
                {
                    m_lCurLineHeight = (imageheight / m_LineHeight) 
                        * m_LineHeight;
                }

                // �и߸ı䣬�����´���ͬ���ϵ�ͼƬ����
                ChangeMaxLineHeight(htmlitem, y, m_lCurLineHeight);
            }
        }
    }
}


// �и߸ı�ʱͼƬ���뷽ʽ��ͬ�߶������µ���
bool MultiTextBox::ChangeMaxLineHeight(DrawHtmlItem& htmlitem, 
        const int top_y, const int new_line_height)
{
    // ����ͬ��ͼƬ����뷽ʽ����λ�øı�
    int count = htmlitem.m_ImageList.size();

    for(int i = count - 1; i >= 0; --i)
    {
        DRAWIMAGEITEM& draw_imageitem = htmlitem.m_ImageList[i];

        if (draw_imageitem.y < top_y)
        {
            break;
        }
        else if (draw_imageitem.imageItem.m_VAlign == L"" ||
            wcsicmp(draw_imageitem.imageItem.m_VAlign.c_str(), L"top") == 0)
        {
            continue;
        }

        // ʵ�ʵ�ͼƬ����y����, ���и��ϰ����뷽ʽ�����¶���
        int real_y = GetImageVAlignPos(&draw_imageitem.imageItem, top_y, new_line_height);

        // ��Ϊ�߶��޸����������������
        AppendLimits(htmlitem, draw_imageitem, draw_imageitem.y, real_y);

        draw_imageitem.y = real_y;
    }

    // ����ͬ���ı�����뷽ʽ���߶ȸı�
    int last_line = (int)htmlitem.m_TextList.size() - 1;

    if (last_line >= 0)
    {
        if (htmlitem.m_TextList[last_line].nTopY == top_y)
        {
            // �����и�
            htmlitem.m_TextList[last_line].nMaxLineHeight = new_line_height;

            if (m_nLineTextAlign < 2)
            {
                // �����ı���������
                std::vector<DRAWTEXTITEM>& text_lines = 
                    htmlitem.m_TextList[last_line].m_TextLine;

                size_t text_count = text_lines.size();

                for (size_t i = 0; i < text_lines.size(); ++i)
                {
                    text_lines[i].rect.bottom = top_y + new_line_height;
                }
            }
        }

    }
    return true;
}

inline int get_line_limit_size(MultiTextBox::DrawHtmlItem& htmlitem, int line_index)
{
    if (line_index >= (int)htmlitem.m_LinesLimits.size())
    {
        return 0;
    }

    return htmlitem.m_LinesLimits[line_index].m_OneLineLimits.size();
}

point_t* MultiTextBox::SetImagePos(DrawHtmlItem& htmlitem, int& x,int& y, 
    HtmlImageItem* pimage)
{
    Assert(pimage);

    static point_t point;

    point.x = 0;
    point.y = 0;

    int Start = x;

    rect_t rRect(pimage->m_Rect);

    bool bResultO = pimage->m_nOnly == 1;// ��ռ��־//��ռĬ�Ͼ���
    bool bResultL = IsFound(L"left", pimage->m_HAlign.c_str());// ������־
    bool bResultC = IsFound(L"center", pimage->m_HAlign.c_str());// ���б�־
    bool bResultR = IsFound(L"right", pimage->m_HAlign.c_str());// �Ҷ����־

    // ��ռʱ���������
    if (bResultO)
    {
        // �����ռһ��
        int yLine = (y/m_LineHeight) > (int)htmlitem.m_LinesLimits.size()
            ? y / (m_LineHeight) : (int)htmlitem.m_LinesLimits.size();
        
        if (Start != 0)
        {
            yLine += m_lCurLineHeight / m_LineHeight;
        }

        if (bResultL)
        {
            point.x = 0;
            point.y = yLine * m_LineHeight;
        }
        else if (bResultR)
        {
            point.x = m_TextWidth - (rRect.right - rRect.left);
            point.y = yLine * m_LineHeight;
        }
        else
        {
            point.x = (m_TextWidth - (rRect.right - rRect.left)) / 2;
            point.y = yLine * m_LineHeight;
        }

        if ((rRect.bottom - rRect.top) % m_LineHeight > 0)
        {
            y = (yLine + (rRect.bottom - rRect.top) / m_LineHeight + 1) 
                * m_LineHeight;
        }
        else
        {
            y = (yLine + (rRect.bottom - rRect.top) / m_LineHeight) 
                * m_LineHeight;
        }

        x = 0;

        return &point;
    }

    // ��ǰ��������
    if (y / m_LineHeight >= (int)htmlitem.m_LinesLimits.size() 
        || y / m_LineHeight < 0)
    {
        if (bResultL)
        {
            if (x > 0)
            {
                point.y = (y / m_LineHeight + 1) * m_LineHeight;
            }
            else
            {
                point.y = (y / m_LineHeight) * m_LineHeight;
            }

            point.x = 0;

            return &point;
        }
        else if (bResultC)
        {
            int left = (m_TextWidth - (rRect.right - rRect.left)) / 2;

            if (x > left)
            {
                point.y = (y / m_LineHeight + m_lCurLineHeight / m_LineHeight) 
                    * m_LineHeight;
            }
            else
            {
                point.y = (y / m_LineHeight) * m_LineHeight;
            }

            point.x = left;

            return &point;
        }
        else if (bResultR)
        {
            int left = m_TextWidth - (rRect.right - rRect.left);

            if (x > left) 
            {
                point.y = (y / m_LineHeight + m_lCurLineHeight / m_LineHeight) 
                    * m_LineHeight;
            }
            else
            {
                point.y = (y / m_LineHeight) * m_LineHeight;
            }

            point.x = left;

            return &point;
        }
        else
        {
            if (x + (rRect.right - rRect.left) <= m_TextWidth)
            {
                // ��ǰ��������
                point.x = x;
                point.y = y;
                x = x + (rRect.right - rRect.left);//�����������

                int imageheight = pimage->m_Rect.bottom - pimage->m_Rect.top;

                if (pimage->m_nOnly == 2)
                {
                    // ��ͬ���ı�һ���ռ��
                    // �����ռһ��
                    //if (m_lCurLineHeight < imageheight)
                    //{
                        //int imageheight = pimage->m_Rect.bottom - pimage->m_Rect.top;

                    if (m_lCurLineHeight < imageheight)
                    {
                        if (imageheight % m_LineHeight > 0)
                        {
                            m_lCurLineHeight = 
                                (imageheight / m_LineHeight + 1) * m_LineHeight;
                        }
                        else
                        {
                            m_lCurLineHeight = 
                                (imageheight / m_LineHeight) * m_LineHeight;
                        }

                        // �и߸ı䣬�����´���ͬ���ϵ�ͼƬ����
                        ChangeMaxLineHeight(htmlitem, y, m_lCurLineHeight);
                    }
                }

                return &point;
            }
            else
            {
                // ��ǰ�зŲ���,ת����һ��
                point.x = 0;
                point.y = y + m_lCurLineHeight;
                y = point.y;

                if (pimage->m_nOnly == 2)
                {
                    // ��ͬ���ı�һ���ռ��
                    // �����ռһ��
                    int imageheight = pimage->m_Rect.bottom - pimage->m_Rect.top;

                    if (m_lCurLineHeight < imageheight)
                    {
                        //if (m_lCurLineHeight < imageheight)
                        //{
                        if (imageheight % m_LineHeight > 0)
                        {
                            m_lCurLineHeight = 
                                (imageheight / m_LineHeight + 1 ) * m_LineHeight;
                        }
                        else
                        {
                            m_lCurLineHeight = imageheight;
                        }

                        // �и߸ı䣬�����´���ͬ���ϵ�ͼƬ����
                        ChangeMaxLineHeight(htmlitem, y, m_lCurLineHeight);
                    }
                }
                
                x = point.x + (rRect.right - rRect.left);

                return &point;
            }

        }
    }
    else//��ǰ��������ʱ
    {
        int nWidth = 0;
        bool bFirst = true;

        // �ӵ�ǰ�п�ʼ�����ж�
        for (int i = y / m_LineHeight; ; ++i)
        {
            // �����һ��ͬ��ʱ���������
            if (i == y / m_LineHeight)
            {
                Start = x;
            }
            else
            {
                int lines = ((m_lCurLineHeight % m_LineHeight) == 0) 
                    ? (m_lCurLineHeight / m_LineHeight) 
                    : (m_lCurLineHeight / m_LineHeight + 1);

                i += lines -1;

                m_lCurLineHeight = lines * m_LineHeight;

                y = i * m_LineHeight;

                Start = 0;
            }

            // һ����������,Ĭ�Ͼ���һ�������ϵ������,����L���Ǿ���
            int nLimitSize = htmlitem.m_LinesLimits.size();
            int nSize = 0;

            if (i < nLimitSize)
            {
                nSize = get_line_limit_size(htmlitem, i);
            }

            if (nSize!=0)
            {
                if (bResultL)//��
                {
                    if (Start != 0 
                        || htmlitem.m_LinesLimits[i].m_OneLineLimits[0].m_Left - Start < (rRect.right - rRect.left))
                    {
                        continue;
                    }
                    else
                    {
                        point.x = 0;
                        point.y = i * m_LineHeight;

                        return &point;
                    }
                }
                else if (bResultC)//��
                {
                    int left = (m_TextWidth - (rRect.right - rRect.left)) / 2;
                    int right = left + (rRect.right - rRect.left);

                    int j = 0;

                    if (Start > left) 
                    {
                        continue;//����ռ�ÿ���
                    }

                    while (j < nSize && htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left <= right)
                    {
                        j++;  //�ѱ�����ռ��
                    }

                    if (j == nSize && htmlitem.m_LinesLimits[i].m_OneLineLimits[j - 1].m_Right > left)
                    {
                        continue;
                    }

                    if (j < nSize)
                    {
                        if(j != 0)//j==0��Ȼ����
                        {
                            if (htmlitem.m_LinesLimits[i].m_OneLineLimits[j-1].m_Right > left) 
                            {
                                continue;
                            }
                        }
                    }

                    point.x =left;
                    point.y = m_LineHeight * i;
                    return &point;
                }
                else if (bResultR)//��
                {
                    int left = m_TextWidth - (rRect.right - rRect.left); 

                    if (Start > left) 
                    {
                        continue;
                    }

                    int j = 0;
                    
                    while (j < nSize && htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Right < left)
                    {
                        j++;
                    }

                    if (j < nSize) 
                    {
                        continue;
                    }

                    point.x = left;
                    point.y = i * m_LineHeight;

                    return &point;
                }

                // Ĭ��,������ǰ�е�������
                for (int j = 0; j < nSize; j++)
                {
                    if (htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left > Start)
                    {
                        //����ʼ���ҵ�������,��Ϊ��������������ÿ���
                        if (bFirst)
                        {
                            // �ĵ���һ���ڻ��Ƶ��ҵ�������
                            nWidth = htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left - Start;
                            bFirst = false;

                            if (nWidth > (rRect.right - rRect.left))
                            {
                                // �пռ����
                                point.x = Start;

                                ProcessY(i != y / m_LineHeight, htmlitem, y, point, pimage);

                                // ���ܻ�������x��������Ҫ���
                                x = point.x + (rRect.right - rRect.left);
                                return &point;
                            }
                        }
                        else
                        {
                            // �м���������
                            if (j == 0)
                            {
                                nWidth = htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left;
                            }
                            else
                            {
                                nWidth = htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left 
                                    - htmlitem.m_LinesLimits[i].m_OneLineLimits[j - 1].m_Right;
                            }

                            if (nWidth > (rRect.right - rRect.left))
                            {
                                if (j == 0)
                                {
                                    point.x = 0;

                                    m_lCurLineHeight = m_LineHeight;
                                }
                                else
                                {
                                    point.x = htmlitem.m_LinesLimits[i].m_OneLineLimits[j - 1].m_Right;
                                }

                                ProcessY(i != y / m_LineHeight, htmlitem, y, point, pimage);

                                x = point.x + (rRect.right - rRect.left);

                                return &point;

                            }
                        }//�м���������
                    }//end>Start
                    
                    if (j == nSize - 1)
                    {
                        // ���һ��������֮��Ŀռ�
                        if (j == nSize - 1)
                        {
                            if (Start >= htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Right)
                            {
                                 if (m_TextWidth - Start > (rRect.right - rRect.left))
                                 {

                                     point.x = Start;
                                     point.y = i * m_LineHeight;

                                     ProcessY(i != y / m_LineHeight, htmlitem, y, point, pimage);

                                     x = point.x + (rRect.right - rRect.left);

                                     return &point;
                                 }
                            }
                            else if (m_TextWidth - htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Right > (rRect.right - rRect.left))
                            {
                                point.x = htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Right;
                            
                                point.y = i* m_LineHeight;

                                ProcessY(i != y / m_LineHeight, htmlitem, y, point, pimage);

                                x = point.x + (rRect.right - rRect.left);

                                return &point;
                            }
                        }
                    }
                }//end for
            }
            else    // һ����������
            {
                // ����
                if (bResultC)
                {
                    if (Start > (m_TextWidth - (rRect.right - rRect.left)) / 2)
                    {
                        continue;
                    }
                    else
                    {
                        point.x = (m_TextWidth - (rRect.right - rRect.left)) / 2;
                        point.y = m_LineHeight * i;

                        point.y = GetImageVAlignPos(pimage, point.y, m_lCurLineHeight);

                        return &point;
                    }
                }// �Ҷ���
                else if (bResultR)
                {
                    if (m_TextWidth - Start < (rRect.right - rRect.left))
                    {
                        continue;
                    }
                    else
                    {
                        point.x = m_TextWidth - (rRect.right - rRect.left);
                        point.y = m_LineHeight * i;

                        point.y = GetImageVAlignPos(pimage, point.y, m_lCurLineHeight);

                        return &point;
                    }
                }// �����
                else if (bResultL)
                {
                    if (Start > 0)
                    {
                        continue;
                    }
                    else
                    {
                        point.x = 0;
                        m_lCurLineHeight = m_LineHeight;
                        point.y = m_LineHeight * i;

                        point.y = GetImageVAlignPos(pimage, point.y, m_lCurLineHeight);

                        return &point;
                    }
                }// �Զ������
                else
                {
                    if (m_TextWidth - (rRect.right - rRect.left) >= Start || // �ռ乻����ʾ
                        m_TextWidth - (rRect.right - rRect.left) < 0)   // ���пռ䶼��ʾ��������ǿ����ʾ
                    {
                        point.x = Start;
                        point.y = m_LineHeight * i;

                        ProcessY(i != y / m_LineHeight, htmlitem, y, point, pimage);

                        x = point.x + (rRect.right - rRect.left);

                        point.y = GetImageVAlignPos(pimage, point.y, m_lCurLineHeight);

                        return &point;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
    }

    return NULL;
}

void MultiTextBox::SetImageLimit(DrawHtmlItem& htmlitem, point_t* pPoint, 
    HtmlImageItem* pImage)
{
    Assert(pImage && pPoint);

    rect_t rRect(pImage->m_Rect);

    // �������������
    int nCurLine = pPoint->y / m_LineHeight;

    // ����ͼƬ����������Ҫ������
    int use_height = rRect.bottom - rRect.top + pPoint->y - nCurLine * m_LineHeight;

    int nNeedLine = (use_height % m_LineHeight == 0 ? 
        use_height / m_LineHeight : 
        use_height / m_LineHeight + 1) + nCurLine;

    if (nNeedLine > (int)htmlitem.m_LinesLimits.size())
    {
        htmlitem.m_LinesLimits.resize(nNeedLine);
    }

    // ��˳���������,������м������
    for (int i = nCurLine; i < nNeedLine; i++)
    {
        DRAWLIMIT limit;

        if (pImage->m_nOnly == 1)
        {
            limit.m_Left = 0;
            limit.m_Right = m_TextWidth;
        }
        else
        {
            limit.m_Left = pPoint->x;
            limit.m_Right = pPoint->x + (rRect.right - rRect.left);
        }

        bool find = false;

        int count = get_line_limit_size(htmlitem, i);

        for (int j = 0; j < count; j++)
        {
            if (htmlitem.m_LinesLimits[i].m_OneLineLimits[j].m_Left 
                >= pPoint->x + (rRect.right - rRect.left))
            {
                htmlitem.m_LinesLimits[i].m_OneLineLimits.insert(
                    htmlitem.m_LinesLimits[i].m_OneLineLimits.begin() + j,limit);
                find = true;
                break;
            }
        }

        if (!find)
        {
            htmlitem.m_LinesLimits[i].m_OneLineLimits.push_back(limit);
        }
    }
}

// ��ͼƬλ�����и߸ı���ı�ʱ�ж��Ƿ񸽼�����
bool MultiTextBox::AppendLimits(
        DrawHtmlItem& htmlitem,
        DRAWIMAGEITEM& draw_imageitem, 
        int old_y, int new_y)
{
    // ��������----------------------------------------
    int old_bottom = old_y + 
        draw_imageitem.imageItem.m_Rect.bottom - 
        draw_imageitem.imageItem.m_Rect.top;

    int new_bottom = new_y + 
        draw_imageitem.imageItem.m_Rect.bottom - 
        draw_imageitem.imageItem.m_Rect.top;

    int new_limit_bottom_line = (new_bottom + m_LineHeight - 1) / m_LineHeight;
    int old_limit_bottom_line = (old_bottom + m_LineHeight - 1) / m_LineHeight;

    if (new_limit_bottom_line > old_limit_bottom_line)
    {
        if (new_limit_bottom_line > (int)htmlitem.m_LinesLimits.size())
        {
            htmlitem.m_LinesLimits.resize(new_limit_bottom_line);
        }

        for(int l = old_limit_bottom_line; l < new_limit_bottom_line; ++l)
        {
            DRAWLIMIT limit;

            limit.m_Left = draw_imageitem.x;

            limit.m_Right = draw_imageitem.x + 
                            draw_imageitem.imageItem.m_Rect.right - 
                            draw_imageitem.imageItem.m_Rect.left;

            std::vector<DRAWLIMIT>& vLineLimits = 
                htmlitem.m_LinesLimits[l].m_OneLineLimits;

            size_t l_i = 0;

            // ����Ƿ�Ҫ����ǰ��
            for (; l_i < vLineLimits.size(); ++l_i)
            {
                if (limit.m_Right <= vLineLimits[l_i].m_Left)
                {
                    vLineLimits.insert(
                        vLineLimits.begin() + l_i, limit);
                    break;
                }
            }

            // ǰ�治�ܷţ��������
            if (l_i >= vLineLimits.size())
            {
                vLineLimits.push_back(limit);
            }
        }
    }

    return true;
}

// ��ȡͼƬ���������¶�����yֵ
int MultiTextBox::GetImageVAlignPos(HtmlImageItem* pimage, int line_y, 
    const int cur_line_height)
{
    int image_height = pimage->m_Rect.bottom - pimage->m_Rect.top;

    if (cur_line_height > image_height)
    {
        //IsFound(L"top", pimage->m_VAlign);// Ĭ�Ͼ����϶���
        if (IsFound(L"center", pimage->m_VAlign.c_str()))
        {
            line_y += (m_lCurLineHeight - image_height) / 2;
        }
        else if (IsFound(L"bottom", pimage->m_VAlign.c_str()))
        {
            line_y += (m_lCurLineHeight - image_height);
        }
    }

    return line_y;
}

void MultiTextBox::CorrectShowString(std::wstring& wcsBuffer, 
    std::wstring& wcsDraw, int lWidth)
{
    //return;
    std::wstring wcsNextWChar;

    wcsNextWChar.resize(1);
    wcsNextWChar[0] = wcsBuffer[0];

    // ������пռ䣬���ַ���ӵ���ʾ�ַ����������ʾ��ֱ�������пռ�
    while (lWidth - GetPainter()->GetTextWidth(wcsDraw.c_str()) > 
        GetPainter()->GetTextWidth(wcsNextWChar.c_str()))
    {
        wcsDraw += wcsBuffer.substr(0, 1);

        if (wcsBuffer.size() == 1)
        {
            wcsBuffer=L"";
            return;
        }

        wcsBuffer = wcsBuffer.substr(1, wcsBuffer.size()-1);

        wcsNextWChar[0] = wcsBuffer[0];
    }

    // ���������е�ʱ�������ո�Ż���
    if (GetGui()->GetWordChangeLine())
    {
        for (int i = wcsDraw.size() - 1; i > 0; --i)
        {
            if (wcsDraw[i] == L' ')
            {
                wcsBuffer = wcsDraw.substr(i + 1, wcsDraw.size() - i - 1) 
                    + wcsBuffer;
                wcsDraw = wcsDraw.substr(0, i + 1);
                break;
            }

        }
    }

}

void MultiTextBox::CorrectInterpunction(std::wstring& szBuffer, 
    std::wstring& szDraw)
{
    wchar_t wChar = 0;
    size_t lFind = 0;
    // ����ʱ���б���ж�
    if (szBuffer.size() > 0)
    {
        wChar = szBuffer[0];
      
// ����д������ڴ�й©��ʾ
//        //������׾��Ǵ���������ŵ���һ��β
//        const static std::wstring Chinese = L"!\"'},.>:;?])��`������������";
//
//        lFind = Chinese.find(wChar);
//
//        if (lFind >= 0 && lFind < Chinese.size())
//        {
//            szDraw += wChar;
//            szBuffer = szBuffer.substr(1, szBuffer.size());
//        }
        
        //static wchar_t s_wsChinese[] = L"!\"'},.>:;?])��`������������\0";
		static std::wstring s_wsChinese = ((Text*)GetCore()->GetEntity(GetGui()->GetTextManagerID()))->GetText("CorrectInterpunction1");

        const wchar_t* wsFind = wcschr(s_wsChinese.c_str(), wChar);

        if (wsFind != NULL)
        {
            szDraw += wChar;
            szBuffer = szBuffer.substr(1, szBuffer.size());
        }
    }
     
    if (szDraw.size() > 0)
    {
        //ȡ��һ�����һ���ַ�
        wChar = szDraw[szDraw.size() - 1];

// ����д������ڴ�й©��ʾ
//        // ��������溯���е�����һ��������ַ�Ų����һ��
//        const static std::wstring ChineseBeg = L"([��[{";
//
//        lFind = ChineseBeg.find(wChar);
//
//        if (lFind >= 0 && lFind < ChineseBeg.size())
//        {
//            szDraw.resize(szDraw.size() - 1);
//            szBuffer = wChar + szBuffer;
//        }
        
        //static wchar_t s_wsChineseBeg[] = L"([��[{\0";
		static std::wstring s_wsChineseBeg = ((Text*)GetCore()->GetEntity(GetGui()->GetTextManagerID()))->GetText("CorrectInterpunction");

        const wchar_t* wsFind = wcschr(s_wsChineseBeg.c_str(), wChar);

        if (wsFind != NULL)
        {
            szDraw.resize(szDraw.size() - 1);
            szBuffer = wChar + szBuffer;
        }
    }
}

void MultiTextBox::DrawImageItem(DrawHtmlItem& htmlitem, int xbase,int ybase, 
    int& x,int& y,HtmlImageItem* pItem, bool bCenter)
{
    //����Ǿ�������<center>����ǿ�ƽ����뷽ʽ�ĳɾ��У�������Ķ��뷽ʽ��������
    if (bCenter)
    {
        pItem->m_nOnly = 1;
        pItem->m_HAlign = L"center";
    }

    //1.��ȡռ������
    GetImageItemWidth(pItem);

    //2.����ռ������λͼƬ
    // pBeginPointΪͼƬռ�ø�����ϵ�
    point_t* pBeginPoint = SetImagePos(htmlitem, x, y, pItem);

    // ʵ�ʵ�ͼƬ����y����, ���и��ϰ����뷽ʽ�����¶���
    int real_y = GetImageVAlignPos(pItem, 
        pBeginPoint->y / m_LineHeight * m_LineHeight, m_lCurLineHeight);

    //3����ͼƬ,û������¾ʹ���,

    /*�����������
    m_rs->DrawTex2D( pItem->m_imageID, 
    xbase+pPoint->x,
    ybase+pPoint->y, 
    pItem->m_Rect, 
    0xffffffff,  NULL );*/

    //4����������
    SetImageLimit(htmlitem, pBeginPoint, pItem);

    DRAWIMAGEITEM hDrawImageItem;

    hDrawImageItem.x = pBeginPoint->x;
    hDrawImageItem.y = real_y;
    hDrawImageItem.imageItem = *pItem;

    htmlitem.m_ImageList.push_back(hDrawImageItem);
}

PERSISTID MultiTextBox::CreateGlyph(const char* pName, int& type, 
    int& width, int& height)
{
    Assert(pName);

    IPainter* pPainter = GetPainter();

    Assert(pPainter);

    PERSISTID id = pPainter->CreateGlyph(pName, false);

    // ���Դ���PNG
    if (pPainter->GetGlyphValid(id))
    {
        type = 1;   //PNG
        width = pPainter->GetGlyphWidth(id);
        height = pPainter->GetGlyphHeight(id);
        return id;
    }

    // ���Դ���TGA
    //id = pPainter->CreateTexture(pName);

    //if (pPainter->GetTextureValid(id))
    //{
    //    type = 0;   //TGA
    //    width = pPainter->GetTextureWidth(id);
    //    height = pPainter->GetTextureHeight(id);
    //    return id;
    //}

    type = -1;

    return PERSISTID();
}

void MultiTextBox::RealDrawText(const char* font, dword color, 
    int x, int y, const wchar_t* szDraw)
{
    if (stricmp(font, "Default") == 0)
    {
        // �������ΪĬ��,��Ĭ����ɿؼ���������
        GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
    }
    else
    {
        GetGui()->SetFont(font, GetShadowColor());
    }

    int FontHeight = GetPainter()->GetTextHeight(NULL);
    //m_rs->WPrint(font,color,x,y+(m_lCurLineHeight-FontHeight)/2,0.0f,L"%s",szDraw);
    int y1 = y;

    if (m_nLineTextAlign == 0)
    {
        // ����
        y1 = m_lCurLineHeight - FontHeight + y;
    }
    else if (m_nLineTextAlign == 1)
    {
        y1 = (m_lCurLineHeight - FontHeight) / 2 + y;
    }

    bool bResult = GetPainter()->WriteText(x, y1, color, szDraw);

    if (bResult)
    {
        int i = 0;
    }
}

//�����������Ԥ��
void MultiTextBox::VirtualDrawText(DrawHtmlItem& htmlitem, const char* font,
    dword color, int x,int y, const wchar_t* szDraw, bool IsHyperLink)
{
    if (stricmp(font, "Default") == 0)
    {
        // �������ΪĬ��,��Ĭ����ɿؼ���������
        GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
    }
    else
    {
        GetGui()->SetFont(font, GetShadowColor());
    }

    int FontHeight = GetPainter()->GetTextHeight(NULL);

    FontHeight = FontHeight > m_LineHeight ? FontHeight : m_LineHeight;
    FontHeight = FontHeight > m_lCurLineHeight ? FontHeight : m_lCurLineHeight;

    int StringWidth = GetPainter()->GetTextWidth(szDraw);

    DRAWTEXTITEM hDrawTextItem;

    hDrawTextItem.rect.left = x;
    hDrawTextItem.rect.top = y;
    hDrawTextItem.rect.right = x + StringWidth;
    hDrawTextItem.rect.bottom = y + FontHeight;

    hDrawTextItem.szFont = font;
    hDrawTextItem.dColor = color;
    hDrawTextItem.szText = szDraw;

    if (htmlitem.m_TextList.empty() 
        || y > htmlitem.m_TextList.back().m_TextLine[0].rect.top)
    {
        DRAWTEXTLINE hDrawTextLine;

        hDrawTextLine.m_TextLine.push_back(hDrawTextItem);
        htmlitem.m_TextList.push_back(hDrawTextLine);
    }
    else
    {
        htmlitem.m_TextList.back().m_TextLine.push_back(hDrawTextItem);
    }

    if (htmlitem.m_TextList.back().nMaxLineHeight < m_lCurLineHeight)
    {
        htmlitem.m_TextList.back().nMaxLineHeight = m_lCurLineHeight;
        htmlitem.m_TextList.back().nTopY = y;
    }

    if (IsHyperLink)
    {
        // ����ǳ���������볬���Ӷ���
        if (!htmlitem.m_vHyperLinks.empty() 
            && htmlitem.m_vHyperLinks.back().m_nLinkIndex == m_lCurLink)
        {
            point_t pt;

            pt.x = htmlitem.m_TextList.size() - 1;
            pt.y = htmlitem.m_TextList.back().m_TextLine.size() - 1;

            htmlitem.m_vHyperLinks.back().m_OneLink.push_back(pt);
        }
    }
}

int MultiTextBox::GetCanShowWordsNum(std::wstring& wcsBuffer, int width)
{
    size_t char_num = 0;
    int cur_width = 0;
    int begin_index = 0;

    while(true)
    {
        // ��ȡ��һ����������ַ�wchart����
        int one_word_cnum = GetPainter()->GetNextLanguageCharacter(
            wcsBuffer.c_str(), begin_index);

        if (one_word_cnum <= 0)
        {
            return char_num;
        }

        // ������һ����ʼλ
        begin_index += one_word_cnum;

        // 
        int test_char_num = char_num + one_word_cnum;

        cur_width = GetPainter()->GetTextLenWidth(wcsBuffer.c_str(), 
            test_char_num);

        if (cur_width > width)
        {
            return char_num;
        }
        else
        {
            char_num = test_char_num;
        }

        if (char_num >= wcsBuffer.size())
        {
            return char_num;
        }
    }

    return 0;
}

int MultiTextBox::GetTextItemWidth(HtmlTextItem* pItem)
{
    std::string szFont = "";

    if (pItem->m_IsHyperLink)
    {
        HyperLinkStyleManager* pHLinkManager = InnerGetHyperlinkMng();

        if (NULL == pHLinkManager)
        {
            return 0;
        }

        StyleElement* pStyleElement = NULL;

        if (pHLinkManager->GetStyleByState(pItem->m_szHPStyle.c_str(), 
            HYPERLINK_NORMAL, pStyleElement))
        {
            szFont = pStyleElement->szFont;
        }
    }

    if (szFont == "")
    {
        szFont = "Default";
    }

    if (stricmp(szFont.c_str(), "Default") == 0)
    {
        // �������ΪĬ��,��Ĭ����ɿؼ���������
        szFont = GuiUtil_GetValidFontName(this);
    }

    GetGui()->SetFont(szFont.c_str(), GetShadowColor());

    int nStrWidth = GetPainter()->GetTextWidth(pItem->m_wtext.c_str());

    // �ֵ�Ĭ��
    GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());

    return (int)nStrWidth;
}

//
int MultiTextBox::GetImageItemWidth(HtmlImageItem* pItem)
{
    //1.��ȡռ������
    if (pItem->m_imageID.IsNull() && pItem->m_nType != 2)
    {
        size_t length = pItem->m_imagesrc.size() * 2 + 1;

        if (length <= 1)
        {
            return 0;
        }

        TAutoMem<char, 256> auto_buf_file(length);

        char* pBuffer = auto_buf_file.GetBuffer();

        ToString(pItem->m_imagesrc.c_str(), pBuffer, length);

#if 0	//ImageAnimationManager
        // ֻ�������һ��ʵ��
        ImageAnimationManager* pAniManager = InnerGetAnimation();

        if (NULL != pAniManager)
        {
            if (pAniManager->IsAnimation(pBuffer))
            {
                point_t pt = pAniManager->GetAnimationSize(pBuffer);

                pItem->m_Rect = MAKE_RECT(0, 0, pt.x, pt.y);
                pItem->m_nType = 2;//��ʾanimation
                pItem->m_bNeedRelease = false;
                pItem->m_bIsAnimation = true;

                return pItem->m_Rect.right;
            }
        }
#endif

        pItem->m_imageID = CreateGlyph(pBuffer, pItem->m_nType, 
            pItem->m_Rect.right, pItem->m_Rect.bottom);

        if (pItem->m_nType > -1)
        {
            pItem->m_Rect.left = 0;
            pItem->m_Rect.top = 0;
            pItem->m_bNeedRelease = true;
        }

        return pItem->m_Rect.right;
    }

    return pItem->m_Rect.right;
}


//�����������е�
bool MultiTextBox::DrawTextSegmentCenter(DrawHtmlItem& htmlitem, int xbase, 
    int ybase, int& x, int& y, TextSegment* pSegment)
{
    std::vector<int> vWidth;

    vWidth.resize(pSegment->m_processlist.size());

    int txt_index = 0;
    int image_index = 0;

    int full_width = 0;

    for (size_t i = 0; i < pSegment->m_processlist.size(); ++i)
    {
        if (pSegment->m_processlist[i])
        {
            vWidth[i] = GetTextItemWidth(
                &pSegment->m_TextItemList[txt_index++]);
        }
        else
        {
            vWidth[i] = GetImageItemWidth(
                &pSegment->m_ImageItemList[image_index++]);
        }

        full_width += vWidth[i];
    }

    // �����ǰ�������ף���Ϊ����У������軻��
    if (x != 0)
    {
        y += m_lCurLineHeight;
    }

    // ��ǰ���ݳ���ȷʵС���п��ɾ���
    if (m_TextWidth > full_width)
    {
        // ȷʵ���Ծ���
        x = (m_TextWidth - full_width) / 2;

        txt_index = 0;
        image_index = 0;

        for (size_t i = 0; i < pSegment->m_processlist.size(); ++i)
        {
            if (pSegment->m_processlist[i])
            {
                DrawTextItem(htmlitem, xbase, ybase, x, y, 
                    &pSegment->m_TextItemList[txt_index++], false);
            }
            else
            {
                DrawImageItem(htmlitem, xbase, ybase, x, y, 
                    &pSegment->m_ImageItemList[image_index++], false);
            }
        }

        return true;
    }

    return false;
}

void MultiTextBox::DrawTextItem(DrawHtmlItem& htmlitem, int xbase, int ybase, 
    int& x, int& y, HtmlTextItem* pItem, bool bCenter)
{
    // m_lCurLineHeight���ֵʼ�����иߵı�����������Ϊ����һ�е�ʱ�䣬
    // һ�н�������ݵ�ǰ���ֵĸ߶�������߶ȵıȽ��ж��Ƿ�ı����ֵ
    // �����з�limit�����ڻ�����
    if (pItem->m_wtext.size() == 0)
    {
        size_t length = pItem->m_text.size();

        if (length > 0)
        {
            TAutoMem<wchar_t, 256> auto_buf2(length + 1);
            wchar_t* pWBuffer = auto_buf2.GetBuffer();

            ToWideStr(pItem->m_text.c_str(), pWBuffer, length);

            pItem->m_wtext = pWBuffer;
        }
        else
        {
            pItem->m_wtext = L"";
            return;
        }
    }
#if 0	//HyperLink
    if (pItem->m_IsHyperLink)
    {
        HyperLinkStyleManager* pHLinkManager = InnerGetHyperlinkMng();

        if (NULL == pHLinkManager)
        {
            return;
        }

        StyleElement* pStyleElement = NULL;

        if (pHLinkManager->GetStyleByState(pItem->m_szHPStyle.c_str(), 
            HYPERLINK_NORMAL, pStyleElement))
        {
            pItem->m_Color = pStyleElement->color;
            pItem->m_Face = pStyleElement->szFont;
            pItem->m_FontSize = pStyleElement->size;
        }
        
        m_lCurLink++;

        HYPERLINKITEM hyperlink;

        hyperlink.m_nLinkIndex = m_lCurLink;
        hyperlink.m_wcsLink = pItem->m_wcsLink;
        hyperlink.m_szHPStyle = pItem->m_szHPStyle;

        htmlitem.m_vHyperLinks.push_back(hyperlink);
    }
#endif

    std::wstring szBuffer = pItem->m_wtext;
    dword color = m_TextColor;
    std::wstring szDraw = L"";

    color = pItem->m_Color;

    char szFont[20] = "";

    size_t length = pItem->m_wtext.size();

    m_CurFont = pItem->m_Face;

    if (m_CurFont == "")
    {
        m_CurFont = "Default";
    }

    if (stricmp(m_CurFont.c_str(), "Default") == 0)
    {
        // �������ΪĬ��,��Ĭ����ɿؼ���������
        m_CurFont = GuiUtil_GetValidFontName(this);
    }

    GetGui()->SetFont(m_CurFont.c_str(), GetShadowColor());

    {
        int FontHeight = GetPainter()->GetTextHeight(NULL);

        if (FontHeight > m_lCurLineHeight)
        {
            if (FontHeight % m_LineHeight>0)
            {
                m_lCurLineHeight = (FontHeight / m_LineHeight + 1) 
                    * m_LineHeight;
            }
            else
            {
                m_lCurLineHeight = FontHeight+2;
            }
        }
    }

    bool firsttime = true;
    int nLine = y / m_LineHeight;

    //�����������
    /*if (bCenter && GetPainter()->GetTextWidth(szBuffer.c_str()) < m_TextWidth)
    {
        y = nLine > (int)htmlitem.m_LinesLimits.size() ?
            nLine * m_LineHeight : 
            (int)htmlitem.m_LinesLimits.size() * m_LineHeight;

        if (x != 0)
        {
            y += m_LineHeight;
        }

        int nStrWidth = GetPainter()->GetTextWidth(szBuffer.c_str());

        x = (m_TextWidth - nStrWidth) / 2;

        VirtualDrawText(htmlitem,m_CurFont.c_str(), color, x, y, 
            szBuffer.c_str(), pItem->m_IsHyperLink);

        x = 0;
        y += m_lCurLineHeight;

        m_lCurLineHeight = m_LineHeight;

        return;
    }*/

    int nlimitLineSize = htmlitem.m_LinesLimits.size();

    for (; nLine < nlimitLineSize; 
        nLine += m_lCurLineHeight/m_LineHeight, m_lCurLineHeight = m_LineHeight)
    {
        y = m_LineHeight* nLine;

        int nSize = htmlitem.m_LinesLimits[nLine].m_OneLineLimits.size();

        if (nSize > 0)
        {
            // ����һ��-----------------------------------------------------------------
            for (int nLimit = 0; nLimit < nSize; nLimit++)
            {
                // ����Ҫ�жϵ�ǰ��x�Ƿ�Ϸ���Ҫ��������ȷ��λ��
                if (firsttime)
                {
                    firsttime = false;

                    while (nLimit<nSize && htmlitem.m_LinesLimits[y / m_LineHeight].m_OneLineLimits[nLimit].m_Left < x)
                    {
                        nLimit++;
                    }

                    if (nLimit >= nSize)
                    {
                        if (htmlitem.m_LinesLimits[y / m_LineHeight].m_OneLineLimits[nLimit - 1].m_Right>x)
                        {
                            x = htmlitem.m_LinesLimits[y / m_LineHeight].m_OneLineLimits[nLimit - 1].m_Right;
                        }

                        break;
                    }
                }

                // 1�����һ����������ǰ�Ŀ�϶
                if (nLimit == 0)
                {
                    // ��ȡʣ�µĿ��
                    bool bIsItemEnd = false;
                    int lWidth = htmlitem.m_LinesLimits[nLine].m_OneLineLimits[nLimit].m_Left - x;

                    // ���������������ʾ���ٸ�����
                    int ndrawsize = GetCanShowWordsNum(szBuffer, lWidth);

                    if (ndrawsize > 0)
                    {
                        if (ndrawsize >= (int)szBuffer.size())
                        {
                            szDraw = szBuffer;
                            bIsItemEnd = true;
                        }
                        else
                        {
                            bIsItemEnd = false;

                            szDraw = szBuffer.substr(0, ndrawsize);
                            szBuffer = szBuffer.substr(ndrawsize, 
                                szBuffer.size() - ndrawsize);

                            CorrectShowString(szBuffer, szDraw, lWidth);
                        }

                        if (szDraw.size() > 0)
                        {
                            VirtualDrawText(htmlitem, m_CurFont.c_str(),
                                color, xbase + x, ybase + y,szDraw.c_str(), 
                                pItem->m_IsHyperLink);
                        }

                        if (bIsItemEnd)
                        {
                            x += GetPainter()->GetTextWidth(szBuffer.c_str());
                            return;
                        }
                    }

                    x = htmlitem.m_LinesLimits[nLine].m_OneLineLimits[nLimit].m_Right;

                    if (szBuffer.size()==0)
                    {
                        return;
                    }
                }
                else // 2����ǰ������������֮ǰ������֮��Ŀ�϶
                {
                    bool bIsItemEnd = false;
                    int lWidth = (htmlitem.m_LinesLimits[nLine].m_OneLineLimits[nLimit].m_Left - x);
                    int ndrawsize = GetCanShowWordsNum(szBuffer, lWidth);

                    if (ndrawsize > 0)
                    {
                        if (ndrawsize >= (int)szBuffer.size())
                        {
                            szDraw = szBuffer;
                            bIsItemEnd = true;                    
                        }
                        else
                        {
                            szDraw = szBuffer.substr(0, ndrawsize);
                            szBuffer = szBuffer.substr(ndrawsize, 
                                szBuffer.size() - ndrawsize);

                            CorrectShowString(szBuffer, szDraw, lWidth);
                        }

                        if (szDraw.size() > 0)
                        {
                            VirtualDrawText(htmlitem, m_CurFont.c_str(), color,
                                xbase + x, ybase + y, szDraw.c_str(), 
                                pItem->m_IsHyperLink);
                        }

                        if (bIsItemEnd)
                        {
                            x += GetPainter()->GetTextWidth(szBuffer.c_str());
                            return;
                        }
                    }

                    x = htmlitem.m_LinesLimits[nLine].m_OneLineLimits[nLimit].m_Right;
                }

                if (szBuffer.size()==0)
                {
                    return;
                }
            }// end for

            // 3��߻�Ҫ�������һ����������֮��Ŀ�϶
            // �����������п�϶�������������
            int lWidth = m_TextWidth - x;
            int ndrawsize = GetCanShowWordsNum(szBuffer, lWidth);

            if (ndrawsize > 0)
            {
                bool bIsItemEnd = false;
                if (ndrawsize >= (int)szBuffer.size())
                {
                    szDraw = szBuffer;
                    bIsItemEnd = true;
                }
                else
                {
                    szDraw = szBuffer.substr(0, ndrawsize);
                    szBuffer = szBuffer.substr(ndrawsize, 
                        szBuffer.size() - ndrawsize);

                    CorrectShowString(szBuffer, szDraw, lWidth);
                    CorrectInterpunction(szBuffer, szDraw);

                    if (szBuffer.size() == 0)
                    {
                        bIsItemEnd = true;
                    }
                }
                //if (bIsDraw)
                {
                    VirtualDrawText(htmlitem, m_CurFont.c_str(), color,
                        xbase + x, ybase + y,szDraw.c_str(), 
                        pItem->m_IsHyperLink);
                }

                if (bIsItemEnd)
                {
                    x += GetPainter()->GetTextWidth(szDraw.c_str());
                    return;
                }
                else
                {
                    x=0;
                }
                
                // ����ʱ�ж�lCurLineHeight�Ƿ�Ҫ�ı�
                int FontHeight = GetPainter()->GetTextHeight(NULL);
                /*if (FontHeight % m_LineHeight>0)
                {
                    m_lCurLineHeight = (FontHeight/m_LineHeight+1) *m_LineHeight;
                }
                else
                {
                    m_lCurLineHeight = FontHeight+2;
                }*/

                if (FontHeight > m_lCurLineHeight)
                {
                    m_lCurLineHeight = FontHeight+2;
                }
            }
            else//����������޿�϶�����ƣ���X�ص�����
            {
                x=0;
            }
            // end����һ��--------------------------------------------------------
        }
        else // ���û�����������������
        {
            bool bIsItemEnd = false;
            int lWidth = m_TextWidth - x;

            int ndrawsize = GetCanShowWordsNum(szBuffer, lWidth);

            if (ndrawsize >= (int)szBuffer.size())
            {
                szDraw = szBuffer;
                bIsItemEnd = true;
            }
            else
            {
                szDraw = szBuffer.substr(0, ndrawsize);
                szBuffer = szBuffer.substr(ndrawsize, 
                    szBuffer.size() - ndrawsize);

                CorrectShowString(szBuffer, szDraw, m_TextWidth - x);
                CorrectInterpunction(szBuffer, szDraw);

                if (szBuffer.size() == 0)
                {
                    bIsItemEnd = true;
                }
            }    

            if (szDraw.size() > 0)
            {
                VirtualDrawText(htmlitem, m_CurFont.c_str(), color, xbase + x,
                    ybase + y, szDraw.c_str(), pItem->m_IsHyperLink);
            }

            if (bIsItemEnd)
            {
                x += GetPainter()->GetTextWidth(szDraw.c_str());
                return;
            }
            else
            {
                x = 0;
            }
        }
    }// end�����з�limit�����ڻ�����endfor

    // ���limit���������������δ����,����������������еĻ���
    if (szBuffer.size() > 0)
    {
        y = m_LineHeight * nLine;

        while (true)
        {
            bool bIsItemEnd = false;
            int lWidth = m_TextWidth - x;

            int ndrawsize = GetCanShowWordsNum(szBuffer, lWidth);

			// ������Ϊ��,����Ȼһ����Ҳ��ʾ����ʱ, ��ʾ����һ���ַ�
			if (lWidth == m_TextWidth &&
				ndrawsize == 0)
			{
				ndrawsize = 1; 
			}

            if (ndrawsize >= (int)szBuffer.size())
            {
                szDraw = szBuffer;
                bIsItemEnd = true;
            }
            else
            {
                szDraw = szBuffer.substr(0, ndrawsize);
                szBuffer = szBuffer.substr(ndrawsize, 
                    szBuffer.size() - ndrawsize);

                CorrectShowString(szBuffer, szDraw, m_TextWidth-x);
                CorrectInterpunction(szBuffer, szDraw);

                if (szBuffer.size() == 0)
                {
                    bIsItemEnd = true;
                }
            }    
            
            if (szDraw.size())
            {
                VirtualDrawText(htmlitem, m_CurFont.c_str(), color, xbase + x,
                    ybase + y, szDraw.c_str(), pItem->m_IsHyperLink);
            }

            if (bIsItemEnd)
            {
                x += GetPainter()->GetTextWidth(szDraw.c_str());
                break;
            }
            else
            {
                x  = 0;
                y += m_lCurLineHeight;
                nLine += m_lCurLineHeight/m_LineHeight;

                // ����ʱ�ж�lCurLineHeight�Ƿ�Ҫ�ı�
                int FontHeight = GetPainter()->GetTextHeight(NULL);

                if (m_lCurLineHeight < FontHeight)
                {
                    if (FontHeight % m_LineHeight>0)
                    {
                        m_lCurLineHeight = (FontHeight / m_LineHeight + 1) 
                            * m_LineHeight;
                    }
                    else
                    {
                        m_lCurLineHeight = FontHeight + 2;
                    }
                }
            }
        }
    }
}

void MultiTextBox::VirtualDrawHTML(DrawHtmlItem& htmlitem, int xbase, int ybase)
{
    // ͼƬֻ����

    m_lCurLink = 0;

    htmlitem.m_vHyperLinks.clear();
    htmlitem.m_ImageList.clear();
    htmlitem.m_TextList.clear();
    htmlitem.m_LinesLimits.clear();

    // ��ǰ���Ƶ�
    int x = 0, y = 0;

    //����
    std::vector<TextSegment>* pTextShow = htmlitem.m_Html.GetSegments();

    int nSegSize = pTextShow->size();

    for (int i = 0; i < nSegSize; i++)
    {
        int nCurText = 0, nCurImage = 0;

        int nPListSize = (*pTextShow)[i].m_processlist.size();

        m_lCurLineHeight = m_LineHeight;

        bool bCenterDraw = false;

        if ((*pTextShow)[i].m_bCenter)
        {
            bCenterDraw = DrawTextSegmentCenter(htmlitem, xbase, ybase, x, y, 
                &(*pTextShow)[i]);
        }

        if (!bCenterDraw)
        {
            for (int j = 0; j < nPListSize; j++)
            {
                if ((*pTextShow)[i].m_processlist[j])
                {
                    DrawTextItem(htmlitem, xbase, ybase, x, y, 
                        &(*pTextShow)[i].m_TextItemList[nCurText],
                        (*pTextShow)[i].m_bCenter);

                    nCurText++;
                }
                else
                {
                    DrawImageItem(htmlitem, xbase,ybase, x, y, 
                        &(*pTextShow)[i].m_ImageItemList[nCurImage],
                        (*pTextShow)[i].m_bCenter);

                    nCurImage++;
                }
            }
        }
        //        if (x!=0)
        //        {
        x = 0;//����
        y += m_lCurLineHeight;

        //���к�ָ�Ĭ���и�
        m_lCurLineHeight = m_LineHeight;
        //        }
    }

    if (m_nHAlign == CGuiConst::ALIGN_CENTER)
    {
    }

    m_nMaxLines = y / m_LineHeight;

    if (m_nMaxLines < (int)htmlitem.m_LinesLimits.size()) 
    {
        m_nMaxLines = (int)htmlitem.m_LinesLimits.size();
    }
}

// ContentTop ContentBottom��ʾ��ȫ�����ݸ߶��е�λ��,��Ե�
// ��HtmlItem����ʼλ��������Content�е���ʼλ�ã���Ե�
// x y��ʾ��ʼ����λ�õľ�������
int MultiTextBox::RealDrawHTML(DrawHtmlItem& htmlitem, int ContentTop, 
    int ContentBottom, int ContentBegin, int x, int y)
{
    // �˺��������ͱ����϶�����ʾ�����н��ĵط�
    bool bIsDraw = false;        // �Ƿ������
    bool bTopHide = false;        // �Ƿ���Ҫ����
    bool bBottomHide = false;
    int TopHideHeight = 0;        // item���ر��ڵ��ĸ߶�
    int BottomHideHeight = 0;

    // ��ȡ�ϱ߱������ĸ߶�
    if (ContentBegin < ContentTop)
    {
        TopHideHeight = ContentTop - ContentBegin;
    }

    // ��ȡ�±߱������ĸ߶�
    if (ContentBegin + htmlitem.m_ContentHeight > ContentBottom)
    {
        BottomHideHeight = ContentBegin + htmlitem.m_ContentHeight 
            - ContentBottom;
    }

    size_t text_num = htmlitem.m_TextList.size();

    for (size_t k = 0; k < text_num; ++k)
    {
        DRAWTEXTLINE* pText = &htmlitem.m_TextList[k];
        
        int rect_top = pText->m_TextLine[0].rect.top;
        
        if (ContentBegin + rect_top >= ContentTop && 
            ContentBegin + rect_top + pText->nMaxLineHeight <= ContentBottom)
        {
            size_t line_num = pText->m_TextLine.size();

            for (size_t j = 0; j < line_num; j++)
            {
                m_lCurLineHeight = pText->nMaxLineHeight;

                DRAWTEXTITEM* pLine = &pText->m_TextLine[j];

                RealDrawText(pLine->szFont.c_str(), pLine->dColor,
                    x + pLine->rect.left, y + pLine->rect.top - TopHideHeight,
                    pLine->szText.c_str());
            }
        }
    }

    bool bTopClip = false; // �Ͻ�
    bool bBottomClip = false; // �½�

    int lImageHeight = 0;
    int yPos = 0;

    size_t image_num = htmlitem.m_ImageList.size();

    for (size_t i = 0; i < image_num; i++)
    {
        bTopClip = false; // �Ͻ�
        bBottomClip = false; // �½�

        DRAWIMAGEITEM* pImage = &htmlitem.m_ImageList[i];
        HtmlImageItem& image_item = pImage->imageItem;
        
        lImageHeight = image_item.m_Rect.bottom - 
            image_item.m_Rect.top;

        rect_t drawRect = image_item.m_Rect;
    
        int image_top = ContentBegin + pImage->y;    
 
        if (image_top + lImageHeight <= ContentTop 
            || image_top >= ContentBottom)
        {
            //���ϻ��¶�������
            continue;
        }
        else if (image_top <= ContentTop 
            &&    image_top + lImageHeight >= ContentBottom)
        {
            //����
            drawRect.top += ContentTop - image_top;
            drawRect.bottom -= image_top + lImageHeight - ContentBottom;
            yPos = 0;
        }
        else if (image_top < ContentTop 
            && image_top + lImageHeight > ContentTop)
        {
            //�Ͻ�
            if (TopHideHeight > 0)
            {
                drawRect.top += TopHideHeight - pImage->y;
            }
            else
            {
                drawRect.top += ContentTop - image_top;
            }

            yPos = 0;
            bTopClip = true;
        }
        else if (image_top >= ContentTop 
            && image_top + lImageHeight <= ContentBottom)
        {
            //����
            if (TopHideHeight > 0)
            {
                yPos = pImage->y - TopHideHeight;
            }
            else
            {
                yPos = pImage->y;
            }
        }
        else if (image_top < ContentBottom 
            && image_top + lImageHeight >= ContentBottom)
        {
            //�½�
            drawRect.bottom -= image_top + lImageHeight - ContentBottom;
            yPos = pImage->y - TopHideHeight; 
            bBottomClip = true;
        }

        int x1 = x + pImage->x;
        int y1 = y + yPos;

        if (image_item.m_bIsAnimation)
        {
            ImageAnimationManager* pAniManager = InnerGetAnimation();

            if (NULL == pAniManager)
            {
                return htmlitem.m_ContentHeight - TopHideHeight 
                    - BottomHideHeight;
            }

            size_t length = image_item.m_imagesrc.size() * 2;

            if (length == 0)
            {
                continue;
            }

            TAutoMem<char, 256> auto_buf(length + 1);
            
            ToString(image_item.m_imagesrc.c_str(), 
                auto_buf.GetBuffer(), length);

            point_t pt_size = pAniManager->GetAnimationSize(
                auto_buf.GetBuffer());

            // �������ʾ�Ĵ�СС��ԭʼ��С������ʾ
            if (drawRect.bottom - drawRect.top >= pt_size.y && 
                drawRect.right - drawRect.left >= pt_size.x)
            {
                pAniManager->DrawAnimation(auto_buf.GetBuffer(), x1, y1, 
                    drawRect);
            }

            bIsDraw = true;
        }
        else
        {
            PERSISTID id = image_item.m_imageID;

            rect_t& rect = image_item.m_Rect;

            int nMaxHeight = drawRect.bottom - drawRect.top;
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;

            h = nMaxHeight > h ? h : nMaxHeight;

            int top = 0;

            // �Ͻ����
            if (bTopClip)
            {
                top = rect.bottom - rect.top - nMaxHeight;
            }

            int bottom = rect.bottom - rect.top;

            // �½����
            if (bBottomClip)
            {
                bottom = nMaxHeight;
            }

            if (image_item.m_nType == 0)
            {
                // TGA
                GetPainter()->DrawGlyph(x1, y1, x1 + w, y1 + h, 0, top, w, 
                    bottom, id);

                bIsDraw = true;
            }
            else
            {
                // PNG
                GetPainter()->DrawGlyph(x1, y1, x1 + w, y1 + h, 0, top, w, 
                    bottom, id);

                bIsDraw = true;
            }
        }
    }

    return htmlitem.m_ContentHeight - TopHideHeight - BottomHideHeight;
}

void MultiTextBox::Paint(float seconds)
{
    IPainter* pPainter = GetPainter();

    if (NULL == pPainter)
    {
        return;
    }

    if (!IControl::GetVisible())
    {
        return;
    }

    RefreshInnerAbsPos();

    if (m_bIsMouseCaptured)
    {
        point_t pos;

        if (GetGui()->GetCursorCoord(pos))
        {
            if (!PtInRect(&m_RealRect, pos))
            {
                m_bIsMouseCaptured = false;

                if (m_lCurMouseInItem >= 0)
                {
                    if (m_lCurMouseInImage >= 0)
                    {
                        m_bCanDelete = false;

                        // �����Ƴ���Ϣ
                        GuiUtil_RunCallback(this, "on_mouseout_image", 
                            CVarList() << m_lCurMouseInItem 
                            << m_lCurMouseInImage);

                        m_bCanDelete = true;
                    }

                    if (!m_bTransHyperlink && 
                        (size_t)m_lCurMouseInItem < m_HtmlItems.size() && 
                        m_lCurMouseInHyperLink >= 0)
                    {
                        DrawHtmlItem* pItem = &m_HtmlItems[m_lCurMouseInItem];

                        if ((size_t)m_lCurMouseInHyperLink < pItem->m_vHyperLinks.size())
                        {
                            HYPERLINKITEM* pLink = 
                                &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];
                            
                            // �����Ƴ���Ϣ
                            InnerGetHyperlinkMng()->OnMouseOutHyperLink(
                                pLink->m_wcsLink);

                            m_bCanDelete = false;

                            GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                                CVarList() << m_lCurMouseInItem << 
                                pLink->m_wcsLink.c_str());

                            m_bCanDelete = true;

                            ChangeHyperLinkState(*pItem, m_lCurMouseInHyperLink, 
                                HYPERLINK_NORMAL);
                        }
                    }

                    m_lCurMouseInItem = -1;
                    m_lCurMouseInImage = -1;
                    m_lCurMouseInHyperLink = -1;
                }

                m_bCanDelete = false;

                // ��������Ƴ���Ϣ
                GuiUtil_RunCallback(this, "on_lost_capture", 
                    CVarList() << this->GetID());

                m_bCanDelete = true;
            }
            else
            {
                OnMouseMove(pos.x, pos.y);
            }
        }
    }

    int old_alpha = pPainter->GetBlendAlpha();

    // ���������͸���ˣ��ӽ���ҲӦ͸��
    if (old_alpha < GetAlpha())
    {
        pPainter->SetBlendAlpha(old_alpha);
    }
    else
    {
        pPainter->SetBlendAlpha(GetAlpha());
    }

    IScrollable::Paint(seconds);

    // �������ݸ߶��е�һ����,�����жϻ�������
    int lContentTop = GetDrawItemTop();
    int lContentBottom = lContentTop + (m_ViewRect.bottom - m_ViewRect.top);

    // �����������,��������,���ڻ���
    int x = InnerGetAbsLeft() + m_ViewRect.left;
    int y = GetDrawBeginTop();
    int endy = y + (m_ViewRect.bottom - m_ViewRect.top);

    // ��ǰ�����ݵ��������ݸ߶��е���ʼλ��
    int ContentBegin = 0;
    int itemheight = 0;
    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        DrawHtmlItem* pItem = &m_HtmlItems[i];

        if (!IsShowKey(pItem->m_nKey))
        {
            continue;
        }

        itemheight = 0;

        if (ContentBegin < lContentBottom 
            && ContentBegin + pItem->m_ContentHeight > lContentTop)
        {
            int top = ContentBegin - lContentTop;
            int bottom = top + pItem->m_ContentHeight;

            pItem->m_ContentTop = top;
            pItem->m_ContentBottom = bottom;

            if (top < 0) 
            {
                top = 0;
            }

            if (bottom > (m_ViewRect.bottom - m_ViewRect.top))
            {
                bottom = m_ViewRect.bottom - m_ViewRect.top;
            }

            itemheight = bottom - top;
        }

        if (itemheight > 0)
        {
            if (m_lCurSelectItem == i 
                && ((m_SelectBarColor & 0xff000000) != 0) 
                && IsItemSelectable(i))
            {
                if (m_SelectBarDraw.IsReady())
                {
                    m_SelectBarDraw.Draw(m_nSelectBarDrawMode,
                        x, y, x + m_TextWidth, y + itemheight);
                }
                else
                {
                    // ��ѡ�п�
                    pPainter->FillRect(x, y, x + m_TextWidth, 
                        y + itemheight, m_SelectBarColor);
                }
            }

            if (m_lCurMouseInItem == i 
                && ((m_MouseInBarColor & 0xff000000) != 0) 
                && IsItemSelectable(i))
            {
                if (m_MouseInBarDraw.IsReady())
                {
                    m_MouseInBarDraw.Draw(m_nMouseInBarDrawMode,
                        x, y, x + m_TextWidth, y + itemheight);
                }
                else
                {
                    // �������
                    pPainter->FillRect(x, y, x + m_TextWidth, 
                        y + itemheight, m_MouseInBarColor);
                }
            }

            int newitemheight = RealDrawHTML(*pItem, lContentTop, 
                lContentBottom, ContentBegin, x, y);

            if (itemheight != newitemheight)
            {
                int i = 0;
            }
        }

        ContentBegin += pItem->m_ContentHeight;
        y += itemheight;
        
        if (y > endy)
        {
            break;
        }
    }

    pPainter->SetBlendAlpha(old_alpha);
}

void MultiTextBox::SetSelectBarDrawModeString(const char* value)
{
    m_nSelectBarDrawMode = GetDrawModeByString(value,CGuiConst::DRAWMODE_FITWINDOW);
}

result_string MultiTextBox::GetSelectBarDrawModeString() const
{
    return GetStringByDrawMode(m_nSelectBarDrawMode);
}

void MultiTextBox::SetMouseInBarDrawModeString(const char* value)
{
    m_nMouseInBarDrawMode = GetDrawModeByString(value,CGuiConst::DRAWMODE_FITWINDOW);
}

result_string MultiTextBox::GetMouseInBarDrawModeString() const
{
    return GetStringByDrawMode(m_nMouseInBarDrawMode);
}

bool MultiTextBox::OnWitchImageTest(int olditemindex, int itemindex, 
    int top, point_t ptMouse)
{
    if (itemindex < 0)
    {
        return false;
    }

    DrawHtmlItem& htmlitem = m_HtmlItems[itemindex];

    // ��ǰ�����ݵ��������ݸ߶��е���ʼλ��
    int ContentBegin = 0;

    int lOldMouseInImage = m_lCurMouseInImage;

    int lImageHeight;
    int lImageWidth;
    bool bFind = false;

    int lNewMouseInImage = -1;

    std::vector<DRAWIMAGEITEM>& m_ImageList = htmlitem.m_ImageList;

    for (int i = 0; i < (int)m_ImageList.size(); i++)
    {
        HtmlImageItem& image_item = m_ImageList[i].imageItem;

        lImageHeight = image_item.m_Rect.bottom - 
            image_item.m_Rect.top;
        lImageWidth = image_item.m_Rect.right - 
            image_item.m_Rect.left;

        rect_t CurRect;

        if (top < 0 && top + lImageHeight >= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //����
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = 0;
            CurRect.bottom = CurRect.top+ (m_ViewRect.bottom - m_ViewRect.top);
        }
        else if (top < 0)
        {
            //�Ͻ�
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = 0;
            CurRect.bottom = lImageHeight + top;
        }
        else if (top >= 0 && 
            top + lImageHeight <= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //����
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = top;
            CurRect.bottom = CurRect.top + lImageHeight;

        }
        else if (top < (m_ViewRect.bottom - m_ViewRect.top) && 
            top + lImageHeight >= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //�½�
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = top;
            CurRect.bottom = m_ViewRect.bottom - m_ViewRect.top;
        }

        if (PtInRect(&CurRect, ptMouse))
        {
            bFind = true;
            lNewMouseInImage = i;
            break;
        }
    }

    if (!bFind)
    {
        // δ�򵽵�����²���Ҫ������Ϣ��������Ϣ����OnMouseMove�д�����
        if (itemindex == olditemindex)
        {
            if (lOldMouseInImage > -1)
            {
                m_bCanDelete = false;

                // �Ƴ�
                GuiUtil_RunCallback(this, "on_mouseout_image", 
                    CVarList() << itemindex << lOldMouseInImage);

                m_bCanDelete = true;
            }
        }
        else
        {
            if (lOldMouseInImage > -1)
            {
                m_bCanDelete = false;

                // �Ƴ�
                GuiUtil_RunCallback(this, "on_mouseout_image", 
                    CVarList() << olditemindex << lOldMouseInImage);

                m_bCanDelete = true;
            }
        }
    }

    return bFind;
}


bool MultiTextBox::OnWitchHyperLinkTest(int olditemindex, int itemindex, 
    int top, point_t ptMouse)
{
    if (itemindex < 0)
    {
        return false;
    }

    DrawHtmlItem& htmlitem = m_HtmlItems[itemindex];

    bool bFind = false;
    int lOldMouseInItem = olditemindex;
    int lOldMouseInHyperLink = m_lCurMouseInHyperLink;
    int lNewMouseInHyperLink = -1;

    std::vector<HYPERLINKITEM>& vHyperLinks = htmlitem.m_vHyperLinks;
    std::vector<DRAWTEXTLINE>& vTextList = htmlitem.m_TextList;

    for (size_t i = 0; i < vHyperLinks.size(); ++i)
    {
        for (size_t j = 0; j < vHyperLinks[i].m_OneLink.size(); ++j)
        {
            point_t& pt = vHyperLinks[i].m_OneLink[j];
            rect_t rect = vTextList[pt.x].m_TextLine[pt.y].rect;
            const int lHeight = rect.bottom - rect.top;

            if (top + rect.bottom < 0 
                || top + rect.top > (m_ViewRect.bottom - m_ViewRect.top))
            {
                //���ϻ��¶�������
                continue;
            }
            else
            {
                rect.top = rect.top + top;
                rect.bottom = rect.top + lHeight;

                if (PtInRect(&rect, ptMouse))
                {
                    bFind = true;
                    lNewMouseInHyperLink = i;
                    break;
                }
            }
        }

        if (bFind)
        {
            break;
        }
    }

    if (!bFind)
    {
        m_lCurMouseInHyperLink = -1;
    }

    if (m_bTransHyperlink || GetTransparent())
    {
        // ͸����ʱ�򲻴�����Ϣ
        return bFind;
    }

    if (!bFind)
    {
        // ͬһITEM
        if (olditemindex == itemindex)
        {
            if (lOldMouseInHyperLink > -1 && itemindex > -1)
            {
                // �Ƴ�
                DrawHtmlItem* pItem = &m_HtmlItems[itemindex];
                HYPERLINKITEM* pLink = 
                    &pItem->m_vHyperLinks[lOldMouseInHyperLink];

                InnerGetHyperlinkMng()->OnMouseOutHyperLink(pLink->m_wcsLink);

                m_bCanDelete = false;

                GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                    CVarList() << itemindex << pLink->m_wcsLink.c_str());

                m_bCanDelete = true;

                ChangeHyperLinkState(*pItem, lOldMouseInHyperLink, 
                    HYPERLINK_NORMAL);
            }
        }
        else    // ��ͬITEM
        {
            if (lOldMouseInHyperLink > -1 && olditemindex > -1)
            {
                // �Ƴ�
                DrawHtmlItem* pItem = &m_HtmlItems[olditemindex];
                HYPERLINKITEM* pLink = 
                    &pItem->m_vHyperLinks[lOldMouseInHyperLink];

                //InnerGetHyperlinkMng()->OnMouseOutHyperLink(pLink->m_wcsLink);

                m_bCanDelete = false;

                GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                    CVarList() << itemindex << pLink->m_wcsLink.c_str());

                m_bCanDelete = true;

                ChangeHyperLinkState(*pItem, lOldMouseInHyperLink, 
                    HYPERLINK_NORMAL);
            }
        }
    }
    else
    {
    }

    return bFind;
}

bool MultiTextBox::OnWitchImage(int olditemindex, int itemindex, int top, 
    point_t ptMouse)
{
    if (itemindex < 0)
    {
        return false;
    }

    DrawHtmlItem& htmlitem = m_HtmlItems[itemindex];

    // ��ǰ�����ݵ��������ݸ߶��е���ʼλ��
    int ContentBegin = 0;

    int lOldMouseInImage = m_lCurMouseInImage;

    int lImageHeight;
    int lImageWidth;
    bool bFind = false;

    std::vector<DRAWIMAGEITEM>& m_ImageList = htmlitem.m_ImageList;

    for (int i = 0; i < (int)m_ImageList.size(); i++)
    {
        HtmlImageItem& image_item = m_ImageList[i].imageItem;

        lImageHeight = image_item.m_Rect.bottom - 
            image_item.m_Rect.top;
        lImageWidth = image_item.m_Rect.right - 
            image_item.m_Rect.left;

        rect_t CurRect;

        if (top < 0 
            && top + lImageHeight >= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //����
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = 0;
            CurRect.bottom = CurRect.top + (m_ViewRect.bottom - m_ViewRect.top);
        }
        else if (top < 0)
        {
            //�Ͻ�
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = 0;
            CurRect.bottom = lImageHeight + top;
        }
        else if (top >= 0 && 
            top + lImageHeight <= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //����
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = top;
            CurRect.bottom = CurRect.top + lImageHeight;

        }
        else if (top < (m_ViewRect.bottom - m_ViewRect.top) && 
            top + lImageHeight >= (m_ViewRect.bottom - m_ViewRect.top))
        {
            //�½�
            CurRect.left = m_ImageList[i].x;
            CurRect.right = CurRect.left + lImageWidth;
            CurRect.top = top;
            CurRect.bottom = (m_ViewRect.bottom - m_ViewRect.top);
        }

        if (PtInRect(&CurRect, ptMouse))
        {
            bFind = true;
            m_lCurMouseInImage = i;
            break;
        }
    }

    if (!bFind)
    {
        m_lCurMouseInImage = -1;
    }

    if (m_bTransImage || GetTransparent())
    {
        // ͸����ʱ�򲻴�����Ϣ
        return bFind;
    }

    if (itemindex == olditemindex)
    {
        if (lOldMouseInImage > -1 && lOldMouseInImage != m_lCurMouseInImage)
        {
            m_bCanDelete = false;

            // �Ƴ�
            GuiUtil_RunCallback(this, "on_mouseout_image", 
                CVarList() << itemindex << lOldMouseInImage);

            m_bCanDelete = true;
        }

        if (m_lCurMouseInImage > -1 && lOldMouseInImage != m_lCurMouseInImage)
        {
            m_bCanDelete = false;
            // ����
            GuiUtil_RunCallback(this, "on_mousein_image", 
                CVarList() << itemindex << m_lCurMouseInImage << ptMouse.x 
                << ptMouse.y);

            m_bCanDelete = true;
        }
    }
    else
    {
        if (lOldMouseInImage > -1)
        {
            m_bCanDelete = false;

            // �Ƴ�
            GuiUtil_RunCallback(this, "on_mouseout_image", 
                CVarList() << olditemindex << lOldMouseInImage);

            m_bCanDelete = true;
        }

        if (m_lCurMouseInImage > -1)
        {
            m_bCanDelete = false;

            // ����
            GuiUtil_RunCallback(this, "on_mousein_image", 
                CVarList() << itemindex << m_lCurMouseInImage << ptMouse.x 
                << ptMouse.y);

            m_bCanDelete = true;
        }
    }

    return bFind;
}

bool MultiTextBox::OnWitchHyperLink(int olditemindex, int itemindex, 
    int top, point_t ptMouse)
{
    if (itemindex < 0)
    {
        return false;
    }

    DrawHtmlItem& htmlitem = m_HtmlItems[itemindex];

    bool bFind = false;
    int lOldMouseInItem = olditemindex;
    int lOldMouseInHyperLink = m_lCurMouseInHyperLink;

    std::vector<HYPERLINKITEM >& vHyperLinks = htmlitem.m_vHyperLinks;
    std::vector<DRAWTEXTLINE >& vTextList = htmlitem.m_TextList;

    for (size_t i = 0; i < vHyperLinks.size(); ++i)
    {
        for (size_t j = 0; j < vHyperLinks[i].m_OneLink.size(); ++j)
        {
            point_t& pt = vHyperLinks[i].m_OneLink[j];
            rect_t rect = vTextList[pt.x].m_TextLine[pt.y].rect;
            const int lHeight = rect.bottom - rect.top;

            if (top + rect.bottom < 0 
                || top + rect.top > (m_ViewRect.bottom - m_ViewRect.top))
            {
                //���ϻ��¶�������
                continue;
            }
            else
            {
                rect.top = rect.top + top;
                rect.bottom = rect.top + lHeight;

                if (PtInRect(&rect, ptMouse))
                {
                    bFind = true;
                    m_lCurMouseInHyperLink = i;
                    break;
                }
            }
        }

        if (bFind)
        {
            break;
        }
    }

    if (!bFind)
    {
        m_lCurMouseInHyperLink = -1;
    }

    if (m_bTransHyperlink || GetTransparent())
    {
        // ͸����ʱ�򲻴�����Ϣ
        return bFind;
    }

    // ͬһITEM
    if (olditemindex == itemindex)
    {
        if (lOldMouseInHyperLink > -1 
            && m_lCurMouseInHyperLink != lOldMouseInHyperLink)
        {
            // �Ƴ�
            DrawHtmlItem* pItem = &m_HtmlItems[itemindex];

            if (lOldMouseInHyperLink < (int)pItem->m_vHyperLinks.size())
            {
                HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[lOldMouseInHyperLink];

                InnerGetHyperlinkMng()->OnMouseOutHyperLink(pLink->m_wcsLink);

                m_bCanDelete = false;

                GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                    CVarList() << itemindex << pLink->m_wcsLink.c_str());

                m_bCanDelete = true;

                if (itemindex < (int)m_HtmlItems.size())
                {
                    ChangeHyperLinkState(*pItem, lOldMouseInHyperLink, 
                        HYPERLINK_NORMAL);
                }
            }
        }
        
        if (m_lCurMouseInHyperLink > -1 
            && m_lCurMouseInHyperLink != lOldMouseInHyperLink)
        {
            // ����
            DrawHtmlItem* pItem = &m_HtmlItems[itemindex];

            if ((size_t)m_lCurMouseInHyperLink < pItem->m_vHyperLinks.size())
            {
                HYPERLINKITEM* pLink = 
                    &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

                InnerGetHyperlinkMng()->OnMouseInHyperLink(pLink->m_wcsLink);

                m_bCanDelete = false;

                GuiUtil_RunCallback(this, "on_mousein_hyperlink", 
                    CVarList() << itemindex << pLink->m_wcsLink.c_str() 
                    << ptMouse.x << ptMouse.y);

                m_bCanDelete = true;

                if (itemindex < (int)m_HtmlItems.size())
                {
                    ChangeHyperLinkState(*pItem, m_lCurMouseInHyperLink, 
                        HYPERLINK_MOUSEIN);
                }
            }
            else
            {
                m_lCurMouseInHyperLink = -1;
            }
        }
    }
    else    // ��ͬITEM
    {
        if (lOldMouseInHyperLink > -1 && olditemindex > -1 &&
            olditemindex < (int)m_HtmlItems.size() && 
            lOldMouseInHyperLink < (int)m_HtmlItems[olditemindex].m_vHyperLinks.size())
        {
            // �Ƴ�
            DrawHtmlItem* pItem = &m_HtmlItems[olditemindex];
            HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[lOldMouseInHyperLink];

            InnerGetHyperlinkMng()->OnMouseOutHyperLink(pLink->m_wcsLink);

            m_bCanDelete = false;

            GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                CVarList() << itemindex << pLink->m_wcsLink.c_str());

            m_bCanDelete = true;

            if (olditemindex < (int)m_HtmlItems.size())
            {
                ChangeHyperLinkState(*pItem, lOldMouseInHyperLink, 
                    HYPERLINK_NORMAL);
            }
        }

        if (m_lCurMouseInHyperLink > -1 && itemindex > -1)
        {
            // ����
            DrawHtmlItem* pItem = &m_HtmlItems[itemindex];
            HYPERLINKITEM* pLink = 
                &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

            InnerGetHyperlinkMng()->OnMouseInHyperLink(pLink->m_wcsLink);

            m_bCanDelete = false;

            GuiUtil_RunCallback(this, "on_mousein_hyperlink", 
                CVarList() << itemindex << pLink->m_wcsLink.c_str() 
                << ptMouse.x << ptMouse.y);

            m_bCanDelete = true;

            if (itemindex < (int)m_HtmlItems.size() && 
                m_lCurMouseInHyperLink < (int)pItem->m_vHyperLinks.size())
            {
                ChangeHyperLinkState(*pItem, m_lCurMouseInHyperLink, 
                    HYPERLINK_MOUSEIN);
            }
        }
    }

    return bFind;
}

void MultiTextBox::OnMouseInItem(int index)
{
}

void MultiTextBox::OnMouseOutItem(int index)
{
}

void MultiTextBox::ChangeHyperLinkState(DrawHtmlItem& htmlitem, int link, 
    int state)
{
    if (link >= 0 && link < (int)htmlitem.m_vHyperLinks.size())
    {
        HyperLinkStyleManager* pHLinkManager = InnerGetHyperlinkMng();

        if (NULL == pHLinkManager)
        {
            return;
        }
        
        StyleElement* pElement = NULL;

        if (!pHLinkManager->GetStyleByState(
            htmlitem.m_vHyperLinks[link].m_szHPStyle.c_str(), 
            (HyperLinkState)state, pElement))
        {
            return;
        }

        for (size_t j = 0; j < htmlitem.m_vHyperLinks[link].m_OneLink.size(); ++j)
        {
            point_t& pt = htmlitem.m_vHyperLinks[link].m_OneLink[j];

            DRAWTEXTITEM& SubLinkDrawItem = 
                htmlitem.m_TextList[pt.x].m_TextLine[pt.y];

            SubLinkDrawItem.szFont = pElement->szFont;
            SubLinkDrawItem.dColor = pElement->color;
        }
    }
}

// ������ר��
int MultiTextBox::GetVerticalTotal() const
{
    int TotalHeight = 0;
    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        const DrawHtmlItem* pItem = &m_HtmlItems[i];

        if (pItem->m_nKey == -1 || this->IsShowKey(pItem->m_nKey))
        {
            TotalHeight += pItem->m_ContentHeight;
        }
    }

    if (TotalHeight % m_LineHeight == 0)
    {
        return TotalHeight/m_LineHeight;
    }

    return TotalHeight / m_LineHeight + 1;
}

int MultiTextBox::GetVerticalCanDisp() const
{
    return (m_ViewRect.bottom - m_ViewRect.top) / m_LineHeight;
}

void MultiTextBox::SetVerticalValue(int value)
{
    if (value >= 0 && value <= GetVerticalTotal())
    {
        m_nTopIndex = value;
    }
}

int MultiTextBox::GetVerticalValue() const
{
    return m_nTopIndex;
}

int MultiTextBox::GetVerticalMaxValue()
{
    return GetVerticalTotal() - GetVerticalCanDisp();
}

int MultiTextBox::GetItemTop(int index)
{
    if (index >= 0 && index < (int)m_HtmlItems.size())
    {
        return m_HtmlItems[index].m_ContentTop;
    }

    return ERROR_HEIGHT_CONTENT;
}

int MultiTextBox::GetItemHeight(int index)
{
    if (index >= 0 && index < (int)m_HtmlItems.size())
    {
        return m_HtmlItems[index].m_ContentHeight;
    }

    return 0;
}

int MultiTextBox::GetContentHeight()
{
    int lMax = 0;
    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        lMax += m_HtmlItems[i].m_ContentHeight;
    }

    return lMax;
}

int MultiTextBox::GetContentWidth()
{
    int lMax = 0;
    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        DrawHtmlItem* pItem = &m_HtmlItems[i];

        if (lMax < pItem->m_ContentWidth)
        {
            lMax = pItem->m_ContentWidth;
        }
    }

    return lMax;
}

bool MultiTextBox::IsShowKey(const int nKey) const
{
    if (nKey == -1 || m_vShowKeys.size() == 0)
    {
        return true;
    }

    for (size_t i = 0; i < m_vShowKeys.size(); ++i)
    {
        if (m_vShowKeys[i] == nKey)
        {
            return true;
        }
    }

    return false;
}

void MultiTextBox::UpdateScroll()
{
    if (m_bAutoScroll)
    {
        GotoEnd();
    }
    else
    {
        if (GetVScrollBar() != PERSISTID())
        {
            IScrollable::UpdateScroll();
        }
    }
}

bool MultiTextBox::IsItemSelectable(size_t index)
{
    if (index < m_HtmlItems.size())
    {
        return m_HtmlItems[index].m_bSelectable;
    }

    return false;
}

IControl* MultiTextBox::OnGetInWhat(int x, int y)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return NULL;
    }

    IControl* pScrollBar = IScrollable::OnGetScrollBar(x, y);

    if (pScrollBar)
    {
        // ѡ�е��ǹ���������ֱ�ӷ���
        return pScrollBar;
    }

    // ����͸��
    if (m_bTransNormalText)
    {
        IControl* pResult = IScrollable::OnGetInWhat(x, y);

        // ���ӿؼ���,�򷵻��ӿؼ�
        if (pResult != NULL && pResult != this)
        {
            return pResult;
        }

        // OnGetInWhat �������ֻ����������������Ż������,��˶�������
        if (!m_bIsMouseCaptured)
        {
            m_bIsMouseCaptured = true;

            m_bCanDelete = false;

            // �������������Ϣ
            GuiUtil_RunCallback(this, "on_get_capture", 
                CVarList() << this->GetID());

            m_bCanDelete = true;

            m_RealRect.left = InnerGetAbsLeft();
            m_RealRect.top = InnerGetAbsTop();
            m_RealRect.right = m_RealRect.left + GetWidth();
            m_RealRect.bottom = m_RealRect.top + GetHeight();
        }

        int lContentBegin = 0;
        point_t pt = { 0, 0 };

        int lOldMouseInItem = m_lCurMouseInItem;

        int newMouseInItem = GetMouseInItem(x, y, pt, lContentBegin);

        // m_lCurMouseInItem = newMouseInItem;

        if (newMouseInItem > -1)
        {
            // �������ݸ߶��е�һ����,�����жϻ�������
            int lContentTop = GetDrawItemTop();
            int lContentBottom = lContentTop 
                + (m_ViewRect.bottom - m_ViewRect.top);

            if (!m_bTransImage)
            {
                // ͼƬ��͸���Ļ��鿴���������ͼƬ������Ӧ��Ϣ
                bool bFound = OnWitchImageTest(lOldMouseInItem, 
                    newMouseInItem, lContentBegin - lContentTop, pt);

                if (bFound )
                {
                    return pResult;
                }
            }

            if (!m_bTransHyperlink)
            {
                // �����Ӳ�͸���Ļ��鿴������ڳ�����������Ӧ��Ϣ
                bool bFound = OnWitchHyperLinkTest(lOldMouseInItem, 
                    newMouseInItem, lContentBegin - lContentTop, pt);

                if (bFound )
                {
                    return pResult;
                }
            }

            // ����͸���ˣ����Ҳ���ͼƬ�ͳ������ϣ��򷵻ؿ�
            return NULL;
            
        }
        else
        {
            // ��������ı��ϣ���ֱ�ӷ��ؿ�
            return NULL;
        }
    }
    else
    {
        IControl* pResult = IScrollable::OnGetInWhat(x, y);

        if (pResult != NULL)
        {
            // OnGetInWhat �������ֻ����������������Ż������,��˶�������
            if (!m_bIsMouseCaptured)
            {
                m_bIsMouseCaptured = true;

                m_bCanDelete = false;

                // �������������Ϣ
                GuiUtil_RunCallback(this, "on_get_capture", 
                    CVarList() << this->GetID());

                m_bCanDelete = true;

                m_RealRect.left = InnerGetAbsLeft();
                m_RealRect.top = InnerGetAbsTop();
                m_RealRect.right = m_RealRect.left + GetWidth();
                m_RealRect.bottom = m_RealRect.top + GetHeight();
            }

            int lContentBegin = 0;
            point_t pt = {0,0};

            int lOldMouseInItem = m_lCurMouseInItem;

            //int newMouseInItem = GetMouseInItem(x, y, pt, lContentBegin);

            return pResult;
        }
    }

    return NULL;
}

int MultiTextBox::OnLButtonUp(int x, int y)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return 0;
    }

	int click_x = x - InnerGetAbsLeft();
	int click_y = y - InnerGetAbsTop();

	//Assert((click_x >= 0) && (click_x < GetWidth()));
	//Assert((click_y >= 0) && (click_y < GetHeight()));

	GuiUtil_RunCallback(this, "on_leftclick_up", 
		CVarList() << click_x << click_y);

    m_lCurSelectItem = -1;

    if (m_lCurMouseInItem != -1)
    {
        if (m_lCurSelectItem != m_lCurMouseInItem)
        {
            m_lCurSelectItem = m_lCurMouseInItem;

            m_bCanDelete = false;

            GuiUtil_RunCallback(this, "on_select_item_change", 
                CVarList() << m_lCurSelectItem);

            m_bCanDelete = true;
        }
    }

    m_lCurSelectImage = -1;

    if (m_lCurSelectItem != -1 && m_lCurMouseInImage != -1)
    {
        m_lCurSelectImage = m_lCurMouseInImage;

        m_bCanDelete = false;

        GuiUtil_RunCallback(this, "on_click_image", 
            CVarList() << m_lCurSelectItem <<  m_lCurSelectImage);

        m_bCanDelete = true;
    }

    m_lCurSelectHyperLink = -1;

    if (! m_bTransHyperlink && 
        m_lCurSelectItem != -1 && m_lCurMouseInHyperLink >= 0 &&
        m_lCurSelectItem < (int)m_HtmlItems.size() && 
        m_lCurMouseInHyperLink < (int)m_HtmlItems[m_lCurSelectItem].m_vHyperLinks.size())
    {
        m_lCurSelectHyperLink = m_lCurMouseInHyperLink;

        DrawHtmlItem* pItem = &m_HtmlItems[m_lCurSelectItem];
        HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

        ChangeHyperLinkState(*pItem, m_lCurMouseInHyperLink, 
            HYPERLINK_MOUSEIN);

        InnerGetHyperlinkMng()->OnClickHyperLink(pLink->m_wcsLink);

        m_bCanDelete = false;

        GuiUtil_RunCallback(this, "on_click_hyperlink", 
            CVarList() << m_lCurSelectItem << pLink->m_wcsLink.c_str());

        m_bCanDelete = true;
    }

    return 1;
}

int MultiTextBox::OnLButtonDown(int x, int y)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return 0;
    }

	int click_x = x - InnerGetAbsLeft();
	int click_y = y - InnerGetAbsTop();

	Assert((click_x >= 0) && (click_x < GetWidth()));
	Assert((click_y >= 0) && (click_y < GetHeight()));

	GuiUtil_RunCallback(this, "on_leftclick_down", 
		CVarList() << click_x << click_y);

    if (! m_bTransHyperlink && 
        m_lCurMouseInItem != -1 && m_lCurMouseInHyperLink >= 0 
        && m_lCurMouseInHyperLink < (int)m_HtmlItems[m_lCurMouseInItem].m_vHyperLinks.size())
    {
        ChangeHyperLinkState(m_HtmlItems[m_lCurMouseInItem], 
            m_lCurMouseInHyperLink, HYPERLINK_PRESS);

        return 1;
    }

    return 0;
}

int MultiTextBox::OnLButtonDoubleClick(int x, int y)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return 0;
    }

    if (m_lCurSelectItem != -1 && m_lCurSelectImage != -1)
    {
        m_bCanDelete = false;

        GuiUtil_RunCallback(this, "on_doubleclick_image", 
            CVarList() << m_lCurSelectItem<< m_lCurSelectImage);

        m_bCanDelete = true;
    }

    if (! m_bTransHyperlink && 
        m_lCurSelectItem != -1 &&  m_lCurMouseInHyperLink >= 0 
        && m_lCurMouseInHyperLink < (int)m_HtmlItems[m_lCurSelectItem].m_vHyperLinks.size())
    {
        DrawHtmlItem* pItem = &m_HtmlItems[m_lCurSelectItem];
        HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

        InnerGetHyperlinkMng()->OnDoubleClickHyperLink(pLink->m_wcsLink);

        m_bCanDelete = false;

        GuiUtil_RunCallback(this, "on_doubleclick_hyperlink", 
            CVarList() << m_lCurSelectItem << pLink->m_wcsLink.c_str());

        m_bCanDelete = true;
    }

    return 0;
}

//
int MultiTextBox::OnRButtonUp(int x, int y)
{
   // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return 0;
    }

    if (m_lCurMouseInItem > -1 && m_lCurMouseInItem < (int)m_HtmlItems.size())
    {
        m_bCanDelete = false;

        GuiUtil_RunCallback(this, "on_right_click_item", 
            CVarList() << m_lCurMouseInItem);

        m_bCanDelete = true;

        if (m_lCurMouseInImage > -1)
        {
            m_bCanDelete = false;

            GuiUtil_RunCallback(this, "on_right_click_image", 
                    CVarList() << m_lCurMouseInItem <<  m_lCurMouseInImage);

            m_bCanDelete = true;
        }

        if (m_lCurMouseInHyperLink > -1 && 
            m_lCurMouseInHyperLink < (int)m_HtmlItems[m_lCurMouseInItem].m_vHyperLinks.size())
        {
            DrawHtmlItem* pItem = &m_HtmlItems[m_lCurMouseInItem];
            HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

            InnerGetHyperlinkMng()->OnRightClickHyperLink(pLink->m_wcsLink);

            m_bCanDelete = false;

            GuiUtil_RunCallback(this, "on_right_click_hyperlink", 
                CVarList() << m_lCurSelectItem << pLink->m_wcsLink.c_str());

            m_bCanDelete = true;
        }
    }

    return 1;
}

int MultiTextBox::GetMouseInItem(int x, int y, point_t& pt, int& lContentBegin)
{
    int nMouseInItem = -1;

    // �������ݸ߶��е�һ����,�����жϻ�������
    int lContentTop = GetDrawItemTop();
    int lContentBottom = lContentTop + (m_ViewRect.bottom - m_ViewRect.top);

    // ��ǰ�����ݵ��������ݸ߶��е���ʼλ��
    lContentBegin = 0;

    int lOldMouseInItem = m_lCurMouseInItem;

    pt.x = x - InnerGetAbsLeft() - m_ViewRect.left;
    pt.y = y - GetDrawBeginTop();

    rect_t rect;

    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        DrawHtmlItem* pItem = &m_HtmlItems[i];
        
        if (!IsShowKey(pItem->m_nKey))
        {
            continue;
        }

        if (lContentBegin < lContentBottom 
            && lContentBegin + pItem->m_ContentHeight > lContentTop)
        {
            rect.top = lContentBegin - lContentTop;
            rect.bottom = rect.top + pItem->m_ContentHeight;

            if (rect.top < 0)
            {
                rect.top = 0;
            }

            if (rect.bottom > (m_ViewRect.bottom - m_ViewRect.top))
            {
                rect.bottom = m_ViewRect.bottom - m_ViewRect.top;
            }

            rect.left = 0;
            rect.right = rect.left + m_TextWidth;

            if (PtInRect(&rect, pt))
            {
                nMouseInItem = i;
                break;
            }
        }

        lContentBegin += pItem->m_ContentHeight;
    }

    return nMouseInItem;
}

int MultiTextBox::OnMouseMove(int x, int y)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return 0;
    }

    char szBuffer[256];

    SafeSprintf(szBuffer, sizeof(szBuffer), "x = %d y = %d", x, y);

    m_bCanDelete = false;

    GuiUtil_RunCallback(this, "on_mousemove", 
        CVarList() << this->GetID() << szBuffer);

    m_bCanDelete = true;

    // �������ݸ߶��е�һ����,�����жϻ�������
    int lContentTop = GetDrawItemTop();
    int lContentBottom = lContentTop + (m_ViewRect.bottom - m_ViewRect.top);
    int lContentBegin = 0;

    point_t pt = { 0, 0 };

    int lOldMouseInItem = m_lCurMouseInItem;

    int newMouseInItem = GetMouseInItem(x, y, pt, lContentBegin);

    if (newMouseInItem != m_lCurMouseInItem)
    {
        m_lCurMouseInItem = newMouseInItem;
        OnMouseInItem(newMouseInItem);
    }

    if (m_lCurMouseInItem > -1)
    {
        // �����µ�ITEM
        OnWitchImage(lOldMouseInItem, m_lCurMouseInItem, 
            lContentBegin - lContentTop, pt);
        OnWitchHyperLink(lOldMouseInItem, m_lCurMouseInItem, 
            lContentBegin - lContentTop, pt);
    }
    else
    {
        // δ�����µ�ITEM, ��ʾ�Ƴ��ؼ�
        
        if (m_lCurMouseInImage > -1)
        {
            m_bCanDelete = false;

            // �Ƴ�
            GuiUtil_RunCallback(this, "on_mouseout_image", 
                CVarList() <<  lOldMouseInItem << m_lCurMouseInImage);

            m_bCanDelete = true;

            m_lCurMouseInImage = -1;
        }

        if (! m_bTransHyperlink && 
            m_lCurMouseInHyperLink > -1)
        {
            if (lOldMouseInItem < (int)m_HtmlItems.size())
            {
                DrawHtmlItem* pItem = &m_HtmlItems[lOldMouseInItem];

                if (m_lCurMouseInHyperLink < (int)pItem->m_vHyperLinks.size())
                {
                    HYPERLINKITEM* pLink = &pItem->m_vHyperLinks[m_lCurMouseInHyperLink];

                                    InnerGetHyperlinkMng()->OnMouseOutHyperLink(pLink->m_wcsLink);

                    m_bCanDelete = false;

                    // �Ƴ�
                    GuiUtil_RunCallback(this, "on_mouseout_hyperlink", 
                        CVarList() << lOldMouseInItem << pLink->m_wcsLink.c_str());

                    m_bCanDelete = true;

                    ChangeHyperLinkState(*pItem, m_lCurMouseInHyperLink, 
                        HYPERLINK_NORMAL);
                }
            }

            m_lCurMouseInHyperLink = -1;
        }
    }

    

    return 1;
}

void MultiTextBox::OnScroll(dword wParam,dword lParam)
{
    // ͸�����������κ���Ϣ
    if (GetTransparent())
    {
        return;
    }

    ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

    if (NULL == pScrollbar)
    {
        return;
    }
    //pScrollbar->OnScroll(wParam,lParam);
    // ˢ����ʾ����λ��
    RefreshContentPos();
}

// �ߴ�ı�
int MultiTextBox::OnWidthChanged(int old)
{
    if (m_ViewRect.left == 0)
    {
        m_ViewRect.right = GetWidth();
    }

    return IScrollable::OnWidthChanged(old);
}

int MultiTextBox::OnHeightChanged(int old)
{
    if (m_ViewRect.top == 0)
    {
        m_ViewRect.bottom = GetHeight();
    }

    return IScrollable::OnHeightChanged(old);
}

rect_t MultiTextBox::GetImageRect(int itemindex, int imageindex)
{
    rect_t rect = { 0, 0, 0, 0 };
    
    if (itemindex >= 0 && itemindex < (int)m_HtmlItems.size() && 
        imageindex >= 0 && imageindex < (int)m_HtmlItems[itemindex].m_ImageList.size())
    {
        rect_t& imagerect = 
            m_HtmlItems[itemindex].m_ImageList[imageindex].imageItem.m_Rect;

        rect.left = m_HtmlItems[itemindex].m_ImageList[imageindex].x
            + InnerGetAbsLeft() + m_ViewRect.left;
        rect.right = rect.left + imagerect.right - imagerect.left;

        int lDrawTop = GetDrawItemTop();

        rect.top = m_HtmlItems[itemindex].m_ImageList[imageindex].y 
            - lDrawTop + GetDrawBeginTop();
        rect.bottom = rect.top + imagerect.bottom - imagerect.top;
    }
    
    return rect;
}

bool MultiTextBox::ChangeImage(int itemindex, int imageindex, const char* image_res)
{
    if (image_res == NULL ||
        image_res[0] == 0)
    {
        return false;
    }

    rect_t rect = { 0, 0, 0, 0 };
    
    if (itemindex >= 0 && itemindex < (int)m_HtmlItems.size() && 
        imageindex >= 0 && imageindex < (int)m_HtmlItems[itemindex].m_ImageList.size())
    {
        HtmlImageItem &image = 
            m_HtmlItems[itemindex].m_ImageList[imageindex].imageItem;

        if (image.m_bNeedRelease)
        {
            GetPainter()->ReleaseGlyph(image.m_imageID);
            image.m_imageID = PERSISTID();
        }

        int len = strlen(image_res);

        TAutoMem<wchar_t, 256> auto_buf_file(len * 2);

        wchar_t* pBuffer = auto_buf_file.GetBuffer();
        
        image.m_imagesrc = ToWideStr(image_res, pBuffer, len * 2);

        // ֻ�������һ��ʵ��
        ImageAnimationManager* pAniManager = InnerGetAnimation();

        if (NULL != pAniManager)
        {
            if (pAniManager->IsAnimation(image_res))
            {
                image.m_bNeedRelease = false;
                image.m_nType = 2;
                image.m_bIsAnimation = true;
                return true;
            }
        }

		image.m_imageID = CreateGlyph(image_res, image.m_nType, 
			image.m_Rect.right, image.m_Rect.bottom);

		image.m_Rect.left = 0;
		image.m_Rect.top = 0;

		image.m_bNeedRelease  = true;
		image.m_nType = 0;
		image.m_bIsAnimation = false;
        return true;
    }

    return false;
}

int MultiTextBox::GetDrawItemTop()
{
    int lDrawTop = m_nTopIndex * m_LineHeight;

    ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

    if (pScrollbar && pScrollbar->GetVisible()) 
    {
        lDrawTop = pScrollbar->GetCurValue() * m_LineHeight;
    }

    return lDrawTop;
}

// ��ȡ��ǰ��ʾ���ݵ���ʼy����,�������Ϸ��Ļ
int MultiTextBox::GetDrawBeginTop()
{
    int y = InnerGetAbsTop() + m_ViewRect.top;
    int endy = y + (m_ViewRect.bottom - m_ViewRect.top);

    int more_height = (m_ViewRect.bottom - m_ViewRect.top) % m_LineHeight;

    // Ĭ���϶���
    //if (m_nVAlign == CGuiConst::V_ANCHOR_TOP)
    // �¶���
    if (m_nVAlign == CGuiConst::V_ANCHOR_BOTTOM)
    {
        y = InnerGetAbsTop() + m_ViewRect.top + more_height;
        endy = y + (m_ViewRect.bottom - m_ViewRect.top);
    }
    // ���ж���
    else if (m_nVAlign == CGuiConst::V_ANCHOR_CENTER)
    {
        y = InnerGetAbsTop() + m_ViewRect.top + more_height / 2;
        endy = y + (m_ViewRect.bottom - m_ViewRect.top);
    }

    return y;
}

const char* MultiTextBox::GetImageData(int itemindex, int imageindex)
{
    if (itemindex >= 0 && itemindex < (int)m_HtmlItems.size() && 
        imageindex >= 0 && imageindex < (int)m_HtmlItems[itemindex].m_ImageList.size())
    {
        DRAWIMAGEITEM& drawimage = 
            m_HtmlItems[itemindex].m_ImageList[imageindex];

        return drawimage.imageItem.m_MarkStr.c_str();
    }

    return "";
}

const wchar_t* MultiTextBox::GetHyperLinkData(int itemindex, int linkindex)
{
    if (itemindex >= 0 && itemindex < (int)m_HtmlItems.size() &&
        linkindex >= 0 && linkindex < (int)m_HtmlItems[itemindex].m_vHyperLinks.size())
    {
        HYPERLINKITEM& hyperlink = 
            m_HtmlItems[itemindex].m_vHyperLinks[linkindex];

        return hyperlink.m_wcsLink.c_str();
    }

    return L"";
}

void MultiTextBox::GetImagePlace(const IVarList& params, IVarList& result)
{
    int item_index = params.IntVal(0);
    int image_index = params.IntVal(1);

    rect_t rect = GetImageRect(item_index, image_index);

    result.AddInt(rect.left);
    result.AddInt(rect.top);
    result.AddInt(rect.right - rect.left);
    result.AddInt(rect.bottom - rect.top);
}

ImageAnimationManager* MultiTextBox::InnerGetAnimation()
{
	const IVar& var = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(var.ObjectVal());
    //return (ImageAnimationManager*)GetCore()->GetSingle(
    //    "ImageAnimationManager");
}

HyperLinkStyleManager* MultiTextBox::InnerGetHyperlinkMng()
{
	//return (HyperLinkStyleManager*)GetCore()->GetSingle(
    //     "HyperLinkStyleManager");
	const IVar& var = GetCore()->GetGlobalValue("HyperLinkStyleManager");
	return (HyperLinkStyleManager*)GetCore()->GetEntity(var.ObjectVal());
}

bool MultiTextBox::RefreshContentPos()
{
    // �������ݸ߶��е�һ����,�����жϻ�������
    int lContentTop = GetDrawItemTop();
    int lContentBottom = lContentTop + (m_ViewRect.bottom - m_ViewRect.top);

    // �����������,��������,���ڻ���
    int x = InnerGetAbsLeft() + m_ViewRect.left;
    int y = GetDrawBeginTop();
    int endy = y + (m_ViewRect.bottom - m_ViewRect.top);

    // ��ǰ�����ݵ��������ݸ߶��е���ʼλ��
    int ContentBegin = 0;
    size_t item_num = m_HtmlItems.size();

    for (size_t i = 0; i < item_num; ++i)
    {
        DrawHtmlItem* pItem = &m_HtmlItems[i];

        if (!IsShowKey(pItem->m_nKey))
        {
            continue;
        }

        if (ContentBegin < lContentBottom 
            && ContentBegin + pItem->m_ContentHeight > lContentTop)
        {
            pItem->m_ContentTop = ContentBegin - lContentTop;
            pItem->m_ContentBottom = pItem->m_ContentTop + pItem->m_ContentHeight;
        }
        else
        {
            pItem->m_ContentTop = ERROR_HEIGHT_CONTENT;
            pItem->m_ContentBottom = ERROR_HEIGHT_CONTENT;
        }

        ContentBegin += pItem->m_ContentHeight;
    }

    return true;
}

// ��ȡ�����ӵ���Ļ����
void MultiTextBox::GetHyperLinkRect(const IVarList& key, IVarList& result)
{
    // �������ݸ߶��е�һ����,�����жϻ�������
    int lContentTop = GetDrawItemTop();
    int lContentBottom = lContentTop + (m_ViewRect.bottom - m_ViewRect.top);

    // �����������,��������,���ڻ���
    int x = InnerGetAbsLeft() + m_ViewRect.left;
    int y = GetDrawBeginTop();

	const wchar_t* key_string = key.WideStrVal(0);

    size_t item_count = m_HtmlItems.size();

    for (size_t i = 0; i < item_count; ++i)
    {
        size_t hyperlink_count = m_HtmlItems[i].m_vHyperLinks.size();

        for (size_t j = 0; j < hyperlink_count; ++j)
        {
            if (m_HtmlItems[i].m_vHyperLinks[j].m_wcsLink.find(key_string) 
                != std::string::npos)
            {
                for (size_t link_index = 0; 
                    link_index < m_HtmlItems[i].m_vHyperLinks[j].m_OneLink.size();
                    ++link_index)
                {
                    const point_t& pt = m_HtmlItems[i].m_vHyperLinks[j].m_OneLink[link_index];

                    const rect_t& rect = m_HtmlItems[i].m_TextList[pt.x].m_TextLine[pt.y].rect;

                    if (rect.top >= lContentTop && rect.bottom <= lContentBottom)
                    {
                        if (result.GetCount() == 0)
                        {
                            result.AddBool(true);
                        }

                        result.AddInt(x + rect.left);
                        result.AddInt(rect.top - lContentTop + y);
                        result.AddInt(x + rect.right);
                        result.AddInt(rect.bottom - lContentTop + y);
                    }
                }
            }
        }

        y = y + m_HtmlItems[i].m_ContentHeight;

    }

    if (result.GetCount() == 0)
    {
        result.AddBool(false);
    }
}