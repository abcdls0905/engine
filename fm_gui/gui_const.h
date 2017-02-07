//--------------------------------------------------------------------
// �ļ���:		gui_const.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GUI_CONST_H
#define _GUI_CONST_H

// ���泣��

class CGuiConst
{
public:
	// �¼�����
	enum EVENT_TYPE
	{
		EVENT_CLICK,			// ���
		EVENT_DRAG_MOVE,		// �϶�
		EVENT_DRAG_LEAVE,		// �ϳ�
		EVENT_VALUE_CHANGED,	// ��ֵ�ı�
		EVENT_SELECT_CLICK,		// ���ѡ��
		EVENT_GET_FOCUS,		// ������뽹��
		EVENT_LOST_FOCUS,		// ʧȥ���뽹��
		EVENT_RIGHT_CLICK,		// �һ�
		EVENT_DOUBLE_CLICK,		// ˫��
	};
	
	// ����λ
	enum MOUSE_BUTTON
	{
		MOUSE_LEFT,			// ���
		MOUSE_MIDDLE,		// �м�
		MOUSE_RIGHT,		// �Ҽ�
	};
	
	// ���ֶ���
	enum ALIGN_TYPE
	{
		ALIGN_LEFT,			// ����
		ALIGN_CENTER,		// ����
		ALIGN_RIGHT,		// ����
	};
	
	// ��ֱ����
	enum V_ANCHOR_TYPE
	{
		V_ANCHOR_TOP,		// ����
		V_ANCHOR_CENTER,	// ����
		V_ANCHOR_BOTTOM,	// ����
	};
	
	// ˮƽ����
	enum H_ANCHOR_TYPE
	{
		H_ANCHOR_LEFT,		// ����
		H_ANCHOR_CENTER,	// ����
		H_ANCHOR_RIGHT,		// ����
	};

	// ������ͼ����
	enum DRAW_MODES
	{
		DRAWMODE_TILE,		// ƽ��
		DRAWMODE_CENTER,	// ����
		DRAWMODE_FITWINDOW,	// ��Ӧ���ڴ�С
		DRAWMODE_EXPAND,	// �ָ����������������
		DRAWMODE_EXPANDH,	// ˮƽ�ָ����������������
		DRAWMODE_EXPANDV,	// ��ֱ�ָ����������������
	};
	
	// ��ͷ����
	enum ARROW_STYLES
	{
		ARROW_NONE,
		ARROW_UP,			// �ϼ�ͷ
		ARROW_DOWN,			// �¼�ͷ
		ARROW_LEFT,			// ���ͷ
		ARROW_RIGHT,		// �Ҽ�ͷ
	};

	// ȱʡǰ��ɫ
	enum { DEF_FORE_COLOR = 0xFF000000};
	// ȱʡ����ɫ
	enum { DEF_BACK_COLOR = 0xFFFFFFFF};
	// ȱʡ����ɫ
	enum { DEF_LINE_COLOR = 0xFF000000};
	// ȱʡ������ʾ��ɫ
	enum { DEF_HINT_COLOR = 0xFFFFFFD0 };
	// ��ť��ɫ
	enum { BUTTON_COLOR = 0xFFC0C0C0 };
	// ���ɫ
	enum { GRAY_COLOR = 0xFFE0E0E0 };
	// ��ɫ
	enum { WHITE_COLOR = 0xFFFFFFFF };
	// ��ɫ
	enum { BLACK_COLOR = 0xFF000000 };
	// ѡ����ǰ��ɫ
	enum { SELECT_COLOR = 0xFF000080 };
	// ѡ������ɫ
	enum { SELECT_FORE_COLOR = 0xFFFFFFFF };
	// ѡ�����ɫ
	enum { SELECT_BACK_COLOR = 0xFF000080 };
	// ��ֹ����ɫ
	enum { DISABLE_COLOR = 0xFF808080 };
};

#endif // _GUI_CONST_H
