//--------------------------------------------------------------------
// 文件名:		gui_utils.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

#include "../public/macros.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/core_type.h"

struct point_t
{
	int x;
	int y;
};

struct rect_t
{
	int left;
	int top;
	int right;
	int bottom;
};

// 工具函数类

class Gui;
class IEntity;
class IControl;
class IPainter;

// 颜色值转换成字符串
result_string GuiUtil_IntToColor(unsigned int val);
// 字符串转换成颜色值
unsigned int GuiUtil_ColorToInt(const char* val);

// 宽字符串转换成整数
int GuiUtil_WideStrToInt(const wchar_t* val);
// 整数宽转换成字符串
core_wstring GuiUtil_IntToWideStr(int val);

// 将字符串分解为整数列表
bool GuiUtil_StringToIntList(const char* val, IVarList& result);

// 字符串转换成图片参数
//bool GuiUtil_ConvertImage(const char* name, core_string& tex,
//	int& left, int& top, int& right, int& bottom);

// 字符串转换成对齐值
int GuiUtil_AlignToInt(const char* val);
// 对齐值转换成字符串
const char* GuiUtil_IntToAlign(int val);

// 矩形范围转换成字符串
result_string GuiUtil_RectToString(const rect_t& r);
// 字符串转换成矩形范围
bool GuiUtil_StringToRect(const char* val, rect_t& r);

// 是否多字节字符的领头字节
bool GuiUtil_IsLeadByte(unsigned int val);
bool GuiUtil_IsLeadByteEx(unsigned int code_page, unsigned int val);

// SHIFT键是否按下
bool GuiUtil_ShiftPressed();
// CTRL键是否按下
bool GuiUtil_ControlPressed();
// ALT键是否按下
bool GuiUtil_MenuPressed();

// 复制到粘贴版
bool GuiUtil_CopyToClipboard(const wchar_t* ws);
// 从粘贴版拷贝
bool GuiUtil_PasteFromClipboard(result_wstring& ws);

// 点坐标是否在指定范围中
bool GuiUtil_PointInRect(int x, int y, int x1, int y1, int x2, int y2);
// 点坐标是否在指定椭圆形中
bool GuiUtil_PointInRound(int x, int y, int x1, int y1, int x2, int y2);

// 将数值规范在父控件相应属性之内
int GuiUtil_NormalizeLeft(IControl* pControl, int value);
int GuiUtil_NormalizeTop(IControl* pControl, int value);
int GuiUtil_NormalizeWidth(IControl* pControl, int value);
int GuiUtil_NormalizeHeight(IControl* pControl, int value);

// 获得字体或父控件的字体
const char* GuiUtil_GetValidFontName(IControl* pControl);

// 执行回调
int GuiUtil_RunCallback(IControl* pControl, const char* event, 
	const IVarList& args);

// 属性值是否相等
bool GuiUtil_PropertyEqual(const IVar& v1, const IVar& v2);

// 转换成字符串
result_string GuiUtil_Utf16ToAnsi(const wchar_t* str);
// 转换成宽字符串
result_wstring GuiUtil_AnsiToUtf16(const char* str);

// UTF16转换成UTF8
result_wstring GuiUtil_Utf8ToUtf16(const char* str);
// UTF8转换成UTF16
result_string GuiUtil_Utf16ToUtf8(const wchar_t* str);

// 是否包含UTF8字符
bool GuiUtil_HasUtf8Char(const char* str);

// 画箭头
bool GuiUtil_PaintArrow(IPainter* painter, int style, int x1, int y1, 
	int width, int height, unsigned int color);
// 画按钮表面
bool GuiUtil_PaintButtonFace(IPainter* painter, int x1, int y1, int x2, 
	int y2);
// 画有颜色的按钮
bool GuiUtil_PaintButtonFaceColor(IPainter* painter, unsigned int icolor, 
	unsigned int icolorwhite, int x1, int y1, int x2, int y2);

bool GuiUtil_PaintButtonFaceDiv(IPainter* painter, int x1, int y1, int x2, 
								int y2 , int iDivIn , int iDivOut , int iDivEnd);

// 返回在指定宽度内能够显示的字符串
const wchar_t* GuiUtil_GetCanShowText(IPainter* painter, 
	const wchar_t* text, int width, const wchar_t* suffix,
	wchar_t* buffer, size_t size);


inline bool PtInRect(const rect_t* area, const point_t& pt)
{
	if(area->left<=pt.x && pt.x<=area->right
		&& area->top<=pt.y && pt.y<=area->bottom)
		return true;

	return false;
}

#endif // _GUI_UTILS_H
