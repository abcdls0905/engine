//--------------------------------------------------------------------
// 文件名:		sun_glow.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sun_glow.h"
#include "../visual/vis_utils.h"
#include "../visual/i_resource.h"
#include "../visual/i_context.h"
#include "../visual/i_texture.h"
#include "../visual/i_scene_view.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"

/// \file sun_glow.cpp
/// \brief 阳光

/// entity: SunGlow
/// \brief 阳光实体
/// \see 继承自IVisBase
DECLARE_ENTITY(SunGlow, 0, IVisBase);

/// property: float GlowSize
/// \brief 阳光的尺寸
DECLARE_PROPERTY(float, SunGlow, GlowSize, GetGlowSize, SetGlowSize);
/// property: float FlareSize
/// \brief 光晕的尺寸
DECLARE_PROPERTY(float, SunGlow, FlareSize, GetFlareSize, SetFlareSize);
/// property: float GlowDistance
/// \brief 阳光的距离
DECLARE_PROPERTY(float, SunGlow, GlowDistance, GetGlowDistance, 
	SetGlowDistance);
/// property: float FlareDistance
/// \brief 光晕的距离
DECLARE_PROPERTY(float, SunGlow, FlareDistance, GetFlareDistance, 
	SetFlareDistance);
/// property: float GlowRotateSpeed
/// \brief 阳光旋转速度
DECLARE_PROPERTY(float, SunGlow, GlowRotateSpeed, GetGlowRotateSpeed, 
	SetGlowRotateSpeed);
/// property: bool ShowGlow
/// \brief 是否显示阳光
DECLARE_PROPERTY(bool, SunGlow, ShowGlow, GetShowGlow, SetShowGlow);
/// property: bool ShowFlare
/// \brief 是否显示光晕
DECLARE_PROPERTY(bool, SunGlow, ShowFlare, GetShowFlare, SetShowFlare);
/// property: bool AsyncLoad
/// \brief 是否异步加载
DECLARE_PROPERTY(bool, SunGlow, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: string GlowTex
/// \brief 太阳贴图
DECLARE_PROPERTY(const char*, SunGlow, GlowTex, GetGlowTex, SetGlowTex);
/// property: string FlareTex
/// \brief 光晕贴图
DECLARE_PROPERTY(const char*, SunGlow, FlareTex, GetFlareTex, SetFlareTex);

#define sunglow_vertex_t_iPos 0
#define sunglow_vertex_t_iDiffuse 1
#define sunglow_vertex_t_iTexCoord 2

// SunGlow

SunGlow::SunGlow()
{
	m_pSunVBuffer = NULL;
	m_pVBuffer = NULL;
	m_pIBuffer = NULL;
	//m_pVSCB = NULL;
	m_pGlowTex = NULL;
	m_pFlareTex = NULL;
	m_fGlowSize = 15.0F;
	m_fFlareSize = 1.0F;
	m_fGlowDistance = 1000.0F;
	m_fFlareDistance = 1000.0F;
	m_fGlowRotateSpeed = 1.0F;
	m_bShowGlow = true;
	m_bShowFlare = true;
	m_bAsyncLoad = false;
	m_bGlowVisible = false;
	m_bFlareVisible = false;
	m_fGlowAlpha = 0.0F;
	m_fFlareAlpha = 0.0F;
	m_pGlowVS = NULL;
	m_pGlowPS = NULL;
	m_pSunVBufferData = NULL;
	m_pVBufferData = NULL;
	m_pShaderHandle = NULL;
}

SunGlow::~SunGlow()
{
	UnloadTexture();
	if(m_pSunVBufferData)
	{
		delete [] m_pSunVBufferData;
		m_pSunVBufferData = NULL;
	}

	if(m_pVBufferData)
	{
		delete [] m_pVBufferData;
		m_pVBufferData = NULL;
	}

	SAFE_RELEASE(m_pSunVBuffer);
	SAFE_RELEASE(m_pVBuffer);
	SAFE_RELEASE(m_pIBuffer);
	if(m_pShaderHandle)
	{
		SAFE_RELEASE(m_pShaderHandle->pShader);
		CORE_FREE(m_pShaderHandle,sizeof(SunGlow::sun_flare_shader_handle));
	}
	//SAFE_RELEASE(m_pVSCB);
}

const char* sunglow_vertex_t_name[] = 
{
	"iPos",
	"iDiffuse",
	"iTexCoord",
};

bool SunGlow::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}
	// 设置着色器
    m_pGlowPS  = m_pRender->LoadPixelShader("sunglow.fsh","ps_main_sunglow","");
	m_pGlowVS = m_pRender->LoadVertexShader("sunglow.vsh","vs_main_sunglow","");

	m_pShaderHandle = (SunGlow::sun_flare_shader_handle*)CORE_ALLOC(sizeof(SunGlow::sun_flare_shader_handle));
	m_pShaderHandle->pShader = m_pRender->CreateShaderProgram(m_pGlowVS,m_pGlowPS,sunglow_vertex_t_name,3);
    IShaderParamOp* pShaderOp = m_pShaderHandle->pShader->GetParamOp();

    m_pShaderHandle->tex_BaseMapHandle = pShaderOp->FindParamIdByName("tex_BaseMap");
	m_pShaderHandle->mat4mtxWVP = pShaderOp->FindParamIdByName("mtxWVP");
	return true;
}

