//--------------------------------------------------------------------
// 文件名:		sky_box.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sky_box.h"
#include "../visual/i_texture.h"
#include "../visual/i_resource.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"


/// \file sky_box.cpp
/// \brief 天空盒

/// entity: SkyBox
/// \brief 天空盒实体
/// \see 继承自IVisBase
DECLARE_ENTITY(SkyBox, 0, IVisBase);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, SkyBox, Visible, GetVisible, SetVisible);
/// property: bool AsyncLoad
/// \brief 是否异步加载贴图
DECLARE_PROPERTY(bool, SkyBox, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ZEnable
/// \brief 是否深度测试
DECLARE_PROPERTY(bool, SkyBox, ZEnable, GetZEnable, SetZEnable);
/// property: float MulFactor
/// \brief 乘法缩放因子
DECLARE_PROPERTY(float, SkyBox, MulFactor, GetMulFactor, SetMulFactor);
/// property: float YawSpeed
/// \brief 旋转速度
DECLARE_PROPERTY(float, SkyBox, YawSpeed, GetYawSpeed, SetYawSpeed);


/// property: string UpTex
/// \brief 天空顶贴图
DECLARE_PROPERTY(const char*, SkyBox, UpTex, GetUpTex, SetUpTex);

/// property: string SideTex
/// \brief 天空四周贴图
DECLARE_PROPERTY(const char*, SkyBox, SideTex, GetSideTex, SetSideTex);

/*
/// property: string FadeInUpTex
/// \brief 淡入的天空顶贴图
DECLARE_PROPERTY(const char*, SkyBox, FadeInUpTex, GetFadeInUpTex, 
	SetFadeInUpTex);
/// property: string FadeInSideTex
/// \brief 淡入的天空四周贴图
DECLARE_PROPERTY(const char*, SkyBox, FadeInSideTex, GetFadeInSideTex, 
	SetFadeInSideTex);
*/

/// property: float FadeInTime
/// \brief 淡入的时间
DECLARE_PROPERTY(float, SkyBox, FadeInTime, GetFadeInTime, SetFadeInTime);

/// readonly: bool FadeInFinish
/// \brief 是否渐变结束
DECLARE_PROPERTY_GET(bool, SkyBox, FadeInFinish, GetFadeInFinish);

#define INIT_SKY_VERTEX(p, pos1, tu1, tv1) \
	{ (p)->pos = pos1; (p)->tu = tu1; (p)->tv = tv1; }




#define VERTEX_ARRAY	0
#define TEXCOORD_ARRAY	1


const char* sky_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};


static inline bool is_cub_tex(const char* tex_name)
{
	const char* slash = strrchr(tex_name, '\\');

	if (slash)
	{
		const char* tex_name = slash + 1;

		if ((strlen(tex_name) >= 4) && (strnicmp(tex_name, "cub_", 4) == 0))
		{
			return true;
		}
	}

	return false;
}

ITexture* create_tex(IRender* pRender, const char* tex_name, bool is_cub, 
	bool asy_load)
{
	if (is_cub)
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", tex_name,pRender->GetTexExtfix());

		return pRender->CreateCubeTexture(fname, asy_load);
	}
	else
	{
		return pRender->CreateTexture(tex_name, asy_load);
	}
}

// SkyBox

SkyBox::SkyBox()
{
	m_pRender = NULL;

	m_pVBuffer = NULL;
	m_pIBuffer = NULL;

	m_pSideTex = NULL;
	m_pUpTex = NULL;
	/*
	m_pSideTex = NULL;
	m_pFadeInUpTex = NULL;
	m_pFadeInSideTex = NULL;
	*/

	m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_vAngle = FmVec3(0.0F, 0.0F, 0.0F);
	m_bVisible = true;
	m_bAsyncLoad = true;
	m_bZEnable = true;
	m_bCubeMap = false;
	m_fMulFactor = 600;
	m_fYawSpeed = 0.00F;
	m_fFadeInTime = 0.0F;
	m_fFadeInElapseTime = 0.0F;
 

	m_strSideTex = "tex\\sky_side.dds";
	m_strUpTex = "tex\\sky_up.dds";
	//m_strUpTex = "tex\\cub_sky.pvr";
	//m_bCubeMap = true;

	/*
	m_strFadeInUpTex = "";
	m_strFadeInSideTex = "";
	*/
}

