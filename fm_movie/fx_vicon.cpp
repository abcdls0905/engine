//--------------------------------------------------------------------
// �ļ���:		FxVicon.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��5��10��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "fx_vicon.h"
#include "../public/core_log.h"
#pragma comment(lib, "ViconDataStreamSDK_CPP.lib")
#include "fxmathinl.h"
#include "../visual/i_dx_dynamic.h"
#include "actor_2.h"
#include "../visual/i_shader_system_2.h"
#include "../visual/dx_scene_batch.h"
#include <map>

/// \file FxVicon.cpp
/// \brief Cortexͨ��

/// entity: FxVicon
/// \brief Cortexͨ��
/// \see �̳���IVisual
DECLARE_ENTITY("", FxVicon, IVisBase);

/// property: object ActorBind
/// \brief �󶨵Ķ�������
DECLARE_PROPERTY(PERSISTID, FxVicon, ActorBind, GetActorBind, SetActorBind);
/// method: bool ConnectVicon(const char* szViconIP);
/// \brief ����Vicon
/// \param szViconIP Vicon��IP
/// \return �����Ƿ�ɹ�
DECLARE_METHOD_1(bool, FxVicon, ConnectVicon, const char*);
/// method: bool GetConnected() const;
/// \brief �Ƿ��Ѿ���Vicon����
/// \return �Ƿ��Ѿ���Vicon����
DECLARE_METHOD_0(bool, FxVicon, GetConnected);
/// property: bool ReceiveViconFrame
/// \brief �Ƿ����Vicon��֡��Ϣ
DECLARE_PROPERTY(bool, FxVicon, ReceiveViconFrame, GetReceiveViconFrame, SetReceiveViconFrame);
/// method: bool GetBodyDefs();
/// \brief ��ø����岿����������Ϣ
/// \return �Ƿ�ɹ�
DECLARE_METHOD_0(bool, FxVicon, GetBodyDefs);
/// method: bool GetFrameOfData();
/// \brief ��ȡ��ǰ֡����
/// \return �Ƿ�ɹ�
DECLARE_METHOD_0(bool, FxVicon, GetFrameOfData);
/// property: std::string MarkerColor
/// \brief marker�����ɫ
DECLARE_PROPERTY(std::string, FxVicon, MarkerColor, GetMarkerColorString, SetMarkerColorString);


static FmMat4 g_identityMatrix;

FxVicon::FxVicon() 
{
	FmMatrixIdentity(&g_identityMatrix);

	m_nMarkers = 0;
	m_pMarkerDynamicVB = NULL;
	m_nMarkerColor = 0xffffffff;
	m_nShader = -1;
	m_nFrameNumber = -1;
}

FxVicon::~FxVicon() 
{
	//ReleaseResource();
}

bool FxVicon::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}

	// ֻ�������һ��ʵ��
	IEntity* pEntity = GetCore()->LookupEntity("FxVicon");
	if (pEntity != this)
	{
		CORE_TRACE("(FxVicon::Init) FxVicon already exists");
		return false;
	}

	IShaderManager* pShaderManager = m_pRender->GetShaderManager();

	m_nShader = pShaderManager->LoadShader("FxVicon.c",
		"vs_main", "ps_main", "");
	if (-1 == m_nShader)
	{
		CORE_TRACE("(FxVicon::Init) Load shader failed.");
		return false;
	}

	return true;
}

bool FxVicon::Shut()
{
	return IVisual::Shut();
}

