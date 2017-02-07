#pragma once

#include "../public/macros.h"
#include "common_device.h"
#include "render_header.h"
#include <EGL/eglext.h>

#if defined GPU_MULTITHREADING && !defined FX_SYSTEM_IOS

class CAndroidRenderThread : public IDeviceContext
{
	int m_width, m_height;
public:
	CAndroidRenderThread();
	~CAndroidRenderThread();

	void Init(int width, int height);

	void StartResourceContext();

	void EndResourceContext();

	//线程端等待主线程端
	void ResThreadWaitSync(CLockUtil& lock, void*& eglSync);
	
	//主线程等待子线程
	bool MainThreadWaitSync(CLockUtil& lock, void* eglSync);

	static CAndroidRenderThread* Inst()
	{
		static CAndroidRenderThread s_inst;
		return &s_inst;
	}
private:
	// 错误提示
    bool TestEGLError();
private:
	// EGL variables
	EGLDisplay			eglDisplay;
	EGLConfig			eglConfig;
	EGLSurface			eglSurface;
	EGLContext			eglContext;	//MainContext;
	EGLContext			eglResContext;	//下载资源专用的Context; 用来做多线程加载
	EGLNativeWindowType	eglWindow;
    EGLint		        pi32ConfigAttribs[32];//创建设备的适合的配置
	int                 usesize;
};

#endif