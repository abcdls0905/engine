//--------------------------------------------------------------------
// �ļ���:		VirtualCamera.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __VIRTUAL_CAMERA_H__
#define __VIRTUAL_CAMERA_H__

#include "i_frame_receiver_user.h"
#include "../public/module.h"

// ���������

class CameraWrapper;

class VirtualCamera: public IFrameReceiverUser
{
public:
	VirtualCamera();
	virtual ~VirtualCamera() {}

	// �����
	void SetCameraWrapperID(const PERSISTID& value);
	PERSISTID GetCameraWrapperID() const;

private:
	virtual void ProcessFrameData(char* pFrameBuffer);
	CameraWrapper* GetCameraWrapper(const PERSISTID& id);

private:
	PERSISTID m_CameraWrapperID;		// �󶨵������
};

#endif // __VIRTUAL_CAMERA_H__
