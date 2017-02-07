//--------------------------------------------------------------------
// 文件名:		tex_font.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TEX_FONT_H
#define _TEX_FONT_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/fast_str.h"


#include "../visual/i_font.h"
#include "../utils/pod_hash_map.h"

// 基于贴图的字体

class Painter;
class Render;
class ITexture;

class CTexFont: public IFont
{
private:
	// 每个输出行的最大字符数
	enum { LINE_CHAR_NUM = 256 };

	// 字符在贴图上的位置信息
	struct char_data_t
	{
		int	nLeft;
		int	nTop;
		int	nRight;
		int nBottom;
	};

public:
	static CTexFont* NewInstance(Painter* pPainter, Render* pRender, 
		const char* font_tex, size_t tex_width, size_t tex_height, 
		size_t height, size_t interval, const int* char_info, size_t char_num);

public:
	CTexFont(Painter* pPainter, Render* pRender, const char* font_tex, 
		size_t tex_width, size_t tex_height, size_t height, size_t interval, 
		const int* char_info, size_t char_num);
	virtual ~CTexFont();

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
	// 输出指定长度文字
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w,
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	virtual bool DumpTex(const char* file_name) { return false; }
	
	// 名字
	virtual void SetName(const char* value) { m_strName = value; }
	virtual const char* GetName() { return m_strName.c_str(); }

private:
	CTexFont();
	CTexFont(const CTexFont&);
	CTexFont& operator=(const CTexFont&);
	
	// 获得字符信息
	char_data_t* GetCharInfo(wchar_t ch)
	{
		char_data_t* data;

		if (!m_CharInfo.GetData(ch, data))
		{
			return NULL;
		}

		return data;
	}

private:
	Painter* m_pPainter;
	Render* m_pRender;
	ITexture* m_pTex;
	core_string m_strName;
	core_string m_strFontTex;
	size_t m_nTexWidth; 
	size_t m_nTexHeight;
	size_t m_nHeight;
	size_t m_nInterval;
	TPodHashMap<int, char_data_t*, TPodTraits<int>, TCoreAlloc> m_CharInfo;
};

#endif // _TEX_FONT_H
