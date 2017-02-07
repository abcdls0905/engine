//--------------------------------------------------------------------
// 文件名:		FxCortex.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年3月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __FXCORTEX_H__
#define __FXCORTEX_H__

#include "cortex.h"
#include "../public/module.h"
#include "../visual/i_visual.h"
#include "../utils/lock_util.h"
#include <list>
#include <vector>
#include <string>

// 与Cortex交互的模块
// MotionCapture简写成Capture

class Actor2;
//struct sFxFrame;
//struct sFxBone;
//struct sFxMarkerVertex;
//struct sFxBody;

class FxCortex: public IVisual
{
protected:
	// 帧
	struct sFxFrame
	{
		bool m_bRender;			// 该帧是否被渲染过
		sFrameOfData cortex_frame;		// cortex的帧数据
	};

	// marker数据
#define MARKER_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
	struct sFxMarkerVertex
	{
		D3DVECTOR pos;
		unsigned int diffuse;
	};

	// 骨骼数据
	struct sFxBone
	{
		sFxBone() 
		{ 
			nParentBoneIndex = 0; 
			FmMatrixIdentity(&mat);
		}

		std::string strBoneName;
		FmMat4 mat;
		int nParentBoneIndex;

		// 动作捕捉模型的Tpose世界矩阵
		FmMat4 mtxCaptureWorldMatrix;
		FmMat4 mtxCaptureWorldMatrixInverse;

		// 游戏里actor的tpose的local matrix
		//FmMat4 matActorLocalMatrix;

		// cortex里模型的tpose到游戏里tpose的映射矩阵
		//FmMat4 matTposeCortexToActor;
//#ifdef _DEBUG
//		FmMat4 debug_matTposeCortexInverse;
//#endif
//
//#ifdef _DEBUG
//		D3DVECTOR debug_vTranslation;
//		FmMat4 debug_local_mat;
//#endif
//
//		// 对应的actor骨骼名字
//		std::string strActorBoneName;
	};

	typedef std::vector<sFxBone> BONES_ARRAY;

	// actor骨骼和动作捕捉骨骼的对应关系表
	struct sBoneMap
	{
		std::string strActorBoneName;		// actor中骨骼的名字
		std::string strActorBoneParentName;	// actor中骨骼的父骨骼名字
		FmMat4 mtxActorBoneLocalMatrix;	// actor中骨骼的local矩阵

		std::string strCaptureBoneName;		// 动作捕捉的骨骼的名字
		unsigned int nCaptureBodyIndex;		// 表示在哪个body
		unsigned int nCaptureBoneIndex;		// 表示是body的哪个骨骼

		// 动作捕捉的模型tpose到actor的tpose的转换矩阵
		FmMat4 mtxTposeCaptureToActor;
	};
	typedef std::vector<sBoneMap> BONES_MAP_CONTAINER;

	// body
	// 对应Cortex软件里的一个MarkerSet
	struct sFxBody
	{
		sFxBody()
		{
			//nRootBoneIndex = 0;
			nMarkerOffset = 0;
			nMarkerCount = 0;
		}

		// 本body的bone array
		BONES_ARRAY bonesArray;
		//int nRootBoneIndex;

		// 本body的marker在总marker数组里的起始位置和总数
		unsigned int nMarkerOffset;
		unsigned int nMarkerCount;
	};

	typedef std::list<sFxFrame*> FrameList;

public:
	FxCortex();
	virtual ~FxCortex();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// 错误消息的回调函数
	static void CortexErrorMsgHandler(int iLevel, char *szMsg);
	// 帧信息的回调函数
	static void CortexFrameDataHandler(sFrameOfData* FrameOfData);

	// 连接Cortex
	bool ConnectCortex(const char* szMyIP, const char* szCortexIP);

	// 绑定的动画对象
	void SetActorBind(const PERSISTID& actor);
	PERSISTID GetActorBind() const;

	// 是否接收Cortex的帧消息
	bool GetReceiveCortexFrame() const { return m_bReceiveCortexFrame; }
	void SetReceiveCortexFrame(bool value) { m_bReceiveCortexFrame = value; }

