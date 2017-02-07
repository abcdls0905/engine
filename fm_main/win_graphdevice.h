
#ifndef _GRAPHDEVICE_H_
#define _GRAPHDEVICE_H_

#include <egl/egl.h>
  

// �豸���egl �İ�װ

class CGraphDevice
{
public:
	CGraphDevice();
	~CGraphDevice();
	// ��ʼ���豸(�����ھ��)
	bool InitDevice(EGLNativeWindowType handle);

	void Present();

	// �豸�ر�
	void ShutDevice();
private:
	// ������ʾ
    bool TestEGLError();

	// �豸�������� Ĭ��Ϊ  24λ��� 8λģ��
	bool SelectEGLConfiguration(int nColorBPP,int DepthBPP,int StencilSize);

	void SetConfig(int type,int value);

	// ��ѯ ���������ݵ�λ��pos
	bool GetConfig(int type,int &value,int& pos);

private:
	// EGL variables
	EGLDisplay			eglDisplay;
	EGLConfig			eglConfig;
	EGLSurface			eglSurface;
	EGLContext			eglContext;
	EGLNativeWindowType	eglWindow;
    EGLint		        pi32ConfigAttribs[32];//�����豸���ʺϵ�����
	int                 usesize;
};





#endif