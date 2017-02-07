//--------------------------------------------------------------------
// 文件名:		gui_scroll.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年4月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GUI_SCROLL_H
#define _GUI_SCROLL_H

#include "../public/macros.h"

// 滚动控件函数类

class IControl;
class ScrollBar;
class Label;

class CGuiScroll
{
private:
	enum { DEF_CORNER_SIZE = 17 };
	
public:
	// 创建垂直滚动条
	static ScrollBar* CreateVScrollBar(IControl* parent, int scroll_size);
	// 创建水平滚动条
	static ScrollBar* CreateHScrollBar(IControl* parent, int scroll_size);
	// 创建右下角填充块
	static Label* CreateCornorLabel(IControl* parent);
	
	// 获得滚动条状态
	static unsigned int GetScrollState(ScrollBar* v_scroll, 
		ScrollBar* h_scroll);
	// 测试滚动条是否有效
	static bool TestScrollValid(ScrollBar* scroll);
	
	// 垂直滚动条是否显示
	static bool GetVerticalVisible(ScrollBar* v_scroll, int can_disp, 
		int total, bool show_always = false);
	// 水平滚动条是否显示
	static bool GetHorizonVisible(ScrollBar* h_scroll, int can_disp, 
		int total, bool show_always = false);

	// 计算滚动附属控件的位置和尺寸
	static void CalcScrollRect(IControl* parent, ScrollBar* v_scroll, 
		ScrollBar* h_scroll, Label* corner, unsigned int scroll_state, 
		bool v_scroll_left);
	
	// 更新垂直滚动条
	static bool UpdateVerticalScroll(ScrollBar* v_scroll, int can_disp, 
		int total, int value);
	// 更新水平垂直滚动条
	static bool UpdateHorizonScroll(ScrollBar* h_scroll, int can_disp, 
		int total, int value);
};

#endif // _GUI_SCROLL_H
