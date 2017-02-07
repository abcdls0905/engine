//--------------------------------------------------------------------
// �ļ���:		gui_scroll.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��4��28��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GUI_SCROLL_H
#define _GUI_SCROLL_H

#include "../public/macros.h"

// �����ؼ�������

class IControl;
class ScrollBar;
class Label;

class CGuiScroll
{
private:
	enum { DEF_CORNER_SIZE = 17 };
	
public:
	// ������ֱ������
	static ScrollBar* CreateVScrollBar(IControl* parent, int scroll_size);
	// ����ˮƽ������
	static ScrollBar* CreateHScrollBar(IControl* parent, int scroll_size);
	// �������½�����
	static Label* CreateCornorLabel(IControl* parent);
	
	// ��ù�����״̬
	static unsigned int GetScrollState(ScrollBar* v_scroll, 
		ScrollBar* h_scroll);
	// ���Թ������Ƿ���Ч
	static bool TestScrollValid(ScrollBar* scroll);
	
	// ��ֱ�������Ƿ���ʾ
	static bool GetVerticalVisible(ScrollBar* v_scroll, int can_disp, 
		int total, bool show_always = false);
	// ˮƽ�������Ƿ���ʾ
	static bool GetHorizonVisible(ScrollBar* h_scroll, int can_disp, 
		int total, bool show_always = false);

	// ������������ؼ���λ�úͳߴ�
	static void CalcScrollRect(IControl* parent, ScrollBar* v_scroll, 
		ScrollBar* h_scroll, Label* corner, unsigned int scroll_state, 
		bool v_scroll_left);
	
	// ���´�ֱ������
	static bool UpdateVerticalScroll(ScrollBar* v_scroll, int can_disp, 
		int total, int value);
	// ����ˮƽ��ֱ������
	static bool UpdateHorizonScroll(ScrollBar* h_scroll, int can_disp, 
		int total, int value);
};

#endif // _GUI_SCROLL_H
