//--------------------------------------------------------------------
// 文件名:		context.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "context.h"
#include "render.h"
#include "helper.h"
#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/vis_utils.h"
#include "device_caps.h"

CContext* CContext::NewInstance(Render* pRender)
{
	CContext* p = (CContext*)CORE_ALLOC(sizeof(CContext));

	new (p) CContext(pRender);

	return p;
}

CContext::CContext(Render* pRender)
{
	m_pRender = pRender;
	m_nViewWidth = 0;
	m_nViewHeight = 0;
	memset(m_IntValues, 0, sizeof(m_IntValues));
	memset(m_FloatValues, 0, sizeof(m_FloatValues));
	memset(m_ColorValues, 0, sizeof(m_ColorValues));
	memset(m_VectorValues, 0, sizeof(m_VectorValues));
	m_FloatValues[F_ROLE_AMBIENT_FACTOR] = 1.0F;
	m_FloatValues[F_ROLE_DIFFUSE_FACTOR] = 1.0F;
	m_FloatValues[F_REFLECT_FACTOR] = 0.2F;
	memset(&m_Camera, 0, sizeof(m_Camera));
	memset(&m_PrevCamera, 0, sizeof(m_PrevCamera));
	m_Camera.fFov = FLOAT_PI / 4.0F;
	m_Camera.fRatio = 1024 / 768.0F;
	m_Camera.fNearZ = 0.5F;
	m_Camera.fFarZ = 1200;
	m_fFarClipDistance = 500.0F;
	m_vReferPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_nBlendDownLevel = 0;
	m_bEnableRealizeRefraction = false;
	m_bEnableRealizeCopyTempColorRT = false;
	m_bEnableRealizeTempColorRT = false;
	m_bEnableRealizeTempDepthRT = false;
	m_bEnableRealizeTempDephtStencilRT = false;
	m_bEnableRealizeTempStencilTex = false;

	m_bEnableClearColorOptimize = false;

	m_pBlendColorRT = NULL;
	m_pBlendDepthRT = NULL;
	m_pRefractionRT = NULL;
	m_pRealizeTempDepthRT = NULL;
	m_pRealizeCopyColorTempRT = NULL;
	m_pRealizeTempColorRT = NULL;
	m_pRealizeTempDepthTex = NULL;
	m_pGlobalEnvMap = NULL;
	m_pRealizeTempDepthStencilRT = NULL;
	m_pRealizeTempStencilTex = NULL;

	// x*y*z = x*y*w^2;
	// z = w^2
	// w =sqrt(z)
	// z为原像素的缩放系数
	// w 实际需要的缩放系数
	m_fRealizeTempRTScale = 1.0f;//sqrt(0.5);

	m_pRealizeCopyTempDepthRT = NULL;
	m_bEnableRealizeCopyTempDepthRT = false;
	m_bEnableRealizeTempDepthTex = false;
	m_pDebugColorRT = NULL;
	m_pDebugDepthRT = NULL;
	m_pDebugTex = NULL;
	m_pDecodeColorRT = NULL;
	m_pDebugDepthStencilRT = NULL;
	m_bEnableEarlyZ = false;

	m_bEnableFixShadow = true;
	m_bEnableDynamicShadow = false;
	m_bPercentageCloserFilter = false;
	m_bVarianceShadowMap = false;
	m_fLogSplitWeight = 0.7F;
	m_nShadowMapSize = 1024;
	m_nShadowMapCount = 1;
	m_nShadowMapIndex = -1;
	m_fShadowDistance = 10.f;
	m_fMaxLightDistance = 0.0f;
	m_fCameraDistance = 1000.0f;
	m_fShadowExpandPercent = 0.001F;
	m_bShadowMapReusable = false;
	m_pShadowRT = NULL;
	m_vShadowLightDir = FmVec3(0.0F, 0.0F, 0.0F);

	m_vAdjustingRect = FmVec4(-2.0f, 5.0f, 5.0f, 10.0f);

	for (int i = 0; i < MAX_SHADOWMAP_NUM; ++i)
	{
		FmMat4Identity(&m_mtxShadowMapView[i]);
		FmMat4Identity(&m_mtxShadowMapProj[i]);
		m_vShadowOrthoMin[i] = FmVec2(0.0F, 0.0F);
		m_vShadowOrthoMax[i] = FmVec2(0.0F, 0.0F);
		m_vShadowOrthoCenter[i] = FmVec3(0.0F, 0.0F, 0.0F);
		m_nShadowMapNoFlush[i] = 0;
		m_fDepthSlope[i] = 0.0F;
		m_fDepthBias[i] = 0.0F;
		m_pShadowMapRT[i] = NULL;
		m_pShadowMapDS[i] = NULL;
	}
}

CContext::~CContext()
{
	SAFE_RELEASE(m_pGlobalEnvMap);
	this->ReleaseBlendRT();
	this->ReleaseRefractionRT();
	this->ReleaseCopyTempColorRT();
	this->ReleaseTempColorRT();
	this->ReleaseTempDepthRT();
	this->ReleaseTempDepthTex();
	this->ReleaseTempDepthStencilRT();
	this->ReleaseLayer();
}

void CContext::Release()
{
	CORE_DELETE(this);
}

// 是否开启Clear 
void CContext::SetEnableClearColorOptimize(bool value)
{
	m_bEnableClearColorOptimize = value;
}

bool CContext::GetEnableClearColorOptimize()
{
	return m_bEnableClearColorOptimize;
}


// 是否要开启深度预渲染
void CContext::SetEnableEarlyZ(bool value)
{
	m_bEnableEarlyZ = value;
}

bool CContext::GetEnableEarlyZ() const
{
	return m_bEnableEarlyZ;
}


int CContext::GetRenderWidth() const
{
	if (0 == m_nViewWidth)
	{
		return m_pRender->GetDeviceWidth();
	}

	return m_nViewWidth;
}

int CContext::GetRenderHeight() const
{
	if (0 == m_nViewHeight)
	{
		return m_pRender->GetDeviceHeight();
	}

	return m_nViewHeight;
}

// camera

