//--------------------------------------------------------------------
// 文件名:		CameraFrameReceiver.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __CameraFrameReceiver_H__
#define __CameraFrameReceiver_H__

#include "i_frame_receiver.h"
#include "d_3dx_9.h"

// 摄像机帧数据接收器

class CameraWrapper;

class CameraFrameReceiver: public IFrameReceiver
{
public:
	CameraFrameReceiver();
	virtual ~CameraFrameReceiver() {}

	// 摄像机
	void SetCameraWrapperID(const PERSISTID& value);
	PERSISTID GetCameraWrapperID() const;

	// 摄像摄像机的参照角度
	void SetCameraRefAngle(float x, float y, float z);

	void SetCameraReversal(bool reversal);
	bool GetCameraReversal() const;

private:
	virtual void ProcessFrameData();
	CameraWrapper* GetCameraWrapper(const PERSISTID& id);

	// 根据局部坐标系中其它的对象作为参照对象更新自己的角度。参数mat指参照对象的旋转矩阵
	void UpdateCameraAngle(const FmMat4 &mat);

	// 将摄像机强制转180度
	void RotationPI(float &angleY, float &x, float &y, float &z);
private:
	PERSISTID	m_CameraWrapperID;		// 绑定的摄像机
	FmMat4	m_matRef;				// 记录参照物的角度
	bool		m_bReversal;			// 摄像机是否翻转180处理
};

#endif // __CameraFrameReceiver_H__
