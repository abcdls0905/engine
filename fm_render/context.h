//--------------------------------------------------------------------
// �ļ���:		context.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"

class TContextAlloc
{
public:
	TContextAlloc() {}
	~TContextAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TContextAlloc& src) {}
};

// ��Ⱦ�����Ĳ���

class Render;

class CContext : public IRenderContext
{
public:
	static CContext* NewInstance(Render* pRender);

private:
	struct layer_data_t
	{
		core_string strName;
		unsigned int nHash;
		TArrayPod<PERSISTID, 1, TContextAlloc> objects;
	};

public:
	CContext(Render* pRender);
	~CContext();

	// �Ƿ���Clear 
	void SetEnableClearColorOptimize(bool value);
	bool GetEnableClearColorOptimize();

	// �ͷ�
	void Release();

	// �����Ӿ������
	bool SetCameraView(const FmVec3& pos, 
		const FmVec3& dir);
	// ����ͶӰ�������
	bool SetCameraProj(float fov, float zn, float zf, float ratio); 
	// �����Ӿ���
	bool SetViewMatrix(const FmMat4& mat);
	// ����ͶӰ����
	bool SetProjMatrix(const FmMat4& mat);
	// ���ɸ�������
	void MakeHelperMatrix();
	// ������ȾĿ��ߴ�
	bool SetViewSize(int width, int height);
	// �����ȾĿ��ߴ�
	int GetViewWidth();
	int GetViewHeight();

	// ���ɼ�����
	void MakeClipPlane();

	// ��͸�����彵����ʾ
	void SetBlendDownLevel(int value);
	int GetBlendDownLevel();
	IColorRT* GetBlendColorRT();
	IDepthRT* GetBlendDepthRT();

	// �Ƿ�Ҫ��Ⱦ����ͼ
	void SetEnableRealizeRefraction(bool value);
	bool GetEnableRealizeRefraction();
	IColorRT* GetRefractionRT();

	// �Ƿ�����ʳ��ȾRT
	// �Ƿ�Ҫ������ʱ��Ⱦ������
	void SetEnableRealizeTempColorRT(bool value);
	bool GetEnableRealizeTempColorRT();
	IColorRT* GetRealizeTempColorRT();

	// �Ƿ�Ҫ������ʱ��Ⱦ��Ȼ���
	void SetEnableRealizTempDepthRT(bool value);
	bool GetEnableRealizTempDepthRT();
	IDepthRT* GetRealizeTempDepthRT();

	// �Ƿ�Ҫ������ʱ��Ⱦ������
	void SetEnableRealizeCopyTempColorRT(bool value);
	bool GetEnableRealizeCopyTempColorRT();
	IColorRT* GetRealizeCopyTempColorRT();

	// ���ɲ�������ʱ��Ȼ�����
	virtual void SetEnableRealizeTempDepthTex(bool value);
	virtual bool GetEnableRealizeTempDepthTex();
	virtual IDepthTex* GetRealizeTempDepthTex();

	// �ɲ�������ʱ���ģ�建����
	virtual void SetEnableRealizeTempDepthStencilRT(bool value);
	virtual bool GetEnableRealizeTempDepthStencilRT();
	virtual IDepthStencilRT* GetRealizeTempDepthStencilRT();

	// ��ʱģ�建����
	virtual void SetEnableRealizeTempStencilTex(bool value);
	virtual bool GetEnableRealizeTempStencilTex();
	virtual IStencilTex* GetRealizeTempStencilTex();

	// �Ƿ�Ҫ������ʱ��Ⱦ��Ȼ���
	virtual void SetEnableRealizeCopyTempDepth(bool value);
	virtual bool GetEnableRealizeCopyTempDepth() const;
	virtual IColorRT* GetRealizeCopyTempDepth();

	// ���ڲ���RT��Ⱦ�ĵ��Ժ��� ����debug�汾
	void SetDebugColorRT(IColorRT* pColorRT);
	IColorRT* GetDebugColorRT();

	// ���ڲ���RT��Ⱦ���Ժ��� ���� debug�汾
	void SetDebugDepthRT(IDepthRT* pDepthRT);
	IDepthRT* GetDebugDepthRT();

	// ���ڲ���Texture��Ⱦ�ĵ��Ժ��� ����debug�汾
	void SetDebugColorTexture(ITexture* pTex);
	ITexture* GetDebugTexture();

	// ���ڲ���RT��Ⱦ���Ժ��� ����debug�汾
	virtual void SetDebugDepthStencilRT(IDepthStencilRT* pDepthStencilRT);
	virtual IDepthStencilRT* GetDebugDepthStencilRT();

