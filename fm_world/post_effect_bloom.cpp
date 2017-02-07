#include "post_effect_filter.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"

DECLARE_ENTITY(CPostEffectFilter, 0, CPostEffect);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, CPostEffectFilter, Visible, GetVisible, SetVisible);

/// property: bool GradualEnable
/// \brief 是否开启垂直颜色渐变效果
DECLARE_PROPERTY(bool, CPostEffectFilter, GradualEnable, GetGradualEnable, 
	SetGradualEnable);
/// property: float GetMaxBrightness
/// \brief 颜色渐变起始
DECLARE_PROPERTY(float, CPostEffectFilter, GradualStart, GetGradualStart, 
	SetGradualStart);
/// property: float GetGradualEnd
/// \brief 颜色渐变结束
DECLARE_PROPERTY(float, CPostEffectFilter, GradualEnd, GetGradualEnd, SetGradualEnd);
/// property: float GetGradualExp
/// \brief 颜色衰减系数
DECLARE_PROPERTY(float, CPostEffectFilter, GradualExp, GetGradualExp, SetGradualExp);
/// property: int GetGradualColor
/// \brief 渐变的颜色
DECLARE_PROPERTY(result_string,CPostEffectFilter, GradualColor, GetGradualColor, 
	SetGradualColor);

/// property: bool AdjustEnable
/// \brief 是否开启颜色调节效果
DECLARE_PROPERTY(bool, CPostEffectFilter, AdjustEnable, GetAdjustEnable, 
	SetAdjustEnable);
// 色彩调整 : x-亮度，y-对比度，z-饱和度
/// property: float AdjustBrightness
/// \brief 亮度
DECLARE_PROPERTY(float, CPostEffectFilter, AdjustBrightness, GetAdjustBrightness, 
	SetAdjustBrightness);
/// property: float AdjustContrast
/// \brief 对比度
DECLARE_PROPERTY(float, CPostEffectFilter, AdjustContrast, GetAdjustContrast, 
	SetAdjustContrast);
/// property: float AdjustSaturation
/// \brief 饱和度
DECLARE_PROPERTY(float, CPostEffectFilter, AdjustSaturation, GetAdjustSaturation, 
	SetAdjustSaturation);
/// property: int AdjustBaseColor
/// \brief 基色
DECLARE_PROPERTY(result_string, CPostEffectFilter, AdjustBaseColor, GetAdjustBaseColor, 
	SetAdjustBaseColor);

// property: bool AngleEnable
/// \brief 是否开启中心颜色渐变效果
DECLARE_PROPERTY(bool, CPostEffectFilter, AngleEnable, GetAngleEnable, SetAngleEnable);
/// property: float AngleStart
/// \brief 颜色渐变起始
DECLARE_PROPERTY(float, CPostEffectFilter, AngleStart, GetAngleStart, SetAngleStart);
/// property: float AngleEnd
/// \brief 颜色渐变结束
DECLARE_PROPERTY(float, CPostEffectFilter, AngleEnd, GetAngleEnd, SetAngleEnd);
/// property: float AngleExp
/// \brief 颜色衰减系数
DECLARE_PROPERTY(float, CPostEffectFilter, AngleExp, GetAngleExp, SetAngleExp);
/// property: int AngleColor
/// \brief 渐变的颜色
DECLARE_PROPERTY(result_string, CPostEffectFilter, AngleColor, GetAngleColor, 
	SetAngleColor);



CPostEffectFilter::CPostEffectFilter()
{
	m_bVisible = false;//默认关闭后处理

	m_bGradualEnable = false;
	m_fGradualStart = -0.5F;
	m_fGradualEnd = 0.8F;
	m_fGradualExp = 3.0F; 
	m_nGradualColor = VisUtil_Vector4ToColor(FmVec4(0.6F, 0.67F, 0.7F, 1.0F));

	m_bAdjustEnable = false;
	m_nAdjustBaseColor = VisUtil_Vector4ToColor(FmVec4(0.9F, 1.0F, 1.0F, 0.89F));
	m_vColorAdjustParam = FmVec3(50.0F, 10.0F, 49.0F);

	m_bAngleEnable = false;
	m_fAngleStart = 0.5F;
	m_fAngleEnd = 0.7F;
	m_fAngleExp = 0.7F; 
	m_nAngleColor = VisUtil_Vector4ToColor(FmVec4(0.47F, 0.52F, 0.7F, 1.0F));
}

