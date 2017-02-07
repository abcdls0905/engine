//--------------------------------------------------------------------
// 文件名:		tex_sampler.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年9月5日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "tex_sampler.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"



CTextureSamplerBindOp* CTextureSamplerBindOp::NewInstance(Render* pRender)
{
	CTextureSamplerBindOp* p = (CTextureSamplerBindOp*)CORE_ALLOC(sizeof(CTextureSamplerBindOp));

	new (p) CTextureSamplerBindOp(pRender);

	return p;
}

CTextureSamplerBindOp::CTextureSamplerBindOp(Render* pRender):m_pRender(pRender)
{
}

// 释放
void CTextureSamplerBindOp::Release()
{
	CORE_DELETE(this);
}

CTextureSampler::CTextureSampler(Render* pRender)
{
	m_nIndex = -1;
	m_pBindShaderTex = NULL;
	m_pRender = pRender;
}

CTextureSampler::~CTextureSampler()
{
}

// 设备就绪时的处理
bool CTextureSampler::Restore()
{
	Create(m_pBindShaderTex,m_tex_sampler_type);
	return true;
}

// 设备丢失时的处理
bool CTextureSampler::Invalidate()
{
	return true;
}


// 释放
void CTextureSampler::Release()
{
	m_pRender->ReleaseResource(m_nIndex);
}


// 创建
bool CTextureSampler::Create(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type)
{
	GLenum GLTextureType = TextureTypeToGLTextureType(tex_type);
	fm_int GLTextureMagFilter;
	glBindTexture( GLTextureType, pBindShaderTex->GetTexture());
	glGetTexParameteriv( GLTextureType ,GL_TEXTURE_MAG_FILTER,&GLTextureMagFilter);
	ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag = GLTextureMagFilterToTextureMagFilter(GLTextureMagFilter);

	fm_int GLTextureMinFilter;
	glGetTexParameteriv( GLTextureType ,GL_TEXTURE_MIN_FILTER,&GLTextureMinFilter);
	ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min = GLTextureMinFilterToTextureMinFilter(GLTextureMinFilter);

	fm_int GLTextureWrapModeU;
	glGetTexParameteriv( GLTextureType ,GL_TEXTURE_WRAP_S,&GLTextureWrapModeU);
	ITextureSampler::TEX_WRAP_MODE tex_wrap_u = GLTextureWrapModeToTextureWrapMode(GLTextureWrapModeU);

	fm_int GLTextureWrapModeV;
	glGetTexParameteriv( GLTextureType ,GL_TEXTURE_WRAP_T,&GLTextureWrapModeV);
	ITextureSampler::TEX_WRAP_MODE tex_wrap_v = GLTextureWrapModeToTextureWrapMode(GLTextureWrapModeV);

	return Create(pBindShaderTex,tex_type,tex_mag,tex_min,tex_wrap_u,tex_wrap_u);
}

// 创建
bool CTextureSampler::Create(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type,ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min,
							 ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode)
{
	m_Tex_Sampler_FilterOp.SetTextureSamplerFilter(tex_mag,tex_min);
	m_Tex_Sampler_WrapModeOp.SetTextureWrapMode( ITextureSampler::TWT_U,u_wrap_mode);
	m_Tex_Sampler_WrapModeOp.SetTextureWrapMode( ITextureSampler::TWT_V,v_wrap_mode);
	m_pBindShaderTex = pBindShaderTex;
	m_tex_sampler_type = tex_type;
	return true;
}

void CTextureSampler::SetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min)
{
	if(m_Tex_Sampler_FilterOp.Equal(tex_mag,tex_min))
	{
		return;
	}

	glBindTexture(TextureTypeToGLTextureType(m_tex_sampler_type), m_pBindShaderTex->GetTexture());

	glTexParameteri(TextureTypeToGLTextureType(m_tex_sampler_type), GL_TEXTURE_MIN_FILTER, TextureMinFilterToGLTextureMinFilter(tex_min) );
	glTexParameteri(TextureTypeToGLTextureType(m_tex_sampler_type), GL_TEXTURE_MAG_FILTER, TextureMagFilterToGLTextureMagFilter(tex_mag) );

	m_Tex_Sampler_FilterOp.SetTextureSamplerFilter(tex_mag,tex_min);
}

