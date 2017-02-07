//--------------------------------------------------------------------
// 文件名:		font_manager.h
// 内  容:		字体纹理管理器
// 说  明:		
// 创建日期:	2010年8月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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
		int nSlotSize;	// 实际单个字占用贴图大小
		int nTexWidth;
		int nTexHeight;
		int nSlotColCount;
		int nSlotRowCount;
		int nSlotMaxCount;
		int nCurSlot;   // 当前贴图使用到第几格
		int nCurOutlineSlot;
		bool use_big_texture;
		CDynamicTex* pFontTex;
	//	CDynamicTex* pOutlineTex;
		TArrayPod<slot_data_t, 1, TCoreAlloc> ShareSlots;
    };

private:
	int GetOneSlot(int tex_index);

	// 设置一个共享格子数据
    bool SetSlotData(int tex_index, int slot_index, int font_index, 
		wchar_t ch);

public:
	static CFontManager* NewInstance(Render* pRender);

public:
    CFontManager(Render* pRender);
    ~CFontManager();

	// 释放
	void Release();
	// 获得字体贴图索引
	bool AddFont(CFont* pFont, int font_size, int tex_width, 
		int tex_height, bool outline, bool use_bigtexture, int& font_index, int& tex_index);

    // 获取共享格子位置
    bool GetSlotPosition(int tex_index, int slot_index, int& left, int& top);

	// 设置一个共享格子数据
    int SetSlotData(int tex_index, int font_index, 
		wchar_t ch);
    
	// 获取贴图分格的大小
    int GetSlotUnitSize(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotSize;
	}

	// 获取共享格子总数
	int GetSlotCount(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotMaxCount;
	}

	// 获取共享格子总列数
	int GetSlotColCount(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nSlotColCount;
	}

    // 获取共享贴图
    CDynamicTex* GetFontTexture(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->pFontTex;
	}

	/*
    // 获取共享描边贴图
    CDynamicTex* GetOutlineTexture(size_t tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->pOutlineTex;
	}*/

	// 获得贴图宽度
    int GetTextureWidth(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nTexWidth;
	}
	
	// 获得贴图高度
    int GetTextureHeight(int tex_index) const
	{
		Assert(size_t(tex_index) < m_FontTextures.size());

		return m_FontTextures[tex_index]->nTexHeight;
	}

	// 字体库
	FT_Library GetFontLibrary() const { return m_FontLibrary; }

    // 导出共享贴图
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
