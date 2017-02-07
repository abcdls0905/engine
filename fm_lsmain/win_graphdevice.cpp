
#include "win_graphdevice.h"
#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime

 
CGraphDevice::CGraphDevice():usesize(0)
{
	SetConfig( EGL_LEVEL ,0 );
	SetConfig( EGL_SURFACE_TYPE, EGL_WINDOW_BIT );
	SetConfig( EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT );
	SetConfig( EGL_NATIVE_RENDERABLE, EGL_FALSE ); 
    SetConfig( EGL_BUFFER_SIZE , EGL_DONT_CARE );
    SetConfig( EGL_DEPTH_SIZE , EGL_DONT_CARE );
	SetConfig( EGL_STENCIL_SIZE, EGL_DONT_CARE );

	// 设置抗锯齿
	SetConfig( EGL_SAMPLE_BUFFERS,0 );//默认关闭抗锯齿
	SetConfig( EGL_SAMPLES ,0);
}

CGraphDevice::~CGraphDevice()
{
}
//
HDC g_hDC;

// 初始化设备
bool CGraphDevice::InitDevice(EGLNativeWindowType handle)
{
	eglWindow = handle;

	g_hDC = GetDC(handle);

	if( !g_hDC )
	{
		MessageBoxA(0,"Failed to create the device context", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	eglDisplay = eglGetDisplay(g_hDC);

    if(eglDisplay == EGL_NO_DISPLAY)
         eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);

	EGLint iMajorVersion, iMinorVersion;
	if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
	{
		MessageBoxA(0, "eglInitialize() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	eglBindAPI(EGL_OPENGL_ES_API);//设置当前全局渲染API
	if( !TestEGLError() )
	{
		MessageBoxA(0, "eglBindAPI failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

 	SelectEGLConfiguration(EGL_DONT_CARE,16,4);
	// 设备匹配
	int iConfigs;
	if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		MessageBoxA(0, "eglChooseConfig() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	// 创建渲染区工作窗口
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

    if(eglSurface == EGL_NO_SURFACE)
    {
        eglGetError(); // Clear error
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
	}

	if (!TestEGLError())
	{
		MessageBoxA(0, "eglCreateWindowSurface() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	// 创建关联
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);
	if (!TestEGLError() )
	{
		MessageBoxA(0, "eglCreateContext() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	// 绑定当前绘制窗口
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!TestEGLError())
	{
		MessageBoxA(0, "eglMakeCurrent() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void CGraphDevice::Present()
{
//	double dtime = performance_time();
	// 提交前台页面
	eglSwapBuffers(eglDisplay, eglSurface);
	if (!TestEGLError())
	{ 
 		MessageBoxA(0, "eglSwapBuffers() failed.", "Error", MB_OK|MB_ICONEXCLAMATION);		
	}
}

// 设备关闭
void CGraphDevice::ShutDevice()
{
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);

	if(g_hDC)
	{
		ReleaseDC(eglWindow,g_hDC);
	}
}

// 设备创建配置 
bool CGraphDevice::SelectEGLConfiguration(int nColorBPP,int nDepthBPP,int nStencilSize)
{
    SetConfig( EGL_BUFFER_SIZE , nColorBPP );
	SetConfig( EGL_DEPTH_SIZE , nDepthBPP );
	SetConfig( EGL_STENCIL_SIZE , nStencilSize );

	return true;
}

void CGraphDevice::SetConfig(int type,int value)
{
	int oldvalue;
	int pos = 0;
	if(GetConfig(type,oldvalue,pos))
	{
        pi32ConfigAttribs[pos] = type;
        pi32ConfigAttribs[pos+1] = value;
	}
	else
	{
        pi32ConfigAttribs[usesize++] = type;
		pi32ConfigAttribs[usesize++] = value;
	}
	//设置结束位数据
    pi32ConfigAttribs[usesize] = EGL_NONE;
}

bool CGraphDevice::GetConfig(int type,int &value,int& pos)
{
	for(int i = 0;i < usesize; i=i+2 )
	{
		if(pi32ConfigAttribs[i]  == type )
		{
			value = pi32ConfigAttribs[i+1];
			pos = i;
			return true;
		}
	}
	return false;
}

// 错误提示
bool CGraphDevice::TestEGLError()
{
	EGLint iErr = eglGetError();
	if( iErr != EGL_SUCCESS )
	{
       return false;
	}
	return true;
}