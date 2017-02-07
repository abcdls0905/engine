//--------------------------------------------------------------------
// �ļ���:		font_manager.h
// ��  ��:		�������������
// ˵  ��:		
// ��������:	2010��8��18��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../visual/i_resource.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class Render;
class CDynamicTex;
class CFont;

class CFontManager
{
public:
	struct slot_data_t
	{
		unsigned short nFontIndex;
		wchar_t nChar;
	};

    struct font_tex_t
    {
		int nSlotSize;	// ʵ�ʵ�����ռ����ͼ��С
		int nTexWidth;
		int nTexHeight;
		int nSlotColCount;
		int nSlotRowCount;
		int nSlotMaxCount;
		int nCurSlot;   // ��ǰ��ͼʹ�õ��ڼ���
		int nCurOutlineSlot;
		bool use_big_texture;
		CDynamicTex* pFontTex;
	//	CDynamicTex* pOutlineTex;
		TArrayPod<slot_data_t, 1, TCoreAlloc> ShareSlots;
    };

private:
	int GetOneSlot(int tex_index);

	// ����һ�������������
    bool SetSlotData(int tex_index, int slot_index, int font_index, 
		wchar_t ch);

public:
	static CFontManager* NewInstance(Render* pRender);

public:
    CFontManager(Render* pRender);
    ~CFontManager();

	// �ͷ�
	void Release();
	// ���������ͼ����
	bool AddFont(CFont* pFont, int font_size, int tex_width, 
		int tex_height, bool outline, bool use_bigtexture, int& font_index, int& tex_index);

    // ��ȡ�������λ��
    bool GetSlotPosition(int tex_index, int slot_index, int& left, int& top);

	// ����һ�������������
    int SetSlotData(int tex_index, int font_index, 
		wchar_t ch);
    
	// ��ȡ��ͼ�ָ�Ĵ�С
    int GetSlotUnitSize(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotSize;
	}

	// ��ȡ�����������
	int GetSlotCount(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotMaxCount;
	}

	// ��ȡ�������������
	int GetSlotColCount(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotColCount;
	}

    // ��ȡ������ͼ
    CDynamicTex* GetFontTexture(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->pFontTex;
	}

	/*
    // ��ȡ���������ͼ
    CDynamicTex* GetOutlineTexture(size_t tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->pOutlineTex;
	}*/

	// �����ͼ���
    int GetTextureWidth(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nTexWidth;
	}
	
	// �����ͼ�߶�
    int GetTextureHeight(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nTexHeight;
	}

	// �����
	FT_Library GetFontLibrary() const { return m_FontLibrary; }

    // ����������ͼ
    bool DumpTex();

private:
	CFontManager();
	CFontManager(const CFontManager&);
	CFontManager& operator=(const CFontManager&);

private:
	Render* m_pRender;
	FT_Library m_FontLibrary;
    TArrayPod<font_tex_t*, 1, TCoreAlloc> m_FontTextures;
    TArrayPod<CFont*, 1, TCoreAlloc> m_FontList;
};

#endif // _DX_FONT_MANAGER_H
