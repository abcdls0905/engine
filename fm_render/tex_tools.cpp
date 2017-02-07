#include "../public/core_file.h"
#include "../public/portable.h"
#include "render.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#ifdef _WIN32
	#include <malloc.h>
#endif
#include "libpvr.h"
#include "tex_tools.h"
#include "libpvr.h"
 
#define FREEIMAGE_LIB
#include "freeimage.h"
 
#include "ddsfile.h"

#ifndef FX_SYSTEM_IOS
#include "ktx20/ktx.h"
#endif

#include "device_caps.h"
namespace TexTools
{
	IRender* g_render;
};

#ifndef FX_SYSTEM_IOS
	#include "nv_global.h"
	
	extern ICore* g_coree;
	extern fm_uint NvCreateTextureFromDDSEx(const char* filename, NvBool flipVertical, NvBool useMipmaps, NvS32* width, NvS32* height, NvBool* alpha, NvBool *isCubeMap);

	bool IsSupportDDS(Render* pRender)
	{
		ICore* core =pRender->GetCore();
		DeviceCaps* device_caps = (DeviceCaps*)core->GetEntity(pRender->GetDeviceCapsID());
		return device_caps->IsDDSSupported();
	}

	bool g_is_dds_support = false;
#endif

bool g_is_npotsupported = false;

void FreeImage_OutputMessage(FREE_IMAGE_FORMAT fif, const char *msg)
{
	char buf[256];
	sprintf(buf, "freeimage: %d, %s", fif, msg);
	CORE_TRACE(buf);
}

void TexTools_Init(Render* pRender)
{
	FreeImage_SetOutputMessageStdCall(FreeImage_OutputMessage);
	TexTools::g_render = pRender;
#ifndef FX_SYSTEM_IOS
	g_is_dds_support = IsSupportDDS(pRender);
	
#endif
	ICore* core = pRender->GetCore();
	DeviceCaps* device_caps = (DeviceCaps*)core->GetEntity(pRender->GetDeviceCapsID());
	g_is_npotsupported = device_caps->IsTextureNPOTSupported();
}

// PVR TO TEX_FORMAT
// 提供PVR 的内建格式和TEX_FORMAT之间相互转化函数
TEX_FORMAT PVR_FormatToTEX_Format(PVRTuint32 dwPVRflags)
{
	TEX_FORMAT tex_format;
	PVRTuint32 nformat= dwPVRflags & PVRTEX_PIXELTYPE;

	switch(nformat)
	{
	case OGL_RGBA_4444:
		tex_format = TEX_FORMAT_RGBA4444_UNORM;
		break;
	case OGL_RGBA_8888:
		tex_format = TEX_FORMAT_RGB888_UNORM;
		break;
	case OGL_RGB_565:
		tex_format = TEX_FORMAT_RGB565_UNORM;
		break;
	default:
		tex_format= TEX_FORMAT_UNKNOWN;
		break;
	};
	return tex_format;
}

//给rgba格式
inline void trans_rgba(unsigned char* bits, int width, int height)
{
	int size = width*height;
	
	for(int i = 0; i < size; i++)
	{
		char tmp = bits[0];
		bits[0] = bits[2];
		bits[2] = tmp;
		bits += 4;
	}
}

inline void trans_rgb(unsigned char* bits, int width, int height)
{
	int size = width*height;
	
	for(int i = 0; i < size; i++)
	{
		char tmp = bits[0];
		bits[0] = bits[2];
		bits[2] = tmp;
		bits += 3;
	}
}

inline bool resize(unsigned int & d)
{
	float value = (float)d;
	float n = log(value)/log(2.0f);
	if(n - (int)n > 0)
	{
		d = (unsigned int)pow(2.0f, (float)((int)n) +1); 
		return true;
	}
	return false;
}


inline bool resize(unsigned int& width, unsigned int& height)
{
	bool ret = false;
	if(resize(width))
		ret = true;
	if(resize(height))
		ret = true;
	return ret;
}

