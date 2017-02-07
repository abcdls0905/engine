//--------------------------------------------------------------------
// 文件名:		gui_utils.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "gui_utils.h"
#include "gui_const.h"
#include "gui_input.h"
#include "i_control.h"
#include "../visual/i_gui_hook.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../visual/i_painter.h"
#include <stdlib.h>
#include <float.h>
#include "../visual/i_event_manager.h"
// CGuiUtils

// 将颜色值转换为字符串
result_string GuiUtil_IntToColor(unsigned int val)
{
	char buf[64];
	
	SafeSprintf(buf, sizeof(buf), "%d,%d,%d,%d", (val >> 24) & 0xFF, 
		(val >> 16) & 0xFF, (val >> 8) & 0xFF, (val >> 0) & 0xFF);
	
	return result_string(buf);
}

unsigned int GuiUtil_ColorToInt(const char* val)
{
	Assert(val != NULL);

	char buf[128];

	CopyString(buf, sizeof(buf), val);

	char* s1 = strchr(buf, ',');

	if (NULL == s1)
	{
		return 0xFFFFFFFF;
	}

	char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return 0xFFFFFFFF;
	}

	char* s3 = strchr(s2 + 1, ',');

	if (NULL == s3)
	{
		return 0xFFFFFFFF;
	}

	*s1 = 0;
	*s2 = 0;
	*s3 = 0;

	unsigned int res = 0;

	res |= (atoi(buf) & 0xFF) << 24;
	res |= (atoi(s1 + 1) & 0xFF) << 16;
	res |= (atoi(s2 + 1) & 0xFF) << 8;
	res |= (atoi(s3 + 1) & 0xFF);

	return res;
}

int GuiUtil_WideStrToInt(const wchar_t* val)
{
	Assert(val != NULL);

	return wcstol(val, NULL, 10);
}

core_wstring GuiUtil_IntToWideStr(int val)
{
	wchar_t buf[32];

	SafeSwprintf(buf, sizeof(buf), L"%d", val);

	return core_wstring(buf);
}

bool GuiUtil_StringToIntList(const char* val, IVarList& result)
{
	Assert(val != NULL);

	result.Clear();

	const char* p1 = val;
	const char* p2 = strchr(p1, ',');

	while (*p1)
	{
		result.AddInt(atoi(p1));

		if (NULL == p2)
		{
			break;
		}

		p1 = p2 + 1;
		p2 = strchr(p1, ',');
	}

	return true;
}

/*
bool GuiUtil_ConvertImage(const char* name, core_string& tex, int& left, 
	int& top, int& right, int& bottom)
{
	const size_t size = strlen(name) + 1;
	char buf[256];
	
	if (size > sizeof(buf))
	{
		return false;
	}
	
	memcpy(buf, name, size);
	
	char* s1 = strchr(buf, ',');
	
	if (NULL == s1)
	{
		return false;
	}
	
	char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return false;
	}

	char* s3 = strchr(s2 + 1, ',');

	if (NULL == s3)
	{
		return false;
	}

	char* s4 = strchr(s3 + 1, ',');

	if (NULL == s4)
	{
		return false;
	}

	*s1 = 0;
	*s2 = 0;
	*s3 = 0;
	*s4 = 0;

	tex = buf;
	left = atoi(s1 + 1);
	top = atoi(s2 + 1);
	right = atoi(s3 + 1);
	bottom = atoi(s4 + 1);
	
	return true;
}
*/

int GuiUtil_AlignToInt(const char* val)
{
	if (stricmp(val, "center") == 0)
	{
		return CGuiConst::ALIGN_CENTER;
	}

	if (stricmp(val, "left") == 0)
	{
		return CGuiConst::ALIGN_LEFT;
	}
	
	if (stricmp(val, "right") == 0)
	{
		return CGuiConst::ALIGN_RIGHT;
	}
	
	return 0;
}

const char* GuiUtil_IntToAlign(int val)
{
	if (val == CGuiConst::ALIGN_CENTER)
	{
		return "center";
	}

	if (val == CGuiConst::ALIGN_LEFT)
	{
		return "left";
	}

	if (val == CGuiConst::ALIGN_RIGHT)
	{
		return "right";
	}
	
	return "";
}

