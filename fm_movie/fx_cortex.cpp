//--------------------------------------------------------------------
// 文件名:		FxCortex.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年3月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief Cortex通信

/// entity: FxCortex
/// \brief Cortex通信
/// \see 继承自IVisual
DECLARE_ENTITY("", FxCortex, IVisBase);

/// property: object ActorBind
/// \brief 绑定的动画对象
DECLARE_PROPERTY(PERSISTID, FxCortex, ActorBind, GetActorBind, SetActorBind);
/// method: bool ConnectCortex(char* szMyIP, char* szCortexIP);
/// \brief 连接Cortex
/// \param szMyIP 本机IP
/// \param szCortexIP Cortex的IP
/// \return 连接是否成功
DECLARE_METHOD_2(bool, FxCortex, ConnectCortex, const char*, const char*);
/// method: bool GetConnected() const;
/// \brief 是否已经和Cortex连接
/// \return 是否已经和Cortex连接
DECLARE_METHOD_0(bool, FxCortex, GetConnected);
/// method: bool GetBodyDefs();
/// \brief 从Cortex获得各身体部件的描述信息
/// \return 是否成功
DECLARE_METHOD_0(bool, FxCortex, GetBodyDefs);
/// property: bool ReceiveCortexFrame
/// \brief 是否接收Cortex的帧消息
DECLARE_PROPERTY(bool, FxCortex, ReceiveCortexFrame, GetReceiveCortexFrame, SetReceiveCortexFrame);
/// method: bool GetFrameOfData();
/// \brief 从cortex获取当前帧数据
/// \return 是否成功
DECLARE_METHOD_0(bool, FxCortex, GetFrameOfData);
/// property: std::string MarkerColor
/// \brief marker点的颜色
DECLARE_PROPERTY(std::string, FxCortex, MarkerColor, GetMarkerColorString, SetMarkerColorString);

/// method: bool CalcTposeTransformMatrix();
/// \brief 计算从cortex的模型tpose变换到游戏模型tpose的变换矩阵, 必须在cortex的模型和游戏里的模型都处在tpose状态时才能调用
/// \return 是否成功
DECLARE_METHOD_0(bool, FxCortex, CalcTposeTransformMatrix);

/// method: bool SetBoneMap(const char* szCortexBoneName, const char* szActorBoneName)
/// \brief 设置cortex模型和actor的骨骼对应关系
/// \param szCortexBoneName cortex骨骼名
/// \param szActorBoneName actor骨骼名
/// \return 是否成功
DECLARE_METHOD_2(bool, FxCortex, SetBoneMap, const char*, const char*);

/// method: void ClearBoneMap()
/// \brief 
DECLARE_METHOD_VOID_0(FxCortex, ClearBoneMap);


// 这个全局变量是为了让cortex的回调可以访问FxCortex实例
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

	// 如果现在不接收cortex的帧消息
	if (!s_pFxCortex->GetReceiveCortexFrame())
	{
		// 直接忽略这个帧
		return;
	}

	// 从空闲帧队列里取个帧数据用
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

	// 加入等待渲染的帧队列
	s_pFxCortex->PushToWaitList(pNewFrame);
}

bool FxCortex::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}

	// 只允许存在一个实例
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

	// 如果已经存在一个该类的实例并对s_pFxCortex赋值了
	// 则释放前一个实例
	// 不应该出现这种情况
	if (s_pFxCortex)
	{
		Assert(0);
		SAFE_RELEASE(s_pFxCortex);
	}

	s_pFxCortex = this;

	// 设置从cortex接收的消息等级
	Cortex_SetVerbosityLevel(VL_Info);

	// cortex sdk的版本号
	unsigned char szCortexSDKVersion[4];
	Cortex_GetSdkVersion(szCortexSDKVersion);
	// 打印 sdk version TODO

	// 设置从cortex接收消息的回调函数
	// 接到的不止有错误消息，还包括一些提示信息等
	if (Cortex_SetErrorMsgHandlerFunc(CortexErrorMsgHandler) != RC_Okay)
	{
		CORE_TRACE("(FxCortex::Init) Cortex_SetErrorMsgHandlerFunc failed.");
	}

	// 设置从cortex接收帧数据的回调函数
	if (Cortex_SetDataHandlerFunc(CortexFrameDataHandler) != RC_Okay)
	{
		CORE_TRACE("(FxCortex::Init) Cortex_SetDataHandlerFunc failed.");
	}

	// 预先创建一些空闲帧
	AllocFreeFrame(10);

	return true;
}

