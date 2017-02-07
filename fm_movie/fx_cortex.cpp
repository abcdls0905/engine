//--------------------------------------------------------------------
// �ļ���:		FxCortex.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��3��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "fx_cortex.h"
#include "../public/core_log.h"
#pragma comment(lib, "Cortex_SDK.lib")
#include "fxmathinl.h"
#include "../visual/i_dx_dynamic.h"
#include "actor_2.h"
#include "../visual/i_shader_system_2.h"
#include "../visual/dx_scene_batch.h"

/// \file FxCortex.cpp
/// \brief Cortexͨ��

/// entity: FxCortex
/// \brief Cortexͨ��
/// \see �̳���IVisual
DECLARE_ENTITY("", FxCortex, IVisBase);

/// property: object ActorBind
/// \brief �󶨵Ķ�������
DECLARE_PROPERTY(PERSISTID, FxCortex, ActorBind, GetActorBind, SetActorBind);
/// method: bool ConnectCortex(char* szMyIP, char* szCortexIP);
/// \brief ����Cortex
/// \param szMyIP ����IP
/// \param szCortexIP Cortex��IP
/// \return �����Ƿ�ɹ�
DECLARE_METHOD_2(bool, FxCortex, ConnectCortex, const char*, const char*);
/// method: bool GetConnected() const;
/// \brief �Ƿ��Ѿ���Cortex����
/// \return �Ƿ��Ѿ���Cortex����
DECLARE_METHOD_0(bool, FxCortex, GetConnected);
/// method: bool GetBodyDefs();
/// \brief ��Cortex��ø����岿����������Ϣ
/// \return �Ƿ�ɹ�
DECLARE_METHOD_0(bool, FxCortex, GetBodyDefs);
/// property: bool ReceiveCortexFrame
/// \brief �Ƿ����Cortex��֡��Ϣ
DECLARE_PROPERTY(bool, FxCortex, ReceiveCortexFrame, GetReceiveCortexFrame, SetReceiveCortexFrame);
/// method: bool GetFrameOfData();
/// \brief ��cortex��ȡ��ǰ֡����
/// \return �Ƿ�ɹ�
DECLARE_METHOD_0(bool, FxCortex, GetFrameOfData);
/// property: std::string MarkerColor
/// \brief marker�����ɫ
DECLARE_PROPERTY(std::string, FxCortex, MarkerColor, GetMarkerColorString, SetMarkerColorString);

/// method: bool CalcTposeTransformMatrix();
/// \brief �����cortex��ģ��tpose�任����Ϸģ��tpose�ı任����, ������cortex��ģ�ͺ���Ϸ���ģ�Ͷ�����tpose״̬ʱ���ܵ���
/// \return �Ƿ�ɹ�
DECLARE_METHOD_0(bool, FxCortex, CalcTposeTransformMatrix);

/// method: bool SetBoneMap(const char* szCortexBoneName, const char* szActorBoneName)
/// \brief ����cortexģ�ͺ�actor�Ĺ�����Ӧ��ϵ
/// \param szCortexBoneName cortex������
/// \param szActorBoneName actor������
/// \return �Ƿ�ɹ�
DECLARE_METHOD_2(bool, FxCortex, SetBoneMap, const char*, const char*);

/// method: void ClearBoneMap()
/// \brief 
DECLARE_METHOD_VOID_0(FxCortex, ClearBoneMap);


// ���ȫ�ֱ�����Ϊ����cortex�Ļص����Է���FxCortexʵ��
static FxCortex* s_pFxCortex = NULL;

FmMat4 g_identityMatrix;

FxCortex::FxCortex() 
{
	FmMatrixIdentity(&g_identityMatrix);

	m_bConnected = false;
	memset(&m_CortexHostInfo, 0, sizeof(m_CortexHostInfo));
	m_pBodyDefs = NULL;
	m_pPrevFrame = NULL;
	m_pNextFrame = NULL;
	m_fSecondsFromPrevFrame = 0;
	m_fSecondsBetweenTwoFrames = 0;
	m_nMarkers = 0;
	m_pMarkerDynamicVB = NULL;
	m_bReceiveCortexFrame = false;
	m_bInterpolation = false;
	m_nMarkerColor = 0xffffffff;
	m_nShader = -1;
}

FxCortex::~FxCortex() 
{
	ReleaseResource(true);
}

void FxCortex::CortexErrorMsgHandler(int iLevel, char *szMsg)
{
	char *szLevel;

	if (VL_Debug == iLevel)
	{
		szLevel = "Debug";
	}
	else if (VL_Info == iLevel)
	{
		szLevel = "Info";
	}
	else if (VL_Warning == iLevel)
	{
		szLevel = "Warning";
	}
	else if (VL_Error == iLevel)
	{
		szLevel = "Error";
	}

	CORE_TRACE("CortexMsg:");
	CORE_TRACE(szLevel);
	CORE_TRACE(szMsg);
}

