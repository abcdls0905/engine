//--------------------------------------------------------------------
// 文件名:		combo_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "combo_box.h"
#include "edit.h"
#include "button.h"
#include "list_box.h"

/// \file combo_box.cpp
/// \brief 下拉输入框

/// entity: ComboBox
/// \brief 下拉输入框实体
/// \see 继承自IControl 
DECLARE_ENTITY(ComboBox, 0, IControl);

/// readonly: object InputEdit
/// \brief 输入文本框对象
DECLARE_PROPERTY_GET(PERSISTID, ComboBox, InputEdit, GetInputEdit);
/// readonly: object DropButton
/// \brief 下拉按钮
DECLARE_PROPERTY_GET(PERSISTID, ComboBox, DropButton, GetDropButton);
/// readonly: object DropListBox
/// \brief 下拉列表框
DECLARE_PROPERTY_GET(PERSISTID, ComboBox, DropListBox, GetDropListBox);

/// property: int ButtonWidth
/// \brief 下拉按钮的宽度
DECLARE_PROPERTY(int, ComboBox, ButtonWidth, GetButtonWidth, SetButtonWidth);
/// property: int DropDownWidth
/// \brief 下拉列表框宽度
DECLARE_PROPERTY(int, ComboBox, DropDownWidth, GetDropDownWidth, 
	SetDropDownWidth);
/// property: int DropDownHeight
/// \brief 下拉列表框高度
DECLARE_PROPERTY(int, ComboBox, DropDownHeight, GetDropDownHeight, 
	SetDropDownHeight);
/// property: bool OnlySelect
/// \brief 是否只允许选择
DECLARE_PROPERTY(bool, ComboBox, OnlySelect, GetOnlySelect, SetOnlySelect);
/// property: bool DroppedDown
/// \brief 下拉列表框当前是否打开
DECLARE_PROPERTY(bool, ComboBox, DroppedDown, GetDroppedDown, SetDroppedDown);

/// event: int on_selected(object self)
/// \brief 选择改变时的回调
/// \param self 本控件
DECLARE_EVENT(on_selected);

// ComboBox

ComboBox::ComboBox()
{
	m_pInputEdit = NULL;
	m_pDropButton = NULL;
	m_pDropListBox = NULL;
	m_bDroppedDown = false;
	m_nButtonWidth = 20;
	m_nDropDownWidth = 0;
	SetDesignSize(60, 20);
}

ComboBox::~ComboBox()
{
}

bool ComboBox::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	m_pInputEdit = (Edit*)CreateSuborControl(this, "Edit");
	
	m_pDropButton = (Button*)CreateSuborControl(this, "Button");
	m_pDropButton->SetStyleString("DownArrow");
	
	m_pDropListBox = (ListBox*)CreateSuborControl(this, "ListBox");
	m_pDropListBox->SetVisible(false);
	m_pDropListBox->SetMouseSelect(true);
	
	CalcSuborRect();

	return true;
}

bool ComboBox::Shut()
{
	SAFE_RELEASE(m_pInputEdit);
	SAFE_RELEASE(m_pDropButton);
	SAFE_RELEASE(m_pDropListBox);
	
	return IControl::Shut();
}

void ComboBox::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->AddPropInfo("ButtonWidth", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("DropDownWidth", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("DropDownHeight", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("DroppedDown", IDesignInfo::TYPE_BOOL, true, true);
	di->AddPropInfo("OnlySelect", IDesignInfo::TYPE_BOOL, true, true);

	di->AddSuborInfo("InputEdit", "Edit");
	di->AddSuborInfo("DropButton", "Button");
	di->AddSuborInfo("DropListBox", "ListBox");

	di->AddEventInfo("on_selected");
}

bool ComboBox::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (m_pInputEdit && !m_pInputEdit->GetLoadFinish())
	{
		return false;
	}

	if (m_pDropButton && !m_pDropButton->GetLoadFinish())
	{
		return false;
	}

	if (m_pDropListBox && !m_pDropListBox->GetLoadFinish())
	{
		return false;
	}
	
	return IControl::GetLoadFinish();
}

void ComboBox::Paint(float seconds)
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
	
	m_pInputEdit->Paint(seconds);
	m_pDropButton->Paint(seconds);
	
	// 下拉列表框只能画在最顶层
}

void ComboBox::CalcSuborRect()
{
	m_pInputEdit->SetLeft(0);
	m_pInputEdit->SetTop(0);
	m_pInputEdit->SetWidth(GetWidth() - m_nButtonWidth);
	m_pInputEdit->SetHeight(GetHeight());
	
	m_pDropButton->SetLeft(m_pInputEdit->GetWidth());
	m_pDropButton->SetTop(0);
	m_pDropButton->SetWidth(m_nButtonWidth);
	m_pDropButton->SetHeight(GetHeight() - 1);

	m_pDropListBox->SetLeft(0);

	CalcDropDownTop();
	CalcDropDownWidth();
}