	// ���ڲ���һЩ����ColorRT ����debug�汾Ŷ
	virtual void SetDebugDecodeColorRT(IColorRT* pColorRT);
    virtual IColorRT* GetDebugDecodeColorRT();

	// �Ƿ���ȫ�ֻ�����ͼ
	void SetGlobalEnvMap(const char* name);
	const char* GetGlobalEnvMap();
	ITexture* GetGlobalEnvCubeTex();

	// �Ƿ�����̬��Ӱ
	void SetEnableDynamicShadow(bool value) ;
	bool GetEnableDynamicShadow() ;

	// �Ƿ�ʹ��Ӳ����Ӱͼ����
	void SetPercentageCloserFilter(bool value) ;
	bool GetPercentageCloserFilter() ;

	// �Ƿ�ʹ�÷�����Ӱͼ
	void SetVarianceShadowMap(bool value) ;
	bool GetVarianceShadowMap();

	// ������Ӱͼ����
	void CalculateShadowMapMatrix();

	// �����Ӱͼ����
	const FmMat4* GetShadowMapViewMatrix(int index);
	const FmMat4* GetShadowMapProjMatrix(int index); 

	// ��Ӱͼ�Ƿ���Լ���ʹ�ã�����ˢ�£�
	bool GetShadowMapNoFlush(int index);

	// ���б��ƫ��
	void SetDepthSlope(int index, float value);
	float GetDepthSlope(int index) ;

	// ��ȳ���ƫ��
	void SetDepthBias(int index, float value) ;
	float GetDepthBias(int index) ;

	// ��Ӱͼ����
	void SetShadowMapCount(int value) ;
	int GetShadowMapCount() ;

	// ��ǰ��Ӱͼ����
	void SetShadowMapIndex(int value) ;
	int GetShadowMapIndex() ;

	// ��Ӱͼ�����з�Ȩ��
	void SetLogSplitWeight(float value) ;
	float GetLogSplitWeight();

	// ��Ӱͼ�ߴ�
	void SetShadowMapSize(unsigned int value);
	unsigned int GetShadowMapSize();

	// ��Ӱͼ�������Ŀ������ľ���
	void SetShadowMapCameraDistance(float value) ;
	float GetShadowMapCameraDistance() ;

	// ��ӰͶӰ�����չ�ٷֱ�
	void SetShadowExpandPercent(float value) ;
	float GetShadowExpandPercent() ;

	// Զ������Ӱͼ�Ƿ�����ظ�ʹ��
	void SetShadowMapReusable(bool value);
	bool GetShadowMapReusable();

	// �����Ӱͼ��ȾĿ��
	IColorRT* GetShadowMapRT(int index);
	IDepthTex* GetShadowMapDS(int index);

	// �����Ļ�ռ䶯̬��Ӱ���ͼ
	IColorRT* GetDynamicShadowRT();
	// ���Ƿ����
	bool LayerExists(const char* name);
	// ��ղ�
	bool LayerClear(const char* name);
	// ��Ӷ��󵽲�
	bool LayerAddObject(const char* name, IVisBase* pObj);
	// �Ӳ�ɾ������
	bool LayerRemoveObject(const char* name, IVisBase* pObj);
	// �Բ�����ѡ
	bool LayerCull(const char* name, const FmPlane* planes, 
		size_t plane_num, const FmVec3& center, float radius, 
		CULL_FUNC cull_func, void* context);
	// �Բ�����ײ���
	float LayerTrace(const char* name, const FmVec3& src, 
		const FmVec3& dst);
	bool LayerTraceHitted(const char* name, const FmVec3& src, 
		const FmVec3& dst);

	// ���ó���Ψһ��ʵ��
	bool SetUnique(IEntity* pEntity);
	// ��ó���Ψһ��ʵ��
	IEntity* GetUnique(const char* entity_name);

	// ����ֵ
	int GetInt(size_t index) const 
	{
		Assert(index < MAX_INT_NUM);

		return m_IntValues[index];
	}

	void SetInt(size_t index, int value)
	{
		Assert(index < MAX_INT_NUM);

		m_IntValues[index] = value;
	}

	// ����ֵ
	float GetFloat(size_t index) const
	{
		Assert(index < MAX_FLOAT_NUM);

		return m_FloatValues[index];
	}

	void SetFloat(size_t index, float value)
	{
		Assert(index < MAX_FLOAT_NUM);

		m_FloatValues[index] = value;
	}

