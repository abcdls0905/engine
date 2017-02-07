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

	//�̶߳˵ȴ����̶߳�
	void ResThreadWaitSync(CLockUtil& lock, void*& eglSync);
	
	//���̵߳ȴ����߳�
	bool MainThreadWaitSync(CLockUtil& lock, void* eglSync);

	static CAndroidRenderThread* Inst()
	{
		static CAndroidRenderThread s_inst;
		return &s_inst;
	}
private:
	// ������ʾ
    bool TestEGLError();
private:
	// EGL variables
	EGLDisplay			eglDisplay;
	EGLConfig			eglConfig;
	EGLSurface			eglSurface;
	EGLContext			eglContext;	//MainContext;
	EGLContext			eglResContext;	//������Դר�õ�Context; ���������̼߳���
	EGLNativeWindowType	eglWindow;
    EGLint		        pi32ConfigAttribs[32];//�����豸���ʺϵ�����
	int                 usesize;
};

#endif