void CTextureSampler::GetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER& tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER& tex_min)
{
	m_Tex_Sampler_FilterOp.GetTextureSamplerFilter(tex_mag,tex_min);
}

void CTextureSampler::SetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode)
{
	if( m_Tex_Sampler_WrapModeOp.Equal(ITextureSampler::TWT_U,u_wrap_mode) && m_Tex_Sampler_WrapModeOp.Equal(ITextureSampler::TWT_V,v_wrap_mode))
	{
		return;
	}
	glBindTexture(TextureTypeToGLTextureType(m_tex_sampler_type), m_pBindShaderTex->GetTexture());
	if(!m_Tex_Sampler_WrapModeOp.Equal(ITextureSampler::TWT_U,u_wrap_mode))
	{
		glTexParameteri(TextureTypeToGLTextureType(m_tex_sampler_type), GL_TEXTURE_WRAP_S, TextureWrapModeToGLTextureWrapMode(u_wrap_mode));
	}
	if(!m_Tex_Sampler_WrapModeOp.Equal(ITextureSampler::TWT_V,v_wrap_mode))
	{
		glTexParameteri(TextureTypeToGLTextureType(m_tex_sampler_type), GL_TEXTURE_WRAP_T, TextureWrapModeToGLTextureWrapMode(v_wrap_mode));
	}
	m_Tex_Sampler_WrapModeOp.SetTextureWrapMode( ITextureSampler::TWT_U,u_wrap_mode);
	m_Tex_Sampler_WrapModeOp.SetTextureWrapMode( ITextureSampler::TWT_V,v_wrap_mode);
}

// 获取UV纹理包装方式
void CTextureSampler::GetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE& u_wrap_type,ITextureSampler::TEX_WRAP_MODE& v_wrap_mode)
{
	u_wrap_type = m_Tex_Sampler_WrapModeOp.GetTextureWrapMode(ITextureSampler::TWT_U);
	v_wrap_mode = m_Tex_Sampler_WrapModeOp.GetTextureWrapMode(ITextureSampler::TWT_V);
}

void CTextureSampler::Destroy()
{
	CORE_DELETE(this);
}

fm_int CTextureSampler::TextureMagFilterToGLTextureMagFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag)
{
	fm_int GLTextureMagFilter = TS_MAG_LINEAR;
	switch(tex_mag)
	{
	case ITextureSampler::TS_MAG_LINEAR:
		GLTextureMagFilter = GL_LINEAR;
		break;
	case ITextureSampler::TS_MAG_NEAREST:
		GLTextureMagFilter = GL_NEAREST;
		break;
	default:
		break;
	}
	return GLTextureMagFilter;
}

fm_int CTextureSampler::TextureMinFilterToGLTextureMinFilter(ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min)
{
	fm_int GLTextureMagFilter = GL_LINEAR;
	switch(tex_min)
	{
	case ITextureSampler::TS_MIN_LINEAR:
		GLTextureMagFilter = GL_LINEAR;
		break;
	case ITextureSampler::TS_MIN_NEAREST:
		GLTextureMagFilter = GL_NEAREST;
		break;
	case ITextureSampler::TS_MIN_NEAREST_MIPMAP_NEAREST:
		GLTextureMagFilter = GL_NEAREST_MIPMAP_NEAREST;
		break;
	case ITextureSampler::TS_MIN_LINEAR_MIPMAP_NEAREST:
		GLTextureMagFilter = GL_LINEAR_MIPMAP_NEAREST;
		break;
	case ITextureSampler::TS_MIN_NEAREST_MIPMAP_LINEAR:
		GLTextureMagFilter = GL_NEAREST_MIPMAP_LINEAR;
		break;
	case ITextureSampler::TS_MIN_LINEAR_MIPMAP_LINEAR:
		GLTextureMagFilter = GL_LINEAR_MIPMAP_LINEAR;
		break;
	default:
		break;
	}

	return GLTextureMagFilter;
}