void FxCortex::CortexFrameDataHandler(sFrameOfData* pFrameOfData)
{
	if (NULL == s_pFxCortex)
	{
		Assert(0);
		CORE_TRACE("(CortexFrameDataHandler) s_pFxCortex is null.");
		return;
	}

	// ������ڲ�����cortex��֡��Ϣ
	if (!s_pFxCortex->GetReceiveCortexFrame())
	{
		// ֱ�Ӻ������֡
		return;
	}

	// �ӿ���֡������ȡ��֡������
	sFxFrame* pNewFrame = s_pFxCortex->PopFromFreeList();
	if (NULL == pNewFrame)
	{
		CORE_TRACE("(CortexFrameDataHandler) Create new frame failed.");
		return;
	}

	int retval = Cortex_CopyFrame(pFrameOfData, &pNewFrame->cortex_frame);
	if (retval != RC_Okay)
	{
		CORE_TRACE("(CortexFrameDataHandler) Copy frame failed.");
		return;
	}

	// ����ȴ���Ⱦ��֡����
	s_pFxCortex->PushToWaitList(pNewFrame);
}

bool FxCortex::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}

	// ֻ�������һ��ʵ��
	IEntity* pEntity = GetCore()->LookupEntity("FxCortex");
	if (pEntity != this)
	{
		CORE_TRACE("(FxCortex::Init) FxCortex already exists");
		return false;
	}

	IShaderManager* pShaderManager = m_pRender->GetShaderManager();

	m_nShader = pShaderManager->LoadShader("FxCortex.c",
		"vs_main", "ps_main", "");
	if (-1 == m_nShader)
	{
		CORE_TRACE("(FxCortex::Init) Load shader failed.");
		return false;
	}

	// ����Ѿ�����һ�������ʵ������s_pFxCortex��ֵ��
	// ���ͷ�ǰһ��ʵ��
	// ��Ӧ�ó����������
	if (s_pFxCortex)
	{
		Assert(0);
		SAFE_RELEASE(s_pFxCortex);
	}

	s_pFxCortex = this;

	// ���ô�cortex���յ���Ϣ�ȼ�
	Cortex_SetVerbosityLevel(VL_Info);

	// cortex sdk�İ汾��
	unsigned char szCortexSDKVersion[4];
	Cortex_GetSdkVersion(szCortexSDKVersion);
	// ��ӡ sdk version TODO

	// ���ô�cortex������Ϣ�Ļص�����
	// �ӵ��Ĳ�ֹ�д�����Ϣ��������һЩ��ʾ��Ϣ��
	if (Cortex_SetErrorMsgHandlerFunc(CortexErrorMsgHandler) != RC_Okay)
	{
		CORE_TRACE("(FxCortex::Init) Cortex_SetErrorMsgHandlerFunc failed.");
	}

	// ���ô�cortex����֡���ݵĻص�����
	if (Cortex_SetDataHandlerFunc(CortexFrameDataHandler) != RC_Okay)
	{
		CORE_TRACE("(FxCortex::Init) Cortex_SetDataHandlerFunc failed.");
	}

	// Ԥ�ȴ���һЩ����֡
	AllocFreeFrame(10);

	return true;
}

bool FxCortex::Shut()
{
	// ��֪���������FxCortex����ʱ�����Ƿ�������⣬��Ҫ����һ�� liujie

	// ���´��벻�ܷŵ���������򴴽����FxCortex����ʱ���ظ�ִ��
	s_pFxCortex = NULL;
	Cortex_Exit();

	return IVisual::Shut();
}

void FxCortex::ReleaseResource(bool bReleaseMemPool)
{
	// ֹͣ�����µ�cortex����
	SetReceiveCortexFrame(false);

	CAutoLock autolock(m_Lock);

	// body�Ķ�����Ϣ
	if (m_pBodyDefs)
	{
		Cortex_FreeBodyDefs(m_pBodyDefs);
		m_pBodyDefs = NULL;
	}

	// �ͷŵ�ǰ������Ⱦ�������ؼ�֡
	if (m_pPrevFrame)
	{
		Cortex_FreeFrame(&m_pPrevFrame->cortex_frame);
		// �������Ҫ�ͷ��ڴ��
		if (!bReleaseMemPool)
		{
			// ��֡���ݷ�����ж��л�������
			PushToFreeList(m_pPrevFrame);
		}
		m_pPrevFrame = NULL;
	}

	if (m_pNextFrame)
	{
		Cortex_FreeFrame(&m_pNextFrame->cortex_frame);
		// �������Ҫ�ͷ��ڴ��
		if (!bReleaseMemPool)
		{
			// ��֡���ݷ�����ж��л�������
			PushToFreeList(m_pNextFrame);
		}
		m_pNextFrame = NULL;
	}

	// �ͷŵȴ���Ⱦ��֡����
	FrameList::iterator ite = m_WaitRenderFrameData.begin();
	for (; ite != m_WaitRenderFrameData.end(); ++ite)
	{
		sFxFrame* pFrame = *ite;
		Cortex_FreeFrame(&pFrame->cortex_frame);

		// �������Ҫ�ͷ��ڴ��
		if (!bReleaseMemPool)
		{
			// ��֡���ݷ�����ж��л�������
			PushToFreeList(pFrame);
		}
	}
	m_WaitRenderFrameData.clear();

	// �Ƿ��ͷ��ڴ��
	if (bReleaseMemPool)
	{
		// ��տ��ж���
		m_FreeFrameData.clear();

		// �ͷ������ڴ�
		size_t nBufferSize = m_Buffers.size();
		for (size_t i = 0; i < nBufferSize; ++i)
		{
			CORE_FREE(m_Buffers[i]);
		}
		m_Buffers.clear();
	}

	m_fSecondsFromPrevFrame = 0.0f;
	m_fSecondsBetweenTwoFrames = 0.0f;

	// �ͷű��mark
	m_nMarkers = 0;
	SAFE_RELEASE(m_pMarkerDynamicVB);

	// �ͷ�body����
	m_bodys.clear();
}

