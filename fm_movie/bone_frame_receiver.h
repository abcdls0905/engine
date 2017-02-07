//--------------------------------------------------------------------
// �ļ���:		BoneFrameReceiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��2��28��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __BONE_FRAME_RECEIVER_H__
#define __BONE_FRAME_RECEIVER_H__

#include "i_frame_receiver.h"

// ��������֡������

class BoneFrameReceiver: public IFrameReceiver
{
public:
	BoneFrameReceiver() {}
	virtual ~BoneFrameReceiver() {}

	void SetActorID(const PERSISTID& value);
	PERSISTID GetActorID() const;

private:
	virtual void ProcessFrameData();
	void InnerProcessFrameData(char* pFrameBuffer);

private:
	PERSISTID m_ActorID;
};

#endif // __BONE_FRAME_RECEIVER_H__
