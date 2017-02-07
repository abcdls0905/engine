//--------------------------------------------------------------------
// 文件名:		terrain_light.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "terrain_light.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "tex_light_map.h"
#include "../visual/i_render.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"

// 混合颜色
static unsigned int blend_color(unsigned int src_value, unsigned int dst_value)
{
	unsigned int dst_alpha = (dst_value >> 24) & 0xFF;

	if (0 == dst_alpha)
	{
		return src_value;
	}

	float src_red = float((src_value >> 16) & 0xFF) / 255.0f;
	float src_green = float((src_value >> 8) & 0xFF) / 255.0f;
	float src_blue = float((src_value >> 0) & 0xFF) / 255.0f;

	float dst_red = float((dst_value >> 16) & 0xFF) / 255.0f;
	float dst_green = float((dst_value >> 8) & 0xFF) / 255.0f;
	float dst_blue = float((dst_value >> 0) & 0xFF) / 255.0f;

	float a = float(dst_alpha) / 255.0f * 2.0f;

	float r = a * src_red * dst_red;
	float g = a * src_green * dst_green;
	float b = a * src_blue * dst_blue;

	if (a < 1.0f)
	{
		r += src_red * (1.0f - a);
		g += src_green * (1.0f - a);
		b += src_blue * (1.0f - a);
	}

	if (r > 1.0f)
	{
		r = 1.0f;
	}

	if (g > 1.0f)
	{
		g = 1.0f;
	}

	if (b > 1.0f)
	{
		b = 1.0f;
	}

	unsigned int src_alpha = (src_value >> 24) & 0xFF;

	return COLOR_ARGB(src_alpha, (unsigned long)(r * 255.0f), (unsigned long)(g * 255.0f), 
		(unsigned long)(b * 255.0f));
}

// CTerrainLight

CTerrainLight::CTerrainLight(Terrain* pTerrain, CTerrainZone* pZone,
	size_t rows, size_t cols)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_pRender = m_pTerrain->GetRender();
	m_nRows = rows;
	m_nCols = cols;
	m_pBlendColors = NULL;
	m_nLightWidth = 0;
	m_nLightHeight = 0;
	m_pLightColors = NULL;
	m_pLightTex = NULL;
	m_pDiffuseTex = NULL;
	m_bIsNull = false;
	m_bIsDesign = false;

	m_vHdrScale = FmVec3(1.0f, 1.0f, 1.0f);
}	

CTerrainLight::~CTerrainLight()
{
	ReleaseAll();
}

void CTerrainLight::ReleaseAll()
{
	//if (m_pBlendColors)
	//{
	//	CORE_FREE(m_pBlendColors, sizeof(unsigned int) * m_nRows * m_nCols);
	//	m_pBlendColors = NULL;
	//}

	if (m_pLightColors)
	{
		CORE_FREE(m_pLightColors, 
			sizeof(unsigned int) * m_nLightWidth * m_nLightHeight);
		m_pLightColors = NULL;
	}

	ReleaseLightTex();
	//SAFE_RELEASE(m_pDiffuseTex);
}

unsigned int* CTerrainLight::GetBlendColors()
{
	if (NULL == m_pBlendColors)
	{
		size_t size = m_nRows * m_nCols;
		
		m_pBlendColors = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * size);
		memset(m_pBlendColors, 0, sizeof(unsigned int) * size);

		if (!m_bIsDesign)
		{
			CreateDesignTex();
		}
	}

	return m_pBlendColors;
}

bool CTerrainLight::SetBlendColor(int row, int col, unsigned int color)
{
	Assert(size_t(row) < m_nRows);
	Assert(size_t(col) < m_nCols);

	unsigned int* colors = GetBlendColors();

	colors[row * m_nCols + col] = color;

	/*
	ITexture* pTexture = m_pLightTex->GetTexture();

	if (NULL == pTexture)
	{
		return false;
	}

	D3DSURFACE_DESC sd;

	HRESULT hr = pTexture->GetLevelDesc(0, &sd);

	Assert(sd.Width >= m_nCols);
	Assert(sd.Height >= m_nRows);

	size_t row_num = sd.Height / m_nRows;
	size_t col_num = sd.Width / m_nCols;

	D3DLOCKED_RECT lr;

	hr = pTexture->LockRect(0, &lr, NULL, 0);

	for (size_t r = 0; r < row_num; ++r)
	{
		size_t cur_row = row * row_num + r;
		
		unsigned int* pRow = (unsigned int*)((char*)lr.pBits 
			+ lr.Pitch * cur_row);

		for (size_t c = 0; c < col_num; ++c)
		{
			size_t cur_col = col * col_num + c;
			
			//pRow[col] = blend_color(
			//	m_pLightColors[cur_row * m_nCols + cur_col], color);
			pRow[col] = blend_color(
				m_pLightColors[cur_row * sd.Width + cur_col], color);
		}
	}

	hr = pTexture->UnlockRect(0);
	*/

	return true;
}

