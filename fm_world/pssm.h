//--------------------------------------------------------------------
// �ļ���:		pssm.h
// ��  ��:		
// ˵  ��:		
// ��������:	 
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PSSM_H
#define	_PSSM_H

#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"

class IRender;
class ISceneView;

// ƽ���з���Ӱͼʵʱ��ӰЧ��
class PSSM : public IVisBase
{
public:

	struct object_t
	{
		PERSISTID ObjectId;
		int nPrior;
		object_t* pNext;
	};

	struct pssm_stencil_shaderhandle
	{
		pssm_stencil_shaderhandle()
		{
			pStencilShader = NULL;
			pFilterShader = NULL;
		}

		IShaderProgram* pStencilShader;
		IShaderProgram* pFilterShader;

		fm_int hTexDepthMap;
		fm_int hTexShadowMap;

		fm_int hMtxLightVP;
		fm_int hDepthBiasAndSlope;
		fm_int hPixelSize;
		fm_int hShadowParam;
		fm_int hMtxProjInverse;
		fm_int hMtxWVP;

		fm_int hShadowMaxZ;

		float fShadowMaxZ;

		FmVec4 vDepthBiasAndSlope;
		FmVec4 vShadowParam;
		FmVec4 vPixelSize;

		FmMat4 matWVP;
		FmMat4 matProjInv;
		FmMat4 matLightUp;
	};

public:

	PSSM();
	virtual ~PSSM();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	virtual bool Load();
	virtual void Realize();

	// ������Ӱͼ
	void GenerateShadowMap();

	// ��Ⱦ�������
	void SetRenderObjets(void* pObjects)
	{
		m_pObjects = (object_t*)pObjects;
	}

	//����ȫ����Ӱͼ
	static void DrawShadowPostBatch(void* pData);

private:

	IRender* m_pRender;
	object_t* m_pObjects;
	pssm_stencil_shaderhandle* m_pStencilShaderHandle;

	bool m_bVisible;
	bool m_bEnablePCF;
	bool m_bEnableVSM;
	bool m_bShadowMapReusable;

	float m_fLogSplitWeight;
	float m_fShadowExpandPercent;
	float m_fShadowInten;

	unsigned int m_nShadowMapCount;
	int m_nShadowMapIndex;

	FmVec3 m_vShadowLightDir;

	FmVec2 m_vShadowOrthoMin[IRenderContext::MAX_SHADOWMAP_NUM];
	FmVec2 m_vShadowOrthoMax[IRenderContext::MAX_SHADOWMAP_NUM];
	FmVec3 m_vShadowOrthoCenter[IRenderContext::MAX_SHADOWMAP_NUM];

	ISceneView* m_pShadowSceneView[IRenderContext::MAX_SHADOWMAP_NUM];

	float m_fDepthBias[IRenderContext::MAX_SHADOWMAP_NUM];
	float m_fDepthSlope[IRenderContext::MAX_SHADOWMAP_NUM];
};

#endif // _PSSM_H