	// ��ɫֵ
	unsigned int GetColor(size_t index) const
	{
		Assert(index < MAX_COLOR_NUM);

		return m_ColorValues[index];
	}

	void SetColor(size_t index, unsigned int value)
	{
		Assert(index < MAX_COLOR_NUM);

		m_ColorValues[index] = value;
	}

	// ����ֵ
	const FmVec3& GetVector(size_t index) const
	{
		Assert(index < MAX_VECTOR_NUM);

		return *(FmVec3*)&m_VectorValues[index];
	}

	void SetVector(size_t index, const FmVec3& value)
	{
		Assert(index < MAX_VECTOR_NUM);

		*(FmVec3*)&m_VectorValues[index] = value;
	}

	// 4Ԫ����ֵ
	const FmVec4& GetVector4(size_t index) const
	{
		Assert(index < MAX_VECTOR_NUM);

		return m_VectorValues[index];
	}

	void SetVector4(size_t index, const FmVec4& value)
	{
		Assert(index < MAX_VECTOR_NUM);

		m_VectorValues[index] = value;
	}

	// �ο�λ��
	void SetReferPosition(float x, float y, float z)
	{
		m_vReferPosition = FmVec3(x, y, z);
	}

	const FmVec3& GetReferPosition() const { return m_vReferPosition; }

	// ��õ�ǰ���������
	const camera_t& GetCamera() const { return m_Camera; }
	// �����һ֡�����������
	const camera_t& GetPrevCamera() const { return m_PrevCamera; }
	// ������һ֡�����������
	void SavePrevCamera() {	m_PrevCamera = m_Camera; }

	// ��ü�����
	const FmPlane* GetClipPlanes() const { return m_ClipPlane; }
	// ��ü���������
	int GetClipPlaneAmount() const { return MAX_CLIP_PLANE; }

	// ����Ӿ�
	void SetFarClipDistance(float value) { m_fFarClipDistance = value; }
	float GetFarClipDistance() const { return m_fFarClipDistance; }

	// ��Χ���Ƿ񳬹�����Ӿ�
	bool IsFarClip(const FmVec3& pos, float radius)
	{
		const FmVec3& cam_pos = m_Camera.vPosition;
		float sx = pos.x - cam_pos.x;
		float sy = pos.y - cam_pos.y;
		float sz = pos.z - cam_pos.z;
		float dist = m_fFarClipDistance + radius;

		return (sx * sx + sy * sy + sz * sz) > (dist * dist);
	}

	// ��Χ���Ƿ񳬹����ˮƽ�Ӿ�
	bool IsFarClip2D(const FmVec3& pos, float radius)
	{
		const FmVec3& cam_pos = m_Camera.vPosition;
		float sx = pos.x - cam_pos.x;
		float sz = pos.z - cam_pos.z;
		float dist = m_fFarClipDistance + radius;

		return (sx * sx + sz * sz) > (dist * dist);
	}

	// ��Χ���Ƿ��ڼ�������
	bool IsViewClip(const FmVec3& pos, float radius)
	{
		for (size_t k = 0; k < MAX_CLIP_PLANE; ++k)
		{
			float len = FmPlaneDotCoord(&m_ClipPlane[k], &pos);

			if (len > radius)	
			{
				return true;
			}
		}

		return false;
	}

	// �Ƿ�Ҫ�������Ԥ��Ⱦ
	virtual void SetEnableEarlyZ(bool value);
	virtual bool GetEnableEarlyZ() const;

	virtual void SetShadowAdjustingRect(FmVec4 vAdjustingRect);
	virtual const FmVec4& GetShadowAdjustingRect();

private:
	// �����ȾĿ��ߴ�
	int GetRenderWidth() const;
	int GetRenderHeight() const;

	// �������ͷ���ȾĿ��
	bool CreateBlendRT();
	bool ReleaseBlendRT();

	bool CreateRefractionRT();
	bool ReleaseRefractionRT();

	bool CreateCopyTempColorRT();
	bool ReleaseCopyTempColorRT();

	bool CreateCopyTempDepth();
	bool ReleaseCopyTempDepth();

	bool CreateShadowMapRT();
	bool ReleaseShadowMapRT();

	bool CreateTempColorRT();
	bool ReleaseTempColorRT();

	bool CreateTempDepthRT();
	bool ReleaseTempDepthRT();

	bool CreateTempDepthTex();
	bool ReleaseTempDepthTex();

	bool CreateTempDepthStencilRT();
	bool ReleaseTempDepthStencilRT();

	bool CreateTempStencilTex();
	bool ReleaseTempStencilTex();