//void FxVicon::ReleaseResource(bool bReleaseMemPool)
//{
//	// ֹͣ�����µ�vicon����
//	SetReceiveCortexFrame(false);
//
//	CAutoLock autolock(m_Lock);
//
//	// body�Ķ�����Ϣ
//	if (m_pBodyDefs)
//	{
//		Cortex_FreeBodyDefs(m_pBodyDefs);
//		m_pBodyDefs = NULL;
//	}
//
//	// �ͷŵ�ǰ������Ⱦ�������ؼ�֡
//	if (m_pPrevFrame)
//	{
//		Cortex_FreeFrame(&m_pPrevFrame->cortex_frame);
//		// �������Ҫ�ͷ��ڴ��
//		if (!bReleaseMemPool)
//		{
//			// ��֡���ݷ�����ж��л�������
//			PushToFreeList(m_pPrevFrame);
//		}
//		m_pPrevFrame = NULL;
//	}
//
//	if (m_pNextFrame)
//	{
//		Cortex_FreeFrame(&m_pNextFrame->cortex_frame);
//		// �������Ҫ�ͷ��ڴ��
//		if (!bReleaseMemPool)
//		{
//			// ��֡���ݷ�����ж��л�������
//			PushToFreeList(m_pNextFrame);
//		}
//		m_pNextFrame = NULL;
//	}
//
//	// �ͷŵȴ���Ⱦ��֡����
//	FrameList::iterator ite = m_WaitRenderFrameData.begin();
//	for (; ite != m_WaitRenderFrameData.end(); ++ite)
//	{
//		sFxFrame* pFrame = *ite;
//		Cortex_FreeFrame(&pFrame->cortex_frame);
//
//		// �������Ҫ�ͷ��ڴ��
//		if (!bReleaseMemPool)
//		{
//			// ��֡���ݷ�����ж��л�������
//			PushToFreeList(pFrame);
//		}
//	}
//	m_WaitRenderFrameData.clear();
//
//	// �Ƿ��ͷ��ڴ��
//	if (bReleaseMemPool)
//	{
//		// ��տ��ж���
//		m_FreeFrameData.clear();
//
//		// �ͷ������ڴ�
//		size_t nBufferSize = m_Buffers.size();
//		for (size_t i = 0; i < nBufferSize; ++i)
//		{
//			CORE_FREE(m_Buffers[i]);
//		}
//		m_Buffers.clear();
//	}
//
//	m_fSecondsFromPrevFrame = 0.0f;
//	m_fSecondsBetweenTwoFrames = 0.0f;
//
//	// �ͷű��mark
//	m_nMarkers = 0;
//	SAFE_RELEASE(m_pMarkerDynamicVB);
//
//	// �ͷ�body����
//	m_bodys.clear();
//}

bool FxVicon::GetConnected() const
{
	return m_ViconClient.IsConnected().Connected;
}

//// ��������ĺ���ת���� liujie
//// ���ҵ���vicon������Ϊ��λ������ʱ����������Ͳ���Ҫ��
static float g_fMeterFactor = 0.001f;
//// �Ƕ�ת���ȵ�ϵ��
//float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

