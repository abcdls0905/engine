#pragma once
#include "render_header.h"
#include "../public/macros.h"
#include "../public/portable.h"
#include "../utils/lock_util.h"
#include <OpenGLES/EAGL.h>
#include "common_device.h"

#if defined GPU_MULTITHREADING && defined FX_SYSTEM_IOS



class CIOSRenderThread : public IDeviceContext
{
	int m_width, m_height;
    // EAGL handles
	EAGLContext*	  eaglMainContext;
	EAGLContext*	  eaglSecContext;
    
public:
	CIOSRenderThread();
	~CIOSRenderThread();

	void Init(int width, int height);

	void StartResourceContext();

	void EndResourceContext();

	//�̶߳˵ȴ����̶߳�
	void ResThreadWaitSync(CLockUtil& lock, void*& eglSync);
	
	//���̵߳ȴ����߳�
	bool MainThreadWaitSync(CLockUtil& lock, void* eglSync);

	static CIOSRenderThread* Inst()
	{
		static CIOSRenderThread s_inst;
		return &s_inst;
	}
private:
	// ������ʾ    bool TestEGLError();
private:
	// EGL variables
	int                 usesize;
};

#endif