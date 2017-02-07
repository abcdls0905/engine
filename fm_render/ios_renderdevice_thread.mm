#include "ios_renderdevice_thread.h"

#if defined GPU_MULTITHREADING && defined FX_SYSTEM_IOS



#include "../public/core_log.h"

/*
PFNEGLCREATESYNCKHRPROC     eglCreateSyncKHR     = NULL;
PFNEGLDESTROYSYNCKHRPROC    eglDestroySyncKHR    = NULL;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR = NULL;
PFNEGLGETSYNCATTRIBKHRPROC  eglGetSyncAttribKHR  = NULL;*/

CIOSRenderThread::CIOSRenderThread()
:usesize(0)
{
	m_width = 1024;
	m_height = 768;
}

CIOSRenderThread::~CIOSRenderThread()
{
}

void CIOSRenderThread::StartResourceContext()
{
    // Create the secondary eagl context.
	EAGLSharegroup* eaglShareGroup = [eaglMainContext sharegroup];
    eaglSecContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:eaglShareGroup];
    
    if(eaglSecContext == nil)
    {
        CORE_TRACE("Failed to create a secondary context.\n");
        Assert(0);
        return ;
    }
	
	if(![EAGLContext setCurrentContext:eaglSecContext])
	{
		CORE_TRACE("Failed to make the secondary context current.\n");
        Assert(0);
		return ;
	}
    
    
    /*
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

	eglMakeCurrent(eglDisplay, eglPBufferSurf, eglPBufferSurf, eglContext);*/
}

void CIOSRenderThread::EndResourceContext()
{
}

void CIOSRenderThread::ResThreadWaitSync(CLockUtil& lock, void* & eglSync)
{
	lock.Lock();
	eglSync = glFenceSyncAPPLE(GL_SYNC_GPU_COMMANDS_COMPLETE_APPLE, 0);
	lock.Unlock();
    
	if(!glIsSyncAPPLE((GLsync)eglSync))
    {
        CORE_TRACE_EX("glFenceSyncAPPLE failed to create a valid fence sync object.\n");
    }

    // EGL_SYNC_FLUSH_COMMANDS_BIT_KHR causes the EGL context to flush.
    GLenum status = glClientWaitSyncAPPLE((GLsync)eglSync, GL_SYNC_FLUSH_COMMANDS_BIT_APPLE, 0);
    if(status == GL_WAIT_FAILED_APPLE)
    {
		CORE_TRACE_EX("glClientWaitSyncAPPLE failed.");
		Assert(0);
    }
}

bool CIOSRenderThread::MainThreadWaitSync(CLockUtil& lock, void* eglSync)
{
	bool ready = false;
	lock.Lock();

	if(glIsSyncAPPLE((GLsync)eglSync))
	{
		// Perform a non-blocking poll to check if the shared egl sync object has been signalled.
		GLenum status = glClientWaitSyncAPPLE((GLsync)eglSync, 0, 0);
		if(status == GL_CONDITION_SATISFIED_APPLE || status == GL_ALREADY_SIGNALED_APPLE)
		{
			// Destroy the gl sync object as soon as we aware of it's signal status.
			glDeleteSyncAPPLE((GLsync)eglSync);
			ready = true;
		}
	}

	lock.Unlock();
	return ready;
}

void CIOSRenderThread::Init(int width, int height)
{
	g_ResourceContext = this;
	m_width = width;
	m_height = height;

    eaglMainContext = [EAGLContext currentContext];
    Assert(eaglMainContext);
    CORE_TRACE("CIOSRenderThread Init !");
 
    /*
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
	eglContext = eglGetCurrentContext();*/
}




CIOSRenderThread ___graph_common_device_inst;

IDeviceContext* g_ResourceContext = &___graph_common_device_inst;
#endif