// 矩形范围转换成字符串
result_string GuiUtil_RectToString(const rect_t& r)
{
	char buf[128];

	SafeSprintf(buf, sizeof(buf), "%d,%d,%d,%d", r.left, r.top, r.right, 
		r.bottom);

	return result_string(buf);
}

// 字符串转换成矩形范围
bool GuiUtil_StringToRect(const char* val, rect_t& r)
{
	Assert(val != NULL);

	char buf[128];

	CopyString(buf, sizeof(buf), val);

	char* s1 = strchr(buf, ',');

	if (NULL == s1)
	{
		return false;
	}

	char* s2 = strchr(s1 + 1, ',');

	if (NULL == s2)
	{
		return false;
	}

	char* s3 = strchr(s2 + 1, ',');

	if (NULL == s3)
	{
		return false;
	}

	*s1 = 0;
	*s2 = 0;
	*s3 = 0;

	r.left = atoi(buf);
	r.top = atoi(s1 + 1);
	r.right = atoi(s2 + 1);
	r.bottom = atoi(s3 + 1);

	return true;
}

bool GuiUtil_IsLeadByte(unsigned int val) 
{
#ifdef _WIN32
	return (IsDBCSLeadByte(val) == TRUE);
#else
    return true;
#endif
}

bool GuiUtil_IsLeadByteEx(unsigned int code_page, unsigned int val) 
{
#ifdef _WIN32
	return (IsDBCSLeadByteEx(code_page, val) == TRUE);
#else
    return true;
#endif
}


bool GuiUtil_ShiftPressed()
{
#ifdef WIN32
	return (GetAsyncKeyState(CGuiInput::KEY_SHIFT) < 0);
#endif
	return false;
}

bool GuiUtil_ControlPressed()
{
#ifdef WIN32
	return (GetAsyncKeyState(CGuiInput::KEY_CONTROL) < 0);
#endif
	return false;
}

bool GuiUtil_MenuPressed()
{
#ifdef WIN32
	return (GetAsyncKeyState(CGuiInput::KEY_MENU) < 0);
#endif
	return false;
}

bool GuiUtil_CopyToClipboard(const wchar_t*  ws)
{
	Assert(ws != NULL);

#ifdef WIN32
	if (!OpenClipboard(NULL))
	{
		return false;
	}

	EmptyClipboard();
	
	const size_t size = (wcslen(ws) + 1) * sizeof(wchar_t);
	
	//HANDLE hAscii = GlobalAlloc(GHND, size);
	HANDLE hUnicode = GlobalAlloc(GHND, size);
	
	//char* ascii = (char*)GlobalLock(hAscii);
	
	wchar_t* unicode = (wchar_t*)GlobalLock(hUnicode);
	
	//ToString(ws, ascii, size);
	
	CopyWideStr(unicode, size, ws);
	
	//GlobalUnlock(hAscii);
	GlobalUnlock(hUnicode);
	
	//SetClipboardData(CF_TEXT, hAscii);
	SetClipboardData(CF_UNICODETEXT, hUnicode);
	
	CloseClipboard();
	
	//GlobalFree(hAscii);
	GlobalFree(hUnicode);
	
	return true;
#endif

	return false;
}

bool GuiUtil_PasteFromClipboard(result_wstring& ws)
{
#ifdef WIN32
	if (!OpenClipboard(NULL))
	{
		return false;
	}
	
	//HANDLE handle = GetClipboardData(CF_TEXT);
	
	//if (NULL == handle)
	//{
		HANDLE handle = GetClipboardData(CF_UNICODETEXT);
		
		if (NULL == handle)
		{
			CloseClipboard();
			return false;
		}
		
		ws = (const wchar_t*)GlobalLock(handle);
	
	GlobalUnlock(handle);
	CloseClipboard();
	
	return true;
#endif

	return false;
}

bool GuiUtil_PointInRect(int x, int y, int x1, int y1, int x2, int y2)
{
	return (x >= x1) && (x < x2) && (y >= y1) && (y < y2);
}