CPostEffectFilter::~CPostEffectFilter()
{
#define shadermap 	TPodHashMap<unsigned int, CPostEffectFilter::post_effect_filter_shaderhandle_t*, TPodTraits<unsigned int>,TPostEffectFilterShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(CPostEffectFilter::post_effect_filter_shaderhandle_t));
	}   
#undef shadermap
	SAFE_RELEASE(m_pTexBackColor);
}

bool CPostEffectFilter::Init(const IVarList& args)
{
	if(!CPostEffect::Init(args))
	{
		return false;
	}
 

	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("filter.vsh", "vs_main_post_filter");
	m_VSList.SetDefine(FLAG_GRADUAL, "#define GRADUAL 1\n");
	m_VSList.SetDefine(FLAG_ADJUST, "#define ADJUST 1\n");
	m_VSList.SetDefine(FLAG_ANGLE, "#define ANGLE 1\n");


	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("filter.fsh", "ps_main_post_filter");
	m_PSList.SetDefine(FLAG_GRADUAL, "#define GRADUAL 1\n");
	m_PSList.SetDefine(FLAG_ADJUST, "#define ADJUST 1\n");
	m_PSList.SetDefine(FLAG_ANGLE, "#define ANGLE 1\n");

	return true;
}

bool CPostEffectFilter::Shut()
{
	return CPostEffect::Shut();
}

void CPostEffectFilter::SetGradualEnable(bool value)
{
	m_bGradualEnable = value;
}

bool CPostEffectFilter::GetGradualEnable() const
{
	return m_bGradualEnable;
}


void CPostEffectFilter::SetGradualStart(float value)
{
	m_fGradualStart = value;
}

float CPostEffectFilter::GetGradualStart() const
{
	return m_fGradualStart;
}
 

void CPostEffectFilter::SetGradualEnd(float value)
{
	m_fGradualEnd = value;
}

float CPostEffectFilter::GetGradualEnd() const
{
	return m_fGradualEnd;
}

void CPostEffectFilter::SetGradualExp(float value)
{
	m_fGradualExp = value;
}

float CPostEffectFilter::GetGradualExp() const
{
	return m_fGradualExp;
}

void CPostEffectFilter::SetGradualColor(const char* value)
{
	m_nGradualColor = VisUtil_ColorToInt(value);
}


result_string CPostEffectFilter::GetGradualColor() const
{
	return VisUtil_IntToColor(m_nGradualColor);
}


void CPostEffectFilter::SetAdjustEnable(bool value)
{
	m_bAdjustEnable = value;
}

bool CPostEffectFilter::GetAdjustEnable() const
{
	return m_bAdjustEnable;
}

void CPostEffectFilter::SetAdjustBrightness(float value)
{
	m_vColorAdjustParam.x = value;
}

float CPostEffectFilter::GetAdjustBrightness() const
{
	return m_vColorAdjustParam.x;
}

void CPostEffectFilter::SetAdjustContrast(float value)
{
	m_vColorAdjustParam.y = value;
}

float CPostEffectFilter::GetAdjustContrast() const
{
	return m_vColorAdjustParam.y;
}

void CPostEffectFilter::SetAdjustSaturation(float value)
{
	m_vColorAdjustParam.z = value;
}

float CPostEffectFilter::GetAdjustSaturation() const
{
	return m_vColorAdjustParam.z;
}

void CPostEffectFilter::SetAdjustBaseColor(const char* value)
{
	m_nAdjustBaseColor = VisUtil_ColorToInt(value);
}

result_string CPostEffectFilter::GetAdjustBaseColor() const
{
	return VisUtil_IntToColor(m_nAdjustBaseColor);
}

void CPostEffectFilter::SetAngleEnable(bool value)
{
	m_bAngleEnable = value;
}

bool CPostEffectFilter::GetAngleEnable() const
{
	return m_bAngleEnable;
}

void CPostEffectFilter::SetAngleStart(float fAngleStart)
{
	m_fAngleStart = fAngleStart;
}

float CPostEffectFilter::GetAngleStart() const
{
	return m_fAngleStart;
}