void FxVicon::Update(float seconds)
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

	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		return ;
	}

	// ���������Ƿ��и���
	bool bBoneUpdate = false;

	// ����������Ҫ��Ⱦ
	if (Result::Success == m_ViconClient.GetFrame().Result && 
		m_nFrameNumber != m_ViconClient.GetFrameNumber().FrameNumber)
	{
		bBoneUpdate = true;
		m_nFrameNumber = m_ViconClient.GetFrameNumber().FrameNumber;

		// ������������õ�����ʱ����
		FmMat4 mat_tmp;
		float px, py, pz, rx, ry, rz;

		sFxMarkerVertex* pMarkers = (sFxMarkerVertex*)m_pMarkerDynamicVB->Lock();

		const unsigned int nSubjects = m_ViconClient.GetSubjectCount().SubjectCount;

		//int b = 0;
		int m = 0;
		for (unsigned int i = 0; i < nSubjects; ++i)
		{
			sFxBody& fxbody = m_bodys[i];
			MARKER_ARRAY& markers = fxbody.markersArray;

			// marker
			for (unsigned int _m = 0; _m < markers.size(); ++_m)
			{
				double* pos = m_ViconClient.GetMarkerGlobalTranslation(fxbody.strBodyName, 
					markers[_m].strMarkerName).Translation;

				// vicon����������ϵ
				// ����������ϵ����
				pMarkers[m].pos.x = (float)pos[0] * g_fMeterFactor;
				pMarkers[m].pos.y = (float)pos[1] * g_fMeterFactor;
				pMarkers[m].pos.z = (float)pos[2] * g_fMeterFactor;
				pMarkers[m].diffuse = m_nMarkerColor;
				++m;
			}

			// ����
			BONES_ARRAY& bodyArray = fxbody.bonesArray;
			for (unsigned int _b = 0; _b < bodyArray.size(); ++_b)
			{
				sFxBone& bone = bodyArray[_b];
				FmMat4& mat = bone.mat;
				FmMatrixIdentity(&mat);

				// ��λ��
				// vicon����������ϵ
				double* pos = m_ViconClient.GetSegmentGlobalTranslation(
					fxbody.strBodyName, bone.strBoneName).Translation;
				px = -pos[0] * g_fMeterFactor;
				py = pos[1] * g_fMeterFactor;
				pz = pos[2] * g_fMeterFactor;

				D3DXMatrixTranslation(&mat, px, py, pz);

				// ��������

				// ����ת
				// cortex����������ϵ
				double* rotate_matrix = m_ViconClient.GetSegmentGlobalRotationMatrix(
					fxbody.strBodyName, bone.strBoneName).Rotation;
				FmMatrixIdentity(&mat_tmp);
				mat_tmp._11 = rotate_matrix[0];
				mat_tmp._12 = rotate_matrix[1];
				mat_tmp._13 = rotate_matrix[2];
				mat_tmp._21 = rotate_matrix[3];
				mat_tmp._22 = rotate_matrix[4];
				mat_tmp._23 = rotate_matrix[5];
				mat_tmp._31 = rotate_matrix[6];
				mat_tmp._32 = rotate_matrix[7];
				mat_tmp._33 = rotate_matrix[8];
				D3DXMatrixMultiply(&mat, &mat_tmp, &mat);
				
//#ifdef _DEBUG
//				bone.debug_vTranslation.x = px;
//				bone.debug_vTranslation.y = py;
//				bone.debug_vTranslation.z = pz;
//				bone.debug_local_mat = mat;
//#endif

				//++b;
			}
		}

		m_pMarkerDynamicVB->Unlock();
	}

	// ������Ϸ�����Ĺ�������
	if (pActor)
	{
		size_t i = 0;
		// actor�и�����������任����
		FmMat4 mtxParentBoneTMInActor;
		// �����任����
		FmMat4 mtxBoneTMtmp;
		float px, py, pz;
		for (size_t i = 0; i < m_bodys.size(); ++i)
		{
			BONES_ARRAY& bones = m_bodys[i].bonesArray;
			for (unsigned int _b = 0; _b < bones.size(); ++_b)
			{
				sFxBone& bone = bones[_b];

				// actor���ж�Ӧ�Ĺ���
				if (pActor->NodeIsExist(bone.strBoneName.c_str()))
				{
					// ���actor�и�����������任����
					if (bone.nParentBoneIndex > -1)
					{
						sFxBone& parentBone = bones[bone.nParentBoneIndex];

						// ���actor�и�����������任����
						if (!pActor->GetBoneMatrix(parentBone.strBoneName.c_str(), 
							&mtxParentBoneTMInActor))
						{
							FmMatrixIdentity(&mtxParentBoneTMInActor);
						}
					}
					else
					{
						FmMatrixIdentity(&mtxParentBoneTMInActor);
					}

					// ����actor�и�����������任���������ù���������任����
					D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matActorLocalMatrix, 
						&mtxParentBoneTMInActor);

					// ��������ʹ���������Ľ��������ʹ�øù����ڶ�����׽�л�õ�����
					px = mtxBoneTMtmp._41;
					py = mtxBoneTMtmp._42;
					pz = mtxBoneTMtmp._43;
					D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matTposeViconToActor, &bone.mat);
					mtxBoneTMtmp._41 = px;
					mtxBoneTMtmp._42 = py;
					mtxBoneTMtmp._43 = pz;

					pActor->SetBoneMatrix(bone.strBoneName.c_str(), &mtxBoneTMtmp);
				}
			}
		}

		pActor->UpdateBoneMatrix();
	}
}

void FxVicon::Realize(float offset_seconds)
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
	if (m_nFrameNumber < 0)
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

