//--------------------------------------------------------------------
// �ļ���:		gui_utils.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ���ߺ�����

class Gui;
class IEntity;
class IControl;
class IPainter;

// ��ɫֵת�����ַ���
result_string GuiUtil_IntToColor(unsigned int val);
// �ַ���ת������ɫֵ
unsigned int GuiUtil_ColorToInt(const char* val);

// ���ַ���ת��������
int GuiUtil_WideStrToInt(const wchar_t* val);
// ������ת�����ַ���
core_wstring GuiUtil_IntToWideStr(int val);

// ���ַ����ֽ�Ϊ�����б�
bool GuiUtil_StringToIntList(const char* val, IVarList& result);

// �ַ���ת����ͼƬ����
//bool GuiUtil_ConvertImage(const char* name, core_string& tex,
//	int& left, int& top, int& right, int& bottom);

// �ַ���ת���ɶ���ֵ
int GuiUtil_AlignToInt(const char* val);
// ����ֵת�����ַ���
const char* GuiUtil_IntToAlign(int val);

// ���η�Χת�����ַ���
result_string GuiUtil_RectToString(const rect_t& r);
// �ַ���ת���ɾ��η�Χ
bool GuiUtil_StringToRect(const char* val, rect_t& r);

// �Ƿ���ֽ��ַ�����ͷ�ֽ�
bool GuiUtil_IsLeadByte(unsigned int val);
bool GuiUtil_IsLeadByteEx(unsigned int code_page, unsigned int val);

// SHIFT���Ƿ���
bool GuiUtil_ShiftPressed();
// CTRL���Ƿ���
bool GuiUtil_ControlPressed();
// ALT���Ƿ���
bool GuiUtil_MenuPressed();

// ���Ƶ�ճ����
bool GuiUtil_CopyToClipboard(const wchar_t* ws);
// ��ճ���濽��
bool GuiUtil_PasteFromClipboard(result_wstring& ws);

// �������Ƿ���ָ����Χ��
bool GuiUtil_PointInRect(int x, int y, int x1, int y1, int x2, int y2);
// �������Ƿ���ָ����Բ����
bool GuiUtil_PointInRound(int x, int y, int x1, int y1, int x2, int y2);

// ����ֵ�淶�ڸ��ؼ���Ӧ����֮��
int GuiUtil_NormalizeLeft(IControl* pControl, int value);
int GuiUtil_NormalizeTop(IControl* pControl, int value);
int GuiUtil_NormalizeWidth(IControl* pControl, int value);
int GuiUtil_NormalizeHeight(IControl* pControl, int value);

// �������򸸿ؼ�������
const char* GuiUtil_GetValidFontName(IControl* pControl);

// ִ�лص�
int GuiUtil_RunCallback(IControl* pControl, const char* event, 
	const IVarList& args);

// ����ֵ�Ƿ����
bool GuiUtil_PropertyEqual(const IVar& v1, const IVar& v2);

// ת�����ַ���
result_string GuiUtil_Utf16ToAnsi(const wchar_t* str);
// ת���ɿ��ַ���
result_wstring GuiUtil_AnsiToUtf16(const char* str);

// UTF16ת����UTF8
result_wstring GuiUtil_Utf8ToUtf16(const char* str);
// UTF8ת����UTF16
result_string GuiUtil_Utf16ToUtf8(const wchar_t* str);

// �Ƿ����UTF8�ַ�
bool GuiUtil_HasUtf8Char(const char* str);

// ����ͷ
bool GuiUtil_PaintArrow(IPainter* painter, int style, int x1, int y1, 
	int width, int height, unsigned int color);
// ����ť����
bool GuiUtil_PaintButtonFace(IPainter* painter, int x1, int y1, int x2, 
	int y2);
// ������ɫ�İ�ť
bool GuiUtil_PaintButtonFaceColor(IPainter* painter, unsigned int icolor, 
	unsigned int icolorwhite, int x1, int y1, int x2, int y2);

bool GuiUtil_PaintButtonFaceDiv(IPainter* painter, int x1, int y1, int x2, 
								int y2 , int iDivIn , int iDivOut , int iDivEnd);

// ������ָ��������ܹ���ʾ���ַ���
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
