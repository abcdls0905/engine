//--------------------------------------------------------------------
// 文件名:		FxVicon.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年5月10日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief Cortex通信

/// entity: FxVicon
/// \brief Cortex通信
/// \see 继承自IVisual
DECLARE_ENTITY("", FxVicon, IVisBase);

/// property: object ActorBind
/// \brief 绑定的动画对象
DECLARE_PROPERTY(PERSISTID, FxVicon, ActorBind, GetActorBind, SetActorBind);
/// method: bool ConnectVicon(const char* szViconIP);
/// \brief 连接Vicon
/// \param szViconIP Vicon的IP
/// \return 连接是否成功
DECLARE_METHOD_1(bool, FxVicon, ConnectVicon, const char*);
/// method: bool GetConnected() const;
/// \brief 是否已经和Vicon连接
/// \return 是否已经和Vicon连接
DECLARE_METHOD_0(bool, FxVicon, GetConnected);
/// property: bool ReceiveViconFrame
/// \brief 是否接收Vicon的帧消息
DECLARE_PROPERTY(bool, FxVicon, ReceiveViconFrame, GetReceiveViconFrame, SetReceiveViconFrame);
/// method: bool GetBodyDefs();
/// \brief 获得各身体部件的描述信息
/// \return 是否成功
DECLARE_METHOD_0(bool, FxVicon, GetBodyDefs);
/// method: bool GetFrameOfData();
/// \brief 获取当前帧数据
/// \return 是否成功
DECLARE_METHOD_0(bool, FxVicon, GetFrameOfData);
/// property: std::string MarkerColor
/// \brief marker点的颜色
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

	// 只允许存在一个实例
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
//	// 停止接收新的vicon数据
//	SetReceiveCortexFrame(false);
//
//	CAutoLock autolock(m_Lock);
//
//	// body的定义信息
//	if (m_pBodyDefs)
//	{
//		Cortex_FreeBodyDefs(m_pBodyDefs);
//		m_pBodyDefs = NULL;
//	}
//
//	// 释放当前正在渲染的两个关键帧
//	if (m_pPrevFrame)
//	{
//		Cortex_FreeFrame(&m_pPrevFrame->cortex_frame);
//		// 如果不需要释放内存池
//		if (!bReleaseMemPool)
//		{
//			// 把帧数据放入空闲队列回收利用
//			PushToFreeList(m_pPrevFrame);
//		}
//		m_pPrevFrame = NULL;
//	}
//
//	if (m_pNextFrame)
//	{
//		Cortex_FreeFrame(&m_pNextFrame->cortex_frame);
//		// 如果不需要释放内存池
//		if (!bReleaseMemPool)
//		{
//			// 把帧数据放入空闲队列回收利用
//			PushToFreeList(m_pNextFrame);
//		}
//		m_pNextFrame = NULL;
//	}
//
//	// 释放等待渲染的帧队列
//	FrameList::iterator ite = m_WaitRenderFrameData.begin();
//	for (; ite != m_WaitRenderFrameData.end(); ++ite)
//	{
//		sFxFrame* pFrame = *ite;
//		Cortex_FreeFrame(&pFrame->cortex_frame);
//
//		// 如果不需要释放内存池
//		if (!bReleaseMemPool)
//		{
//			// 把帧数据放入空闲队列回收利用
//			PushToFreeList(pFrame);
//		}
//	}
//	m_WaitRenderFrameData.clear();
//
//	// 是否释放内存池
//	if (bReleaseMemPool)
//	{
//		// 清空空闲队列
//		m_FreeFrameData.clear();
//
//		// 释放所有内存
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
//	// 释放标记mark
//	m_nMarkers = 0;
//	SAFE_RELEASE(m_pMarkerDynamicVB);
//
//	// 释放body数据
//	m_bodys.clear();
//}

bool FxVicon::GetConnected() const
{
	return m_ViconClient.IsConnected().Connected;
}

//// 把数据里的毫米转成米 liujie
//// 等找到让vicon发以米为单位的数据时，这个东西就不需要了
static float g_fMeterFactor = 0.001f;
//// 角度转弧度的系数
//float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