inline int LoadFile(const char* filename, char* & buf)
{
	FILE* fp = core_file::fopen(filename, "rb");
	if(fp == NULL)
		return 0;
	core_file::fseek(fp, 0, SEEK_END);
	size_t dds_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);
	
	IFileSys* pFileSys = g_pCore->GetFileSys();
	// 尝试直接获得文件数据，避免临时申请大块内存
	const void* dds_data = pFileSys->FileContent(fp);

	if (NULL == dds_data)
	{
		buf = (char*)CORE_ALLOC(dds_size);

		if (core_file::fread(buf, sizeof(unsigned char), dds_size, fp) 
			!= dds_size)
		{
			CORE_TRACE("(CTexLoader::Load)read tex data failed");
			CORE_TRACE(filename);
			core_file::fclose(fp);
			return false;
		}
	}
	core_file::fclose(fp);
	return dds_size;
}
 
bool FreeImage_LoadFromPointer(const char* file_name, const void* buffer, unsigned int size, fm_uint& texture, _IMAGE_INFO* info)
{
	info->DataSize = 0;
	enum IMAGEBPP
	{
		IB_NONE,
		IB_A8R8G8B8,
		IB_A4R4G4B4,
		IB_RGB565,
	};

	unsigned char* bits(0);

	FIBITMAP* m_dib = 0;

	FIMEMORY* m_mem = FreeImage_OpenMemory((unsigned char*)buffer, size);
	//check the file signature and deduce its format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(m_mem, size);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(file_name);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
		FreeImage_CloseMemory(m_mem);
		return false;
	}

	//check that the plugin has reading capabilities and load the file
	if(!FreeImage_FIFSupportsReading(fif))
	{
		FreeImage_CloseMemory(m_mem);
		return false;
	}
	
	m_dib = FreeImage_LoadFromMemory(fif, m_mem, 0);
	
	//if the image failed to load, return failure
	if(!m_dib)
	{
		FreeImage_CloseMemory(m_mem);
		return false;
	}

	int bpp = ::FreeImage_GetBPP(m_dib);
	
	if(bpp == 24)
	{
		FIBITMAP* olddib = m_dib;
		m_dib = FreeImage_ConvertTo16Bits565(m_dib);
		::FreeImage_Unload(olddib);
		bpp = ::FreeImage_GetBPP(m_dib);
	}
	else if(bpp < 24 || bpp >32)
	{
		FIBITMAP* olddib = m_dib;
		m_dib = ::FreeImage_ConvertTo32Bits(m_dib);
		::FreeImage_Unload(olddib);
		bpp = ::FreeImage_GetBPP(m_dib);
		if(bpp > 32)
		{
			CORE_TRACE_EX("FreeImage_LoadFromPointer WARING: pnt bpp is too much %d", bpp);
		}
	}
	
	//retrieve the image data
	bits = FreeImage_GetBits(m_dib);
	unsigned int width = FreeImage_GetWidth(m_dib);
	unsigned int height = FreeImage_GetHeight(m_dib);	

	if((bits == 0) || (width == 0) || (height == 0))
	{
		FreeImage_CloseMemory(m_mem);
		return false;
	}

	info->OriginWidth = width;
	info->OriginHeight = height;
	unsigned int full_width = width;
	unsigned int full_height = height;
	
	if(!g_is_npotsupported && resize(full_width, full_height))
	{
		FIBITMAP* srcdib = m_dib;
		m_dib = ::FreeImage_Allocate(full_width, full_height, bpp);
		FreeImage_Paste(m_dib, srcdib, 0, 0, 0xffffffff);
		bits = FreeImage_GetBits(m_dib);
		
		FreeImage_Unload(srcdib);
	}
	
	IMAGEBPP image_format = IB_A8R8G8B8;
	if(bpp < 32)
	{
		image_format = IB_RGB565;
	}
	else
	{
		int i = 0;
		int size = full_width*full_height*4;
		for ( unsigned char *point = bits; i< (int)(full_width*full_height*4) && point !=0; point += 4, i+=4)
		{

			unsigned char temp = *point;
			*point = *(point + 2);
			*(point + 2) = temp;
		}
		image_format = IB_A8R8G8B8;
	}

	FreeImage_FlipVertical(m_dib);

	int PixelFormat;
	int InternalPixelFormat;
	GLenum BufferType;
	
	switch(image_format)
	{
	case IB_A8R8G8B8:
		PixelFormat = GL_RGBA;
		InternalPixelFormat = GL_RGBA;
		BufferType = GL_UNSIGNED_BYTE;
		break;
	case IB_A4R4G4B4:
		PixelFormat = GL_RGBA;
		InternalPixelFormat = GL_RGBA;
		BufferType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
	case IB_RGB565:
		PixelFormat = GL_RGB;
		InternalPixelFormat = GL_RGB;
		BufferType = GL_UNSIGNED_SHORT_5_6_5;
		break;
	default:
		assert(0);
		break;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalPixelFormat, full_width, full_height, 0, PixelFormat, BufferType, bits);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    // 线性滤波
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // 线性滤波

	//glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(m_dib);
	FreeImage_CloseMemory(m_mem);

	if(info)
	{
		info->Width = full_width;
		info->Height = full_height;
		switch(image_format)
		{
		case IB_A8R8G8B8:
			info->Format = TEX_FORMAT_RGBA8888_UNORM;
			info->DataSize = full_width*full_height*4;
			break;
		case IB_A4R4G4B4:
			info->Format = TEX_FORMAT_RGBA4444_UNORM;
			info->DataSize = full_width*full_height*2;
			break;
		case IB_RGB565:
			info->Format = TEX_FORMAT_RGB565_UNORM;
			info->DataSize = full_width*full_height*2;
			break;
		default:
			assert(0);
			break;
		}
	}

	return true;
}

