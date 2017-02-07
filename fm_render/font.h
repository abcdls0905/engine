//--------------------------------------------------------------------
// 文件名:		font.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年9月1日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _FONT_H
#define _FONT_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../visual/i_font.h"
#include "../utils/pod_hash_map.h"
#include "paint_batch.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
//#define NEW_FREETYPE_FONT
// 系统字体

class Painter;
class Render;
class CFontManager;

enum
{
	FONT_FADE_NONE, //不渐变
	FONT_FADE_UP,
	FONT_FADE_DOWN,
	FONT_FADE_MIDDLE
};

class CFont: public IFont
{
	struct Rect
	{
		 float x, y;
		 float w, h;
	};
private:
	// 最大字体尺寸
	enum { MAX_FONT_HEIGHT = 256 };
	// 每个输出行的最大字符数
	enum { LINE_CHAR_NUM = 256 };

	struct char_data_t
	{
		unsigned short nCharWidth;
		unsigned short nCharHeight;
		unsigned short nCharSlot;
		bool bInit;
		short nXAdvance;
		short nYOffset;
		short nXOffset;
		short nUserYOffset;
		short nUserXOffset;
	};

	//是否使用大贴图，但描边字有渐变时
	inline bool IsUseBigTexture() 
	{

#ifndef NEW_FREETYPE_FONT
		return m_nOutlineNum > 0 && m_nFadeType;
#else
		return m_nOutlineNum > 0;
#endif

	}

	bool FillCharInfo(wchar_t ch, char_data_t& info, bool commit_gpu = false);

	unsigned char* getGlyphBitmap(wchar_t theChar, long &outWidth, long &outHeight, Rect &outRect,int &xAdvance);
	unsigned char * getGlyphBitmapWithOutline(wchar_t theChar, FT_BBox &bbox);

	//取得一些不显示字符的宽度，比如空格，制表符等
	inline int get_hide_char_width(wchar_t ch)
	{
		switch(ch)
		{
		case 32: //半角空格
			return (m_nTexUnitHeight+1)/3;
		case 12288: //中文全角空格
			return m_nTexUnitHeight;
		case '\t':
			return (m_nTexUnitHeight+1)/3*4;
		default:
			return -1;
		}

	}
public:
	static CFont* NewInstance(Painter* pPainter, Render* pRender, 
		CFontManager* pManager, const char* font_file, 
		unsigned int tex_size, unsigned int height, unsigned int interval, 
		bool bold, bool smooth, bool italic, bool underline, 
		bool strike_out, bool var_extent, bool outline, int outline_num, const FmVec3* outline_color,
		int fade_type,
		unsigned int shadow_color, int shadow_x, int shadow_y);

public:
	CFont(Painter* pPainter, Render* pRender, 
		CFontManager* pManager, const char* font_file, 
		unsigned int tex_size, unsigned int height, unsigned int interval, 
		bool bold, bool smooth, bool italic, bool underline, 
		bool strike_out, bool var_extent, bool outline, int outline_num, const FmVec3* outline_color, 
		int fade_type, 
		unsigned int shadow_color, int shadow_x, int shadow_y);
	virtual ~CFont();

	// 释放
	virtual void Release();
	// 初始化
	virtual bool Initialize();
	// 关闭
	virtual bool Shutdown();
	
	// 获得文字间距
	virtual int GetInterval();
	// 获得文字高度
	virtual int GetTextHeight();
	// 获得指定长度文字宽度
	virtual int GetTextLenWidth(const wchar_t* text, size_t len);
	// 获取下一个语言字符的宽度
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin);
	// 设置临时阴影/描边色，不同颜色可以临时设置后再调输出接口,仅描边字体有效
	virtual bool SetShadowColor(unsigned int color);
	// 输出指定长度文字
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	// 输出字体贴图
	virtual bool DumpTex(const char* file_name);

	// 名字
	virtual void SetName(const char* value) { m_strName = value; }
	virtual const char* GetName() { return m_strName.c_str(); }

    bool RemoveChar(wchar_t ch);

	virtual void SetXOffset(wchar_t ch, int offset);

	virtual void SetYOffset(wchar_t ch, int offset);

	virtual int GetXOffset(wchar_t ch);

	virtual int GetYOffset(wchar_t ch);

	virtual bool IsVarExtent()const { return m_bVarExtent; }

	virtual bool GetCharDataInfo(wchar_t ch, char_info_t & info);
private:
	CFont();
	CFont(const CFont&);
	CFont& operator=(const CFont&);
	
	// 添加字符到贴图
	bool AddToTexture(wchar_t ch);
	bool InnerWriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	// 填充顶点缓冲
	size_t FillVertexBuffer(CPaintBatchs::vertex_pict_t* pv, int len, 
		const wchar_t* text, float x, float y, float depth_z, float depth_w,
		unsigned int color, float ratio);

	// 填充顶点缓冲（一个字符）;
	size_t FillVertexBufferChar(CPaintBatchs::vertex_pict_t* pv, 
		wchar_t text, float& x, float& y, float depth_z, float depth_w,
		unsigned int color, float ratio);

	// 填充顶点缓冲;
	size_t FillVertexBufferText(const wchar_t* text, int len, int x, int y, float depth_z, float depth_w,
		unsigned int color, float ratio);


	// 获得字符信息
	char_data_t* GetCharInfo(wchar_t ch);

private:
	Painter* m_pPainter;
    Render* m_pRender;
    CFontManager* m_pFontManager;
	core_string m_strName;
	core_string m_strFontFile;
	int m_nTexSize;
	int m_nHeight;
	unsigned int m_nInterval;
	bool m_bBold;
	bool m_bSmooth;
	bool m_bItalic;
	bool m_bUnderline;
	bool m_bStrikeOut;
	bool m_bVarExtent;
	bool m_bOutline;
	int m_nOutlineNum;
	FmVec3 m_nOutlineColor;
	unsigned int m_nDefShadowColor;
	unsigned int m_nShadowColor;
	int m_nShadowOffsetX;
	int m_nShadowOffsetY;
    unsigned int m_nTexUnitHeight;
	FT_Face m_FontFace;
	FT_Stroker m_Stroker;
    int m_nTexIndex;
    int m_nFontIndex;
	bool m_bNoBitmap;
	int m_nFadeType;
	TPodHashMap<wchar_t, char_data_t, TPodTraits<wchar_t>, 
		TCoreAlloc> m_HashCharInfo;
};

#endif // _FONT_H