bool FxCortex::GetConnected() const
{
	return m_bConnected;
}

// ��������ĺ���ת���� liujie
// ���ҵ���cortex������Ϊ��λ������ʱ����������Ͳ���Ҫ��
float g_fMeterFactor = 0.001f;
// �Ƕ�ת���ȵ�ϵ��
float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

void FxCortex::Update(float seconds)
{
	if (!GetConnected())
	{
		return;
	}

	// û����Ϸ����Ͳ�����
	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	m_fSecondsFromPrevFrame += seconds;

	// ���������û��ʼ������֡�ڵȴ���Ⱦ
	if (NULL == m_pPrevFrame && !m_WaitRenderFrameData.empty())
	{
		// ��һ���ؼ�֡����Ϊ��
		Assert(NULL == m_pNextFrame);
		if (m_pNextFrame)
		{
			PushToFreeList(m_pNextFrame);
			m_pNextFrame = NULL;
		}

		m_pPrevFrame = PopFromWaitList();

		// ���¼�ʱ
		m_fSecondsFromPrevFrame = 0;
		m_fSecondsBetweenTwoFrames = 0; // liujie
	}

	// �����û��һ֡������֡�ڵȴ���Ⱦ
	if (NULL == m_pNextFrame && !m_WaitRenderFrameData.empty())
	{
		Assert(m_pPrevFrame != NULL);
		m_pNextFrame = PopFromWaitList();

		// ���¼�ʱ
		m_fSecondsFromPrevFrame = 0;
		m_fSecondsBetweenTwoFrames = 0; // liujie
	}

	// ������һ֡����ʱ�䵽��򳬹���һ���ؼ�֡ʱ
	if (m_pNextFrame && m_fSecondsFromPrevFrame >= m_fSecondsBetweenTwoFrames)
	{
		// �ͷ�ǰһ���ؼ�֡
		PushToFreeList(m_pPrevFrame);
		m_pPrevFrame = NULL;

		// ��һ���ؼ�֡���ǰһ���ؼ�֡
		m_pPrevFrame = m_pNextFrame;
		m_pNextFrame = NULL;

		// ����еȴ���Ⱦ��֡
		if (!m_WaitRenderFrameData.empty())
		{
			// �����һ֡
			m_pNextFrame = PopFromWaitList();
			// ���¼�ʱ
			m_fSecondsFromPrevFrame = 0;
			m_fSecondsBetweenTwoFrames = 0; // liujie
		}
	}

	// ���������Ƿ��и���
	bool bBoneUpdate = false;

	// ��Ҫ��ֵ������һ֡�Ͳ�ֵ
	if (GetInterpolation() && m_pNextFrame)
	{
		//bBoneUpdate = true;
	}
	// ֱ����ʾǰһ֡
	else
	{
		// ���ǰһ֡��û��Ⱦ�������marker�͹�������
		if (!m_pPrevFrame->m_bRender)
		{
			bBoneUpdate = true;

			// ������������õ�����ʱ����
			FmMat4 mat_tmp;
			float px, py, pz, rx, ry, rz;

			sFxMarkerVertex* pMarkers = (sFxMarkerVertex*)m_pMarkerDynamicVB->Lock();

			//int b = 0;
			int m = 0;
			for (int i = 0; i < m_pPrevFrame->cortex_frame.nBodies; ++i)
			{
				sFxBody& fxbody = m_bodys[i];

				sBodyData& body = m_pPrevFrame->cortex_frame.BodyData[i];
				// marker
				for (int _m = 0; _m < body.nMarkers; ++_m)
				{
					tMarkerData& marker = body.Markers[_m];
					// cortex����������ϵ
					//pMarkers[m].pos.x = -marker[0] * g_fMeterFactor;
					// ����������ϵ����
					pMarkers[m].pos.x = -marker[0] * g_fMeterFactor;
					pMarkers[m].pos.y = marker[1] * g_fMeterFactor;
					pMarkers[m].pos.z = marker[2] * g_fMeterFactor;
					pMarkers[m].diffuse = m_nMarkerColor;
					++m;
				}

				// ����
				BONES_ARRAY& bodyArray = fxbody.bonesArray;
				for (int _b = 0; _b < body.nSegments; ++_b)
				{
					sFxBone& bone = bodyArray[_b];
					FmMat4& mat = bone.mat;
					FmMatrixIdentity(&mat);

					tSegmentData& segment = body.Segments[_b];
					// ��λ��
					// cortex����������ϵ
					px = -(float)segment[0] * g_fMeterFactor;
					py = (float)segment[1] * g_fMeterFactor;
					pz = (float)segment[2] * g_fMeterFactor;

					D3DXMatrixTranslation(&mat, px, py, pz);

					// ��������

					// ����ת
					// cortex����������ϵ
					rx = -(float)segment[3] * g_fRadianFactor;
					ry = -(float)segment[4] * g_fRadianFactor;
					rz = (float)segment[5] * g_fRadianFactor;

					// ��ת˳��: rx-ry-rz
					D3DXMatrixRotationX(&mat_tmp, rx);
					D3DXMatrixMultiply(&mat, &mat_tmp, &mat);
					FmMatrixRotationY(&mat_tmp, ry);
					D3DXMatrixMultiply(&mat, &mat_tmp, &mat);
					D3DXMatrixRotationZ(&mat_tmp, rz);
					D3DXMatrixMultiply(&mat, &mat_tmp, &mat);

//#ifdef _DEBUG
//					bone.debug_vTranslation.x = px;
//					bone.debug_vTranslation.y = py;
//					bone.debug_vTranslation.z = pz;
//					bone.debug_local_mat = mat;
//#endif

					//++b;
				}
			}

			m_pMarkerDynamicVB->Unlock();
		}
	}

	// ǰһ֡�Ѿ���ʼ����Ⱦ��
	m_pPrevFrame->m_bRender = true;

	if (bBoneUpdate)
	{
#ifdef _DEBUG
		FmVec3 debug_vOutScale, debug_vOutTranslation;
		FmQuat debug_QOutRotate;
		FmVec3 debug_vAxis;
		float debug_fAngle;
#endif
		// �����������
		for (size_t i = 0; i < m_bodys.size(); ++i)
		{
			BONES_ARRAY& bones= m_bodys[i].bonesArray;
			for (size_t _b = 0; _b < bones.size(); ++_b)
			{
				sFxBone& bone = bones[_b];
				if (bone.nParentBoneIndex > -1)
				{
					// ���ӹ���������任����
					D3DXMatrixMultiply(&bone.mat, &bone.mat, &bones[bone.nParentBoneIndex].mat);
//#ifdef _DEBUG
//					// ����任
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bone.mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//
//					// �ֲ��任
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bone.debug_local_mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//
//					// ������
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bones[bone.nParentBoneIndex].mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//#endif
				}
			}
		}
	}

	// ������Ϸ�����Ĺ�������
	if (pActor)
	{
		size_t i = 0;
		// actor�и�����������任����
		FmMat4 mtxParentBoneWorldMatrixInActor;
		// �����任����
		FmMat4 mtxBoneTMtmp;
		float px, py, pz;
		// 
		for (size_t b = 0; b < m_bonesMap.size(); ++b)
		{
			sBoneMap& boneMap = m_bonesMap[b];

			// ���actor�и�����������任����
			if (!pActor->GetBoneMatrix(boneMap.strActorBoneParentName.c_str(), 
				&mtxParentBoneWorldMatrixInActor))
			{
				FmMatrixIdentity(&mtxParentBoneWorldMatrixInActor);
			}

			// ����actor�и�����������任���������ù���������任����
			D3DXMatrixMultiply(&mtxBoneTMtmp, &boneMap.mtxActorBoneLocalMatrix, 
				&mtxParentBoneWorldMatrixInActor);

			// ��������׽ģ�Ϳ��ƵĹ�����Ҫ���óɶ�����׽�Ĺ�������
			if (boneMap.strCaptureBoneName != "")
			{
				sFxBone& bone = 
					m_bodys[boneMap.nCaptureBodyIndex].bonesArray[boneMap.nCaptureBoneIndex];

				// ��������ʹ���������Ľ��������ʹ�øù����ڶ�����׽�л�õ�����
				px = mtxBoneTMtmp._41;
				py = mtxBoneTMtmp._42;
				pz = mtxBoneTMtmp._43;
				D3DXMatrixMultiply(&mtxBoneTMtmp, &boneMap.mtxTposeCaptureToActor, &bone.mat);
				mtxBoneTMtmp._41 = px;
				mtxBoneTMtmp._42 = py;
				mtxBoneTMtmp._43 = pz;
			}

			pActor->SetBoneMatrix(boneMap.strActorBoneName.c_str(), &mtxBoneTMtmp);
		}

		//for (size_t i = 0; i < m_bodys.size(); ++i)
		//{
		//	BONES_ARRAY& bones = m_bodys[i].bonesArray;
		//	for (unsigned int _b = 0; _b < bones.size(); ++_b)
		//	{
		//		sFxBone& bone = bones[_b];

		//		// actor���ж�Ӧ�Ĺ���
		//		if (bone.strActorBoneName != "" &&
		//			pActor->NodeIsExist(bone.strActorBoneName.c_str()))
		//		{
		//			// ���actor�и�����������任����
		//			if (bone.nParentBoneIndex > -1)
		//			{
		//				sFxBone& parentBone = bones[bone.nParentBoneIndex];

		//				// ���actor�и�����������任����
		//				if (!pActor->GetBoneMatrix(parentBone.strActorBoneName.c_str(), 
		//					&mtxParentBoneTMInActor))
		//				{
		//					FmMatrixIdentity(&mtxParentBoneTMInActor);
		//				}
		//			}
		//			else
		//			{
		//				FmMatrixIdentity(&mtxParentBoneTMInActor);
		//			}

		//			// ����actor�и�����������任���������ù���������任����
		//			D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matActorLocalMatrix, 
		//				&mtxParentBoneTMInActor);

		//			// ��������ʹ���������Ľ��������ʹ�øù����ڶ�����׽�л�õ�����
		//			px = mtxBoneTMtmp._41;
		//			py = mtxBoneTMtmp._42;
		//			pz = mtxBoneTMtmp._43;
		//			D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matTposeCortexToActor, &bone.mat);
		//			mtxBoneTMtmp._41 = px;
		//			mtxBoneTMtmp._42 = py;
		//			mtxBoneTMtmp._43 = pz;

		//			pActor->SetBoneMatrix(bone.strActorBoneName.c_str(), &mtxBoneTMtmp);
		//		}
		//	}
		//}

		pActor->UpdateBoneMatrix();
	}
}

