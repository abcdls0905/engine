//--------------------------------------------------------------------
// 文件名:		tex_light_map.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_light_map.h"
#include "../visual/i_resource.h"
#include "../public/core_log.h"

// CTexLightMap

CTexLightMap::CTexLightMap()
{
	m_nRows = 16;
	m_nCols = 16;
	m_pTexture = NULL;
}

CTexLightMap::~CTexLightMap()
{
	ReleaseTex();
}

void CTexLightMap::SetTexture(IRender* pRender, ITexture* value)
{
	SAFE_RELEASE(m_pTexture);

	if (value)
	{
//		m_pTexture = pRender->CreateStaticTexFrom(value);
	}
}

ITexture* CTexLightMap::GetTexture()  
{ 
	return m_pTexture; 
}

bool CTexLightMap::CreateTex(IRender* pRender)
{
	Assert(pRender != NULL);
	
	/*
	m_pTexture = pRender->CreateStaticTex(m_nCols, m_nRows, 1, 
		TEX_FORMAT_RGBA8888_UNORM);

	if (NULL == m_pTexture)
	{
		CORE_TRACE("(CTexLightMap::CreateTex)CreateTexture failed");
		return false;
	}
	*/

	return true;
}

void CTexLightMap::ReleaseTex()
{
	SAFE_RELEASE(m_pTexture);
}

bool CTexLightMap::BuildTex(IRender* pRender, ITexture* pTex)
{
	Assert(pRender != NULL);
	Assert(pTex != NULL);

	m_pTexture = pTex;
	return true;
}

bool CTexLightMap::WriteColor32(const unsigned int* colors, size_t rows, 
	size_t cols)
{
	/*
	Assert(colors != NULL);
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	
	D3DLOCKED_RECT lr;
	
	m_pTexture->LockRect(0, &lr, NULL);
	
	// 图片的数据是从上到下的
	BYTE* pDstRow = (BYTE*)lr.pBits;
	
	for (size_t r = 0; r < m_nRows; ++r)
	{
		DWORD* pDst32 = (DWORD*)pDstRow;
		
		for (size_t c = 0; c < m_nCols; ++c)
		{
			*pDst32++ = colors[c];
		}
		
		colors += cols;
		
		pDstRow += lr.Pitch;
	}
	
	m_pTexture->UnlockRect(0);
	*/
	
	return true;
}

bool CTexLightMap::WriteValue(unsigned int value, size_t rows, size_t cols)
{
	/*
	Assert(rows == m_nRows);
	Assert(cols == m_nCols);
	
	D3DLOCKED_RECT lr;
	
	m_pTexture->LockRect(0, &lr, NULL);
	
	// 图片的数据是从上到下的
	BYTE* pDstRow = (BYTE*)lr.pBits;
	
	for (size_t r = 0; r < m_nRows; ++r)
	{
		DWORD* pDst32 = (DWORD*)pDstRow;
		
		for (size_t c = 0; c < m_nCols; ++c)
		{
			*pDst32++ = value;
		}
		
		pDstRow += lr.Pitch;
	}
	
	m_pTexture->UnlockRect(0);
	*/
	
	return true;
}

bool CTexLightMap::SaveBitmap(const char* file)
{
	/*
	Assert(file != NULL);
	Assert(m_pTexture != NULL);

	return SUCCEEDED(D3DXSaveTextureToFile(file, D3DXIFF_BMP, 
		m_pTexture->GetDxTexture(), NULL));
	*/
	return true;
}

bool CTexLightMap::SaveDDS(IRender* pRender, const char* file,
	bool save_occlusion)
{
	/*
	Assert(m_pTexture != NULL);
	
	HRESULT hr;
	
	// 转换文件格式为DXT1
	ID3DXBuffer* pTempBuffer = NULL;

	hr = D3DXSaveTextureToFileInMemory(&pTempBuffer, D3DXIFF_DDS, 
		m_pTexture->GetDxTexture(), NULL);

	if (FAILED(hr))
	{
		CORE_TRACE("(CTexLightMap::SaveDDS)save to temp buffer failed");
		CORE_TRACE(file);
		return false;
	}

	DxTexture* pTexSave = NULL;

	if (save_occlusion)
	{
		hr = D3DXCreateTextureFromFileInMemoryEx(pRender->GetDevice(), 
			pTempBuffer->GetBufferPointer(), pTempBuffer->GetBufferSize(),
			m_nCols, m_nRows, 0, 0, D3DFMT_DXT5, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexSave);
	}
	else
	{
		hr = D3DXCreateTextureFromFileInMemoryEx(pRender->GetDevice(), 
			pTempBuffer->GetBufferPointer(), pTempBuffer->GetBufferSize(),
			m_nCols, m_nRows, 0, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexSave);
	}

	if (FAILED(hr))
	{
		CORE_TRACE("(CTexLightMap::SaveDDS)create from buffer failed");
		CORE_TRACE(file);
		pTempBuffer->Release();
		return false;
	}
	
	hr = D3DXSaveTextureToFile(file, D3DXIFF_DDS, pTexSave, NULL);

	pTempBuffer->Release();
	pTexSave->Release();

	if (FAILED(hr))
	{
		CORE_TRACE("(CTexLightMap::SaveDDS)save to file failed");
		CORE_TRACE(file);
		return false;
	}
	*/

	return true;
}
 
