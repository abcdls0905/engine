//--------------------------------------------------------------------
// 文件名:		dx_render_resource.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "render.h"
#include "texture.h"
#include "tex_factory.h"
#include "render_tool.h"
#include "paint_batch.h"
#include "static_vb.h"
#include "static_ib.h"
#include "static_tex.h"
#include "static_cube.h"
#include "dynamic_vb.h"
#include "dynamic_ib.h"
#include "dynamic_tex.h"
#include "color_rt.h"
#include "depth_rt.h"
#include "depthstencil_rt.h"

#include "color_tex.h"
#include "depth_tex.h"
#include "stencil_tex.h"
#include "depthstencil_tex.h"


#include "frame_rt.h"
#include "vertex_shader.h"
#include "pixel_shader.h"
#include "shader_program.h"
#include "tex_sampler.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

#include "../public/portable.h"
 
#define performance_time Port_GetPerformanceTime

// 渲染资源管理

int Render::GetResourceIndex()
{
	if (!m_ResourceFree.empty())
	{
		int index = m_ResourceFree.back();

		m_ResourceFree.pop_back();
		return index;
	}

	m_ResourceList.push_back(NULL);

	return (int)(m_ResourceList.size() - 1);
}

bool Render::ReleaseResource(int index)
{
	Assert(size_t(index) < m_ResourceList.size());
	Assert(m_ResourceList[index] != NULL);

	if (NULL == m_ResourceList[index])
	{
		return false;
	}

	m_ResourceList[index]->Destroy();
	m_ResourceList[index] = NULL;
	m_ResourceFree.push_back(index);

	return true;
}

