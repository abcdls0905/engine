//--------------------------------------------------------------------
// 文件名:		MotionCaptureFrameReceiver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月27日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief 动作捕捉帧接收器
/// \see 继承自IFrameReceiver
DECLARE_ENTITY("", MotionCaptureFrameReceiver, IFrameReceiver);
/// readonly: size_t BodyCount
/// \brief mvn角色个数
DECLARE_PROPERTY_GET(size_t, MotionCaptureFrameReceiver, BodyCount, GetBodyCount);
/// method: table GetBodyList()
/// \brief 获得动作列表
DECLARE_METHOD_T(MotionCaptureFrameReceiver, GetBodyList);
/// method: bool SetActorBind(size_t nMVNBodyID, const PERSISTID& actor);
/// \brief 绑定游戏的动画对象到MVN角色
/// \param nMVNBodyID MVN角色ID
/// \param actor 游戏里的角色ID
DECLARE_METHOD_2(bool, MotionCaptureFrameReceiver, SetActorBind, size_t, const PERSISTID&);
/// method: PERSISTID GetActorBind(size_t nMVNBodyID) const;
/// \brief 获得绑定到指定MVN的游戏角色ID
/// \param nMVNBodyID MVN角色ID
DECLARE_METHOD_1(PERSISTID, MotionCaptureFrameReceiver, GetActorBind, size_t);
/// method: bool DeleteActorBind(size_t nMVNBodyID);
/// \brief 解除绑定游戏的动画对象到MVN角色
/// \param nMVNBodyID MVN角色ID
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, DeleteActorBind, size_t);
/// method: bool SetBoneMap(size_t nMVNBodyID, size_t nSegmentIndex, const char* szActorBoneName);
/// \brief 设置动作捕捉模型和actor的骨骼对应关系
/// \param nMVNBodyID MVN角色ID
/// \param nSegmentIndex MVN骨骼索引
/// \param szActorBoneName actor2的骨骼名
DECLARE_METHOD_3(bool, MotionCaptureFrameReceiver, SetBoneMap, size_t, size_t, const char*);
/// method: const char* GetBoneMapByMVNBoneIndex(size_t nMVNBodyID, size_t nSegmentIndex);
/// \brief 获得MVN骨骼绑定的Actor2骨骼名
/// \param nMVNBodyID MVN角色ID
/// \param nSegmentIndex MVN骨骼索引
DECLARE_METHOD_2(const char*, MotionCaptureFrameReceiver, GetBoneMapByMVNBoneIndex, size_t, size_t);
/// method: void ClearBoneMap(size_t nMVNBodyID);
/// \brief 清空动作捕捉模型和actor的骨骼对应关系
/// \param nMVNBodyID MVN角色ID
DECLARE_METHOD_VOID_1(MotionCaptureFrameReceiver, ClearBoneMap, size_t);
/// method: bool GetActor2TposeReady();
/// \brief tpose数据是否就绪
DECLARE_METHOD_0(bool, MotionCaptureFrameReceiver, GetActor2TposeReady);
/// method: bool LoadActor2TposeFromObject(PERSISTID actor2_tpose);
/// \brief 从tpose模型加载actor2的标准tpose数据
/// \param actor2_tpose 做tpose动作的actor2角色
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, LoadActor2TposeFromObject, PERSISTID);
/// method: bool SaveActor2TposeToFile(const char* szFile);
/// \brief 保存actor2标准tpose数据到文件
/// \param szFile tpose配置文件
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, SaveActor2TposeToFile, const char*);
/// method: bool LoadActor2TposeFromFile(const char* szFile);
/// \brief 从文件加载actor2标准tpose数据
/// \param szFile tpose配置文件
DECLARE_METHOD_1(bool, MotionCaptureFrameReceiver, LoadActor2TposeFromFile, const char*);

/// event: int on_new_role(object self, size_t role_id)
/// \brief 有新角色
/// \param self 本对象
/// \param role_id 角色的ID
DECLARE_EVENT(on_new_role);

/// event: int on_role_position(object self, size_t role_id, float px, float py, float pz)
/// \brief 角色坐标移动
/// \param self 本对象
/// \param role_id 角色的ID
/// \param px x坐标
/// \param py y坐标
/// \param pz z坐标
DECLARE_EVENT(on_role_position);

// MVN里的最大角色数
//#define MVN_MAX_BODY_COUNT 4
// MVN里一个角色骨骼的个数
#define MVN_BONES_COUNT 23

// 把数据里的厘米转成米
static float g_fMeterFactor = 0.01f;
// 角度转弧度的系数
static float g_fRadianFactor = 1.0f / 360.0f * FM_PIf * 2.0f;