fm_int  CTextureSampler::TextureWrapModeToGLTextureWrapMode(ITextureSampler::TEX_WRAP_MODE tex_wrap_mode)
{
	fm_int GLTextureWrapMode = GL_REPEAT;
	switch(tex_wrap_mode)
	{
	case ITextureSampler::TWM_CLAMP_TO_EDGE:
		GLTextureWrapMode = GL_CLAMP_TO_EDGE;
		break;
	case ITextureSampler::TWM_REPEAT:
		GLTextureWrapMode = GL_REPEAT;
		break;
	case ITextureSampler::TWM_MIRRORED_REPEAT:
		GLTextureWrapMode = GL_MIRRORED_REPEAT;
		break;
	default:
		break;
	}
	return GLTextureWrapMode;
}

GLenum CTextureSampler::TextureTypeToGLTextureType(ITextureSampler::TEX_TYPE tex_type)
{
	GLenum GLTextureType = 0;
	switch(tex_type)
	{
	case ITextureSampler::TEX_2D:
		GLTextureType = GL_TEXTURE_2D;
		break;
	case ITextureSampler::TEX_3D:
#ifdef GL_TEXTURE_3D
		 GLTextureType = GL_TEXTURE_3D;//展示不支持3D操作
#endif
		break;
	case ITextureSampler::TEX_CUBE:
		GLTextureType = GL_TEXTURE_CUBE_MAP;
		break;
	default:
		break;
	}
	return GLTextureType;
}

ITextureSampler::TEX_SAMPLER_MAG_FILTER CTextureSampler::GLTextureMagFilterToTextureMagFilter(fm_int tex_mag)
{
	ITextureSampler::TEX_SAMPLER_MAG_FILTER _tex_mag = ITextureSampler::TS_MAG_LINEAR;
	switch(tex_mag)
	{
	case GL_LINEAR:
		_tex_mag = ITextureSampler::TS_MAG_LINEAR;
		break;
	case GL_NEAREST:
		_tex_mag = ITextureSampler::TS_MAG_NEAREST;
		break;
	default:
		break;
	}
	return _tex_mag;
}

ITextureSampler::TEX_SAMPLER_MIN_FILTER CTextureSampler::GLTextureMinFilterToTextureMinFilter(fm_int tex_min)
{
	ITextureSampler::TEX_SAMPLER_MIN_FILTER _tex_min = ITextureSampler::TS_MIN_LINEAR;
	switch(tex_min)
	{
	case GL_LINEAR:
        _tex_min = ITextureSampler::TS_MIN_LINEAR;
		break;
	case GL_NEAREST:
        _tex_min = ITextureSampler::TS_MIN_NEAREST;
		break;
	case GL_NEAREST_MIPMAP_NEAREST:
		_tex_min = ITextureSampler::TS_MIN_NEAREST_MIPMAP_NEAREST;
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		_tex_min = ITextureSampler::TS_MIN_LINEAR_MIPMAP_NEAREST;
		break;
	case GL_NEAREST_MIPMAP_LINEAR:
		_tex_min = ITextureSampler::TS_MIN_NEAREST_MIPMAP_LINEAR;
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		_tex_min = ITextureSampler::TS_MIN_LINEAR_MIPMAP_LINEAR;
		break;
	default:
		break;
	}
	return _tex_min;
}

ITextureSampler::TEX_WRAP_MODE CTextureSampler::GLTextureWrapModeToTextureWrapMode(fm_int tex_wrap_mode)
{
	ITextureSampler::TEX_WRAP_MODE _tex_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE;
	switch(tex_wrap_mode)
	{
	case GL_CLAMP_TO_EDGE:
		_tex_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE;
		break;
	case GL_REPEAT:
		_tex_wrap_mode = ITextureSampler::TWM_REPEAT;
		break;
	case GL_MIRRORED_REPEAT:
		_tex_wrap_mode = ITextureSampler::TWM_MIRRORED_REPEAT;
		break;
	default:
		break;
	}
	return _tex_wrap_mode;
}

ITextureSampler::TEX_TYPE CTextureSampler::GetTextureType()
{
	return m_tex_sampler_type;
}