bool CContext::SetCameraView(const FmVec3& pos, 
							 const FmVec3& angle)
{
	m_Camera.vPosition = pos;
	m_Camera.vAngle = angle;

	FmMat4 mtxRotate;

	FmMat4RotationYawPitchRoll(&mtxRotate, angle.y, angle.x, angle.z);
	mtxRotate._41 = pos.x;
	mtxRotate._42 = pos.y;
	mtxRotate._43 = pos.z;
	FmMat4Inverse(&m_Camera.mtxView, NULL, &mtxRotate);
	mtxRotate._41 -= m_vReferPosition.x;
	mtxRotate._42 -= m_vReferPosition.y;
	mtxRotate._43 -= m_vReferPosition.z;
	FmMat4Inverse(&m_Camera.mtxViewRelative, NULL, &mtxRotate);

	return true;
}



bool CContext::SetCameraProj(float fov, float zn, float zf, float ratio)
{
	m_Camera.fFov = fov;
	m_Camera.fRatio = ratio;
	m_Camera.fNearZ = zn;
	m_Camera.fFarZ = zf;

	m_Camera.mtxProj = FmMat4::PerspectiveFovLH(fov, ratio, zn, zf,FmMat4::D3D,false); 
	m_Camera.mtxDxProj = m_Camera.mtxProj;

	FmMat4 matD3DtoGL;
	FmMat4Identity(&matD3DtoGL);
	//matD3DtoGL._11 = -1.0f;
	matD3DtoGL._33 = 2.0f;
	matD3DtoGL._43 = -1.0f;

 
	FmMatrixMultiply(&m_Camera.mtxProj,&m_Camera.mtxProj,&matD3DtoGL);
	return true;
}

bool CContext::SetViewMatrix(const FmMat4& mat)
{
	m_Camera.mtxView = mat;

	FmMat4 mtxInverse;

	FmMat4Inverse(&mtxInverse, NULL, &mat);
	m_Camera.vPosition.x = mtxInverse._41;
	m_Camera.vPosition.y = mtxInverse._42;
	m_Camera.vPosition.z = mtxInverse._43;
	mtxInverse._41 -= m_vReferPosition.x;
	mtxInverse._42 -= m_vReferPosition.y;
	mtxInverse._43 -= m_vReferPosition.z;
	FmMat4Inverse(&m_Camera.mtxViewRelative, NULL, &mtxInverse);

	return true;
}

bool CContext::SetProjMatrix(const FmMat4& mat)
{
	m_Camera.mtxProj = mat;

	return true;
}

void CContext::MakeHelperMatrix()
{
	FmMatrixMultiply(&m_Camera.mtxViewProj, &m_Camera.mtxView,
		&m_Camera.mtxProj);

	FmMatrixMultiply(&m_Camera.mtxDxViewProj,&m_Camera.mtxView,&m_Camera.mtxDxProj);

	FmMatrixMultiply(&m_Camera.mtxViewProjRelative, 
		&m_Camera.mtxViewRelative, &m_Camera.mtxProj);

	FmMat4Inverse(&m_Camera.mtxViewInverse, NULL, &m_Camera.mtxView);
	FmMat4Inverse(&m_Camera.mtxProjInverse, NULL, &m_Camera.mtxProj);
}

bool CContext::SetViewSize(int width, int height)
{
	m_nViewWidth = width;
	m_nViewHeight = height;

	return true;
}

int CContext::GetViewWidth()
{
	if (0 == m_nViewWidth)
	{
		return m_pRender->GetDeviceWidth();
	}

	return m_nViewWidth;
}

int CContext::GetViewHeight()
{
	if (0 == m_nViewHeight)
	{
		return m_pRender->GetDeviceHeight();
	}

	return m_nViewHeight;
}

void CContext::MakeClipPlane()
{
//	Helper_MakeViewPlanes(m_Camera.mtxView, m_Camera.mtxProj, 
//		m_ClipPlane, 4);
	Helper_MakeViewPlanes(m_Camera.mtxView, m_Camera.mtxDxProj, 
		m_ClipPlane, 4);

}

// render target

void CContext::SetBlendDownLevel(int value)
{
	if (value != m_nBlendDownLevel)
	{
		m_nBlendDownLevel = value;

		this->ReleaseBlendRT();
		this->ReleaseRefractionRT();

		if (m_nBlendDownLevel > 0)
		{
			this->CreateBlendRT();
		}

		if (m_bEnableRealizeRefraction)
		{
			this->CreateRefractionRT();
		}
	}
}

int CContext::GetBlendDownLevel()
{
	return m_nBlendDownLevel;
}

IColorRT* CContext::GetBlendColorRT()
{
	return m_pBlendColorRT;
}

IDepthRT* CContext::GetBlendDepthRT()
{
	return m_pBlendDepthRT;
}

bool CContext::CreateBlendRT()
{
	if (m_pBlendColorRT)
	{
		return true;
	}

	int width = this->GetRenderWidth() >> m_nBlendDownLevel;
	int height = this->GetRenderHeight() >> m_nBlendDownLevel;

	m_pBlendColorRT = m_pRender->CreateColorRT(width, height, 
		TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pBlendColorRT)
	{
		CORE_TRACE("(CContext::CreateBlendRT)create color rt failed");
		return false;
	}

	m_pBlendDepthRT = m_pRender->CreateDepthRT(width, height,
		TEX_FORMAT_D16_UINT, RT_TYPE_DEVICE);

	if (NULL == m_pBlendDepthRT)
	{
		m_pBlendDepthRT = m_pRender->CreateDepthRT(width, height,
			TEX_FORMAT_D24S8_UINT, RT_TYPE_DEVICE);

		if (NULL == m_pBlendDepthRT)
		{
			CORE_TRACE("(CContext::CreateBlendRT)create depth rt failed");
			SAFE_RELEASE(m_pBlendColorRT);
			return false;
		}
	}

	return true;
}

bool CContext::ReleaseBlendRT()
{
	SAFE_RELEASE(m_pBlendColorRT);
	SAFE_RELEASE(m_pBlendDepthRT);

	return true;
}

void CContext::SetEnableRealizeRefraction(bool value)
{
	if (value)
	{
		CreateRefractionRT();
	}
	else
	{
		ReleaseRefractionRT();
	}

	m_bEnableRealizeRefraction = value;
}

bool CContext::GetEnableRealizeRefraction()
{
	return m_bEnableRealizeRefraction && (m_pRefractionRT != NULL);
}

IColorRT* CContext::GetRefractionRT()
{
	return m_pRefractionRT;
}

