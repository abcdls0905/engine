//--------------------------------------------------------------------
// �ļ���:		BoneFrameReceiver.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��2��28��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "bone_frame_receiver.h"
#include "../public/var_list.h"
#include "../visual/dx_define.h"
#include "helper.h"

/// entity: BoneFrameReceiver
/// \brief ��������֡������
/// \see �̳���IFrameReceiver
DECLARE_ENTITY("", BoneFrameReceiver, IFrameReceiver);
/// property: object Actor
/// \brief �����
DECLARE_PROPERTY(PERSISTID, BoneFrameReceiver, Actor, GetActorID, SetActorID);

struct location_t
{
	float pos_x;
	float pos_y;
	float pos_z;

	float quat_x;
	float quat_y;
	float quat_z;
	float quat_w;

	//float scale_x;
	//float scale_y;
	//float scale_z;
};


void BoneFrameReceiver::ProcessFrameData()
{
	if (m_ProcessingFrameData.empty())
	{
		Assert(0);
		return;
	}

	IEntity* pActor = GetCore()->GetEntity(m_ActorID);
	// ���û�а󶨽�ɫ
	if (NULL == pActor)
	{
		return;
	}

	FRAME_LIST::iterator ite_frame = m_ProcessingFrameData.begin();
	FRAME_LIST::const_iterator end_frame = m_ProcessingFrameData.end();
	char* pFrameBuffer = NULL;
	for (; ite_frame != end_frame; ++ite_frame)
	{
		pFrameBuffer = *ite_frame;
		InnerProcessFrameData(pFrameBuffer);
	}

	CVarList args;
	bool ret;

	// ʹactor2�Ĺ���������Ч
	args.Clear();
	ret = CHelper::MethodBool(pActor, "UpdateBoneMatrix", args);
	Assert(ret);
}

void BoneFrameReceiver::InnerProcessFrameData(char* pFrameBuffer)
{
	IEntity* pActor = GetCore()->GetEntity(m_ActorID);
	// ���û�а󶨽�ɫ
	if (NULL == pActor)
	{
		return;
	}

	char* p = pFrameBuffer;

	// ��������ͷ
	p += 4;

	size_t nSectionCount = *(size_t*)p;
	p += sizeof(size_t);

	location_t location;
	CVarList args;
	CVarList result;
	bool ret;
	FmMat4 mat;
	FmQuat quat;
	for (size_t s = 0; s < nSectionCount; ++s)
	{
		size_t uNodeNameLen = *(size_t*)p;
		p += sizeof(size_t);

		const char* szNodeName = p;
		p += uNodeNameLen;

		memcpy(&location, p, sizeof(location));
		p += sizeof(location);

		quat.x = location.quat_x;
		quat.y = location.quat_y;
		quat.z = location.quat_z;
		quat.w = location.quat_w;
		D3DXMatrixRotationQuaternion(&mat, &quat);
		
		mat._41 = location.pos_x;
		mat._42 = location.pos_y;
		mat._43 = location.pos_z;

		//mat._11 = location.scale_x;
		//mat._22 = location.scale_y;
		//mat._33 = location.scale_z;

		// ���ù������ݵ�actor2
		args.Clear();
		args.AddString(szNodeName);
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; ++j)
			{
				args.AddFloat(mat(i, j));
			}
		}
		ret = CHelper::MethodBool(pActor, "SetBoneMatrix", args);
		//Assert(ret);
	}
}

void BoneFrameReceiver::SetActorID(const PERSISTID& value)
{
	m_ActorID = value;
}

PERSISTID BoneFrameReceiver::GetActorID() const
{
	return m_ActorID;
}
