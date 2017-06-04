
#include "dynamic_tex.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

CDynamicTex::CDynamicTex(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nFormat = TEX_FORMAT_NONE;
	m_nTexture = 0;
	m_pTexSampler = NULL;
	m_size = 0;
}

CDynamicTex::~CDynamicTex()
{
	m_pRender->GetMemPerformance()->nUsedDynamicTextureData -= m_size;
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}

#ifdef PROCESS_LOST_DEVICE
	if(m_pdata)
		CORE_FREE(m_pdata, m_nWidth*m_nHeight);
#endif
}

void CDynamicTex::Destroy()
{
	CORE_DELETE(this);
}

void CDynamicTex::Release()
{
	SAFE_RELEASE(m_pTexSampler);
	m_pRender->ReleaseResource(m_nIndex);
}

bool CDynamicTex::Create(unsigned int width, unsigned int height,
	unsigned int levels, TEX_FORMAT format)
{
	Assert(0 == m_nTexture);

	glGenTextures(1, &m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);

	switch(format)
	{
	case TEX_FORMAT_A8_UNORM:
		{
			m_PixelFormat = GL_ALPHA;
			m_InternalPixelFormat = GL_ALPHA;
			m_BufferType = GL_UNSIGNED_BYTE;
			m_size += width * height;
		}
		break;
	case TEX_FORMAT_RGB888_UNORM:
		{
			m_PixelFormat = GL_RGB;
			m_InternalPixelFormat = GL_RGB;
			m_BufferType = GL_UNSIGNED_BYTE;
			m_size += width * height * 3;
		}break;
	case TEX_FORMAT_RGBA4444_UNORM:
		{
			m_PixelFormat = GL_RGBA;
			m_InternalPixelFormat = GL_RGBA;
			m_BufferType = GL_UNSIGNED_SHORT_4_4_4_4;
			m_size += width * height * 2;
		}
		break;
	default:
		Assert(0);
		break;
	}

	this->m_pRender->GetMemPerformance()->nUsedDynamicTextureData += m_size;

#ifdef PROCESS_LOST_DEVICE
			int size = width * height;
			m_pdata = CORE_ALLOC(width*height);
			memset(m_pdata, 0, size);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalPixelFormat, width, height, 0, m_PixelFormat, m_BufferType, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CDynamicTex::Create)[DxLog][err:%d]glTexImage2D failed, "
			"width:%d height:%d format:%d", err, m_nWidth, m_nHeight, 
			m_nFormat);
		CORE_TRACE(info);

		if (err == GL_OUT_OF_MEMORY)
		{
			// 显存不足
			m_pRender->RaiseDeviceError(err,
				"(CDynamicTex::Create)glTexImage2D failed");
		}

		return false;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nFormat = format;

	m_pTexSampler = m_pRender->CreateTextureSamper(this,ITextureSampler::TEX_2D,ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR,
		ITextureSampler::TWM_CLAMP_TO_EDGE,ITextureSampler::TWM_CLAMP_TO_EDGE);

	return true;
}

bool CDynamicTex::Update(int level, unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata)
{

	glBindTexture(GL_TEXTURE_2D, GetTexture());
	glTexSubImage2D(GL_TEXTURE_2D, level, offset_x, offset_y, width, height,
		m_PixelFormat, m_BufferType, pdata);
	
	GLenum res = glGetError();

	if (res != GL_NO_ERROR)
	{
		CORE_TRACE("(Render::UpdateDynamicTex)update texture failed");
	}

	//glBindTexture(GL_TEXTURE_2D, 0);
#ifdef PROCESS_LOST_DEVICE
	Blit(m_pdata, offset_x, offset_y, pdata, width, height);
#endif
	
	return (res == GL_NO_ERROR);
}

#ifdef PROCESS_LOST_DEVICE
// 设备就绪时的处理
bool CDynamicTex::Restore()
{
	glGenTextures(1, &m_nTexture);
	glBindTexture(GL_TEXTURE_2D, m_nTexture);

	int PixelFormat;
	int InternalPixelFormat;
	GLenum BufferType;

	switch(m_nFormat)
	{
	case TEX_FORMAT_A8_UNORM:
		{
			PixelFormat = GL_ALPHA;
			InternalPixelFormat = GL_ALPHA;
			BufferType = GL_UNSIGNED_BYTE;
		}
		break;
	case TEX_FORMAT_RGB888_UNORM:
		{
			PixelFormat = GL_RGB;
			InternalPixelFormat = GL_RGB;
			BufferType = GL_UNSIGNED_BYTE;
		}break;
	default:
		Assert(0);
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, InternalPixelFormat, m_nWidth, this->m_nHeight, 0, PixelFormat, BufferType, m_pdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int err = glGetError();

	if (err != GL_NO_ERROR)
	{
		char info[256];
		SafeSprintf(info, sizeof(info), 
			"(CDynamicTex::Create)[DxLog][err:%d]glTexImage2D failed, "
			"width:%d height:%d format:%d", err, m_nWidth, m_nHeight, 
			m_nFormat);
		CORE_TRACE(info);

		if (err == GL_OUT_OF_MEMORY)
		{
			// 显存不足
			m_pRender->RaiseDeviceError(err,
				"(CDynamicTex::Create)glTexImage2D failed");
		}

		return false;
	}
	return true;
}
	
// 设备丢失时的处理
bool CDynamicTex::Invalidate()
{
	if(m_nTexture)
	{
		::glDeleteTextures(1, &m_nTexture);
		m_nTexture = 0;
	}
	return true;
}
#endif

#ifdef PROCESS_LOST_DEVICE
void CDynamicTex::Blit(void* dst_data, int dx, int dy, const void* src_data, int src_width, int src_height)
{
	char* dst_bits = &((char*)dst_data)[dy*m_nWidth + dx];
	const char* src_bits = (const char*)src_data;
	for(int y = 0; y < src_height; y++)
	{
		memcpy(dst_bits, src_bits, src_width);
		src_bits += src_width;
		dst_bits += m_nWidth;
	}
}
#endif