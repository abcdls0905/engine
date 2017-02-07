//--------------------------------------------------------------------
// 文件名:		image_access.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年4月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "image_access.h"
#include "render.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"



#pragma pack(push, 1)

struct tga_header_t
{
	unsigned char byte1;		// = 0
	unsigned char byte2;		// = 0
	unsigned char type;
	unsigned char byte4_9[9];	// = 0
	unsigned short width;
	unsigned short height;
	unsigned char bpp;			// bit per pixel
	union
	{
		unsigned char attr8;
		struct
		{
			unsigned char attr: 4;
			unsigned char rez: 1;
			unsigned char origin: 1;
			unsigned char storage: 2;
		};
	};
};
	
#pragma pack(pop)

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


// CImageAccess

IImageAccess* CImageAccess::NewInstance(Render* pRender)
{
	CImageAccess* p = (CImageAccess*)CORE_ALLOC(sizeof(CImageAccess));

	new (p) CImageAccess(pRender);

	return p;
}

CImageAccess::CImageAccess(Render* pRender)
{
	Assert(pRender != NULL);

	m_pRender = pRender;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBitPerPixel = 0;
	m_pData = NULL;
	m_nSize = 0;
}

CImageAccess::~CImageAccess()
{
	ReleaseAll();
}

void CImageAccess::ReleaseAll()
{
	if (m_pData)
	{
		CORE_FREE(m_pData, m_nSize);
		m_pData = NULL;
		m_nSize = 0;
	}
}

void CImageAccess::Release()
{
	CORE_DELETE(this);
}

unsigned int CImageAccess::GetWidth()
{
	return m_nWidth;
}

unsigned int CImageAccess::GetHeight()
{
	return m_nHeight;
}

unsigned int CImageAccess::GetBitPerPixel()
{
	return m_nBitPerPixel;
}

unsigned char* CImageAccess::GetData()
{
	return m_pData;
}

bool CImageAccess::SetFormat(unsigned int width, unsigned int height, 
	unsigned int bit_per_pixel)
{
	Assert(width > 0);
	Assert(height > 0);
	Assert((bit_per_pixel == 8) || (bit_per_pixel == 16)
		|| (bit_per_pixel == 24) || (bit_per_pixel == 32));
	
	ReleaseAll();
	
	m_nWidth = width;
	m_nHeight = height;
	m_nBitPerPixel = bit_per_pixel;

	unsigned int bytes = width * height* (bit_per_pixel / 8);

	m_pData = (unsigned char*)CORE_ALLOC(bytes);
	memset(m_pData, 0, bytes);
	m_nSize = bytes;

	return true;
}

bool CImageAccess::SetData(const void* pdata, size_t size)
{
	Assert(pdata != NULL);

	size_t bytes = m_nWidth * m_nHeight* (m_nBitPerPixel / 8);

	if (size != bytes)
	{
		return false;
	}
	
	memcpy(m_pData, pdata, bytes);

	return true;
}

bool CImageAccess::ConvertToARGB()
{
	if (NULL == m_pData)
	{
		return false;
	}

	if (m_nBitPerPixel == 32)
	{
		return true;
	}

	unsigned int pixels = m_nWidth * m_nHeight;
	unsigned char* values = (unsigned char*)CORE_ALLOC(pixels * 4);

	if (m_nBitPerPixel == 8)
	{
		unsigned char* p = values;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int gray = m_pData[i];

			*p++ = gray;
			*p++ = gray;
			*p++ = gray;
			*p++ = 0xFF;
		}
	}
	else if (m_nBitPerPixel == 16)
	{
		unsigned char* p = values;
		unsigned int k = 0;

		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int red = 0, green = 0, blue = 0;
			unsigned int low = m_pData[k];
			unsigned int high = m_pData[k + 1];
			int color = ((high << 8) & 0xFF00) | (low & 0x00FF);

			red = (color & 0xF800) >> 8;
			green = (color & 0x07E0) >> 3;
			blue = (color & 0x001F) << 3;
			*p++ = blue;
			*p++ = green;
			*p++ = red;
			*p++ = 0xFF;
			k += 2;
		}
	}
	else if (m_nBitPerPixel == 24)
	{
		unsigned char* p = values;
		unsigned int k = 0;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int blue = m_pData[k];
			unsigned int green = m_pData[k + 1];
			unsigned int red = m_pData[k + 2];
			
			*p++ = blue;
			*p++ = green;
			*p++ = red;
			*p++ = 0xFF;
			k += 3;
		}
	}
	else
	{
		return false;
	}
	
	CORE_FREE(m_pData, m_nSize);
	m_pData = values;
	m_nSize = pixels * 4;
	m_nBitPerPixel = 32;
	
	return true;
}