#define INI_SECTION_TPOSE "tpose"

#pragma pack(push, 1)

// 数据报头
struct sDatagramHeader
{
	char            id[6];				// "MXTP##"，最后两位字符组成的数字表示消息类型
	unsigned int	sample_counter;		// 当前捕捉序号，相当于帧序号
	// 报文序号
	// 如果一帧数据全在一个报文中，该字段为0x80
	// 如果一帧数据在多个报文中，序号从0开始计数，最后一个报文的序号为0x82
	unsigned char	datagram_counter;
	unsigned char   numItems;			// number of segments or points in the packet
	unsigned int    time_code;			// 时间
	unsigned char   avatarID;			// 模特的ID
	char            reserve[7];
};

enum MessageType
{
	MSG_POS_EULER = 1,		// 坐标和欧拉角的方式
	MSG_POS_QUATE,			// 坐标和四元素的方式
	MSG_POS_SELECT,			// 没用到
	MSG_TAG_POS,			// 标记的坐标

	MSG_SCALL_INFO = 10,	//缩放信息   
	MSG_PROP_INFO,			//骨骼的名字
	MSG_MAN_INFO,			//人物ID
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

// 数据尺寸
// 因为Msg_pos_quate最大，所以这里使用Msg_pos_quate的尺寸
#define FRAME_DATA_SIZE sizeof(Msg_pos_quate) * MVN_BONES_COUNT
struct sMVNFrame
{
	sDatagramHeader header;
	char buffer[FRAME_DATA_SIZE];
};

// MVN的骨骼信息
struct sMVNBone
{
	sMVNBone()
	{
		memset(&frame_pos_euler, 0, sizeof(Msg_pos_euler));
		D3DXQuaternionIdentity(&qFrameBoneRotateToTpose);
		FmMatrixIdentity(&mtxFrameBoneRotateToTpose);
		nBindedActor2BoneIndex = -1;
	}

	Msg_pos_euler frame_pos_euler;				// 当前帧的骨骼信息
	FmQuat qFrameBoneRotateToTpose;		// 当前帧相对于Tpose的旋转四元数
	FmMat4 mtxFrameBoneRotateToTpose;		// 当前帧相对于Tpose的旋转矩阵
	FmVec3 vectFrameBoneTranslation;		// 当前帧模型空间位移变化

	int nBindedActor2BoneIndex;						// MVN骨骼对应的Actor2骨骼索引
};

// actor2的骨骼信息
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
	std::string strBoneName;				// 骨骼名字
	std::string strBoneParentName;			// 父骨骼名字
	int nBindedMVNBoneIndex;				// 绑定的MVN骨骼索引，-1表示没绑定

	FmMat4 mtxBoneLocalMatrix;			// 骨骼的Local变换矩阵
	//FmMat4 mtxBoneLocalRotateMatrix;	// 骨骼的Local旋转矩阵
	FmVec3 vectBoneLocalTranslation;	// 骨骼的Local位移

	FmMat4 mtxFrameBone;				// 骨骼的当前帧变换矩阵
	//FmMat4 mtxFrameBoneRotate;			// 骨骼的当前帧旋转矩阵
	//FmVec3 vectFrameBoneTranslation;	// 骨骼的当前帧位移

	FmMat4 mtxBoneTposeRotate;			// Actor2骨骼的Tpose旋转矩阵
};
typedef std::vector<sActor2Bone> Actor2Bones;

// 角色数据
struct sBody
{
	sBody()
	{ 
		bNotifyNewBody = true; 
		fRootBoneHeightOfMVN = 0; 
		bInitRootBoneHeightOfMVN = true;
		bReceivedFrame = false;
	}

	PERSISTID actor2_bind;								// 角色绑定的actor2 ID

	Msg_pos_euler MVN_frame_pos_euler[MVN_BONES_COUNT];	// MVN当前帧的骨骼信息
	Msg_pos_quate MVN_frame_pos_quate[MVN_BONES_COUNT];
	sMVNBone MVN_bones[MVN_BONES_COUNT];				// MVN骨骼数据
	Actor2Bones actor2_bones;							// actor2骨骼数据

	bool bNotifyNewBody;								// 通知有新角色
	float fRootBoneHeightOfMVN;							// MVN角色根骨骼高度
	bool bInitRootBoneHeightOfMVN;						// 是否需要初始化根骨骼高度

	bool bReceivedFrame;								// 是否已经收到帧数据
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

