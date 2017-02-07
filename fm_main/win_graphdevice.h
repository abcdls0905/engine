
#ifndef _GRAPHDEVICE_H_
#define _GRAPHDEVICE_H_

#include <egl/egl.h>
  

// 设备相关egl 的包装

class CGraphDevice
{
public:
	CGraphDevice();
	~CGraphDevice();
	// 初始化设备(主窗口句柄)
	bool InitDevice(EGLNativeWindowType handle);

	void Present();

	// 设备关闭
	void ShutDevice();
private:
	// 错误提示
    bool TestEGLError();

	// 设备创建配置 默认为  24位深度 8位模板
	bool SelectEGLConfiguration(int nColorBPP,int DepthBPP,int StencilSize);

	void SetConfig(int type,int value);

	// 查询 并返回数据的位置pos
	bool GetConfig(int type,int &value,int& pos);

private:
	// EGL variables
	EGLDisplay			eglDisplay;
	EGLConfig			eglConfig;
	EGLSurface			eglSurface;
	EGLContext			eglContext;
	EGLNativeWindowType	eglWindow;
    EGLint		        pi32ConfigAttribs[32];//创建设备的适合的配置
	int                 usesize;
};





#endif