void CPostEffectFilter::SetAngleEnd(float fAngleEnd)
{
	m_fAngleEnd = fAngleEnd;
}

float CPostEffectFilter::GetAngleEnd() const
{
	return m_fAngleEnd;
}

void CPostEffectFilter::SetAngleExp(float fAngleExp)
{
	m_fAngleExp = fAngleExp;
}

float CPostEffectFilter::GetAngleExp() const
{
	return m_fAngleExp;
}

void CPostEffectFilter::SetAngleColor(const char* value)
{
	m_nAngleColor = VisUtil_ColorToInt(value);
}

result_string CPostEffectFilter::GetAngleColor() const
{
	return VisUtil_IntToColor(m_nAngleColor);
}

void CPostEffectFilter::Realize()
{
	if (!m_bAdjustEnable && !m_bAngleEnable && !m_bGradualEnable)
	{
		// 不需要做任何操作
		return;
	}

	if(m_pTexBackColor == NULL)
	{
		return;
	}
	IRenderContext* pContext = m_pRender->GetContext();

	if(!pContext->GetEnableRealizeTempColorRT())
	{
		return ;
	}

	m_pRender->GetSceneView()->AddPostBatch(RenderPostFilterEffect, this,"PostEffectFilter");

	//m_pRender->GetSceneView()->GetContext()->SetDebugColorRT(m_pTexBackColor);



	m_filter_cb.bAdjustEnable = m_bAdjustEnable;
	if(m_bAdjustEnable)
	{
		m_filter_cb.ColorAdjustParam = FmVec3( m_vColorAdjustParam ) * 0.02f;
		m_filter_cb.GradualBaseColor = VisUtil_ColorToVector4( m_nAdjustBaseColor );

	}


    m_filter_cb.bGradualEnable = m_bGradualEnable;

    if(m_bGradualEnable)
	{
		FmMat4 mtxView = pContext->GetCamera().mtxView;
        FmMat4 mtxViewProjI = pContext->GetCamera().mtxViewProj;
        FmMat4Inverse(&mtxViewProjI,NULL,&mtxViewProjI);
        FmMat4Transpose(&mtxViewProjI,&mtxViewProjI);
		m_filter_cb.matViewProjI = mtxViewProjI;
        m_filter_cb.GradualColor = VisUtil_ColorToVector4( m_nGradualColor );
		m_filter_cb.GradualParam = FmVec3( m_fGradualStart , m_fGradualEnd , m_fGradualExp );

		FmVec3 vecLookDir = FmVec3(0.0F, 0.0F, 1.0F);
		FmVec3 vecLookDirWorld;

		FmMat4 mtxViewInverse;

		FmMat4Inverse(&mtxViewInverse, NULL, &mtxView);

		FmVec3TransformCoord(&vecLookDirWorld, &vecLookDir, 
			&mtxViewInverse);

		vecLookDirWorld.x -= mtxViewInverse._41;
		vecLookDirWorld.y -= mtxViewInverse._42;
		vecLookDirWorld.z -= mtxViewInverse._43;

		FmVec3Normalize(&vecLookDirWorld, &vecLookDirWorld);

        m_filter_cb.vecViewDirection = FmVec4(vecLookDirWorld.x, vecLookDirWorld.y, vecLookDirWorld.z, 0.0F);
	}

    m_filter_cb.bAngleEnable = m_bAngleEnable;
	if(m_bAngleEnable)
	{
       m_filter_cb.AngleParam = FmVec3( m_fAngleStart, m_fAngleEnd , m_fAngleExp);
	   m_filter_cb.AngleColor = VisUtil_ColorToVector4( m_nAngleColor );
	}

	m_filter_cb.TexScaleUV = FmVec2( (float)m_pRender->GetDeviceWidth()/m_pTexBackColor->GetWidth(),(float)m_pRender->GetDeviceHeight()/m_pTexBackColor->GetHeight());

}