bool CImageAccess::ConvertToRGB()
{
	if (NULL == m_pData)
	{
		return false;
	}
	
	if (m_nBitPerPixel == 24)
	{
		return true;
	}
	
	unsigned int pixels = m_nWidth * m_nHeight;
	unsigned char* values = (unsigned char*)CORE_ALLOC(pixels * 3);
	
	if (m_nBitPerPixel == 8)
	{
		unsigned char* p = values;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int gray = m_pData[i];
			
			*p++ = gray;
			*p++ = gray;
			*p++ = gray;
		}
	}
	else if (m_nBitPerPixel == 32)
	{
		unsigned char* p = values;
		unsigned int k = 0;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int blue = m_pData[k];
			unsigned int green = m_pData[k + 1];
			unsigned int red = m_pData[k + 2];
			
			*p++ = blue;
			*p++ = green;
			*p++ = red;
			k += 4;
		}
	}
	else
	{
		return false;
	}
	
	CORE_FREE(m_pData, m_nSize);
	m_pData = values;
	m_nSize = pixels * 3;
	m_nBitPerPixel = 24;
	
	return true;
}

bool CImageAccess::ConvertToGray8()
{
	if (NULL == m_pData)
	{
		return false;
	}
	
	if (m_nBitPerPixel == 8)
	{
		return true;
	}
	
	unsigned int pixels = m_nWidth * m_nHeight;
	unsigned char* values = (unsigned char*)CORE_ALLOC(pixels);
	
	if (m_nBitPerPixel == 32)
	{
		unsigned char* p = values;
		unsigned int k = 0;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int blue = m_pData[k];
			unsigned int green = m_pData[k + 1];
			unsigned int red = m_pData[k + 2];
			
			*p++ = (blue + green + red) / 3;
			k += 4;
		}
	}
	else if (m_nBitPerPixel == 24)
	{
		unsigned char* p = values;
		unsigned int k = 0;
		
		for (unsigned int i = 0; i < pixels; ++i)
		{
			unsigned int blue = m_pData[k];
			unsigned int green = m_pData[k + 1];
			unsigned int red = m_pData[k + 2];
			
			*p++ = (blue + green + red) / 3;
			k += 3;
		}
	}
	else
	{
		return false;
	}
	
	CORE_FREE(m_pData, m_nSize);
	m_pData = values;
	m_nSize = pixels;
	m_nBitPerPixel = 8;
	
	return true;
}

bool CImageAccess::LoadFile(const char* file_name)
{
	Assert(file_name != NULL);
	
	const char* dot = strrchr(file_name, '.');

	if (NULL == dot)
	{
		CORE_TRACE("(CImageAccess::LoadFile)file extension not found");
		CORE_TRACE(file_name);
		return false;
	}

	if (stricmp(dot + 1, "bmp") == 0)
	{
		return LoadBmp(file_name);
	}
	else if (stricmp(dot + 1, "tga") == 0)
	{
		return LoadTga(file_name);
	}
	else if (stricmp(dot + 1, "dds") == 0)
	{
		return LoadDDS(file_name);
	}
	else
	{
		CORE_TRACE("(CImageAccess::LoadFile)unknown file type");
		CORE_TRACE(file_name);
	}
	
	return false;
}