unsigned int CTerrainLight::GetBlendColor(int row, int col)
{
	Assert(size_t(row) < m_nRows);
	Assert(size_t(col) < m_nCols);

	unsigned int* colors = GetBlendColors();

	return colors[row * m_nCols + col];
}

float CTerrainLight::GetDiffuse(float u, float v)
{
	/*
	if (m_pDiffuseTex)
	{
		// 专门用于读取亮度的贴图
		D3DSURFACE_DESC sd;

		HRESULT hr = m_pDiffuseTex->GetLevelDesc(0, &sd);

		if (FAILED(hr))
		{
			CORE_TRACE("(CTerrainLight::GetDiffuse)GetLevelDesc failed");
			return 0.5f;
			//return 1.0f;
		}

		D3DLOCKED_RECT lr;

		hr = m_pDiffuseTex->LockRect(0, &lr, NULL, D3DLOCK_READONLY);

		if (FAILED(hr))
		{
			CORE_TRACE("(CTerrainLight::GetDiffuse)LockRect failed");
			return 0.5f;
			//return 1.0f;
		}

		float value = 1.0f;
		
		int x = int(u * sd.Width);
		int y = int(v * sd.Height);
		
		if ((sd.Format == D3DFMT_X8R8G8B8) || (sd.Format == D3DFMT_A8R8G8B8))
		{
			unsigned int color = *(unsigned int*)
				((unsigned char*)lr.pBits + lr.Pitch * y + x * 4);

			unsigned int red = (color >> 16) & 0xFF;
			unsigned int green = (color >> 8) & 0xFF;
			unsigned int blue = color & 0xFF;

			value = (red + green + blue) / 3 / 255.0f;
		}
		else if (sd.Format == D3DFMT_DXT1)
		{
			size_t block_x = x / 4;
			size_t block_y = y / 4;

			unsigned char* pdata = (unsigned char*)lr.pBits;

			pdata += lr.Pitch * block_y + block_x * 8;

			unsigned short* pcolor = (unsigned short*)pdata;

			unsigned int color[4];

			color[0] = pcolor[0];
			color[1] = pcolor[1];

			if (color[0] > color[1]) 
			{
				// 四色编码块：导出另两个颜色
				// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
				// 这些二位编码对应于存储在64位块中的二位位域
				color[2] = (2 * color[0] + color[1] + 1) / 3;
				color[3] = (color[0] + 2 * color[1] + 1) / 3;
			}    
			else
			{ 
				// 三色编码：导出一个颜色
				// 00 = color_0, 01 = color_1,  10 = color_2,  
				// 11 = transparent.
				// 这些二位编码对应于存储在64位块中的二位位域
				color[2] = (color[0] + color[1]) / 2;    
				color[3] = 0;    
			}

			unsigned int info = *(unsigned int*)(pdata + 4);

			size_t index = (y % 4) * 4 + (x % 4);

			unsigned int c = color[(info >> (index * 2)) & 0x3];

			unsigned int red = ((c >> 11) & 0x1F) << 3;
			unsigned int green = ((c >> 5) & 0x3F) << 2;
			unsigned int blue = (c & 0x1F) << 3;

			value = (red + green + blue) / 3 / 255.0f;
		}

		if (value < 0.0f)
		{
			value = 0.0f;
		}

		if (value > 1.0f)
		{
			value = 1.0f;
		}

		m_pDiffuseTex->UnlockRect(0);

		return value;
	}
	else
	{
		if (NULL == m_pLightColors)
		{
			return 0.5f;
			//return 1.0f;
		}

		//int x = int(u * m_nCols);
		//int y = int(v * m_nRows);
		//unsigned int color = m_pLightColors[y * m_nCols + x];

		int cols = m_pLightTex->GetCols();
		int rows = m_pLightTex->GetRows();
		
		int x = int(u * cols);
		int y = int(v * rows);

		unsigned int color = m_pLightColors[y * cols + x];

		unsigned int red = (color >> 16) & 0xFF;
		unsigned int green = (color >> 8) & 0xFF;
		unsigned int blue = (color >> 0) & 0xFF;

		return (red + green + blue) / 3 / 255.0f;
	}
	*/
	return 0.5f;
}