	// �ͷŲ�
	void ReleaseLayer();
	// ���ղ��еĶ���
	void CollectLayer();
	// ���Ҳ��еĶ���
	bool FindLayerIndex(const char* name, size_t& index) const;

	// �����׶����
	void GetViewFrustumPointList(FmMat4 mtxView, float fFOV, float fAspect, float fNear, float fFar, FmVec3* vFrustum);
	// ������ĵ�
	void GetPointListCenter(unsigned int nPointCount, const FmVec3* pPointList, FmVec3* center);

private:
	Render* m_pRender;
	bool m_bEnableEarlyZ;
	int m_nViewWidth;
	int m_nViewHeight;
	int m_nBlendDownLevel;
	bool m_bEnableClearColorOptimize;
	bool m_bEnableRealizeRefraction;
	bool m_bEnableRealizeCopyTempColorRT;
	bool m_bEnableRealizeCopyTempDepthRT;
	bool m_bEnableRealizeTempColorRT;
	bool m_bEnableRealizeTempDepthRT;
	bool m_bEnableRealizeTempDepthTex;
	bool m_bEnableRealizeTempDephtStencilRT;
	bool m_bEnableRealizeTempStencilTex;
	IColorRT* m_pBlendColorRT;
	IDepthRT* m_pBlendDepthRT;
	IColorRT* m_pRefractionRT;
	IColorRT* m_pRealizeCopyColorTempRT;
	IColorRT* m_pRealizeCopyTempDepthRT;
	IColorRT* m_pRealizeTempColorRT;
	IDepthRT* m_pRealizeTempDepthRT;
	IDepthTex* m_pRealizeTempDepthTex;
	IDepthStencilRT* m_pRealizeTempDepthStencilRT;
	IStencilTex* m_pRealizeTempStencilTex;

	float     m_fRealizeTempRTScale;

	IColorRT* m_pDebugColorRT;
	IDepthRT* m_pDebugDepthRT;
	ITexture* m_pDebugTex;
	IDepthStencilRT* m_pDebugDepthStencilRT;
	IColorRT* m_pDecodeColorRT;

	core_string m_strGlobalEnvMap;
	ITexture* m_pGlobalEnvMap;
 
	bool m_bEnableDynamicShadow;
	bool m_bPercentageCloserFilter;
	bool m_bVarianceShadowMap;
	float m_fLogSplitWeight;
	unsigned int m_nShadowMapSize;
	int m_nShadowMapCount;
	int m_nShadowMapIndex;
	float m_fCameraDistance;
	float m_fShadowExpandPercent;
	bool m_bShadowMapReusable;
	FmVec3 m_vShadowLightDir;
	FmMat4 m_mtxShadowMapView[MAX_SHADOWMAP_NUM];
	FmMat4 m_mtxShadowMapProj[MAX_SHADOWMAP_NUM];
	FmVec2 m_vShadowOrthoMin[MAX_SHADOWMAP_NUM];
	FmVec2 m_vShadowOrthoMax[MAX_SHADOWMAP_NUM];
	FmVec3 m_vShadowOrthoCenter[MAX_SHADOWMAP_NUM];
	int m_nShadowMapNoFlush[MAX_SHADOWMAP_NUM];
	float m_fDepthSlope[MAX_SHADOWMAP_NUM];
	float m_fDepthBias[MAX_SHADOWMAP_NUM];
	IColorRT* m_pShadowMapRT[MAX_SHADOWMAP_NUM];
	IDepthTex* m_pShadowMapDS[MAX_SHADOWMAP_NUM];
	IColorRT* m_pShadowRT;

	TArrayPod<layer_data_t*, 1, TContextAlloc> m_Layers;
	TStringPod<char, PERSISTID, TStringTraits<char>, 
		TContextAlloc> m_Uniques;
	int m_IntValues[MAX_INT_NUM];
	float m_FloatValues[MAX_FLOAT_NUM];
	unsigned int m_ColorValues[MAX_COLOR_NUM];
	FmVec4 m_VectorValues[MAX_VECTOR_NUM];
	FmVec3 m_vReferPosition;
	camera_t m_Camera;
	camera_t m_PrevCamera;
	float m_fFarClipDistance;
	FmPlane m_ClipPlane[MAX_CLIP_PLANE];

	FmVec4 m_vAdjustingRect;
};

#endif // _CONTEXT_H


/*
��Ⱦ������
����Ⱦ��صĶ���ͳһ���д浵

����˵��Ҫ�򿪺��������Ƚ�SetEnableRealizeTempRT ����


*/