bool GuiUtil_PointInRound(int x, int y, int x1, int y1, int x2, int y2)
{
	// 先用矩形作判断
	if (!GuiUtil_PointInRect(x, y, x1, y1, x2, y2))
	{
		return false;
	}
	
	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;

	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	
	float dx = float(x) - cx;
	float dy = float(y) - cy;
	
	// 椭圆公式
	float value = (dx * dx) / (rx * rx) + (dy * dy) / (ry * ry);
	
	return (value <= 1.0F);
}

int GuiUtil_NormalizeLeft(IControl* pControl, int value)
{
	Assert(pControl != NULL);
	
	if (value < 0)
	{
		return 0;
	}
	
	if (value >= pControl->GetWidth())
	{
		return pControl->GetWidth() - 1;
	}
	
	return value;
}

int GuiUtil_NormalizeTop(IControl* pControl, int value)
{
	Assert(pControl != NULL);
	
	if (value < 0)
	{
		return 0;
	}
	
	if (value >= pControl->GetHeight())
	{
		return pControl->GetHeight() - 1;
	}
	
	return value;
}

int GuiUtil_NormalizeWidth(IControl* pControl, int value)
{
	Assert(pControl != NULL);
	
	if (value < 1)
	{
		return 1;
	}
	
	if (value > pControl->GetWidth())
	{
		return pControl->GetWidth();
	}
	
	return value;
}

int GuiUtil_NormalizeHeight(IControl* pControl, int value)
{
	Assert(pControl != NULL);
	
	if (value < 1)
	{
		return 1;
	}
	
	if (value > pControl->GetHeight())
	{
		return pControl->GetHeight();
	}
	
	return value;
}

const char* GuiUtil_GetValidFontName(IControl* pControl)
{
	Assert(pControl != NULL);

	const char* name = pControl->GetFontName();
	
	if (StringEmpty(name))
	{
		IControl* pParent = pControl->GetParentControl();

		if (pParent != NULL)
		{
			return GuiUtil_GetValidFontName(pParent);
		}
		else
		{
			return "Default";
		}
	}
	
	return name;
}

//控件回调
int GuiUtil_RunCallback(IControl* pControl, const char* event, 
	const IVarList& args)
{
	if(pControl)
	{
		CVarList _args;
		_args << pControl->GetID();
		_args.Append(args, 0, args.GetCount());
		return event_send(pControl->GetID(), event, _args);
	}
	else
	{
		return event_send(pControl->GetID(), event, args);
	}
}


bool GuiUtil_PropertyEqual(const IVar& v1, const IVar& v2)
{
	if (v1.GetType() != v2.GetType())
	{
		return false;
	}

	switch (v1.GetType())
	{
	case VTYPE_BOOL:
		return (v1.BoolVal() == v2.BoolVal());
	case VTYPE_INT:
		return (v1.IntVal() == v2.IntVal());
	case VTYPE_INT64:
		return (v1.Int64Val() == v2.Int64Val());
	case VTYPE_FLOAT:
		return FloatEqual(v1.FloatVal(), v2.FloatVal());
	case VTYPE_DOUBLE:
		return DoubleEqual(v1.DoubleVal(), v2.DoubleVal());
	case VTYPE_STRING:
		return (strcmp(v1.StringVal(), v2.StringVal()) == 0);
	case VTYPE_WIDESTR:
		return (wcscmp(v1.WideStrVal(), v2.WideStrVal()) == 0);
	default:
		break;
	}

	return false;
}

result_string GuiUtil_Utf16ToAnsi(const wchar_t* str)
{
	Assert(str != NULL);
	
	size_t size = GetToStringLen(str);
	TAutoMem<char, 256, TCoreAlloc> buf(size);
	char* p = buf.GetBuffer();

	ToString(str, p, size);

	return result_string(p);
}

result_wstring GuiUtil_AnsiToUtf16(const char* str)
{
	Assert(str != NULL);
	
	size_t size = GetToWideStrLen(str);
	TAutoMem<wchar_t, 256, TCoreAlloc> buf(size);
	wchar_t* p = buf.GetBuffer();
	
	ToWideStr(str, p, size * sizeof(wchar_t));
	
	return result_wstring(p);
}

