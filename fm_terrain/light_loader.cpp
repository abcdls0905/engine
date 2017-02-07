//--------------------------------------------------------------------
// 文件名:		light_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "light_loader.h"
#include "terrain_zone.h"
#include "terrain.h"
#include "terrain_light.h"
//#include "../visual/tga_file.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../visual/i_image_access.h"

//#include "../fm_render/tex_tools.h"

// CLightLoader

CLightLoader* CLightLoader::NewInstance()
{
	return CORE_NEW(CLightLoader);
}

CLightLoader::CLightLoader()
{
	m_pOwner = NULL;
	m_pRender = NULL;
	m_nRows = 0;
	m_nCols = 0;
	m_nBmpWidth = 0;
	m_nBmpHeight = 0;
	m_pBmpColors = NULL;
	m_pTex = NULL;
	m_nBlendRows = 0;
	m_nBlendCols = 0;
	m_pBlendColors = NULL;
}

CLightLoader::~CLightLoader()
{
	if (m_pBmpColors)
	{
		CORE_FREE(m_pBmpColors, 
			sizeof(unsigned int) * m_nBmpWidth * m_nBmpHeight);
	}
	
	if (m_pBlendColors)
	{
		CORE_FREE(m_pBlendColors, 
			sizeof(unsigned int) * m_nBlendRows * m_nBlendCols);
	}

#ifdef LOG_DX_RESOURCE
	if (m_pTex)
	{
		char info[256];
		SafeSprintf(info, sizeof(info),
			"(CLightLoader::~CLightLoader)[DxLog]texture pointer:%p released",
			m_pTex);
		CORE_TRACE(info);
	}
#endif 

	SAFE_RELEASE(m_pTex);
}

void CLightLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CLightLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	m_pOwner = pOwner;
	m_pRender = pOwner->GetTerrain()->GetRender();

	CTerrainLight* pLight = pOwner->GetLight();

	m_nRows = pLight->GetRows();
	m_nCols = pLight->GetCols();
}

void CLightLoader::SetBitmap(const char* file_name)
{
	Assert(file_name != NULL);

	m_strBitmap = file_name;
}

void CLightLoader::SetTexFile(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strTexFile = file_name;
}

void CLightLoader::SetBlendFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strBlendFile = file_name;
}


#if defined FX_SYSTEM_IOS || defined __ANDROID__

typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned long   bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
        unsigned long      biSize;
        long       biWidth;
        long       biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
} BITMAPINFOHEADER;

#define BI_RGB        0L

#endif



