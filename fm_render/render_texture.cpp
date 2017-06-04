
#include "render.h"
#include "helper.h"
#include "texture.h"
#include "tex_factory.h"
#include "color_rt.h"
#include "dynamic_tex.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"
// 贴图管理

CTexture* Render::GetTextureById(const PERSISTID& id)
{
	return m_pTexFactory->Get(id);
}

ITexture* Render::FindTexture(const char* name)
{
	Assert(name != NULL);
	
	CTexture* p;

	if (!m_Textures.GetData(name, p))
	{
		return NULL;
	}

	return p;
}

ITexture* Render::CreateTexture(const char* name, bool async)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);

	if (async && pTexture->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pTexture->BeginCreate())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 请求线程加载
		if (!this->RequestLoadResource(0, pTexture->GetLoader()))
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置正在加载状态
		pTexture->SetState(CTexture::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pTexture->Create())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置资源就绪
		pTexture->SetState(CTexture::STATE_READY);
	}

	m_Textures.Add(name, pTexture);

	return pTexture;
}

CTexture* Render::CreateGlyphTexture(const char* name, bool async)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);
	pTexture->SetOriginSize(true);

	if (async && pTexture->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pTexture->BeginCreate())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 请求线程加载
		if (!this->RequestLoadResource(0, pTexture->GetLoader()))
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置正在加载状态
		pTexture->SetState(CTexture::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pTexture->Create())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置资源就绪
		pTexture->SetState(CTexture::STATE_READY);
	}

	m_Textures.Add(name, pTexture);

	return pTexture;
}

ITexture* Render::CreateCubeTexture(const char* name, bool async)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);

	if (async && pTexture->CanAsync())
	{
		// 创建异步加载的相关内容
		if (!pTexture->BeginCreateCube())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 请求线程加载
		if (!this->RequestLoadResource(0, pTexture->GetLoader()))
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置正在加载状态
		pTexture->SetState(CTexture::STATE_LOADING);
	}
	else
	{
		// 同步加载
		if (!pTexture->CreateCube())
		{
			m_pTexFactory->Destroy(pTexture);
			return NULL;
		}

		// 设置资源就绪
		pTexture->SetState(CTexture::STATE_READY);
	}

	m_Textures.Add(name, pTexture);

	return pTexture;
}

ITexture* Render::CreateNewTexture(const char* name, 
	unsigned int width, unsigned int height, unsigned int levels, 
	TEX_FORMAT format, const void* tex_data, unsigned int data_size)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);

	if (!pTexture->CreateNew(width, height, levels, format, tex_data, 
		data_size))
	{
		m_pTexFactory->Destroy(pTexture);
		return NULL;
	}

	// 设置资源就绪
	pTexture->SetState(CTexture::STATE_READY);
	m_Textures.Add(name, pTexture);

	return pTexture;
}

ITexture* Render::CreateDynamicTexture(const char* name, 
	unsigned int width, unsigned int height, unsigned int levels, 
	TEX_FORMAT format)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);

	if (!pTexture->CreateDynamic(width, height, levels, format))
	{
		m_pTexFactory->Destroy(pTexture);
		return NULL;
	}

	// 设置资源就绪
	pTexture->SetState(CTexture::STATE_READY);
	m_Textures.Add(name, pTexture);

	return pTexture;
}

ITexture* Render::CreateRenderTexture(const char* name, 
	unsigned int width, unsigned int height, TEX_FORMAT format)
{
	Assert(name != NULL);

	CTexture* p;

	if (m_Textures.GetData(name, p))
	{
		p->IncRefs();
		return p;
	}

	CTexture* pTexture = m_pTexFactory->Create();

	if (NULL == pTexture)
	{
		return NULL;
	}

	pTexture->SetName(name);

	if (!pTexture->CreateRenderTarget(width, height, format))
	{
		m_pTexFactory->Destroy(pTexture);
		return NULL;
	}

	// 设置资源就绪
	pTexture->SetState(CTexture::STATE_READY);
	m_Textures.Add(name, pTexture);

	return pTexture;
}