	// 获得actor的骨骼列表
	pBody->actor2_bones.clear();
	CVarList actorBoneList;
	args.Clear();
	bool ret = CHelper::MethodVarList(pActor, "GetBoneNodeList", args, actorBoneList);
	if (!ret || actorBoneList.GetCount() == 0)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::SetActorBind) Get BoneNodeList failed.");
		return false;
	}

	// 骨骼总数
	size_t nBoneCount = actorBoneList.GetCount();

	Actor2Bones& actor2_bones = pBody->actor2_bones;
	actor2_bones.resize(nBoneCount);
	for (size_t b = 0; b < nBoneCount; ++b)
	{
		sActor2Bone& bone = actor2_bones[b];

		// 获得骨骼名
		const char* szBoneName = actorBoneList.StringVal(b);
		bone.strBoneName = szBoneName;

		// 获得父骨骼名
		args.Clear();
		args.AddString(szBoneName);
		bone.strBoneParentName = CHelper::MethodString(pActor, "GetBoneParentName", args);

		// 获得骨骼局部变换矩阵
		ret = GetActor2LocalBoneMatrix(pActor, szBoneName, bone.mtxBoneLocalMatrix);
		Assert(ret);

		bone.vectBoneLocalTranslation.x = bone.mtxBoneLocalMatrix._41;
		bone.vectBoneLocalTranslation.y = bone.mtxBoneLocalMatrix._42;
		bone.vectBoneLocalTranslation.z = bone.mtxBoneLocalMatrix._43;
		//bone.mtxBoneLocalRotateMatrix = bone.mtxBoneLocalMatrix;
		//bone.mtxBoneLocalRotateMatrix._41 = 0;
		//bone.mtxBoneLocalRotateMatrix._42 = 0;
		//bone.mtxBoneLocalRotateMatrix._43 = 0;

		// 先用自己当前的骨骼矩阵初始化Tpose矩阵
		ret = GetActor2BoneMatrix(pActor, szBoneName, bone.mtxBoneTposeRotate);
		Assert(ret);
	}

	// 使用actor2角色的标准tpose更新角色tpose
	UpdateActor2TposeBone(nMVNBodyID);

	//// 记录根骨骼的高度
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
			// 需要建立新连接的MVN骨骼
			sMVNBone& MVN_bone = pBody->MVN_bones[nSegmentIndex];
			// 如果该MVN骨骼已经对应了其他Actor2骨骼，则解除对应关系
			if (MVN_bone.nBindedActor2BoneIndex > -1)
			{
				// 解除那个actor2骨骼对这个MVN骨骼的索引
				actor2_bones[MVN_bone.nBindedActor2BoneIndex].nBindedMVNBoneIndex = -1;
			}

			// 如果当前actor2骨骼已经对应了其他MVN骨骼，则解除对应关系
			if (actor2_bone.nBindedMVNBoneIndex > -1)
			{
				// 解除那个MVN骨骼对当前actor2骨骼的索引
				pBody->MVN_bones[actor2_bone.nBindedMVNBoneIndex].nBindedActor2BoneIndex = -1;
			}

			// 当前actor2骨骼指向这个MVN骨骼
			actor2_bone.nBindedMVNBoneIndex = nSegmentIndex;
			// 这个MVN骨骼指向当前actor2骨骼
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

	// 数据报头
	sDatagramHeader& head = pFrame->header;

	// 获得消息类型
	char szMessageType[3];
	memcpy(szMessageType, head.id+4, 2);
	szMessageType[2] = 0;
	int nMessageType = atoi(szMessageType);

	head.sample_counter = (int)ntohl(head.sample_counter);
	head.time_code = (unsigned int)ntohl(head.time_code);

	sBody* pBody = GetBody(head.avatarID);

	// 还没有该角色
	if (NULL == pBody)
	{
		pBody = AddBody(head.avatarID);
		Assert(pBody != NULL);

		fm_movie::Util_RunCallback(this, "on_new_role", CVarList()<<head.avatarID);
	}

	// 如果已经收到数据则不需要再处理了
	if (pBody->bReceivedFrame)
	{
		return;
	}
	pBody->bReceivedFrame = true;

	IVisual* pActor = InnerGetActor(pBody->actor2_bind);
	// 如果没有绑定角色
	if (NULL == pActor)
	{
		return;
	}

	const char *p = pFrame->buffer;
	switch (nMessageType)
	{
	case  MSG_POS_EULER:
		{
			// 获得MVN骨骼信息
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

			// 整理MVN骨骼信息
			FmMat4 mat_rx, mat_ry, mat_rz;
			float px, py, pz, rx, ry, rz;
			sMVNBone* MVN_bones = pBody->MVN_bones;
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				sMVNBone& MVN_bone = MVN_bones[i];
				FmMat4& mtxFrameBoneRotateToTpose = MVN_bone.mtxFrameBoneRotateToTpose;
				FmVec3& vectFrameBoneTranslation = MVN_bone.vectFrameBoneTranslation;

				// 右手坐标系转左手
				rx = -MVN_frame_pos_euler[i].rx * g_fRadianFactor;
				ry = -MVN_frame_pos_euler[i].ry * g_fRadianFactor;
				rz = MVN_frame_pos_euler[i].rz * g_fRadianFactor;
				// 旋转顺序: rx-ry-rz
				D3DXMatrixRotationX(&mat_rx, rx);
				FmMatrixRotationY(&mat_ry, ry);
				D3DXMatrixRotationZ(&mat_rz, rz);
				mtxFrameBoneRotateToTpose = mat_rx * mat_ry * mat_rz;

				// 右手坐标系转左手
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
			// 获得MVN骨骼信息
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

			// 整理MVN骨骼信息
			float px, py, pz;
			sMVNBone* MVN_bones = pBody->MVN_bones;
			for(int i = 0; i < MVN_BONES_COUNT; ++i)
			{
				sMVNBone& MVN_bone = MVN_bones[i];
				FmVec3& vectFrameBoneTranslation = MVN_bone.vectFrameBoneTranslation;

				// z向上的右手坐标系，x向前，y向左
				ConvertQuaternionFormMVNToActor2(MVN_bone.qFrameBoneRotateToTpose,
					MVN_frame_pos_quate[i].i, MVN_frame_pos_quate[i].j,
					MVN_frame_pos_quate[i].k, MVN_frame_pos_quate[i].re);

				D3DXMatrixRotationQuaternion(&MVN_bone.mtxFrameBoneRotateToTpose, 
					&MVN_bone.qFrameBoneRotateToTpose);

				// z向上的右手坐标系，x向前，y向左
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

	// 初始化根骨骼高度
	if (pBody->bInitRootBoneHeightOfMVN)
	{
		pBody->bInitRootBoneHeightOfMVN = false;
		pBody->fRootBoneHeightOfMVN = pos_root_bone_MVN.y;
	}

	IVisual* pActor = InnerGetActor(pBody->actor2_bind);
	// 如果没有绑定角色则不处理
	if (NULL == pActor)
	{
		return;
	}

	// 模型移动坐标
	FmVec3 pos;
	pos.x = pos_root_bone_MVN.x;
	pos.y = pos_root_bone_MVN.y-pBody->fRootBoneHeightOfMVN;
	pos.z = pos_root_bone_MVN.z;

	// 存储相对坐标信息
	IVarTable *pVarTable = pActor->GetCustoms();
	if (pVarTable)
	{
		pVarTable->Assign("px", CVar(VTYPE_FLOAT, pos.x));
		pVarTable->Assign("py", CVar(VTYPE_FLOAT, pos.y));
		pVarTable->Assign("pz", CVar(VTYPE_FLOAT, pos.z));
	}

	// 根据模型的旋转角度对移动坐标也进行旋转
	FmVec3 angle = pActor->GetAngle();
	FmMat4 mtxRoleRotate;
	FmMat4RotationYawPitchRoll(&mtxRoleRotate, angle.y, angle.x, angle.z);
	FmVec3TransformCoord(&pos, &pos, &mtxRoleRotate);

	// 发送移动坐标到脚本
	fm_movie::Util_RunCallback(this, "on_role_position", CVarList() << nMVNBodyID
		<< pos.x << pos.y << pos.z);

	size_t nActor2BoneCount = pBody->actor2_bones.size();

	CVarList args;
	CVarList result;
	bool ret;
	FmMat4 mtx_tmp;
	// actor2中父骨骼的模型空间变换矩阵
	FmMat4 mtxActor2ParentBoneModelMatrix;
	for (size_t b = 0; b < nActor2BoneCount; ++b)
	{
		sActor2Bone& actor2_bone = pBody->actor2_bones[b];

		// 获得actor中父骨骼的变换矩阵
		//args.Clear();
		//args.AddString(actor2_bone.strBoneParentName.c_str());
		//args.AddPointer(&mtxActor2ParentBoneModelMatrix);
		//ret = CHelper::MethodBool(pActor, "GetBoneMatrix", args);
		ret = GetActor2BoneMatrix(pActor, actor2_bone.strBoneParentName.c_str(), 
			mtxActor2ParentBoneModelMatrix);

		// 没有父骨骼
		if (!ret)
		{
			FmMatrixIdentity(&mtxActor2ParentBoneModelMatrix);
		}

		// 如果绑定了MVN里的骨骼
		if (actor2_bone.nBindedMVNBoneIndex >= 0)
		{
			sMVNBone& MVN_bone = pBody->MVN_bones[actor2_bone.nBindedMVNBoneIndex];

			// 根据MVN里骨骼变换量变换actor2的骨骼

			// 注意：
			//		由于大部分情况下两套骨骼的身高体形不一样，动作幅度也不一样，
			//		如果直接使用动作捕捉角色的骨骼位置变换信息，
			//		将使游戏里的角色被拉伸或收缩，
			//		所以无视动作捕捉软件发来的位移变换量。

			// 计算actor2骨骼模型空间的旋转矩阵
			actor2_bone.mtxFrameBone = actor2_bone.mtxBoneTposeRotate * 
				MVN_bone.mtxFrameBoneRotateToTpose;

			// local位移矩阵乘以actor中父骨骼的模型空间变换矩阵
			// 计算actor2骨骼模型空间的位移矩阵
			FmVec3& local_pos = actor2_bone.vectBoneLocalTranslation;
			D3DXMatrixTranslation(&mtx_tmp, local_pos.x, local_pos.y, local_pos.z);
			D3DXMatrixMultiply(&mtx_tmp, &mtx_tmp, &mtxActor2ParentBoneModelMatrix);

			//actor2_bone.vectFrameBoneTranslation.x = mtx_tmp._41;
			//actor2_bone.vectFrameBoneTranslation.y = mtx_tmp._42;
			//actor2_bone.vectFrameBoneTranslation.z = mtx_tmp._43;

			//actor2_bone.mtxFrameBone = actor2_bone.mtxFrameBoneRotate;
			// 得到最终的模型空间变换矩阵
			actor2_bone.mtxFrameBone._41 = mtx_tmp._41;
			actor2_bone.mtxFrameBone._42 = mtx_tmp._42;
			actor2_bone.mtxFrameBone._43 = mtx_tmp._43;
		}
		else
		{
			// local矩阵乘以actor中父骨骼的变换矩阵计算出该骨骼的变换矩阵
			D3DXMatrixMultiply(&actor2_bone.mtxFrameBone, 
				&actor2_bone.mtxBoneLocalMatrix, &mtxActor2ParentBoneModelMatrix);
		}

		// 设置骨骼数据到actor2
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

	// 使actor2的骨骼数据生效
	args.Clear();
	ret = CHelper::MethodBool(pActor, "UpdateBoneMatrix", args);
	Assert(ret);
}

void MotionCaptureFrameReceiver::ConvertQuaternionFormMVNToActor2(FmQuat& quaternion_actor2,
											  float mvn_qx, float mvn_qy, float mvn_qz, float mvn_qw)
{
	// mvn的四元数是右手坐标系，z轴朝上，x轴朝前，y轴朝左；
	// mvn旋转轴对应的dx旋转轴为：qx=-mvn_qy, qy=mvn_qz, qz=mvn_qx
	// 右手转左手，旋转角取反，即qx,qy,qz全取反；qx=mvn_qy, qy=-mvn_qz, qz=-mvn_qx
	// 因为引擎里的角色要转180度，所以qx和qz再取反：qx=-mvn_qy, qy=-mvn_qz, qz=mvn_qx
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
	// 如果没有绑定角色则不处理
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

		// 查找对应的tpose骨骼
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

	// 获得actor的骨骼列表
	CVarList args;
	CVarList actorBoneList;
	args.Clear();
	bool ret = CHelper::MethodVarList(pActor, "GetBoneNodeList", args, actorBoneList);
	if (!ret || actorBoneList.GetCount() == 0)
	{
		CORE_TRACE("(MotionCaptureFrameReceiver::LoadActor2TposeFromObject) Get BoneNodeList failed.");
		return false;
	}

	// 骨骼总数
	size_t nBoneCount = actorBoneList.GetCount();
	for (size_t b = 0; b < nBoneCount; ++b)
	{
		// 获得骨骼名
		const char* szBoneName = actorBoneList.StringVal(b);

		std::pair<ACTOR2TPOSE::iterator, bool> pair_ret = 
			m_actor2Tpose.insert(std::make_pair(szBoneName, FmMat4()));
		FmMat4& mat = pair_ret.first->second;

		// 获得骨骼Tpose矩阵
		//args.Clear();
		//args.AddString(szBoneName);
		//args.AddPointer(&mat);
		//ret = CHelper::MethodBool(pActor, "GetBoneMatrix", args);
		ret = GetActor2BoneMatrix(pActor, szBoneName, mat);
		Assert(ret);

		// 去掉位置信息
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