CPostEffectFilter::post_effect_filter_shaderhandle_t* CPostEffectFilter::GetShaderHandle()
{
	size_t shader_flag = size_t(m_bGradualEnable) 
		+ (size_t(m_bAdjustEnable) << 1) + (size_t(m_bAngleEnable) << 2);
	if(!m_ShaderMapHandle.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IShaderProgram* pShader = CreateShaderProgram(pVS,pPS);
		IShaderParamOp* pShaderOp = pShader->GetParamOp();
        CPostEffectFilter::post_effect_filter_shaderhandle_t* pShaderHandle = (CPostEffectFilter::post_effect_filter_shaderhandle_t*) CORE_ALLOC(sizeof(CPostEffectFilter::post_effect_filter_shaderhandle_t));
		pShaderHandle->pShader = pShader;
		pShaderHandle->mat4ViewProjIHandle = pShaderOp->FindParamIdByName("c_matViewProjI");
		pShaderHandle->vec4GradualColorHandle = pShaderOp->FindParamIdByName("c_GradualColor");
		pShaderHandle->vec3GradualParamHandle = pShaderOp->FindParamIdByName("c_GradualParam");
		pShaderHandle->vec4ViewDirectionHandle = pShaderOp->FindParamIdByName("c_vecViewDirection");
		pShaderHandle->vec3ColorAdjustParamHandle = pShaderOp->FindParamIdByName("c_ColorAdjustParam");
		pShaderHandle->vec4GradualBaseColorHandle = pShaderOp->FindParamIdByName("c_GradualBaseColor");
		pShaderHandle->vec3AngleParamHandle =  pShaderOp->FindParamIdByName("c_AngleParam");
		pShaderHandle->vec4AngleColorHandle = pShaderOp->FindParamIdByName("c_AngleColor");
		pShaderHandle->vec2TexScaleUVHandle = pShaderOp->FindParamIdByName("c_TexScaleUV");
		pShaderHandle->tex_colorHandle =  pShaderOp->FindParamIdByName("tex_color");
		m_ShaderMapHandle.Add(shader_flag,pShaderHandle);
	} 

	return m_ShaderMapHandle.Find(shader_flag).GetData();
}




void CPostEffectFilter::SetShaderConstValue(CPostEffectFilter::post_effect_filter_shaderhandle_t* pShaderHandle)
{

	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();
    
	if(m_filter_cb.bGradualEnable)
	{
		pShaderOp->SetParamValue(pShaderHandle->mat4ViewProjIHandle,m_filter_cb.matViewProjI);
		pShaderOp->SetParamValue(pShaderHandle->vec4GradualColorHandle,m_filter_cb.GradualColor);
        pShaderOp->SetParamValue(pShaderHandle->vec3GradualParamHandle,m_filter_cb.GradualParam);
        pShaderOp->SetParamValue(pShaderHandle->vec4ViewDirectionHandle,m_filter_cb.vecViewDirection);
	}

	if(m_filter_cb.bAdjustEnable)
	{
		pShaderOp->SetParamValue(pShaderHandle->vec3ColorAdjustParamHandle,m_filter_cb.ColorAdjustParam);
		pShaderOp->SetParamValue(pShaderHandle->vec4GradualBaseColorHandle,m_filter_cb.GradualBaseColor);	
	}

	if(m_filter_cb.bAngleEnable)
	{
		pShaderOp->SetParamValue(pShaderHandle->vec3AngleParamHandle,m_filter_cb.AngleParam);	
		pShaderOp->SetParamValue(pShaderHandle->vec4AngleColorHandle,m_filter_cb.AngleColor);	
	}
    
	pShaderOp->SetParamValue(pShaderHandle->vec2TexScaleUVHandle,m_filter_cb.TexScaleUV);
}

void CPostEffectFilter::RenderPostFilterEffect(void* pdata)
{
	CPostEffectFilter* pthis = (CPostEffectFilter*)pdata;
    //先拷贝backcolor
	pthis->CopyBackColor("PostEffectFilter Err!");


	CPostEffectFilter::post_effect_filter_shaderhandle_t* pShaderHandle = pthis->GetShaderHandle();

	IShaderParamOp* pShaderOp = pShaderHandle->pShader->GetParamOp();

#define ShaderID pShaderHandle->pShader->GetProgram()


	//glUseProgram(ShaderID);
	pShaderHandle->pShader->UsedShader();
	pShaderOp->SetTexture2D(pShaderHandle->tex_colorHandle,pthis->m_pTexBackColor->GetTexture());

	pthis->SetShaderConstValue(pShaderHandle);
 
	pthis->DrawAxisAlignedQuad(pShaderHandle->pShader);
 
	//glUseProgram(0);

#undef ShaderID

}