bool Render::ReleaseTexture(CTexture* pTexture)
{
	Assert(pTexture != NULL);

	if (1 == pTexture->GetRefs())
	{
		if(TextureCache::Inst())
		{
			TextureCache::Inst()->AddTexture(pTexture);
		}
		else
		{
			m_Textures.Remove(pTexture->GetName());
			m_pTexFactory->Destroy(pTexture);
		}
	}
	else
	{
		pTexture->DecRefs();
	}

	return true;
}

bool Render::DestroyTexture(CTexture* pTexture)
{
	Assert(pTexture != NULL);

	if (pTexture->GetRefs() <= 1)
	{
		m_Textures.Remove(pTexture->GetName());
		m_pTexFactory->Destroy(pTexture);
	}
	else
	{
		pTexture->DecRefs();
	}

	return true;
}




// 深度贴图拷贝
bool Render::CopyDepthStencilRT(IDepthStencilRT* src,IColorRT* dest,const char* pstrIdent)
{
	if(src == NULL)
	{
		CORE_TRACE("CopyDepthRT sr == NULL!");
		return false;
	}

    bool bENCODE_RGB888 = false;
	bool bENCODE_RGBA8888 = false;
	bool bENCODE_R16 = false;
	bool bENCODE_R32 = false;

	if( dest->GetFormat() == TEX_FORMAT_RGB888_UNORM )
	{
        bENCODE_RGB888 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_RGBA8888_UNORM )
	{
		bENCODE_RGBA8888 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_R16_FLOAT )
	{
		bENCODE_R16 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_R32_FLOAT )
	{
		bENCODE_R32 = true;
	}
	else
	{
		CORE_TRACE("CopyDepthRT Only Support: TEX_FORMAT_RGB888_UNORM TEX_FORMAT_RGBA8888_UNORM TEX_FORMAT_R16_FLOAT TEX_FORMAT_R32_FLOAT ");
		return false;
	}

	Render::copy_depth_shaderhandle_t* pShaderHandle = GetShaderHandle(bENCODE_RGB888,bENCODE_RGBA8888,bENCODE_R16,bENCODE_R32);
    
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1

	if( dest == NULL)
	{
		return Print2D(src,pShaderHandle,pstrIdent);
	}

 
   //先检测FBO
	if( m_pCopyDepthFrameRT == NULL)
	{
		m_pCopyDepthFrameRT = this->CreateFrameRT(dest);
		if(m_pCopyDepthFrameRT == NULL)
		{
			CORE_TRACE("CopyDepthRT CreateFrameRT Fail");
			CORE_TRACE(pstrIdent);
		}
	}
   
	if(!m_pCopyDepthFrameRT->SetColorRT( dest ) )
	{
		CORE_TRACE("CopyDepthRT SetColorRT Fail");
		CORE_TRACE(pstrIdent);
		return false;
	}

	// 保存原先的渲染环境
	IFrameRT* pOldFrameRT = GetUsedFrameRT();
    
	// 获取当前视口数据
	unsigned int x;
	unsigned int y;
	fm_sizei width;
	fm_sizei height; 	
	m_pRenderStateOp->GetViewPort(x,y,width,height);

	m_pCopyDepthFrameRT->UsedFrameRT();

	m_pRenderStateOp->SetViewPort( 0,0,dest->GetWidth(),dest->GetHeight());

	bool bReturn = Print2D(src,pShaderHandle,pstrIdent);

	// 恢复FBO
	UsedFrameRT(pOldFrameRT);
	m_pRenderStateOp->SetViewPort(x,y,width,height);

	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return bReturn;
}

bool Render::DrawAxisAlignedQuad(const char* pstrIdent)
{
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1

	//先保存当前的渲染环境

	IRenderDrawOp* pRenderDrawOp = this->GetRenderDrawOp();

	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);
	// 使用三角形顶点
	static const float s_Quad_VertexData[] = { -1.0, -1.0 , 1.0, -1.0 , -1.0,  1.0 , 1.0,  1.0 };

	static const float s_Quad_TexCoordData[] = { 0, 0,  1, 0,  0, 1,  1, 1 };

	//glEnableVertexAttribArray(post_copy_rt_vertex_t_pos);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_pos, true);
	//glVertexAttribPointer(post_copy_rt_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,0, s_Quad_VertexData);
	pRenderDrawOp->SetVertexAttribPointer(post_copy_rt_vertex_t_pos, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, 0,  s_Quad_VertexData);
    //glEnableVertexAttribArray(post_copy_rt_vertex_t_uv);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_uv,true);
    //glVertexAttribPointer(post_copy_rt_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, 0, s_Quad_TexCoordData);
	pRenderDrawOp->SetVertexAttribPointer( post_copy_rt_vertex_t_uv , 2, IRenderDrawOp::VERTEX_DATA_FLOAT, 0, s_Quad_TexCoordData);
	pRenderDrawOp->EnableVertexAttribArray(2, false);

	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0, 4);

	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return true;
}

