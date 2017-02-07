//--------------------------------------------------------------------
// �ļ���:		MotionCaptureFrameReceiver.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��27��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "motion_capture_frame_receiver.h"
#include "helper.h"
#include "utility.h"
#include "../public/core_log.h"
#include "../visual/i_dx_dynamic.h"
#include "../visual/i_shader_system_2.h"
#include "../visual/dx_scene_batch.h"
#include "../public/core_mem.h"
#include "../visual/i_actor_2.h"
#include "../public/i_ini_file.h"
#include "../public/auto_mem.h"
#include <vector>

/// entity: MotionCaptureFrameReceiver
/// \brief ������׽֡������
/// \see �̳���IFrameReceiver
DECLARE_ENTITY("", MotionCaptureFrameReceiver, IFrameReceiver);
/// readonly: size_t BodyCount
/// \brief mvn��ɫ����
DECLARE_PROPERTY_GET(size_t, MotionCaptureFrameReceiver, BodyCount, GetBodyCount);
/// method: table GetBodyList()
/// \brief ��ö����б�
DECLARE_METHOD_T(MotionCaptureFrameReceiver, GetBodyList);
/// method: bool SetActorBind(size_t nMVNBodyID, const PERSISTID& actor);
/// \brief ����Ϸ�Ķ�������MVN��ɫ
/// \param nMVNBodyID MVN��ɫID
/// \param actor ��Ϸ��Ľ�ɫID
DECLARE_METHOD_2(bool, MotionCaptureFrameReceiver, SetActorBind, size_t, const PERSISTID&);
/// method: PERSISTID GetActorBind(size_t nMVNBodyID) const;
/// \brief ��ð󶨵�ָ��MVN����Ϸ��ɫID
/// \param nMVNBodyID MVN��ɫID
DECLARE_METHOD_1(PERSISTID, MotionCaptureFrameReceiver, GetActorBind, size_t);
/// method: bool DeleteActorBind(size_t nMVNBodyID);
/// \brief �������Ϸ�Ķ�������MVN��ɫ
/// \param nMVNBodyID MVN��ɫID
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, DeleteActorBind, size_t);
/// method: bool SetBoneMap(size_t nMVNBodyID, size_t nSegmentIndex, const char* szActorBoneName);
/// \brief ���ö�����׽ģ�ͺ�actor�Ĺ�����Ӧ��ϵ
/// \param nMVNBodyID MVN��ɫID
/// \param nSegmentIndex MVN��������
/// \param szActorBoneName actor2�Ĺ�����
DECLARE_METHOD_3(bool, MotionCaptureFrameReceiver, SetBoneMap, size_t, size_t, const char*);
/// method: const char* GetBoneMapByMVNBoneIndex(size_t nMVNBodyID, size_t nSegmentIndex);
/// \brief ���MVN�����󶨵�Actor2������
/// \param nMVNBodyID MVN��ɫID
/// \param nSegmentIndex MVN��������
DECLARE_METHOD_2(const char*, MotionCaptureFrameReceiver, GetBoneMapByMVNBoneIndex, size_t, size_t);
/// method: void ClearBoneMap(size_t nMVNBodyID);
/// \brief ��ն�����׽ģ�ͺ�actor�Ĺ�����Ӧ��ϵ
/// \param nMVNBodyID MVN��ɫID
DECLARE_METHOD_VOID_1(MotionCaptureFrameReceiver, ClearBoneMap, size_t);
/// method: bool GetActor2TposeReady();
/// \brief tpose�����Ƿ����
DECLARE_METHOD_0(bool, MotionCaptureFrameReceiver, GetActor2TposeReady);
/// method: bool LoadActor2TposeFromObject(PERSISTID actor2_tpose);
/// \brief ��tposeģ�ͼ���actor2�ı�׼tpose����
/// \param actor2_tpose ��tpose������actor2��ɫ
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, LoadActor2TposeFromObject, PERSISTID);
/// method: bool SaveActor2TposeToFile(const char* szFile);
/// \brief ����actor2��׼tpose���ݵ��ļ�
/// \param szFile tpose�����ļ�
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, SaveActor2TposeToFile, const char*);
/// method: bool LoadActor2TposeFromFile(const char* szFile);
/// \brief ���ļ�����actor2��׼tpose����
/// \param szFile tpose�����ļ�
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, LoadActor2TposeFromFile, const char*);

/// event: int on_new_role(object self, size_t role_id)
/// \brief ���½�ɫ
/// \param self ������
/// \param role_id ��ɫ��ID
DECLARE_EVENT(on_new_role);

/// event: int on_role_position(object self, size_t role_id, float px, float py, float pz)
/// \brief ��ɫ�����ƶ�
/// \param self ������
/// \param role_id ��ɫ��ID
/// \param px x����
/// \param py y����
/// \param pz z����
DECLARE_EVENT(on_role_position);

// MVN�������ɫ��
//#define MVN_MAX_BODY_COUNT 4
// MVN��һ����ɫ�����ĸ���
#define MVN_BONES_COUNT 23

// �������������ת����
static float g_fMeterFactor = 0.01f;
// �Ƕ�ת���ȵ�ϵ��
static float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

