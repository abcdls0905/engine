//--------------------------------------------------------------------
// �ļ���:		IFrameReceiverUser.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __IFRAME_RECEIVER_USER_H__
#define __IFRAME_RECEIVER_USER_H__

#include "frame_receiver.h"
#include "../visual/i_visual.h"

// FrameReceiverʹ���߳�����

class IFrameReceiverUser: public IVisual
{
public:
	IFrameReceiverUser();
	virtual ~IFrameReceiverUser();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	//virtual void Realize(float offset_seconds);

	// ��ʼ����MVN����������
	bool StartListen(const char* szMyIP, unsigned short nPort);
	// ֹͣ����
	bool StopListen();

	// ֡���ݳߴ�
	void SetFrameSize(unsigned int value) { m_nFrameSize = value; }
	unsigned int GetFrameSize() const { return m_nFrameSize; }

private:
	virtual void ProcessFrameData(char* pFrameBuffer) = 0;

private:
	unsigned int m_nFrameSize;
	FrameReceiver* m_pFrameReceiver;
};

#endif // __IFRAME_RECEIVER_USER_H__