void FxVicon::Update(float seconds)
{
	if (!GetConnected())
	{
		return;
	}

	// 没绑定游戏对象就不绘制
	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		return ;
	}

	// 骨骼数据是否有更新
	bool bBoneUpdate = false;

	// 有新数据需要渲染
	if (Result::Success == m_ViconClient.GetFrame().Result && 
		m_nFrameNumber != m_ViconClient.GetFrameNumber().FrameNumber)
	{
		bBoneUpdate = true;
		m_nFrameNumber = m_ViconClient.GetFrameNumber().FrameNumber;

		// 定义骨骼操作用到的临时变量
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

				// vicon是右手坐标系
				// 按右手坐标系绘制
				pMarkers[m].pos.x = (float)pos[0] * g_fMeterFactor;
				pMarkers[m].pos.y = (float)pos[1] * g_fMeterFactor;
				pMarkers[m].pos.z = (float)pos[2] * g_fMeterFactor;
				pMarkers[m].diffuse = m_nMarkerColor;
				++m;
			}

			// 骨骼
			BONES_ARRAY& bodyArray = fxbody.bonesArray;
			for (unsigned int _b = 0; _b < bodyArray.size(); ++_b)
			{
				sFxBone& bone = bodyArray[_b];
				FmMat4& mat = bone.mat;
				FmMatrixIdentity(&mat);

				// 做位移
				// vicon是右手坐标系
				double* pos = m_ViconClient.GetSegmentGlobalTranslation(
					fxbody.strBodyName, bone.strBoneName).Translation;
				px = -pos[0] * g_fMeterFactor;
				py = pos[1] * g_fMeterFactor;
				pz = pos[2] * g_fMeterFactor;

				D3DXMatrixTranslation(&mat, px, py, pz);

				// 不做缩放

				// 做旋转
				// cortex是右手坐标系
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

	// 更新游戏里对象的骨骼数据
	if (pActor)
	{
		size_t i = 0;
		// actor中父骨骼的世界变换矩阵
		FmMat4 mtxParentBoneTMInActor;
		// 骨骼变换矩阵
		FmMat4 mtxBoneTMtmp;
		float px, py, pz;
		for (size_t i = 0; i < m_bodys.size(); ++i)
		{
			BONES_ARRAY& bones = m_bodys[i].bonesArray;
			for (unsigned int _b = 0; _b < bones.size(); ++_b)
			{
				sFxBone& bone = bones[_b];

				// actor里有对应的骨骼
				if (pActor->NodeIsExist(bone.strBoneName.c_str()))
				{
					// 获得actor中父骨骼的世界变换矩阵
					if (bone.nParentBoneIndex > -1)
					{
						sFxBone& parentBone = bones[bone.nParentBoneIndex];

						// 获得actor中父骨骼的世界变换矩阵
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

					// 根据actor中父骨骼的世界变换矩阵计算出该骨骼的世界变换矩阵
					D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matActorLocalMatrix, 
						&mtxParentBoneTMInActor);

					// 骨骼坐标使用上面计算的结果，轴向使用该骨骼在动作捕捉中获得的轴向
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

	// 没绑定游戏对象就不绘制
	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	// 动画还没开始
	if (m_nFrameNumber < 0)
	{
		return;
	}

	// 绘制marker
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
	// 重置ip
	m_strViconIP = "";

	if (NULL == szViconIP)
	{
		CORE_TRACE("(FxVicon::ConnectVicon) IP is NULL.");
		return false;
	}

	// 初始化并连接vicon
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
	// 重新获得BodyDefs将导致以前的所有数据作废
	// 释放以前的数据
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

	// 创建subject数组
	m_bodys.resize(nSubjects);

	unsigned int i;
	for (i = 0; i < nSubjects; ++i)
	{
		sFxBody& body = m_bodys[i];
		body.strBodyName = m_ViconClient.GetSubjectName(i).SubjectName;
	}

	// 初始化marker数据
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

		// 一次性创建所有骨骼
		bones.resize(nSegmentCount);

		// 骨骼名和骨骼索引映射表
		typedef std::map<std::string, unsigned int> MapBoneNameToIndex;
		MapBoneNameToIndex mapBoneNameToIndex;

		// 对骨骼进行初始化
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

	// 手动做一次混合，确保actor里的骨骼矩阵是初始化过的
	pActor->BlendByHand();

	if (m_ViconClient.GetFrame().Result != Result::Success)
	{
		return false;
	}

	// 初始化游戏里actor的tpose的local matrix
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

	// 获得Vicon模型的tpose矩阵
	const unsigned int nSubjects = m_ViconClient.GetSubjectCount().SubjectCount;

	// 定义骨骼操作用到的临时变量
	FmMat4 mat_tmp;
	float px, py, pz, rx, ry, rz;
	for (unsigned int i = 0; i < nSubjects; ++i)
	{
		sFxBody& fxbody = m_bodys[i];

		// 骨骼
		BONES_ARRAY& bodyArray = fxbody.bonesArray;
		for (int _b = 0; _b < bodyArray.size(); ++_b)
		{
			sFxBone& bone = bodyArray[_b];
			FmMat4& matTposeViconToActor = bone.matTposeViconToActor;
			FmMatrixIdentity(&matTposeViconToActor);

			// 做位移
			// vicon是右手坐标系
			double* pos = m_ViconClient.GetSegmentGlobalTranslation(
				fxbody.strBodyName, bone.strBoneName).Translation;
			px = -pos[0] * g_fMeterFactor;
			py = pos[1] * g_fMeterFactor;
			pz = pos[2] * g_fMeterFactor;

			D3DXMatrixTranslation(&matTposeViconToActor, px, py, pz);

			// 不做缩放

			// 做旋转
			// cortex是右手坐标系
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

	// 计算Vicon的tpose到actor的变换矩阵
	for (size_t i = 0; i < m_bodys.size(); ++i)
	{
		BONES_ARRAY& bones= m_bodys[i].bonesArray;
		for (size_t _b = 0; _b < bones.size(); ++_b)
		{
			sFxBone& bone = bones[_b];

			// 求vicon骨骼世界变换矩阵的逆
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