#define INI_SECTION_TPOSE "tpose"

#pragma pack(push, 1)

// ���ݱ�ͷ
struct sDatagramHeader
{
	char            id[6];				// "MXTP##"�������λ�ַ���ɵ����ֱ�ʾ��Ϣ����
	unsigned int	sample_counter;		// ��ǰ��׽��ţ��൱��֡���
	// �������
	// ���һ֡����ȫ��һ�������У����ֶ�Ϊ0x80
	// ���һ֡�����ڶ�������У���Ŵ�0��ʼ���������һ�����ĵ����Ϊ0x82
	unsigned char	datagram_counter;
	unsigned char   numItems;			// number of segments or points in the packet
	unsigned int    time_code;			// ʱ��
	unsigned char   avatarID;			// ģ�ص�ID
	char            reserve[7];
};

enum MessageType
{
	MSG_POS_EULER = 1,		// �����ŷ���ǵķ�ʽ
	MSG_POS_QUATE,			// �������Ԫ�صķ�ʽ
	MSG_POS_SELECT,			// û�õ�
	MSG_TAG_POS,			// ��ǵ�����

	MSG_SCALL_INFO = 10,	//������Ϣ   
	MSG_PROP_INFO,			//����������
	MSG_MAN_INFO,			//����ID
};

struct Msg_pos_euler
{
	int segmentID;
	float x;
	float y;
	float z;

	float rx;
	float ry;
	float rz;
};

struct Msg_pos_quate
{
	int segmentID;
	float x;
	float y;
	float z;

	float re;
	float i;
	float j;
	float k;
};

struct Msg_tag
{
	int tagID;
	int x;
	int y;
	int z;
};

#pragma pack(pop)

// ���ݳߴ�
// ��ΪMsg_pos_quate�����������ʹ��Msg_pos_quate�ĳߴ�
#define FRAME_DATA_SIZE sizeof(Msg_pos_quate) * MVN_BONES_COUNT
struct sMVNFrame
{
	sDatagramHeader header;
	char buffer[FRAME_DATA_SIZE];
};

// MVN�Ĺ�����Ϣ
struct sMVNBone
{
	sMVNBone()
	{
		memset(&frame_pos_euler, 0, sizeof(Msg_pos_euler));
		D3DXQuaternionIdentity(&qFrameBoneRotateToTpose);
		FmMatrixIdentity(&mtxFrameBoneRotateToTpose);
		nBindedActor2BoneIndex = -1;
	}

	Msg_pos_euler frame_pos_euler;				// ��ǰ֡�Ĺ�����Ϣ
	FmQuat qFrameBoneRotateToTpose;		// ��ǰ֡�����Tpose����ת��Ԫ��
	FmMat4 mtxFrameBoneRotateToTpose;		// ��ǰ֡�����Tpose����ת����
	FmVec3 vectFrameBoneTranslation;		// ��ǰ֡ģ�Ϳռ�λ�Ʊ仯

	int nBindedActor2BoneIndex;						// MVN������Ӧ��Actor2��������
};

// actor2�Ĺ�����Ϣ
struct sActor2Bone
{
	sActor2Bone() 
	{ 
		nBindedMVNBoneIndex = -1; 
		FmMatrixIdentity(&mtxBoneLocalMatrix);
		//FmMatrixIdentity(&mtxBoneLocalRotateMatrix);
		//FmMatrixIdentity(&mtxFrameBoneRotate);
		FmMatrixIdentity(&mtxFrameBone);
		FmMatrixIdentity(&mtxBoneTposeRotate);
	}
	std::string strBoneName;				// ��������
	std::string strBoneParentName;			// ����������
	int nBindedMVNBoneIndex;				// �󶨵�MVN����������-1��ʾû��

	FmMat4 mtxBoneLocalMatrix;			// ������Local�任����
	//FmMat4 mtxBoneLocalRotateMatrix;	// ������Local��ת����
	FmVec3 vectBoneLocalTranslation;	// ������Localλ��

	FmMat4 mtxFrameBone;				// �����ĵ�ǰ֡�任����
	//FmMat4 mtxFrameBoneRotate;			// �����ĵ�ǰ֡��ת����
	//FmVec3 vectFrameBoneTranslation;	// �����ĵ�ǰ֡λ��

	FmMat4 mtxBoneTposeRotate;			// Actor2������Tpose��ת����
};
typedef std::vector<sActor2Bone> Actor2Bones;

// ��ɫ����
struct sBody
{
	sBody()
	{ 
		bNotifyNewBody = true; 
		fRootBoneHeightOfMVN = 0; 
		bInitRootBoneHeightOfMVN = true;
		bReceivedFrame = false;
	}

	PERSISTID actor2_bind;								// ��ɫ�󶨵�actor2 ID

	Msg_pos_euler MVN_frame_pos_euler[MVN_BONES_COUNT];	// MVN��ǰ֡�Ĺ�����Ϣ
	Msg_pos_quate MVN_frame_pos_quate[MVN_BONES_COUNT];
	sMVNBone MVN_bones[MVN_BONES_COUNT];				// MVN��������
	Actor2Bones actor2_bones;							// actor2��������

