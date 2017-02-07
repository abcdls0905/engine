//--------------------------------------------------------------------
// 文件名:		dynamic_weather.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月25日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_weather.h"
#include "perlin.h"
#include "../public/auto_mem.h"

/// \file DynamicWeatherEffect.cpp
/// \brief 动态天气系统

/// entity: DynamicWeatherEffect
/// \brief 动态天气系统
/// \see 继承自IVisBase
DECLARE_ENTITY(DynamicWeather,0, IVisBase);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, DynamicWeather, Visible, GetVisible, SetVisible);

/// property: bool UseTexture
/// \brief 使否使用纹理
DECLARE_PROPERTY(bool, DynamicWeather, UseTexture, GetUseTexture, SetUseTexture);

/// property: bool AsyncLoad
/// \brief 是否异步加载贴图
DECLARE_PROPERTY(bool, DynamicWeather, AsyncLoad, GetAsyncLoad, SetAsyncLoad);


/// property: float Density
/// \brief 粒子密度
DECLARE_PROPERTY(float, DynamicWeather, Density, GetDensity, SetDensity);

/// property: float Speed
/// \brief 粒子速度
DECLARE_PROPERTY(float, DynamicWeather, Speed, GetSpeed, SetSpeed);

/// property: float Length
/// \brief 粒子长度
DECLARE_PROPERTY(float, DynamicWeather, Length, GetLength, SetLength);

/// property: float Width
/// \brief 粒子宽度
DECLARE_PROPERTY(float, DynamicWeather, Width, GetWidth, SetWidth);

/// property: float Alpha
/// \brief 粒子Alpha
DECLARE_PROPERTY(float, DynamicWeather, Alpha, GetAlpha, SetAlpha);

/// property: float Wind
/// \brief 粒子风力
DECLARE_PROPERTY(float, DynamicWeather, Wind, GetWind, SetWind);

/// property: float AlphaFadeSpeedBegin
/// \brief 粒子alpha淡入速度
DECLARE_PROPERTY(float, DynamicWeather, AlphaFadeSpeedBegin, GetAlphaFadeSpeedBegin, SetAlphaFadeSpeedBegin);

/// property: float fAlphaFadeSpeedEnd
/// \brief 粒子alpha淡出速度
DECLARE_PROPERTY(float, DynamicWeather, AlphaFadeSpeedEnd, GetAlphaFadeSpeedEnd, SetAlphaFadeSpeedEnd);

/// property: float fAlphaFadeSpeedEnd
/// \brief 粒子alpha淡出速度
DECLARE_PROPERTY(bool, DynamicWeather, Occlusion, GetOcclusion, SetOcclusion);

/// \brief 设置粒子的名字
DECLARE_PROPERTY(const char*, DynamicWeather, ParticleNodeTex, GetParticleNodeTex, SetParticleNodeTex); 


inline static float Random(float amount)
{
	int random = rand();
	return amount * (float)random / (float)RAND_MAX;
}


DynamicWeather::DynamicWeather() : m_pVBuffer(NULL), m_pIBuffer(NULL),m_pTexture(NULL),m_fParticleBox(30.0f)
{
	m_nPasses = 0;
	m_bVisible = true;
	m_bAsyncLoad = false;

	m_bUseTexture = true;

	m_bUseOcclusion = false;

	m_vLighting = FmVec4(0.5f, 1.0f, 0.5f, 1.0f);

	m_iNoiseX = 0;
	m_iNoiseY = 10000;
	m_iNoiseZ = 20000;

	for(int i = 0; i < MAX_RANDER_PASS_NUM; i++)
	{
		m_fSpeedScale[i] = 0.75f + Random(0.5f);
		m_vOffset[i] = FmVec3(0.f, 0.f, 0.f);
		m_vWind[i] = FmVec3(0.f, 0.f, 0.f);
		// value ~ (0,30)
		m_vRandom[i] = FmVec3(Random(m_fParticleBox), Random(m_fParticleBox), Random(m_fParticleBox));
	}

	m_strTexFilePath = "tex\\blob.dds";
	FmMat4 mIdentity;
	FmMat4Identity(&mIdentity);

	// 
	SetOcclusionBox(FmVec3(-5.0f, 0.0f, -5.0f), FmVec3(1.0f, 1.0f, 1.0f), mIdentity);
}

