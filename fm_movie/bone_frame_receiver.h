//--------------------------------------------------------------------
// 文件名:		BoneFrameReceiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月28日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef __BONE_FRAME_RECEIVER_H__
#define __BONE_FRAME_RECEIVER_H__

#include "i_frame_receiver.h"

// 骨骼动画帧接收器

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