SkyBox::~SkyBox()
{
	Unload();
}

bool SkyBox::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}

	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("sky_box.vsh", "vs_main_sky_box");
	m_VSList.SetDefine(FLAG_CUBEMAP,"#define CUBEMAP 1\n");
	m_VSList.SetDefine(FLAG_FOG,"#define FOG 1\n");

	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("sky_box.fsh", "ps_main_sky_box");
	m_PSList.SetDefine(FLAG_CUBEMAP,"#define CUBEMAP 1\n");
	m_PSList.SetDefine(FLAG_FOG,"#define FOG 1\n");


	CVar prop;
	//检查是否支持纹理贴图
	m_pRender->GetCore()->GetProperty(
		m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID()),
		"IsTegraGPU", prop);
	m_bNvGPU = prop.BoolVal();

	return true;
}

bool SkyBox::Shut()
{
	SAFE_RELEASE(m_pRender);
	
	return true;
}

bool SkyBox::GetLoadFinish()
{
	/*
	if (m_pUpTex)
	{
		if (!m_pUpTex->IsLoadComplete())
		{
			return false;
		}
	}

	if (m_pSideTex)
	{
		if (!m_pSideTex->IsLoadComplete())
		{
			return false;
		}
	}

	if (m_pFadeInUpTex)
	{
		if (!m_pFadeInUpTex->IsLoadComplete())
		{
			return false;
		}
	}

	if (m_pFadeInSideTex)
	{
		if (!m_pFadeInSideTex->IsLoadComplete())
		{
			return false;
		}
	}
	*/

	return true;
}

bool SkyBox::SetPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;

	return true;
}

FmVec3 SkyBox::GetPosition() const
{ 
	return m_vPosition; 
}

bool SkyBox::SetAngle(float x, float y, float z)
{
	m_vAngle.x = x;
	m_vAngle.y = y;
	m_vAngle.z = z;

	return true;
}

FmVec3 SkyBox::GetAngle() const
{ 
	return m_vAngle; 
}

void SkyBox::SetVisible(bool value)
{
	m_bVisible = value;
}

bool SkyBox::GetVisible() const
{
	return m_bVisible;
}

void SkyBox::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool SkyBox::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void SkyBox::SetZEnable(bool value)
{
	m_bZEnable = value;
}

bool SkyBox::GetZEnable() const
{
	return m_bZEnable;
}

void SkyBox::SetMulFactor(float value)
{
	m_fMulFactor = value;
}

float SkyBox::GetMulFactor() const
{
	return m_fMulFactor;
}

void SkyBox::SetYawSpeed(float value)
{
	m_fYawSpeed = value;
}

float SkyBox::GetYawSpeed() const
{
	return m_fYawSpeed;
}


void SkyBox::SetUpTex(const char* value)
{
	Assert(value != NULL);

	m_strUpTex = value;
}

const char* SkyBox::GetUpTex() const
{
	return m_strUpTex.c_str();
}


void SkyBox::SetSideTex(const char* value)
{
	Assert(value != NULL);

	m_strSideTex = value;
}

const char* SkyBox::GetSideTex() const
{
	return m_strSideTex.c_str();
}

