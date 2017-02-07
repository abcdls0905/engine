//--------------------------------------------------------------------
// 文件名:		gui_input.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月21日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GUI_INPUT_H
#define _GUI_INPUT_H

#ifndef _WIN32
	//mouse event
	#define WM_MOUSEFIRST                   0x0200
	#define WM_MOUSEMOVE                    0x0200
	#define WM_LBUTTONDOWN                  0x0201
	#define WM_LBUTTONUP                    0x0202
	#define WM_LBUTTONDBLCLK                0x0203
	#define WM_RBUTTONDOWN                  0x0204
	#define WM_RBUTTONUP                    0x0205
	#define WM_RBUTTONDBLCLK                0x0206
	#define WM_MBUTTONDOWN                  0x0207
	#define WM_MBUTTONUP                    0x0208
	#define WM_MBUTTONDBLCLK                0x0209
	#define WM_MOUSELAST                    0x0209
	#define WHEEL_DELTA      120      // Default value for rolling one notch
	#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  // message that will be supported

	//key event
	#define WM_KEYFIRST                     0x0100
	#define WM_KEYDOWN                      0x0100
	#define WM_KEYUP                        0x0101
	#define WM_CHAR                         0x0102
	#define WM_KEYLAST                      0x0108

	//vkey
	#define VK_BACK           0x08
	#define VK_TAB            0x09
	#define VK_RETURN         0x0D
	#define VK_SHIFT          0x10
	#define VK_CONTROL        0x11
	#define VK_MENU           0x12
	#define VK_PAUSE          0x13
	#define VK_CAPITAL        0x14
	#define VK_ESCAPE         0x1B
	#define VK_SPACE          0x20
	#define VK_PRIOR          0x21
	#define VK_NEXT           0x22
	#define VK_END            0x23
	#define VK_HOME           0x24
	#define VK_LEFT           0x25
	#define VK_UP             0x26
	#define VK_RIGHT          0x27
	#define VK_DOWN           0x28
	#define VK_INSERT         0x2D
	#define VK_DELETE         0x2E
	#define VK_HELP           0x2F

	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef float               FLOAT;
	typedef unsigned long		DWORD_PTR;
	typedef long				LONG;
	typedef unsigned long		DWORD;

	#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
	#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
	#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
	#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
	#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
	#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))
	#define CP_ACP                    0           // default to ANSI code page
	#define CP_UTF8                   65001       // UTF-8 translation
#else
	#include <windows.h>
	#ifndef WM_MOUSEWHEEL
		#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  // message that will be supported
	#endif
#endif


// 输入相关

class CGuiInput
{
public:
	enum 
	{
		KEY_UP = VK_UP,
		KEY_DOWN = VK_DOWN,
		KEY_LEFT = VK_LEFT,
		KEY_RIGHT = VK_RIGHT,
		KEY_PRIOR = VK_PRIOR,
		KEY_NEXT = VK_NEXT,
		KEY_HOME = VK_HOME,
		KEY_END = VK_END,
		KEY_RETURN = VK_RETURN,
		KEY_INSERT = VK_INSERT,
		KEY_DELETE = VK_DELETE,
		KEY_BACK = VK_BACK,
		KEY_TAB = VK_TAB,
		KEY_CAPITAL = VK_CAPITAL,
		KEY_SHIFT = VK_SHIFT,
		KEY_CONTROL = VK_CONTROL,
		KEY_MENU = VK_MENU,
		/*
		KEY_F1 = VK_F1,
		KEY_F2 = VK_F2,
		KEY_F3 = VK_F3,
		KEY_F4 = VK_F4,
		KEY_F5 = VK_F5,
		KEY_F6 = VK_F6,
		KEY_F7 = VK_F7,
		KEY_F8 = VK_F8,
		KEY_F9 = VK_F9,
		KEY_F10 = VK_F10,
		KEY_F11 = VK_F11,
		KEY_F12 = VK_F12,*/
	};
};

#endif // _GUI_INPUT_H
