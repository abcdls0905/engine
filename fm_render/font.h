//--------------------------------------------------------------------
// �ļ���:		font.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��9��1��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
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
// ϵͳ����

class Painter;
class Render;
class CFontManager;

enum
{
	FONT_FADE_NONE, //������
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
	// �������ߴ�
	enum { MAX_FONT_HEIGHT = 256 };
	// ÿ������е�����ַ���
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

	//�Ƿ�ʹ�ô���ͼ����������н���ʱ
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

	//ȡ��һЩ����ʾ�ַ��Ŀ�ȣ�����ո��Ʊ����
	inline int get_hide_char_width(wchar_t ch)
	{
		switch(ch)
		{
		case 32: //��ǿո�
			return (m_nTexUnitHeight+1)/3;
		case 12288: //����ȫ�ǿո�
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

	// �ͷ�
	virtual void Release();
	// ��ʼ��
	virtual bool Initialize();
	// �ر�
	virtual bool Shutdown();
	
	// ������ּ��
	virtual int GetInterval();
	// ������ָ߶�
	virtual int GetTextHeight();
	// ���ָ���������ֿ��
	virtual int GetTextLenWidth(const wchar_t* text, size_t len);
	// ��ȡ��һ�������ַ��Ŀ��
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin);
	// ������ʱ��Ӱ/���ɫ����ͬ��ɫ������ʱ���ú��ٵ�����ӿ�,�����������Ч
	virtual bool SetShadowColor(unsigned int color);
	// ���ָ����������
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	// ���������ͼ
	virtual bool DumpTex(const char* file_name);

	// ����
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
	
	// ����ַ�����ͼ
	bool AddToTexture(wchar_t ch);
	bool InnerWriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	// ��䶥�㻺��
	size_t FillVertexBuffer(CPaintBatchs::vertex_pict_t* pv, int len, 
		const wchar_t* text, float x, float y, float depth_z, float depth_w,
		unsigned int color, float ratio);

	// ��䶥�㻺�壨һ���ַ���;
	size_t FillVertexBufferChar(CPaintBatchs::vertex_pict_t* pv, 
		wchar_t text, float& x, float& y, float depth_z, float depth_w,
		unsigned int color, float ratio);

	// ��䶥�㻺��;
	size_t FillVertexBufferText(const wchar_t* text, int len, int x, int y, float depth_z, float depth_w,
		unsigned int color, float ratio);


	// ����ַ���Ϣ
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