/*
void SkyBox::SetFadeInUpTex(const char* value)
{
	Assert(value != NULL);

	if (0 == m_strFadeInUpTex.compare(value))
	{
		return;
	}

	bool is_cub_map = is_cub_tex(value);

	if (is_cub_map != m_bCubeMap)
	{
		CORE_TRACE("(SkyBox::SetFadeInUpTex)texture type not match");
		return;
	}

	if (m_pFadeInUpTex)
	{
		if (m_pUpTex && (0 == stricmp(m_pUpTex->GetName(), value)))
		{
			ITexture* pTemp = m_pFadeInUpTex;

			m_pFadeInUpTex = m_pUpTex;
			m_pUpTex = pTemp;
			m_strFadeInUpTex = value;
			return;
		}
		else
		{
			// 如果正在淡入，则直接切换
			SAFE_RELEASE(m_pUpTex);
			m_pUpTex = m_pFadeInUpTex;
		}
	}

	m_pFadeInUpTex = create_tex(m_pRender, value, is_cub_map, m_bAsyncLoad);

	if (NULL == m_pFadeInUpTex)
	{
		CORE_TRACE("(SkyBox::SetFadeInUpTex)create texture failed");
		return;
	}

	m_strFadeInUpTex = value;
}

const char* SkyBox::GetFadeInUpTex() const
{
	return m_strFadeInUpTex.c_str();
}

void SkyBox::SetFadeInSideTex(const char* value)
{
	Assert(value != NULL);

	if (0 == m_strFadeInSideTex.compare(value))
	{
		return;
	}

	bool is_cub_map = is_cub_tex(value);

	if (is_cub_map != m_bCubeMap)
	{
		CORE_TRACE("(SkyBox::SetFadeInSideTex)texture type not match");
		return;
	}

	if (m_pFadeInSideTex)
	{
		if (m_pSideTex && (0 == stricmp(m_pSideTex->GetName(), value)))
		{
			ITexture* pTemp = m_pFadeInSideTex;
			
			m_pFadeInSideTex = m_pSideTex;
			m_pSideTex = pTemp;
			m_strFadeInSideTex = value;
			return;
		}
		else
		{
			// 如果正在淡入，则直接切换
			SAFE_RELEASE(m_pSideTex);
			m_pSideTex = m_pFadeInSideTex;
		}
	}

	if (!is_cub_map)
	{
		m_pFadeInSideTex = create_tex(m_pRender, value, is_cub_map, 
			m_bAsyncLoad);

		if (NULL == m_pFadeInSideTex)
		{
			CORE_TRACE("(SkyBox::SetFadeInSideTex)create texture failed");
			return;
		}
	}

	m_strFadeInSideTex = value;
}

const char* SkyBox::GetFadeInSideTex() const
{
	return m_strFadeInSideTex.c_str();
}
*/

void SkyBox::SetFadeInTime(float value)
{
	m_fFadeInTime = value;
	m_fFadeInElapseTime = 0.0F;
}

float SkyBox::GetFadeInTime() const
{
	return m_fFadeInTime;
}

bool SkyBox::GetFadeInFinish() const
{
	return m_fFadeInElapseTime <= 0.0F;
}

bool SkyBox::UpdateTexture()
{
	UnloadTexture();

	return LoadTexture();
}

bool SkyBox::LoadTexture()
{

	const char* up_tex = GetUpTex();

	m_bCubeMap = is_cub_tex(up_tex);

	if (m_bCubeMap)
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s", up_tex);
		m_pUpTex = m_pRender->CreateCubeTexture(fname, m_bAsyncLoad);
		ITextureSampler* pCubeTexSampler = m_pUpTex->GetCanUseShaderTex()->GetTextureSampler();
		pCubeTexSampler->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);
		pCubeTexSampler->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_CLAMP_TO_EDGE);
	}
	else
	{
		// 天空顶贴图
		m_pUpTex = m_pRender->CreateTexture(up_tex, m_bAsyncLoad);
		ITextureSampler* pUpTexSamper = m_pUpTex->GetCanUseShaderTex()->GetTextureSampler();
		pUpTexSamper->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);
		pUpTexSamper->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);

		// 天空四周贴图
		m_pSideTex = m_pRender->CreateTexture(GetSideTex(), m_bAsyncLoad);

		ITextureSampler* pSideTexSamper = m_pSideTex->GetCanUseShaderTex()->GetTextureSampler();
		pSideTexSamper->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);
		pSideTexSamper->SetTextureUVWrapMode(ITextureSampler::TWM_REPEAT,ITextureSampler::TWM_REPEAT);

		if (NULL == m_pSideTex)
		{
			CORE_TRACE("(SkyBox::LoadTexture)create side texture failed");
			CORE_TRACE(GetSideTex());
		}
	}

	if (NULL == m_pUpTex)
	{
		CORE_TRACE("(SkyBox::LoadTexture)create up texture failed");
		CORE_TRACE(GetUpTex());
	}
	
	return true;
}