bool FxVicon::ConnectVicon(const char* szViconIP)
{
	// ����ip
	m_strViconIP = "";

	if (NULL == szViconIP)
	{
		CORE_TRACE("(FxVicon::ConnectVicon) IP is NULL.");
		return false;
	}

	// ��ʼ��������vicon
	m_ViconClient.Connect(szViconIP);
	if (!GetConnected())
	{
		CORE_TRACE("(FxVicon::ConnectCortex) Connect failed");
		CORE_TRACE(szViconIP);
		return false;
	}

	m_strViconIP = szViconIP;

	// Enable some different data types
	m_ViconClient.EnableSegmentData();
	m_ViconClient.EnableMarkerData();
	m_ViconClient.EnableUnlabeledMarkerData();
	m_ViconClient.EnableDeviceData();

	// Set the streaming mode
	m_ViconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
	// m_ViconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );
	//m_ViconClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ServerPush );

	// Set the global up axis
	m_ViconClient.SetAxisMapping( Direction::Right, 
		Direction::Up, 
		Direction::Forward ); // Y-up
	return true;
}

void FxVicon::SetActorBind(const PERSISTID& actor)
{
	m_ActorBind = actor;
}

PERSISTID FxVicon::GetActorBind() const
{
	return m_ActorBind;
}

Actor2* FxVicon::InnerGetActor() const
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

void FxVicon::SetMarkerColorString(const char* value)
{
	m_nMarkerColor = VisUtil_ColorToInt(value);
}

std::string FxVicon::GetMarkerColorString() const
{
	return VisUtil_IntToColor(m_nMarkerColor);
}

bool FxVicon::GetFrameOfData()
{
	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		CORE_TRACE("(FxVicon::GetFrameOfData) Get frame failed.");
		return false;
	}
	return true;
}

bool FxVicon::GetBodyDefs()
{
	// ���»��BodyDefs��������ǰ��������������
	// �ͷ���ǰ������
	//ReleaseResource(false);

	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		CORE_TRACE("(FxVicon::GetBodyDefs) Get frame failed.");
		return false;
	}

	const unsigned int nSubjects = m_ViconClient.GetSubjectCount().SubjectCount;
	if (0 == nSubjects)
	{
		CORE_TRACE("(FxVicon::GetBodyDefs) No body.");
		return false;
	}

	// ����subject����
	m_bodys.resize(nSubjects);

	unsigned int i;
	for (i = 0; i < nSubjects; ++i)
	{
		sFxBody& body = m_bodys[i];
		body.strBodyName = m_ViconClient.GetSubjectName(i).SubjectName;
	}

	// ��ʼ��marker����
	m_nMarkers = 0;
	for (i = 0; i < nSubjects; ++i)
	{
		sFxBody& body = m_bodys[i];
		unsigned int nMarkers = m_ViconClient.GetMarkerCount(body.strBodyName).MarkerCount;
		m_bodys[i].nMarkerOffset = m_nMarkers;
		m_bodys[i].nMarkerCount = nMarkers;
		m_nMarkers += nMarkers;
	}

	if (m_nMarkers > 0)
	{
		m_pMarkerDynamicVB = m_pRender->CreateDynamicVB(
			m_nMarkers * sizeof(sFxMarkerVertex), MARKER_FVF);
	}

	//int b = 0;
	for (i = 0; i < nSubjects; ++i)
	{
		sFxBody& body = m_bodys[i];
		MARKER_ARRAY& markers = body.markersArray;
		unsigned int nMarkerCount = m_ViconClient.GetMarkerCount(body.strBodyName).MarkerCount;
		markers.resize(nMarkerCount);
		for (unsigned int _m = 0; _m < nMarkerCount; ++_m)
		{
			markers[_m].strMarkerName = m_ViconClient.GetMarkerName(body.strBodyName, _m).MarkerName;
		}

		BONES_ARRAY& bones = body.bonesArray;
		unsigned int nSegmentCount = m_ViconClient.GetSegmentCount(body.strBodyName).SegmentCount;

		// һ���Դ������й���
		bones.resize(nSegmentCount);

		// �������͹�������ӳ���
		typedef std::map<std::string, unsigned int> MapBoneNameToIndex;
		MapBoneNameToIndex mapBoneNameToIndex;

		// �Թ������г�ʼ��
		MapBoneNameToIndex::iterator ite;
		for (unsigned int _b = 0; _b < nSegmentCount; ++_b)
		{
			sFxBone& bone = bones[_b];

			bone.strBoneName = 
				m_ViconClient.GetSegmentName(body.strBodyName, _b).SegmentName;

			mapBoneNameToIndex[bone.strBoneName] = _b;

			std::string strSegmentParentName = 
				m_ViconClient.GetSegmentParentName(body.strBodyName, bone.strBoneName).SegmentName;

			ite = mapBoneNameToIndex.find(strSegmentParentName);
			if (mapBoneNameToIndex.end() == ite)
			{
				bone.nParentBoneIndex = -1;
			}
			else
			{
				bone.nParentBoneIndex = ite->second;
			}

			//b++;
		}
	}

	return true;
}