	bool bNotifyNewBody;								// ֪ͨ���½�ɫ
	float fRootBoneHeightOfMVN;							// MVN��ɫ�������߶�
	bool bInitRootBoneHeightOfMVN;						// �Ƿ���Ҫ��ʼ���������߶�

	bool bReceivedFrame;								// �Ƿ��Ѿ��յ�֡����
};

inline float fx_ntohf(const void *p)
{
	u_long ltmp = 0;
	memcpy(&ltmp,p,4);
	ltmp = ntohl(ltmp);
	return *(float*)&ltmp;
}

inline int fx_ntohn(const void *p)
{

	u_long ltmp = 0;
	memcpy(&ltmp,p,4);
	ltmp = ntohl(ltmp);
	return *(int*)&ltmp;
}

MotionCaptureFrameReceiver::MotionCaptureFrameReceiver() 
{
	SetFrameSize(sizeof(sMVNFrame));
	//m_bInitMVNOrigin = true;
}

MotionCaptureFrameReceiver::~MotionCaptureFrameReceiver() 
{
	ClearBodies();
}

void MotionCaptureFrameReceiver::ClearBodies()
{
	BODY_CONTAINER::iterator ite_body;
	for (ite_body = m_Bodies.begin(); ite_body != m_Bodies.end(); ++ite_body)
	{
		CORE_DELETE((*ite_body).second);
	}
	m_Bodies.clear();
}

bool MotionCaptureFrameReceiver::SetActorBind(size_t nMVNBodyID, const PERSISTID& actor)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::SetActorBind) Invalid MVN Body ID.");
		return false;
	}

	IActor2* pActor = InnerGetActor(actor);
	if (NULL == pActor)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::SetActorBind) Invalid actor.");
		return false;
	}

	pBody->actor2_bind = actor;

	CVarList args;
	CVarList result;

	// ���actor�Ĺ����б�
	pBody->actor2_bones.clear();
	CVarList actorBoneList;
	args.Clear();
	bool ret = CHelper::MethodVarList(pActor, "GetBoneNodeList", args, actorBoneList);
	if (!ret || actorBoneList.GetCount() == 0)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::SetActorBind) Get BoneNodeList failed.");
		return false;
	}

	// ��������
	size_t nBoneCount = actorBoneList.GetCount();

	Actor2Bones& actor2_bones = pBody->actor2_bones;
	actor2_bones.resize(nBoneCount);
	for (size_t b = 0; b < nBoneCount; ++b)
	{
		sActor2Bone& bone = actor2_bones[b];

		// ��ù�����
		const char* szBoneName = actorBoneList.StringVal(b);
		bone.strBoneName = szBoneName;

		// ��ø�������
		args.Clear();
		args.AddString(szBoneName);
		bone.strBoneParentName = CHelper::MethodString(pActor, "GetBoneParentName", args);

		// ��ù����ֲ��任����
		ret = GetActor2LocalBoneMatrix(pActor, szBoneName, bone.mtxBoneLocalMatrix);
		Assert(ret);

		bone.vectBoneLocalTranslation.x = bone.mtxBoneLocalMatrix._41;
		bone.vectBoneLocalTranslation.y = bone.mtxBoneLocalMatrix._42;
		bone.vectBoneLocalTranslation.z = bone.mtxBoneLocalMatrix._43;
		//bone.mtxBoneLocalRotateMatrix = bone.mtxBoneLocalMatrix;
		//bone.mtxBoneLocalRotateMatrix._41 = 0;
		//bone.mtxBoneLocalRotateMatrix._42 = 0;
		//bone.mtxBoneLocalRotateMatrix._43 = 0;

		// �����Լ���ǰ�Ĺ��������ʼ��Tpose����
		ret = GetActor2BoneMatrix(pActor, szBoneName, bone.mtxBoneTposeRotate);
		Assert(ret);
	}

	// ʹ��actor2��ɫ�ı�׼tpose���½�ɫtpose
	UpdateActor2TposeBone(nMVNBodyID);

	//// ��¼�������ĸ߶�
	//if (nBoneCount > 0)
	//{
	//	args.Clear();
	//	const char* szRootBoneName = actorBoneList.StringVal(0);
	//	ret = CHelper::MethodVarList(pActor, "GetNodePosition", args<<szRootBoneName, result);
	//	if (ret)
	//	{
	//		body.fRootBoneHeightOfActor2 = result.FloatVal(1) - actor_pos.y;
	//	}
	//}

	return true;
}

bool MotionCaptureFrameReceiver::DeleteActorBind(size_t nMVNBodyID)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		return false;
	}

	ClearBoneMap(nMVNBodyID);
	pBody->actor2_bind = PERSISTID();
	return true;
}

PERSISTID MotionCaptureFrameReceiver::GetActorBind(size_t nMVNBodyID) const
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		return PERSISTID();
	}
	return pBody->actor2_bind;
}

IActor2* MotionCaptureFrameReceiver::InnerGetActor(PERSISTID actor2) const
{
	IEntity* pEntity = GetCore()->GetEntity(actor2);
	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Actor2"))
	{
		return NULL;
	}
	return (IActor2*)pEntity;
}