bool Render::DrawHalfCube(const char* pstrIdent)
{
#define post_copy_rt_vertex_t_pos 0

	//先保存当前的渲染环境

	IRenderDrawOp* pRenderDrawOp = this->GetRenderDrawOp();

	// 使用三角形顶点
	static const FmVec4 s_half_cube_vb[8] =
	{
		FmVec4(-1.0F, +1.0F, 1.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, 1.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, 1.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, 1.0F, +1.0F),
		FmVec4(-1.0F, +1.0F, 0.0F, +1.0F),
		FmVec4(+1.0F, +1.0F, 0.0F, +1.0F),
		FmVec4(-1.0F, -1.0F, 0.0F, +1.0F),
		FmVec4(+1.0F, -1.0F, 0.0F, +1.0F),
	};

	static const unsigned short s_half_cube_ib[12 * 3] = 
	{
		0, 2, 1,
		1, 2, 3,
		0, 1, 5,
		0, 5, 4,
		0, 6, 2,
		0, 4, 6,
		1, 3, 7,
		1, 7, 5,
		2, 6, 7,
		2, 7, 3,
		4, 5, 6,
		5, 7, 6,
	};

	static IStaticVB* s_pHalfCubeVB = CreateStaticVB(s_half_cube_vb, 
		sizeof(s_half_cube_vb));
	static IStaticIB* s_pHalfCubeIB = CreateStaticIB(s_half_cube_ib, 
		sizeof(s_half_cube_ib));

	pRenderDrawOp->SetIB(s_pHalfCubeIB->GetBuffer());
	pRenderDrawOp->SetVB(s_pHalfCubeVB->GetBuffer());

	//glEnableVertexAttribArray(post_copy_rt_vertex_t_pos);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_pos, true);
	pRenderDrawOp->SetVertexAttribPointer(post_copy_rt_vertex_t_pos, 4, IRenderDrawOp::VERTEX_DATA_FLOAT, (4 * sizeof(float)), 0);
	pRenderDrawOp->EnableVertexAttribArray(1, false);

	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, 36, IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT, 0);

	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);
	pRenderDrawOp->EnableVertexAttribArray(0,false);

	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
	return true;
}