bool SunGlow::Shut()
{
	return true;
}

bool SunGlow::GetLoadFinish()
{
	if (m_pGlowTex)
	{
		if (!m_pGlowTex->IsLoadComplete())
		{
			return false;
		}
	}

	if (m_pFlareTex)
	{
		if (!m_pFlareTex->IsLoadComplete())
		{
			return false;
		}
	}

	return true;
}

void SunGlow::SetGlowSize(float value)
{
	m_fGlowSize = value;
}

float SunGlow::GetGlowSize() const
{
	return m_fGlowSize;
}

void SunGlow::SetFlareSize(float value)
{
	m_fFlareSize = value;
}

float SunGlow::GetFlareSize() const
{
	return m_fFlareSize;
}

void SunGlow::SetGlowDistance(float value)
{
	m_fGlowDistance = value;
}

float SunGlow::GetGlowDistance() const
{
	return m_fGlowDistance;
}

void SunGlow::SetFlareDistance(float value)
{
	m_fFlareDistance = value;
}

float SunGlow::GetFlareDistance() const
{
	return m_fFlareDistance;
}

void SunGlow::SetGlowRotateSpeed(float value)
{
	m_fGlowRotateSpeed = value;
}

float SunGlow::GetGlowRotateSpeed() const
{
	return m_fGlowRotateSpeed;
}

void SunGlow::SetShowGlow(bool value)
{
	m_bShowGlow = value;
}

bool SunGlow::GetShowGlow() const
{
	return m_bShowGlow;
}

void SunGlow::SetShowFlare(bool value)
{
	m_bShowFlare = value;
}

bool SunGlow::GetShowFlare() const
{
	return m_bShowFlare;
}

void SunGlow::SetGlowTex(const char* value)
{
	Assert(value != NULL);

	m_strGlowTex = value;
}

const char* SunGlow::GetGlowTex() const
{
	return m_strGlowTex.c_str();
}

void SunGlow::SetFlareTex(const char* value)
{
	Assert(value != NULL);

	m_strFlareTex = value;
}

const char* SunGlow::GetFlareTex() const
{
	return m_strFlareTex.c_str();
}

bool SunGlow::UpdateTexture()
{
	UnloadTexture();

	return LoadTexture();
}

bool SunGlow::AddFlare(float distance, float size, int sub_tex, 
	const char* color)
{
	Assert(color != NULL);

	if (m_Flares.size() >= MAX_FLARE_NUM)
	{
		return false;
	}

	m_Flares.push_back(sun_flare_t());

	sun_flare_t& data = m_Flares.back();

	data.fDistance = distance;
	data.fSize = size * m_fFlareSize;
	data.nSubTex = sub_tex;
	data.nColor = VisUtil_ColorToInt(color);
	data.nDiffuse = 0;
	data.fAngle = 0.0F;
	data.vPos = FmVec3(0.0F, 0.0F, 0.0F);

	return true;
}

bool SunGlow::ClearFlare()
{
	m_Flares.clear();

	return true;
}

bool SunGlow::LoadTexture()
{
	if (m_bShowGlow)
	{
		// 太阳贴图
		m_pGlowTex = m_pRender->CreateTexture(m_strGlowTex.c_str(), m_bAsyncLoad);

		if (NULL == m_pGlowTex)
		{
			CORE_TRACE("(SunGlow::LoadTexture)create glow texture failed");
			CORE_TRACE(GetGlowTex());
		}
	}

	if (m_bShowFlare)
	{
		// 光晕贴图
		m_pFlareTex = m_pRender->CreateTexture(m_strFlareTex.c_str(), 
			m_bAsyncLoad);

		if (NULL == m_pFlareTex)
		{
			CORE_TRACE("(SunGlow::LoadTexture)create flare texture failed");
			CORE_TRACE(GetFlareTex());
		}
	}
	
	return true;
}

