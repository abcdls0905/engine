//--------------------------------------------------------------------
// 文件名:		tex_sampler.h
// 内  容:		纹理采样器
// 说  明:		
// 创建日期:	2013年9月5日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _TEX_SAMPLER_H_
#define _TEX_SAMPLER_H_



#include "../visual/i_resource.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "render.h"



#define MAX_SAMPLER_TEXTURE_NUM 16 //最大支持16个
// 采样器绑定记录操作
class CTextureSamplerBindOp
{
public:
	static CTextureSamplerBindOp* NewInstance(Render* pRender);  

	CTextureSamplerBindOp(Render* pRender);
	// 释放
	void Release();

	// 当前记录的
    class CTextureSamplerBind
	{
	public:
		void SetTextureSamplerBind(fm_uint nSamplerID,fm_uint nTextureID)
		{
			m_nSamplerID = nSamplerID;
            m_nTextureID = nTextureID;
		}

		bool EqualTextureSamplerBind(fm_uint nSamplerID,fm_uint nTextureID)
		{
			return (m_nSamplerID == nSamplerID)&&(nTextureID == m_nTextureID);
		}

	private:
       fm_uint m_nSamplerID;
       fm_uint m_nTextureID;
	};

	// 当前激活的采样器
	void SetActiveSamplerID(fm_uint nSamplerID) 
	{
         m_nActiveSamplerID = nSamplerID;
	}

	bool EqualActiveSamplerID(fm_uint nSamplerID)
	{
		return m_nActiveSamplerID == nSamplerID;
	}

	// 记录绑定的采样器绑定的纹理ID
	void SetTextureIDBindSamplerID(fm_uint nSamplerID,fm_uint nTextureID)
	{
		if(nSamplerID>= MAX_SAMPLER_TEXTURE_NUM)
		{
			return;
		}
		m_TextureSamplerBind[nSamplerID].SetTextureSamplerBind(nSamplerID,nTextureID);
	}

	bool EqualTextureIDBindSamplerID(fm_uint nSamplerID,fm_uint nTextureID)
	{
		if(nSamplerID>= MAX_SAMPLER_TEXTURE_NUM)
		{
			return false;
		}
		return m_TextureSamplerBind[nSamplerID].EqualTextureSamplerBind(nSamplerID,nTextureID);
	}
private:
	CTextureSamplerBind m_TextureSamplerBind[MAX_SAMPLER_TEXTURE_NUM];
    fm_uint m_nActiveSamplerID;//当前激活的ID
	Render* m_pRender;
};


// 纹理采样器过滤方式
class CTextureSamplerFilterOp
{
public:
	void SetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min)
	{
		m_tex_mag = tex_mag;
        m_tex_min = tex_min;
	}

	void GetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER& tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER& tex_min)
	{
        tex_mag = m_tex_mag;
		tex_min = m_tex_min;
	}

	bool Equal(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min)
	{
		return (tex_mag == m_tex_mag) && (tex_min == m_tex_min);
	}
private:
    ITextureSampler::TEX_SAMPLER_MAG_FILTER m_tex_mag;
    ITextureSampler::TEX_SAMPLER_MIN_FILTER m_tex_min;
};

// 纹理采样器UV包装模式
class CTextureWrapModeOp
{
public:
	void SetTextureWrapMode(ITextureSampler::TEX_WRAP_TYPE wrap_type,ITextureSampler::TEX_WRAP_MODE wrap_mode)
	{
        m_wrap_mode[wrap_type] = wrap_mode;
	}

	bool Equal(ITextureSampler::TEX_WRAP_TYPE wrap_type,ITextureSampler::TEX_WRAP_MODE wrap_mode)
	{
		return m_wrap_mode[wrap_type] == wrap_mode;
	}

	ITextureSampler::TEX_WRAP_MODE GetTextureWrapMode(ITextureSampler::TEX_WRAP_TYPE wrap_type)
	{
		return m_wrap_mode[wrap_type];
	}

private:
	ITextureSampler::TEX_WRAP_MODE m_wrap_mode[ITextureSampler::TWT_MAX];
};

// 纹理采样器
class CTextureSampler:public ITextureSampler
{
public:
	CTextureSampler(Render* pRender);
	virtual ~CTextureSampler();

	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();

	// 释放
	virtual void Release();

	// 创建
	bool Create(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type);

	// 创建
	bool Create(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type,ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min,
		ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

	void SetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min);
	void GetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER& tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER& tex_min) ;

	void SetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode);
	// 获取UV纹理包装方式
	void GetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE& u_wrap_type,ITextureSampler::TEX_WRAP_MODE& v_wrap_mode);

	ITextureSampler::TEX_TYPE GetTextureType();

private:
	CTextureSampler(const CTextureSampler&);
	CTextureSampler& operator=(const CTextureSampler&);

	// 销毁
	virtual void Destroy();

	fm_int TextureMagFilterToGLTextureMagFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag);
	fm_int TextureMinFilterToGLTextureMinFilter(ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min);
	fm_int  TextureWrapModeToGLTextureWrapMode(ITextureSampler::TEX_WRAP_MODE tex_wrap_mode);
	GLenum TextureTypeToGLTextureType(ITextureSampler::TEX_TYPE tex_type);

	ITextureSampler::TEX_SAMPLER_MAG_FILTER GLTextureMagFilterToTextureMagFilter(fm_int tex_mag);
	ITextureSampler::TEX_SAMPLER_MIN_FILTER GLTextureMinFilterToTextureMinFilter(fm_int tex_min);
    ITextureSampler::TEX_WRAP_MODE GLTextureWrapModeToTextureWrapMode(fm_int tex_wrap_mode);
    ITextureSampler::TEX_TYPE GLTextureTypeToTextureType(GLenum tex_type);

private:
	Render* m_pRender;
	int m_nIndex;
	IShaderTex* m_pBindShaderTex;
    ITextureSampler::TEX_TYPE m_tex_sampler_type;

	CTextureSamplerFilterOp m_Tex_Sampler_FilterOp;
	CTextureWrapModeOp      m_Tex_Sampler_WrapModeOp;
};



#endif