// 深度贴图拷贝
bool Render::CopyDepthRT(IDepthRT* src,IColorRT* dest,const char* pstrIdent)
{
	if(src == NULL)
	{
		CORE_TRACE("CopyDepthRT sr == NULL!");
		return false;
	}

    bool bENCODE_RGB888 = false;
	bool bENCODE_RGBA8888 = false;
	bool bENCODE_R16 = false;
	bool bENCODE_R32 = false;

	if( dest->GetFormat() == TEX_FORMAT_RGB888_UNORM )
	{
        bENCODE_RGB888 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_RGBA8888_UNORM )
	{
		bENCODE_RGBA8888 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_R16_FLOAT )
	{
		bENCODE_R16 = true;
	}
	else if( dest->GetFormat() == TEX_FORMAT_R32_FLOAT )
	{
		bENCODE_R32 = true;
	}
	else
	{
		CORE_TRACE("CopyDepthRT Only Support: TEX_FORMAT_RGB888_UNORM TEX_FORMAT_RGBA8888_UNORM TEX_FORMAT_R16_FLOAT TEX_FORMAT_R32_FLOAT ");
		return false;
	}

	Render::copy_depth_shaderhandle_t* pShaderHandle = GetShaderHandle(bENCODE_RGB888,bENCODE_RGBA8888,bENCODE_R16,bENCODE_R32);
    
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1

	if( dest == NULL)
	{
		return Print2D(src,pShaderHandle,pstrIdent);
	}

 
   //先检测FBO
	if( m_pCopyDepthFrameRT == NULL)
	{
		m_pCopyDepthFrameRT = this->CreateFrameRT(dest);
		if(m_pCopyDepthFrameRT == NULL)
		{
			CORE_TRACE("CopyDepthRT CreateFrameRT Fail");
			CORE_TRACE(pstrIdent);
		}
	}
   
	if(!m_pCopyDepthFrameRT->SetColorRT( dest ) )
	{
		CORE_TRACE("CopyDepthRT SetColorRT Fail");
		CORE_TRACE(pstrIdent);
		return false;
	}

	// 保存原先的渲染环境
	IFrameRT* pOldFrameRT = GetUsedFrameRT();
    
	// 获取当前视口数据
	unsigned int x;
	unsigned int y;
	fm_sizei width;
	fm_sizei height; 	
	m_pRenderStateOp->GetViewPort(x,y,width,height);

	m_pCopyDepthFrameRT->UsedFrameRT();

	m_pRenderStateOp->SetViewPort( 0,0,dest->GetWidth(),dest->GetHeight());

	bool bReturn = Print2D(src,pShaderHandle,pstrIdent);

	// 恢复FBO
	UsedFrameRT(pOldFrameRT);
	m_pRenderStateOp->SetViewPort(x,y,width,height);

	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return bReturn;
}

// Color RT拷贝（特殊解决后处理的问题)
bool Render::CopyColorRT(IColorRT* src,IColorRT* dest,const char* pstrIdent)
{
	if(src == NULL)
	{
		CORE_TRACE("CopyColorRT sr == NULL!");
		return false;
	}
    
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1

	if( dest == NULL)
	{
		return Print2D(src,pstrIdent);
	}


   //先检测FBO
	if( m_pCopyColorFrameRT == NULL)
	{
		m_pCopyColorFrameRT = this->CreateFrameRT(dest);
		if(m_pCopyColorFrameRT == NULL)
		{
			CORE_TRACE("CopyColorRT CreateFrameRT Fail");
			CORE_TRACE(pstrIdent);
		}
	}
   
	if(!m_pCopyColorFrameRT->SetColorRT( dest ) )
	{
		CORE_TRACE("CopyColorRT SetColorRT Fail");
		CORE_TRACE(pstrIdent);
		return false;
	}

	// 保存原先的渲染环境
	IFrameRT* pOldFrameRT = GetUsedFrameRT();
    
	// 获取当前视口数据
	unsigned int x;
	unsigned int y;
	fm_sizei width;
	fm_sizei height; 	
	m_pRenderStateOp->GetViewPort(x,y,width,height);

	m_pCopyColorFrameRT->UsedFrameRT();

	m_pRenderStateOp->SetViewPort( 0,0,dest->GetWidth(),dest->GetHeight());

	bool bReturn = Print2D(src,pstrIdent);

	// 恢复FBO
	UsedFrameRT(pOldFrameRT);
	m_pRenderStateOp->SetViewPort(x,y,width,height);

	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return bReturn;
}

const char* copy_color_rt_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};