void FxCortex::Realize(float offset_seconds)
{
	if (!GetConnected())
	{
		return;
	}

	if (m_nShader < 0)
	{
		return;
	}

	IShaderManager* pShaderManager = m_pRender->GetShaderManager();
	if (NULL == pShaderManager)
	{
		return;
	}

	// û����Ϸ����Ͳ�����
	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	// ������û��ʼ
	if (NULL == m_pPrevFrame)
	{
		return;
	}

	// ����marker
	DxSceneBatch* pBatch = m_pRender->AddBlendBatch("DrawMarkers");
	if (NULL == pBatch)
	{
		return;
	}

	pBatch->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pBatch->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pBatch->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	IDxContext* pContext = m_pRender->GetContext();

	FmMat4 mtxWorld;
	pActor->GetWorldMatrix(mtxWorld);
	FmMat4 mtxWVP;
	D3DXMatrixMultiplyTranspose(&mtxWVP, &mtxWorld, &pContext->GetCamera().mtxViewProj);
	pBatch->SetVertexShaderConstantF(0, (const float*)&mtxWVP, 4);

	pBatch->SetShader(pShaderManager->GetShader(m_nShader));
	pBatch->SetFVF(MARKER_FVF);
	pBatch->SetStreamSource(0, m_pMarkerDynamicVB->GetDxVertexBuffer(), 0, sizeof(sFxMarkerVertex));
	pBatch->SetPrimitive(D3DPT_POINTLIST, m_nMarkers, m_nMarkers);
}

