//--------------------------------------------------------------------
// �ļ���:		GroupScrollableBox.h
// ��  ��:		�Զ����������,��ӵ�й������Ŀؼ������ؼ�
// ˵  ��:		
// ��������:	2008��9��10��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "group_scrollable_box.h"
#include "scroll_bar.h"
#include "gui_scroll.h"

/// entity: GroupScrollableBox
/// \brief �����ؼ���������
/// \see �̳���GroupBox
DECLARE_ENTITY(GroupScrollableBox, 0, GroupBox);

/// readonly: object VScrollBar
/// \brief ��ֱ����������
DECLARE_PROPERTY_GET(PERSISTID, GroupScrollableBox, VScrollBar, GetVScrollBar);
/// property: int ScrollSize
/// \brief �������ĳߴ�
DECLARE_PROPERTY(int, GroupScrollableBox, ScrollSize, GetScrollSize, 
	SetScrollSize);
/// property: bool HasVScroll
/// \brief �Ƿ������ֱ������
DECLARE_PROPERTY(bool, GroupScrollableBox, HasVScroll, GetHasVScroll, 
	SetHasVScroll);
/// property: bool AlwaysVScroll
/// \brief �Ƿ�������ʾ��ֱ������
DECLARE_PROPERTY(bool, GroupScrollableBox, AlwaysVScroll, GetAlwaysVScroll, 
	SetAlwaysVScroll);
/// property: bool VScrollLeft
/// \brief ��ֱ�������Ƿ��������ʾ
DECLARE_PROPERTY(bool, GroupScrollableBox, VScrollLeft, GetVScrollLeft, 
	SetVScrollLeft);
/// property: bool IsEdit
/// \brief �Ƿ����ڱ༭
DECLARE_PROPERTY(bool, GroupScrollableBox, IsEditMode, GetIsEdit, SetIsEdit);

#if 0	//DECLARE_METHOD
/// method: bool ResetChildrenYPos()
/// �����ϸ������ϵ�Y����,X����,�����ӿؼ���Yֵ����ݿؼ�˳��0��ʼ���δ��ϵ���һ������һ������
/// ����ʾ�Ŀؼ���������
DECLARE_METHOD_0(bool, GroupScrollableBox, ResetChildrenYPos);
/// method:int GetContentHeight();
/// ��ȡʵ����ʾ���ݸ߶�
DECLARE_METHOD_0(int, GroupScrollableBox, GetContentHeight);
/// method:int GetChildControlCount();
/// ��ȡ�ӿؼ�����
DECLARE_METHOD_0(int, GroupScrollableBox, GetChildControlCount);
/// method:PERSISTID GetChildControlByIndex(int index);
/// ͨ����Ż�ȡ�ӿؼ�ID
DECLARE_METHOD_1(PERSISTID, GroupScrollableBox, GetChildControlByIndex, int);

/// method:bool UpdateScroll();
/// ���¹�������
DECLARE_METHOD_0(bool, GroupScrollableBox, UpdateScroll);
#endif

GroupScrollableBox::GroupScrollableBox()
{
    m_nTopIndex = 0;
    m_nScrollSize = 17;
    m_bVScrollLeft = false;
    m_bHasVScroll = false;
    m_bAlwaysVScroll = false;
    m_bIsEdit = true;
    m_pVScrollBar = NULL;

	m_nDragOldX = 0;
	m_nDragOldY = 0;

    //���ÿؼ��ɱ�ѡ��
    SetCanSelect(true);
    //���ÿؼ��ɱ�����Ϊ����
    SetCanFocus(true);
}

GroupScrollableBox::~GroupScrollableBox()
{
}

bool GroupScrollableBox::Init(const IVarList& args)
{
    if (!GroupBox::Init(args))
	{
		return false;
	}

    return true;
}

bool GroupScrollableBox::Shut()
{
	SAFE_RELEASE(m_pVScrollBar);

    return GroupBox::Shut();
}

void GroupScrollableBox::PrepareDesignInfo(IDesignInfo* di)
{
    GroupBox::PrepareDesignInfo(di);

	CVar varDefault;
    di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true, &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("HasVScroll", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, false);
    di->AddPropInfo("IsEditMode", IDesignInfo::TYPE_BOOL, true, false, &(varDefault.SetBool(true),varDefault));

    di->AddSuborInfo("VScrollBar", "ScrollBar");

	di->AddEventInfo("on_drag_move");
}