bool CContext::CreateRefractionRT()
{
	if (m_pRefractionRT)
	{
		return true;
	}

	int width = this->GetRenderWidth() >> m_nBlendDownLevel;
	int height = this->GetRenderHeight() >> m_nBlendDownLevel;

	m_pRefractionRT = m_pRender->CreateColorRT(width, height,
		TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pRefractionRT)
	{
		CORE_TRACE("(CContext::CreateRefractionRT)CreateTexture failed");
		return false;
	}

	if (NULL == m_pBlendDepthRT)
	{
		m_pBlendDepthRT = m_pRender->CreateDepthRT(width, height,
			TEX_FORMAT_D16_UINT, RT_TYPE_DEVICE);

		if (NULL == m_pBlendDepthRT)
		{
			CORE_TRACE("(CContext::CreateRefractionRT)create depth rt failed");
			return false;
		}
	}

	return true;
}

bool CContext::ReleaseRefractionRT()
{
	SAFE_RELEASE(m_pRefractionRT);

	return true;
}

// 是否开启零食渲染RT
// 是否要开启临时渲染纹理缓冲
void CContext::SetEnableRealizeTempColorRT(bool value)
{
	if (value)
	{
		CreateTempColorRT();
	}
	else
	{
		ReleaseTempColorRT();
	}

	m_bEnableRealizeTempColorRT = value;
}

bool CContext::GetEnableRealizeTempColorRT()
{
	return m_bEnableRealizeTempColorRT && (m_pRealizeTempColorRT != NULL);
}

IColorRT* CContext::GetRealizeTempColorRT()
{
	return m_pRealizeTempColorRT;
}

// 是否要开启临时渲染深度缓冲
void CContext::SetEnableRealizTempDepthRT(bool value)
{
	if (value)
	{
		CreateTempDepthRT();
	}
	else
	{
		ReleaseTempDepthRT();
	}

	m_bEnableRealizeTempDepthRT = value;
}

bool CContext::GetEnableRealizTempDepthRT()
{
	return m_bEnableRealizeTempDepthRT && (m_pRealizeTempDepthRT != NULL);;
}

IDepthRT* CContext::GetRealizeTempDepthRT()
{
	return m_pRealizeTempDepthRT;
}

// 不可采样的零时深度缓冲区
void CContext::SetEnableRealizeTempDepthTex(bool value)
{
	if (value)
	{
		CreateTempDepthTex();
	}
	else
	{
		ReleaseTempDepthTex();
	}

	m_bEnableRealizeTempDepthTex = value;
}

bool CContext::GetEnableRealizeTempDepthTex()
{
	return m_bEnableRealizeTempDepthTex && (m_pRealizeTempDepthTex != NULL);;
}

IDepthTex* CContext::GetRealizeTempDepthTex()
{
	return m_pRealizeTempDepthTex;
}

// 可采样的零时深度模板缓冲区
void CContext::SetEnableRealizeTempDepthStencilRT(bool value)
{
	if (value)
	{
		CreateTempDepthStencilRT();
	}
	else
	{
		ReleaseTempDepthStencilRT();
	}

	m_bEnableRealizeTempDephtStencilRT = value;
}

bool CContext::GetEnableRealizeTempDepthStencilRT()
{
	return (m_pRealizeTempDepthStencilRT != NULL ) && m_bEnableRealizeTempDephtStencilRT;
}

IDepthStencilRT* CContext::GetRealizeTempDepthStencilRT()
{
	return m_pRealizeTempDepthStencilRT;
}

// 临时模板缓冲区
void CContext::SetEnableRealizeTempStencilTex(bool value)
{
	if (value)
	{
		CreateTempStencilTex();
	}
	else
	{
		ReleaseTempStencilTex();
	}

	m_bEnableRealizeTempStencilTex = value;
}

bool CContext::GetEnableRealizeTempStencilTex()
{
	return (m_pRealizeTempStencilTex != NULL ) && m_bEnableRealizeTempStencilTex;
}

IStencilTex* CContext::GetRealizeTempStencilTex()
{
	return m_pRealizeTempStencilTex;
}

void CContext::SetEnableRealizeCopyTempColorRT(bool value)
{
	if (value)
	{
		CreateCopyTempColorRT();
	}
	else
	{
		ReleaseCopyTempColorRT();
	}

	m_bEnableRealizeCopyTempColorRT = value;
}

bool CContext::GetEnableRealizeCopyTempColorRT()
{
	return m_bEnableRealizeCopyTempColorRT && (m_pRealizeCopyColorTempRT != NULL);
}

IColorRT* CContext::GetRealizeCopyTempColorRT()
{
	return m_pRealizeCopyColorTempRT;
}

// 是否要开启临时渲染深度缓冲
void CContext::SetEnableRealizeCopyTempDepth(bool value)
{
	if (value)
	{
		CreateCopyTempDepth();
	}
	else
	{
		ReleaseCopyTempDepth();
	}

	m_bEnableRealizeCopyTempDepthRT = value;
}

bool CContext::GetEnableRealizeCopyTempDepth() const
{
	return m_bEnableRealizeCopyTempDepthRT && (m_pRealizeCopyTempDepthRT != NULL);
}

IColorRT* CContext::GetRealizeCopyTempDepth()
{
	return m_pRealizeCopyTempDepthRT;
}


// 用于测试RT渲染的调试函数 用于debug版本
void CContext::SetDebugColorRT(IColorRT* pColorRT)
{
	m_pDebugColorRT = pColorRT;
}

IColorRT* CContext::GetDebugColorRT()
{
	return m_pDebugColorRT;
}

// 用于测试RT渲染调试函数 用于 debug版本
void CContext::SetDebugDepthRT(IDepthRT* pDepthRT)
{
	m_pDebugDepthRT = pDepthRT;
}

IDepthRT* CContext::GetDebugDepthRT()
{
	return m_pDebugDepthRT;
}

// 用于测试Texture渲染的调试函数 用于debug版本
void CContext::SetDebugColorTexture(ITexture* pTex)
{
	m_pDebugTex = pTex;
}

ITexture* CContext::GetDebugTexture()
{
	return m_pDebugTex;
}

void CContext::SetDebugDepthStencilRT(IDepthStencilRT* pDepthStencilRT)
{
	m_pDebugDepthStencilRT = pDepthStencilRT;
}

