//--------------------------------------------------------------------
// 文件名:		group_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "group_box.h"

/// \file group_box.cpp
/// \brief 分组控件

/// entity: GroupBox
/// \brief 分组控件实体
DECLARE_ENTITY(GroupBox, 0, IContainer);

/// property: bool Fixed
/// \brief 是否固定位置
DECLARE_PROPERTY(bool, GroupBox, Fixed, GetFixed, SetFixed);
/// property: bool Transparent
/// \brief 是否透明
DECLARE_PROPERTY(bool, GroupBox, Transparent, GetTransparent, SetTransparent);
/// property: bool ClipChildren
/// \brief 是否要剪裁子控件
DECLARE_PROPERTY(bool, GroupBox, ClipChildren, GetClipChildren, 
	SetClipChildren);

// GroupBox

GroupBox::GroupBox()
{
	m_bFixed = true;
	m_nClickX = 0;
	m_nClickY = 0;
	m_bTransparent = false;
	m_bClipChildren = true;
	SetDesignSize(100, 80);
	SetCanSelect(false);
}

GroupBox::~GroupBox()
{
}

bool GroupBox::Init(const IVarList& args)
{
	if (!IContainer::Init(args))
	{
		return false;
	}
	
	return true;
}

bool GroupBox::Shut()
{
	return IContainer::Shut();
}

void GroupBox::SetTransparent(bool value)
{
    m_bTransparent = value;
}

bool GroupBox::GetTransparent() const
{
    return m_bTransparent;
}

void GroupBox::PrepareDesignInfo(IDesignInfo* di)
{
	IContainer::PrepareDesignInfo(di);
	CVar v(VTYPE_BOOL, true);
	di->AddPropInfo("Fixed", IDesignInfo::TYPE_BOOL, true, true, &v);
    v = CVar(VTYPE_BOOL, false);
    di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, &v);
    v = CVar(VTYPE_BOOL, true);
    di->AddPropInfo("Visible", IDesignInfo::TYPE_BOOL, true, true, &v);
    v = CVar(VTYPE_BOOL, true);
	di->AddPropInfo("ClipChildren", IDesignInfo::TYPE_BOOL, true, true, &v);

}

void GroupBox::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}

    RefreshInnerAbsPos();

    int old_alpha = GetPainter()->GetBlendAlpha();

    // 如果父界面透明了，子界面也应透明
    if (old_alpha < GetAlpha())
    {
        GetPainter()->SetBlendAlpha(old_alpha);
    }
    else
    {
        GetPainter()->SetBlendAlpha(GetAlpha());
    }

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
			GetGui()->SetFont(GuiUtil_GetValidFontName(this), GetShadowColor());
			
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
	
	if (m_bClipChildren)
	{
		if (GetPainter()->SetViewport(x1, y1, x2, y2))
		{
			IContainer::Paint(seconds);
			GetPainter()->ResetViewport();
		}
	}
	else
	{
		// 不剪裁子控件
		IContainer::Paint(seconds);
	}

    GetPainter()->SetBlendAlpha(old_alpha);

}

void GroupBox::SetFixed(bool value) 
{ 
	m_bFixed = value; 

	SetCanSelect(!value);
}

bool GroupBox::GetFixed() const 
{ 
	return m_bFixed; 
}

int GroupBox::OnLButtonDown(int x, int y)
{
	if (!GetEnabled())
	{
		return 0;
	}
	
	if (m_bFixed)
	{
		return 0;
	}
	
	m_nClickX = x - InnerGetAbsLeft();
	m_nClickY = y - InnerGetAbsTop();
	
	Assert((m_nClickX >= 0) && (m_nClickX < GetWidth()));
	Assert((m_nClickY >= 0) && (m_nClickY < GetHeight()));
	
	return 1;
}

int GroupBox::OnDragMove(int x, int y)
{
	if (m_bFixed)
	{
		return 0;
	}
	
	int new_x = x - InnerGetAbsLeft();
	int new_y = y - InnerGetAbsTop();
	int delta_x = new_x - m_nClickX; 
	int delta_y = new_y - m_nClickY;
	int new_left = InnerGetAbsLeft() + delta_x;
	int new_top = InnerGetAbsTop() + delta_y;
	IControl* parent = GetParentControl();
	
	if (parent != NULL)
	{
		int parent_left = parent->GetAbsLeft();
		int parent_top = parent->GetAbsTop();
		int parent_right = parent_left + parent->GetWidth();
		int parent_bottom = parent_top + parent->GetHeight();
		
		if (new_left < parent_left)
		{
			new_left = parent_left;
		}

		if (new_left > (parent_right - GetWidth()))
		{
			new_left = parent_right - GetWidth();
		}
		
		if (new_top < parent_top)
		{
			new_top = parent_top;
		}
		
		if (new_top > (parent_bottom - GetHeight()))
		{
			new_top = parent_bottom - GetHeight();
		}
	}
	
	SetAbsLeft(new_left);
	SetAbsTop(new_top);
	
	return 1;
}

IControl* GroupBox::OnGetInWhat(int x, int y)
{
    IControl* pControl = IContainer::OnGetInWhat(x, y);

    if (NULL == pControl)
    {
        return NULL;
    }

    if ((pControl == this) && m_bTransparent)
    {
        // 透明状态时
        if (!GetDesignMode())
        {
            return NULL;
        }
    }

    return pControl;
}
