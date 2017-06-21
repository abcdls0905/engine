#include "post_debug.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

DECLARE_ENTITY(CPostDebug, 0, IVisBase);
/// property: float LeftUpX
/// \brief  
DECLARE_PROPERTY(float, CPostDebug, LeftUpX, GetLeftUpX, SetLeftUpX);
/// property: float LeftUpX
/// \brief  
DECLARE_PROPERTY(float, CPostDebug, LeftUpY, GetLeftUpY, SetLeftUpY);

/// property: float LeftUpX
/// \brief  
DECLARE_PROPERTY(float, CPostDebug, RightBottomX, GetRightBottomX, SetRightBottomX);
/// property: float LeftUpX
/// \brief  
DECLARE_PROPERTY(float, CPostDebug, RightBottomY, GetRightBottomY, SetRightBottomY);


CPostDebug::CPostDebug()
{
	
	m_leftup = FmVec2(0.0,0.75);
	m_rightbottom = FmVec2(0.25,1.0);
	
// 	m_leftup = FmVec2(0.0,0.0);
// 	m_rightbottom = FmVec2(0.5,0.5);

	m_pTexColor = NULL;
	m_pTexDepth = NULL;
	m_pTexDepthStencil = NULL;
	m_pTex = NULL;
}

CPostDebug::~CPostDebug()
{
#define shadermap 	TPodHashMap<unsigned int, CPostDebug::post_debug_shaderhandle_t*, TPodTraits<unsigned int>,TPostDebugShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(CPostDebug::post_debug_shaderhandle_t));
	}   
#undef shadermap


//	SAFE_RELEASE(m_pShader);
//	SAFE_RELEASE(m_pDepthShader);
}

const char* post_debug_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};

/*

C(1.0,1.0)                 D(1.0,1.0)
                Y
            /|\
			 |
             |
             |
 ------------------------>X
             |
			 |
			 |

A(-1.0,-1.0)              B(1.0,-1.0)

*/

bool CPostDebug::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}

	if(!args.IsEmpty())
	{
		//开始根据IVarList解析坐标
		m_leftup = FmVec2(args.FloatVal(0),args.FloatVal(1));
		m_rightbottom = FmVec2(args.FloatVal(2),args.FloatVal(3));
	}  

	UpDataVertByScreenPos(m_leftup,m_rightbottom);

    
	/*
    IVertexShader* pVS = m_pRender->LoadVertexShader("post_debug.vsh", "vs_main_post", "");
	IPixelShader* pPS = m_pRender->LoadPixelShader("post_debug.fsh", "ps_main_post", "");
	IPixelShader* pDepthPS = m_pRender->LoadPixelShader("post_debug.fsh", "ps_main_post", "#define DEPTH 1\n");
	*/

	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("post_debug.vsh", "vs_main_post_debug");
	m_VSList.SetDefine(FLAG_COLOR, "#define COLOR 1\n");
	m_VSList.SetDefine(FLAG_DEPTH, "#define DEPTH 1\n");
	m_VSList.SetDefine(FLAG_TEXTURE, "#define TEXTURE 1\n");
	m_VSList.SetDefine(FLAG_DECODE_DEPTH, "#define DECODE_DEPTH 1\n");


	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("post_debug.fsh", "ps_main_post_debug");
	m_PSList.SetDefine(FLAG_COLOR, "#define COLOR 1\n");
	m_PSList.SetDefine(FLAG_DEPTH, "#define DEPTH 1\n");
	m_PSList.SetDefine(FLAG_TEXTURE, "#define TEXTURE 1\n");
	m_PSList.SetDefine(FLAG_DECODE_DEPTH, "#define DECODE_DEPTH 1\n");
	/*
    m_pShader = m_pRender->CreateShaderProgram(pVS,pPS,post_debug_vertex_t_name,2);

	m_pDepthShader = m_pRender->CreateShaderProgram(pVS,pDepthPS,post_debug_vertex_t_name,2);
	*/

	m_vert[0].uv = FmVec2(0.0,1.0);
	m_vert[1].uv = FmVec2(1.0,1.0);
	m_vert[2].uv = FmVec2(0.0,0.0);
	m_vert[3].uv = FmVec2(1.0,0.0);
	return true;
}


float CPostDebug::GetLeftUpX()
{
	return m_leftup.x;
}

void  CPostDebug::SetLeftUpX(float value) 
{
	m_leftup.x = value;
	UpDataVertByScreenPos(m_leftup,m_rightbottom);
}

float CPostDebug::GetLeftUpY() 
{ 
	return m_leftup.y;
}

void  CPostDebug::SetLeftUpY(float value)
{
	m_leftup.y = value; 
	UpDataVertByScreenPos(m_leftup,m_rightbottom);
}