bool SunGlow::UnloadTexture()
{
	SAFE_RELEASE(m_pGlowTex);
	SAFE_RELEASE(m_pFlareTex);
	
	return true;
}

bool SunGlow::Load()
{
	m_pSunVBuffer = m_pRender->CreateDynamicVB(4 * sizeof(sunglow_vertex_t),"SunGlow::Load SunVBuffer");

	if (NULL == m_pSunVBuffer)
	{
		CORE_TRACE("(SunGlow::Load)create sun vertex buffer failed");
		return false;
	}

	m_pSunVBufferData = new sunglow_vertex_t[4];

	m_pVBuffer = m_pRender->CreateDynamicVB(
		MAX_FLARE_NUM * 4 * sizeof(sunglow_vertex_t),"SunGlow::Load SunVBufferData");

	if (NULL == m_pVBuffer)
	{
		CORE_TRACE("(SunGlow::Load)create vertex buffer failed");
		return false;
	}

	m_pVBufferData = new sunglow_vertex_t[4*MAX_FLARE_NUM];

	unsigned short ib_data[MAX_FLARE_NUM * 6];
	unsigned short* pIndex = ib_data;
	unsigned short count = 0;

	for (size_t i = 0; i < MAX_FLARE_NUM; ++i)
	{
		*pIndex++ = count + 0;
		*pIndex++ = count + 1;
		*pIndex++ = count + 2;
		*pIndex++ = count + 1;
		*pIndex++ = count + 3;
		*pIndex++ = count + 2;
		count += 4;
	}

	m_pIBuffer = m_pRender->CreateStaticIB(ib_data, sizeof(ib_data));

	if (NULL == m_pIBuffer)
	{
		CORE_TRACE("(SunGlow::Load)create index buffer failed");
		return false;
	}
	
	UnloadTexture();
	LoadTexture();

	return true;
}

// 将浮点数规范到0到1之间
static inline float clamp_float(float v)
{
	if (v > 1.0F)
	{
		return 1.0F;
	}

	if (v < 0.0F)
	{
		return 0.0F;
	}

	return v;
}
void SunGlow::Update(float seconds)
{
	float delta = seconds * 8.0F;

	if (m_bGlowVisible)
	{
		m_fGlowAlpha += delta;
	}
	else
	{
		m_fGlowAlpha -= delta;
	}

	m_fGlowAlpha = clamp_float(m_fGlowAlpha);

	if (m_bFlareVisible)
	{
		m_fFlareAlpha += delta;
	}
	else
	{
		m_fFlareAlpha -= delta;
	}

	m_fFlareAlpha = clamp_float(m_fFlareAlpha);

	IRenderContext* pContext = m_pRender->GetContext();
	//开始保存投影矩阵
	FmMat4Transpose(&m_mtxWVP,&pContext->GetCamera().mtxProj);
}

// 绕Y轴旋转
inline void rotate_around_y(float& x, float& z, float cos, float sin)
{
	float xx = x * cos + z * sin;
	float zz = z * cos - x * sin;

	x = xx;
	z = zz;
}

SunGlow::sunglow_vertex_t* SunGlow::LockVBuffer()
{
	return m_pVBufferData;
}

void SunGlow::UnLockVBuffer()
{
	m_pRender->UpdateDynamicVB(m_pVBuffer,0,m_pVBufferData,m_pVBuffer->GetSize());
}


SunGlow::sunglow_vertex_t* SunGlow::LockSunVBuffer()
{
	return m_pSunVBufferData;
}

void SunGlow::UnLockSunVBuffer()
{
	m_pRender->UpdateDynamicVB(m_pSunVBuffer,0,m_pSunVBufferData, m_pSunVBuffer->GetSize());
}