// ����Cortex
bool FxCortex::ConnectCortex(const char* szMyIP, const char* szCortexIP)
{
	// ֹͣ�����µ�cortex����
	SetReceiveCortexFrame(false);

	// �ͷ���ǰ����Դ
	ReleaseResource(false);

	// cortex��ͨ����ʼ���������ӵģ�
	// ֻҪ֮ǰ��Cortex_Initialize�������Թ����ӣ������Ƿ����ӳɹ�
	// �����ʼ��cortex sdk��
	// ���֮���ٵ���Cortex_Initialize���᷵�ش�����Ϊ�Ѿ���ʼ��
	// ���������ȹر�cortex�ٵ���Cortex_Initialize
	Cortex_Exit();
	m_bConnected = false;

	// ����ip
	m_strCortexIP = "";

	if (NULL == szMyIP || NULL == szCortexIP)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) IP is NULL.");
		return false;
	}

	// ��ʼ��������cortex
	int retval = Cortex_Initialize((char*)szMyIP, (char*)szCortexIP);
	if (RC_Okay != retval)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) Initialize Cortex failed");
		CORE_TRACE(szMyIP);
		CORE_TRACE(szCortexIP);
		return false;
	}

	// ��ѯ���ӵĽ��
	retval = Cortex_GetHostInfo(&m_CortexHostInfo);
	if (RC_Okay != retval || !m_CortexHostInfo.bFoundHost)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) Connect Cortex failed");
		CORE_TRACE(szMyIP);
		CORE_TRACE(szCortexIP);
		return false;
	}

	m_strCortexIP = szCortexIP;
	m_bConnected = true;

	return true;
}

void FxCortex::SetActorBind(const PERSISTID& actor)
{
	m_ActorBind = actor;

	// ��ʼ�����к�actor��ص�����

	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	// �ֶ���һ�λ�ϣ�ȷ��actor��Ĺ��������ǳ�ʼ������
	pActor->BlendByHand();

	// ���actor�Ĺ����б�
	CVarList actorBoneList;
	pActor->GetBoneNodeList(CVarList(), actorBoneList);

	// ��ʼ��actor�����Ͷ�����׽�����Ķ�Ӧ��ϵ��
	m_bonesMap.resize(actorBoneList.GetCount());
	for (size_t b = 0; b < m_bonesMap.size(); ++b)
	{
		sBoneMap& boneMap = m_bonesMap[b];
		boneMap.strActorBoneName = actorBoneList.StringVal(b);

		boneMap.strActorBoneParentName = 
			pActor->GetBoneParentName(boneMap.strActorBoneName.c_str());

		if (!pActor->GetBoneLocalMatrix(boneMap.strActorBoneName.c_str(), 
			&boneMap.mtxActorBoneLocalMatrix))
		{
			Assert(0);
		}
	}
}