float CPostDebug::GetRightBottomX() 
{ 
	return m_rightbottom.x; 
}

void  CPostDebug::SetRightBottomX(float value) 
{ 
	m_rightbottom.x = value;
	UpDataVertByScreenPos(m_leftup,m_rightbottom);
}

float CPostDebug::GetRightBottomY() 
{
	return m_rightbottom.y;
};

void  CPostDebug::SetRightBottomY(float value) 
{ 
	m_rightbottom.y = value;
	UpDataVertByScreenPos(m_leftup,m_rightbottom);
}


bool CPostDebug::Shut()
{
	return true;
}

void CPostDebug::Update(float seconds)
{
}

void CPostDebug::Realize()
{
	m_post_debug_cb.mat4InvProj = m_pRender->GetSceneView()->GetContext()->GetCamera().mtxProjInverse;

	m_pTexColor = m_pRender->GetSceneView()->GetContext()->GetDebugColorRT();	

	if(m_pTexColor)
	{
		m_pRender->GetSceneView()->AddPostBatch(RenderColorRT, this,"PostDebugColorRT");

		m_uvSacleColorRT = FmVec2((float)m_pRender->GetDeviceWidth() / m_pTexColor->GetWidth(),(float)m_pRender->GetDeviceHeight() / m_pTexColor->GetHeight());
		m_uvSacleColorRT = FmVec2(FMABS(m_uvSacleColorRT.x),FMABS(m_uvSacleColorRT.y));
		m_post_debug_cb.vec2scale = m_uvSacleColorRT;
		return;
	}

	m_pTexDepth = m_pRender->GetSceneView()->GetContext()->GetDebugDepthRT();
	if(m_pTexDepth&&m_pTexColor==NULL)
	{
		const camera_t  camera = m_pRender->GetSceneView()->GetContext()->GetCamera();
		m_vec4DepthParam = FmVec4(camera.fFarZ / (camera.fFarZ * camera.fNearZ),(camera.fFarZ - camera.fNearZ) / (camera.fFarZ * camera.fNearZ) ,camera.fNearZ,camera.fFarZ);
		m_pRender->GetSceneView()->AddPostBatch(RenderDepthRT, this,"PostDebugDepthRT");

		m_vsScaleDepthRT =  FmVec2((float)m_pRender->GetDeviceWidth() / m_pTexDepth->GetWidth(),(float)m_pRender->GetDeviceHeight() / m_pTexDepth->GetHeight());
		m_vsScaleDepthRT = FmVec2(FMABS(m_vsScaleDepthRT.x),FMABS(m_vsScaleDepthRT.y));
		m_post_debug_cb.vec2scale = m_vsScaleDepthRT;
		m_post_debug_cb.vec4DepthParam = m_vec4DepthParam;
		return;
	}

	m_pTexDepthStencil = m_pRender->GetSceneView()->GetContext()->GetDebugDepthStencilRT();
	if(m_pTexDepthStencil&& (m_pTexColor==NULL) && (m_pTexDepth == NULL) )
	{
		const camera_t  camera = m_pRender->GetSceneView()->GetContext()->GetCamera();
		m_vec4DepthParam = FmVec4(camera.fFarZ / (camera.fFarZ * camera.fNearZ),(camera.fFarZ - camera.fNearZ) / (camera.fFarZ * camera.fNearZ) ,camera.fNearZ,camera.fFarZ);
		m_pRender->GetSceneView()->AddPostBatch(RenderDepthStencilRT, this,"PostDebugDepthStencilRT");

		m_vsScaleDepthRT =  FmVec2((float)m_pRender->GetDeviceWidth() / m_pTexDepth->GetWidth(),(float)m_pRender->GetDeviceHeight() / m_pTexDepth->GetHeight());
		m_vsScaleDepthRT = FmVec2(FMABS(m_vsScaleDepthRT.x),FMABS(m_vsScaleDepthRT.y));
		m_post_debug_cb.vec2scale = m_vsScaleDepthRT;
		m_post_debug_cb.vec4DepthParam = m_vec4DepthParam;
		return;
	}

	m_pTex = m_pRender->GetSceneView()->GetContext()->GetDebugTexture();
	if(m_pTex && (m_pTexColor == NULL) && (m_pTexDepth == NULL)&& (m_pTexDepthStencil == NULL) )
	{
		m_pRender->GetSceneView()->AddPostBatch(RenderTexture, this,"PostDebugDepthRT");

		m_uvSacleColorRT = FmVec2(1.0,1.0);
		m_post_debug_cb.vec2scale = m_uvSacleColorRT;
		return;
	}

	//解码深度图
	m_pTexDecodeColor = m_pRender->GetSceneView()->GetContext()->GetDebugDecodeColorRT();
	if(m_pTexDecodeColor)
	{
		m_pRender->GetSceneView()->AddPostBatch(RenderDecodeDepthRT, this,"PostDebugDecodeDepthRT");
		m_post_debug_cb.vec2scale = FmVec2(1.0,1.0);
		return;
	}
}