IDepthStencilRT* CContext::GetDebugDepthStencilRT()
{
	return m_pDebugDepthStencilRT;
}

// 用于测试一些解码ColorRT 用于debug版本哦
void CContext::SetDebugDecodeColorRT(IColorRT* pColorRT)
{
	m_pDecodeColorRT = pColorRT;
}

IColorRT* CContext::GetDebugDecodeColorRT()
{
	return m_pDecodeColorRT;
}

bool CContext::CreateCopyTempColorRT()
{
	if (m_pRealizeCopyColorTempRT)
	{
		return true;
	}
 
	int width = (int)(GetRenderWidth() * 0.5);
	int height =(int)(GetRenderHeight() * 0.5);

	m_pRealizeCopyColorTempRT = m_pRender->CreateColorRT(width, height,
		TEX_FORMAT_RGB565_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pRealizeCopyColorTempRT)
	{
		CORE_TRACE("(CContext::CreateTempRT)CreateTexture failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseCopyTempColorRT()
{
	SAFE_RELEASE(m_pRealizeCopyColorTempRT);
	return true;
}

bool CContext::CreateCopyTempDepth()
{
	if(m_pRealizeCopyTempDepthRT)
	{
		return true;
	}

	// 必须支持深度纹理采样
	if(! m_pRender->GetDeviceCaps()->IsDepthTextureSupported() )
	{
		CORE_TRACE("(CContext::CreateCopyTempDepth)CreateDepthRT failed cann't support depthtexture!");
		return false;
	}

//	int width = VisUtil_Pow2(GetRenderWidth());
// 	int height =VisUtil_Pow2(GetRenderHeight());
 	int width  = GetRenderWidth() * 0.5;
	int height = GetRenderHeight() * 0.5;

	m_pRealizeCopyTempDepthRT = m_pRender->CreateColorRT(width,height,TEX_FORMAT_RGB888_UNORM, RT_TYPE_DEVICE);
	if(m_pRealizeCopyTempDepthRT == NULL)
	{
        CORE_TRACE("(CContext::CreateCopyTempDepth)CreateDepthRT failed");
		return false;
	}
	return true;
}

bool CContext::ReleaseCopyTempDepth()
{
	SAFE_RELEASE(m_pRealizeCopyTempDepthRT);
	return true;
}

void CContext::SetGlobalEnvMap(const char* name)
{
	Assert(name != NULL);

	if (name[0] == 0)
	{
		return;
	}

	if (m_strGlobalEnvMap == name)
	{
		return;
	}

	m_pGlobalEnvMap = m_pRender->CreateCubeTexture(name, false);
	m_strGlobalEnvMap = name;
}

const char* CContext::GetGlobalEnvMap()
{
	return m_strGlobalEnvMap.c_str();
}

ITexture* CContext::GetGlobalEnvCubeTex()
{
	return m_pGlobalEnvMap;
}


void CContext::SetEnableDynamicShadow(bool value)
{
	if (value)
	{
		CreateShadowMapRT();
	}
	else
	{
		ReleaseShadowMapRT();
	}
	
	m_bEnableDynamicShadow = value;
}

bool CContext::GetEnableDynamicShadow()
{
	return m_bEnableDynamicShadow && (m_pShadowRT != NULL);
}


void CContext::ReleaseLayer()
{
	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		CORE_DELETE(m_Layers[i]);
	}

	m_Layers.clear();
}

void CContext::CollectLayer()
{
	ICore* pCore = m_pRender->GetCore();

	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		layer_data_t* layer = m_Layers[i];
		size_t k = 0;

		while (k < layer->objects.size())
		{		
			IEntity* pEntity = pCore->GetEntity(layer->objects[k]);

			if (NULL == pEntity)
			{
				layer->objects.remove(k);
			}
			else
			{
				++k;
			}
		}
	}
}

bool CContext::FindLayerIndex(const char* name, size_t& index) const
{
	Assert(name != NULL);

	unsigned int hash = GetHashValueCase(name);

	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		if ((m_Layers[i]->nHash == hash) && (m_Layers[i]->strName == name))
		{
			index = i;
			return true;
		}
	}

	return false;
}

bool CContext::LayerExists(const char* name)
{
	size_t index;

	return FindLayerIndex(name, index);
}

bool CContext::LayerClear(const char* name)
{
	size_t index;

	if (!FindLayerIndex(name, index))
	{
		return false;
	}

	CORE_DELETE(m_Layers[index]);
	m_Layers.remove(index);

	return true;
}

bool CContext::LayerAddObject(const char* name, IVisBase* pObj)
{
	Assert(pObj != NULL);

	CollectLayer();

	size_t index;

	if (!FindLayerIndex(name, index))
	{
		index = m_Layers.size();

		layer_data_t* p = CORE_NEW(layer_data_t);

		p->strName = name;
		p->nHash = GetHashValueCase(name);

		m_Layers.push_back(p);
	}

	m_Layers[index]->objects.push_back(pObj->GetID());

	return true;
}

bool CContext::LayerRemoveObject(const char* name, IVisBase* pObj)
{
	Assert(pObj != NULL);

	size_t index;

	if (!FindLayerIndex(name, index))
	{
		return false;
	}

	layer_data_t* layer = m_Layers[index];

	for (size_t i = 0; i < layer->objects.size(); ++i)
	{
		if (layer->objects[i] == pObj->GetID())
		{
			layer->objects.remove(i);
			return true;
		}
	}

	return false;
}

bool CContext::LayerCull(const char* name, const FmPlane* planes, 
						 size_t plane_num, const FmVec3& center, float radius, 
						 CULL_FUNC cull_func, void* context)
{
	size_t index;

	if (!FindLayerIndex(name, index))
	{
		return false;
	}

	ICore* pCore = m_pRender->GetCore();
	size_t size = m_Layers[index]->objects.size();
	PERSISTID* data = m_Layers[index]->objects.data();
	bool retval = false;

	for (size_t i = 0; i < size; ++i)
	{
		IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(data[i]);

		if (pVisBase != NULL && pVisBase->GetCullEnable())
		{
			if (pVisBase->Cull(planes, plane_num, center, radius, cull_func, 
				context))
			{
				retval = true;
			}
		}
	}

	return retval;
}