	// 是否和Cortex连着
	bool GetConnected() const;

	// 从Cortex获得各身体部件的描述信息
	bool GetBodyDefs();

	// 渲染时是否需要插值
	bool GetInterpolation() const { return false;/*m_bInterpolation; 目前不需要插值*/ }
	void SetInterpolation(bool value) { m_bInterpolation = value; }

	// marker点的颜色
	void SetMarkerColorString(const char* value);
	std::string GetMarkerColorString() const;

	// 从cortex获取当前帧数据
	bool GetFrameOfData();

	// 计算从cortex的模型tpose变换到游戏模型tpose的变换矩阵
	// 必须在cortex的模型和游戏里的模型都处在tpose状态时才能调用
	bool CalcTposeTransformMatrix();

	// 设置动作捕捉模型和actor的骨骼对应关系
	bool SetBoneMap(const char* szCaptureBoneName, const char* szActorBoneName);

	void ClearBoneMap();

protected:
	// 释放资源
	// 参数bReleaseMemPool为true表示内存池也要释放
	// 为了提高效率，目前只在本实例销毁时才释放内存池
	void ReleaseResource(bool bReleaseMemPool = false);

	// 从等待处理的帧队列里取出一帧
	sFxFrame* PopFromWaitList();
	// 添加一个新帧到等待处理的帧队列里
	void PushToWaitList(sFxFrame* pFrame);

	// 从空闲帧队列里获得一个空帧
	sFxFrame* PopFromFreeList();
	// 添加一个空帧到空闲帧队列里
	void PushToFreeList(sFxFrame* pFrame);

	// 分配空闲帧
	void AllocFreeFrame(unsigned int nAddFrameCount);

	Actor2* InnerGetActor() const;

protected:
	// 绑定的动画对象
	PERSISTID m_ActorBind;

	// 是否和Cortex连着
	bool m_bConnected;

	// Cortex主机的信息
	sHostInfo m_CortexHostInfo;

	// 从Cortex获得的各身体部件的描述信息
	sBodyDefs* m_pBodyDefs;
	//BodyArray m_vBodys;

	// 当前正在渲染的两个关键帧
	sFxFrame* m_pPrevFrame;
	sFxFrame* m_pNextFrame;

	// 等待渲染的帧队列
	FrameList m_WaitRenderFrameData;
	// 空闲帧队列
	FrameList m_FreeFrameData;
	// 内存管理队列
	std::vector<char*> m_Buffers;

	// 从上个关键帧到现在的时间
	float m_fSecondsFromPrevFrame;
	// 两个关键帧间的总时长
	float m_fSecondsBetweenTwoFrames;

	// Cortex_sdk的网络传输是个另外的线程，有些地方需要加锁
	CLockUtil m_Lock;

	// cortex的IP
	std::string m_strCortexIP;

	// 所有body的marker数组
	int m_nMarkers;
	IDxDynamicVB* m_pMarkerDynamicVB;

	// body数组
	std::vector<sFxBody> m_bodys;

	// 是否接收Cortex的帧消息
	bool m_bReceiveCortexFrame;

	// 渲染时是否需要插值
	bool m_bInterpolation;

	// marker点的颜色
	unsigned int m_nMarkerColor;

	int m_nShader;

	// actor骨骼和动作捕捉骨骼的对应关系表
	BONES_MAP_CONTAINER m_bonesMap;

#ifdef _DEBUG
	// 调试分配内存的次数
	// debug版本测试结果是：
	//		通过预创建400个帧等各种实验发现:
	//			预创建多个帧并不影响实际使用帧数的峰值，对效率没多少影响
	//			每次帧不够时一次创建新帧的数量并不影响实际使用帧数的峰值，对效率没多少影响
	// debug版本，等待队列里的帧数峰值在300~400
	// 所以目前的设计是：
	//		预创建很少的量，每次创建的量也不多
	std::vector<unsigned int> m_vDebugAllocTime;
#endif
};

#endif // __FXCORTEX_H__