void CPostDebug::DrawAxisAlignedQuad(IShaderProgram* pShader)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1
    IRenderDrawOp* pRenderDrawOp = pShader->GetRenderDrawOp();
//	TestErr("CPostDebug::DrawAxisAlignedQuad");
	pRenderDrawOp->SetIB(0);
	pRenderDrawOp->SetVB(0);
	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_pos, true);
	pRenderDrawOp->SetVertexAttribPointer(post_effect_vertex_t_pos, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(post_debug_vertex_t), &m_vert[0].pos);
	//glEnableVertexAttribArray(post_effect_vertex_t_pos);
	//glVertexAttribPointer(post_effect_vertex_t_pos, 2, GL_FLOAT, GL_FALSE,sizeof(post_debug_vertex_t), &m_vert[0].pos);

	pRenderDrawOp->EnableVertexAttribArray(post_effect_vertex_t_uv, true);
	pRenderDrawOp->SetVertexAttribPointer(post_effect_vertex_t_uv, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(post_debug_vertex_t), &m_vert[0].uv);
    //glEnableVertexAttribArray(post_effect_vertex_t_uv);
	//glVertexAttribPointer(post_effect_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, sizeof(post_debug_vertex_t), &m_vert[0].uv);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0, 4);
	//glDisableVertexAttribArray(post_effect_vertex_t_pos);
 	//glDisableVertexAttribArray(post_effect_vertex_t_uv);
}

//根据位置创建一个输出位置
void CPostDebug::UpDataVertByScreenPos(FmVec2 leftup_pos,FmVec2 rightbottom_pos)
{
	//进行防错处理
	FmVec2 size = rightbottom_pos-leftup_pos;

	if((size.x < 0.001f) || (size.y < 0.001f) )
	{
		CORE_TRACE("UpDataVertByScreenPos size err!");
	}

	size.x = FMABS(size.x);
	size.y = FMABS(size.y);
	leftup_pos.x = FM_CLAMP(leftup_pos.x, 0.0f, 1.0f);
	leftup_pos.y = FM_CLAMP(leftup_pos.y, 0.0f, 1.0f);


#define PRJ_TO_SCR_POS(pos) FmVec2((pos.x-0.5f) * 2.0f, (pos.y-0.5f) * 2.0f)
 
	//C
	m_vert[2].pos = PRJ_TO_SCR_POS(leftup_pos);
	//D
    m_vert[3].pos = PRJ_TO_SCR_POS(leftup_pos)+FmVec2(size.x * 2.0f, 0.0f);
	//A
	m_vert[0].pos = PRJ_TO_SCR_POS(leftup_pos)+FmVec2(0.0,size.y * 2.0f);
	//B
  m_vert[1].pos = PRJ_TO_SCR_POS(leftup_pos)+FmVec2(size.x*2,size.y * 2.0f);
  int x = 0;
  m_vert[0].pos = FmVec2(-1.0, 1.0);
  m_vert[1].pos = FmVec2(0.0, 1.0);
  m_vert[2].pos = FmVec2(-1.0, 0.0);
  m_vert[3].pos = FmVec2(0.0, 0.0);
}


void CPostDebug::RenderColorRT(void* pdata)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1


	CPostDebug* pthis = (CPostDebug*)pdata;

   
	/*
	static IColorRT* pColorTemp = pthis->m_pRender->CreateColorRT(320, 240, TEX_FORMAT_RGB565_UNORM, 0);

    pthis->m_pRender->CopyColorRT(pthis->m_pTexColor, pColorTemp,"Copy" );

	pthis->m_pRender->CopyColorRT(pColorTemp ,NULL,"TestDraw2D");
	return;
	*/
 
	CPostDebug::post_debug_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle(false,false,true);
	pShaderHandle->pShader->UsedShader();

	pthis->SetShaderConstValue(pShaderHandle);
	fm_uint tex_id = pthis->m_pTexColor->GetTexture();


	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

	pShaderOp->SetTexture2D(pShaderHandle->tex_ColorHandle,tex_id);

	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);
 

#undef post_effect_vertex_t_uv
#undef post_effect_vertex_t_pos

}

void CPostDebug::RenderDepthStencilRT(void* pdata)
{

#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1

	CPostDebug* pthis = (CPostDebug*)pdata;

	CPostDebug::post_debug_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle(true,false,false);
	pShaderHandle->pShader->UsedShader();

	pthis->SetShaderConstValue(pShaderHandle);
	fm_uint tex_id = pthis->m_pTexDepthStencil->GetTexture();
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

	pShaderOp->SetTexture2D(pShaderHandle->tex_DepthHandle,tex_id);

	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);


