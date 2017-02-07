//--------------------------------------------------------------------
// 文件名:		VirtualCamera.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __VIRTUAL_CAMERA_H__
#define __VIRTUAL_CAMERA_H__

#include "i_frame_receiver_user.h"
#include "../public/module.h"

// 虚拟摄像机

class CameraWrapper;

class VirtualCamera: public IFrameReceiverUser
{
public:
	VirtualCamera();
	virtual ~VirtualCamera() {}

	// 摄像机
	void SetCameraWrapperID(const PERSISTID& value);
	PERSISTID GetCameraWrapperID() const;

private:
	virtual void ProcessFrameData(char* pFrameBuffer);
	CameraWrapper* GetCameraWrapper(const PERSISTID& id);

private:
	PERSISTID m_CameraWrapperID;		// 绑定的摄像机
};

#endif // __VIRTUAL_CAMERA_H__
