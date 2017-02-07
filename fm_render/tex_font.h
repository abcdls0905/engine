//--------------------------------------------------------------------
// �ļ���:		tex_font.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ������ͼ������

class Painter;
class Render;
class ITexture;

class CTexFont: public IFont
{
private:
	// ÿ������е�����ַ���
	enum { LINE_CHAR_NUM = 256 };

	// �ַ�����ͼ�ϵ�λ����Ϣ
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
	// ���ָ����������
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w,
		unsigned int color, const wchar_t* text, size_t len, float ratio);
	virtual bool DumpTex(const char* file_name) { return false; }
	
	// ����
	virtual void SetName(const char* value) { m_strName = value; }
	virtual const char* GetName() { return m_strName.c_str(); }

private:
	CTexFont();
	CTexFont(const CTexFont&);
	CTexFont& operator=(const CTexFont&);
	
	// ����ַ���Ϣ
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