PERSISTID FxCortex::GetActorBind() const
{
	return m_ActorBind;
}

FxCortex::sFxFrame* FxCortex::PopFromWaitList()
{
	CAutoLock autolock(m_Lock);
	if (m_WaitRenderFrameData.empty())
	{
		return NULL;
	}
	sFxFrame* pFrame = m_WaitRenderFrameData.front();
	m_WaitRenderFrameData.pop_front();
	return pFrame;
}

void FxCortex::PushToWaitList(sFxFrame* pFrame)
{
	CAutoLock autolock(m_Lock);
	m_WaitRenderFrameData.push_back(pFrame);
}

#define MIN_ALLOC_FRAME_COUNT 5
FxCortex::sFxFrame* FxCortex::PopFromFreeList()
{
	CAutoLock autolock(m_Lock);
	// ���ж���Ϊ��
	if (m_FreeFrameData.empty())
	{
		// ������֡
		//const int nAddFrameCount = m_WaitRenderFrameData.size() * 2 + MIN_ALLOC_FRAME_COUNT;
		const int nAddFrameCount = MIN_ALLOC_FRAME_COUNT;
		AllocFreeFrame(nAddFrameCount);

		if (m_FreeFrameData.empty())
		{
			Assert(0);
			return NULL;
		}

		return PopFromFreeList();
	}

	// ֱ�Ӵӿ��ж�����ȡ��һ֡
	sFxFrame* pFrame = m_FreeFrameData.front();
	m_FreeFrameData.pop_front();

	// ����
	pFrame->m_bRender = false;
	return pFrame;
}

void FxCortex::PushToFreeList(sFxFrame* pFrame)
{
	CAutoLock autolock(m_Lock);
	if (pFrame)
	{
		// ����
		pFrame->m_bRender = false;
		// �����ȵ���Cortex���ͷ�
		Cortex_FreeFrame(&pFrame->cortex_frame);
		m_FreeFrameData.push_back(pFrame);
	}
}

void FxCortex::AllocFreeFrame(unsigned int nAddFrameCount)
{
	CAutoLock autolock(m_Lock);

	const int nBufferSize = sizeof(sFxFrame) * nAddFrameCount;
	char* pBuffer = (char*)CORE_ALLOC(char, nBufferSize);
	if (NULL == pBuffer)
	{
		Assert(0);
		return;
	}

	// ����
	// ��ֹ�����ط�������δ��ʼ������Ч����
	// ��ֹ��δ��ʼ�������ݵ���Cortex_FreeFrameʱ���ܳ��ִ���
	memset(pBuffer, 0, nBufferSize);

	// �����ڴ��
	m_Buffers.push_back(pBuffer);

#ifdef _DEBUG
	m_vDebugAllocTime.push_back(nAddFrameCount);
#endif

	sFxFrame* frame_array = (sFxFrame*)pBuffer;

	// ������ж���
	for (unsigned int i = 0; i < nAddFrameCount; ++i)
	{
		PushToFreeList(frame_array + i);
	}
}

bool FxCortex::GetBodyDefs()
{
	// ��ִ�б�����ʱ���ܽ����µ�cortex����
	bool bReceiveOld = GetReceiveCortexFrame();
	SetReceiveCortexFrame(false);

	// ���»��BodyDefs��������ǰ��������������
	// �ͷ���ǰ������
	ReleaseResource(false);

	m_pBodyDefs = Cortex_GetBodyDefs();
	if (NULL == m_pBodyDefs)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) Get body defs failed.");
		return false;
	}

	if (0 == m_pBodyDefs->nBodyDefs)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) No body.");
		return false;
	}

	// ����bodys
	m_bodys.resize(m_pBodyDefs->nBodyDefs);

	// ��ʼ��marker����
	m_nMarkers = 0;
	int i;
	for (i = 0; i < m_pBodyDefs->nBodyDefs; ++i)
	{
		m_bodys[i].nMarkerOffset = m_nMarkers;
		m_bodys[i].nMarkerCount = m_pBodyDefs->BodyDefs[i].nMarkers;
		m_nMarkers += m_pBodyDefs->BodyDefs[i].nMarkers;
		//m_nBones += m_pBodyDefs->BodyDefs[i].Hierarchy.nSegments;
	}

	if (m_nMarkers > 0)
	{
		m_pMarkerDynamicVB = m_pRender->CreateDynamicVB(
			m_nMarkers * sizeof(sFxMarkerVertex), MARKER_FVF);
	}

	//int b = 0;
	for (i = 0; i < m_pBodyDefs->nBodyDefs; ++i)
	{
		sFxBody& body = m_bodys[i];
		BONES_ARRAY& bones = body.bonesArray;

		sHierarchy& h = m_pBodyDefs->BodyDefs[i].Hierarchy;

		// һ���Դ������й���
		bones.resize(h.nSegments);
		for (int _b = 0; _b < h.nSegments; ++_b)
		{
			sFxBone& bone = bones[_b];

			bone.strBoneName = h.szSegmentNames[_b];
			bone.nParentBoneIndex = h.iParents[_b];
			
			//b++;
		}
	}

	// �ָ�����
	SetReceiveCortexFrame(bReceiveOld);

	return true;
}

