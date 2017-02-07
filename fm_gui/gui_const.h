//--------------------------------------------------------------------
// 文件名:		gui_const.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GUI_CONST_H
#define _GUI_CONST_H

// 界面常量

class CGuiConst
{
public:
	// 事件类型
	enum EVENT_TYPE
	{
		EVENT_CLICK,			// 点击
		EVENT_DRAG_MOVE,		// 拖动
		EVENT_DRAG_LEAVE,		// 拖出
		EVENT_VALUE_CHANGED,	// 数值改变
		EVENT_SELECT_CLICK,		// 点击选项
		EVENT_GET_FOCUS,		// 获得输入焦点
		EVENT_LOST_FOCUS,		// 失去输入焦点
		EVENT_RIGHT_CLICK,		// 右击
		EVENT_DOUBLE_CLICK,		// 双击
	};
	
	// 鼠标键位
	enum MOUSE_BUTTON
	{
		MOUSE_LEFT,			// 左键
		MOUSE_MIDDLE,		// 中键
		MOUSE_RIGHT,		// 右键
	};
	
	// 文字对齐
	enum ALIGN_TYPE
	{
		ALIGN_LEFT,			// 居左
		ALIGN_CENTER,		// 居中
		ALIGN_RIGHT,		// 居右
	};
	
	// 垂直对齐
	enum V_ANCHOR_TYPE
	{
		V_ANCHOR_TOP,		// 靠上
		V_ANCHOR_CENTER,	// 居中
		V_ANCHOR_BOTTOM,	// 靠下
	};
	
	// 水平对齐
	enum H_ANCHOR_TYPE
	{
		H_ANCHOR_LEFT,		// 居左
		H_ANCHOR_CENTER,	// 居中
		H_ANCHOR_RIGHT,		// 居右
	};

	// 背景贴图画法
	enum DRAW_MODES
	{
		DRAWMODE_TILE,		// 平铺
		DRAWMODE_CENTER,	// 居中
		DRAWMODE_FITWINDOW,	// 适应窗口大小
		DRAWMODE_EXPAND,	// 分割并复制扩充填满窗口
		DRAWMODE_EXPANDH,	// 水平分割并复制扩充填满窗口
		DRAWMODE_EXPANDV,	// 垂直分割并复制扩充填满窗口
	};
	
	// 箭头类型
	enum ARROW_STYLES
	{
		ARROW_NONE,
		ARROW_UP,			// 上箭头
		ARROW_DOWN,			// 下箭头
		ARROW_LEFT,			// 左箭头
		ARROW_RIGHT,		// 右箭头
	};

	// 缺省前景色
	enum { DEF_FORE_COLOR = 0xFF000000};
	// 缺省背景色
	enum { DEF_BACK_COLOR = 0xFFFFFFFF};
	// 缺省线颜色
	enum { DEF_LINE_COLOR = 0xFF000000};
	// 缺省浮动提示颜色
	enum { DEF_HINT_COLOR = 0xFFFFFFD0 };
	// 按钮颜色
	enum { BUTTON_COLOR = 0xFFC0C0C0 };
	// 深灰色
	enum { GRAY_COLOR = 0xFFE0E0E0 };
	// 白色
	enum { WHITE_COLOR = 0xFFFFFFFF };
	// 黑色
	enum { BLACK_COLOR = 0xFF000000 };
	// 选择项前景色
	enum { SELECT_COLOR = 0xFF000080 };
	// 选择项颜色
	enum { SELECT_FORE_COLOR = 0xFFFFFFFF };
	// 选择项背景色
	enum { SELECT_BACK_COLOR = 0xFF000080 };
	// 禁止项颜色
	enum { DISABLE_COLOR = 0xFF808080 };
};

#endif // _GUI_CONST_H