// 直接全屏输出到屏幕上
bool Render::Print2D(IColorRT* src,const char* pstrIdent)
{
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1
     
	//先保存当前的渲染环境
	bool bDepthTest = m_pRenderStateOp->IsEnableDepthTest();
	bool bDepthWrite = m_pRenderStateOp->IsEnableDepthWrite();
	bool bCullFace = m_pRenderStateOp->IsCullFaceEnable();
	IRenderStateOp::CULL_FACE_MODE cull = m_pRenderStateOp->GetCullFaceModel();
	     
	IShaderProgram* pShader = this->GetUsedShader();


	IRenderDrawOp* pRenderDrawOp = this->GetRenderDrawOp();

	if(m_pCopyColorRTShader == NULL)
	{
		IVertexShader* pVS = this->LoadVertexShader("copy_color_rt.vsh", "vs_main_copy_color", "");

		IPixelShader* pPS = this->LoadPixelShader("copy_color_rt.fsh", "ps_main_copy_color", "");
		m_pCopyColorRTShader = this->CreateShaderProgram(pVS,pPS,copy_color_rt_vertex_t_name,2);
		m_nCopyColorRTSrcTex = m_pCopyColorRTShader->GetParamOp()->FindParamIdByName("tex_color");
	}

	m_pCopyColorRTShader->UsedShader();

 	m_pRenderStateOp->EnableDepthTest(false);
    m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->SetCullFaceModel(IRenderStateOp::CULL_FRONT);

	IShaderParamOp* pShaderOp = m_pCopyColorRTShader->GetParamOp();
	pShaderOp->SetTexture2D(m_nCopyColorRTSrcTex,src->GetTexture());
	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);

	// 使用三角形顶点
	static const float s_Quad_VertexData[] = { -1.0, -1.0 , 1.0, -1.0 , -1.0,  1.0 , 1.0,  1.0 };

	static const float s_Quad_TexCoordData[] = { 0, 0,  1, 0,  0, 1,  1, 1 };

	//glEnableVertexAttribArray(post_copy_rt_vertex_t_pos);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_pos,true);
	//glVertexAttribPointer(post_copy_rt_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,0, s_Quad_VertexData);
	pRenderDrawOp->SetVertexAttribPointer(post_copy_rt_vertex_t_pos, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0, s_Quad_VertexData);
    //glEnableVertexAttribArray(post_copy_rt_vertex_t_uv);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_uv,true);
    //glVertexAttribPointer(post_copy_rt_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, 0, s_Quad_TexCoordData);
	pRenderDrawOp->SetVertexAttribPointer( post_copy_rt_vertex_t_uv , 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0,s_Quad_TexCoordData);
	pRenderDrawOp->EnableVertexAttribArray(3,false);

	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0,4);

	//回复环境
	if(pShader)
		pShader->UsedShader();

	m_pRenderStateOp->EnableDepthTest(bDepthTest);
	m_pRenderStateOp->EnableDepthWrite(bDepthWrite);
	m_pRenderStateOp->EnableCullFace(bCullFace);
	m_pRenderStateOp->SetCullFaceModel(cull);
	 
	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return true;
}

// 直接绘制到全屏上面
bool Render::Print2D(IDepthRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent)
{
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1
     
	//先保存当前的渲染环境
	bool bDepthTest = m_pRenderStateOp->IsEnableDepthTest();
	bool bDepthWrite = m_pRenderStateOp->IsEnableDepthWrite();
	bool bCullFace = m_pRenderStateOp->IsCullFaceEnable();
	IRenderStateOp::CULL_FACE_MODE cull = m_pRenderStateOp->GetCullFaceModel();
	     
	IShaderProgram* pShader = this->GetUsedShader();


	IRenderDrawOp* pRenderDrawOp = this->GetRenderDrawOp();


	IShaderProgram* pCopyDepthShader = pShaderHandle->pShader;
	pCopyDepthShader->UsedShader();

 	m_pRenderStateOp->EnableDepthTest(false);
    m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->SetCullFaceModel(IRenderStateOp::CULL_FRONT);

	IShaderParamOp* pShaderOp = pCopyDepthShader->GetParamOp();
	pShaderOp->SetTexture2D(pShaderHandle->tex_DepthHandle,src->GetTexture());

	// 动态计算一下当前的深度相关的信息
	const camera_t  camera = this->GetSceneView()->GetContext()->GetCamera();
	FmVec4 DepthParam = FmVec4(camera.fFarZ / (camera.fFarZ * camera.fNearZ),(camera.fFarZ - camera.fNearZ) / (camera.fFarZ * camera.fNearZ) ,camera.fNearZ,camera.fFarZ);

	pShaderOp->SetParamValue(pShaderHandle->vec4DepthParamHandle,DepthParam);
	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);

	// 使用三角形顶点
	static const float s_Quad_VertexData[] = { -1.0, -1.0 , 1.0, -1.0 , -1.0,  1.0 , 1.0,  1.0 };

	static const float s_Quad_TexCoordData[] = { 0, 0,  1, 0,  0, 1,  1, 1 };

	//glEnableVertexAttribArray(post_copy_rt_vertex_t_pos);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_pos,true);
	//glVertexAttribPointer(post_copy_rt_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,0, s_Quad_VertexData);
	pRenderDrawOp->SetVertexAttribPointer(post_copy_rt_vertex_t_pos, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0, s_Quad_VertexData);
    //glEnableVertexAttribArray(post_copy_rt_vertex_t_uv);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_uv,true);
    //glVertexAttribPointer(post_copy_rt_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, 0, s_Quad_TexCoordData);
	pRenderDrawOp->SetVertexAttribPointer( post_copy_rt_vertex_t_uv , 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0,s_Quad_TexCoordData);
	pRenderDrawOp->EnableVertexAttribArray(3,false);

	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0,4);

	//回复环境
	if(pShader)
		pShader->UsedShader();

	m_pRenderStateOp->EnableDepthTest(bDepthTest);
	m_pRenderStateOp->EnableDepthWrite(bDepthWrite);
	m_pRenderStateOp->EnableCullFace(bCullFace);
	m_pRenderStateOp->SetCullFaceModel(cull);
	 
	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return true;
}

	// 直接绘制到全屏上面