Actor2* FxCortex::InnerGetActor() const
{
	IEntity* pEntity = GetCore()->GetEntity(m_ActorBind);
	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Actor2"))
	{
		return NULL;
	}
	return (Actor2*)pEntity;
}

void FxCortex::SetMarkerColorString(const char* value)
{
	m_nMarkerColor = VisUtil_ColorToInt(value);
}

std::string FxCortex::GetMarkerColorString() const
{
	return VisUtil_IntToColor(m_nMarkerColor);
}

bool FxCortex::GetFrameOfData()
{
	sFrameOfData* pFrameOfData = Cortex_GetCurrentFrame();
	if (NULL == pFrameOfData)
	{
		return false;
	}

	CortexFrameDataHandler(pFrameOfData);
	return true;
}

bool FxCortex::CalcTposeTransformMatrix()
{
	// �Ƿ������˶�����׽���
	if (!GetConnected())
	{
		return false;
	}

	// �Ƿ����յ�������׽��֡����
	sFrameOfData* pFrameOfData = Cortex_GetCurrentFrame();
	if (NULL == pFrameOfData)
	{
		return false;
	}

	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return false;
	}

	// �ֶ���һ�λ�ϣ�ȷ��actor��Ĺ��������ǳ�ʼ������
	//pActor->BlendByHand();

	//// ��ʼ����Ϸ��actor��tpose��local matrix
	//for (size_t i = 0; i < m_bodys.size(); ++i)
	//{
	//	BONES_ARRAY& bones= m_bodys[i].bonesArray;
	//	for (size_t _b = 0; _b < bones.size(); ++_b)
	//	{
	//		sFxBone& bone = bones[_b];
	//		if (!pActor->GetBoneLocalMatrix(bone.strBoneName.c_str(), &bone.matActorLocalMatrix))
	//		{
	//			FmMatrixIdentity(&bone.matActorLocalMatrix);
	//		}
	//	}
	//}

	// cortex��ģ�͵�tpose
	sFrameOfData frameCortexTpose;
	memset(&frameCortexTpose, 0, sizeof(sFrameOfData));
	int retval = Cortex_CopyFrame(pFrameOfData, &frameCortexTpose);
	if (retval != RC_Okay)
	{
		CORE_TRACE("(FxCortex::CalcTposeTransformMatrix) Copy frame failed.");
		return false;
	}

	// ������������õ�����ʱ����
	FmMat4 mat_tmp;
	float px, py, pz, rx, ry, rz;

	for (int i = 0; i < frameCortexTpose.nBodies; ++i)
	{
		sFxBody& fxbody = m_bodys[i];

		sBodyData& body = frameCortexTpose.BodyData[i];

		// ����
		BONES_ARRAY& bodyArray = fxbody.bonesArray;
		for (int _b = 0; _b < body.nSegments; ++_b)
		{
			sFxBone& bone = bodyArray[_b];
			FmMat4& mtxCaptureWorldMatrix = bone.mtxCaptureWorldMatrix;

			FmMatrixIdentity(&mtxCaptureWorldMatrix);

			tSegmentData& segment = body.Segments[_b];
			// ��λ��
			// cortex����������ϵ
			px = -(float)segment[0] * g_fMeterFactor;
			py = (float)segment[1] * g_fMeterFactor;
			pz = (float)segment[2] * g_fMeterFactor;

			D3DXMatrixTranslation(&mtxCaptureWorldMatrix, px, py, pz);

			// ��������

			// ����ת
			// cortex����������ϵ
			rx = -(float)segment[3] * g_fRadianFactor;
			ry = -(float)segment[4] * g_fRadianFactor;
			rz = (float)segment[5] * g_fRadianFactor;

			// ��ת˳��: rx-ry-rz
			D3DXMatrixRotationX(&mat_tmp, rx);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
			FmMatrixRotationY(&mat_tmp, ry);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
			D3DXMatrixRotationZ(&mat_tmp, rz);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
		}
	}

	// ���㶯����׽ģ�͵Ĺ�������任����
	for (size_t i = 0; i < m_bodys.size(); ++i)
	{
		BONES_ARRAY& bones= m_bodys[i].bonesArray;
		for (size_t _b = 0; _b < bones.size(); ++_b)
		{
			sFxBone& bone = bones[_b];
			if (bone.nParentBoneIndex > -1)
			{
				// �����������任����
				FmMat4& mat_parent = bones[bone.nParentBoneIndex].mtxCaptureWorldMatrix;
				D3DXMatrixMultiply(&bone.mtxCaptureWorldMatrix, 
					&bone.mtxCaptureWorldMatrix, &mat_parent);
			}
			// ���������任����������
			FmMatrixInverse(&bone.mtxCaptureWorldMatrixInverse, NULL, 
				&bone.mtxCaptureWorldMatrix);
		}
	}

	// ��actor�б�������׽ģ�Ϳ��ƵĹ�����ʼ��ת������
	for (size_t b = 0; b < m_bonesMap.size(); ++b)
	{
		sBoneMap& boneMap = m_bonesMap[b];
		if (boneMap.strCaptureBoneName != "" && 
			pActor->GetBoneMatrix(boneMap.strActorBoneName.c_str(), &mat_tmp))
		{
			sFxBone& bone = 
				m_bodys[boneMap.nCaptureBodyIndex].bonesArray[boneMap.nCaptureBoneIndex];
			D3DXMatrixMultiply(&boneMap.mtxTposeCaptureToActor, 
				&mat_tmp, &bone.mtxCaptureWorldMatrixInverse);
		}
		else
		{
			FmMatrixIdentity(&boneMap.mtxTposeCaptureToActor);
		}
	}

	//for (size_t i = 0; i < m_bodys.size(); ++i)
	//{
	//	BONES_ARRAY& bones= m_bodys[i].bonesArray;
	//	for (size_t _b = 0; _b < bones.size(); ++_b)
	//	{
	//		sFxBone& bone = bones[_b];
	//		// ��cortex��������任�������
	//		FmMatrixInverse(&bone.matTposeCortexToActor, NULL, &bone.matTposeCortexToActor);

	//		if (bone.strActorBoneName != "" && 
	//			pActor->GetBoneMatrix(bone.strActorBoneName.c_str(), &mat_tmp))
	//		{
	//			D3DXMatrixMultiply(&bone.matTposeCortexToActor, 
	//				&mat_tmp, &bone.matTposeCortexToActor);
	//		}
	//		else
	//		{
	//			FmMatrixIdentity(&bone.matTposeCortexToActor);
	//		}
	//	}
	//}

	return true;
}