float CContext::LayerTrace(const char* name, const FmVec3& src, 
						   const FmVec3& dst)
{
	size_t index;

	if (!FindLayerIndex(name, index))
	{
		return 2.0F;
	}

	ICore* pCore = m_pRender->GetCore();
	size_t size = m_Layers[index]->objects.size();
	PERSISTID* data = m_Layers[index]->objects.data();
	float best_res = 2.0F;
	trace_info_t ti;

	for (size_t i = 0; i < size; ++i)
	{
		IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(data[i]);

		if (pVisBase != NULL && pVisBase->GetTraceEnable())
		{
			if (pVisBase->TraceDetail(src, dst, ti))
			{
				if (ti.fDistance < best_res)
				{
					best_res = ti.fDistance;
				}
			}
		}
	}

	return best_res;
}

bool CContext::LayerTraceHitted(const char* name, const FmVec3& src, 
								const FmVec3& dst)
{
	size_t index;

	if (!FindLayerIndex(name, index))
	{
		return false;
	}

	ICore* pCore = m_pRender->GetCore();
	size_t size = m_Layers[index]->objects.size();
	PERSISTID* data = m_Layers[index]->objects.data();

	for (size_t i = 0; i < size; ++i)
	{
		IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(data[i]);

		if (pVisBase != NULL && pVisBase->GetTraceEnable())
		{
			if (pVisBase->TraceHitted(src, dst))
			{
				return true;
			}
		}
	}

	return false;
}

bool CContext::SetUnique(IEntity* pEntity)
{
	Assert(pEntity != NULL);

	const char* entity_name = pEntity->GetEntInfo()->GetEntityName();

	if (m_Uniques.Exists(entity_name))
	{
		m_Uniques.Remove(entity_name);
	}

	m_Uniques.Add(entity_name, pEntity->GetID());

	return true;
}

IEntity* CContext::GetUnique(const char* entity_name)
{
	Assert(entity_name != NULL);

	PERSISTID id;

	if (!m_Uniques.GetData(entity_name, id))
	{
		return NULL;
	}

	return m_pRender->GetCore()->GetEntity(id);
}