DynamicWeather::~DynamicWeather()
{
#define shadermap 	TPodHashMap<unsigned int, DynamicWeather::dynamic_weather_shaderhandle_t*, TPodTraits<unsigned int>,TDynamicWeatherShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(DynamicWeather::dynamic_weather_shaderhandle_t));
	}   
#undef shadermap
 	SAFE_RELEASE(m_pWhiteTexture);
}

bool DynamicWeather::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	return true;
}

bool DynamicWeather::Shut()
{

	return true;
}


void DynamicWeather::Update(float seconds)
{
	m_fTime = seconds;
	// 粒子的密度
	m_nPasses = (int)(m_Parameters.fDensity + 0.999f);

	// make each pass use the wind a varying amount
	// 变化层次
	int nWrap = MAX_RANDER_PASS_NUM / 5;
	//
	float fWind = m_Parameters.fWind / (float)nWrap;

	PerlinNoiseDefault noise;

	// increment the noise lookup values for each axis
	// 一个周期性递增的
	m_iNoiseX += (int)(m_fTime * 5000.0f);/*每5000秒周期一次*/
	m_iNoiseY += (int)(m_fTime * 5000.0f);
	m_iNoiseZ += (int)(m_fTime * 5000.0f);

	// get a noise vector
	// 获取一个随机向量
	FmVec3 vNoise(noise.SoftNoise1D(m_iNoiseX), noise.SoftNoise1D(m_iNoiseY) * 0.5f, noise.SoftNoise1D(m_iNoiseZ));

	// 下面是每层的影响音素
	for(int i = 0; i < m_nPasses; i++)
	{
		// calculate and add the per-pass gravity and wind
		// 重力因素
		float gravity = m_Parameters.fSpeed * m_fTime * m_fSpeedScale[i];

		// 风向
		m_vWind[i] = vNoise * fWind * (float)((i+1)%(int)nWrap);

		// add the movement from wind
		// 偏移值
		m_vOffset[i] += m_vWind[i] * m_fTime;

		// add the movement due to gravity
		m_vOffset[i].y = m_vOffset[i].y + gravity;

		// wrap the offset to be within 0.0 - m_fParticleBox in each dimension
		// 粒子包围盒
		if(m_vOffset[i].x > m_fParticleBox)
			m_vOffset[i].x = 0.0f;
		else if(m_vOffset[i].x < 0.0f)
			m_vOffset[i].x = m_fParticleBox;

		if(m_vOffset[i].y > m_fParticleBox)
			m_vOffset[i].y = 0.0f;
		else if(m_vOffset[i].y < 0.0f)
			m_vOffset[i].y = m_fParticleBox;

		if(m_vOffset[i].z > m_fParticleBox)
			m_vOffset[i].z = 0.0f;
		else if(m_vOffset[i].z < 0.0f)
			m_vOffset[i].z = m_fParticleBox;
	}

}

void DynamicWeather::Realize()
{
	if(!m_bVisible)
		return;

	if(!GetLoadFinish())
		return;

	Render();
}

// 是否加载完成
bool DynamicWeather::GetLoadFinish()
{
	if(m_pTexture == NULL)
		return false;
	//这里处理高度图问题和部分需要的纹理使否加载完毕
	if(m_pTexture->IsLoadComplete())
		return true;
	return false;
}

void DynamicWeather::SetLightColor(FmVec4 vLighting)
{
	m_vLighting = vLighting;
}

FmVec4 DynamicWeather::GetLightColor() const
{
	return m_vLighting;
}
void DynamicWeather::SetUseTexture(bool bUseTexture)
{
	m_bUseTexture = bUseTexture;

}

bool DynamicWeather::GetUseTexture() const
{
	return m_bUseTexture;
}

// 是否可见
void DynamicWeather::SetVisible(bool value)
{
	m_bVisible = value;
}

bool DynamicWeather::GetVisible() const
{
	return m_bVisible;
}

// 是否异步加载贴图
void DynamicWeather::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool DynamicWeather::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

// 更新贴图
bool DynamicWeather::UpdateTexture()
{
	UnloadTexture();

	return LoadTexture();
}