IStaticTex* Render::CreateStaticTexFrom(fm_uint nTex)
{
	CStaticTex* p = (CStaticTex*)CORE_ALLOC(sizeof(CStaticTex));

	new (p) CStaticTex(this);

	if (!p->CreateFrom(nTex))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IStaticCube* Render::CreateStaticCubeFrom(fm_uint nTex)
{
	CStaticCube* p = (CStaticCube*)CORE_ALLOC(sizeof(CStaticCube));

	new (p) CStaticCube(this);

	if (!p->CreateFrom(nTex))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IStaticVB* Render::CreateStaticVB(const void* pdata, unsigned int size)
{
	CStaticVB* p = (CStaticVB*)CORE_ALLOC(sizeof(CStaticVB));

	new (p) CStaticVB(this);

	if (!p->Create(pdata, size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IStaticIB* Render::CreateStaticIB(const void* pdata, unsigned int size)
{
	CStaticIB* p = (CStaticIB*)CORE_ALLOC(sizeof(CStaticIB));

	new (p) CStaticIB(this);

	if (!p->Create(GL_UNSIGNED_SHORT, pdata, size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IDynamicVB* Render::CreateDynamicVB(unsigned int size,const char* pstrIdent)
{
	CDynamicVB* p = (CDynamicVB*)CORE_ALLOC(sizeof(CDynamicVB));

	new (p) CDynamicVB(this,pstrIdent);

	if (!p->Create(size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IDynamicIB* Render::CreateDynamicIB(unsigned int size,const char* pstrIdent)
{
	CDynamicIB* p = (CDynamicIB*)CORE_ALLOC(sizeof(CDynamicIB));

	new (p) CDynamicIB(this,pstrIdent);

	if (!p->Create(GL_UNSIGNED_SHORT, size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IStaticTex* Render::CreateStaticTex(unsigned int width, 
	unsigned int height, unsigned int levels, TEX_FORMAT format,
	const void* tex_data, unsigned int data_size)
{
	CStaticTex* p = (CStaticTex*)CORE_ALLOC(sizeof(CStaticTex));

	new (p) CStaticTex(this);

	if (!p->Create(width, height, levels, format, tex_data, data_size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IDynamicTex* Render::CreateDynamicTex(unsigned int width, 
	unsigned int height, unsigned int levels, TEX_FORMAT format)
{
	CDynamicTex* p = (CDynamicTex*)CORE_ALLOC(sizeof(CDynamicTex));

	new (p) CDynamicTex(this);

	if (!p->Create(width, height, levels, format))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IColorRT* Render::CreateColorRT(unsigned int width, 
	unsigned int height, TEX_FORMAT format, int type)
{
	CColorRT* p = (CColorRT*)CORE_ALLOC(sizeof(CColorRT));

	new (p) CColorRT(this, width, height, format, false, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateColorRT Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IColorRT* Render::CreateMipmapColorRT(unsigned int width, 
	unsigned int height, TEX_FORMAT format, int type)
{
	CColorRT* p = (CColorRT*)CORE_ALLOC(sizeof(CColorRT));

	new (p) CColorRT(this, width, height, format, true, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IDepthRT* Render::CreateDepthRT(unsigned int width, 
	unsigned int height, TEX_FORMAT format, int type)
{
	CDepthRT* p = (CDepthRT*)CORE_ALLOC(sizeof(CDepthRT));

	new (p) CDepthRT(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateDepthRT Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建一个FrameRT
IFrameRT* Render::CreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));

	new (p) CFrameRT(this,pColorRT,pDepthRT,pStencilTex);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 只要深度采样
IFrameRT* Render::CreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorTex,pDepthRT,pStencilTex);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}


// 只要深度采样
IFrameRT* Render::CreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorRT,pDepthTex,pStencilTex);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 只要深度采样
IFrameRT* Render::CreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorTex,pDepthStencilTex);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建一个FrameRT colortx和depthstencil
IFrameRT* Render::CreateFrameRT(IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorRT,pDepthStencilTex);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建一个FrameRT colortx和depthstencil
IFrameRT* Render::CreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorRT,pDepthStencilRT);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建一个
IFrameRT* Render::CreateFrameRT(IColorRT* pColorRT)
{
	CFrameRT* p = (CFrameRT*)CORE_ALLOC(sizeof(CFrameRT));
	new (p) CFrameRT(this,pColorRT);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}


bool Render::UpdateDynamicVB(IDynamicVB* pVBuffer, unsigned int offset,
	const void* pdata, unsigned int size)
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif
	Assert(pVBuffer != NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, pVBuffer->GetBuffer());
	m_pRenderDrawOp->SetVB( pVBuffer->GetBuffer() );
	//如果是发现是整个数据更新，则丢弃掉原先所有数据 
	if(offset == 0 && pVBuffer->GetSize() == size)
	{
		glBufferData(GL_ARRAY_BUFFER,size,NULL,GL_DYNAMIC_DRAW);
	}
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, pdata);


#ifdef PERFORMANCE_DEBUG
	this->GetPerformance()->dVBUploadGPUTime += performance_time() - dTime;
	this->GetPerformance()->dLoadVBData +=size/1024.0f;
#endif	

	GLenum res = glGetError();

	if (res != GL_NO_ERROR)
	{
		CORE_TRACE_EX("(Render::UpdateDynamicVB)update buffer failed! offset:%d size:%d",offset,size);
		if(pVBuffer->GetIdent())
			CORE_TRACE(pVBuffer->GetIdent());
	}
	
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_pRenderDrawOp->SetVB( 0 );


	return (res == GL_NO_ERROR);
}

bool Render::UpdateDynamicIB(IDynamicIB* pIBuffer, unsigned int offset,
	const void* pdata, unsigned int size)
{
#ifdef PERFORMANCE_DEBUG
	double dTime = performance_time();
#endif
	Assert(pIBuffer != NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIBuffer->GetBuffer());
	m_pRenderDrawOp->SetIB(pIBuffer->GetBuffer());
	if(offset == 0 && pIBuffer->GetSize() == size)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,NULL,GL_DYNAMIC_DRAW);
	}
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, pdata);


#ifdef PERFORMANCE_DEBUG
	this->GetPerformance()->dIBUploadGPUTime += performance_time() - dTime;
	this->GetPerformance()->dLoadIBData +=size/1024.0f;
#endif

	GLenum res = glGetError();

	if (res != GL_NO_ERROR)
	{
		CORE_TRACE("(Render::UpdateDynamicIB)update buffer failed");
	}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_pRenderDrawOp->SetIB(0);

	return (res == GL_NO_ERROR);
}

bool Render::UpdateDynamicTex(IDynamicTex* pTexture, int level, 
	unsigned int offset_x, unsigned int offset_y, unsigned int width, 
	unsigned int height, const void* pdata)
{
	Assert(pTexture != NULL);

	return ((CDynamicTex*)pTexture)->Update(level, offset_x, offset_y, width, height, pdata);
}

CVertexShader* Render::CreateVertexShader(const void* shader_data, 
	unsigned int shader_size)
{
	Assert(shader_data != NULL);

	CVertexShader* p = (CVertexShader*)CORE_ALLOC(sizeof(CVertexShader));

	new (p) CVertexShader(this);

	if (!p->Create(shader_data, shader_size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

CPixelShader* Render::CreatePixelShader(const void* shader_data, 
	unsigned int shader_size)
{
	Assert(shader_data != NULL);

	CPixelShader* p = (CPixelShader*)CORE_ALLOC(sizeof(CPixelShader));

	new (p) CPixelShader(this);

	if (!p->Create(shader_data, shader_size))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

IShaderProgram* Render::CreateShaderProgram(IVertexShader* pVS, 
	IPixelShader* pPS, const char* element_names[], unsigned int element_num)
{
	Assert(pVS != NULL);
	Assert(pPS != NULL);

	CShaderProgram* p = (CShaderProgram*)CORE_ALLOC(sizeof(CShaderProgram));

	new (p) CShaderProgram(this);

	if (!p->Create(pVS, pPS, element_names, 
		element_num))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

bool Render::DumpResource(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	for (size_t i = 0; i < m_ResourceList.size(); ++i)
	{
		if (NULL == m_ResourceList[i])
		{
			continue;
		}

		switch (m_ResourceList[i]->GetResourceType())
		{
		case IResource::TYPE_DYNAMIC_VB:
			{
				CDynamicVB* pVB = (CDynamicVB*)m_ResourceList[i];

				fprintf(fp, "dynamic vb handle:%d size:%d\r\n", 
					pVB->GetBuffer(), pVB->GetSize());
			}
			break;
		case IResource::TYPE_DYNAMIC_IB:
			{
				CDynamicIB* pIB = (CDynamicIB*)m_ResourceList[i];

				fprintf(fp, "dynamic ib handle:%d size:%d\r\n", 
					pIB->GetBuffer(), pIB->GetSize());
			}
			break;
		case IResource::TYPE_DYNAMIC_TEX:
			{
				CDynamicTex* pTex = (CDynamicTex*)m_ResourceList[i];

				fprintf(fp, 
					"dynamic tex handle:%d width:%d height:%d format:%d\r\n",
					pTex->GetTexture(), pTex->GetWidth(), 
					pTex->GetHeight(), pTex->GetFormat());
			}
			break;
		case IResource::TYPE_COLOR_RT:
			{
				CColorRT* pColorRT = (CColorRT*)m_ResourceList[i];

				fprintf(fp, 
					"color rt handle:%d width:%d height:%d format:%d\r\n",
					pColorRT->GetTexture(), pColorRT->GetWidth(), 
					pColorRT->GetHeight(), pColorRT->GetFormat());
			}
			break;
		case IResource::TYPE_DEPTH_RT:
			{
				CDepthRT* pDepthRT = (CDepthRT*)m_ResourceList[i];

				fprintf(fp, 
					"depth rt handle:%d width:%d height:%d format:%d\r\n",
					pDepthRT->GetTexture(), pDepthRT->GetWidth(), 
					pDepthRT->GetHeight(), pDepthRT->GetFormat());
			}
			break;
		case IResource::TYPE_STATIC_VB:
			{
				CStaticVB* pVB = (CStaticVB*)m_ResourceList[i];

				fprintf(fp, "static vb handle:%d size:%d\r\n", 
					pVB->GetBuffer(), pVB->GetSize());
			}
			break;
		case IResource::TYPE_STATIC_IB:
			{
				CStaticIB* pIB = (CStaticIB*)m_ResourceList[i];

				fprintf(fp, "static ib handle:%d size:%d\r\n", 
					pIB->GetBuffer(), pIB->GetSize());
			}
			break;
		case IResource::TYPE_STATIC_TEX:
			{
				CStaticTex* pTex = (CStaticTex*)m_ResourceList[i];

				fprintf(fp, 
					"static tex handle:%d width:%d height:%d format:%d\r\n", 
					pTex->GetTexture(), pTex->GetWidth(), 
					pTex->GetHeight(), pTex->GetFormat());
			}
			break;
		case IResource::TYPE_STATIC_CUBE:
			{
				CStaticCube* pTex = (CStaticCube*)m_ResourceList[i];

				fprintf(fp, 
					"static cube handle:%d width:%d height:%d format:%d\r\n", 
					pTex->GetTexture(), pTex->GetWidth(), 
					pTex->GetHeight(), pTex->GetFormat());
			}
			break;
		case IResource::TYPE_VERTEX_SHADER:
			break;
		case IResource::TYPE_PIXEL_SHADER:
			break;
		case IResource::TYPE_SHADER_PROGRAM:
			break;
		default:
			Assert(0);
			break;
		}
	}

	fprintf(fp, "\r\n");

	// 贴图
	int count = 0;
	int size = 0;

	for (texture_container_t::iterator it1 = m_Textures.Begin(); 
		it1 != m_Textures.End(); ++it1)
	{
		CTexture* p = it1.GetData();

		fprintf(fp, "texture:%s pointer:%p refs:%d state:%d width:%d "
			"height:%d format:%d size:%d\r\n", 
			p->GetName(), p->GetShaderTex(), p->GetRefs(), p->GetState(), 
			p->GetWidth(), p->GetHeight(), p->GetFormat(), p->GetSize());
		size += p->GetSize();
		++count;
	}

	fprintf(fp, "\r\ntotal %d textures, size %d bytes\r\n\r\n", count, size);
	fclose(fp);

	return true;
}

bool Render::ClearRenderResource()
{
	m_pRenderTool->ClearRenderResource();
	
	return true;
}

// 创建一个纹理采样器
ITextureSampler* Render::CreateTextureSamper(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type,ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,
											 ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min,ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode)
{
	CTextureSampler* p = (CTextureSampler*)CORE_ALLOC(sizeof(CTextureSampler));

	new (p) CTextureSampler(this);

	if (!p->Create(pBindShaderTex,tex_type,tex_mag,tex_min,u_wrap_mode,v_wrap_mode))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);
    return p;
}


// 从已存在存在的采样器中查询状态并创建
ITextureSampler* Render::CreateTextureSamperFromShaderTex(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type)
{
	CTextureSampler* p = (CTextureSampler*)CORE_ALLOC(sizeof(CTextureSampler));

	new (p) CTextureSampler(this);

	if (!p->Create(pBindShaderTex,tex_type))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);
    return p;
}


bool Render::SaveTextureToFile(IShaderTex* pShaderTex, const char* file_name)
{
	Assert(pShaderTex != NULL);
	Assert(file_name != NULL);

	return false;
}


// 创建不可采样的ColorTex
IColorTex* Render::CreateColorTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type)
{
	CColorTex* p = (CColorTex*)CORE_ALLOC(sizeof(CColorTex));

	new (p) CColorTex(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateDepthTex Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建不可采样的DepthTex
IDepthTex* Render::CreateDepthTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type)
{
	CDepthTex* p = (CDepthTex*)CORE_ALLOC(sizeof(CDepthTex));

	new (p) CDepthTex(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateDepthTex Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建不可以采样的模板图
IStencilTex* Render::CreateStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type)
{
	CStencilTex* p = (CStencilTex*)CORE_ALLOC(sizeof(CStencilTex));

	new (p) CStencilTex(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateStencilTex Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

// 创建不可以采样的模板图
IDepthStencilTex* Render::CreateDepthStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type)
{
	CDepthStencilTex* p = (CDepthStencilTex*)CORE_ALLOC(sizeof(CDepthStencilTex));

	new (p) CDepthStencilTex(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateDepthStencilTex Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}


// 创建不可以采样的模板图
IDepthStencilRT* Render::CreateDepthStencilRT(unsigned int width,unsigned int height, TEX_FORMAT format, int type)
{

	CDepthStencilRT* p = (CDepthStencilRT*)CORE_ALLOC(sizeof(CDepthStencilRT));
	new (p) CDepthStencilRT(this, width, height, format, type);

	if (!p->Create())
	{
		CORE_DELETE(p);
		return NULL;
	}

	// 检测创建结果
	if(TestErr("CreateDepthStencilRT Failed!"))
	{
		CORE_DELETE(p);
		return NULL;
	}

	int index = GetResourceIndex();

	Assert(m_ResourceList[index] == NULL);
	m_ResourceList[index] = p;
	p->SetIndex(index);

	return p;
}

//以下2个接口提供内部渲染相关资源的查询
size_t Render::GetResourceNum()
{
	return m_ResourceList.size();
}

IResource* Render::GetResource(size_t index)
{
	if(index >= m_ResourceList.size() )
		return NULL;
	return m_ResourceList[index];
}