bool FxVicon::CalcTposeTransformMatrix()
{
	if (!GetConnected())
	{
		return false;
	}

	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return false;
	}

	// �ֶ���һ�λ�ϣ�ȷ��actor��Ĺ��������ǳ�ʼ������
	pActor->BlendByHand();

	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		return false;
	}

	// ��ʼ����Ϸ��actor��tpose��local matrix
	for (size_t i = 0; i < m_bodys.size(); ++i)
	{
		BONES_ARRAY& bones= m_bodys[i].bonesArray;
		for (size_t _b = 0; _b < bones.size(); ++_b)
		{
			sFxBone& bone = bones[_b];
			if (!pActor->GetBoneLocalMatrix(bone.strBoneName.c_str(), &bone.matActorLocalMatrix))
			{
				FmMatrixIdentity(&bone.matActorLocalMatrix);
			}
		}
	}

	// ���Viconģ�͵�tpose����
	const unsigned int nSubjects = m_ViconClient.GetSubjectCount().SubjectCount;

	// ������������õ�����ʱ����
	FmMat4 mat_tmp;
	float px, py, pz, rx, ry, rz;
	for (unsigned int i = 0; i < nSubjects; ++i)
	{
		sFxBody& fxbody = m_bodys[i];

		// ����
		BONES_ARRAY& bodyArray = fxbody.bonesArray;
		for (int _b = 0; _b < bodyArray.size(); ++_b)
		{
			sFxBone& bone = bodyArray[_b];
			FmMat4& matTposeViconToActor = bone.matTposeViconToActor;
			FmMatrixIdentity(&matTposeViconToActor);

			// ��λ��
			// vicon����������ϵ
			double* pos = m_ViconClient.GetSegmentGlobalTranslation(
				fxbody.strBodyName, bone.strBoneName).Translation;
			px = -pos[0] * g_fMeterFactor;
			py = pos[1] * g_fMeterFactor;
			pz = pos[2] * g_fMeterFactor;

			D3DXMatrixTranslation(&matTposeViconToActor, px, py, pz);

			// ��������

			// ����ת
			// cortex����������ϵ
			double* rotate_matrix = m_ViconClient.GetSegmentGlobalRotationMatrix(
				fxbody.strBodyName, bone.strBoneName).Rotation;
			FmMatrixIdentity(&mat_tmp);
			mat_tmp._11 = rotate_matrix[0];
			mat_tmp._12 = rotate_matrix[1];
			mat_tmp._13 = rotate_matrix[2];
			mat_tmp._21 = rotate_matrix[3];
			mat_tmp._22 = rotate_matrix[4];
			mat_tmp._23 = rotate_matrix[5];
			mat_tmp._31 = rotate_matrix[6];
			mat_tmp._32 = rotate_matrix[7];
			mat_tmp._33 = rotate_matrix[8];
			D3DXMatrixMultiply(&matTposeViconToActor, &mat_tmp, &matTposeViconToActor);
		}
	}

	// ����Vicon��tpose��actor�ı任����
	for (size_t i = 0; i < m_bodys.size(); ++i)
	{
		BONES_ARRAY& bones= m_bodys[i].bonesArray;
		for (size_t _b = 0; _b < bones.size(); ++_b)
		{
			sFxBone& bone = bones[_b];

			// ��vicon��������任�������
			FmMatrixInverse(&bone.matTposeViconToActor, NULL, &bone.matTposeViconToActor);

			if (pActor->GetBoneMatrix(bone.strBoneName.c_str(), &mat_tmp))
			{
				D3DXMatrixMultiply(&bone.matTposeViconToActor, 
					&mat_tmp, &bone.matTposeViconToActor);
			}
			else
			{
				FmMatrixIdentity(&bone.matTposeViconToActor);
			}
		}
	}

	return true;
}
