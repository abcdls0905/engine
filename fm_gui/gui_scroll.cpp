//--------------------------------------------------------------------
// 文件名:		gui_scroll.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年4月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "gui_scroll.h"
#include "i_control.h"
#include "scroll_bar.h"
#include "label.h"

// CGuiScroll

ScrollBar* CGuiScroll::CreateVScrollBar(IControl* parent, int scroll_size)
{
	Assert(parent != NULL);
	
	ScrollBar* scroll_bar = (ScrollBar*)parent->CreateSuborControl(parent, 
		"ScrollBar");

	scroll_bar->SetWidth(scroll_size);
	
	return scroll_bar;
}

ScrollBar* CGuiScroll::CreateHScrollBar(IControl* parent, int scroll_size)
{
	Assert(parent != NULL);
	
	ScrollBar* scroll_bar = (ScrollBar*)parent->CreateSuborControl(parent, 
		"ScrollBar");
	
	scroll_bar->SetOrient("Horizon");
	scroll_bar->SetHeight(scroll_size);
	
	return scroll_bar;
}

Label* CGuiScroll::CreateCornorLabel(IControl* parent)
{
	Assert(parent != NULL);
	
	Label* label = (Label*)parent->CreateSuborControl(parent, "Label");

	label->SetSolid(true);
	label->SetNoFrame(false);
	
	return label;
}

unsigned int CGuiScroll::GetScrollState(ScrollBar* v_scroll, ScrollBar* h_scroll)
{
	unsigned int res = 0;
	
	if (v_scroll && v_scroll->GetVisible())
	{
		res |= 0x1;
	}
	
	if (h_scroll && h_scroll->GetVisible())
	{
		res |= 0x2;
	}
	
	return res;
}

bool CGuiScroll::TestScrollValid(ScrollBar* scroll)
{
	return (scroll != NULL) && (scroll->GetVisible()) 
		&& (scroll->GetEnabled());
}

bool CGuiScroll::GetVerticalVisible(ScrollBar* v_scroll, int can_disp, 
									int total, bool show_always)
{
	if (NULL == v_scroll)
	{
		return false;
	}
	
	if (show_always)
	{
		return true;
	}
	
	if (can_disp < total)
	{
		return true;
	}
	
	return false;
}

bool CGuiScroll::GetHorizonVisible(ScrollBar* h_scroll, int can_disp, 
								   int total, bool show_always)
{
	if (NULL == h_scroll)
	{
		return false;
	}
	
	if (show_always)
	{
		return true;
	}
	
	if (can_disp < total)
	{
		return true;
	}
	
	return false;
}

void CGuiScroll::CalcScrollRect(IControl* parent, ScrollBar* v_scroll, 
							    ScrollBar* h_scroll, Label* corner,
							    unsigned int scroll_state, bool v_scroll_left)
{
	if (corner)
	{
		corner->SetVisible(scroll_state == 0x3);
	}
	
	if (v_scroll && v_scroll->GetVisible())
	{
		int h = parent->GetHeight();
		
		if (h_scroll && h_scroll->GetVisible())
		{
			h -= h_scroll->GetHeight() - 1;
		}
		
		v_scroll->SetHeight(h);
		v_scroll->SetTop(0);
		
		if (v_scroll_left)
		{
			v_scroll->SetLeft(0);
		}
		else
		{
			v_scroll->SetLeft(parent->GetWidth() - v_scroll->GetWidth());
		}
	}
	
	if (h_scroll && h_scroll->GetVisible())
	{
		int w = parent->GetWidth();
		
		if (v_scroll && v_scroll->GetVisible())
		{
			w -= v_scroll->GetWidth() - 1;
		}
		
		h_scroll->SetWidth(w);
		h_scroll->SetTop(parent->GetHeight() - h_scroll->GetHeight());

		if (v_scroll_left && v_scroll && v_scroll->GetVisible())
		{
			h_scroll->SetLeft(v_scroll->GetWidth());
		}
		else
		{
			h_scroll->SetLeft(0);
		}
	}
	
	if (corner && corner->GetVisible())
	{
		corner->SetWidth(v_scroll->GetWidth());
		corner->SetHeight(h_scroll->GetHeight());
		corner->SetTop(parent->GetHeight() - corner->GetHeight());
		
		if (v_scroll_left)
		{
			corner->SetLeft(0);
		}
		else
		{
			corner->SetLeft(parent->GetWidth() - corner->GetWidth());
		}
	}
}

bool CGuiScroll::UpdateVerticalScroll(ScrollBar* v_scroll, int can_disp,
									  int total, int value)
{
	if (NULL == v_scroll)
	{
		return false;
	}
	
	if (!v_scroll->GetVisible())
	{
		return false;
	}
	
	int maximum = total - can_disp;

	if (maximum < 0)
	{
		maximum = 0;
	}
	
	v_scroll->SetEnabled(can_disp < total);
	v_scroll->SetMinimum(0);
	v_scroll->SetMaximum(maximum);
	
	if (total > 0)
	{
		v_scroll->SetLargeChange(can_disp);
	}
	
	v_scroll->SetCurValue(value);
	
	return true;
}

bool CGuiScroll::UpdateHorizonScroll(ScrollBar* h_scroll, int can_disp,
									 int total, int value)
{
	if (NULL == h_scroll)
	{
		return false;
	}
	
	if (!h_scroll->GetVisible())
	{
		return false;
	}
	
	int maximum = total - can_disp;
	
	if (maximum < 0)
	{
		maximum = 0;
	}
	
	h_scroll->SetEnabled(can_disp < total);
	h_scroll->SetMinimum(0);
	h_scroll->SetMaximum(maximum);
	
	if (total > 0)
	{
		h_scroll->SetLargeChange(can_disp);
	}
	
	h_scroll->SetCurValue(value);
	
	return true;
}

