//--------------------------------------------------------------------
// 文件名:		font_manager.h
// 内  容:		字体纹理管理器
// 说  明:		
// 创建日期:	2010年8月18日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "font_manager.h"
#include "font.h"
#include "render.h"
#include "dynamic_tex.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"

CFontManager* CFontManager::NewInstance(Render* pRender)
{
	CFontManager* p = (CFontManager*)CORE_ALLOC(
		sizeof(CFontManager));

	new (p) CFontManager(pRender);

	return p;
}

CFontManager::CFontManager(Render* pRender)
{
    Assert(pRender != NULL);
	
	m_pRender = pRender;

	int error = FT_Init_FreeType(&m_FontLibrary);

	if (error)
	{
		abort();
	}
}

CFontManager::~CFontManager()
{
	size_t count = m_FontTextures.size();

	for (size_t i = 0; i < count; ++i)
	{
		SAFE_RELEASE(m_FontTextures[i]->pFontTex);
//		SAFE_RELEASE(m_FontTextures[i]->pOutlineTex);
		CORE_DELETE(m_FontTextures[i]);
	}
	
	FT_Done_FreeType(m_FontLibrary);
}

void CFontManager::Release()
{
	CORE_DELETE(this);
}

bool CFontManager::AddFont(CFont* pFont, int font_size, int tex_width, 
	int tex_height, bool outline, bool use_bigtexture, int& font_index, int& tex_index)
{
	Assert(pFont != NULL);

	// +1为字体本身容错，+2则是描边用
	int need_size = font_size + 3;
	int slot_size = 16;

	while (slot_size < need_size)
	{
		slot_size *= 2;
	}

	tex_index = -1;

	for (size_t i = 0; i < m_FontTextures.size(); ++i)
	{
		if ((m_FontTextures[i]->nSlotSize == slot_size)
			&& (m_FontTextures[i]->nTexWidth == tex_width)
			&& (m_FontTextures[i]->nTexHeight == tex_height)
			&& m_FontTextures[i]->use_big_texture == use_bigtexture)
		{
			tex_index = (int)i;
			break;
		}
	}

	font_tex_t* pFontTex;

	if (tex_index < 0)
	{
		tex_index = (int)m_FontTextures.size();

		pFontTex = CORE_NEW(font_tex_t);
		pFontTex->nSlotSize = slot_size;
		pFontTex->nTexWidth = tex_width;
		pFontTex->nTexHeight = tex_height;
		pFontTex->nSlotColCount = tex_width / slot_size;
		pFontTex->nSlotRowCount = tex_width / slot_size;
		pFontTex->nSlotMaxCount = 
			pFontTex->nSlotColCount * pFontTex->nSlotRowCount;
		pFontTex->nCurSlot = 0;
		pFontTex->nCurOutlineSlot = 0;
		if(use_bigtexture)
			pFontTex->pFontTex = (CDynamicTex*)m_pRender->CreateDynamicTex(
				tex_width, tex_height, 1, TEX_FORMAT_RGB888_UNORM);
		else
			pFontTex->pFontTex = (CDynamicTex*)m_pRender->CreateDynamicTex(
				tex_width, tex_height, 1, TEX_FORMAT_A8_UNORM);
//		pFontTex->pOutlineTex = NULL;
		pFontTex->use_big_texture = use_bigtexture;

		m_FontTextures.push_back(pFontTex);
	}
	else
	{
		pFontTex = m_FontTextures[tex_index];
	}

	/*
	if (outline && (NULL == pFontTex->pOutlineTex))
	{
		pFontTex->pOutlineTex = (CDynamicTex*)m_pRender->CreateDynamicTex(
			tex_width, tex_height, 1, TEX_FORMAT_A8_UNORM);
	}*/

	font_index = (int)m_FontList.size();
	m_FontList.push_back(pFont);

	return true;
}

int CFontManager::GetOneSlot(int tex_index)
{
	Assert(size_t(tex_index) < m_FontTextures.size());

	font_tex_t* pFontTex = m_FontTextures[tex_index];

	if ((pFontTex->nCurSlot) < pFontTex->nSlotMaxCount)
	{
		pFontTex->nCurSlot += 1;
		return pFontTex->nCurSlot - 1;
	}
	else
	{
		pFontTex->nCurSlot = 0;
		return 0;
	}
}

bool CFontManager::SetSlotData(int tex_index, int slot_index, 
	int font_index, wchar_t ch)
{
	Assert(size_t(tex_index) < m_FontTextures.size());

	font_tex_t* pFontTex = m_FontTextures[tex_index];
	size_t count = m_FontTextures[tex_index]->ShareSlots.size();

	if (size_t(slot_index) >= count)
	{
		slot_data_t data;

		data.nFontIndex = font_index;
		data.nChar = ch;

		pFontTex->ShareSlots.push_back(data);
		return true;
	}
	else
	{
		slot_data_t& data = pFontTex->ShareSlots[slot_index];
		// 清理之前字体占用的信息
		CFont* pFont = m_FontList[data.nFontIndex];

		pFont->RemoveChar(data.nChar);
		// 添加新的字体占用的信息
		data.nFontIndex = font_index;
		data.nChar = ch;
		return true;
	}
}

// 设置一个共享格子数据
int CFontManager::SetSlotData(int tex_index, int font_index, 
	wchar_t ch)
{
	int slot_index = GetOneSlot(tex_index);
	if(!SetSlotData(tex_index, slot_index, font_index, ch))
		return -1;
	return slot_index;
}

bool CFontManager::GetSlotPosition(int tex_index, int slot_index, int& left, 
	int& top)
{
	Assert(size_t(tex_index) < m_FontTextures.size());

	font_tex_t* pFontTex = m_FontTextures[tex_index];
	unsigned int col = 
		(unsigned int)slot_index % (unsigned int)pFontTex->nSlotColCount;
	unsigned int row = 
		(unsigned int)slot_index / (unsigned int)pFontTex->nSlotColCount;

	left = col * pFontTex->nSlotSize;
	top = row * pFontTex->nSlotSize;

	return true;
}

bool CFontManager::DumpTex()
{
  
	/*
	size_t count = m_FontTextures.size();

    for (size_t i = 0; i < count; ++i)
    {
        char file_name[256];

        SafeSprintf(file_name, sizeof(file_name), "share_font_%d.dds", i);

		
        ID3D11Texture2D* pFontTex = m_FontTextures[i]->pFontTex->GetTexture();
	    HRESULT hr = D3DX11SaveTextureToFile(m_pRender->GetDeviceContext(),
			pFontTex, D3DX11_IFF_DDS, file_name);

	    if (FAILED(hr))
	    {
		    return false;
	    }

        ID3D11Texture2D* pOutlineTex = 
			m_FontTextures[i]->pOutlineTex->GetTexture();

        if (pOutlineTex != NULL)
        {
            char fname[256];

			SafeSprintf(fname, sizeof(fname), "outline_%s", file_name);
			
			HRESULT hr = D3DX11SaveTextureToFile(m_pRender->GetDeviceContext(),
				pOutlineTex, D3DX11_IFF_DDS, fname);

	        if (FAILED(hr))
	        {
		        return false;
	        }
        }
    }

*/
	return true;
}