bool CLightLoader::LoadBitmapFile(const char* file_name)
{
	Assert(file_name != NULL);
	
	FILE* fp = core_file::fopen(file_name, "rb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	BITMAPFILEHEADER bmh;
	
	if (core_file::fread(&bmh, sizeof(bmh), 1, fp) != 1)
	{
		CORE_TRACE("(CLightLoader::LoadBitmapFile)read header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	BITMAPINFOHEADER bmi;
	
	if (core_file::fread(&bmi, sizeof(bmi), 1, fp) != 1)
	{
		CORE_TRACE("(CLightLoader::LoadBitmapFile)read info header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	if (bmi.biBitCount != 32)
	{
		CORE_TRACE("(CLightLoader::LoadBitmapFile)not 32 bit");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	// 翻转图像
	size_t width = bmi.biWidth;
	size_t height = bmi.biHeight;
	unsigned int* values = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * width * height);
	unsigned int* pdata = values + width * height;
	
	for (size_t k = 0; k < height; k++)
	{
		pdata -= width;

		if (core_file::fread(pdata, sizeof(unsigned int), width, fp) != width)
		{
			CORE_TRACE("(CLightLoader::LoadBitmapFile)read data error");
			CORE_TRACE(file_name);
			CORE_FREE(values, sizeof(unsigned int) * width * height);
			core_file::fclose(fp);
			return false;
		}
	}
	
	core_file::fclose(fp);
	
	m_nBmpWidth = bmi.biWidth;
	m_nBmpHeight = bmi.biWidth;
	m_pBmpColors = values;

	return true;
}

bool CLightLoader::LoadDDSFile(const char* file_name)
{
	Assert(file_name != NULL);

 
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fclose(fp);
 

	m_pTex = m_pRender->CreateTexture(file_name,true);
	return true;
}


bool CLightLoader::LoadBlendFile(const char* file_name)
{	
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}


	tga_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CLightLoader::LoadBlendFile)reader header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	if (header.bpp != 32)
	{
		CORE_TRACE("(CLightLoader::LoadBlendFile)bpp error");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	// 是否需要翻转
	bool need_reverse = (header.origin == 0);
	size_t width = header.width;
	size_t height = header.height;
	size_t pixels = width * height;

	m_pBlendColors = (unsigned int*)CORE_ALLOC(sizeof(unsigned int) * pixels);
	m_nBlendRows = header.height;
	m_nBlendCols = header.width;

	if (need_reverse)
	{
		unsigned int* pdata = m_pBlendColors + pixels;

		for (size_t k = 0; k < height; ++k)
		{
			if (need_reverse)
			{
				pdata -= width;
			}

			if (core_file::fread(pdata, sizeof(unsigned int), width, fp) 
				!= width)
			{
				CORE_TRACE("(CLightLoader::LoadBlendFile)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else
	{
		if (core_file::fread(m_pBlendColors, sizeof(unsigned int), pixels, fp) 
			!= pixels)
		{
			CORE_TRACE("(CLightLoader::LoadBlendFile)read data error");
			CORE_TRACE(file_name);
			core_file::fclose(fp);
			return false;
		}
	}

	core_file::fclose(fp);

	return true;
}

const char* CLightLoader::GetName()
{
	if (m_strTexFile.empty())
	{
		return m_strBitmap.c_str();
	}
	else
	{
		return m_strTexFile.c_str();
	}
}

bool CLightLoader::Load(bool async)
{
	if (!m_strBitmap.empty())
	{
		// 加载光照位图文件（调试用）
		LoadBitmapFile(m_strBitmap.c_str());
	}
	
	if (!m_strTexFile.empty())
	{
		// 加载光贴图文件
		return LoadDDSFile(m_strTexFile.c_str());
	}
	
	if (!m_strBlendFile.empty())
	{
		// 加载混合颜色文件
		LoadBlendFile(m_strBlendFile.c_str());
	}

	return true;
}

bool CLightLoader::Create(bool async)
{
	CTerrainLight* pLight = m_pOwner->GetLight();
	bool build_tex = false;

	if (m_pBmpColors)
	{
		if (!pLight->BuildTexByColors(m_nBmpHeight, m_nBmpWidth, 
			m_pBmpColors))
		{
			CORE_TRACE("(CLightLoader::Create)build texture from bmp failed");
			CORE_TRACE(m_strBitmap.c_str());
		}
		else
		{
			build_tex = true;
		}
	}
	else
	{
		if ((m_pTex != NULL) && (!build_tex))
		{
			build_tex = pLight->BuildTex(m_pTex);

			if (build_tex)
			{
				// 数据所有权已经转移
				m_pTex = NULL;
			}
		}
	}

	if (!build_tex)
	{
		// 否则创建全为空白的顶点色
		if (!pLight->BuildNull())
		{
			return false;
		}
	}

	if (m_pBlendColors)
	{
		pLight->BuildBlendColors(m_nBlendRows, m_nBlendCols, m_pBlendColors);
	}
	
	m_pOwner->SetLightLoaded(true);
	
	return true;
}

bool CLightLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

bool CLightLoader::SaveBlendColors(const char* file_name, CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);

	CTerrainLight* pLight = pZone->GetLight();
	size_t rows = pLight->GetRows();
	size_t cols = pLight->GetCols();
	unsigned int* colors = pLight->GetBlendColors();
	IImageAccess* pImage = 
		pZone->GetTerrain()->GetRender()->CreateImageAccess();

	pImage->SetFormat(cols, rows, 32);
	pImage->SetData(colors, rows * cols * sizeof(unsigned int));

	bool succeed = pImage->SaveFile(file_name);

	pImage->Release();
	
	return succeed;
}