bool CContext::CreateTempColorRT()
{
	if (m_pRealizeTempColorRT)
	{
		return true;
	}
 
 	int width = GetRenderWidth() * m_fRealizeTempRTScale;
	int height = GetRenderHeight() * m_fRealizeTempRTScale;

	m_pRealizeTempColorRT = m_pRender->CreateColorRT(width, height,
		TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pRealizeTempColorRT)
	{
		CORE_TRACE("(CContext::CreateTempRT)CreateTexture failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseTempColorRT()
{
	SAFE_RELEASE(m_pRealizeTempColorRT);
	return true;
}

bool CContext::CreateTempDepthRT()
{
	if (m_pRealizeTempDepthRT)
	{
		return true;
	}
 
 	int width = GetRenderWidth() * m_fRealizeTempRTScale;
	int height =GetRenderHeight() * m_fRealizeTempRTScale;

	//
	DeviceCaps* device_caps = (DeviceCaps*)m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID());

	if( !device_caps->IsDepthTextureSupported() )
	{
		return false;
	}

	if(device_caps->IsDepth24Supported())
	{
		m_pRealizeTempDepthRT = m_pRender->CreateDepthRT(width, height,
			TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
	}
	else if(device_caps->IsDepth32Supported())
	{
		m_pRealizeTempDepthRT = m_pRender->CreateDepthRT(width, height,
			TEX_FORMAT_D32_UINT, RT_TYPE_DEVICE);
	}
	else
	{
		m_pRealizeTempDepthRT = m_pRender->CreateDepthRT(width, height,
			TEX_FORMAT_D_DEFAULT, RT_TYPE_DEVICE);
	}


	if (NULL == m_pRealizeTempDepthRT)
	{
		CORE_TRACE("(CContext::CreateTempRT)CreateTexture failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseTempDepthRT()
{
	SAFE_RELEASE(m_pRealizeTempDepthRT);
	return true;
}


bool CContext::CreateTempDepthTex()
{
	if (m_pRealizeTempDepthTex)
	{
		return true;
	} 
 
 	int width = GetRenderWidth()*m_fRealizeTempRTScale;
	int height = GetRenderHeight()*m_fRealizeTempRTScale;

	//
	DeviceCaps* device_caps = (DeviceCaps*)m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID());

	if(device_caps->IsDepth24Supported())
	{
		m_pRealizeTempDepthTex = m_pRender->CreateDepthTex(width, height,
			TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
	}
	else if(device_caps->IsDepth32Supported())
	{
		m_pRealizeTempDepthTex = m_pRender->CreateDepthTex(width, height,
			TEX_FORMAT_D32_UINT, RT_TYPE_DEVICE);
	}
	else
	{
		m_pRealizeTempDepthTex = m_pRender->CreateDepthTex(width, height,
			TEX_FORMAT_D_DEFAULT, RT_TYPE_DEVICE);
	}

	if (NULL == m_pRealizeTempDepthTex)
	{
		CORE_TRACE("(CContext::CreateTempRT)CreateTexture failed");
		return false;
	}

	return true;
}

bool CContext::CreateTempDepthStencilRT()
{
	if( m_pRealizeTempDepthStencilRT )
	{
		return true;
	}

 	int width = GetRenderWidth()*m_fRealizeTempRTScale;
	int height =GetRenderHeight()*m_fRealizeTempRTScale;

	//
	DeviceCaps* device_caps = (DeviceCaps*)m_pRender->GetDeviceCaps();

	if(device_caps->IsDepth24Stencil8Supported())
	{
		m_pRealizeTempDepthStencilRT = m_pRender->CreateDepthStencilRT(width, height,
			TEX_FORMAT_D24S8_UINT, RT_TYPE_DEVICE);
	}

	if (NULL == m_pRealizeTempDepthStencilRT)
	{
		CORE_TRACE("(CContext::CreateTempDepthStencilRT)CreateTempDepthStencilRT failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseTempDepthStencilRT()
{
	SAFE_RELEASE(m_pRealizeTempDepthStencilRT);
	return true;
}

bool CContext::CreateTempStencilTex()
{
	if (m_pRealizeTempStencilTex)
	{
		return true;
	}

	int width = GetRenderWidth() * m_fRealizeTempRTScale;
	int height = GetRenderHeight()* m_fRealizeTempRTScale;

	//
	m_pRealizeTempStencilTex = m_pRender->CreateStencilTex(width, height,
		TEX_FORMAT_S_DEFAULT, RT_TYPE_DEVICE);

	if (NULL == m_pRealizeTempStencilTex)
	{
		CORE_TRACE("(CContext::CreateTempStencilTex)CreateTempStencilTex failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseTempStencilTex()
{
	SAFE_RELEASE(m_pRealizeTempStencilTex);
	return true;
}


bool CContext::ReleaseTempDepthTex()
{
	SAFE_RELEASE(m_pRealizeTempDepthTex);
	return true;
}

bool CContext::CreateShadowMapRT()
{
	if (m_pShadowRT)
	{
		return true;
	}

	m_pShadowRT = m_pRender->CreateColorRT(m_nShadowMapSize, 
		m_nShadowMapSize / 2, TEX_FORMAT_RGB565_UNORM, RT_TYPE_DEVICE);

	if (NULL == m_pShadowRT)
	{
		m_pShadowRT = m_pRender->CreateColorRT(m_nShadowMapSize, 
			m_nShadowMapSize / 2, TEX_FORMAT_RGB888_UNORM, RT_TYPE_DEVICE);

		if (NULL == m_pShadowRT)
		{
			m_pShadowRT = m_pRender->CreateColorRT(m_nShadowMapSize, 
				m_nShadowMapSize / 2, TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);

			if (NULL == m_pShadowRT)
			{
				CORE_TRACE("(CDxContext::CreateShadowRT)create shadow RT failed");
				return false;
			}
		}
	}

	m_pShadowRT->GetTextureSampler()->SetTextureUVWrapMode(ITextureSampler::TWM_CLAMP_TO_EDGE, ITextureSampler::TWM_CLAMP_TO_EDGE);

	Assert(NULL == m_pShadowMapDS[0]);

	DeviceCaps* device_caps = (DeviceCaps*)m_pRender->GetCore()->GetEntity(m_pRender->GetDeviceCapsID());

	if(device_caps->IsDepth24Supported())
	{
		m_pShadowMapDS[0] = m_pRender->CreateDepthTex(m_nShadowMapSize, m_nShadowMapSize/ 2,
			TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
	}
	else if(device_caps->IsDepth32Supported())
	{
		m_pShadowMapDS[0] = m_pRender->CreateDepthTex(m_nShadowMapSize, m_nShadowMapSize/ 2,
			TEX_FORMAT_D32_UINT, RT_TYPE_DEVICE);
	}
	else
	{
		m_pShadowMapDS[0] = m_pRender->CreateDepthTex(m_nShadowMapSize, m_nShadowMapSize/ 2,
			TEX_FORMAT_D_DEFAULT, RT_TYPE_DEVICE);
	}

	if (NULL == m_pShadowMapDS[0])
	{
		CORE_TRACE("(PSSM::Load)create DS failed");
		return false;
	}

	return true;
}

bool CContext::ReleaseShadowMapRT()
{
	for (int i = 0; i < MAX_SHADOWMAP_NUM; ++i)
	{
		SAFE_RELEASE(m_pShadowMapRT[i]);
		SAFE_RELEASE(m_pShadowMapDS[i]);
	}

	SAFE_RELEASE(m_pShadowRT);

	return true;
}


void CContext::SetPercentageCloserFilter(bool value)
{
	if (value != m_bPercentageCloserFilter)
	{
		m_bPercentageCloserFilter = value;

		this->ReleaseShadowMapRT();

		if (m_bEnableDynamicShadow)
		{
			this->CreateShadowMapRT();
		}
	}
}

bool CContext::GetPercentageCloserFilter()
{
	return m_bPercentageCloserFilter;
}

void CContext::SetVarianceShadowMap(bool value)
{
	if (value != m_bVarianceShadowMap)
	{
		m_bVarianceShadowMap = value;
		
		this->ReleaseShadowMapRT();

		if (m_bEnableDynamicShadow)
		{
			this->CreateShadowMapRT();
		}
	}
}

bool CContext::GetVarianceShadowMap()
{
	return m_bVarianceShadowMap;
}

// 获得视锥顶点
void CContext::GetViewFrustumPointList(FmMat4 mtxView, float fFOV, float fAspect, float fNear, float fFar, FmVec3* vFrustum)
{
	FmMat4 mtxProj;
	FmMat4PerspectiveFovLH(&mtxProj, fFOV, fAspect, fNear, fFar, 0);

	FmVec3 vScreen[8];
	vScreen[0] = FmVec3(-1.0F, +1.0F, 0.0F);
	vScreen[1] = FmVec3(-1.0F, +1.0F, 1.0F);
	vScreen[2] = FmVec3(-1.0F, -1.0F, 0.0F);
	vScreen[3] = FmVec3(-1.0F, -1.0F, 1.0F);
	vScreen[4] = FmVec3(+1.0F, -1.0F, 0.0F);
	vScreen[5] = FmVec3(+1.0F, -1.0F, 1.0F);
	vScreen[6] = FmVec3(+1.0F, +1.0F, 0.0F);
	vScreen[7] = FmVec3(+1.0F, +1.0F, 1.0F);

	FmMat4 mtxTransform;
	FmMat4Multiply(&mtxTransform, &mtxView, &mtxProj);
	FmMat4Inverse(&mtxTransform, NULL, &mtxTransform);

	for (size_t i = 0; i < 8; ++i)
	{
		FmVec3TransformCoord(&vFrustum[i], &vScreen[i], &mtxTransform);
	}
}

// 获得中心点
void CContext::GetPointListCenter(unsigned int nPointCount, 
								 const FmVec3* pPointList, FmVec3* center)
{
	center->x = 0.0f;
	center->y = 0.0f;
	center->z = 0.0f;

	for (unsigned int i = 0; i < nPointCount; ++i)
	{
		*center += pPointList[i];
	}

	*center /= (float)nPointCount;
}

void CContext::CalculateShadowMapMatrix()
{
	unsigned int shadow_map_num = m_nShadowMapCount;

	if (0 == shadow_map_num)
	{
		return;
	}

	const camera_t& camera = m_Camera;
	FmMat4 mtxView = camera.mtxView;
	FmMat4 mtxViewInverse = camera.mtxViewInverse;
	FmVec3 vViewRight = FmVec3(1.0F, 0.0F, 0.0F);

	FmVec3TransformNormal(&vViewRight, &vViewRight, &mtxViewInverse);

	FmVec3 vSun = GetVector(IRenderContext::V_LIGHT_DIRECTION);

	FmVec3Normalize(&vSun, &vSun);

	bool can_reuse = false;

	if (m_fShadowExpandPercent > 0.001F)
	{
		// 光源方向是否变化
		if (FloatEqual(vSun.x, m_vShadowLightDir.x)
			&& FloatEqual(vSun.y, m_vShadowLightDir.y)
			&& FloatEqual(vSun.z, m_vShadowLightDir.z))
		{
			can_reuse = true;
		}
	}

	m_vShadowLightDir = vSun;
	vSun *= m_fCameraDistance;

	float fFOV = camera.fFov;
	float fAspect = camera.fRatio;
	float fNear = camera.fNearZ;
	float fFar = camera.fFarZ;
	float shadow_distance = GetShadowDistance();

	for (unsigned int i = 0; i < shadow_map_num; ++i)
	{
		float fTempNear;
		float fTempFar;

		if (shadow_map_num > 1)
		{
			float n = fNear;
			float f = shadow_distance;
			float factor_n = float(i) / float(shadow_map_num);
			float factor_f = float(i + 1) / float(shadow_map_num);
			float log_split_n = n * pow(f / n, factor_n);
			float uni_split_n = n + (f - n) * factor_n;

			fTempNear = log_split_n * m_fLogSplitWeight 
				+ (1.0F - m_fLogSplitWeight) * uni_split_n;

			float log_split_f = n * pow(f / n, factor_f);
			float uni_split_f = n + (f - n) * factor_f;

			fTempFar = log_split_f * m_fLogSplitWeight 
				+ (1.0F - m_fLogSplitWeight) * uni_split_f;
		}
		else
		{
			fTempNear = fNear;
			fTempFar = shadow_distance;
		}

		// 视锥的八个顶点的位置
		FmVec3 vViewFrustum[8];
		// 视锥的中心点
		FmVec3 vViewFrustumCenter;

		GetViewFrustumPointList(mtxView, fFOV, fAspect, fTempNear, 
			fTempFar, vViewFrustum);
		GetPointListCenter(8, vViewFrustum, &vViewFrustumCenter);

		FmMat4 mtxShadowMapView;
		FmMat4 mtxShadowMapProj;

		if (can_reuse)
		{
			// 转换到上一帧光源空间的视锥顶点
			const FmMat4* pShadowMapView = &m_mtxShadowMapView[i];
			FmVec3 vPrevLightFrustum[8];

			FmVec3TransformCoordStream(vPrevLightFrustum, sizeof(FmVec3),
				vViewFrustum, sizeof(FmVec3), 8, pShadowMapView);

			// 测试上一帧的投影空间是否可以包含需要投影的视锥
			for (unsigned int k = 0; k < 8; ++k)
			{
				float frustum_x = vPrevLightFrustum[k].x;
				float frustum_y = vPrevLightFrustum[k].y;

				if (frustum_x < m_vShadowOrthoMin[i].x)
				{
					can_reuse = false;
					break;
				}

				if (frustum_x > m_vShadowOrthoMax[i].x)
				{
					can_reuse = false;
					break;
				}

				if (frustum_y < m_vShadowOrthoMin[i].y)
				{
					can_reuse = false;
					break;
				}

				if (frustum_y > m_vShadowOrthoMax[i].y)
				{
					can_reuse = false;
					break;
				}
			}
		}

		if (can_reuse)
		{
			FmVec3 vCenter = m_vShadowOrthoCenter[i] - vViewFrustumCenter;
			float center_dist = FmVec3Length(&vCenter);

			//float center_dist = D3DXVec3Length(
			//	&(m_vShadowOrthoCenter[i] - vViewFrustumCenter));

			if (center_dist < m_fCameraDistance * 0.1F) 
			{
				if (m_bShadowMapReusable)
				{
					// 减少远处阴影图的刷新频率
					if (shadow_map_num == 4)
					{
						if (i == 2)
						{
							// 两帧刷新一次
							m_nShadowMapNoFlush[i] = 
								(m_nShadowMapNoFlush[i] + 1) & 0x1;
						}
						else if (i == 3)
						{
							// 四帧刷新一次
							m_nShadowMapNoFlush[i] = 
								(m_nShadowMapNoFlush[i] + 1) & 0x3;
						}
						else
						{
							m_nShadowMapNoFlush[i] = 0;
						}
					}
					else
					{
						if (i == 1)
						{
							// 两帧刷新一次
							m_nShadowMapNoFlush[i] = 
								(m_nShadowMapNoFlush[i] + 1) & 0x1;
						}
						else if (i == 2)
						{
							// 四帧刷新一次
							m_nShadowMapNoFlush[i] = 
								(m_nShadowMapNoFlush[i] + 1) & 0x3;
						}
						else
						{
							m_nShadowMapNoFlush[i] = 0;
						}
					}
				}
				else
				{
					m_nShadowMapNoFlush[i] = 0;
				}

				// 阴影投影矩阵不需要变化，这样可以防止抖动
				continue;
			}
		}

		m_nShadowMapNoFlush[i] = 0;

		// 阴影图的视矩阵
		FmVec3 vLightLookAt = vViewFrustumCenter;
		FmVec3 vLightEye = vLightLookAt + vSun;
		FmVec3 vLightDir = -vSun;
		FmVec3 vLightUp;
		FmMat4 mtxShadowView;
		FmMat4 mtxShadowProj;

		vLightDir = vLightDir.normalize();
		FmVec3Cross(&vLightUp, &vLightDir, &vViewRight);
		FmMat4LookAtLH(&mtxShadowView, &vLightEye, &vLightLookAt, &vLightUp);

		// 转换到光源空间的视锥顶点
		FmVec3 vLightViewFrustum[8];
		FmVec3 vLightViewFrustumCenter;

		FmVec3TransformCoordStream(vLightViewFrustum, sizeof(FmVec3), vViewFrustum, sizeof(FmVec3), 8, &mtxShadowView);
		FmVec3TransformCoord(&vLightViewFrustumCenter, &vViewFrustumCenter, &mtxShadowView);

		// 阴影图的投影矩阵
		FmVec4 vOrthoRect(-1.0f, -1.0f, 1.0f, 1.0f);

		for (unsigned int t = 0; t < 8; ++t)
		{
			float frustum_x = vLightViewFrustum[t].x;
			float frustum_y = vLightViewFrustum[t].y;

			if (frustum_x < vOrthoRect.x)
			{
				vOrthoRect.x = frustum_x;
			}

			if (frustum_x > vOrthoRect.y)
			{
				vOrthoRect.y = frustum_x;
			}

			if (frustum_y < vOrthoRect.z)
			{
				vOrthoRect.z = frustum_y;
			}

			if (frustum_y > vOrthoRect.w)
			{
				vOrthoRect.w = frustum_y;
			}
		}

		// 调整阴影投影矩阵大小
		vOrthoRect += m_vAdjustingRect;

		if (m_fShadowExpandPercent > 0.001F)
		{
			// 扩大投影面积
			float expand_value = 1.0F + m_fShadowExpandPercent;

			vOrthoRect.x *= expand_value;
			vOrthoRect.y *= expand_value;
			vOrthoRect.z *= expand_value;
			vOrthoRect.w *= expand_value;

			// 防止阴影边缘抖动
			float ortho_w = m_vShadowOrthoMax[i].x - m_vShadowOrthoMin[i].x;
			float ortho_h = m_vShadowOrthoMax[i].y - m_vShadowOrthoMin[i].y;
			float pixel_size_x = ortho_w / float(m_nShadowMapSize);
			float pixel_size_y = ortho_h / float(m_nShadowMapSize / 2);

			if ((pixel_size_x > 0) && (pixel_size_y > 0))
			{
				int num_x = int((vOrthoRect.y - vOrthoRect.x) / pixel_size_x);
				int num_y = int((vOrthoRect.w - vOrthoRect.z) / pixel_size_y);

				vOrthoRect.y = vOrthoRect.x + num_x * pixel_size_x;
				vOrthoRect.w = vOrthoRect.z + num_y * pixel_size_y;
			}
		}

		float distance_add = m_fCameraDistance * 0.5F;

		if (distance_add < shadow_distance)
		{
			distance_add = shadow_distance;
		}

		distance_add = m_fCameraDistance + distance_add;
		FmMat4OrthoOffCenterLH(&mtxShadowProj, vOrthoRect.x, vOrthoRect.y, vOrthoRect.z, vOrthoRect.w, 0.1F, distance_add);

		m_vShadowOrthoMin[i] = FmVec2(vOrthoRect.x, vOrthoRect.z);
		m_vShadowOrthoMax[i] = FmVec2(vOrthoRect.y, vOrthoRect.w);
		m_vShadowOrthoCenter[i] = vViewFrustumCenter;

		m_mtxShadowMapView[i] = mtxShadowView;
		m_mtxShadowMapProj[i] = mtxShadowProj;
	}
}

const FmMat4* CContext::GetShadowMapViewMatrix(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	return &m_mtxShadowMapView[index];
}

const FmMat4* CContext::GetShadowMapProjMatrix(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	return &m_mtxShadowMapProj[index];
}

bool CContext::GetShadowMapNoFlush(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	return m_nShadowMapNoFlush[index] != 0;
}

void CContext::SetDepthSlope(int index, float value)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	m_fDepthSlope[index] = value;
}

float CContext::GetDepthSlope(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	return m_fDepthSlope[index];
}

void CContext::SetDepthBias(int index, float value)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	m_fDepthBias[index] = value;
}

float CContext::GetDepthBias(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	return m_fDepthBias[index];
}

void CContext::SetShadowMapCount(int value)
{
	if ((value <= 0) || (value > MAX_SHADOWMAP_NUM))
	{
		return;
	}

	if (value != m_nShadowMapCount)
	{
		m_nShadowMapCount = value;
		
		this->ReleaseShadowMapRT();

		if (m_bEnableDynamicShadow)
		{
			this->CreateShadowMapRT();
		}
	}
}

int CContext::GetShadowMapCount()
{
	return m_nShadowMapCount;
}

void CContext::SetShadowMapIndex(int value)
{
	m_nShadowMapIndex = value;
}

int CContext::GetShadowMapIndex()
{
	return m_nShadowMapIndex;
}

void CContext::SetLogSplitWeight(float value)
{
	m_fLogSplitWeight = value;
}

float CContext::GetLogSplitWeight()
{
	return m_fLogSplitWeight;
}

void CContext::SetShadowMapSize(unsigned int value)
{
	Assert(value != 0);

	if (value != m_nShadowMapSize)
	{
		m_nShadowMapSize = value;

		this->ReleaseShadowMapRT();

		if (m_bEnableDynamicShadow)
		{
			this->CreateShadowMapRT();
		}
	}
}

unsigned int CContext::GetShadowMapSize()
{
	return m_nShadowMapSize;
}

void CContext::SetShadowMapCameraDistance(float value)
{
	m_fCameraDistance = value;
}

float CContext::GetShadowMapCameraDistance()
{
	return m_fCameraDistance;
}

void CContext::SetShadowExpandPercent(float value)
{
	m_fShadowExpandPercent = value;
}

float CContext::GetShadowExpandPercent()
{
	return m_fShadowExpandPercent;
}

void CContext::SetShadowMapReusable(bool value)
{
	m_bShadowMapReusable = value;
}

bool CContext::GetShadowMapReusable()
{
	return m_bShadowMapReusable;
}


IColorRT* CContext::GetShadowMapRT(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);
	
	if (index >= MAX_SHADOWMAP_NUM)
	{
		return NULL;
	}

	return m_pShadowMapRT[index];
}

IDepthTex* CContext::GetShadowMapDS(int index)
{
	Assert(index < MAX_SHADOWMAP_NUM);

	if (index >= MAX_SHADOWMAP_NUM)
	{
		return NULL;
	}

	return m_pShadowMapDS[index];
}

IColorRT* CContext::GetDynamicShadowRT()
{
	return m_pShadowRT;
}

void CContext::SetShadowAdjustingRect(FmVec4 vAdjustingRect)
{
	m_vAdjustingRect = vAdjustingRect;
}

const FmVec4& CContext::GetShadowAdjustingRect()
{
	return m_vAdjustingRect;
}