bool FxCortex::Shut()
{
	// 不知道创建多个FxCortex对象时这里是否会有问题，需要调试一下 liujie

	// 以下代码不能放到析构里，否则创建多个FxCortex对象时将重复执行
	s_pFxCortex = NULL;
	Cortex_Exit();

	return IVisual::Shut();
}

void FxCortex::ReleaseResource(bool bReleaseMemPool)
{
	// 停止接收新的cortex数据
	SetReceiveCortexFrame(false);

	CAutoLock autolock(m_Lock);

	// body的定义信息
	if (m_pBodyDefs)
	{
		Cortex_FreeBodyDefs(m_pBodyDefs);
		m_pBodyDefs = NULL;
	}

	// 释放当前正在渲染的两个关键帧
	if (m_pPrevFrame)
	{
		Cortex_FreeFrame(&m_pPrevFrame->cortex_frame);
		// 如果不需要释放内存池
		if (!bReleaseMemPool)
		{
			// 把帧数据放入空闲队列回收利用
			PushToFreeList(m_pPrevFrame);
		}
		m_pPrevFrame = NULL;
	}

	if (m_pNextFrame)
	{
		Cortex_FreeFrame(&m_pNextFrame->cortex_frame);
		// 如果不需要释放内存池
		if (!bReleaseMemPool)
		{
			// 把帧数据放入空闲队列回收利用
			PushToFreeList(m_pNextFrame);
		}
		m_pNextFrame = NULL;
	}

	// 释放等待渲染的帧队列
	FrameList::iterator ite = m_WaitRenderFrameData.begin();
	for (; ite != m_WaitRenderFrameData.end(); ++ite)
	{
		sFxFrame* pFrame = *ite;
		Cortex_FreeFrame(&pFrame->cortex_frame);

		// 如果不需要释放内存池
		if (!bReleaseMemPool)
		{
			// 把帧数据放入空闲队列回收利用
			PushToFreeList(pFrame);
		}
	}
	m_WaitRenderFrameData.clear();

	// 是否释放内存池
	if (bReleaseMemPool)
	{
		// 清空空闲队列
		m_FreeFrameData.clear();

		// 释放所有内存
		size_t nBufferSize = m_Buffers.size();
		for (size_t i = 0; i < nBufferSize; ++i)
		{
			CORE_FREE(m_Buffers[i]);
		}
		m_Buffers.clear();
	}

	m_fSecondsFromPrevFrame = 0.0f;
	m_fSecondsBetweenTwoFrames = 0.0f;

	// 释放标记mark
	m_nMarkers = 0;
	SAFE_RELEASE(m_pMarkerDynamicVB);

	// 释放body数据
	m_bodys.clear();
}

bool FxCortex::GetConnected() const
{
	return m_bConnected;
}

// 把数据里的毫米转成米 liujie
// 等找到让cortex发以米为单位的数据时，这个东西就不需要了
float g_fMeterFactor = 0.001f;
// 角度转弧度的系数
float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