bool Render::Print2D(IDepthStencilRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent)
{
#define post_copy_rt_vertex_t_pos 0
#define post_copy_rt_vertex_t_uv  1
     
	//先保存当前的渲染环境
	bool bDepthTest = m_pRenderStateOp->IsEnableDepthTest();
	bool bDepthWrite = m_pRenderStateOp->IsEnableDepthWrite();
	bool bCullFace = m_pRenderStateOp->IsCullFaceEnable();
	IRenderStateOp::CULL_FACE_MODE cull = m_pRenderStateOp->GetCullFaceModel();
	     
	IShaderProgram* pShader = this->GetUsedShader();


	IRenderDrawOp* pRenderDrawOp = this->GetRenderDrawOp();


	IShaderProgram* pCopyDepthShader = pShaderHandle->pShader;
	pCopyDepthShader->UsedShader();

 	m_pRenderStateOp->EnableDepthTest(false);
    m_pRenderStateOp->EnableDepthWrite(false);
	m_pRenderStateOp->SetCullFaceModel(IRenderStateOp::CULL_FRONT);

	IShaderParamOp* pShaderOp = pCopyDepthShader->GetParamOp();
	pShaderOp->SetTexture2D(pShaderHandle->tex_DepthHandle,src->GetTexture());

	// 动态计算一下当前的深度相关的信息
	const camera_t  camera = this->GetSceneView()->GetContext()->GetCamera();
	FmVec4 DepthParam = FmVec4(camera.fFarZ / (camera.fFarZ * camera.fNearZ),(camera.fFarZ - camera.fNearZ) / (camera.fFarZ * camera.fNearZ) ,camera.fNearZ,camera.fFarZ);

	pShaderOp->SetParamValue(pShaderHandle->vec4DepthParamHandle,DepthParam);
	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);

	// 使用三角形顶点
	static const float s_Quad_VertexData[] = { -1.0, -1.0 , 1.0, -1.0 , -1.0,  1.0 , 1.0,  1.0 };

	static const float s_Quad_TexCoordData[] = { 0, 0,  1, 0,  0, 1,  1, 1 };

	//glEnableVertexAttribArray(post_copy_rt_vertex_t_pos);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_pos,true);
	//glVertexAttribPointer(post_copy_rt_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,0, s_Quad_VertexData);
	pRenderDrawOp->SetVertexAttribPointer(post_copy_rt_vertex_t_pos, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0, s_Quad_VertexData);
    //glEnableVertexAttribArray(post_copy_rt_vertex_t_uv);
	pRenderDrawOp->EnableVertexAttribArray(post_copy_rt_vertex_t_uv,true);
    //glVertexAttribPointer(post_copy_rt_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, 0, s_Quad_TexCoordData);
	pRenderDrawOp->SetVertexAttribPointer( post_copy_rt_vertex_t_uv , 2,IRenderDrawOp::VERTEX_DATA_FLOAT,0,s_Quad_TexCoordData);
	pRenderDrawOp->EnableVertexAttribArray(3,false);

	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0,4);

	//回复环境
	if(pShader)
		pShader->UsedShader();

	m_pRenderStateOp->EnableDepthTest(bDepthTest);
	m_pRenderStateOp->EnableDepthWrite(bDepthWrite);
	m_pRenderStateOp->EnableCullFace(bCullFace);
	m_pRenderStateOp->SetCullFaceModel(cull);
	 
	::TestErr(pstrIdent);