bool GroupScrollableBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pVScrollBar && !m_pVScrollBar->GetLoadFinish())
	{
		return false;
	}
	
	return GroupBox::GetLoadFinish();
}

// ����������
int GroupScrollableBox::OnLButtonDown(int x, int y)
{
	m_nDragOldX = x;
	m_nDragOldY = y;

	return 0;
}

// ��ʼ�϶�
int GroupScrollableBox::OnDragMove(int x, int y)
{
	int drag_x = x - m_nDragOldX;
	int drag_y = y - m_nDragOldY;

	m_nDragOldX = x;
	m_nDragOldY = y;

	//GuiUtil_RunCallback(this, "on_drag_move", CVarList() << drag_x << drag_y);

	int total_height = GetVerticalTotal();	// ��ֱ����������
	int page_height = GetVerticalCanDisp();	// ������ʾ������
	if (total_height==0 || page_height==0)
	{
		return 0;
	}

	int new_value = GetVerticalValue() - drag_y;
	
	new_value = (new_value>total_height) ? total_height : new_value;
	new_value = (new_value<=0) ? 0 : new_value;

	SetVerticalValue(new_value);

	return 0;
}

void GroupScrollableBox::Paint(float seconds)
{
    if (!GroupBox::GetVisible())
    {
        return;
    }

    RefreshInnerAbsPos();

	int old_alpha = GetPainter()->GetBlendAlpha();

	// ���������͸���ˣ��ӽ���ҲӦ͸��
	GetPainter()->SetBlendAlpha(GetAlpha() * old_alpha / 255);

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
            GetGui()->SetFont(GuiUtil_GetValidFontName(this), 
				GetShadowColor());

            const wchar_t* text = GetShowText();

            int w = GetPainter()->GetTextWidth(text);
            int h = GetPainter()->GetTextHeight(text);

            int offs = h / 2;

            GetPainter()->DrawLine(x1 + offs, y1 + offs,
                x1 + offs, y2 - offs, GetLineColor());
            GetPainter()->DrawLine(x1 + offs, y2 - offs,
                x2 - offs + 1, y2 - offs, GetLineColor());
            GetPainter()->DrawLine(x2 - offs, y1 + offs,
                x2 - offs, y2 - offs, GetLineColor());
            GetPainter()->DrawLine(x1 + offs, y1 + offs,
                x1 + h, y1 + offs, GetLineColor());
            GetPainter()->DrawLine(x1 + h + w, y1 + offs,
                x2 - offs, y1 + offs, GetLineColor());

            GetPainter()->WriteText(x1 + h, y1, GetForeColor(), text);
        }
    }

	bool succeed = true;

    if (this->GetClipChildren())
	{
		succeed = GetPainter()->SetViewport(x1, y1, x2, y2);
	}

    if (succeed)
	{
        size_t sChildCount = GroupBox::GetChildCount();

        if (sChildCount > 0)
        {
            for (size_t i = 0; i < sChildCount; ++i)
            {
                IControl* pControl = GroupBox::GetChildByIndex(i);

                if (pControl)
                {
                    if (!m_bIsEdit)
                    {
                        const PERSISTID& id = pControl->GetID();

						long data = 0;

						m_ChildrenYPos.GetData(id, data);

						pControl->SetTop(data - m_nTopIndex);
                    }

                    pControl->Paint(seconds);
                }
            }
        }

        // ������ؼ�
        PaintTopLayer(seconds);

        if (m_pVScrollBar && m_pVScrollBar->GetVisible())
        {
            m_pVScrollBar->Paint(seconds);
        }

        if (this->GetClipChildren())
		{
			GetPainter()->ResetViewport();
		}
    }

    GetPainter()->SetBlendAlpha(old_alpha);
}

int GroupScrollableBox::OnMouseWheel(int x, int y, int delta)
{
	if (!CGuiScroll::TestScrollValid(m_pVScrollBar))
	{
		return 0;
	}

	m_pVScrollBar->IncValue(-delta* m_pVScrollBar->GetSmallChange());

	return 1;
}

int GroupScrollableBox::OnSuborEvent(IControl* subor, int event, 
	const IVarList& args)
{
    if (event == CGuiConst::EVENT_VALUE_CHANGED)
    {
        if (subor == (IControl*)m_pVScrollBar)
        {
            SetVerticalValue(m_pVScrollBar->GetCurValue());

            return 1;
        }
    }

    return 0;
}

int GroupScrollableBox::OnWidthChanged(int old)
{
    GroupBox::OnWidthChanged(old);

    UpdateScroll();

    return 0;
}

