//--------------------------------------------------------------------
// �ļ���:		MotionCaptureFrameReceiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��27��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __MotionCaptureFrameReceiver_H__
#define __MotionCaptureFrameReceiver_H__

#include "i_frame_receiver.h"
#include "../visual/dx_header.h"
#include <map>

// ������׽֡������

class IActor2;
struct sBody;

class MotionCaptureFrameReceiver: public IFrameReceiver
{
public:
	MotionCaptureFrameReceiver();
	virtual ~MotionCaptureFrameReceiver();

	//virtual void Update(float seconds);
	//virtual void Realize(float offset_seconds) {}

	//// ��ʼ����MVN����������
	//bool StartListen(const char* szMyIP, int nPort);
	//// ֹͣ����
	//bool StopListen();

	// ���mvn��ɫ����
	size_t GetBodyCount() const;
	int GetBodyList(const IVarList& args, IVarList& result) const;

	// ����Ϸ�Ķ�������MVN��ɫ
	bool SetActorBind(size_t nMVNBodyID, const PERSISTID& actor);
	PERSISTID GetActorBind(size_t nMVNBodyID) const;
	bool DeleteActorBind(size_t nMVNBodyID);

	// ���ö�����׽ģ�ͺ�actor�Ĺ�����Ӧ��ϵ
	bool SetBoneMap(size_t nMVNBodyID, size_t nSegmentIndex, 
		const char* szActorBoneName);
	// ���MVN�����󶨵�Actor2������
	const char* GetBoneMapByMVNBoneIndex(size_t nMVNBodyID, size_t nSegmentIndex);
	// ��չ�����Ӧ��
	void ClearBoneMap(size_t nMVNBodyID);

	// tpose�����Ƿ����
	bool GetActor2TposeReady();
	// ��tposeģ�ͼ���actor2�ı�׼tpose����
	bool LoadActor2TposeFromObject(PERSISTID actor2_tpose);
	// ����actor2��׼tpose���ݵ��ļ�
	bool SaveActor2TposeToFile(const char* szFile);
	// ���ļ�����actor2��׼tpose����
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
	// �����ƵĽ�ɫʹ��actor2��ɫ�ı�׼tpose
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

	// MVN��ԭ������Ϸ�е�����
	//FmVec3 m_posMVNOrigin;
	// �Ƿ��ʼ����MVN��ԭ������
	//bool m_bInitMVNOrigin;
};

#endif // __MotionCaptureFrameReceiver_H__
