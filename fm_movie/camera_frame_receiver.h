//--------------------------------------------------------------------
// �ļ���:		CameraFrameReceiver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��12��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __CameraFrameReceiver_H__
#define __CameraFrameReceiver_H__

#include "i_frame_receiver.h"
#include "d_3dx_9.h"

// �����֡���ݽ�����

class CameraWrapper;

class CameraFrameReceiver: public IFrameReceiver
{
public:
	CameraFrameReceiver();
	virtual ~CameraFrameReceiver() {}

	// �����
	void SetCameraWrapperID(const PERSISTID& value);
	PERSISTID GetCameraWrapperID() const;

	// ����������Ĳ��սǶ�
	void SetCameraRefAngle(float x, float y, float z);

	void SetCameraReversal(bool reversal);
	bool GetCameraReversal() const;

private:
	virtual void ProcessFrameData();
	CameraWrapper* GetCameraWrapper(const PERSISTID& id);

	// ���ݾֲ�����ϵ�������Ķ�����Ϊ���ն�������Լ��ĽǶȡ�����matָ���ն������ת����
	void UpdateCameraAngle(const FmMat4 &mat);

	// �������ǿ��ת180��
	void RotationPI(float &angleY, float &x, float &y, float &z);
private:
	PERSISTID	m_CameraWrapperID;		// �󶨵������
	FmMat4	m_matRef;				// ��¼������ĽǶ�
	bool		m_bReversal;			// ������Ƿ�ת180����
};

#endif // __CameraFrameReceiver_H__