bool SkyBox::UnloadTexture()
{
	SAFE_RELEASE(m_pSideTex);
	SAFE_RELEASE(m_pUpTex);
	/*
	SAFE_RELEASE(m_pFadeInUpTex);
	SAFE_RELEASE(m_pFadeInSideTex);
	*/
	return true;
}

void SkyBox::UpdataRealize(sky_cb_object_t& renderdata)
{
	IRenderContext* pContext = m_pRender->GetContext();	 
	// 相对坐标参考位置
	const FmVec3& refer_pos = pContext->GetReferPosition();
	FmVec3 angle = m_vAngle;
	FmMat4 mtxWorld;

	// 加上预测的角度偏移值
	angle.y = fmodf(m_vAngle.y + m_fYawSpeed * m_offset_seconds, 
		FLOAT_PI * 2.0F);
	FmMat4RotationYawPitchRoll(&mtxWorld, angle.y, angle.x, angle.z);
	mtxWorld._41 = m_vPosition.x - refer_pos.x;
	mtxWorld._42 = m_vPosition.y - refer_pos.y;
	mtxWorld._43 = m_vPosition.z - refer_pos.z;

	FmMat4Multiply(&m_matWVP, &mtxWorld, 
		&pContext->GetCamera().mtxViewProjRelative);

	FmMat4Transpose(&m_matWVP, &m_matWVP);
 
	bool fog_enable = pContext->GetInt( IRenderContext::I_FOG_LINEAR )!=0;
	m_bIsEnablefog = fog_enable;

	size_t shader_flag = (size_t(fog_enable) << FLAG_FOG) + (size_t(m_bCubeMap) << FLAG_CUBEMAP);

    sky_box_filter_shaderhandle* pShaderHandle;
	if(!m_ShaderMap.Exists(shader_flag))
	{
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,sky_vertex_t_name,2);

        pShaderHandle = (sky_box_filter_shaderhandle*) CORE_ALLOC(sizeof(sky_box_filter_shaderhandle));
        pShaderHandle->pShader = pShader;
	 	pShaderHandle->hFogColor = pShader->GetParamOp()->FindParamIdByName("c_FogColor");
		pShaderHandle->hFogHeight = pShader->GetParamOp()->FindParamIdByName("c_FogHeight");
		pShaderHandle->hMat4PMV =  pShader->GetParamOp()->FindParamIdByName("mat4PMV");
		pShaderHandle->hTexSide = pShader->GetParamOp()->FindParamIdByName("tex_sky");
		pShaderHandle->hTexUp =  pShader->GetParamOp()->FindParamIdByName("tex_sky");
        m_ShaderMap.Add(shader_flag,pShaderHandle);
	} 

	pShaderHandle = m_ShaderMap.Find(shader_flag).GetData();

	if(fog_enable)
	{
		//FmVec4(0.180144, 0.22331, 0.453456, 1);
		m_fogcolor =  pContext->GetVector4(IRenderContext::V_FOG_CURRENT);
		m_fogheight = pContext->GetFloat(IRenderContext::F_FOG_HEIGHT) / 1000.0F;
	}

	renderdata.bCubeMap = m_bCubeMap;
	renderdata.bIsEnablefog = m_bIsEnablefog;
	renderdata.bZEnable = m_bZEnable;
	renderdata.c_FogColor = m_fogcolor;
	renderdata.c_FogHeight = m_fogheight;
	renderdata.pIBuffer = m_pIBuffer;
	renderdata.pVBuffer = m_pVBuffer;
	renderdata.pShaderHandle = pShaderHandle;
	renderdata.pTexSide = m_pSideTex;
	renderdata.pTexUp = m_pUpTex;
	renderdata.c_mat4PMV = m_matWVP;
}