void ComboBox::CalcDropDownTop()
{
	IControl* root = GetRootControl();

	// 最大Y坐标
	int y = InnerGetAbsTop() + GetHeight() + m_pDropListBox->GetHeight();
	
	// 是否超过窗口的高度
	if ((root != NULL) && (y > (root->GetAbsTop() + root->GetHeight())))
	{
		// 显示在上部
		int top = -int(m_pDropListBox->GetHeight());
		
		m_pDropListBox->SetTop(top);
	}
	else
	{
		// 显示在下部
		m_pDropListBox->SetTop(GetHeight());
	}
}

void ComboBox::CalcDropDownWidth()
{
	if (0 == m_nDropDownWidth)
	{
		m_pDropListBox->SetWidth(GetWidth());
	}
	else
	{
		m_pDropListBox->SetWidth(m_nDropDownWidth);
	}
}

PERSISTID ComboBox::GetInputEdit() const
{
	return m_pInputEdit->GetID();
}

PERSISTID ComboBox::GetDropButton() const
{
	return m_pDropButton->GetID();
}

PERSISTID ComboBox::GetDropListBox() const
{
	return m_pDropListBox->GetID();
}

void ComboBox::SetText(const wchar_t* val)
{
	m_pInputEdit->SetText(val);
}

const wchar_t* ComboBox::GetText() const
{
	return m_pInputEdit->GetText();
}

void ComboBox::SetOnlySelect(bool val)
{
	m_pInputEdit->SetReadOnly(val);
}

bool ComboBox::GetOnlySelect() const
{
	return m_pInputEdit->GetReadOnly();
}

void ComboBox::SetDroppedDown(bool val)
{
	if (val == m_bDroppedDown)
	{
		return;
	}
	
	m_bDroppedDown = val;

	m_pDropListBox->SetVisible(val);

	if (val)
	{
		// 在最顶层显示
		GetRootControl()->AddTopLayer(m_pDropListBox->GetID());
	}
	else
	{
		GetRootControl()->RemoveTopLayer(m_pDropListBox->GetID());
	}
}

bool ComboBox::GetDroppedDown() const
{
	return m_bDroppedDown;
}

void ComboBox::SetButtonWidth(int value)
{
	if ((value < 1) || (value > 100))
	{
		return;
	}

	m_nButtonWidth = value;

	CalcSuborRect();
}

int ComboBox::GetButtonWidth() const
{
	return m_nButtonWidth;
}

int ComboBox::GetDropDownWidth() const
{
	return m_nDropDownWidth;
}

void ComboBox::SetDropDownWidth(int val)
{
	m_nDropDownWidth = val;
	
	CalcDropDownWidth();
}

int ComboBox::GetDropDownHeight() const
{
	return m_pDropListBox->GetHeight();
}

void ComboBox::SetDropDownHeight(int val)
{
	m_pDropListBox->SetHeight(val);
}

int ComboBox::OnWidthChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int ComboBox::OnHeightChanged(int old)
{
	CalcSuborRect();
	
	return 1;
}

int ComboBox::OnSuborEvent(IControl* subor, int event, 
						   const IVarList& args)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (event == CGuiConst::EVENT_CLICK)
	{
		if (subor == m_pDropButton)
		{
			SetDroppedDown(!m_bDroppedDown);

			CalcDropDownTop();

			// 输入框获得输入焦点
			GetGui()->SetFocusControl(m_pInputEdit);
		}
		
		return 1;
	}

	if (event == CGuiConst::EVENT_SELECT_CLICK)
	{
		if (subor == m_pDropListBox)
		{
			m_pInputEdit->SetText(m_pDropListBox->GetSelectString());
			// 隐藏下拉列表
			SetDroppedDown(false);
			GuiUtil_RunCallback(this, "on_selected", CVarList());
		}

		return 1;
	}

	if (event == CGuiConst::EVENT_LOST_FOCUS)
	{
		// 失去焦点时隐藏下拉列表
		if (m_bDroppedDown)
		{
			IControl* get_control = GetControl(args.ObjectVal(0));
			
			if (get_control != NULL)
			{
				// 是否有关联的控件
				if ((!IsAncestorOf(get_control)) && (get_control != this))
				{
					SetDroppedDown(false);
				}
			}
		}

		return 1;
	}
	
	return 0;
}

bool ComboBox::RemoveTopLayer(const PERSISTID& id)
{
	SetDroppedDown(false);

	return true;
}

bool ComboBox::RefreshInnerAbsPos()
{
    if (GetRefreshInnerAbsPos())
    {
        if (m_pInputEdit != NULL) m_pInputEdit->SetRefreshInnerAbsPos(true);
        if (m_pDropButton != NULL) m_pDropButton->SetRefreshInnerAbsPos(true);
        if (m_pDropListBox != NULL) m_pDropListBox->SetRefreshInnerAbsPos(true);
        IControl::RefreshInnerAbsPos();
    }

    return true;
}