result_wstring GuiUtil_Utf8ToUtf16(const char* str)
{
	// 获得长度
	int size = Port_GetUTF8ToWideStrLen(str);
	TAutoMem<wchar_t, 256, TCoreAlloc> auto_buf(size);
	wchar_t* buf = auto_buf.GetBuffer();
	const wchar_t* wstr = Port_UTF8ToWideStr(str, buf, size*sizeof(wchar_t));
	return result_wstring(wstr);
}

result_string GuiUtil_Utf16ToUtf8(const wchar_t* str)
{
	// 获得长度
	int size = Port_GetWideStrToUTF8Len(str);
	TAutoMem<char, 256, TCoreAlloc> auto_buf(size);
	char* buf = auto_buf.GetBuffer();
	const char* utf8 = Port_WideStrToUTF8(str, buf, size*sizeof(char));
	return result_string(utf8);
}

bool GuiUtil_HasUtf8Char(const char* str)
{
	for (const char* s = str; *s; ++s)
	{
        // 这边原本有BUG *s > 127 不对，*s 为utf-8时将会<0
		if ((*s & 0x80) == 0x80)
		{
			return true;
		}
	}

	return false;
}

bool GuiUtil_PaintArrow(IPainter* painter, int style, int x1, int y1, 
	int width, int height, unsigned int color)
{
	int size;
	
	if (width < height)
	{
		size = width / 3;
	}
	else
	{
		size = height / 3;
	}
	
	if (0 == size)
	{
		return false;
	}
	
	switch (style)
	{
	case CGuiConst::ARROW_UP:
		{
			int h = size;
			int w = h * 2 - 1;
			int x4 = x1 + (width - 1) / 2;
			int y4 = y1 + (height - h) / 2 - 1;
			int x3 = x4 - w / 2 - 1;
			int y3 = y4 + h + 1;
			int x5 = x4 + w / 2 + 1;
			int y5 = y3;
			
			return painter->DrawTriangle(x3, y3, x4, y4, x5, y5, color);
		}
		break;
	case CGuiConst::ARROW_DOWN:
		{
			int h = size;
			int w = h * 2 - 1;
			int x5 = x1 + (width - 1) / 2;
			int y3 = y1 + (height - h) / 2;
			int x3 = x5 - w / 2;
			int y5 = y3 + h;
			int x4 = x5 + w / 2 + 1;
			int y4 = y3;
			
			return painter->DrawTriangle(x3, y3, x4, y4, x5, y5, color);
		}
		break;
	case CGuiConst::ARROW_LEFT:
		{
			int w = size;
			int h = w * 2 - 1;
			int x3 = x1 + (width - w) / 2;
			int y3 = y1 + (height - 1) / 2;
			int x4 = x3 + w;
			int y4 = y3 - h / 2 - 1;
			int x5 = x4;
			int y5 = y3 + h / 2 + 1;
			
			return painter->DrawTriangle(x3, y3, x4, y4, x5, y5, color);
		}
		break;
	case CGuiConst::ARROW_RIGHT:
		{
			int w = size;
			int h = w * 2 - 1;
			int x3 = x1 + (width - w) / 2;
			int y4 = y1 + (height - 1) / 2;
			int x4 = x3 + w;
			int y3 = y4 - h / 2 - 1;
			int x5 = x3;
			int y5 = y4 + h / 2 + 1;
			
			return painter->DrawTriangle(x3, y3, x4, y4, x5, y5, color);
		}
		break;
	default:
		return false;
	}

	return true;
}

bool GuiUtil_PaintButtonFace(IPainter* painter, int x1, int y1, int x2, 
	int y2)
{
	//painter->FillRect(x1, y1, x2, y2, CGuiConst::WHITE_COLOR);
	//painter->FillRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, 
	//	CGuiConst::BUTTON_COLOR);
	// 下阴影
	//painter->DrawLine(x1, y2 - 1, x2, y2 - 1, CGuiConst::BLACK_COLOR);
	//painter->DrawLine(x1 + 1, y2 - 2, x2, y2 - 2, CGuiConst::BLACK_COLOR);
	// 右阴影
	//painter->DrawLine(x2 - 1, y1, x2 - 1, y2, CGuiConst::BLACK_COLOR);
	//painter->DrawLine(x2 - 2, y1 + 1, x2 - 2, y2, CGuiConst::BLACK_COLOR);

	painter->FillRect(x1, y1, x2, y2, CGuiConst::WHITE_COLOR);
	painter->FillRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, 
		CGuiConst::BUTTON_COLOR);
	// 下阴影
	painter->DrawLine(x1, y2, x2, y2, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x1 + 1, y2 - 1, x2, y2 - 1, CGuiConst::BLACK_COLOR);
	// 右阴影
	painter->DrawLine(x2, y1, x2, y2, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x2 - 1, y1 + 1, x2 - 1, y2, CGuiConst::BLACK_COLOR);

	return true;
}

