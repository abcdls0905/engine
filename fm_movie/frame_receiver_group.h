//--------------------------------------------------------------------
// �ļ���:		FrameReceiverGroup.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __FrameReceiverGroup_H__
#define __FrameReceiverGroup_H__

#include "../utils/array_pod.h"
#include "../visual/i_visual.h"

// ֡���ݽ�������
// һ�������������һ���˿�

class IFrameReceiver;

class FrameReceiverGroup: public IVisual
{
private:
	// ����������
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

	// ��ʼ����
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// ֹͣ����
	bool StopListen();
	// ��ȡ��ǰ��״̬
	bool GetListenState();

	// �����֡������
	void SetCameraFrameReceiver(const PERSISTID& id);
	PERSISTID GetCameraFrameReceiver() const;
	// ������׽֡������
	void SetMotionCaptureFrameReceiver(const PERSISTID& id);
	PERSISTID GetMotionCaptureFrameReceiver() const;
	// ��������֡������
	void SetBoneFrameReceiver(const PERSISTID& id);
	PERSISTID GetBoneFrameReceiver() const;

	// ��������ŷ������ת
	int VectorRotation(const IVarList& args, IVarList& result) const;

	// ��ȡ������ip
	static const char* GetHostIp();

private:
	// �����߳�
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