bool SkyBox::Load()
{
	Unload();	

	LoadTexture();
	
	// 初始化顶点数据
	sky_vertex_t vertex[40];
	FmVec3 pos[8];

	pos[0] = FmVec3(-1.0F, 0.0F, -1.0F);
	pos[1] = FmVec3(+1.0F, 0.0F, -1.0F);
	pos[2] = FmVec3(+1.0F, 0.0F, +1.0F);
	pos[3] = FmVec3(-1.0F, 0.0F, +1.0F);
	pos[4] = FmVec3(-1.0F, 1.0F, -1.0F);
	pos[5] = FmVec3(+1.0F, 1.0F, -1.0F);
	pos[6] = FmVec3(+1.0F, 1.0F, +1.0F);
	pos[7] = FmVec3(-1.0F, 1.0F, +1.0F);

	float delta = 1.0F / 1024.0F;
	float fpdx = 0.25F - delta;
	float fp0 = delta;
	float fp1 = 0.25F + delta;
	float fp2 = 0.5F + delta;
	float fp3 = 0.75F + delta;
	float fp4 = 1.0F - delta;

	// 天空顶
	INIT_SKY_VERTEX(&vertex[0], pos[5], fp0, fp0);
	INIT_SKY_VERTEX(&vertex[1], pos[6], fp0, fp4);
	INIT_SKY_VERTEX(&vertex[2], pos[7], fp4, fp4);
	INIT_SKY_VERTEX(&vertex[3], pos[4], fp4, fp0);

	for (size_t k = 0; k < 4; ++k)
	{
		vertex[k].pos *= m_fMulFactor;
		vertex[k + 4] = vertex[k];
		vertex[k + 4].pos.y = -vertex[k + 4].pos.y;
	}

	// 四周
	INIT_SKY_VERTEX(&vertex[8], pos[1], fp4, fp0 + fpdx);
	INIT_SKY_VERTEX(&vertex[9], pos[5], fp4, fp0);
	INIT_SKY_VERTEX(&vertex[10], pos[4], fp0, fp0);
	INIT_SKY_VERTEX(&vertex[11], pos[0], fp0, fp0 + fpdx);
	
	INIT_SKY_VERTEX(&vertex[12], pos[2], fp4, fp1 + fpdx);
	INIT_SKY_VERTEX(&vertex[13], pos[6], fp4, fp1);
	INIT_SKY_VERTEX(&vertex[14], pos[5], fp0, fp1);
	INIT_SKY_VERTEX(&vertex[15], pos[1], fp0, fp1 + fpdx);

	INIT_SKY_VERTEX(&vertex[16], pos[3], fp4, fp2 + fpdx);
	INIT_SKY_VERTEX(&vertex[17], pos[7], fp4, fp2);
	INIT_SKY_VERTEX(&vertex[18], pos[6], fp0, fp2);
	INIT_SKY_VERTEX(&vertex[19], pos[2], fp0, fp2 + fpdx);

	INIT_SKY_VERTEX(&vertex[20], pos[0], fp4, fp3 + fpdx);
	INIT_SKY_VERTEX(&vertex[21], pos[4], fp4, fp3);
	INIT_SKY_VERTEX(&vertex[22], pos[7], fp0, fp3);
	INIT_SKY_VERTEX(&vertex[23], pos[3], fp0, fp3 + fpdx);

	for (size_t i = 0; i < 16; ++i)
	{
		vertex[i + 8].pos *= m_fMulFactor;
		vertex[i + 24] = vertex[i + 8];
		vertex[i + 24].pos.y = -vertex[i + 24].pos.y;//这里就是给顶点做镜像
	}

	// 创建顶点缓冲
	m_pVBuffer = m_pRender->CreateStaticVB(vertex, sizeof(vertex));
	
	if (NULL == m_pVBuffer)
	{
		CORE_TRACE("(SkyBox::Load)CreateVertexBuffer failed");
		return false;
	}


	// 创建索引缓冲
	unsigned short indices[20 * 3 + 6];
	unsigned short* pIndex = indices;

	 
	for (size_t i = 0; i < 6; ++i)
	{
		*pIndex++ = (unsigned short)(i * 4) + 0;
		*pIndex++ = (unsigned short)(i * 4) + 1;
		*pIndex++ = (unsigned short)(i * 4) + 2;
		*pIndex++ = (unsigned short)(i * 4) + 0;
		*pIndex++ = (unsigned short)(i * 4) + 2;
		*pIndex++ = (unsigned short)(i * 4) + 3;
	}

	for (size_t i = 6; i < 10; ++i)
	{
		*pIndex++ = (unsigned short)(i * 4) + 0;
		*pIndex++ = (unsigned short)(i * 4) + 2;
		*pIndex++ = (unsigned short)(i * 4) + 1;
		*pIndex++ = (unsigned short)(i * 4) + 0;
		*pIndex++ = (unsigned short)(i * 4) + 3;
		*pIndex++ = (unsigned short)(i * 4) + 2;
	}

    //下面一段代码，是将天空和底边
	*pIndex++ = (unsigned short)6;
	*pIndex++ = (unsigned short)5;
	*pIndex++ = (unsigned short)4;
	*pIndex++ = (unsigned short)7;
	*pIndex++ = (unsigned short)6;
	*pIndex++ = (unsigned short)4;


	m_pIBuffer = m_pRender->CreateStaticIB(indices, sizeof(indices));

	if (NULL == m_pIBuffer)
	{
		CORE_TRACE("(SkyBox::Load)CreateIndexBuffer failed");
		return false;
	}



	return true;
}