bool MotionCaptureFrameReceiver::SetBoneMap(size_t nMVNBodyID, size_t nSegmentIndex, const char* szActorBoneName)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody || nSegmentIndex >= MVN_BONES_COUNT || NULL == szActorBoneName)
	{
		Assert(0);
		return false;
	}

	Actor2Bones& actor2_bones = pBody->actor2_bones;
	size_t nBonesCount = actor2_bones.size();
	for (size_t i = 0; i < nBonesCount; ++i)
	{
		sActor2Bone& actor2_bone = actor2_bones[i];
		if (actor2_bone.strBoneName == szActorBoneName)
		{
			// ��Ҫ���������ӵ�MVN����
			sMVNBone& MVN_bone = pBody->MVN_bones[nSegmentIndex];
			// �����MVN�����Ѿ���Ӧ������Actor2������������Ӧ��ϵ
			if (MVN_bone.nBindedActor2BoneIndex > -1)
			{
				// ����Ǹ�actor2���������MVN����������
				actor2_bones[MVN_bone.nBindedActor2BoneIndex].nBindedMVNBoneIndex = -1;
			}

			// �����ǰactor2�����Ѿ���Ӧ������MVN������������Ӧ��ϵ
			if (actor2_bone.nBindedMVNBoneIndex > -1)
			{
				// ����Ǹ�MVN�����Ե�ǰactor2����������
				pBody->MVN_bones[actor2_bone.nBindedMVNBoneIndex].nBindedActor2BoneIndex = -1;
			}

			// ��ǰactor2����ָ�����MVN����
			actor2_bone.nBindedMVNBoneIndex = nSegmentIndex;
			// ���MVN����ָ��ǰactor2����
			MVN_bone.nBindedActor2BoneIndex = i;
			return true;
		}
	}
	return false;
}

const char* MotionCaptureFrameReceiver::GetBoneMapByMVNBoneIndex(size_t nMVNBodyID, size_t nSegmentIndex)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody || nSegmentIndex >= MVN_BONES_COUNT)
	{
		Assert(0);
		return "";
	}

	int nBindedActor2BoneIndex = pBody->MVN_bones[nSegmentIndex].nBindedActor2BoneIndex;
	if (nBindedActor2BoneIndex > -1)
	{
		return pBody->actor2_bones[nBindedActor2BoneIndex].strBoneName.c_str();
	}

	return "";
}

void MotionCaptureFrameReceiver::ClearBoneMap(size_t nMVNBodyID)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		Assert(0);
		return;
	}

	Actor2Bones& actor2_bones = pBody->actor2_bones;
	size_t nBonesCount = actor2_bones.size();
	for (size_t i = 0; i < nBonesCount; ++i)
	{
		sActor2Bone& actor2_bone = actor2_bones[i];
		actor2_bone.nBindedMVNBoneIndex = -1;
	}

	sMVNBone* MVN_bones = pBody->MVN_bones;
	for(int i = 0; i < MVN_BONES_COUNT; ++i)
	{
		sMVNBone& MVN_bone = MVN_bones[i];
		MVN_bone.nBindedActor2BoneIndex = -1;
	}
}

sBody* MotionCaptureFrameReceiver::AddBody(size_t nMVNBodyID)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (pBody)
	{
		Assert(0);
		return pBody;
	}

	pBody = CORE_NEW(sBody);
	m_Bodies.insert(std::make_pair(nMVNBodyID, pBody));
	return pBody;
}

void MotionCaptureFrameReceiver::ProcessFrameData()
{
	if (m_ProcessingFrameData.empty())
	{
		Assert(0);
		return;
	}

	BODY_CONTAINER::const_iterator ite_body = m_Bodies.begin();
	for (; ite_body != m_Bodies.end(); ++ite_body)
	{
		sBody* pBody = (*ite_body).second;
		pBody->bReceivedFrame = false;
	}

	FRAME_LIST::reverse_iterator ite_frame = m_ProcessingFrameData.rbegin();
	FRAME_LIST::const_reverse_iterator _rend_frame = m_ProcessingFrameData.rend();
	char* pFrameBuffer = NULL;
	for (; ite_frame != _rend_frame; ++ite_frame)
	{
		pFrameBuffer = *ite_frame;
		InnerProcessFrameData(pFrameBuffer);
	}
}