#undef post_copy_rt_vertex_t_pos
#undef post_copy_rt_vertex_t_uv
	return true;
}


bool Render::UpdateDynamicTexture(ITexture* pTexture, int level, 
	unsigned int offset_x, unsigned int offset_y, unsigned int width, 
	unsigned int height, const void* pdata)
{
	Assert(pTexture != NULL);

	CTexture* pTex = (CTexture*)pTexture;
	IShaderTex* pShaderTex = pTex->GetShaderTex();

	if (pShaderTex->GetResourceType() != IResource::TYPE_DYNAMIC_TEX)
	{
		return false;
	}

	return UpdateDynamicTex((IDynamicTex*)pShaderTex, level, offset_x, 
		offset_y, width, height, pdata);
}

unsigned int Render::GetTextureAlpha(ITexture* pTexture, float u, float v)
{
	Assert(pTexture != NULL);

	CTexture* pTex = (CTexture*)pTexture;
	
	return pTex->GetTextureAlpha(u, v);
//	ID3D11Texture2D* pTraceTex = pTex->GetTraceTex();
//
//	if (NULL == pTraceTex)
//	{
//		return 0xFF;
//	}
//
//	D3D11_TEXTURE2D_DESC td;
//
//	pTraceTex->GetDesc(&td);
//
//	int x = (int)(u * (float)td.Width);
//	int y = (int)(v * (float)td.Height);
//	unsigned int alpha;
//
//	if (!Helper_GetTextureAlpha(pTraceTex, x, y, alpha))
//	{
//		return 0xFF;
//	}
//
//	return alpha;
}

bool Render::CopyTextureFromColorRT(ITexture* pTexture, IColorRT* pRT)
{
	Assert(pTexture != NULL);
	Assert(pRT != NULL);
	
	IShaderTex* pShaderTex = pTexture->GetShaderTex();

	if (pShaderTex->GetResourceType() != IResource::TYPE_DYNAMIC_TEX)
	{
		// 只有动态贴图可以做复制目标
		return false;
	}

	CDynamicTex* pDynamicTex = (CDynamicTex*)pShaderTex;
	CColorRT* pColorRT = (CColorRT*)pRT;

	if (pDynamicTex->GetFormat() != pColorRT->GetFormat())
	{
		// 纹理格式必须相同
		return false;
	}

//	m_pDeviceContext->CopyResource(pDynamicTex->GetTexture(),
//		pColorRT->GetTexture());

	return true;
}




DECLARE_ENTITY(TextureCache, 0, IEntity);

void TextureCache::AddTexture(CTexture* pTexture)
{
	Item item;
	item.pTexture = pTexture;
	item.time = 0;
	m_items.push_back(item);
};

bool TextureCache::Init(const IVarList& args)
{
	s_inst = this;
	g_pCore->AddExecute(this);
	m_cache_texture_time = 0.0f;
	return true;
}

bool TextureCache::Shut()
{
	return true;
}

void TextureCache::Execute(float seconds) 
{
	for(size_t i = 0; i < m_items.size(); i++)
	{
		if((m_items[i].time += seconds) > m_cache_texture_time)
		{
			m_pRender->DestroyTexture(m_items[i].pTexture);
			m_items.erase(m_items.begin() + i);
			i--;
		}
	}
}

void TextureCache::Clear()
{
	for(size_t i = 0; i < m_items.size(); i++)
	{
		m_pRender->DestroyTexture(m_items[i].pTexture);
	}
	m_items.clear();
}

void Render::SetTextureCacheTime(float time)
{
	TextureCache* pTextureCache = (TextureCache*)g_pCore->LookupEntity("TextureCache");
	if(pTextureCache)
		pTextureCache->m_cache_texture_time = time;
}

void Render::ClearTextureCache()
{
	TextureCache* pTextureCache = (TextureCache*)g_pCore->LookupEntity("TextureCache");
	if(pTextureCache)
		pTextureCache->Clear();

}