void SunGlow::RenderSun(void* pdata)
{
	SunGlow* pthis = (SunGlow*)pdata;
	IShaderParamOp* pShaderOp = pthis->m_pShaderHandle->pShader->GetParamOp();
	IRenderStateOp* pRenderStateOp = pthis->m_pShaderHandle->pShader->GetRenderStateOp();
	IRenderDrawOp*  pDrawOp = pthis->m_pShaderHandle->pShader->GetRenderDrawOp();

	pRenderStateOp->EnableBlend(true);
	//glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_ONE,GL_ONE);
	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE ,IRenderStateOp::BLEND_COLOR_ONE );
	pRenderStateOp->SetBlendEquation( IRenderStateOp::BLEND_FUNC_ADD );

	pthis->m_pShaderHandle->pShader->UsedShader();
	pShaderOp->SetTexture2D(pthis->m_pShaderHandle->tex_BaseMapHandle,pthis->m_pGlowTex->GetCanUseShaderTex()->GetTexture());
	pShaderOp->SetParamValue(pthis->m_pShaderHandle->mat4mtxWVP,pthis->m_mtxWVP);

	pDrawOp->SetVB( pthis->m_pSunVBuffer->GetBuffer());
	pDrawOp->SetIB( pthis->m_pIBuffer->GetBuffer());

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iPos,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iPos, 3 ,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), 0);

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iDiffuse,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iDiffuse, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), (void*) (3 * sizeof(float)));

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iTexCoord,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iTexCoord, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), (void*) (7 * sizeof(float)));

    fm_sizei count = pthis->m_pIBuffer->GetSize()/sizeof(unsigned short);

	pDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, 6,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);

	pRenderStateOp->EnableBlend(false);
}

// 绘制光斑
void SunGlow::RenderFlares(void* pdata)
{
	SunGlow* pthis = (SunGlow*)pdata;

	pthis->DrawFlares(&pthis->m_Flares[0],pthis->m_Flares.size(),2,2,pthis->m_pFlareTex,pthis->m_pVBuffer);
}


bool SunGlow::DrawSun(sun_flare_t* flares, size_t flare_num, 
	size_t sub_tex_x, size_t sub_tex_y, ITexture* pTexture)
{
	Assert(flares != NULL);

	IRenderContext* pContext = m_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	bool use_sub_tex = (sub_tex_x > 1) || (sub_tex_y > 1);


	sunglow_vertex_t* pVerts = (sunglow_vertex_t*)LockSunVBuffer();

	if (NULL == pVerts)
	{
		return false;
	}

	FmVec3 pos;

	for (size_t i = 0; i < flare_num; ++i)
	{
		sunglow_vertex_t* pv = &pVerts[i * 4];

		FmVec3TransformCoord(&pos, &flares[i].vPos, &mtxView);

		float size = flares[i].fSize;

		pv[0].pos = FmVec3(-size, size, 0.0F);
		pv[1].pos = FmVec3(size, size, 0.0F);
		pv[2].pos = FmVec3(-size, -size, 0.0F);
		pv[3].pos = FmVec3(size, -size, 0.0F);

		float cos_value = cosf(flares[i].fAngle);
		float sin_value = sinf(flares[i].fAngle);

		pv[0].tu = 0.0F;
		pv[0].tv = 0.0F;
		pv[1].tu = 1.0F;
		pv[1].tv = 0.0F;
		pv[2].tu = 0.0F;				
		pv[2].tv = 1.0F;
		pv[3].tu = 1.0F;		
		pv[3].tv = 1.0F;

		float dx = 1.0F;
		float dy = 1.0F;
		float sx = 0.0F;
		float sy = 0.0F;

		if (use_sub_tex)
		{
			dx = 1.0F / float(sub_tex_x);
			dy = 1.0F / float(sub_tex_y);
			sx = dx * (flares[i].nSubTex 
				- sub_tex_x * (flares[i].nSubTex / sub_tex_x));
			sy = dy * (flares[i].nSubTex / sub_tex_y);
		}

		for (size_t k = 0; k < 4; ++k) 
		{
			rotate_around_y(pv[k].pos.x, pv[k].pos.y, cos_value, sin_value);

			pv[k].pos += pos;
			pv[k].diffuse = VisUtil_ColorToVector4(flares[i].nDiffuse);

			if (use_sub_tex)
			{
				pv[k].tu = pv[k].tu * dx + sx;
				pv[k].tv = pv[k].tv * dy + sy;
			}
		}
	}

	UnLockSunVBuffer();

	if(m_pGlowTex==NULL)
		return false;
    m_pRender->GetSceneView()->AddSkyBatch(RenderSun, this,"Sun");

	return true;
}