bool SkyBox::Unload()
{
	/*
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	*/

	SAFE_RELEASE(m_pVBuffer);
	SAFE_RELEASE(m_pIBuffer);

	UnloadTexture();
	m_fFadeInTime = 0.0F;
	m_fFadeInElapseTime = 0.0F;

#define shadermap 	TPodHashMap<unsigned int, sky_box_filter_shaderhandle*, TPodTraits<unsigned int>,TSkyBoxShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMap.Begin(); iter != m_ShaderMap.End(); iter++ )
	{
        iter.GetData()->pShader->Release();
        CORE_FREE(iter.GetData(),sizeof(sky_box_filter_shaderhandle));
	}   
#undef shadermap
    m_ShaderMap.Clear();

	return true;
}

void SkyBox::Update(float seconds)
{

	// 跟随摄象机移动
	m_vPosition = m_pRender->GetContext()->GetCamera().vPosition;
	m_vPosition.y /= 4.0F;

	// 旋转
	if (m_fYawSpeed != 0.0F)
	{
		m_vAngle.y = fmodf(m_vAngle.y + m_fYawSpeed * seconds, 
			FLOAT_PI * 2.0F);
	}


	m_offset_seconds = seconds;
}

void SkyBox::Realize()
{
	//这里做一个优化检查
	if(m_bNvGPU)
	{
		m_pRender->GetSceneView()->GetContext()->SetEnableClearColorOptimize(true);
	}
	if (!m_bVisible)
	{
		m_pRender->GetSceneView()->GetContext()->SetEnableClearColorOptimize(false);
		return;
	}


	// 获取当前视图模式
	ISceneView::SCENE_VIEW_TYPE scene_view_type = m_pRender->GetSceneView()->GetSceneViewType();
	switch(scene_view_type)
	{
	case ISceneView::TYPE_NORMAL:
		RealizeNormal();
		break;
	case ISceneView::TYPE_REFLECTION:
		RealizeReflect();
		break;
	default:
        break;
	}

}

//正常绘制天空
void SkyBox::RealizeNormal()
{
    UpdataRealize(m_Realize);

	if( m_bCubeMap )
		m_pRender->GetSceneView()->AddSkyBatch(RenderCub_Sky, &m_Realize,"SkyBox");
	else
	 	m_pRender->GetSceneView()->AddSkyBatch(RenderSky, &m_Realize,"SkyBox");
}

