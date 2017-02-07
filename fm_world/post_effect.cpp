#include "post_effect.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"


DECLARE_ENTITY(CPostEffect, 0, IVisBase);




CPostEffect::CPostEffect()
{
    m_pTexBackColor = NULL;
	m_pRender = NULL;
}

CPostEffect::~CPostEffect()
{
	SAFE_RELEASE(m_pTexBackColor);
}
 
const char* post_effect_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};


const float s_Quad_VertexData[] = { -1.0, -1.0 , 1.0, -1.0 ,
	                           -1.0,  1.0 , 1.0,  1.0 };


const float s_Quad_TexCoordData[] = { 0, 0,  1, 0,  0, 1,  1, 1 };


bool CPostEffect::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}

	int width = m_pRender->GetDeviceWidth();
	int height = m_pRender->GetDeviceHeight();

	m_pTexBackColor = m_pRender->CreateColorRT(width,height,TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);
	if(m_pTexBackColor == NULL)
	{
		m_pTexBackColor = m_pRender->CreateColorRT(width,height,TEX_FORMAT_RGB565_UNORM, RT_TYPE_DEVICE);
		if(m_pTexBackColor == NULL)
			return false;
	}

	return true;
}

bool CPostEffect::Shut()
{
	return true;
}

void CPostEffect::Update(float seconds)
{

}

void CPostEffect::Realize()
{

}



void CPostEffect::DrawAxisAlignedQuad(IShaderProgram* pShader)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1

	IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();
	IRenderStateOp* pRenderStateOp = pShader->GetRenderStateOp();

	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);

	pRenderStateOp->SetCullFaceModel(IRenderStateOp::CULL_FRONT);

	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_pos);
	pRenderDrawOp->SetVertexAttribPointer(post_effect_vertex_t_pos,2,IRenderDrawOp::VERTEX_DATA_FLOAT,0, s_Quad_VertexData);
//	glEnableVertexAttribArray(post_effect_vertex_t_pos);
//	glVertexAttribPointer(post_effect_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,0, s_Quad_VertexData);
//  glEnableVertexAttribArray(post_effect_vertex_t_uv);
//  glVertexAttribPointer(post_effect_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, 0, s_Quad_TexCoordData);
	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_uv);
	pRenderDrawOp->SetVertexAttribPointer(post_effect_vertex_t_uv,2,IRenderDrawOp::VERTEX_DATA_FLOAT,0, s_Quad_TexCoordData);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0,4);
	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_pos,false);
	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_uv,false);
//	glDisableVertexAttribArray(post_effect_vertex_t_pos);
// 	glDisableVertexAttribArray(post_effect_vertex_t_uv);
	pRenderStateOp->SetCullFaceModel();
#undef post_effect_vertex_t_pos
#undef post_effect_vertex_t_uv

}


// 根据PS和VS创建
IShaderProgram* CPostEffect::CreateShaderProgram(IVertexShader* pVS,IPixelShader* pPS)
{
	return m_pRender->CreateShaderProgram(pVS,pPS,post_effect_vertex_t_name,2);
}

// 拷贝渲染表面内容
void CPostEffect::CopyBackColor(const char* ident)
{
	//拷贝表面到纹理上面去
	if(m_pTexBackColor&&m_pRender->GetContext()->GetEnableRealizeTempColorRT())
	{
		m_pRender->CopyColorRT( m_pRender->GetContext()->GetRealizeTempColorRT(), m_pTexBackColor,ident);
	}
}