bool SunGlow::DrawFlares(sun_flare_t* flares, size_t flare_num, 
	size_t sub_tex_x, size_t sub_tex_y, ITexture* pTexture, 
	IDynamicVB* pVBuffer)
{
    IShaderParamOp* pShaderOp = m_pShaderHandle->pShader->GetParamOp();
	IRenderDrawOp*  pDrawOp = m_pShaderHandle->pShader->GetRenderDrawOp();
	IRenderStateOp* pRenderStateOp = m_pShaderHandle->pShader->GetRenderStateOp();

	Assert(flares != NULL);
	
	if(m_pFlareTex==NULL)
		return false;
	IRenderContext* pContext = m_pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	bool use_sub_tex = (sub_tex_x > 1) || (sub_tex_y > 1);

 
	sunglow_vertex_t* pVerts = LockVBuffer();

	if (NULL == pVerts)
	{
		return false;
	}

	FmVec3 pos;

	for (size_t i = 0; i < flare_num; ++i)
	{
		sunglow_vertex_t* pv = &pVerts[i * 4];

		FmVec3TransformCoord(&pos, &flares[i].vPos, &mtxView);

		float size = flares[i].fSize;

		pv[0].pos = FmVec3(-size, size, 0.0F);
		pv[1].pos = FmVec3(size, size, 0.0F);
		pv[2].pos = FmVec3(-size, -size, 0.0F);
		pv[3].pos = FmVec3(size, -size, 0.0F);

		float cos_value = cosf(flares[i].fAngle);
		float sin_value = sinf(flares[i].fAngle);

		pv[0].tu = 0.0F;
		pv[0].tv = 0.0F;
		pv[1].tu = 1.0F;
		pv[1].tv = 0.0F;
		pv[2].tu = 0.0F;				
		pv[2].tv = 1.0F;
		pv[3].tu = 1.0F;		
		pv[3].tv = 1.0F;

		float dx = 1.0F;
		float dy = 1.0F;
		float sx = 0.0F;
		float sy = 0.0F;

		if (use_sub_tex)
		{
			dx = 1.0F / float(sub_tex_x);
			dy = 1.0F / float(sub_tex_y);
			sx = dx * (flares[i].nSubTex 
				- sub_tex_x * (flares[i].nSubTex / sub_tex_x));
			sy = dy * (flares[i].nSubTex / sub_tex_y);
		}

		for (size_t k = 0; k < 4; ++k) 
		{
			rotate_around_y(pv[k].pos.x, pv[k].pos.y, cos_value, sin_value);
			pv[k].pos += pos;
			pv[k].diffuse = VisUtil_ColorToVector4(flares[i].nDiffuse);

			if (use_sub_tex)
			{
				pv[k].tu = pv[k].tu * dx + sx;
				pv[k].tv = pv[k].tv * dy + sy;
			}
		}
	}

	UnLockVBuffer();

	//glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_ONE,GL_ONE);
	pRenderStateOp->SetBlendEquation( IRenderStateOp::BLEND_FUNC_ADD );
	pRenderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE ,IRenderStateOp::BLEND_COLOR_ONE );

	m_pShaderHandle->pShader->UsedShader();
	pShaderOp->SetTexture2D(m_pShaderHandle->tex_BaseMapHandle,pTexture->GetCanUseShaderTex()->GetTexture());
	pShaderOp->SetParamValue(m_pShaderHandle->mat4mtxWVP,m_mtxWVP);

	pDrawOp->SetVB( pVBuffer->GetBuffer());
	pDrawOp->SetIB( m_pIBuffer->GetBuffer());

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iPos,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iPos, 3 ,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), 0);

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iDiffuse,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iDiffuse, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), (void*) (3 * sizeof(float)));

	pDrawOp->EnableVertexAttribArray( sunglow_vertex_t_iTexCoord,true);
	pDrawOp->SetVertexAttribPointer( sunglow_vertex_t_iTexCoord, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sunglow_vertex_t), (void*) (7 * sizeof(float)));
 
	pDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,(unsigned int)flare_num*6,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);


	return true;
}