// 设置碰撞包围盒大小
void DynamicWeather::SetOcclusionBox(const FmVec3& vMin, const FmVec3& vMax, const FmMat4& mTransform)
{
	m_OcclusionBox.mTransform = mTransform;
	m_OcclusionBox.vMin = vMin;
	m_OcclusionBox.vMax = vMax;

	// get bounding sphere taking into account transformation

	CalculateOcclusionMatrix();
}

void DynamicWeather::SetDensity(float value)
{
	m_Parameters.fDensity = value;
}

float DynamicWeather::GetDensity() const
{
	return m_Parameters.fDensity;
}

void DynamicWeather::SetSpeed(float value)
{
	m_Parameters.fSpeed = value;
}

float DynamicWeather::GetSpeed() const
{
	return m_Parameters.fSpeed;
}

void DynamicWeather::SetLength(float value)
{
	m_Parameters.fLength = value;
}

float DynamicWeather::GetLength() const
{
	return m_Parameters.fLength;
}

void DynamicWeather::SetWidth(float value)
{
	m_Parameters.fWidth = value;
}

float DynamicWeather::GetWidth() const
{
	return m_Parameters.fWidth;
}

void DynamicWeather::SetAlpha(float value)
{
	m_Parameters.fAlpha = value;
}

float DynamicWeather::GetAlpha() const
{
	return m_Parameters.fAlpha;
}

void DynamicWeather::SetWind(float value)
{
	m_Parameters.fWind = value;
}

float DynamicWeather::GetWind() const
{
	return m_Parameters.fWind;
}

void DynamicWeather::SetAlphaFadeSpeedBegin(float value)
{
	m_Parameters.fAlphaFadeSpeedBegin = value;
}

float DynamicWeather::GetAlphaFadeSpeedBegin() const
{
	return m_Parameters.fAlphaFadeSpeedBegin;
}

void DynamicWeather::SetAlphaFadeSpeedEnd(float value)
{
	m_Parameters.fAlphaFadeSpeedEnd = value;
}

float DynamicWeather::GetAlphaFadeSpeedEnd() const
{
	return m_Parameters.fAlphaFadeSpeedEnd;
}

void DynamicWeather::SetOcclusion(bool value)
{
	m_bUseOcclusion = value;
}

bool DynamicWeather::GetOcclusion() const
{
	return m_bUseOcclusion;
}


void DynamicWeather::InitVertex()
{ 
    unsigned int nColorTex = 0xFFFFFFFF;
	m_pWhiteTexture = m_pRender->CreateStaticTex(1,1,0,TEX_FORMAT_RGBA8888_UNORM,&nColorTex,sizeof(nColorTex));

	//建立粒子顶点数据
	m_nParticles = 5000;//一万个粒子上限
	m_nVertices = m_nParticles * 4;
	m_nIndices = m_nParticles * 6;
	FmVec3 vDefaultOffset(m_fParticleBox, m_fParticleBox, m_fParticleBox);

	size_t data_vb_size = m_nVertices *sizeof(weather_vertex_t);
	TAutoMem<unsigned char, 5000*sizeof(weather_vertex_t), TCoreAlloc> auto_buf_vb(data_vb_size);

	size_t data_ib_size = m_nParticles*6*sizeof(unsigned short);
	TAutoMem<unsigned short, 5000*sizeof(unsigned short), TCoreAlloc> auto_buf_ib(data_ib_size);
	

	
	// lock index buffer
	unsigned short* pIndices = auto_buf_ib.GetBuffer();
	// lock vertex buffer
	weather_vertex_t* pVertices = (weather_vertex_t*)auto_buf_vb.GetBuffer();

	for(int particle = 0; particle < m_nParticles; ++particle)
	{
		FmVec3 vPosition = FmVec3(Random(m_fParticleBox), Random(m_fParticleBox), Random(m_fParticleBox));
		vPosition += vDefaultOffset;

		// bottom left 通一点上面的矩形
		pVertices->position = vPosition;
		pVertices->uv = FmVec2(0.0,1.0);
		++pVertices;

		//top left
		pVertices->position = vPosition;
		pVertices->uv = FmVec2(0.0,0.0);
		++pVertices;

		//top right
		pVertices->position = vPosition;
		pVertices->uv = FmVec2(1.0,0.0);
		++pVertices;

		// bottom right
		pVertices->position = vPosition;
		pVertices->uv = FmVec2(1.0,1.0);
		++pVertices;

		// indices
		// 建立索引关系
		*pIndices = particle * 4; pIndices++;
		*pIndices = particle * 4 + 1; pIndices++;
		*pIndices = particle * 4 + 2; pIndices++;
		*pIndices = particle * 4 + 2; pIndices++;
		*pIndices = particle * 4 + 3; pIndices++;
		*pIndices = particle * 4; pIndices++;
	}
 
 	m_pVBuffer = m_pRender->CreateStaticVB( auto_buf_vb.GetBuffer(),m_nVertices *sizeof(weather_vertex_t));
	m_pIBuffer = m_pRender->CreateStaticIB( auto_buf_ib.GetBuffer(),m_nIndices * sizeof(unsigned short));
}