bool GuiUtil_PaintButtonFaceColor(IPainter* painter, unsigned int icolor, 
	unsigned int icolorwhite, int x1, int y1, int x2, int y2)
{
	//painter->FillRect(x1, y1, x2, y2, icolorwhite);
	//painter->FillRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, icolor);
	// 下阴影
	//painter->DrawLine(x1, y2 - 1, x2, y2 - 1, CGuiConst::BLACK_COLOR);
	//painter->DrawLine(x1 + 1, y2 - 2, x2, y2 - 2, CGuiConst::BLACK_COLOR);
	// 右阴影
	//painter->DrawLine(x2 - 1, y1, x2 - 1, y2, CGuiConst::BLACK_COLOR);
	//painter->DrawLine(x2 - 2, y1 + 1, x2 - 2, y2, CGuiConst::BLACK_COLOR);

	painter->FillRect(x1, y1, x2, y2, icolorwhite);
	painter->FillRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, icolor);
	// 下阴影
	painter->DrawLine(x1, y2, x2, y2, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x1 + 1, y2 - 1, x2, y2 - 1, CGuiConst::BLACK_COLOR);
	// 右阴影
	painter->DrawLine(x2, y1, x2, y2, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x2 - 1, y1 + 1, x2 - 1, y2, CGuiConst::BLACK_COLOR);

	return true;
}

bool GuiUtil_PaintButtonFaceDiv(IPainter* painter, int x1, int y1, int x2, 
								int y2 , int iDivIn , int iDivOut , int iDivEnd)
{
	painter->FillRect(x1, y1, x2, y2, CGuiConst::WHITE_COLOR);
	painter->FillRect(x1 + 2, y1 + 2, x1 + 2 + iDivIn - 2, y2 - 2, 
		CGuiConst::BLACK_COLOR / 2);
	painter->FillRect(x1 + 2 + iDivIn - 2, y1 + 2, iDivOut - 2, y2 - 2, 
		CGuiConst::BUTTON_COLOR);
	painter->FillRect(iDivOut - 2, y1 + 2, iDivEnd - 2 , y2 - 2, 
		CGuiConst::BUTTON_COLOR / 2);
	painter->FillRect(iDivEnd - 2, y1 + 2, x2 - 2 , y2 - 2, 
		CGuiConst::BUTTON_COLOR);

	// 下阴影
	painter->DrawLine(x1, y2 - 1, x2, y2 - 1, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x1 + 1, y2 - 2, x2, y2 - 2, CGuiConst::BLACK_COLOR);

	// 右阴影
	painter->DrawLine(x2 - 1, y1, x2 - 1, y2, CGuiConst::BLACK_COLOR);
	painter->DrawLine(x2 - 2, y1 + 1, x2 - 2, y2, CGuiConst::BLACK_COLOR);

	return true;
}

const wchar_t* GuiUtil_GetCanShowText(IPainter* painter, const wchar_t* text, 
	int width, const wchar_t* suffix, wchar_t* buffer, size_t size)
{
	Assert(text != NULL);
	Assert(suffix != NULL);

	int w = painter->GetTextWidth(text);

	if (w <= width)
	{
		return text;
	}

	size_t suffix_len = wcslen(suffix);
	
	CopyWideStr(buffer, size - suffix_len * sizeof(wchar_t), text);

	size_t len = wcslen(buffer);
	int suffix_width = painter->GetTextWidth(suffix);

	while (w > width)
	{
		if (0 == len)
		{
			return buffer;
		}
		
		buffer[len - 1] = 0;
		len--;
		w = painter->GetTextWidth(buffer) + suffix_width;
	}

	memcpy(buffer + len, suffix, (suffix_len + 1) * sizeof(wchar_t));

	return buffer;
}

