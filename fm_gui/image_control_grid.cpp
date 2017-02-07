//--------------------------------------------------------------------
// �ļ���:		ImageControlGrid.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2009��3��3��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#include "image_control_grid.h"
#include "../tools/string_util.h"
#include "scroll_bar.h"
#include "button.h"
#include "multitext_box.h"
#include "imageanimation_manager.h"

/// \file ImageControlGrid.cpp
/// \brief ͼƬ�ؼ�����ؼ�

/// entity: ImageControlGrid
/// \brief ͼƬ�ؼ�����ؼ�
DECLARE_ENTITY(ImageControlGrid, 0, ImageGrid);

/// property: int ButtonCount,��ǰ���4��
/// \brief ���Ӱ�ť����
DECLARE_PROPERTY(int, ImageControlGrid, ButtonCount, 
				 GetButtonCount, SetButtonCount);
/// property: bool HasMultiTextBox
/// \brief �Ƿ��и�����Ϣ��
DECLARE_PROPERTY(bool, ImageControlGrid, HasMultiTextBox, 
				 GetHasMultiTextBox, SetHasMultiTextBox);
 /// property: int MultiTextBoxCount
 /// \brief MultTextBox������
 DECLARE_PROPERTY(int, ImageControlGrid, MultiTextBoxCount, 
                GetMultiTextBoxCount, SetMultiTextBoxCount);
/// property: string ButtonPos
/// \brief ÿ����ť����ڸ��ӵ������ַ�����ʽ"x1,y1;x2,y2;x3,y3;x4,y4"
DECLARE_PROPERTY(const char*, ImageControlGrid, ButtonPos, 
				 GetButtonPos, SetButtonPos);
/// property: int MultiTextBoxLeft
/// \brief ������Ϣ����Ը��ӵ���ƫ��
DECLARE_PROPERTY(int, ImageControlGrid, MultiTextBoxLeft, 
				 GetMultiTextBoxLeft, SetMultiTextBoxLeft);
/// property: int MultiTextBoxTop
/// \brief ������Ϣ����Ը��ӵ���ƫ��
DECLARE_PROPERTY(int, ImageControlGrid, MultiTextBoxTop, 
				 GetMultiTextBoxTop, SetMultiTextBoxTop);

/// readonly: object Button1
/// \brief ���Ӱ�ť1
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, Button1, GetButton1);
/// readonly: object Button2
/// \brief ���Ӱ�ť2
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, Button2, GetButton2);
/// readonly: object Button3
/// \brief ���Ӱ�ť3
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, Button3, GetButton3);
/// readonly: object Button4
/// \brief ���Ӱ�ť4
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, Button4, GetButton4);

/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox1, GetMultiTextBox1);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox2, GetMultiTextBox2);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox3, GetMultiTextBox3);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox4, GetMultiTextBox4);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox5, GetMultiTextBox5);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox6, GetMultiTextBox6);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox7, GetMultiTextBox7);
/// \brief �����ı���
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox8, GetMultiTextBox8);

/// readonly: object MultiTextBox
/// \brief ������Ϣ��
DECLARE_PROPERTY_GET(PERSISTID, ImageControlGrid, MultiTextBox, GetMultiTextBox);

/// property: string MultiTextBoxPos
/// \brief ÿ���ı�������ڸ��ӵ������ַ�����ʽ"x1,y1;x2,y2;x3,y3;x4,y4"
DECLARE_PROPERTY(const char*, ImageControlGrid, MultiTextBoxPos, 
GetMultiTextBoxPos, SetMultiTextBoxPos);

#if 0	//DECLARE_METHOD
/// \brief ������Ʒ��Ϣ
/// \method bool bool SetItemInfo(int index, const wchar_t *pText);
/// \param index �������
/// \param pText ���ַ���
/// \return true
DECLARE_METHOD_2(bool, ImageControlGrid, SetItemInfo, int, const wchar_t*);

/// \brief ��ʾ��Ʒ������Ϣ
/// \method bool bool ShowItemAddInfo(int item_index, int info_index, bool show);
/// \param item_index �������
/// \param info_index multitext���
/// \param show �Ƿ���ʾ
/// \return true
DECLARE_METHOD_3(bool, ImageControlGrid, ShowItemAddInfo, int, int, bool);

///����ÿ��ĸ��Ӱ�ť�Ƿ���ʾ
///virtual bool ShowItemAddButton(int index, int buttonindex, bool bShow);
DECLARE_METHOD_3(bool, ImageControlGrid, ShowItemAddButton, int, int, bool);

//����ÿ��ĸ���ɨť�Ƿ����
///virtual bool DisableItemAddButton(int index, int buttonindex, bool bDisable);
DECLARE_METHOD_3(bool, ImageControlGrid, DisableItemAddButton, int, int, bool);