void MotionCaptureFrameReceiver::InnerProcessFrameData(char* pFrameBuffer)
{
	sMVNFrame* pFrame = (sMVNFrame*)pFrameBuffer;

	// ���ݱ�ͷ
	sDatagramHeader& head = pFrame->header;

	// �����Ϣ����
	char szMessageType[3];
	memcpy(szMessageType, head.id+4, 2);
	szMessageType[2] = 0;
	int nMessageType = atoi(szMessageType);

	head.sample_counter = (int)ntohl(head.sample_counter);
	head.time_code = (unsigned int)ntohl(head.time_code);

	sBody* pBody = GetBody(head.avatarID);

	// ��û�иý�ɫ
	if (NULL == pBody)
	{
		pBody = AddBody(head.avatarID);
		Assert(pBody != NULL);

		fm_movie::Util_RunCallback(this, "on_new_role", CVarList()<<head.avatarID);
	}

	// ����Ѿ��յ���������Ҫ�ٴ�����
	if (pBody->bReceivedFrame)
	{
		return;
	}
	pBody->bReceivedFrame = true;

	IVisual* pActor = InnerGetActor(pBody->actor2_bind);
	// ���û�а󶨽�ɫ
	if (NULL == pActor)
	{
		return;
	}

	const char *p = pFrame->buffer;
	switch (nMessageType)
	{
	case  MSG_POS_EULER:
		{
			// ���MVN������Ϣ
			Msg_pos_euler* MVN_frame_pos_euler = pBody->MVN_frame_pos_euler;
			memset(MVN_frame_pos_euler, 0, sizeof(Msg_pos_euler) * MVN_BONES_COUNT);
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				MVN_frame_pos_euler[i].segmentID = fx_ntohn(p);
				MVN_frame_pos_euler[i].x  = fx_ntohf(p+4);
				MVN_frame_pos_euler[i].y  = fx_ntohf(p+8);
				MVN_frame_pos_euler[i].z  = fx_ntohf(p+12);
				MVN_frame_pos_euler[i].rx = fx_ntohf(p+16);
				MVN_frame_pos_euler[i].ry = fx_ntohf(p+20);
				MVN_frame_pos_euler[i].rz = fx_ntohf(p+24);
				p += 28;
			}

			// ����MVN������Ϣ
			FmMat4 mat_rx, mat_ry, mat_rz;
			float px, py, pz, rx, ry, rz;
			sMVNBone* MVN_bones = pBody->MVN_bones;
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				sMVNBone& MVN_bone = MVN_bones[i];
				FmMat4& mtxFrameBoneRotateToTpose = MVN_bone.mtxFrameBoneRotateToTpose;
				FmVec3& vectFrameBoneTranslation = MVN_bone.vectFrameBoneTranslation;

				// ��������ϵת����
				rx = -MVN_frame_pos_euler[i].rx * g_fRadianFactor;
				ry = -MVN_frame_pos_euler[i].ry * g_fRadianFactor;
				rz = MVN_frame_pos_euler[i].rz * g_fRadianFactor;
				// ��ת˳��: rx-ry-rz
				D3DXMatrixRotationX(&mat_rx, rx);
				FmMatrixRotationY(&mat_ry, ry);
				D3DXMatrixRotationZ(&mat_rz, rz);
				mtxFrameBoneRotateToTpose = mat_rx * mat_ry * mat_rz;

				// ��������ϵת����
				px = -MVN_frame_pos_euler[i].x * g_fMeterFactor;
				py = MVN_frame_pos_euler[i].y * g_fMeterFactor;
				pz = MVN_frame_pos_euler[i].z* g_fMeterFactor;
				vectFrameBoneTranslation.x = px;
				vectFrameBoneTranslation.y = py;
				vectFrameBoneTranslation.z = pz;
			}

			UpdateActor2Bones(head.avatarID);
		}
		break;
	case MSG_POS_QUATE:
		{
			// ���MVN������Ϣ
			Msg_pos_quate* MVN_frame_pos_quate = pBody->MVN_frame_pos_quate;
			memset(MVN_frame_pos_quate, 0, sizeof(Msg_pos_quate) * MVN_BONES_COUNT);
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				MVN_frame_pos_quate[i].segmentID = fx_ntohn(p);
				MVN_frame_pos_quate[i].x  = fx_ntohf(p+4);
				MVN_frame_pos_quate[i].y  = fx_ntohf(p+8);
				MVN_frame_pos_quate[i].z  = fx_ntohf(p+12);
				MVN_frame_pos_quate[i].re = fx_ntohf(p+16);
				MVN_frame_pos_quate[i].i = fx_ntohf(p+20);
				MVN_frame_pos_quate[i].j = fx_ntohf(p+24);
				MVN_frame_pos_quate[i].k = fx_ntohf(p+28);
				p += 32;
			}

			// ����MVN������Ϣ
			float px, py, pz;
			sMVNBone* MVN_bones = pBody->MVN_bones;
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				sMVNBone& MVN_bone = MVN_bones[i];
				FmVec3& vectFrameBoneTranslation = MVN_bone.vectFrameBoneTranslation;

				// z���ϵ���������ϵ��x��ǰ��y����
				ConvertQuaternionFormMVNToActor2(MVN_bone.qFrameBoneRotateToTpose,
					MVN_frame_pos_quate[i].i, MVN_frame_pos_quate[i].j,
					MVN_frame_pos_quate[i].k, MVN_frame_pos_quate[i].re);

				D3DXMatrixRotationQuaternion(&MVN_bone.mtxFrameBoneRotateToTpose, 
					&MVN_bone.qFrameBoneRotateToTpose);

				// z���ϵ���������ϵ��x��ǰ��y����
				px = -MVN_frame_pos_quate[i].y;
				py = MVN_frame_pos_quate[i].z;
				pz = MVN_frame_pos_quate[i].x;

				vectFrameBoneTranslation.x = px;
				vectFrameBoneTranslation.y = py;
				vectFrameBoneTranslation.z = pz;
			}

			UpdateActor2Bones(head.avatarID);
		}
		break;
	case MSG_SCALL_INFO:
		break;
	case MSG_PROP_INFO:
		break;
	case MSG_MAN_INFO:
		break;
	default:
		break;
	}
}

