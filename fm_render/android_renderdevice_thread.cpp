
#include "android_renderdevice_thread.h"

#if defined GPU_MULTITHREADING && !defined FX_SYSTEM_IOS



#include "../public/core_log.h"


PFNEGLCREATESYNCKHRPROC     eglCreateSyncKHR     = NULL;
PFNEGLDESTROYSYNCKHRPROC    eglDestroySyncKHR    = NULL;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR = NULL;
PFNEGLGETSYNCATTRIBKHRPROC  eglGetSyncAttribKHR  = NULL;

CAndroidRenderThread::CAndroidRenderThread()
:usesize(0)
{
	m_width = 1024;
	m_height = 768;
}

CAndroidRenderThread::~CAndroidRenderThread()
{
}


bool CAndroidRenderThread::TestEGLError()
{
	EGLint iErr = eglGetError();
	if( iErr != EGL_SUCCESS )
	{
       return false;
	}
	return true;
}

void CAndroidRenderThread::StartResourceContext()
{
	// Create a new EGL configuration to specify that we require a pbuffer surface.
	EGLint attrs[] = {
		EGL_SURFACE_TYPE,    EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	
	EGLint iConfigs;
	EGLConfig pbufferConf;
	if(!eglChooseConfig(eglDisplay, attrs, &pbufferConf, 1, &iConfigs) || (iConfigs != 1))
	{
		return ;
	}
    
	// Create a dummy pbuffer surface
	EGLint PBufferAttribs[] =
	{
		EGL_WIDTH, m_width,
		EGL_HEIGHT, m_height,
		EGL_NONE
	};
	EGLSurface eglPBufferSurf = eglCreatePbufferSurface(eglDisplay, pbufferConf, PBufferAttribs);
    
	// Create the secondary egl context.
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };    
	eglContext = eglCreateContext(eglDisplay, pbufferConf, eglContext, ai32ContextAttribs);
   	if(eglContext == EGL_NO_CONTEXT)
	{
		return ;
	}

	eglMakeCurrent(eglDisplay, eglPBufferSurf, eglPBufferSurf, eglContext);
}

void CAndroidRenderThread::EndResourceContext()
{
}

void CAndroidRenderThread::ResThreadWaitSync(CLockUtil& lock, void* & eglSync)
{
	Assert(eglSync == 0);
	lock.Lock();
	eglSync = eglCreateSyncKHR(eglDisplay, EGL_SYNC_FENCE_KHR, NULL);
	lock.Unlock();
    
    if(eglSync == EGL_NO_SYNC_KHR)
    {
		CORE_TRACE_EX("eglCreateSyncKHR returned unexpected EGL_NO_SYNC_KHR %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		Assert(0);
    }
    
    // EGL_SYNC_FLUSH_COMMANDS_BIT_KHR causes the EGL context to flush.
    EGLint status = eglClientWaitSyncKHR(eglDisplay, eglSync, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 0);
    if(status == EGL_FALSE)
    {
		CORE_TRACE_EX("eglClientWaitSyncKHR returned unexpected EGL_FALSE. %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		Assert(0);
    }
}

bool CAndroidRenderThread::MainThreadWaitSync(CLockUtil& lock, void* eglSync)
{
	bool ready = false;
	lock.Lock();

	if(eglSync != EGL_NO_SYNC_KHR)
	{
		// Perform a non-blocking poll to check if the shared egl sync object has been signalled.
		EGLint status = eglClientWaitSyncKHR(eglDisplay, eglSync, 0, 0);
		if(status == EGL_CONDITION_SATISFIED_KHR)
		{
			// Destroy the egl sync object as soon as we aware of it's signal status.
			if(eglDestroySyncKHR(eglDisplay, eglSync) != EGL_TRUE)
			{
				CORE_TRACE_EX("eglDestroySyncKHR returned unexpected EGL_FALSE. %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
				Assert(0);
				return false;
			}
			ready = true;
		}
		else if(status == EGL_FALSE)
		{
			CORE_TRACE_EX("eglClientWaitSyncKHR returned unexpected EGL_FALSE. %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			Assert(0); 
		}
	}

	lock.Unlock();
	return ready;
}

void CAndroidRenderThread::Init(int width, int height)
{
	g_ResourceContext = this;
	m_width = width;
	m_height = height;

	eglCreateSyncKHR     = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
	eglDestroySyncKHR    = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
	eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
	eglGetSyncAttribKHR  = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");
	
	if(!(eglCreateSyncKHR && eglDestroySyncKHR && eglClientWaitSyncKHR && eglGetSyncAttribKHR))
	{
		CORE_TRACE("Error: Failed to retrieve function pointers for KHR_fence_sync extension functions.\nIt's possible that the host system does not support this extension.\n");
		g_ResourceContext = 0;
		return ;
	}

	eglDisplay = eglGetCurrentDisplay();
	eglContext = eglGetCurrentContext();
}




CAndroidRenderThread ___graph_common_device_inst;

IDeviceContext* g_ResourceContext = &___graph_common_device_inst;
#endif