//����ÿ��ĸ���ɨť�Ƿ����
///virtual bool SetItemAddInfo(int index, int buttonindex, string);
DECLARE_METHOD_3(bool, ImageControlGrid, SetItemAddInfo, int, int, const wchar_t*);

DECLARE_METHOD_2(const wchar_t*, ImageControlGrid, GetItemAddText, int, int);
#endif
/// event: int on_click_addbutton(object self, int gridindex, int buttonindex)
/// param:self ����
/// param:gridindex �������
/// param:buttonindex ��ť���
DECLARE_EVENT(on_click_addbutton);

ImageControlGrid::ImageControlGrid()
{
    m_vPos.resize(MAX_ADDBUTTON);

    m_nMouseInButtonGridIndex = -1;
    m_nMouseInButtinIndex = -1;
    m_bPushSubButton = false;

}

//�����������һ����Ʒ,���ԭ���Ѵ������滻
bool ImageControlGrid::AddItem(int index, const char* pImageName, 
							   const wchar_t* pItemName, int ItemNumber, 
							   int lMark)
{
    if (!pImageName)
    {
        return false;
    }

	if (0 == pImageName[0])
	{
		return false;
	}

    // ��������ѱ�ʹ�����滻
    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        additem.index = index;
        additem.vbShowMultiTextBox.push_back(true);
        additem.vwcsText[0] = pItemName;
        additem.ButtonState[0] = BUTTON_SHOW;
        additem.ButtonState[1] = BUTTON_SHOW;
        additem.ButtonState[2] = BUTTON_SHOW;
        additem.ButtonState[3] = BUTTON_SHOW;
    }
    else
    {
        // �������δ��ʹ��,���½�
        AddImageItem additem;

        additem.index = index;
        additem.vbShowMultiTextBox.push_back(true);
        additem.vbShowMultiTextBox.resize(8);
        additem.vwcsText.resize(8);
        additem.vwcsText[0] = pItemName;
        for (size_t k = 0; k< m_vButtons.size(); ++k)
        {
            additem.ButtonState[k] = BUTTON_SHOW;
            additem.vbShowButton.push_back(true);
        }
        
        size_t i = 0;

        for (i = 0; i < m_vAddItems.size(); ++i)
        {
            if (m_vAddItems[i].index > additem.index)
            {
                break;
            }
        }

        if (i < m_vAddItems.size())
        {
            m_vAddItems.insert(m_vAddItems.begin() + i , additem);
        }
        else
        {
            m_vAddItems.push_back(additem);
        }
    }

    return ImageGrid::AddItem(index, pImageName, pItemName, ItemNumber, lMark);
}

//������������
bool ImageControlGrid::Clear()
{
    m_vAddItems.clear();

    return ImageGrid::Clear();
}

//ɾ��һ����Ʒ
bool ImageControlGrid::DelItem(int index)
{
    if (ImageGrid::DelItem(index))
    {
        for (size_t i = 0; i < m_vAddItems.size(); ++i)
        {
            if (m_vAddItems[i].index == index)
            {
                m_vAddItems.erase(m_vAddItems.begin() + i);
                return true;
            }
        }
    }

    return false;
}

//������Ʒ��Ϣ
bool ImageControlGrid::SetItemInfo(int index, const wchar_t* pText)
{
    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        additem.vwcsText[0] = pText;

        return true;
    }

    return false;
}

//����ÿ��ĸ��Ӱ�ť�Ƿ���
bool ImageControlGrid::ShowItemAddButton(int index, int buttonindex, 
										 bool bShow)
{
    if (buttonindex >= MAX_ADDBUTTON || buttonindex < 0)
    {
        return false;
    }

    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        additem.ButtonState[buttonindex] = (int)bShow;

        return true;
    }

    return false;
}

bool ImageControlGrid::DisableItemAddButton(int index, int buttonindex, 
											bool bDisable)
{
    if (buttonindex >= MAX_ADDBUTTON || buttonindex < 0)
    {
        return false;
    }

    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        if (bDisable)
        {
            additem.ButtonState[buttonindex] = BUTTON_DISABLE;
        }
        else
        {
            additem.ButtonState[buttonindex] = BUTTON_SHOW;
        }

        return true;
    }

    return false;
}

bool ImageControlGrid::ShowItemAddInfo(int item_index, int info_index, bool bShow)
{
    long UsedItemIndex =  GetItem(item_index);

    if (UsedItemIndex != -1)
    {
        if (info_index < 0 || info_index >= (int)m_vMultiTextBox.size())
        {
            return false;
        }

        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        if ((int)additem.vbShowMultiTextBox.size() > info_index)
        {
            additem.vbShowMultiTextBox[info_index] = bShow;
        }
        else
        {
            additem.vbShowMultiTextBox.resize(m_vMultiTextBox.size());
            additem.vbShowMultiTextBox[info_index] = bShow;
        }

        return true;
    }

    return false;
}

