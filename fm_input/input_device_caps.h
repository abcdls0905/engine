//--------------------------------------------------------------------
// 文件名:		input_device_caps.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年5月14日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef  __INPUT_DEVICE_CAPS_H__
#define  __INPUT_DEVICE_CAPS_H__

#include "../public/core_type.h"
#include "../public/module.h"

/*
用于扫描输入设备
检测设备是否支持
*/
//设备
typedef enum
{
	INPUT_NONE = 0,
	INPUT_MOUSE_,//鼠标
	INPUT_KEYBOARD_,//键盘
	INPUT_TOUCH,//触摸屏
	INPUT_ACCELEROMETER,//加速计
	INPUT_GYROSCOPE,//陀螺仪
	INPUT_CAMERA,//摄像头
	INPUT_GPS,//GPS定位
	INPUT_MAX_NUM = INPUT_GPS,
}INPUT_DEVICE_TYPE;



class CInputDeviceCaps:public IEntity
{
public:
	CInputDeviceCaps();
	~CInputDeviceCaps();

	virtual bool Init(const IVarList& args);
 
	virtual bool Shut();

    //是否支持指定设备
    bool IsSupported(INPUT_DEVICE_TYPE device);
private:
	//进行扫描
	void ScanDevice();
private:
    bool m_bDeviceSupported[INPUT_MAX_NUM];
};

#endif