void MotionCaptureFrameReceiver::UpdateActor2Bones(size_t nMVNBodyID)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		return;
	}

	FmVec3& pos_root_bone_MVN = pBody->MVN_bones[0].vectFrameBoneTranslation;

	// ��ʼ���������߶�
	if (pBody->bInitRootBoneHeightOfMVN)
	{
		pBody->bInitRootBoneHeightOfMVN = false;
		pBody->fRootBoneHeightOfMVN = pos_root_bone_MVN.y;
	}

	IVisual* pActor = InnerGetActor(pBody->actor2_bind);
	// ���û�а󶨽�ɫ�򲻴���
	if (NULL == pActor)
	{
		return;
	}

	// ģ���ƶ�����
	FmVec3 pos;
	pos.x = pos_root_bone_MVN.x;
	pos.y = pos_root_bone_MVN.y-pBody->fRootBoneHeightOfMVN;
	pos.z = pos_root_bone_MVN.z;

	// �洢���������Ϣ
	IVarTable *pVarTable = pActor->GetCustoms();
	if (pVarTable)
	{
		pVarTable->Assign("px", CVar(VTYPE_FLOAT, pos.x));
		pVarTable->Assign("py", CVar(VTYPE_FLOAT, pos.y));
		pVarTable->Assign("pz", CVar(VTYPE_FLOAT, pos.z));
	}

	// ����ģ�͵���ת�Ƕȶ��ƶ�����Ҳ������ת
	FmVec3 angle = pActor->GetAngle();
	FmMat4 mtxRoleRotate;
	FmMat4RotationYawPitchRoll(&mtxRoleRotate, angle.y, angle.x, angle.z);
	FmVec3TransformCoord(&pos, &pos, &mtxRoleRotate);

	// �����ƶ����굽�ű�
	fm_movie::Util_RunCallback(this, "on_role_position", CVarList() << nMVNBodyID
		<< pos.x << pos.y << pos.z);

	size_t nActor2BoneCount = pBody->actor2_bones.size();

	CVarList args;
	CVarList result;
	bool ret;
	FmMat4 mtx_tmp;
	// actor2�и�������ģ�Ϳռ�任����
	FmMat4 mtxActor2ParentBoneModelMatrix;
	for (size_t b = 0; b < nActor2BoneCount; ++b)
	{
		sActor2Bone& actor2_bone = pBody->actor2_bones[b];

		// ���actor�и������ı任����
		//args.Clear();
		//args.AddString(actor2_bone.strBoneParentName.c_str());
		//args.AddPointer(&mtxActor2ParentBoneModelMatrix);
		//ret = CHelper::MethodBool(pActor, "GetBoneMatrix", args);
		ret = GetActor2BoneMatrix(pActor, actor2_bone.strBoneParentName.c_str(), 
			mtxActor2ParentBoneModelMatrix);

		// û�и�����
		if (!ret)
		{
			FmMatrixIdentity(&mtxActor2ParentBoneModelMatrix);
		}

		// �������MVN��Ĺ���
		if (actor2_bone.nBindedMVNBoneIndex >= 0)
		{
			sMVNBone& MVN_bone = pBody->MVN_bones[actor2_bone.nBindedMVNBoneIndex];

			// ����MVN������任���任actor2�Ĺ���

			// ע�⣺
			//		���ڴ󲿷���������׹�����������β�һ������������Ҳ��һ����
			//		���ֱ��ʹ�ö�����׽��ɫ�Ĺ���λ�ñ任��Ϣ��
			//		��ʹ��Ϸ��Ľ�ɫ�������������
			//		�������Ӷ�����׽���������λ�Ʊ任����

			// ����actor2����ģ�Ϳռ����ת����
			actor2_bone.mtxFrameBone = actor2_bone.mtxBoneTposeRotate * 
				MVN_bone.mtxFrameBoneRotateToTpose;

			// localλ�ƾ������actor�и�������ģ�Ϳռ�任����
			// ����actor2����ģ�Ϳռ��λ�ƾ���
			FmVec3& local_pos = actor2_bone.vectBoneLocalTranslation;
			D3DXMatrixTranslation(&mtx_tmp, local_pos.x, local_pos.y, local_pos.z);
			D3DXMatrixMultiply(&mtx_tmp, &mtx_tmp, &mtxActor2ParentBoneModelMatrix);

			//actor2_bone.vectFrameBoneTranslation.x = mtx_tmp._41;
			//actor2_bone.vectFrameBoneTranslation.y = mtx_tmp._42;
			//actor2_bone.vectFrameBoneTranslation.z = mtx_tmp._43;

			//actor2_bone.mtxFrameBone = actor2_bone.mtxFrameBoneRotate;
			// �õ����յ�ģ�Ϳռ�任����
			actor2_bone.mtxFrameBone._41 = mtx_tmp._41;
			actor2_bone.mtxFrameBone._42 = mtx_tmp._42;
			actor2_bone.mtxFrameBone._43 = mtx_tmp._43;
		}
		else
		{
			// local�������actor�и������ı任���������ù����ı任����
			D3DXMatrixMultiply(&actor2_bone.mtxFrameBone, 
				&actor2_bone.mtxBoneLocalMatrix, &mtxActor2ParentBoneModelMatrix);
		}

		// ���ù������ݵ�actor2
		args.Clear();
		args.AddString(actor2_bone.strBoneName.c_str());
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; ++j)
			{
				args.AddFloat(actor2_bone.mtxFrameBone(i, j));
			}
		}
		//args.AddPointer(&actor2_bone.mtxFrameBone);
		ret = CHelper::MethodBool(pActor, "SetBoneMatrix", args);
		Assert(ret);
	}

	// ʹactor2�Ĺ���������Ч
	args.Clear();
	ret = CHelper::MethodBool(pActor, "UpdateBoneMatrix", args);
	Assert(ret);
}