class FreeImageHandle
{
private:
	FIMEMORY* m_memory;
	FIBITMAP * m_dib;
	
	inline void Create(FIMEMORY* memory, FIBITMAP* dib)
	{
		m_memory = memory;
		m_dib = dib;
	}
public:
	FreeImageHandle()
		:m_memory(0)
		,m_dib(0)
	{
	}

	
	~FreeImageHandle()
	{
		FreeImage_Unload(m_dib);
		FreeImage_CloseMemory(m_memory);
	};

	//如果没有alpha通道，自动调整成有alpha通道的(8888)
	inline void ResizeWithAlpha()
	{
		FIBITMAP* olddib = m_dib;
		m_dib = ::FreeImage_ConvertTo32Bits(m_dib);
		::FreeImage_Unload(olddib);
	}

	inline unsigned int GetWidth()
	{
		return FreeImage_GetWidth(m_dib);
	}

	inline unsigned int GetHeight()
	{
		return FreeImage_GetHeight(m_dib);
	}

	inline int GetBPP()
	{
		return FreeImage_GetBPP(this->m_dib);
	}

	unsigned char* GetBits()
	{
		return FreeImage_GetBits(m_dib);
	}

	inline void Blend(FreeImageHandle& other)
	{
		int width = GetWidth();
		int height = GetHeight();
		int bpp = GetBPP();
		int data_size = width*height;
		unsigned char* bits = GetBits();
		other.ResizeWithAlpha();
		unsigned char* other_bits = other.GetBits();

		
		for(int i = 0; i < data_size; i++)
		{
			bits[3] = other_bits[0];
			bits += 4;
			other_bits += 4;
		}
	}