void FxCortex::Update(float seconds)
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

	m_fSecondsFromPrevFrame += seconds;

	// 如果动画还没开始并且有帧在等待渲染
	if (NULL == m_pPrevFrame && !m_WaitRenderFrameData.empty())
	{
		// 后一个关键帧必须为空
		Assert(NULL == m_pNextFrame);
		if (m_pNextFrame)
		{
			PushToFreeList(m_pNextFrame);
			m_pNextFrame = NULL;
		}

		m_pPrevFrame = PopFromWaitList();

		// 重新计时
		m_fSecondsFromPrevFrame = 0;
		m_fSecondsBetweenTwoFrames = 0; // liujie
	}

	// 如果还没下一帧并且有帧在等待渲染
	if (NULL == m_pNextFrame && !m_WaitRenderFrameData.empty())
	{
		Assert(m_pPrevFrame != NULL);
		m_pNextFrame = PopFromWaitList();

		// 重新计时
		m_fSecondsFromPrevFrame = 0;
		m_fSecondsBetweenTwoFrames = 0; // liujie
	}

	// 当有下一帧并且时间到达或超过后一个关键帧时
	if (m_pNextFrame && m_fSecondsFromPrevFrame >= m_fSecondsBetweenTwoFrames)
	{
		// 释放前一个关键帧
		PushToFreeList(m_pPrevFrame);
		m_pPrevFrame = NULL;

		// 后一个关键帧变成前一个关键帧
		m_pPrevFrame = m_pNextFrame;
		m_pNextFrame = NULL;

		// 如果有等待渲染的帧
		if (!m_WaitRenderFrameData.empty())
		{
			// 获得下一帧
			m_pNextFrame = PopFromWaitList();
			// 重新计时
			m_fSecondsFromPrevFrame = 0;
			m_fSecondsBetweenTwoFrames = 0; // liujie
		}
	}

	// 骨骼数据是否有更新
	bool bBoneUpdate = false;

	// 需要插值并有下一帧就插值
	if (GetInterpolation() && m_pNextFrame)
	{
		//bBoneUpdate = true;
	}
	// 直接显示前一帧
	else
	{
		// 如果前一帧还没渲染过则更新marker和骨骼数据
		if (!m_pPrevFrame->m_bRender)
		{
			bBoneUpdate = true;

			// 定义骨骼操作用到的临时变量
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
					// cortex是右手坐标系
					//pMarkers[m].pos.x = -marker[0] * g_fMeterFactor;
					// 按右手坐标系绘制
					pMarkers[m].pos.x = -marker[0] * g_fMeterFactor;
					pMarkers[m].pos.y = marker[1] * g_fMeterFactor;
					pMarkers[m].pos.z = marker[2] * g_fMeterFactor;
					pMarkers[m].diffuse = m_nMarkerColor;
					++m;
				}

				// 骨骼
				BONES_ARRAY& bodyArray = fxbody.bonesArray;
				for (int _b = 0; _b < body.nSegments; ++_b)
				{
					sFxBone& bone = bodyArray[_b];
					FmMat4& mat = bone.mat;
					FmMatrixIdentity(&mat);

					tSegmentData& segment = body.Segments[_b];
					// 做位移
					// cortex是右手坐标系
					px = -(float)segment[0] * g_fMeterFactor;
					py = (float)segment[1] * g_fMeterFactor;
					pz = (float)segment[2] * g_fMeterFactor;

					D3DXMatrixTranslation(&mat, px, py, pz);

					// 不做缩放

					// 做旋转
					// cortex是右手坐标系
					rx = -(float)segment[3] * g_fRadianFactor;
					ry = -(float)segment[4] * g_fRadianFactor;
					rz = (float)segment[5] * g_fRadianFactor;

					// 旋转顺序: rx-ry-rz
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

	// 前一帧已经开始被渲染了
	m_pPrevFrame->m_bRender = true;

	if (bBoneUpdate)
	{
#ifdef _DEBUG
		FmVec3 debug_vOutScale, debug_vOutTranslation;
		FmQuat debug_QOutRotate;
		FmVec3 debug_vAxis;
		float debug_fAngle;
#endif
		// 计算骨骼矩阵
		for (size_t i = 0; i < m_bodys.size(); ++i)
		{
			BONES_ARRAY& bones= m_bodys[i].bonesArray;
			for (size_t _b = 0; _b < bones.size(); ++_b)
			{
				sFxBone& bone = bones[_b];
				if (bone.nParentBoneIndex > -1)
				{
					// 求子骨骼的世界变换矩阵
					D3DXMatrixMultiply(&bone.mat, &bone.mat, &bones[bone.nParentBoneIndex].mat);
//#ifdef _DEBUG
//					// 世界变换
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bone.mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//
//					// 局部变换
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bone.debug_local_mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//
//					// 父矩阵
//					D3DXMatrixDecompose(&debug_vOutScale, &debug_QOutRotate, 
//						&debug_vOutTranslation, &bones[bone.nParentBoneIndex].mat);
//					D3DXQuaternionToAxisAngle(&debug_QOutRotate, &debug_vAxis, &debug_fAngle);
//#endif
				}
			}
		}
	}

	// 更新游戏里对象的骨骼数据
	if (pActor)
	{
		size_t i = 0;
		// actor中父骨骼的世界变换矩阵
		FmMat4 mtxParentBoneWorldMatrixInActor;
		// 骨骼变换矩阵
		FmMat4 mtxBoneTMtmp;
		float px, py, pz;
		// 
		for (size_t b = 0; b < m_bonesMap.size(); ++b)
		{
			sBoneMap& boneMap = m_bonesMap[b];

			// 获得actor中父骨骼的世界变换矩阵
			if (!pActor->GetBoneMatrix(boneMap.strActorBoneParentName.c_str(), 
				&mtxParentBoneWorldMatrixInActor))
			{
				FmMatrixIdentity(&mtxParentBoneWorldMatrixInActor);
			}

			// 根据actor中父骨骼的世界变换矩阵计算出该骨骼的世界变换矩阵
			D3DXMatrixMultiply(&mtxBoneTMtmp, &boneMap.mtxActorBoneLocalMatrix, 
				&mtxParentBoneWorldMatrixInActor);

			// 被动作捕捉模型控制的骨骼需要设置成动作捕捉的骨骼轴向
			if (boneMap.strCaptureBoneName != "")
			{
				sFxBone& bone = 
					m_bodys[boneMap.nCaptureBodyIndex].bonesArray[boneMap.nCaptureBoneIndex];

				// 骨骼坐标使用上面计算的结果，轴向使用该骨骼在动作捕捉中获得的轴向
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

		//		// actor里有对应的骨骼
		//		if (bone.strActorBoneName != "" &&
		//			pActor->NodeIsExist(bone.strActorBoneName.c_str()))
		//		{
		//			// 获得actor中父骨骼的世界变换矩阵
		//			if (bone.nParentBoneIndex > -1)
		//			{
		//				sFxBone& parentBone = bones[bone.nParentBoneIndex];

		//				// 获得actor中父骨骼的世界变换矩阵
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

		//			// 根据actor中父骨骼的世界变换矩阵计算出该骨骼的世界变换矩阵
		//			D3DXMatrixMultiply(&mtxBoneTMtmp, &bone.matActorLocalMatrix, 
		//				&mtxParentBoneTMInActor);

		//			// 骨骼坐标使用上面计算的结果，轴向使用该骨骼在动作捕捉中获得的轴向
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

	// 没绑定游戏对象就不绘制
	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	// 动画还没开始
	if (NULL == m_pPrevFrame)
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

// 连接Cortex
bool FxCortex::ConnectCortex(const char* szMyIP, const char* szCortexIP)
{
	// 停止接收新的cortex数据
	SetReceiveCortexFrame(false);

	// 释放以前的资源
	ReleaseResource(false);

	// cortex是通过初始来建立连接的；
	// 只要之前用Cortex_Initialize函数尝试过连接，不论是否连接成功
	// 都会初始化cortex sdk，
	// 如果之后再调用Cortex_Initialize都会返回错误，因为已经初始化
	// 所以这里先关闭cortex再调用Cortex_Initialize
	Cortex_Exit();
	m_bConnected = false;

	// 重置ip
	m_strCortexIP = "";

	if (NULL == szMyIP || NULL == szCortexIP)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) IP is NULL.");
		return false;
	}

	// 初始化并连接cortex
	int retval = Cortex_Initialize((char*)szMyIP, (char*)szCortexIP);
	if (RC_Okay != retval)
	{
		CORE_TRACE("(FxCortex::ConnectCortex) Initialize Cortex failed");
		CORE_TRACE(szMyIP);
		CORE_TRACE(szCortexIP);
		return false;
	}

	// 查询连接的结果
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

	// 初始化所有和actor相关的数据

	Actor2* pActor = InnerGetActor();
	if (NULL == pActor)
	{
		return;
	}

	// 手动做一次混合，确保actor里的骨骼矩阵是初始化过的
	pActor->BlendByHand();

	// 获得actor的骨骼列表
	CVarList actorBoneList;
	pActor->GetBoneNodeList(CVarList(), actorBoneList);

	// 初始化actor骨骼和动作捕捉骨骼的对应关系表
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
	// 空闲队列为空
	if (m_FreeFrameData.empty())
	{
		// 创建的帧
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

	// 直接从空闲队列里取出一帧
	sFxFrame* pFrame = m_FreeFrameData.front();
	m_FreeFrameData.pop_front();

	// 重置
	pFrame->m_bRender = false;
	return pFrame;
}

void FxCortex::PushToFreeList(sFxFrame* pFrame)
{
	CAutoLock autolock(m_Lock);
	if (pFrame)
	{
		// 重置
		pFrame->m_bRender = false;
		// 必须先调用Cortex的释放
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

	// 清零
	// 防止其他地方误用了未初始化的无效数据
	// 防止对未初始化的数据调用Cortex_FreeFrame时可能出现错误
	memset(pBuffer, 0, nBufferSize);

	// 放入内存池
	m_Buffers.push_back(pBuffer);

#ifdef _DEBUG
	m_vDebugAllocTime.push_back(nAddFrameCount);
#endif

	sFxFrame* frame_array = (sFxFrame*)pBuffer;

	// 放入空闲队列
	for (unsigned int i = 0; i < nAddFrameCount; ++i)
	{
		PushToFreeList(frame_array + i);
	}
}

bool FxCortex::GetBodyDefs()
{
	// 在执行本函数时不能接收新的cortex数据
	bool bReceiveOld = GetReceiveCortexFrame();
	SetReceiveCortexFrame(false);

	// 重新获得BodyDefs将导致以前的所有数据作废
	// 释放以前的数据
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

	// 创建bodys
	m_bodys.resize(m_pBodyDefs->nBodyDefs);

	// 初始化marker数据
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

		// 一次性创建所有骨骼
		bones.resize(h.nSegments);
		for (int _b = 0; _b < h.nSegments; ++_b)
		{
			sFxBone& bone = bones[_b];

			bone.strBoneName = h.szSegmentNames[_b];
			bone.nParentBoneIndex = h.iParents[_b];
			
			//b++;
		}
	}

	// 恢复设置
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
	// 是否连接了动作捕捉软件
	if (!GetConnected())
	{
		return false;
	}

	// 是否能收到动作捕捉的帧数据
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

	// 手动做一次混合，确保actor里的骨骼矩阵是初始化过的
	//pActor->BlendByHand();

	//// 初始化游戏里actor的tpose的local matrix
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

	// cortex里模型的tpose
	sFrameOfData frameCortexTpose;
	memset(&frameCortexTpose, 0, sizeof(sFrameOfData));
	int retval = Cortex_CopyFrame(pFrameOfData, &frameCortexTpose);
	if (retval != RC_Okay)
	{
		CORE_TRACE("(FxCortex::CalcTposeTransformMatrix) Copy frame failed.");
		return false;
	}

	// 定义骨骼操作用到的临时变量
	FmMat4 mat_tmp;
	float px, py, pz, rx, ry, rz;

	for (int i = 0; i < frameCortexTpose.nBodies; ++i)
	{
		sFxBody& fxbody = m_bodys[i];

		sBodyData& body = frameCortexTpose.BodyData[i];

		// 骨骼
		BONES_ARRAY& bodyArray = fxbody.bonesArray;
		for (int _b = 0; _b < body.nSegments; ++_b)
		{
			sFxBone& bone = bodyArray[_b];
			FmMat4& mtxCaptureWorldMatrix = bone.mtxCaptureWorldMatrix;

			FmMatrixIdentity(&mtxCaptureWorldMatrix);

			tSegmentData& segment = body.Segments[_b];
			// 做位移
			// cortex是右手坐标系
			px = -(float)segment[0] * g_fMeterFactor;
			py = (float)segment[1] * g_fMeterFactor;
			pz = (float)segment[2] * g_fMeterFactor;

			D3DXMatrixTranslation(&mtxCaptureWorldMatrix, px, py, pz);

			// 不做缩放

			// 做旋转
			// cortex是右手坐标系
			rx = -(float)segment[3] * g_fRadianFactor;
			ry = -(float)segment[4] * g_fRadianFactor;
			rz = (float)segment[5] * g_fRadianFactor;

			// 旋转顺序: rx-ry-rz
			D3DXMatrixRotationX(&mat_tmp, rx);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
			FmMatrixRotationY(&mat_tmp, ry);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
			D3DXMatrixRotationZ(&mat_tmp, rz);
			D3DXMatrixMultiply(&mtxCaptureWorldMatrix, &mat_tmp, &mtxCaptureWorldMatrix);
		}
	}

	// 计算动作捕捉模型的骨骼世界变换矩阵
	for (size_t i = 0; i < m_bodys.size(); ++i)
	{
		BONES_ARRAY& bones= m_bodys[i].bonesArray;
		for (size_t _b = 0; _b < bones.size(); ++_b)
		{
			sFxBone& bone = bones[_b];
			if (bone.nParentBoneIndex > -1)
			{
				// 求骨骼的世界变换矩阵
				FmMat4& mat_parent = bones[bone.nParentBoneIndex].mtxCaptureWorldMatrix;
				D3DXMatrixMultiply(&bone.mtxCaptureWorldMatrix, 
					&bone.mtxCaptureWorldMatrix, &mat_parent);
			}
			// 求骨骼世界变换矩阵的逆矩阵
			FmMatrixInverse(&bone.mtxCaptureWorldMatrixInverse, NULL, 
				&bone.mtxCaptureWorldMatrix);
		}
	}

	// 对actor中被动作捕捉模型控制的骨骼初始化转换矩阵
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
	//		// 求cortex骨骼世界变换矩阵的逆
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

	// 在动作捕捉的模型里查找指定的骨骼
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

	// 在actor里查找指定的骨骼
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
				// 如果在动作捕捉模型里有对应的骨骼
				boneMap.strCaptureBoneName = szCaptureBoneName;
				boneMap.nCaptureBodyIndex = nBodyIndex;
				boneMap.nCaptureBoneIndex = nCaptureBoneIndex;
			}
			else
			{
				// 如果在动作捕捉模型里没有对应的骨骼
				// 只需要把名字设置为空就可以，body和bone索引值会被忽略
				boneMap.strCaptureBoneName = "";
			}
			break;
		}
	}

	//// 如果在动作捕捉模型里有该骨骼，而actor里没该骨骼
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