void MotionCaptureFrameReceiver::ConvertQuaternionFormMVNToActor2(FmQuat& quaternion_actor2,
											  float mvn_qx, float mvn_qy, float mvn_qz, float mvn_qw)
{
	// mvn����Ԫ������������ϵ��z�ᳯ�ϣ�x�ᳯǰ��y�ᳯ��
	// mvn��ת���Ӧ��dx��ת��Ϊ��qx=-mvn_qy, qy=mvn_qz, qz=mvn_qx
	// ����ת���֣���ת��ȡ������qx,qy,qzȫȡ����qx=mvn_qy, qy=-mvn_qz, qz=-mvn_qx
	// ��Ϊ������Ľ�ɫҪת180�ȣ�����qx��qz��ȡ����qx=-mvn_qy, qy=-mvn_qz, qz=mvn_qx
	quaternion_actor2.x = -mvn_qy;
	quaternion_actor2.y = -mvn_qz;
	quaternion_actor2.z = mvn_qx;
	quaternion_actor2.w = mvn_qw;
}

void MotionCaptureFrameReceiver::UpdateActor2TposeBone(size_t nMVNBodyID)
{
	sBody* pBody = GetBody(nMVNBodyID);
	if (NULL == pBody)
	{
		Assert(0);
		return;
	}

	IVisual* pActor = InnerGetActor(pBody->actor2_bind);
	// ���û�а󶨽�ɫ�򲻴���
	if (NULL == pActor)
	{
		Assert(0);
		return;
	}

	Actor2Bones& actor2_bones = pBody->actor2_bones;
	size_t nActor2BonesCount = actor2_bones.size();
	for (size_t b = 0; b < nActor2BonesCount; ++b)
	{
		sActor2Bone& actor2_bone = actor2_bones[b];

		// ���Ҷ�Ӧ��tpose����
		ACTOR2TPOSE::iterator ite = m_actor2Tpose.find(actor2_bone.strBoneName);
		if (m_actor2Tpose.end() == ite)
		{
			continue;
		}
		actor2_bone.mtxBoneTposeRotate = ite->second;
	}
}

bool MotionCaptureFrameReceiver::GetActor2TposeReady()
{
	return !m_actor2Tpose.empty();
}

bool MotionCaptureFrameReceiver::LoadActor2TposeFromObject(PERSISTID actor2_tpose)
{
	IVisual* pActor = InnerGetActor(actor2_tpose);
	if (NULL == pActor)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromObject) Invalid actor.");
		return false;
	}

	// ���actor�Ĺ����б�
	CVarList args;
	CVarList actorBoneList;
	args.Clear();
	bool ret = CHelper::MethodVarList(pActor, "GetBoneNodeList", args, actorBoneList);
	if (!ret || actorBoneList.GetCount() == 0)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromObject) Get BoneNodeList failed.");
		return false;
	}

	// ��������
	size_t nBoneCount = actorBoneList.GetCount();
	for (size_t b = 0; b < nBoneCount; ++b)
	{
		// ��ù�����
		const char* szBoneName = actorBoneList.StringVal(b);

		std::pair<ACTOR2TPOSE::iterator, bool> pair_ret = 
			m_actor2Tpose.insert(std::make_pair(szBoneName, FmMat4()));
		FmMat4& mat = pair_ret.first->second;

		// ��ù���Tpose����
		//args.Clear();
		//args.AddString(szBoneName);
		//args.AddPointer(&mat);
		//ret = CHelper::MethodBool(pActor, "GetBoneMatrix", args);
		ret = GetActor2BoneMatrix(pActor, szBoneName, mat);
		Assert(ret);

		// ȥ��λ����Ϣ
		mat._41 = 0;
		mat._42 = 0;
		mat._43 = 0;
	}
	return true;
}

bool MotionCaptureFrameReceiver::SaveActor2TposeToFile(const char* szFile)
{
	std::string path = GetCore()->GetResourcePath();
	path = path + szFile;

	IIniFile* pIni = GetCore()->CreateIniFile(path.c_str());

	pIni->AddSection(INI_SECTION_TPOSE);

	char buf[256];
	ACTOR2TPOSE::iterator ite = m_actor2Tpose.begin();
	while (ite != m_actor2Tpose.end())
	{
		const std::string& strBoneName = (*ite).first;
		FmMat4& mat = (*ite).second;

		sprintf(buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f",
			mat._11, mat._12, mat._13, mat._21, mat._22, mat._23,
			mat._31, mat._32, mat._33);
		pIni->AddString(INI_SECTION_TPOSE, strBoneName.c_str(), buf);

		++ite;
	}

	pIni->SetFileName(path.c_str());
	pIni->SaveToFile();

	pIni->Release();
	return true;
}