bool FxCortex::SetBoneMap(const char* szCaptureBoneName, const char* szActorBoneName)
{
	Assert(szCaptureBoneName != NULL && szActorBoneName != NULL);
	if (NULL == szCaptureBoneName || NULL == szActorBoneName)
	{
		return false;
	}

	// �ڶ�����׽��ģ�������ָ���Ĺ���
	bool bFindBoneInCapture = false;
	size_t nBodyIndex = 0;
	size_t nCaptureBoneIndex = 0;
	if (szCaptureBoneName[0] != 0)
	{
		for (nBodyIndex = 0; nBodyIndex < m_bodys.size(); ++nBodyIndex)
		{
			BONES_ARRAY& bones= m_bodys[nBodyIndex].bonesArray;
			for (nCaptureBoneIndex = 0; nCaptureBoneIndex < bones.size(); ++nCaptureBoneIndex)
			{
				sFxBone& bone = bones[nCaptureBoneIndex];
				if (bone.strBoneName == szCaptureBoneName)
				{
					//bone.strActorBoneName = szActorBoneName;
					bFindBoneInCapture = true;
					break;
				}
			}

			if (bFindBoneInCapture)
			{
				break;
			}
		}

		if (!bFindBoneInCapture)
		{
			CORE_TRACE("(FxCortex::SetBoneMap) Cant find bone name in Motion Capture Model.");
			CORE_TRACE(szCaptureBoneName);
		}
	}

	// ��actor�����ָ���Ĺ���
	bool bFindBoneInActor = false;
	size_t nActorBoneIndex;
	for (nActorBoneIndex = 0; nActorBoneIndex < m_bonesMap.size(); ++nActorBoneIndex)
	{
		sBoneMap& boneMap = m_bonesMap[nActorBoneIndex];
		if (boneMap.strActorBoneName == szActorBoneName)
		{
			bFindBoneInActor = true;
			if (bFindBoneInCapture)
			{
				// ����ڶ�����׽ģ�����ж�Ӧ�Ĺ���
				boneMap.strCaptureBoneName = szCaptureBoneName;
				boneMap.nCaptureBodyIndex = nBodyIndex;
				boneMap.nCaptureBoneIndex = nCaptureBoneIndex;
			}
			else
			{
				// ����ڶ�����׽ģ����û�ж�Ӧ�Ĺ���
				// ֻ��Ҫ����������Ϊ�վͿ��ԣ�body��bone����ֵ�ᱻ����
				boneMap.strCaptureBoneName = "";
			}
			break;
		}
	}

	//// ����ڶ�����׽ģ�����иù�������actor��û�ù���
	//if (!bFindBoneInActor && bFindBoneInCapture)
	//{
	//	sFxBone& bone = m_bodys[nBodyIndex].bonesArray[nCaptureBoneIndex];
	//	bone.strActorBoneName = "";
	//}

	return true;
}

void FxCortex::ClearBoneMap()
{
	for (size_t nActorBoneIndex = 0; nActorBoneIndex < m_bonesMap.size(); ++nActorBoneIndex)
	{
		sBoneMap& boneMap = m_bonesMap[nActorBoneIndex];
		boneMap.strCaptureBoneName = "";
	}
}