bool DynamicWeather::LoadTexture()
{ 
	if(!m_strTexFilePath.empty())
		m_pTexture = m_pRender->CreateTexture(m_strTexFilePath.c_str(),m_bAsyncLoad);
	if (NULL == m_pTexture)
	{
		CORE_TRACE("(DynamicWeather::LoadTexture) load texture failed");
		CORE_TRACE(m_strTexFilePath.c_str());
	}

	return true;
}


// 设置粒子的贴图
void DynamicWeather::SetParticleNodeTex(const char* value)
{
	m_strTexFilePath = value;
}

 
const char* DynamicWeather::GetParticleNodeTex() const
{
	return m_strTexFilePath.c_str();
}

bool DynamicWeather::UnloadTexture()
{
	SAFE_RELEASE(m_pTexture);
	return true;
}

void DynamicWeather::CalculateOcclusionMatrix()
{
	// the purpose of this matrix is to go from worldspace to box space

	// get vector for size of box
	// x 6 y 1 z6
	FmVec3 vScale = m_OcclusionBox.vMax - m_OcclusionBox.vMin;
	// get vector to scale box to unit cube 
	FmVec3 vScaleRcp = FmVec3(1.0f / vScale.x, 1.0f / vScale.y, 1.0f / vScale.z);

	// get vector to translate from m_OcclusionBox.vMin to (0, 0, 0)
	// from world space to cuble space
	FmVec3 vTranslate(m_OcclusionBox.vMin.x * vScaleRcp.x,
		m_OcclusionBox.vMin.y * vScaleRcp.y,
		m_OcclusionBox.vMin.z * vScaleRcp.z);
	vTranslate = -vTranslate;

	// flip the Z to take into account sampling the V coordinate
	 
	if(true)
	{
		vScaleRcp.z = -vScaleRcp.z;
		vTranslate.z = 1.0f - vTranslate.z;
	}
	 

	// construct matrix to go from world space to box space
	// 建立一个又BOX空间转换到world空间的矩阵
	FmMat4 mScale, mTranslate;
	FmMat4Scaling(&mScale, vScaleRcp.x, vScaleRcp.y, vScaleRcp.z);

	FmMat4Translation(&mTranslate, vTranslate.x, vTranslate.y, vTranslate.z);

	FmMat4 mScaleTranslate = mScale * mTranslate;

	FmMat4 mScaleTranslateInverse;

	FmMat4Inverse(&mScaleTranslateInverse, NULL, &mScaleTranslate);

	// multiply by the inverse transform then apply bounds

	m_OcclusionBox.mUnitCubeToWorld = mScaleTranslateInverse * m_OcclusionBox.mTransform;

	FmMat4Inverse(&m_OcclusionBox.mWorldToUnitCube, NULL, &m_OcclusionBox.mUnitCubeToWorld);
}