bool MotionCaptureFrameReceiver::LoadActor2TposeFromFile(const char* szFile)
{
	std::string path = GetCore()->GetResourcePath();
	path = path + szFile;

	IIniFile* pIni = GetCore()->CreateIniFile(path.c_str());

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromFile) Load tpose file failed.");
		CORE_TRACE(path.c_str());
		return false;
	}

	if (!pIni->FindSection(INI_SECTION_TPOSE))
	{
		pIni->Release();
		CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromFile) No tpose section.");
		CORE_TRACE(path.c_str());
		return false;
	}

	m_actor2Tpose.clear();

	CVarList item_list;
	size_t item_count = pIni->GetItemList(INI_SECTION_TPOSE, item_list);
	for (size_t i = 0; i < item_count; ++i)
	{
		const char* key = item_list.StringVal(i);
		Assert(key);
		const char* value = pIni->ReadString(INI_SECTION_TPOSE, key, "");
		if (value[0] == '\0')
		{
			CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromFile) value is null.");
			CORE_TRACE(key);
			continue;
		}

		CVarList float_list;
        fm_movie::ext_split_string(float_list, value, ',');
		size_t float_count = float_list.GetCount();
		if (float_count != 9)
		{
			CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromFile) value is not matrix.");
			CORE_TRACE(key);
			continue;
		}

		std::pair<ACTOR2TPOSE::iterator, bool> pair_ret = 
			m_actor2Tpose.insert(std::make_pair(key, FmMat4()));
		FmMat4& mat = pair_ret.first->second;
		FmMatrixIdentity(&mat);

		mat._11 = (float)atof(float_list.StringVal(0));
		mat._12 = (float)atof(float_list.StringVal(1));
		mat._13 = (float)atof(float_list.StringVal(2));
		mat._21 = (float)atof(float_list.StringVal(3));
		mat._22 = (float)atof(float_list.StringVal(4));
		mat._23 = (float)atof(float_list.StringVal(5));
		mat._31 = (float)atof(float_list.StringVal(6));
		mat._32 = (float)atof(float_list.StringVal(7));
		mat._33 = (float)atof(float_list.StringVal(8));
	}

	pIni->Release();

	return true;
}

bool MotionCaptureFrameReceiver::GetActor2BoneMatrix(IEntity* pActor, 
											   const char* szBoneName, FmMat4& mat)
{
	if (NULL == pActor || NULL == szBoneName)
	{
		Assert(0);
		return false;
	}

	CVarList args, result;
	args.AddString(szBoneName);

	bool ret = CHelper::MethodVarList(pActor, "GetBoneMatrix", args, result);
	if (!ret || result.GetCount() != 17)
	{
		return false;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat(i, j) = result.FloatVal(i * 4 + j + 1);
		}
	}
	return true;
}

bool MotionCaptureFrameReceiver::GetActor2LocalBoneMatrix(IEntity* pActor, 
														  const char* szBoneName, FmMat4& mat)
{
	if (NULL == pActor || NULL == szBoneName)
	{
		Assert(0);
		return false;
	}

	CVarList args, result;
	args.AddString(szBoneName);

	bool ret = CHelper::MethodVarList(pActor, "GetBoneLocalMatrix", args, result);
	if (!ret || result.GetCount() != 17)
	{
		return false;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat(i, j) = result.FloatVal(i * 4 + j + 1);
		}
	}
	return true;
}

//sBody* MotionCaptureFrameReceiver::GetBody(size_t nMVNBodyID)
//{
//	BODY_CONTAINER::iterator ite_body = m_Bodies.find(nMVNBodyID);
//	if (m_Bodies.end() == ite_body)
//	{
//		CORE_TRACE("(MotionCaptureFrameReceiver::GetBody) Invalid MVN Body ID.");
//		return NULL;
//	}
//
//	return (*ite_body).second;
//}

sBody* MotionCaptureFrameReceiver::GetBody(size_t nMVNBodyID) const
{
	BODY_CONTAINER::const_iterator ite_body = m_Bodies.find(nMVNBodyID);
	if (m_Bodies.end() == ite_body)
	{
		//CORE_TRACE("(MotionCaptureFrameReceiver::GetBody) Invalid MVN Body ID.");
		return NULL;
	}

	return (*ite_body).second;
}

size_t MotionCaptureFrameReceiver::GetBodyCount() const
{
	return m_Bodies.size();
}

int MotionCaptureFrameReceiver::GetBodyList(const IVarList& args, 
											IVarList& result) const
{
	result.Clear();

	BODY_CONTAINER::const_iterator ite_body;
	for (ite_body = m_Bodies.begin(); ite_body != m_Bodies.end(); ++ite_body)
	{
		result.AddInt((*ite_body).first);
	}

	return result.GetCount();
}

void MotionCaptureFrameReceiver::Reset()
{
	ClearBodies();
}