// ���ø��Ӹ�����Ϣ
bool ImageControlGrid::SetItemAddInfo(int index, int controlindex, const wchar_t* pText)
{
    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        if (controlindex < (int)additem.vwcsText.size())
        {
            additem.vwcsText[controlindex] = pText;
            return true;
        }
    }

    return false;
}

const wchar_t* ImageControlGrid::GetItemAddText(int index, int controlindex)
{
    long UsedItemIndex =  GetItem(index);

    if (UsedItemIndex != -1)
    {
        AddImageItem& additem = m_vAddItems[UsedItemIndex];

        if (controlindex < (int)additem.vwcsText.size())
        {
            return additem.vwcsText[controlindex].c_str();
        }
    }

    return L"";
}

// ʵ��IControl����麯��
bool ImageControlGrid::Init(const IVarList& args)
{
    SetSolid(false);

    return ImageGrid::Init(args);
}

bool ImageControlGrid::Shut()
{
    SetButtonCount(0);
    SetHasMultiTextBox(false);

    return ImageGrid::Shut();
}

void ImageControlGrid::PrepareDesignInfo(IDesignInfo* di)
{
    ImageGrid::PrepareDesignInfo(di);

    di->AddPropInfo("ButtonCount", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("HasMultiTextBox", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("MultiTextBoxCount", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("ButtonPos", IDesignInfo::TYPE_STRING, true, false);
    di->AddPropInfo("MultiTextBoxLeft", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("MultiTextBoxTop", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("MultiTextBoxPos", IDesignInfo::TYPE_STRING, true, false);
    //di->AddPropInfo("MultiTextPos", IDesignInfo::TYPE_STRING, true, false);

	di->AddSuborInfo("Button1", "Button");
	di->AddSuborInfo("Button2", "Button");
	di->AddSuborInfo("Button3", "Button");
	di->AddSuborInfo("Button4", "Button");
    di->AddSuborInfo("MultiTextBox1", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox2", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox3", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox4", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox5", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox6", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox7", "MultiTextBox");
    di->AddSuborInfo("MultiTextBox8", "MultiTextBox");

    di->AddEventInfo("on_click_addbutton");
}

bool ImageControlGrid::DrawEditAddControl(const int xbase, const int ybase, const int GridWidth, const int GridHeight, const float seconds)
{
    BeginShowAddControl();

    for (long i = 0; i < m_nRow; ++i)
    {
        for (long j = 0; j < m_nClomn; ++j)
        {
            long index = i * m_nClomn + j;

            int add_x = j * GridWidth;
            int add_y = i * GridHeight;

            // �Զ���λ��
            if (index < (int)m_vGridsPos.size())
            {
                add_x = m_vGridsPos[index].x;
                add_y = m_vGridsPos[index].y;
            }

            for (size_t m = 0; m < m_vMultiTextBox.size(); ++m)
            {
                IControl* pControl = 
                    (IControl*)GetCore()->GetEntity(m_vMultiTextBox[m]);

                if (pControl)
                {
                    if (m < m_vMultiTextBoxPos.size())
                    {
                        pControl->SetAbsLeft(xbase + add_x + m_vMultiTextBoxPos[m].x);
                        pControl->SetAbsTop(ybase + add_y + m_vMultiTextBoxPos[m].y);
                    }
                    else
                    {
                        pControl->SetAbsLeft(xbase + add_x);
                        pControl->SetAbsTop(ybase + add_y);
                    }
                    pControl->Paint(seconds);
                }
            }

            for (size_t k=0; k<m_vButtons.size(); ++k)
            {
                Button* pControl = 
                    (Button*)GetCore()->GetEntity(m_vButtons[k]);

                if (pControl)
                {
                    pControl->SetAbsLeft(xbase + add_x + m_vPos[k].x);
                    pControl->SetAbsTop(ybase + add_y + m_vPos[k].y);

                    if (index == m_nMouseInButtonGridIndex 
                        && k == m_nMouseInButtinIndex)
                    {
                        if (m_bPushSubButton)
                        {
                            pControl->SetPressed(true);
                        }
                        else
                        {
                            pControl->SetCapture(true);
                        }
                    }
                    else
                    {
                        pControl->SetCapture(false);
                        pControl->SetPressed(false);
                    }

                    pControl->Paint(seconds);
                }
            }
        }
    }

    EndShowAddControl();
    
    return true;
}

bool ImageControlGrid::DrawOneAddItem(const int item_index, const AddImageItem& additem, 
                    const int xbase, const int ybase, const int x, const int y, 
                    const int GridWidth, const int GridHeight,
                    const int nGridWidth, const int nGridHeight, const float seconds)
{
    // ѡ���ı�
    for (size_t mi = 0; mi < additem.vbShowMultiTextBox.size(); ++mi)
    {
        if (additem.vbShowMultiTextBox[mi])
        {
            if (mi < m_vMultiTextBox.size())
            {
                MultiTextBox* pControl = 
                    (MultiTextBox*)GetCore()->GetEntity(m_vMultiTextBox[mi]);

                if (pControl)
                {
                    if (mi < m_vMultiTextBoxPos.size())
                    {
                        pControl->SetAbsLeft(xbase + x + m_vMultiTextBoxPos[mi].x);
                        pControl->SetAbsTop(ybase + y + m_vMultiTextBoxPos[mi].y);
                    }
                    else
                    {   
                        pControl->SetAbsLeft(xbase + x);
                        pControl->SetAbsTop(ybase + y);
                    }
                    pControl->SetHtmlText(additem.vwcsText[mi].c_str());

                    pControl->Paint(seconds);
                }
            }
        }
    }


    // �ٻ���ť
    for (size_t k = 0; k < m_vButtons.size(); ++k)
    {
        if (additem.vbShowButton[k])
        {
            if (additem.ButtonState[k] != BUTTON_HIDE)
            {
                Button* pControl = 
                    (Button*)GetCore()->GetEntity(m_vButtons[k]);

                if (pControl)
                {
                    pControl->SetAbsLeft(xbase + x + m_vPos[k].x);
                    pControl->SetAbsTop(ybase + y + m_vPos[k].y);

                    if (additem.ButtonState[k] == BUTTON_DISABLE)
                    {
                        pControl->SetCapture(false);
                        pControl->SetPressed(false);
                        pControl->SetEnabled(false);
                    }
                    else
                    {
                        pControl->SetEnabled(true);

                        if (item_index == m_nMouseInButtonGridIndex 
                            && k == m_nMouseInButtinIndex)
                        {
                            if (m_bPushSubButton)
                            {
                                pControl->SetPressed(true);
                            }
                            else
                            {
                                pControl->SetCapture(true);
                            }
                        }
                        else
                        {
                            pControl->SetCapture(false);
                            pControl->SetPressed(false);
                        }
                    }


                    pControl->Paint(seconds);
                }
            }
        }

    }

    return true;
}

void ImageControlGrid::Paint(float seconds)
{
    if (!GetVisible())
    {
        return;
    }

	RefreshInnerAbsPos();

    // ��һ�λ���,���г�ʼ��------------------------------------
    if (m_IsFirstTime)
    {
        DrawFirstTime();
    }

    IScrollable::Paint(seconds);

    // ��ʼ��һЩ��������---------------------------------------
    // �༭ģʽ�²Ż���ʾ�߿�
    rect_t ViewRect = m_ViewRect;

	ViewRect.left = ViewRect.left + InnerGetAbsLeft();
	ViewRect.right = ViewRect.right + InnerGetAbsLeft();
	ViewRect.top = ViewRect.top + InnerGetAbsTop();
	ViewRect.bottom = ViewRect.bottom + InnerGetAbsTop();

    long ViewWidth = ViewRect.right - ViewRect.left;
    long ViewHeight = ViewRect.bottom - ViewRect.top;

	int nGridWidth = m_nGridWidth;
	int nGridHeight = m_nGridHeight;

    long xbase = ViewRect.left;
    long ybase = ViewRect.top;

    int GridWidth = ViewWidth / m_nClomn;
    int GridHeight = ViewHeight / m_nRow;

    ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

    // �������ݸ߶��е�һ����,�����жϻ�������
    long lContentTop = m_nTopIndex;

    if (pScrollbar && pScrollbar->GetVisible())
	{
		lContentTop = pScrollbar->GetCurValue();
	}

    size_t indexstart = lContentTop * m_nClomn;
    size_t indexend = indexstart + m_nRow * m_nClomn;

    // ��������б����Ļ��ͻ�����
    DrawGridBack(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);

    // �༭ģʽ�»����߿�-----------------------------------------
    if (m_bEdit)
    {
        DrawEdit(ViewRect, xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight);
    }

    // ��ʼ���Ƹ���------------------------------------------------
    CoolManager* pCoolManager = InnerGetCoolManager();

    long CurRow = 0;
    long CurClomn = 0;

    long x = 0;
    long y = 0;

    long dx = 0;
    long dy = 0;

    // ��ʾ���ӿؼ�
    BeginShowAddControl();

    for (size_t i = 0; i < m_vItems.size(); ++i)
    {
        if (m_vItems[i]->Index >= indexstart 
			&& m_vItems[i]->Index < indexend)
        {
            size_t realindex = (size_t)(m_vItems[i]->Index - indexstart);

            CurClomn =  realindex % m_nClomn;

            CurRow = realindex / m_nClomn;

            if (CurRow > m_nRow)
            {
                continue;
            }

            // ͨ�����������Ͻ�
            x = CurClomn * GridWidth;
            y = CurRow * GridHeight;

            // ������Զ���λ��,��ȡ�Զ���λ�����Ͻ�
            if (realindex < m_vGridsPos.size())
            {
				x = m_vGridsPos[realindex].x;
				y = m_vGridsPos[realindex].y;
            }

			int old_alpha = GetPainter()->GetBlendAlpha();
			int cur_alpha = GetPainter()->SetBlendAlpha(old_alpha * m_vItems[i]->Alpha / 255);

            // ����ImageGrid���ֵĸ�������
            DrawOneGrid(*m_vItems[i], xbase, ybase, x, y, nGridWidth, nGridHeight);

            // ���ư�ť��multitextbox
            DrawOneAddItem(m_vItems[i]->Index, m_vAddItems[i], xbase, ybase, x, y, 
                GridWidth, GridHeight, nGridWidth, nGridHeight, seconds);

			GetPainter()->SetBlendAlpha(old_alpha);
        }
    }

    // ���ظ��ӿؼ�--------------------------------------------------
    EndShowAddControl();

    // �������------------------------------------------------------
    DrawImageHelpByIndex(m_MouseInImage, m_MouseInColor, m_nCurMouseIn, 
        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
        nGridWidth, nGridHeight);

    // ���ѡ��------------------------------------------------------
    DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
        nGridWidth, nGridHeight);

    // ��������б����õĻ�����ƽ��ÿ�/ͼƬ-------------------------
    DrawCover(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);

    // �༭ģʽ�»��Ƹ��ӿؼ�----------------------------------------
    if (m_bEdit)
    {
        DrawEditAddControl(xbase, ybase, GridWidth, GridHeight, seconds);
    }
}

void ImageControlGrid::BeginShowAddControl()
{
    // ��ʾ�ؼ�
    for (size_t i=0; i<m_vButtons.size(); ++i)
    {
        Button* pControl = (Button*)GetCore()->GetEntity(m_vButtons[i]);

        if (pControl)
        {
            pControl->SetVisible(true);
        }
    }

    for (size_t j = 0; j < m_vMultiTextBox.size(); ++j)
    {
        IControl* pControl = (IControl*)GetCore()->GetEntity(m_vMultiTextBox[j]);

        if (pControl)
        {
            pControl->SetVisible(true);
        }
    }

}

void ImageControlGrid::EndShowAddControl()
{
    // ���ؿؼ�
    for (size_t i=0; i<m_vButtons.size(); ++i)
    {
        Button* pControl = (Button*)GetCore()->GetEntity(m_vButtons[i]);

        if (pControl)
        {
            pControl->SetVisible(false);
        }
    }

    for (size_t j = 0; j < m_vMultiTextBox.size(); ++j)
    {
        IControl* pControl = (IControl*)GetCore()->GetEntity(m_vMultiTextBox[j]);

        if (pControl)
        {
            pControl->SetVisible(false);
        }
    }
}

void ImageControlGrid::SetButtonCount(int value)
{
    if (value < 0)
    {
        value = 0;
    }

    if (value > MAX_ADDBUTTON)
    {
        value = MAX_ADDBUTTON;
    }

    if (value == (int)m_vButtons.size())
    {
        return;
    }

    // ��ť�����ˣ�ɾ��
    if (value < (int)m_vButtons.size())
    {
        int cur_count = (int)m_vButtons.size();
        for (int i = value; i < cur_count; ++i)
        {
            IControl* pControl = 
				(IControl*)GetCore()->GetEntity(m_vButtons.back());

            if (pControl)
            {
                // ������ӹ�ϵ
                this->RemoveSuborControl(pControl);

                // ����
                pControl->Release();

                // ��Button�б����Ƴ�
                m_vButtons.erase(m_vButtons.end() - 1);
            }
        }
    }
    else    // ��ť�����ˣ����
    {
        int cur_count = (int)m_vButtons.size();
        for (int i = cur_count; i < value; ++i)
        {
            // �����������ӹ�ϵ
            IControl* pControl = 
				(IControl*)CreateSuborControl(this, "Button");

            if (pControl)
            {
                std::string szValue = StringUtil::IntAsString(i);

                pControl->SetName(("Button" + szValue).c_str());

                // ��ӵ��б���
                m_vButtons.push_back(pControl->GetID());
            }
        }

        if (m_vAddItems.size() > 0)
        {
            for (size_t i = 0; i < m_vAddItems.size(); ++i)
            {
                m_vAddItems[i].vbShowButton.resize(m_vButtons.size());
            }
        }
    }
}

int ImageControlGrid::GetButtonCount()
{
    return m_vButtons.size();
}

void ImageControlGrid::SetHasMultiTextBox(bool bValue)
{
    if (bValue)
    {
        if (m_vMultiTextBox.size() > 0)
        {
            return;
        }
        else
        {
            SetMultiTextBoxCount(1);
        }
    }
    else
    {
        SetMultiTextBoxCount(0);
    }
}

bool ImageControlGrid::GetHasMultiTextBox()
{
    return m_vMultiTextBox.size() > 0;
}

void  ImageControlGrid::SetMultiTextBoxCount(int value)
{
    if (value > 8)
    {
        value = 8;
    }
    else if (value < 0)
    {
        value = 0;
    }

    if ((int)m_vMultiTextBox.size() != value)
    {
        if (value > (int)m_vMultiTextBox.size())
        {
            int cur_count = m_vMultiTextBox.size();
            // ����ı���
            for (int i = cur_count; i < value; ++i)
            {
                // �����������ӹ�ϵ
                IControl* pControl = 
                    (IControl*)CreateSuborControl(this, "MultiTextBox");

                std::string szValue = "MultiTextBox" + StringUtil::IntAsString(i);
                pControl->SetName(szValue.c_str());
                m_vMultiTextBox.push_back(pControl->GetID());
            }
        }
        else
        {
            // �����ı���
            int cur_count = m_vMultiTextBox.size();
            for (int i = value; i < cur_count; ++i)
            {
                // ɾ�����һ��
                PERSISTID back_id = m_vMultiTextBox.back();
                m_vMultiTextBox.erase(m_vMultiTextBox.begin() + m_vMultiTextBox.size() - 1);
                
                IControl* pControl = (IControl*)GetCore()->GetEntity(back_id);

                if (pControl)
                {
                    // ������ӹ�ϵ
                    this->RemoveSuborControl(pControl);

                    // ����
                    pControl->Release();
                }
            }
        }
    }
}

int  ImageControlGrid::GetMultiTextBoxCount()
{
    return m_vMultiTextBox.size();
}

void ImageControlGrid::SetButtonPos(const char* pValue)
{
    if (pValue == NULL)
    {
        return;
    }

    m_szButtonPos = pValue;

	CVarList vData;

	StringUtil::ParseToVector(pValue, ';', vData); //ÿ������֮���÷ֺŸ���

	for (size_t i = 0; i < m_vButtons.size() && i < vData.GetCount(); ++i)
	{
		CVarList vSubData;

		StringUtil::ParseToVector(vData.StringVal(i), ',', vSubData); //x��y�����ö��Ÿ���

		if (i >= m_vPos.size())
		{
			point_t pt;

			m_vPos.push_back(pt);
		}

		if (vSubData.GetCount() < 2)
		{
			continue;
		}

		m_vPos[i].x = StringUtil::StringAsInt(vSubData.StringVal(0));
		m_vPos[i].y = StringUtil::StringAsInt(vSubData.StringVal(1));
	}
    /*
	std::vector<std::string> vData;
    
	StringUtil::ParseToVector(vData, pValue, ";"); //ÿ������֮���÷ֺŸ���

    for (size_t i = 0; i < m_vButtons.size() && i < vData.size(); ++i)
    {
        std::vector<std::string> vSubData;

        StringUtil::ParseToVector(vSubData, vData[i].c_str(), ","); //x��y�����ö��Ÿ���

        if (i >= m_vPos.size())
        {
            point_t pt;

            m_vPos.push_back(pt);
        }

        if (vSubData.size() < 2)
        {
            continue;
        }

        m_vPos[i].x = StringUtil::StringAsInt(vSubData[0]);
        m_vPos[i].y = StringUtil::StringAsInt(vSubData[1]);
    }
	*/
}

const char* ImageControlGrid::GetButtonPos()
{
    if (m_szButtonPos.empty())
    {
        return "";
    }

    return m_szButtonPos.c_str();
}

PERSISTID ImageControlGrid::GetButton1()
{
    return GetButtonByIndex(0);
}

PERSISTID ImageControlGrid::GetButton2()
{
    return GetButtonByIndex(1);
}

PERSISTID ImageControlGrid::GetButton3()
{
    return GetButtonByIndex(2);
}

PERSISTID ImageControlGrid::GetButton4()
{
    return GetButtonByIndex(3);
}

PERSISTID ImageControlGrid::GetButtonByIndex(int index)
{
    if (index < (int)m_vButtons.size())
    {
        return m_vButtons[index];
    }

    return PERSISTID();
}

PERSISTID ImageControlGrid::GetMultiTextBox()
{
    if (m_vMultiTextBox.size() > 0)
    {
        return m_vMultiTextBox[0];
    }

    return PERSISTID();
}

PERSISTID ImageControlGrid::GetMultiTextBox1()
{
    return GetMultiTextBoxByIndex(0);
}

PERSISTID ImageControlGrid::GetMultiTextBox2()
{
    return GetMultiTextBoxByIndex(1);
}

PERSISTID ImageControlGrid::GetMultiTextBox3()
{
    return GetMultiTextBoxByIndex(2);
}

PERSISTID ImageControlGrid::GetMultiTextBox4()
{
    return GetMultiTextBoxByIndex(3);
}

PERSISTID ImageControlGrid::GetMultiTextBox5()
{
    return GetMultiTextBoxByIndex(4);
}

PERSISTID ImageControlGrid::GetMultiTextBox6()
{
    return GetMultiTextBoxByIndex(5);
}

PERSISTID ImageControlGrid::GetMultiTextBox7()
{
    return GetMultiTextBoxByIndex(6);
}

PERSISTID ImageControlGrid::GetMultiTextBox8()
{
    return GetMultiTextBoxByIndex(7);
}

PERSISTID ImageControlGrid::GetMultiTextBoxByIndex(int index)
{
    if (index < (int)m_vMultiTextBox.size())
    {
        return m_vMultiTextBox[index];
    }

    return PERSISTID();
}


void ImageControlGrid::SetMultiTextBoxPos(const char* pValue)
{
    if (pValue == NULL)
    {
        return;
    }

    m_szMultiTextboxPos = pValue;

	CVarList vData;

	StringUtil::ParseToVector(pValue, ';', vData); //ÿ������֮���÷ֺŸ���

	for (size_t i = 0; i < m_vMultiTextBox.size() && i < vData.GetCount(); ++i)
	{
		CVarList vSubData;

		StringUtil::ParseToVector(vData.StringVal(i), ',', vSubData); //x��y�����ö��Ÿ���

		if (i >= m_vMultiTextBoxPos.size())
		{
			point_t pt;

			m_vMultiTextBoxPos.push_back(pt);
		}

		if (vSubData.GetCount() < 2)
		{
			continue;
		}

		m_vMultiTextBoxPos[i].x = StringUtil::StringAsInt(vSubData.StringVal(0));
		m_vMultiTextBoxPos[i].y = StringUtil::StringAsInt(vSubData.StringVal(1));
	}
    /*
	std::vector<std::string> vData;

    StringUtil::ParseToVector(vData, pValue, ";"); //ÿ������֮���÷ֺŸ���

    for (size_t i = 0; i < m_vMultiTextBox.size() && i<vData.size(); ++i)
    {
        std::vector<std::string> vSubData;

        StringUtil::ParseToVector(vSubData, vData[i].c_str(), ","); //x��y�����ö��Ÿ���

        if (i >= m_vMultiTextBoxPos.size())
        {
            point_t pt;

            m_vMultiTextBoxPos.push_back(pt);
        }

        if (vSubData.size() < 2)
        {
            continue;
        }

        m_vMultiTextBoxPos[i].x = StringUtil::StringAsInt(vSubData[0]);
        m_vMultiTextBoxPos[i].y = StringUtil::StringAsInt(vSubData[1]);
    }
	*/
}

const char* ImageControlGrid::GetMultiTextBoxPos()
{
    if (m_szMultiTextboxPos.empty())
    {
        return "";
    }

    return m_szMultiTextboxPos.c_str();
}

void ImageControlGrid::SetMultiTextBoxLeft(int value)
{
    if (m_vMultiTextBoxPos.size() > 0)
    {
        m_vMultiTextBoxPos[0].x = value;
    }
    else
    {
        point_t pt;
        pt.x = value;
        m_vMultiTextBoxPos.push_back(pt);
    }
}

int ImageControlGrid::GetMultiTextBoxLeft()
{
    if (m_vMultiTextBoxPos.size() > 0)
    {
        return m_vMultiTextBoxPos[0].x;
    }

    return 0;
}

void ImageControlGrid::SetMultiTextBoxTop(int value)
{
    if (m_vMultiTextBoxPos.size() > 0)
    {
        m_vMultiTextBoxPos[0].y = value;
    }
    else
    {
        point_t pt;
        pt.y = value;
        m_vMultiTextBoxPos.push_back(pt);
    }
}

int ImageControlGrid::GetMultiTextBoxTop()
{
    if (m_vMultiTextBoxPos.size() > 0)
    {
        return m_vMultiTextBoxPos[0].y;
    }

    return 0;
}

int ImageControlGrid::OnMouseMove(int x, int y)
{
    ImageGrid::OnMouseMove(x, y);

    point_t pt = {x, y };

    // �༭ģʽ�²Ż���ʾ�߿�
    rect_t ViewRect = m_ViewRect;

	ViewRect.left = ViewRect.left + InnerGetAbsLeft();
	ViewRect.right = ViewRect.right + InnerGetAbsLeft();
	ViewRect.top = ViewRect.top + InnerGetAbsTop();
	ViewRect.bottom = ViewRect.bottom + InnerGetAbsTop();

    long ViewWidth = ViewRect.right - ViewRect.left;
    long ViewHeight = ViewRect.bottom - ViewRect.top;

	int nGridWidth = m_nGridWidth;
	int nGridHeight = m_nGridHeight;

    long xbase = ViewRect.left;
    long ybase = ViewRect.top;

    int GridWidth = ViewWidth / m_nClomn;
    int GridHeight = ViewHeight / m_nRow;

    if (m_bEdit)
    {
        bool bFound = false;

        for (long i = 0; i < m_nRow; ++i)
        {
            for (long j = 0; j < m_nClomn; ++j)
            {
                long grid_index = i * m_nClomn + j;

                int add_x = j * GridWidth;
                int add_y = i * GridHeight;

                // �Զ���λ��
                if (grid_index < (int)m_vGridsPos.size())
                {
					add_x = m_vGridsPos[grid_index].x;
					add_y = m_vGridsPos[grid_index].y;
                }

                if (grid_index < (long)m_vGridsPos.size())
                {
                    for (size_t i = 0; i < m_vButtons.size(); ++i)
                    {
                        IControl* pControl = 
							(IControl*)GetCore()->GetEntity(m_vButtons[i]);
                        
						if (pControl)
                        {
							rect_t targetRect = { xbase + add_x + m_vPos[i].x,
								ybase + add_y + m_vPos[i].y, 0,0 };

                            targetRect.right = targetRect.left 
								+ pControl->GetWidth();
                            targetRect.bottom = targetRect.top 
								+ pControl->GetHeight();

                            if (PtInRect(&targetRect, pt))
                            {
                                m_nMouseInButtonGridIndex = grid_index;
                                m_nMouseInButtinIndex = i;
                                bFound = true;
                                break;
                            }
                        }
                    }
                }

                if (bFound)
                {
                    break;
                }
            }

            if (bFound)
            {
                break;
            }
        }

        if (!bFound)
        {
            m_nMouseInButtonGridIndex = -1;
            m_nMouseInButtinIndex = -1;
        }
    }
    else
    {
        bool bFound = false;

        ScrollBar* pScrollbar = 
			(ScrollBar*)GetGui()->GetControl(GetVScrollBar());

        // �������ݸ߶��е�һ����,�����жϻ�������
        long lContentTop = m_nTopIndex;

        if (pScrollbar && pScrollbar->GetVisible())
		{
			lContentTop = pScrollbar->GetCurValue();
		}

        size_t indexstart = lContentTop * m_nClomn;
        size_t indexend = indexstart + m_nRow * m_nClomn;

        for (size_t i = 0; i < m_vItems.size(); ++i)
        {
            if (m_vItems[i]->Index >= indexstart 
				&& m_vItems[i]->Index < indexend)
            {
                size_t realindex = (size_t)(m_vItems[i]->Index - indexstart);

                int CurClomn =  realindex % m_nClomn;

                int CurRow = realindex / m_nClomn;

                if (CurRow > m_nRow)
                {
                    continue;
                }

                int add_x = CurClomn * GridWidth;
                int add_y = CurRow * GridHeight;

                // �Զ���λ��
                if (realindex < m_vGridsPos.size())
                {
					add_x = m_vGridsPos[realindex].x;
					add_y = m_vGridsPos[realindex].y;
                }

                AddImageItem& additem = m_vAddItems[i];

                for (size_t k = 0; k < m_vButtons.size(); ++k)
                {
                    if (additem.ButtonState[k] != BUTTON_HIDE)
                    {
                        Button* pControl = 
							(Button*)GetCore()->GetEntity(m_vButtons[k]);

                        if (pControl)
                        {
							rect_t rect = { xbase + add_x + m_vPos[k].x, 
								ybase + add_y + m_vPos[k].y, 0, 0 };
                            
							rect.right = rect.left + pControl->GetWidth();
                            rect.bottom = rect.top + pControl->GetHeight();
                            
							if (PtInRect(&rect, pt))
                            {
                                m_nMouseInButtonGridIndex = m_vItems[i]->Index;
                                m_nMouseInButtinIndex = k;
                                bFound = true;
                                break;
                            }
                        }
                    }

                } 
            }

            if (bFound)
            {
                break;
            }
        }

        if (!bFound)
        {
            m_nMouseInButtonGridIndex = -1;
            m_nMouseInButtinIndex = -1;
        }
    }

    return 1;
}

int ImageControlGrid::OnLButtonDown(int x, int y)
{
    if (m_nMouseInButtinIndex != -1 && m_nMouseInButtonGridIndex != -1)
    {
        m_bPushSubButton = true;
    }
    else
    {
        ImageGrid::OnLButtonDown(x, y);
    }

    return 1;
}

int ImageControlGrid::OnLButtonUp(int x, int y)
{
    if (m_bPushSubButton)
    {
        int res = GuiUtil_RunCallback(this, "on_click_addbutton", 
			CVarList() << m_nMouseInButtonGridIndex << m_nMouseInButtinIndex);
    }
    else
    {
        ImageGrid::OnLButtonUp(x, y);
    }
    
	m_bPushSubButton = false;

    return 1;
}