	inline void CommitGPU(GLuint& texture)
	{
		Assert(texture);
		int PixelFormat = GL_RGBA;
		int InternalPixelFormat = GL_RGBA;
		int BufferType = GL_UNSIGNED_BYTE;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalPixelFormat, GetWidth(), GetHeight(), 0, PixelFormat, BufferType, GetBits());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    // 线性滤波
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // 线性滤波
	}

	inline void Flip()
	{
		FreeImage_FlipVertical(m_dib);
	}

	inline void TransColor()
	{
		trans_rgba(GetBits(), GetWidth(), GetHeight());
	}

	//把尺寸扩展成2的整数次方
	inline void ResizeNPOT()
	{
		unsigned int width = GetWidth();
		unsigned int height = GetHeight();
		resize(width);
		resize(height);
		
		FIBITMAP* srcdib = m_dib;
		m_dib = ::FreeImage_Allocate(width, height, GetBPP());
		FreeImage_Paste(m_dib, srcdib, 0, 0, 0xffffffff);
		FreeImage_Unload(srcdib);
	}

	inline bool Load(const char* file_name, const void* buffer, unsigned int size)
	{
		unsigned char* bits(0);

		FIBITMAP* dib = 0;

		FIMEMORY* mem = FreeImage_OpenMemory((unsigned char*)buffer, size);
		//check the file signature and deduce its format
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mem, size);
		//if still unknown, try to guess the file format from the file extension
		if(fif == FIF_UNKNOWN && file_name) 
			fif = FreeImage_GetFIFFromFilename(file_name);
		//if still unkown, return failure
		if(fif == FIF_UNKNOWN)
		{
			FreeImage_CloseMemory(mem);
			return false;
		}

		//check that the plugin has reading capabilities and load the file
		if(!FreeImage_FIFSupportsReading(fif))
		{
			FreeImage_CloseMemory(mem);
			return false;
		}
		
		dib = FreeImage_LoadFromMemory(fif, mem, 0);
		
		//if the image failed to load, return failure
		if(!dib)
		{
			FreeImage_CloseMemory(mem);
			return false;
		}

		Create(mem, dib);
		return true;
	}
};

#ifdef PERFORMANCE_DEBUG
class tex_profile
{
	double m_texture_uploadgpu_time;
public:
	tex_profile()
	{
		m_texture_uploadgpu_time = Port_GetPerformanceTime();
	}

	~tex_profile()
	{
		m_texture_uploadgpu_time = Port_GetPerformanceTime() - m_texture_uploadgpu_time;
		TexTools::g_render->GetPerformance()->dTexUploadGPUTime += m_texture_uploadgpu_time;
	}
};
#endif

//软解压的方式解析各种文件格式
inline bool soft_create_texture_from_file(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info)
{
	info->DataSize = 0;
	ImageData imagedata;
	DDSFile ddsfile;
	bool ok = ddsfile.Parse(&imagedata, (const char*)data, data_size);
	
	if(ok)
	{
		GLenum target = GL_TEXTURE_2D;
		if(imagedata.iscubmap)
			target = GL_TEXTURE_CUBE_MAP;
	
		unsigned long* ptr = (unsigned long*)imagedata.data;
		glGenTextures(1, &texture);
		glBindTexture(target, texture);
		
		int PixelFormat = GL_RGBA;
		int InternalPixelFormat = GL_RGBA;
		int BufferType = GL_UNSIGNED_BYTE;
		int w = imagedata.width;
		int h = imagedata.height;
		char* dst = imagedata.data;
		if(imagedata.iscubmap)
		{
			for(int f = 0; f < 6; f++)
			{
				w = imagedata.width;
				h = imagedata.height;
				info->DataSize += w*h*4;
				for(int i = 0; i < imagedata.num_mipmaps; i++)
				{ 
					trans_rgba((unsigned char*)dst, w, h);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, i, InternalPixelFormat, w, h, 0, PixelFormat, BufferType, dst);	
					dst += w*h*sizeof(long);
					w>>=1; if(w < 1) w = 1;
					h>>=1; if(h < 1) h = 1;
				}
			}
		}
		else
		{
			for(int i = 0; i < imagedata.num_mipmaps; i++)
			{ 
				info->DataSize += w*h*4;
				trans_rgba((unsigned char*)dst, w, h);
				glTexImage2D(GL_TEXTURE_2D, i, InternalPixelFormat, w, h, 0, PixelFormat, BufferType, dst);	
				dst += w*h*sizeof(long);
				w>>=1; if(w < 1) w = 1;
				h>>=1; if(h < 1) h = 1;
			}
		}

		free(imagedata.data);

		if(imagedata.num_mipmaps > 1)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    // 线性滤波
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // 线性滤波
		}
						
		glBindTexture(target, 0);
					
		info->Format = TEX_FORMAT_RGBA8888_UNORM;
		info->Width = imagedata.width;
		info->Height = imagedata.height;
		info->OriginHeight = imagedata.height;
		info->OriginWidth = imagedata.width;
		info->MipLevels = imagedata.num_mipmaps;
		return true;
	}
	else
	{
		//DDSFILE暂时不支持的格式继续用FREEIAMGE
 		return FreeImage_LoadFromPointer(file_name, data, data_size, texture, info);	
	}
}