bool CImageAccess::LoadTga(const char* file_name)
{
	Assert(file_name != NULL);
	
	ReleaseAll();
	
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		CORE_TRACE("(CImageAccess::LoadTga)open file failed");
		CORE_TRACE(file_name);
		return false;
	}

	tga_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CImageAccess::LoadTga)reader header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	if ((header.bpp != 8) && (header.bpp != 16) && (header.bpp != 24) 
		&& (header.bpp != 32))
	{
		CORE_TRACE("(CImageAccess::LoadTga)bpp error");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	// 是否需要翻转
	bool need_reverse = (header.origin == 0);
	unsigned int width = header.width;
	unsigned int height = header.height;
	unsigned int pixels = width * height;

	if (header.bpp == 8)
	{
		m_pData = (unsigned char*)CORE_ALLOC(pixels);
		m_nSize = pixels;

		if (need_reverse)
		{
			unsigned char* pdata = m_pData + pixels;

			for (unsigned int k = 0; k < height; k++)
			{
				if (need_reverse)
				{
					pdata -= width;
				}
				
				if (core_file::fread(pdata, sizeof(unsigned char), width, fp) 
					!= width)
				{
					CORE_TRACE("(CImageAccess::LoadTga)read data error");
					CORE_TRACE(file_name);
					core_file::fclose(fp);
					return false;
				}
			}
		}
		else
		{
			if (core_file::fread(m_pData, sizeof(unsigned char), pixels, fp) 
				!= pixels)
			{
				CORE_TRACE("(CImageAccess::LoadTga)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else if (header.bpp == 16)
	{
		m_pData = (unsigned char*)CORE_ALLOC(pixels * 2);
		m_nSize = pixels * 2;

		if (need_reverse)
		{
			unsigned char* pdata = m_pData + pixels * 2;

			for (unsigned int k = 0; k < height; k++)
			{
				if (need_reverse)
				{
					pdata -= width * 2;
				}

				if (core_file::fread(pdata, sizeof(unsigned char), width * 2, 
					fp) != width * 2)
				{
					CORE_TRACE("(CImageAccess::LoadTga)read data error");
					CORE_TRACE(file_name);
					core_file::fclose(fp);
					return false;
				}
			}
		}
		else
		{
			if (core_file::fread(m_pData, sizeof(unsigned char), pixels * 2, 
				fp) != pixels * 2)
			{
				CORE_TRACE("(CImageAccess::LoadTga)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else if (header.bpp == 24)
	{
		m_pData = (unsigned char*)CORE_ALLOC(pixels * 3);
		m_nSize = pixels * 3;
		
		if (need_reverse)
		{
			unsigned char* pdata = m_pData + pixels * 3;
			
			for (unsigned int k = 0; k < height; k++)
			{
				if (need_reverse)
				{
					pdata -= width * 3;
				}
				
				if (core_file::fread(pdata, sizeof(unsigned char), width * 3, 
					fp) != width * 3)
				{
					CORE_TRACE("(CImageAccess::LoadTga)read data error");
					CORE_TRACE(file_name);
					core_file::fclose(fp);
					return false;
				}
			}
		}
		else
		{
			if (core_file::fread(m_pData, sizeof(unsigned char), pixels * 3, 
				fp) != pixels * 3)
			{
				CORE_TRACE("(CImageAccess::LoadTga)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else
	{
		m_pData = (unsigned char*)CORE_ALLOC(pixels * 4);
		m_nSize = pixels * 4;
		
		if (need_reverse)
		{
			unsigned char* pdata = m_pData + pixels * 4;
			
			for (unsigned int k = 0; k < height; k++)
			{
				if (need_reverse)
				{
					pdata -= width * 4;
				}
				
				if (core_file::fread(pdata, sizeof(unsigned char), width * 4, 
					fp) != width * 4)
				{
					CORE_TRACE("(CImageAccess::LoadTga)read data error");
					CORE_TRACE(file_name);
					core_file::fclose(fp);
					return false;
				}
			}
		}
		else
		{
			if (core_file::fread(m_pData, sizeof(unsigned char), pixels * 4, 
				fp) != pixels * 4)
			{
				CORE_TRACE("(CImageAccess::LoadTga)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
		
	core_file::fclose(fp);

	m_nWidth = header.width;
	m_nHeight = header.height;
	m_nBitPerPixel = header.bpp;

	return true;
}

bool CImageAccess::LoadBmp(const char* file_name)
{
	Assert(file_name != NULL);
	
	ReleaseAll();
	
	FILE* fp = core_file::fopen(file_name, "rb");
	
	if (NULL == fp)
	{
		CORE_TRACE("(CImageAccess::LoadBmp)open file failed");
		CORE_TRACE(file_name);
		return false;
	}
	
	BITMAPFILEHEADER bmh;
	
	if (core_file::fread(&bmh, sizeof(bmh), 1, fp) != 1)
	{
		CORE_TRACE("((CImageAccess::LoadBmp))read header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	BITMAPINFOHEADER bmi;
	
	if (core_file::fread(&bmi, sizeof(bmi), 1, fp) != 1)
	{
		CORE_TRACE("(CImageAccess::LoadBmp)read info header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	if ((bmi.biBitCount != 24) && (bmi.biBitCount != 32))
	{
		CORE_TRACE("(CImageAccess::LoadBmp)not 24 or 32 bit");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	// 翻转图像
	unsigned int width = bmi.biWidth;
	unsigned int height = bmi.biHeight;
	unsigned int pixels = width * height;
	
	if (bmi.biBitCount == 32)
	{
		m_pData = (unsigned char*)CORE_ALLOC(pixels * 4);
		m_nSize = pixels * 4;

		unsigned char* pdata = m_pData + pixels * 4;
		
		for (unsigned int k = 0; k < height; k++)
		{
			pdata -= width * 4;
			
			if (core_file::fread(pdata, sizeof(unsigned char), width * 4, fp) 
				!= width * 4)
			{
				CORE_TRACE("(CImageAccess::LoadBmp)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else
	{
		// 32BIT对齐
		unsigned int row_bytes = (width * 3 + 3) / 4 * 4;
		TAutoMem<unsigned char, 256, TCoreAlloc> auto_buf(row_bytes);
		unsigned char* row_data = auto_buf.GetBuffer();
		
		m_pData = (unsigned char*)CORE_ALLOC(pixels * 3);
		m_nSize = pixels * 3;
		
		unsigned char* pdata = m_pData + pixels * 3;
		
		for (unsigned int k = 0; k < height; k++)
		{
			pdata -= width * 3;
			
			if (core_file::fread(row_data, sizeof(unsigned char), row_bytes, 
				fp) != row_bytes)
			{
				CORE_TRACE("(CImageAccess::LoadBmp)read data error");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}

			memcpy(pdata, row_data, width * 3);
		}
	}
	
	core_file::fclose(fp);

	m_nWidth = bmi.biWidth;
	m_nHeight = bmi.biHeight;
	m_nBitPerPixel = bmi.biBitCount;

	return true;
}

bool CImageAccess::LoadDDS(const char* file_name)
{
	/*
	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t dds_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<unsigned char, 1024, TCoreAlloc> auto_buf(dds_size);
	unsigned char* dds_data = auto_buf.GetBuffer();

	if (core_file::fread(dds_data, sizeof(unsigned char), dds_size, fp) 
		!= dds_size)
	{
		CORE_TRACE("(CImageAccess::LoadDDS)read dds data failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);

	D3DX11_IMAGE_INFO image_info;

	HRESULT hr = D3DX11GetImageInfoFromMemory(dds_data, dds_size, NULL, 
		&image_info, NULL);

	if (FAILED(hr))
	{
		char info[256];
		SafeSprintf(info, sizeof(info), "(CImageAccess::LoadDDS)[hr:%d]"
			"get image info failed! file = %s, size = %d", HRESULT_CODE(hr), 
			file_name, dds_size);
		CORE_TRACE(info);
		return false;
	}

	if (image_info.ResourceDimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), "(CImageAccess::LoadDDS)"
			"texture format error! file = %s, size = %d", file_name, dds_size);
		CORE_TRACE(info);
		return false;
	}

	if ((image_info.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
		&& (image_info.Format != DXGI_FORMAT_B8G8R8A8_UNORM))
	{
		char info[256];
		SafeSprintf(info, sizeof(info), "(CImageAccess::LoadDDS)"
			"texture format error! file = %s, size = %d", file_name, dds_size);
		CORE_TRACE(info);
		return false;
	}

	D3DX11_IMAGE_LOAD_INFO ili;

	ili.Width = image_info.Width;
	ili.Height = image_info.Height;
	ili.Depth = image_info.Depth;
	ili.FirstMipLevel = 0;
	ili.MipLevels = 1;
	ili.Usage = D3D11_USAGE_STAGING;
	ili.BindFlags = 0;
	ili.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
	ili.MiscFlags = 0;
	ili.Format = image_info.Format;
	ili.Filter = D3DX11_FILTER_BOX;
	ili.MipFilter = D3DX11_FILTER_NONE;
	ili.pSrcInfo = &image_info;

	ID3D11Device* pDevice = m_pRender->GetDevice();
	// 创建在系统内存里的贴图
	ID3D11Resource* pResource = NULL;

	hr = D3DX11CreateTextureFromMemory(pDevice,	dds_data, dds_size, &ili, 
		NULL, &pResource, NULL);

	if (FAILED(hr))
	{
		CORE_TRACE("(CImageAccess::LoadDDSData)create trace texture failed");
		CORE_TRACE(file_name);
		return false;
	}

	ID3D11DeviceContext* pDeviceContext = m_pRender->GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_data;

	hr = pDeviceContext->Map(pResource, 0, D3D11_MAP_READ, 0, &mapped_data);

	if (FAILED(hr))
	{
		pResource->Release();
		return false;
	}

	unsigned int width = image_info.Width;
	unsigned int height = image_info.Height;

	m_pData = (unsigned char*)CORE_ALLOC(width * height * 4);
	m_nSize = width * height * 4;
	m_nWidth = width;
	m_nHeight = height;
	m_nBitPerPixel = 32;

	unsigned char* pSrc = (unsigned char*)mapped_data.pData;
	unsigned char* pDst = m_pData;

	if (image_info.Format == DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		for (size_t i = 0; i < height; ++i)
		{
			memcpy(pDst, pSrc, width * 4);
			pDst += width * 4;
			pSrc += mapped_data.RowPitch;
		}
	}
	else
	{
		for (size_t i = 0; i < height; ++i)
		{
			for (size_t k = 0; k < width; ++k)
			{
				pDst[0] = pSrc[2];
				pDst[1] = pSrc[1];
				pDst[2] = pSrc[0];
				pDst[3] = pSrc[3];
				pSrc += 4;
				pDst += 4;
			}
			
			pSrc += mapped_data.RowPitch - width * 4;
		}
	}
	
	pDeviceContext->Unmap(pResource, 0);
	pResource->Release();

	return true;
	*/
	return false;
}

bool CImageAccess::SaveFile(const char* file_name)
{
	Assert(file_name != NULL);

	const char* dot = strrchr(file_name, '.');

	if (NULL == dot)
	{
		CORE_TRACE("(CImageAccess::SaveFile)file extension not found");
		CORE_TRACE(file_name);
		return false;
	}

	if (stricmp(dot + 1, "bmp") == 0)
	{
		return SaveBmp(file_name);
	}
	else if (stricmp(dot + 1, "tga") == 0)
	{
		return SaveTga(file_name);
	}
	else
	{
		CORE_TRACE("(CImageAccess::SaveFile)unknown file type");
		CORE_TRACE(file_name);
	}

	return false;
}

bool CImageAccess::SaveTga(const char* file_name)
{
	if ((m_nBitPerPixel != 24) && (m_nBitPerPixel != 32)
		&& (m_nBitPerPixel != 8) && (m_nBitPerPixel != 16))
	{
		return false;
	}

	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		CORE_TRACE("(CImageAccess::SaveTga)open file failed");
		CORE_TRACE(file_name);
		return false;
	}

	tga_header_t header;

	memset(&header, 0, sizeof(header));

	if ((m_nBitPerPixel == 8) || (m_nBitPerPixel == 16))
	{
		header.type = 0x3;
	}
	else
	{
		header.type = 0x2;
	}

	header.width = m_nWidth;
	header.height = m_nHeight;
	header.bpp = m_nBitPerPixel;
	header.attr8 = 0x28;

	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CImageAccess::SaveTga)write header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	unsigned int bytes = m_nWidth * m_nHeight * (m_nBitPerPixel / 8);

	if (core_file::fwrite(m_pData, sizeof(unsigned char), bytes, fp) != bytes)
	{
		CORE_TRACE("(CImageAccess::SaveTga)write data failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);

	return true;
}

bool CImageAccess::SaveBmp(const char* file_name)
{
	Assert(file_name != NULL);
	Assert(m_pData != NULL);

	if ((m_nBitPerPixel != 24) && (m_nBitPerPixel != 32))
	{
		return false;
	}
	
	FILE* fp = core_file::fopen(file_name, "wb");

	if (NULL == fp)
	{
		CORE_TRACE("(CImageAccess::SaveBmp)open file failed");
		CORE_TRACE(file_name);
		return false;
	}

	unsigned int pixel_bytes = m_nBitPerPixel / 8;

	BITMAPFILEHEADER bmh;

	memset(&bmh, 0, sizeof(bmh));
	bmh.bfType = 'MB';
	bmh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmh.bfSize = bmh.bfOffBits + m_nWidth * m_nHeight * pixel_bytes;

	if (core_file::fwrite(&bmh, sizeof(bmh), 1, fp) != 1)
	{
		CORE_TRACE("(CImageAccess::SaveBmp)write header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	BITMAPINFOHEADER bmi;

	memset(&bmi, 0, sizeof(bmi));
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = m_nWidth;
	bmi.biHeight = m_nHeight;
	bmi.biPlanes = 1;
	bmi.biBitCount = m_nBitPerPixel;
	bmi.biCompression = BI_RGB;
	bmi.biXPelsPerMeter	= 0x0B12;
	bmi.biYPelsPerMeter	= 0x0B12;

	if (core_file::fwrite(&bmi, sizeof(bmi), 1, fp) != 1)
	{
		CORE_TRACE("(CImageAccess::SaveBmp)write info header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	// 翻转图像
	unsigned char* pdata = m_pData + m_nWidth * m_nHeight * pixel_bytes;

	if (m_nBitPerPixel == 24)
	{
		// 32BIT对齐
		unsigned int row_bytes = (m_nWidth * 3 + 3) / 4 * 4;
		TAutoMem<unsigned char, 256, TCoreAlloc> auto_buf(row_bytes);
		unsigned char* row_data = auto_buf.GetBuffer();

		memset(row_data, 0, row_bytes);

		for (unsigned int k = 0; k < m_nHeight; ++k)
		{
			pdata -= m_nWidth * pixel_bytes;

			memcpy(row_data, pdata, m_nWidth * pixel_bytes);

			if (core_file::fwrite(row_data, sizeof(unsigned char), row_bytes, 
				fp) != row_bytes)
			{
				CORE_TRACE("(CImageAccess::SaveBmp)write values failed");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}
	else
	{
		for (unsigned int k = 0; k < m_nHeight; ++k)
		{
			pdata -= m_nWidth * pixel_bytes;

			if (core_file::fwrite(pdata, sizeof(unsigned char), 
				m_nWidth * pixel_bytes, fp) != m_nWidth * pixel_bytes)
			{
				CORE_TRACE("(CImageAccess::SaveBmp)write values failed");
				CORE_TRACE(file_name);
				core_file::fclose(fp);
				return false;
			}
		}
	}

	core_file::fclose(fp);

	return true;
}