int GroupScrollableBox::OnHeightChanged(int old)
{
    GroupBox::OnHeightChanged(old);

    UpdateScroll();

    return 0;
}

// ����ӿؼ�
int GroupScrollableBox::OnAdd(const PERSISTID& child_id)
{
    int iReturn  = GroupBox::OnAdd(child_id);

    IControl* pControl = (IControl*)GetCore()->GetEntity(child_id);

    if (pControl)
    {
        const PERSISTID& id = pControl->GetID();

        m_ChildrenYPos.Remove(id);
		m_ChildrenYPos.Add(id, pControl->GetTop());
    }

    UpdateScroll();

    return iReturn;
}

// �Ƴ��ӿؼ�
int GroupScrollableBox::OnRemove(const PERSISTID& child_id)
{
    int iReturn  = GroupBox::OnRemove(child_id);

    m_ChildrenYPos.Remove(child_id);

    UpdateScroll();

    return iReturn;
}

IControl* GroupScrollableBox::OnGetInWhat(int x, int y)
{
    // ����ؼ�
	if (!m_TopLayer.empty())
	{
		int top_num = (int)m_TopLayer.size();
		PERSISTID* tops = &m_TopLayer[0];

		for (int i = top_num - 1; i >= 0; --i)
		{
			IControl* pChild = (IControl*)GetCore()->GetEntity(tops[i]);

			if (NULL == pChild)
			{
				continue;
			}

			IControl* pControl = pChild->GetInWhat(x, y);

			if (pControl != NULL)
			{
				return pControl;
			}
		}
	}
    /*
	TOP_CONTAINER::reverse_iterator it1;

    for (it1 = m_TopLayer.rbegin(); it1 != m_TopLayer.rend(); ++it1)
    {
        IControl* pChild = (IControl*)GetCore()->GetEntity(*it1);

        if (NULL == pChild)
        {
            continue;
        }

        IControl* pControl = pChild->GetInWhat(x, y);

        if (pControl != NULL)
        {
            return pControl;
        }
    }
	*/

    size_t sChildCount = GroupBox::GetChildCount();

    if (sChildCount > 0)
    {
        IControl* pInControl = NULL;

        for (size_t i = 0; i < sChildCount; ++i)
        {
            IControl* pControl = GroupBox::GetChildByIndex(i);

            if (pControl)
            {
                if (!m_bIsEdit)
                {
                    const PERSISTID& id = pControl->GetID();

					long data = 0;

					m_ChildrenYPos.GetData(id, data);

					pControl->SetTop(data - m_nTopIndex);
                }

                pInControl = pControl->GetInWhat(x, y);
            }

            if (pInControl != NULL)
            {
                return pInControl;
            }
        }
    }

    return IControl::OnGetInWhat(x, y);
}

void GroupScrollableBox::SetHasVScroll(bool value)
{
    m_bHasVScroll = value;

    if (m_bHasVScroll && (m_pVScrollBar == NULL))
    {
        m_pVScrollBar = CGuiScroll::CreateVScrollBar(this, m_nScrollSize);
    }

    UpdateScroll();
}

bool GroupScrollableBox::ResetChildrenYPos()
{
    size_t sChildCount = GroupBox::GetChildCount();

    long y = 0;

    if (sChildCount > 0)
    {
        IControl* pInControl = NULL;

        for (size_t i = 0; i < sChildCount; ++i)
        {
            IControl* pControl = GroupBox::GetChildByIndex(i);

            if (pControl && pControl->GetVisible())
            {
                // ����ʾ�Ŀؼ���������
                pControl->SetTop(y);

                const PERSISTID& id = pControl->GetID();

				m_ChildrenYPos.Remove(id);
				m_ChildrenYPos.Add(id, y);

                y += pControl->GetHeight();
            }
        }
    }

    UpdateScroll();

    return true;
}

int GroupScrollableBox::GetContentHeight()
{
    return GetVerticalTotal();
}

// ��ȡ�ӿؼ�����
int GroupScrollableBox::GetChildControlCount() const
{
    return (int)IContainer::GetChildCount();
}

// ���ָ��λ�õ��ӿؼ�
PERSISTID GroupScrollableBox::GetChildControlByIndex(int index)
{
    IControl* pControl = IContainer::GetChildByIndex(index);

    if (pControl)
	{
        return pControl->GetID();
	}
    
    return PERSISTID();
}