//绘制天空倒影
void SkyBox::RealizeReflect()
{
    UpdataRealize(m_RealizeReflect);

	if( m_bCubeMap )
		m_pRender->GetSceneView()->AddSkyBatch(RenderCub_Sky, &m_RealizeReflect,"SkyBox");
	else
	 	m_pRender->GetSceneView()->AddSkyBatch(RenderSky, &m_RealizeReflect,"SkyBox");
}


void SkyBox::RenderCub_Sky(void* pdata)
{
	sky_cb_object_t* pthis = (sky_cb_object_t*)pdata;
    pthis->pShaderHandle->pShader->UsedShader();

    IShaderParamOp* pShader = pthis->pShaderHandle->pShader->GetParamOp();
	IRenderDrawOp*  pDrawOp = pthis->pShaderHandle->pShader->GetRenderDrawOp();
	pShader->SetParamValue(pthis->pShaderHandle->hMat4PMV,pthis->c_mat4PMV,false);

	if(pthis->bIsEnablefog)
	{
		FmVec4& fogcolor = pthis->c_FogColor;
		float   fogheight = pthis->c_FogHeight;
		pShader->SetParamValue(pthis->pShaderHandle->hFogColor,fogcolor);
		pShader->SetParamValue(pthis->pShaderHandle->hFogHeight,fogheight);
	}

	pShader->SetTextureCube(pthis->pShaderHandle->hTexUp,pthis->pTexUp->GetCanUseShaderTex()->GetTexture());

	pDrawOp->SetVB(pthis->pVBuffer->GetBuffer());
	pDrawOp->SetIB(pthis->pIBuffer->GetBuffer());

	pDrawOp->EnableVertexAttribArray(VERTEX_ARRAY,true);
	pDrawOp->SetVertexAttribPointer(VERTEX_ARRAY, 3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sky_vertex_t), 0);
	pDrawOp->EnableVertexAttribArray(TEXCOORD_ARRAY,true);
	pDrawOp->SetVertexAttribPointer(TEXCOORD_ARRAY, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sky_vertex_t), (void*) (3 * sizeof(float)));

    fm_sizei count = pthis->pIBuffer->GetSize()/sizeof(unsigned short);

	pDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);
}

void SkyBox::RenderSky(void* pdata)
{
	sky_cb_object_t* pthis = (sky_cb_object_t*)pdata;
    IShaderParamOp* pShader = pthis->pShaderHandle->pShader->GetParamOp();
	IRenderDrawOp*  pDrawOp = pthis->pShaderHandle->pShader->GetRenderDrawOp();

	pthis->pShaderHandle->pShader->UsedShader();

	pShader->SetTexture2D(pthis->pShaderHandle->hTexUp,pthis->pTexUp->GetCanUseShaderTex()->GetTexture());
	
	pShader->SetParamValue(pthis->pShaderHandle->hMat4PMV,pthis->c_mat4PMV,false);

	if(pthis->bIsEnablefog)
	{
		FmVec4& fogcolor = pthis->c_FogColor;
		float   fogheight = pthis->c_FogHeight;
         
		pShader->SetParamValue(pthis->pShaderHandle->hFogColor,fogcolor);
		pShader->SetParamValue(pthis->pShaderHandle->hFogHeight,fogheight);
	}

	pDrawOp->SetVB( pthis->pVBuffer->GetBuffer());
	pDrawOp->SetIB( pthis->pIBuffer->GetBuffer());

	pDrawOp->EnableVertexAttribArray( VERTEX_ARRAY ,true);
	pDrawOp->SetVertexAttribPointer( VERTEX_ARRAY, 3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sky_vertex_t), 0);
	pDrawOp->EnableVertexAttribArray( TEXCOORD_ARRAY ,true);
    pDrawOp->SetVertexAttribPointer( TEXCOORD_ARRAY, 2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(sky_vertex_t), (void*) (3 * sizeof(float)));
	//先绘制顶
	pDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,6,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);
	
    fm_sizei count = pthis->pIBuffer->GetSize()/sizeof(unsigned short)-6;	
	pShader->SetTexture2D(pthis->pShaderHandle->hTexSide,pthis->pTexSide->GetCanUseShaderTex()->GetTexture());
	//绘制4周
	pDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES,count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[6]);
	
}
