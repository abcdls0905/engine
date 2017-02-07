//--------------------------------------------------------------------
// 文件名:		FxVicon.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年5月10日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __FXVICON_H__
#define __FXVICON_H__

#include "client.h"
#include "../public/module.h"
#include "../visual/i_visual.h"
#include "../utils/lock_util.h"
#include <list>
#include <vector>
#include <string>

// 与Vicon交互的模块

using namespace ViconDataStreamSDK::CPP;

class Actor2;
//struct sFxFrame;
//struct sFxBone;
//struct sFxMarkerVertex;
//struct sFxBody;

class FxVicon: public IVisual
{
protected:
	//// 帧
	//struct sFxFrame
	//{
	//	bool m_bRender;			// 该帧是否被渲染过
	//	sFrameOfData cortex_frame;		// cortex的帧数据
	//};

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
#ifdef _DEBUG
		D3DVECTOR debug_vTranslation;
		FmMat4 debug_local_mat;
#endif
		// 游戏里actor的tpose的local matrix
		FmMat4 matActorLocalMatrix;

		// vicon里模型的tpose到游戏里tpose的映射矩阵
		FmMat4 matTposeViconToActor;
	};

	typedef std::vector<sFxBone> BONES_ARRAY;

	struct sFxMarker
	{
		std::string strMarkerName;
	};
	typedef std::vector<sFxMarker> MARKER_ARRAY;

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

		std::string strBodyName;

		// 本body的bone array
		BONES_ARRAY bonesArray;
		//int nRootBoneIndex;

		// 本body的marker array
		MARKER_ARRAY markersArray;

		// 本body的marker在总marker数组里的起始位置和总数
		unsigned int nMarkerOffset;
		unsigned int nMarkerCount;
	};
	//typedef std::list<sFxFrame*> FrameList;

public:
	FxVicon();
	virtual ~FxVicon();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// 连接Vicon
	bool ConnectVicon(const char* szViconIP);

	// 绑定的动画对象
	void SetActorBind(const PERSISTID& actor);
	PERSISTID GetActorBind() const;

	// 是否接收Vicon的帧消息
	bool GetReceiveViconFrame() const { return m_bReceiveViconFrame; }
	void SetReceiveViconFrame(bool value) { m_bReceiveViconFrame = value; }

	// 是否和Cortex连着
	bool GetConnected() const;

	// 获得各部件的描述信息
	bool GetBodyDefs();

	// marker点的颜色
	void SetMarkerColorString(const char* value);
	std::string GetMarkerColorString() const;

	// 获取当前帧数据
	bool GetFrameOfData();

	// 计算从cortex的模型tpose变换到游戏模型tpose的变换矩阵
	// 必须在cortex的模型和游戏里的模型都处在tpose状态时才能调用
	bool CalcTposeTransformMatrix();

protected:
	// 释放资源
	//void ReleaseResource();

	Actor2* InnerGetActor() const;

protected:
	// 绑定的动画对象
	PERSISTID m_ActorBind;

	// Vicon client class
	Client m_ViconClient;

	//// 从Cortex获得的各身体部件的描述信息
	//sBodyDefs* m_pBodyDefs;
	////BodyArray m_vBodys;

	// vicon的IP
	std::string m_strViconIP;

	// 所有body的marker数组
	int m_nMarkers;
	IDxDynamicVB* m_pMarkerDynamicVB;

	// body数组
	std::vector<sFxBody> m_bodys;

	// 是否接收Vicon的帧消息
	bool m_bReceiveViconFrame;

	// marker点的颜色
	unsigned int m_nMarkerColor;

	int m_nShader;

	// 当前帧序号
	int m_nFrameNumber;
};

#endif // __FXVICON_H__
