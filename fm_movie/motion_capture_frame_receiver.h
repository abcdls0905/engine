//--------------------------------------------------------------------
// 文件名:		MotionCaptureFrameReceiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月27日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __MotionCaptureFrameReceiver_H__
#define __MotionCaptureFrameReceiver_H__

#include "i_frame_receiver.h"
#include "../visual/dx_header.h"
#include <map>

// 动作捕捉帧接收器

class IActor2;
struct sBody;

class MotionCaptureFrameReceiver: public IFrameReceiver
{
public:
	MotionCaptureFrameReceiver();
	virtual ~MotionCaptureFrameReceiver();

	//virtual void Update(float seconds);
	//virtual void Realize(float offset_seconds) {}

	//// 开始监听MVN发来的数据
	//bool StartListen(const char* szMyIP, int nPort);
	//// 停止监听
	//bool StopListen();

	// 获得mvn角色个数
	size_t GetBodyCount() const;
	int GetBodyList(const IVarList& args, IVarList& result) const;

	// 绑定游戏的动画对象到MVN角色
	bool SetActorBind(size_t nMVNBodyID, const PERSISTID& actor);
	PERSISTID GetActorBind(size_t nMVNBodyID) const;
	bool DeleteActorBind(size_t nMVNBodyID);

	// 设置动作捕捉模型和actor的骨骼对应关系
	bool SetBoneMap(size_t nMVNBodyID, size_t nSegmentIndex, 
		const char* szActorBoneName);
	// 获得MVN骨骼绑定的Actor2骨骼名
	const char* GetBoneMapByMVNBoneIndex(size_t nMVNBodyID, size_t nSegmentIndex);
	// 清空骨骼对应表
	void ClearBoneMap(size_t nMVNBodyID);

	// tpose数据是否就绪
	bool GetActor2TposeReady();
	// 从tpose模型加载actor2的标准tpose数据
	bool LoadActor2TposeFromObject(PERSISTID actor2_tpose);
	// 保存actor2标准tpose数据到文件
	bool SaveActor2TposeToFile(const char* szFile);
	// 从文件加载actor2标准tpose数据
	bool LoadActor2TposeFromFile(const char* szFile);

	virtual void Reset();

private:
	virtual void ProcessFrameData();

private:
	void InnerProcessFrameData(char* pFrameBuffer);
	IActor2* InnerGetActor(PERSISTID actor2) const;
	//sBody* GetBody(size_t nMVNBodyID);
	sBody* GetBody(size_t nMVNBodyID) const;
	sBody* AddBody(size_t nMVNBodyID);

	void UpdateActor2Bones(size_t nMVNBodyID);
	void ConvertQuaternionFormMVNToActor2(FmQuat& quaternion_actor2,
		float mvn_qx, float mvn_qy, float mvn_qz, float mvn_qw);
	// 被控制的角色使用actor2角色的标准tpose
	void UpdateActor2TposeBone(size_t nMVNBodyID);

	bool GetActor2BoneMatrix(IEntity* pActor, const char* szBoneName, 
		FmMat4& mat);
	bool GetActor2LocalBoneMatrix(IEntity* pActor, const char* szBoneName, 
		FmMat4& mat);

	void ClearBodies();

private:
	typedef std::map<int, sBody*> BODY_CONTAINER;
	BODY_CONTAINER m_Bodies;

	//sBody m_bodies[MVN_MAX_BODY_COUNT];

	typedef std::map<std::string, FmMat4> ACTOR2TPOSE;
	ACTOR2TPOSE m_actor2Tpose;

	// MVN的原点在游戏中的坐标
	//FmVec3 m_posMVNOrigin;
	// 是否初始化过MVN的原点坐标
	//bool m_bInitMVNOrigin;
};

#endif // __MotionCaptureFrameReceiver_H__