bool DynamicWeather::Render()
{	
	
	IRenderContext* pContext = m_pRender->GetContext();

	FmMat4 viewMatrix  = pContext->GetCamera().mtxView;
	FmVec3 vCameraPosition = pContext->GetCamera().vPosition;
	FmVec3 vCameraMovement = pContext->GetPrevCamera().vPosition - vCameraPosition;

	float fCameraSpeed = FmVec3Length(&vCameraMovement) * (1.0f / m_fTime);// 1/m_fTime FPS
	fCameraSpeed = (fCameraSpeed - m_Parameters.fAlphaFadeSpeedBegin) / m_Parameters.fAlphaFadeSpeedEnd;

	if(fCameraSpeed < 0.0f)
		fCameraSpeed = 0.0f;
	else if(fCameraSpeed > 1.0f)
		fCameraSpeed = 1.0f;

	m_fAlphaFade = 1.0f - fCameraSpeed; 

    ISceneView* pSceneView = m_pRender->GetSceneView();

	FmMat4  mtxViewProj;
	FmMat4Transpose(&mtxViewProj,&pContext->GetCamera().mtxViewProj);
	FmMat4  mtxViewProjPrev;
	FmMat4Transpose(&mtxViewProjPrev,&pContext->GetPrevCamera().mtxViewProj);

	float fBoxOffset = m_fParticleBox * 0.5f;
	FmVec4 vForward(	viewMatrix._13 * fBoxOffset,
		viewMatrix._23 * fBoxOffset,
		viewMatrix._33 * fBoxOffset,
		viewMatrix._43);
	if(m_fAlphaFade > 0.0f)
	{
		float fAlpha = m_Parameters.fDensity;
		for(int i = 0; i < m_nPasses; ++i )
		{
			pSceneView->AddBlendBatch(RenderWeatherCallBack,&m_dynamic_weather_cb[i],0.0,"DynamicWeather");
 
			m_dynamic_weather_cb[i].pShaderCB = GetShaderHandle();
			// set current and previous world view proj matrices
			{
				float fLengthScale = (m_Parameters.fLength / m_Parameters.fSpeed) / m_fTime;
				float fVerticalZoom = pContext->GetCamera().mtxProj._22 / 1.778341f;  // account for fov changes
				float fWidth = m_Parameters.fWidth * fVerticalZoom;

				m_dynamic_weather_cb[i].vec4SizeScale = FmVec4(fWidth, fLengthScale, 0.0f, 0.0f);
			}
		 
			m_dynamic_weather_cb[i].mat4ViewProj = mtxViewProj;
			m_dynamic_weather_cb[i].mat4ViewProjPrev = mtxViewProjPrev;
			// set view position vector
			m_dynamic_weather_cb[i].vec4ViewPosition = FmVec4(vCameraPosition,1.0);
			// set lighting
			m_dynamic_weather_cb[i].vec4Lighting = m_vLighting;
			// set a forward shift vector 
			m_dynamic_weather_cb[i].vec4Forward = vForward;
            // set occlusion matrix to the shader
			m_dynamic_weather_cb[i].mat4OcclusionMatrix = m_OcclusionBox.mWorldToUnitCube;
           
            // set texture
            m_dynamic_weather_cb[i].bUseTex = m_bUseTexture;
			m_dynamic_weather_cb[i].pTexColor = m_pTexture;
			m_dynamic_weather_cb[i].pTexWhite = m_pWhiteTexture;

			// 这里获取实时渲染的碰撞图
			m_dynamic_weather_cb[i].bOcclusion = false;
			m_dynamic_weather_cb[i].pTexOcclusion = NULL;//暂时关闭这个功能
            FmVec3 vForward = FmVec3(viewMatrix._13 * fBoxOffset,viewMatrix._23 * fBoxOffset,viewMatrix._33 * fBoxOffset);
			SetPassConstants(i,fAlpha,vForward,vCameraPosition);
			fAlpha -= 1.0f;

			m_dynamic_weather_cb[i].pIBuffer = m_pIBuffer;
			m_dynamic_weather_cb[i].pVBuffer = m_pVBuffer;
		}

		return true;
	}
	else
	{
		return  false;
	}

	return true;
}

// set pass param
void DynamicWeather::SetPassConstants(int iPass, float fAlpha,FmVec3 vForward,FmVec3 vCameraPosition)
{  
	FmVec3 vOffset = vCameraPosition + m_vRandom[iPass] + m_vOffset[iPass] + vForward;
	// 粒子位置
	FmVec4 vPositionOffset(	fmodf(vOffset.x, m_fParticleBox),
		fmodf(vOffset.y, m_fParticleBox),
		fmodf(vOffset.z, m_fParticleBox),
		0.0f);

	m_dynamic_weather_cb[iPass].vec4PositionOffset = vPositionOffset;

	// 设置风力
	FmVec4 vVelocity(	m_vWind[iPass].x * m_fTime,
							m_vWind[iPass].y * m_fTime + m_fSpeedScale[iPass] * m_Parameters.fSpeed * m_fTime,
							m_vWind[iPass].z * m_fTime,
							0.0f);

	m_dynamic_weather_cb[iPass].vec4Velocity = vVelocity;

	// Set colour for this pass
	if(fAlpha > 1.0f)
		fAlpha = 1.0f;

	fAlpha = fAlpha * m_Parameters.fAlpha * m_fAlphaFade;
	FmVec4 vAlpha(fAlpha, fAlpha, fAlpha, fAlpha);

	m_dynamic_weather_cb[iPass].vec4Alpha = vAlpha;
}