void SunGlow::Realize()
{
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 camera_pos = pContext->GetCamera().vPosition;
	FmVec3 camera_angle = pContext->GetCamera().vAngle;
	FmVec3 sun_dir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);
	FmVec3 sun_pos = camera_pos + (sun_dir * m_fGlowDistance);
	bool sun_visible = true;
	bool flare_visible = true;
	const FmPlane* planes = pContext->GetClipPlanes();
	
	if (planes)
	{
		FmVec3 v_sun = sun_dir * m_fGlowDistance;
		
		for (size_t i = 0; i < 4; ++i)
		{
			float len = planes[i].a * v_sun.x + planes[i].b * v_sun.y
				+ planes[i].c * v_sun.z;
			
			if (len > m_fGlowSize)
			{
				sun_visible = false;
			}
			
			if (len > 0.0F)
			{ 
				flare_visible = false;
				break; 
			}
		}
	}

	if (!sun_visible)
	{
		// 太阳不可见
		return;
	}

	// 设置角色的碰撞检测屏蔽
	bool old_mask = false;

	IEntity* pTerrain = pContext->GetUnique("Terrain");


	// 测试碰撞
	int old_alpha_ref = m_pRender->GetTraceAlphaRef();

	// 穿透树叶等物体
	m_pRender->SetTraceAlphaRef(255);

	FmVec3 src = camera_pos;
	FmVec3 dst = src + sun_dir * 1000.0F;
	bool hitted = pContext->LayerTraceHitted("sun_trace", src, dst);

	m_bGlowVisible = true;
	m_bFlareVisible = flare_visible && (!hitted);

	m_pRender->SetTraceAlphaRef(old_alpha_ref);

	
	// 画太阳
	if (m_bShowGlow)
	{
		// 阳光颜色
		unsigned int diffuse_color = pContext->GetColor(
			IRenderContext::C_DIFFUSE_COLOR);
		FmVec3 vGlowColor = VisUtil_ColorToVector(diffuse_color);
		// 摄像机与阳光的夹角
		FmVec3 vSunDir = FmVec3(sun_dir.x, 0.0F, sun_dir.z);
		FmVec3 vCamDir = FmVec3(cosf(camera_angle.y), 
			0.0F, sinf(camera_angle.y));
		float angle = FmVec3Dot(&vSunDir, &vCamDir);
		FmVec3 vColor = vGlowColor * m_fGlowAlpha;
		sun_flare_t flare;
		
		memset(&flare, 0, sizeof(flare));
		flare.nDiffuse = VisUtil_VectorToColor(vColor);
		flare.vPos = sun_pos;
		flare.fAngle = angle;
		flare.fSize = m_fGlowSize;
		
		DrawSun(&flare, 1, 1, 1, m_pGlowTex);
	}
	
	// 画光晕
	if (m_bShowFlare && (!m_Flares.empty()))
	{
		const FmMat4& mtxView = pContext->GetCamera().mtxView;
		FmVec3 cam_pos = pContext->GetCamera().vPosition;

		float pos_x = cam_pos.x * mtxView._11 + cam_pos.y * mtxView._12 
			+ cam_pos.z * mtxView._13;
		float pos_y = cam_pos.x * mtxView._21 + cam_pos.y * mtxView._22 
			+ cam_pos.z * mtxView._23;
		float pos_z = cam_pos.x * mtxView._31 + cam_pos.y * mtxView._32 
			+ cam_pos.z * mtxView._33;

		float flare_dist = m_fFlareDistance;
		FmVec3 v_cen_pos;

		v_cen_pos.x = mtxView._13 * flare_dist * 0.5F - pos_x;
		v_cen_pos.y = mtxView._23 * flare_dist * 0.5F - pos_y;
		v_cen_pos.z = mtxView._33 * flare_dist * 0.5F - pos_z;
 
		FmVec3 v_delta = v_cen_pos - sun_pos;
			
		FmVec3Normalize(&v_delta, &v_delta);
		v_delta *= flare_dist;

		float alpha = m_fGlowAlpha * m_fFlareAlpha;
		size_t flare_num = m_Flares.size();
		
		for (size_t i = 0; i < flare_num; ++i)
		{
			unsigned int color = m_Flares[i].nColor;
			unsigned int r = (unsigned int)(alpha	* ((color & 0xFF0000) >> 16));
			unsigned int g = (unsigned int)(alpha	* ((color & 0xFF00) >> 8));
			unsigned int b = (unsigned int)(alpha	* ((color & 0xFF) >> 0));
			
			m_Flares[i].nDiffuse = COLOR_ARGB(255, r, g, b);
			m_Flares[i].fAngle = 0.0F;
			m_Flares[i].vPos = sun_pos 
				+ v_delta * (1.0F - m_Flares[i].fDistance);
		}

        m_pRender->GetSceneView()->AddTopmostBatch(RenderFlares, this,"SunFlares");		
	}
}
