//--------------------------------------------------------------------
// 文件名:		FrameReceiverGroup.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __FrameReceiverGroup_H__
#define __FrameReceiverGroup_H__

#include "../utils/array_pod.h"
#include "../visual/i_visual.h"

// 帧数据接收器组
// 一个接收器组监听一个端口

class IFrameReceiver;

class FrameReceiverGroup: public IVisual
{
private:
	// 接收器类型
	enum RECEIVER_TYPE
	{
		RECEIVER_TYPE_CAMERA = 0,
		RECEIVER_TYPE_MOTION_CAPTURE,
		RECEIVER_TYPE_BONE,
		RECEIVER_TYPE_MAX,
		RECEIVER_TYPE_INVALID,
	};

public:
	FrameReceiverGroup();
	virtual ~FrameReceiverGroup();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);

	// 开始监听
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// 停止监听
	bool StopListen();
	// 获取当前的状态
	bool GetListenState();

	// 摄像机帧接收器
	void SetCameraFrameReceiver(const PERSISTID& id);
	PERSISTID GetCameraFrameReceiver() const;
	// 动作捕捉帧接收器
	void SetMotionCaptureFrameReceiver(const PERSISTID& id);
	PERSISTID GetMotionCaptureFrameReceiver() const;
	// 骨骼动画帧接收器
	void SetBoneFrameReceiver(const PERSISTID& id);
	PERSISTID GetBoneFrameReceiver() const;

	// 向量进行欧拉角旋转
	int VectorRotation(const IVarList& args, IVarList& result) const;

	// 获取本机的ip
	static const char* GetHostIp();

private:
	// 接收线程
	static void __cdecl WorkerProc(void* lpParameter);

	void ProcessFrameData(char* pBuffer, int nBufferSize);
	IFrameReceiver* GetFrameReceiver(const PERSISTID& id);

	void Reset();

private:
	HANDLE			m_hThread;
	bool			m_bQuit;
	result_string	m_strIP;
	unsigned short	m_nPort;
	SOCKET			m_sock;

	PERSISTID m_Receivers[RECEIVER_TYPE_MAX];
};

#endif // __FrameReceiverGroup_H__