inline bool create_texture_from_file(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info)
{
	if(file_name == 0)
		return false;

	info->DataSize = 0;
	bool result = false;
#ifdef PERFORMANCE_DEBUG
	tex_profile _tex_profile;
#endif

#ifndef FX_SYSTEM_IOS
	if(strstr(file_name, ".ktx"))
	{
		GLenum target;
		GLboolean isMipmapped;
		GLenum glerror;
		unsigned char* pKvData;
		unsigned int  kvDataLen;
		KTX_dimensions dimensions;
	
		texture = 0;
		result = (KTX_SUCCESS == ktxLoadTextureM(data, data_size, &texture, &target, &dimensions,
							   &isMipmapped, &glerror, &kvDataLen, &pKvData));
		info->Width = dimensions.width;
		info->Height = dimensions.height;
		info->OriginHeight = dimensions.height;
		info->OriginWidth = dimensions.width;
		info->DataSize = data_size;
		if(result)
		{
 			::glBindTexture(target, texture);
			if (isMipmapped) 
				/* Enable bilinear mipmapping */
				/* TO DO: application can consider inserting a key,value pair in the KTX
				 * that indicates what type of filtering to use.
				 */
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			::glBindTexture(target,0);

			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			//glTexParameteriv(target, GL_TEXTURE_CROP_RECT_OES, iCropRect);
		}
		glBindTexture(target, 0);
		TestErr("load ktx");
		return result;
	}
#endif
	if(strstr(file_name, ".pvr"))
	{
		PVR_Texture_Header pvr_header;
		result = (PVR_SUCCESS == PVRTTextureLoadFromPointer(data, &texture, &pvr_header));
		if(info)
		{
			info->Width = pvr_header.dwWidth;
			info->Height = pvr_header.dwHeight;
			info->OriginHeight = pvr_header.dwHeight;
			info->OriginWidth = pvr_header.dwWidth;
			info->MipLevels = pvr_header.dwMipMapCount;

			PVRTuint32 nformat= pvr_header.dwpfFlags & PVRTEX_PIXELTYPE;
			info->Format = PVR_FormatToTEX_Format(nformat);
		}
	}
	else
	{
#ifndef FX_SYSTEM_IOS
		if(g_is_dds_support && strstr(file_name, ".dds"))
		{
			NvS32 width, height;
			NvBool alpha, iscubemap;
			texture = NvCreateTextureFromDDSEx(file_name, false, true, &width, &height, &alpha, &iscubemap);

			if(texture)
			{
				info->Width = width;
				info->Height = height;
				info->OriginWidth = width;
				info->OriginHeight = height;
				info->MipLevels = 0;
				info->Format = TXF_FORMAT_DDS;
				return (texture != 0);
			}
			else
			{
				//解码失败，使用软件解压的方式尝试
				result = soft_create_texture_from_file(file_name, data, data_size, texture, info);
			}
		}
		else
#endif
		{
			result = soft_create_texture_from_file(file_name, data, data_size, texture, info);
		}
	}

	return result;
}

bool CreateCubeTextureFromFile(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info)
{
	return create_texture_from_file(file_name, data, data_size, texture, info);
}

bool CreateTextureFromFile(const char* file_name, fm_uint& texture, _IMAGE_INFO* info)
{
	char* buffer;
	size_t size = LoadFile(file_name, buffer);
	if(size == 0)
		return false;

	bool result =  CreateTextureFromData(file_name, buffer, size, texture, info);
	CORE_FREE(buffer, size);
	return result;
}


bool CreateTextureFromData(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info)
{
	return create_texture_from_file(file_name, data, data_size, texture, info);
}