bool DynamicWeather::Load()
{
	Unload();

	InitVertex();
	LoadTexture();
	LoadShader();


	return true;
}

const char* dynamic_weather_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};

DynamicWeather::dynamic_weather_shaderhandle_t* DynamicWeather::GetShaderHandle()
{
	
	size_t shader_flag = size_t(FLAG_NONE) 
		+ (size_t(m_bUseOcclusion) << 1);
    
	if(!m_ShaderMapHandle.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,dynamic_weather_vertex_t_name,2);
		IShaderParamOp* pShaderOp = pShader->GetParamOp();
        DynamicWeather::dynamic_weather_shaderhandle_t* pShaderHandle = (DynamicWeather::dynamic_weather_shaderhandle_t*) CORE_ALLOC(sizeof(DynamicWeather::dynamic_weather_shaderhandle_t));
		pShaderHandle->pShader = pShader;

		pShaderHandle->mat4ViewProjHandle = pShaderOp->FindParamIdByName("c_mtxViewProj");
		pShaderHandle->mat4ViewProjPrevHandle = pShaderOp->FindParamIdByName("c_mtxViewProjPrev");
		pShaderHandle->vec4PositionOffsetHandle = pShaderOp->FindParamIdByName("c_vPositionOffset");
		pShaderHandle->vec4VelocityHandle = pShaderOp->FindParamIdByName("c_vVelocity");
        pShaderHandle->vec4Alpha = pShaderOp->FindParamIdByName("c_vAlpha");
		pShaderHandle->vec4ViewPositionHandle = pShaderOp->FindParamIdByName("c_vViewPosition");
		pShaderHandle->vec4SizeScaleHandle = pShaderOp->FindParamIdByName("c_vSizeScale");
		pShaderHandle->vec4LightingHandle =  pShaderOp->FindParamIdByName("c_vLighting");
		pShaderHandle->vec4ForwardHandle = pShaderOp->FindParamIdByName("c_vForward");
		pShaderHandle->mat4OcclusionMatrixHandle = pShaderOp->FindParamIdByName("c_mOcclusionMatrix");
		pShaderHandle->tex_ColorHandle =  pShaderOp->FindParamIdByName("tex_color");
		pShaderHandle->tex_OcclusionHeightMapHandle =  pShaderOp->FindParamIdByName("tex_color");
		m_ShaderMapHandle.Add(shader_flag,pShaderHandle);
	} 
    
	return m_ShaderMapHandle.Find(shader_flag).GetData();
}

