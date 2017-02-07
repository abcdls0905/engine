//--------------------------------------------------------------------
// 文件名:		i_font.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_FONT_H
#define _VISUAL_I_FONT_H

#include "../public/macros.h"

// 字体基类

struct char_info_t
{
	unsigned short nCharWidth;
	unsigned short nCharHeight;
	short nXAdvance;
	short nYOffset;
	short nXOffset;
	short nUserYOffset;
	short nUserXOffset;
};

class IFont
{
public:
	virtual ~IFont() = 0;

	// 释放
	virtual void Release() = 0;
	// 初始化
	virtual bool Initialize() = 0;
	// 关闭
	virtual bool Shutdown() = 0;
	
	// 获得文字间距
	virtual int GetInterval() = 0;
	// 获得文字高度
	virtual int GetTextHeight() = 0;
	// 获得指定长度文字宽度
	virtual int GetTextLenWidth(const wchar_t* text, size_t len) = 0;
	// 获取下一个语言字符的宽度
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin) { return 1; };
	// 设置临时阴影/描边色，不同颜色可以临时设置后再调输出接口，仅描边字体有效
	virtual bool SetShadowColor(unsigned int color) { return true; }
	// 输出指定长度文字
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio) = 0;
	// 输出字体贴图
	virtual bool DumpTex(const char* file_name) = 0;

	// 名字
	virtual void SetName(const char* value) = 0;
	virtual const char* GetName() = 0;

	virtual void SetXOffset(wchar_t ch, int offset){};

	virtual void SetYOffset(wchar_t ch, int offset){};
	
	virtual int GetXOffset(wchar_t ch)  {return 0;};

	virtual int GetYOffset(wchar_t ch)  {return 0;};

	virtual bool GetCharDataInfo(wchar_t ch, char_info_t & info){ return false; }


};

inline IFont::~IFont() {}

#endif // _VISUAL_I_FONT_H