#undef post_effect_vertex_t_uv
#undef post_effect_vertex_t_pos
}


void CPostDebug::RenderDepthRT(void* pdata)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1


	CPostDebug* pthis = (CPostDebug*)pdata;
 
	/*
	static IColorRT* pColorTemp = pthis->m_pRender->CreateColorRT(1024, 768, TEX_FORMAT_RGBA8888_UNORM, 0);
	pthis->m_pTexDepth->GetTextureSampler()->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);
    pthis->m_pRender->CopyDepthRT(pthis->m_pTexDepth, pColorTemp,"Copy" );

	pthis->m_pRender->CopyColorRT(pColorTemp ,NULL,"TestDraw2D");
	return;	
    */
   

	CPostDebug::post_debug_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle(true,false,false);
	pShaderHandle->pShader->UsedShader();

	pthis->SetShaderConstValue(pShaderHandle);
	fm_uint tex_id = pthis->m_pTexDepth->GetTexture();
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

	pShaderOp->SetTexture2D(pShaderHandle->tex_DepthHandle,tex_id);

	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);


#undef post_effect_vertex_t_uv
#undef post_effect_vertex_t_pos
}

void CPostDebug::RenderTexture(void* pdata)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1
 
	CPostDebug* pthis = (CPostDebug*)pdata;
	CPostDebug::post_debug_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle(false,true,false);
	pShaderHandle->pShader->UsedShader();
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

    fm_uint tex_id = pthis->m_pTex->GetCanUseShaderTex()->GetTexture();
  	pShaderOp->SetTexture2D(pShaderHandle->tex_ColorHandle,tex_id);
 
	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);
 

#undef post_effect_vertex_t_uv
#undef post_effect_vertex_t_pos
}

void CPostDebug::RenderDecodeDepthRT(void* pdata)
{
#define post_effect_vertex_t_pos 0
#define post_effect_vertex_t_uv  1
 
	CPostDebug* pthis = (CPostDebug*)pdata;
	CPostDebug::post_debug_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle(false,false,false,true);
	pShaderHandle->pShader->UsedShader();
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

    fm_uint tex_id = pthis->m_pTexDecodeColor->GetTexture();
  	pShaderOp->SetTexture2D(pShaderHandle->tex_ColorHandle,tex_id);
 
	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);
 

#undef post_effect_vertex_t_uv
#undef post_effect_vertex_t_pos
}

// 是否是深度图
CPostDebug::post_debug_shaderhandle_t* CPostDebug::GetShaderHandle(bool bDepth,bool bTexture,bool bColor,bool bDecode )
{
	size_t shader_flag = (size_t(bDepth)<<FLAG_DEPTH) + (size_t(bTexture)<<FLAG_TEXTURE) + (size_t(bColor)<<FLAG_COLOR)
		                 + (size_t(bDecode)<<FLAG_DECODE_DEPTH);

	if(!m_ShaderMapHandle.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,post_debug_vertex_t_name,2);
		IShaderParamOp* pShaderOp = pShader->GetParamOp();
        CPostDebug::post_debug_shaderhandle_t* pShaderHandle = (CPostDebug::post_debug_shaderhandle_t*) CORE_ALLOC(sizeof(CPostDebug::post_debug_shaderhandle_t));
		pShaderHandle->pShader = pShader;
		pShaderHandle->tex_ColorHandle = pShaderOp->FindParamIdByName("tex_color");
        pShaderHandle->tex_DepthHandle = pShaderOp->FindParamIdByName("tex_depth");
		pShaderHandle->vec2UVscaleHandle = pShaderOp->FindParamIdByName("c_uvScaleRT"); 
		pShaderHandle->vec4DepthParamHandle =  pShaderOp->FindParamIdByName("c_DepthParam");
		pShaderHandle->mat4InvProjHandle = pShaderOp->FindParamIdByName("c_matInvProj");
		m_ShaderMapHandle.Add(shader_flag,pShaderHandle);
	}
	return m_ShaderMapHandle.Find(shader_flag).GetData();
}

void CPostDebug::SetShaderConstValue(CPostDebug::post_debug_shaderhandle_t* pShaderHandle)
{
	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();
	pShaderOp->SetParamValue(pShaderHandle->vec2UVscaleHandle,m_post_debug_cb.vec2scale);
	pShaderOp->SetParamValue(pShaderHandle->vec4DepthParamHandle,m_post_debug_cb.vec4DepthParam);
	pShaderOp->SetParamValue(pShaderHandle->mat4InvProjHandle,m_post_debug_cb.mat4InvProj);
}