void GroupScrollableBox::SetScrollSize(int value)
{
    if ((value < 1) || (value >= 100))
    {
        return;
    }

    m_nScrollSize = value;

    if (m_pVScrollBar)
    {
        m_pVScrollBar->SetWidth(m_nScrollSize);
    }

    UpdateScroll();
}

int GroupScrollableBox::GetScrollSize() const
{
    return m_nScrollSize;
}

bool GroupScrollableBox::GetAlwaysVScroll() const 
{ 
    return m_bAlwaysVScroll; 
}

void GroupScrollableBox::SetAlwaysVScroll(bool value)
{
    m_bAlwaysVScroll = value;

    if (m_bAlwaysVScroll)
    {
        SetHasVScroll(true);
    }

    UpdateScroll();
}

void GroupScrollableBox::SetIsEdit(bool IsEdit)
{
    m_bIsEdit = IsEdit;

    if (!IsEdit)
    {
        size_t sChildCount = GroupBox::GetChildCount();

        if (sChildCount > 0)
        {
            IControl* pInControl = NULL;

            for (size_t i = 0; i < sChildCount; ++i)
            {
                IControl* pControl = GroupBox::GetChildByIndex(i);

                if (pControl)
                {
                    const PERSISTID& id = pControl->GetID();

					m_ChildrenYPos.Remove(id);
					m_ChildrenYPos.Add(id, pControl->GetTop());
                }
            }
        }
    }
}

bool GroupScrollableBox::GetHasVScroll() const
{
    return m_bHasVScroll;
}

void GroupScrollableBox::SetVScrollLeft(bool value)
{
    if (m_bVScrollLeft != value)
    {
		m_bVScrollLeft = value;

        UpdateScroll();
    }
}

bool GroupScrollableBox::GetVScrollLeft() const
{
    return m_bVScrollLeft;
}

// ģ�巽��
// ��ֱ����������
int GroupScrollableBox::GetVerticalTotal() const
{
    long nTotalHeight = 0;
    size_t sChildCount = GroupBox::GetChildCount();

    if (sChildCount > 0)
    {
        for (size_t i = 0; i < sChildCount; ++i)
        {
            IControl* pControl = GroupBox::GetChildByIndex(i);

            if (pControl && pControl->GetVisible())
            {
                if (nTotalHeight < pControl->GetTop() + pControl->GetHeight())
                {
                    nTotalHeight = pControl->GetTop() + pControl->GetHeight();
                }
            }
        }
    }

    return nTotalHeight;
}

// ������ʾ������
int GroupScrollableBox::GetVerticalCanDisp() const
{
    return GetHeight();
}

// ���õ�ǰֵ
void GroupScrollableBox::SetVerticalValue(int value)
{
    m_nTopIndex = value;
}

// ��ȡ��ǰֵ
int GroupScrollableBox::GetVerticalValue() const
{
    return m_nTopIndex;
}

PERSISTID GroupScrollableBox::GetVScrollBar()
{
    if (m_pVScrollBar)
    {
        return m_pVScrollBar->GetID();
    }

    return PERSISTID();
}

bool GroupScrollableBox::UpdateScroll()
{
    /*if (m_bInUpdate)
    {
        return false;
    }*/

    unsigned int state = 0;

    while (true)
    {
        unsigned int new_state = CalcScrollVisible();

        if (new_state == state)
        {
            break;
        }
        else
        {
            state = new_state;
        }
    }

    if (!CGuiScroll::TestScrollValid(m_pVScrollBar))
    {
        // ��������Чʱ��֤�ص�����
        SetVerticalValue(0);
    }

    CGuiScroll::UpdateVerticalScroll(m_pVScrollBar, GetVerticalCanDisp(), 
		GetVerticalTotal(), GetVerticalValue());

    CGuiScroll::CalcScrollRect(this, m_pVScrollBar, NULL, NULL, state, 
		m_bVScrollLeft);

    return true;
}

unsigned int GroupScrollableBox::CalcScrollVisible() const
{
    if (m_pVScrollBar)
    {
        m_pVScrollBar->SetVisible(CGuiScroll::GetVerticalVisible(
            m_pVScrollBar, GetVerticalCanDisp(), GetVerticalTotal(),
            m_bAlwaysVScroll));
    }

    return CGuiScroll::GetScrollState(m_pVScrollBar, NULL);
}

bool GroupScrollableBox::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        if (m_pVScrollBar != NULL) m_pVScrollBar->SetRefreshInnerAbsPos(true);

        GroupBox::RefreshInnerAbsPos();
    }

    return true;
}