void DynamicWeather::RenderWeatherCallBack(void* pdata)
{
	dynamic_weather_cb_t* pDCB = (dynamic_weather_cb_t*)pdata;
	DynamicWeather::dynamic_weather_shaderhandle_t* pShaderCB = pDCB->pShaderCB;
	pShaderCB->pShader->UsedShader();


	IShaderParamOp* pShaderParamOp = pShaderCB->pShader->GetParamOp();
	IRenderStateOp* pShaderStateOp = pShaderCB->pShader->GetRenderStateOp();
	IRenderDrawOp* pRenderDrawOp = pShaderCB->pShader->GetRenderDrawOp();

	//修改混合方式
	//glBlendFunc(GL_ONE, GL_ONE);
	pShaderStateOp->SetBlendFunc( IRenderStateOp::BLEND_COLOR_ONE ,IRenderStateOp::BLEND_COLOR_ONE );


	//SetConst
	pShaderParamOp->SetParamValue(pShaderCB->mat4ViewProjHandle,pDCB->mat4ViewProj);
	pShaderParamOp->SetParamValue(pShaderCB->mat4ViewProjPrevHandle,pDCB->mat4ViewProjPrev);
	pShaderParamOp->SetParamValue(pShaderCB->vec4PositionOffsetHandle,pDCB->vec4PositionOffset);
	pShaderParamOp->SetParamValue(pShaderCB->vec4VelocityHandle,pDCB->vec4Velocity);
	pShaderParamOp->SetParamValue(pShaderCB->vec4Alpha,pDCB->vec4Alpha);
	pShaderParamOp->SetParamValue(pShaderCB->vec4ViewPositionHandle,pDCB->vec4ViewPosition);
	pShaderParamOp->SetParamValue(pShaderCB->vec4SizeScaleHandle,pDCB->vec4SizeScale);
	pShaderParamOp->SetParamValue(pShaderCB->vec4LightingHandle,pDCB->vec4Lighting);
	pShaderParamOp->SetParamValue(pShaderCB->vec4ForwardHandle,pDCB->vec4Forward);
	pShaderParamOp->SetParamValue(pShaderCB->mat4OcclusionMatrixHandle,pDCB->mat4OcclusionMatrix);

	//SetTexture
	if(pDCB->bUseTex)
	{
		pShaderParamOp->SetTexture2D(pShaderCB->tex_ColorHandle,pDCB->pTexColor->GetCanUseShaderTex()->GetTexture());
	}
	else
	{
		pShaderParamOp->SetTexture2D(pShaderCB->tex_ColorHandle,pDCB->pTexWhite->GetTexture());
	}

	//SetOcclusion
	if(pDCB->bOcclusion)
	{
		pShaderParamOp->SetTexture2D(pShaderCB->tex_OcclusionHeightMapHandle,pDCB->pTexOcclusion->GetTexture());
	}

	//Set VB and IB
	//glBindBuffer(GL_ARRAY_BUFFER, pDCB->pVBuffer->GetBuffer());
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pDCB->pIBuffer->GetBuffer());
	pRenderDrawOp->SetVB( pDCB->pVBuffer->GetBuffer());
	pRenderDrawOp->SetIB( pDCB->pIBuffer->GetBuffer());
 
#define dynamic_weather_vertex_t_pos 0
#define dynamic_weather_vertex_t_uv  1
	
//	glEnableVertexAttribArray(dynamic_weather_vertex_t_pos);
//	glVertexAttribPointer(dynamic_weather_vertex_t_pos, 3, GL_FLOAT, GL_FALSE, sizeof(weather_vertex_t), 0);
//	glEnableVertexAttribArray(dynamic_weather_vertex_t_uv);
//	glVertexAttribPointer(dynamic_weather_vertex_t_uv, 2, GL_FLOAT, GL_FALSE, sizeof(weather_vertex_t), (void*) (3 * sizeof(float)));

	pRenderDrawOp->EnableVertexAttribArray(dynamic_weather_vertex_t_pos);
	pRenderDrawOp->SetVertexAttribPointer(dynamic_weather_vertex_t_pos,3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(weather_vertex_t), 0);
	pRenderDrawOp->EnableVertexAttribArray(dynamic_weather_vertex_t_uv);
	pRenderDrawOp->SetVertexAttribPointer(dynamic_weather_vertex_t_uv,2,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(weather_vertex_t), (void*) (3 * sizeof(float)));

    fm_sizei count = pDCB->pIBuffer->GetSize()/sizeof(unsigned short);
	//glDrawElements(GL_TRIANGLES, count , GL_UNSIGNED_SHORT, 0);
	pRenderDrawOp->DrawIndex(IRenderDrawOp::DRAW_TRIANGLES, count ,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,0);

	/*
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	*/
#undef post_effect_vertex_t_pos
#undef post_effect_vertex_t_uv
}

bool DynamicWeather::LoadShader()
{
	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("dynamic_weather.vsh", "vs_main_dynamic_weather");
	m_VSList.SetDefine(FLAG_NONE, "#define NONE 1\n");
	m_VSList.SetDefine(FLAG_OCCLUSION, "#define OCCLUSION 1\n");


	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("dynamic_weather.fsh", "ps_main_dynamic_weather");
	m_PSList.SetDefine(FLAG_NONE, "#define NONE 1\n");
	m_PSList.SetDefine(FLAG_OCCLUSION, "#define OCCLUSION 1\n");

	return true;
}
bool DynamicWeather::Unload()
{
	SAFE_RELEASE(m_pVBuffer);
	SAFE_RELEASE(m_pIBuffer);
	UnloadTexture();
	return true;
}