bool CTerrainLight::CreateDesignTex()
{
	if (m_bIsDesign)
	{
		return true;
	}
	
	CreateLightTex();

	/*
	DxTexture* pTexture = m_pLightTex->GetTexture();

	if (NULL == pTexture)
	{
		return false;
	}

	DxSurface* pOldSurface = NULL;
	HRESULT hr = pTexture->GetSurfaceLevel(0, &pOldSurface);
	D3DSURFACE_DESC old_sd;

	pOldSurface->GetDesc(&old_sd);
	pOldSurface->Release();

	size_t width = old_sd.Width;
	size_t height = old_sd.Height;

	if (width < m_nCols)
	{
		width = m_nCols;
	}

	if (height < m_nRows)
	{
		height = m_nRows;
	}

	ID3DXBuffer* pBuffer = NULL;
	
	hr = D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_BMP, pTexture, NULL);

	if (FAILED(hr))
	{
		return false;
	}

	Render* pRender = m_pRender;

	hr = D3DXCreateTextureFromFileInMemoryEx(pRender->GetDevice(),
		pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), width,
		height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
		D3DX_DEFAULT, 0, NULL, NULL, &pTexture);

	pBuffer->Release();

	if (FAILED(hr))
	{
		return false;
	}

	m_pLightTex->SetRows(height);
	m_pLightTex->SetCols(width);
	m_pLightTex->SetTexture(pRender, pTexture);

	// 记录原始光贴图的颜色
	if (m_pLightColors)
	{
		CORE_FREE(m_pLightColors, 
			sizeof(unsigned int) * m_nLightWidth * m_nLightHeight);
	}
	
	m_nLightWidth = width;
	m_nLightHeight = height;
	m_pLightColors = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * width * height);
	memset(m_pLightColors, 0, width * height * sizeof(unsigned int));

	// 颜色混合
	D3DLOCKED_RECT lr;

	pTexture->LockRect(0, &lr, NULL, 0);

	size_t row_num = height / m_nRows;
	size_t col_num = width / m_nCols;

	for (size_t r = 0; r < height; ++r)
	{
		unsigned int* pRow = (unsigned int*)((char*)lr.pBits + lr.Pitch * r);
		
		for (size_t c = 0; c < width; ++c)
		{
			unsigned int src_value = pRow[c];

			// 记录原来的颜色
			m_pLightColors[r * width + c] = src_value;

			if (m_pBlendColors)
			{
				unsigned int dst_value = 
					m_pBlendColors[r / row_num * m_nCols + c / col_num];

				pRow[c] = blend_color(src_value, dst_value);
			}
		}
	}
	
	pTexture->UnlockRect(0);
	*/

	m_bIsDesign = true;
	
	return true;
}

CTexLightMap* CTerrainLight::CreateLightTex()
{
	if (m_pLightTex != NULL)
	{
		return m_pLightTex;
	}
	
	m_pLightTex = CORE_NEW(CTexLightMap);
	
	// 贴图尺寸
	size_t rows = 8;
	size_t cols = 8;

	m_pLightTex->SetRows(rows);
	m_pLightTex->SetCols(cols);
	
	//if (!m_pLightTex->CreateTex(m_pTerrain->GetRender()))
	if (!m_pLightTex->CreateTex(m_pRender))
	{
		CORE_TRACE("(CTerrainLight::CreateLightTex)create failed");
		return NULL;
	}
	
	m_pLightTex->WriteValue(0xFFC0C0C0, rows, cols);

	m_bIsNull = true;
	
	return m_pLightTex;
}

void CTerrainLight::ReleaseLightTex()
{
	if (m_pLightTex)
	{
		CORE_DELETE(m_pLightTex);
		m_pLightTex = NULL;
	}
}

bool CTerrainLight::BuildNull()
{
	return true;
}

bool CTerrainLight::BuildTexByColors(size_t rows, size_t cols, 
	const unsigned int* colors)
{
	Assert(colors != NULL);

	ReleaseLightTex();
	
	m_pLightTex = CORE_NEW(CTexLightMap);
	m_pLightTex->SetRows(rows);
	m_pLightTex->SetCols(cols);

	//if (!m_pLightTex->CreateTex(m_pTerrain->GetRender()))
	if (!m_pLightTex->CreateTex(m_pRender))
	{
		CORE_TRACE("(CTerrainLight::BuildTexByColors)create failed");
		return false;
	}
	
	if (!m_pLightTex->WriteColor32(colors, rows, cols))
	{
		CORE_TRACE("(CTerrainLight::BuildTexByColors)write color failed");
		return false;
	}

	m_bIsNull = false;
	m_bIsDesign = false;

	CreateDesignTex();

	return true;
}

bool CTerrainLight::BuildTex(ITexture* pTex)
{
	Assert(pTex != NULL);

	ReleaseLightTex();

	m_pLightTex = CORE_NEW(CTexLightMap);
	m_pLightTex->SetRows(m_nRows);
	m_pLightTex->SetCols(m_nCols);
 

	if (!m_pLightTex->BuildTex(m_pRender, pTex))
	{
		CORE_TRACE("(CTerrainLight::BuildTex)create failed");
		return false;
	}

	m_bIsNull = false;
	m_bIsDesign = false;

	return true;
}

bool CTerrainLight::BuildBlendColors(size_t rows, size_t cols, 
	const unsigned int* colors)
{
	Assert(colors != NULL);
	
	if ((rows != m_nRows) || (cols != m_nCols))
	{
		return false;
	}

	unsigned int* pColors = GetBlendColors();

	memcpy(pColors, colors, rows * cols * sizeof(unsigned int));

	// 设置为编辑模式下的光贴图
	m_bIsDesign = false;

	CreateDesignTex();
	
	return true;
}
 