bool CreateTextureFromData(const char* file_name, 
								  const void* data, unsigned int data_size, 
								  const void* alpha_mask_data, unsigned int alpha_mask_size, 
								  fm_uint& texture, _IMAGE_INFO* info)
{
	FreeImageHandle handle;

	if(!handle.Load(file_name, data, data_size))
		return false;
	
	FreeImageHandle alpha_handle;
	if(!alpha_handle.Load(file_name, alpha_mask_data, alpha_mask_size))
		return false;

	if(alpha_handle.GetWidth()  == handle.GetWidth() && alpha_handle.GetHeight() == handle.GetHeight())
	{
		handle.ResizeWithAlpha();
		info->MipLevels = 0;
		info->OriginWidth = handle.GetWidth();
		info->OriginHeight = handle.GetHeight();
		handle.Blend(alpha_handle);
	}
	else
	{
		CORE_TRACE("Blend Texture Failed! ==> alpha texture size not is main texture size");
	}
	handle.TransColor();
	if(!g_is_npotsupported)
		handle.ResizeNPOT();
	handle.Flip();
	info->Width = handle.GetWidth();
	info->Height = handle.GetHeight();
	info->DataSize = info->OriginWidth * info->OriginHeight;
	handle.CommitGPU(texture);
	return true;
}

#define FREEIMAGE_LIB
#include "freeimage.h"

bool SaveScreenToFile(const char* name, int x, int y, int w, int h)
{
	Assert(name != NULL);

	const char* dot = strrchr(name, '.');

	if (NULL == dot)
	{
		return false;
	}

	FREE_IMAGE_FORMAT format;

	if (stricmp(dot, ".bmp") == 0)
	{
		format = FIF_BMP;
	}
	else if (stricmp(dot, ".jpg") == 0)
	{
		format = FIF_JPEG;
	}
	else if (stricmp(dot, ".png") == 0)
	{
		format = FIF_PNG;
	}
	else if (stricmp(dot, ".dds") == 0)
	{
		format = FIF_DDS;
	}
	else
	{
		return false;
	}

	FIBITMAP * bitmap = ::FreeImage_Allocate(w, h, 32);
		
	unsigned char* bits = FreeImage_GetBits(bitmap);
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	::glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (void*)bits);
	bool result = TestErr("SaveScreenToFile")==0;
	if(format == FIF_JPEG)
	{
		FIBITMAP *old_dib = bitmap;
		bitmap = FreeImage_ConvertTo24Bits(bitmap);
		FreeImage_Unload(old_dib);
		bits = FreeImage_GetBits(bitmap);
		trans_rgb(bits, w, h);
	}
	else
	{	
		trans_rgba(bits, w, h);
	}
	
	char buf[256];

	
	if(!FreeImage_Save(format, bitmap, name))
	{
		sprintf(buf, "SaveScreenToFile error ==> can't save to %s", name);
		CORE_TRACE(buf);
	}
	
	FreeImage_Unload(bitmap);
	return result;
}

//从文件中提取alpha通道
bool CreateTextureAlphaFromData(const char* file_name, const void* data, unsigned int data_size, _ALPHA_INFO* info)
{
	unsigned char* bits(0);

	FIBITMAP* dib = 0;

	FIMEMORY* mem = FreeImage_OpenMemory((unsigned char*)data, data_size);
	//check the file signature and deduce its format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mem, data_size);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(file_name);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
		FreeImage_CloseMemory(mem);
		return false;
	}

	//check that the plugin has reading capabilities and load the file
	if(!FreeImage_FIFSupportsReading(fif))
	{
		FreeImage_CloseMemory(mem);
		return false;
	}
	
	dib = FreeImage_LoadFromMemory(fif, mem, 0);
	
	//if the image failed to load, return failure
	if(!dib)
	{
		FreeImage_CloseMemory(mem);
		return false;
	}

	int bpp = ::FreeImage_GetBPP(dib);
	
	if(bpp != 32)//有alpha
	{
		FreeImage_Unload(dib);
		FreeImage_CloseMemory(mem);
		return false;
	}

	 
	FreeImage_FlipVertical(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);
	bits = FreeImage_GetBits(dib);

	int size = width*height;
	unsigned char* p = (unsigned char*)malloc(size);
	for(int i = 0; i < size; i++)
	{
		p[i] = bits[3];
		bits += 4;
	}

	info->Data = p;
	info->Width = width;
	info->Height = height;
	info->DataSize = size;
	FreeImage_Unload(dib);
	FreeImage_CloseMemory(mem);

	return p;
}

