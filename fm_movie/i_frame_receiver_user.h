//--------------------------------------------------------------------
// 文件名:		IFrameReceiverUser.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __IFRAME_RECEIVER_USER_H__
#define __IFRAME_RECEIVER_USER_H__

#include "frame_receiver.h"
#include "../visual/i_visual.h"

// FrameReceiver使用者抽象类

class IFrameReceiverUser: public IVisual
{
public:
	IFrameReceiverUser();
	virtual ~IFrameReceiverUser();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	//virtual void Realize(float offset_seconds);

	// 开始监听MVN发来的数据
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// 停止监听
	bool StopListen();

	// 帧数据尺寸
	void SetFrameSize(unsigned int value) { m_nFrameSize = value; }
	unsigned int GetFrameSize() const { return m_nFrameSize; }

private:
	virtual void ProcessFrameData(char* pFrameBuffer) = 0;

private:
	unsigned int m_nFrameSize;
	FrameReceiver* m_pFrameReceiver;
};

#endif // __IFRAME_RECEIVER_USER_H__
