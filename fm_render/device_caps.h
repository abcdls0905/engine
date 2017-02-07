//--------------------------------------------------------------------
// �ļ���:		device_caps.h
// ��  ��:		
// ˵  ��:		
// ��������:	20123��3��22��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef _DEVICE_CAPS_H
#define _DEVICE_CAPS_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/vis_utils.h"
#include "render_header.h"

// ϵͳ����

class DeviceCaps: public IEntity
{
public:

//#ifndef FX_SYSTEM_IOS
	typedef void (GL_APIENTRY * PFNGLDISCARDFRAMEBUFFEREXT)(fm_enum target, fm_sizei numAttachments, const fm_enum *attachments);
//#endif

    typedef void* (GL_APIENTRY *PFNGLMAPBUFFEROES)(fm_enum target, fm_enum access);
    typedef fm_boolean (GL_APIENTRY *PFNGLUNMAPBUFFEROES)(fm_enum target);
    typedef void (GL_APIENTRY *PFNGLGETBUFFERPOINTERVOES)(fm_enum target, fm_enum pname, void** params);

	typedef void (GL_APIENTRY *PFNGLGENQUERIESEXT) (fm_sizei n, GLuint *ids);
	typedef void (GL_APIENTRY *PFNGLDELETEQUERIESEXT) (fm_sizei n, const fm_uint *ids);
	typedef fm_boolean (GL_APIENTRY *PFNGLISQUERYEXT) (fm_uint id);
	typedef void (GL_APIENTRY *PFNGLBEGINQUERYEXT) (fm_enum target, GLuint id);
	typedef void (GL_APIENTRY *PFNGLENDQUERYEXT) (fm_enum target);
	typedef void (GL_APIENTRY *PFNGLGETQUERYIVEXT) (fm_enum target, fm_enum pname, GLint *params);
	typedef void (GL_APIENTRY *PFNGLGETQUERYOBJECTUIVEXT) (GLuint id, fm_enum pname, GLuint *params);

	DeviceCaps();
	virtual ~DeviceCaps();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ƿ�֧��PVR��ʽ
	bool IsPVRTCSupported();

	// �Ƿ�֧��ETC1��ʽ
	bool IsETCSupported();

	// �Ƿ�֧��ATC��ʽ
	bool IsATCSupported();

	// �Ƿ�֧��DDS��ʽ
	bool IsDDSSupported();

	// �Ƿ�֧��BGRA8888��ʽ
	bool IsBGRA8888Supported();

	// �Ƿ�֧��RGB8��RGBA8
	bool IsRGB888_RGBA8888Supported();

	// �Ƿ�֧��RGBA8
	bool IsRGBA8888Supported();

	// �Ƿ�֧�ּ���2����shader
	bool IsBinaryShaderSupported();

	// �Ƿ�֧�ְ���ȵ�����ͼ
	bool IsDepthTextureSupported();

	// �Ƿ�֧��24λ��Ⱥ�8λģ��
	bool IsDepth24Stencil8Supported();

	// �Ƿ�֧��1λģ��
	bool IsStencil1Supported();

	// �Ƿ�֧��4λģ��
	bool IsStencil4Supported();

	// �Ƿ�֧��3D��ͼ
	bool IsTexture3DSupported();

	// �Ƿ�֧����ͼ2ָ����С non-power-of-two (npot)
	bool IsTextureNPOTSupported();

	// �Ƿ�֧��RG˫ͨ������(��Ҫ���ڷ���)
	bool IsTextureRGSupported();

	// �Ƿ�֧��16λ��������
	bool IsTextureFloat16Supported();

	// �Ƿ�֧��16λ�����������Բ�ֵ
	bool IsTextureFloat16LinearSupported();

	// �Ƿ�֧�ָ��������COLOR_HALF_FLOAT(ȷ�����������Ƿ�֧��FBO COLOR�ҵ�)
	bool IsTextureFloat16ColorBufferSupported();

	// �Ƿ�֧�ָ��������COLOR_HALF_FLOAT(ȷ�����������Ƿ�֧��FBO COLOR�ҵ�)
	bool IsTextureFloat32ColorBufferSupported();

	// �Ƿ�֧��32λ��������
	bool IsTextureFloat32Supported();

	// �Ƿ�֧��32λ�����������Թ��˷�ʽ
	bool IsTextureFloat32LinearSupported();

	// �Ƿ�֧��32λ��������
	bool IsVertex32BitIndexSupported();

	// �Ƿ�֧��24λ���
	bool IsDepth24Supported();

	// �Ƿ�֧��32λ���
	bool IsDepth32Supported();

	// �Ƿ�֧��FBO Render MipMap ��FBO��Ⱦ��һ�Ŵ�MipMap����ͼ��
	bool IsFboRenderMipMap();

	// �Ƿ�ΪTegra оƬ
	bool IsTegraGPU();

	// �Ƿ�ΪAdreno оƬ
	bool IsAdrenoGPU();

	// �Ƿ�ΪPowerVR оƬ
	bool IsPowerVRGPU();

	// �Ƿ�ΪMail оƬ
	bool IsMaliGPU();

	// �Ƿ�Ϊ��������GPU
	bool IsPartnerGPU();

	// �Ƿ�֧��16λ���㶥������
	bool IsVertexFloat16Supported();

    // �Ƿ�֧��shadow2d2DEXT ��shadow2DProjExt����
	bool IsShadowSamplersSupported();

	// �Ƿ�֧�������������
	bool IsTextureFilterAnisotropicSupported();

	// �Ƿ�֧��texture2DLodExt,texture2DProjLodEXT,texture2DProjLodEXT,textureCubeLodEXT ����.����Ҫ�ǿ������Թ��˺���Ҫʹ�����ֲ�����ʽ��
	bool IsShaderTextureLodSupported();

	// �Ƿ�֧���ڵ���ѯ
	bool IsOcclusionQuerySupported();

	// �Ƿ�֧��glMapBuffer
	bool IsMapBufferSupported();

	// �Ƿ�֧��gamma correction
	bool IsSRGBSupported();

	// �Ƿ�֧�ֶ���FrameBuffer������
	bool IsDiscardFrameBufferSupported();

	// ��ȡ�������ߴ�
	int GetTextureMaxSize() const;

	// ��ȡ�������������Ŀ
	int GetVertexAttribsMaxNum() const;

	// ��ȡ���������Ĵ��������Ŀ
	int GetVertexUniformVectorsMaxNum() const;

	// ��ȡƬ�������Ĵ��������Ŀ
	int GetFragmentUniformVectorsMaxNum() const;

	// ��ȡ�ɱ������Ĵ��������Ŀ
	int GetVaryingVectorsMaxNum() const;

    // ��ȡ������������������Ŀ
	int GetVertexTextureSamplerMaxNum() const;

	// ��ȡƬ����������������Ŀ
	int GetFragmentTextureSamplerMaxNum() const;

	// ��ȡ�����������������Ŀ
	int GetCombindTextureSamplerMaxNum() const;

	// ��ȡ�豸֧����Ϣ
	result_string GetDeviceInfoString();

	// ��ӡ���豸��Ϣ
	void PrintDeviceInfoString();

    // ��ȡ��ǰcolor depth stencil 
	result_string GetCurrentColor_Depth_Stencil_BitInfoString();

	// ����FrameBuffer�󶨵�����,�Ż������⴦��
	void DiscardFramebufferEXT(fm_sizei numAttachments, const fm_enum *attachments);

	void* MapBufferOES(fm_enum target, fm_enum access);

    fm_boolean UnmapBufferOES(fm_enum target);

	void GetBufferPointervOES(fm_enum target, fm_enum pname, fm_void **params);

	// ��ѯ֧�ֵ�ǰMapBuffer���ݵĶ�Ȩ�޲�����Ȩ��ֵ
	bool IsMapBufferReadAccess(fm_enum target,fm_int& access);
    
	// �Ƿ�֧��BUFFER_MAPPED ָ������
	bool IsMapBufferPointerSupported();

    void GenQueriesEXT(fm_sizei n, fm_uint *ids);

	void DeleteQueriesEXT(fm_sizei n, const fm_uint *ids);

    fm_boolean IsQueryEXT(fm_uint id);

	void BeginQueryEXT(fm_enum target, fm_uint id);

	void EndQueryEXT(fm_enum target);

	void GetQueryivEXT(fm_enum target, fm_enum pname, fm_int *params);

	void GetQueryObjectuivEXT(fm_uint id, fm_enum pname, fm_uint *params);
 
private:
	// ����豸��Ϣ
	bool GetDeviceInfo();

	bool IsGLExtensionSupported(const char* const extension);

	// ɨ��GPU������Ϣ
	void GetGpuInfo();
private:
   bool m_bPVRSupported;
   bool m_bETCSupported;
   bool m_bATCSupported;
   bool m_bDDSSupported;
   bool m_bBGRA8888Supported;
   bool m_bBinaryShaderSupported;
   bool m_bDepthTextureSupported;
   bool m_bTexture3DSupported;
   bool m_bTextureNPOTSupported;
   bool m_bDepth24Stencil8Supported;
   bool m_bRGB888_RGBA8888Supported;
   bool m_bRGBA8888Supported;
   bool m_bDepth24Supported;
   bool m_bDepth32Supported;
   bool m_bStencil1Supported;
   bool m_bStencil4Supported;
   bool m_bTextureRGSupported;
   bool m_bVertex32BitIndexSupported;
   bool m_bMapBufferSupported;
   bool m_bSRGBSupported;
   bool m_bFboRenderMipMap;
   bool m_bTextureFloat16Supported;
   bool m_bTextureFloat16LinearSupported;
   bool m_bTextureFloat32Supported;
   bool m_bTextureFloat32LinearSupported;
   bool m_bTextureFloat16ColorBufferSupported;
   bool m_bTextureFloat32ColorBufferSupported;
   bool m_bVertexFloat16Supported;
   bool m_bShadowSamplersSupported;
   bool m_bTextureFilterAnisotropicSupported;
   bool m_bShaderTextureLodSupported;
   bool m_bOcclusionQuerySupported;
   bool m_bDiscardFrameBufferSupported;
   int  m_nFragmentTextureSamplerMaxNum;
   int  m_nVertexTextureSamplerMaxNum;
   int  m_nCombindTextureSamplerMaxNum;
   int  m_nVaryingVectorsMaxNum;
   int  m_nFragmentUniformVectorsMaxNum;
   int  m_nVertexUniformVectorsMaxNum;
   int  m_nVertexAttribsMaxNum;
   int  m_nTextureMaxSize;

   //������һ�¹���GPUӲ��ʶ��ı��
   bool m_bTegraGPU;
   bool m_bAdrenoGPU;
   bool m_bPowerVRGPU;
   bool m_bMaliGPU;

   bool m_bPartnerGPU;
//#ifndef FX_SYSTEM_IOS
   /* GL_EXT_discard_framebuffer */
   PFNGLDISCARDFRAMEBUFFEREXT _glDiscardFramebufferEXT;
//#endif

    PFNGLMAPBUFFEROES                   _glMapBufferOES;
    PFNGLUNMAPBUFFEROES                 _glUnmapBufferOES;
    PFNGLGETBUFFERPOINTERVOES           _glGetBufferPointervOES;

	PFNGLGENQUERIESEXT                  _glGenQueriesEXT;
	PFNGLDELETEQUERIESEXT               _glDeleteQueriesEXT;
	PFNGLISQUERYEXT                     _glIsQueryEXT;
	PFNGLBEGINQUERYEXT                  _glBeginQueryEXT;
	PFNGLENDQUERYEXT                    _glEndQueryEXT;
	PFNGLGETQUERYIVEXT                  _glGetQueryivEXT;
	PFNGLGETQUERYOBJECTUIVEXT           _glGetQueryObjectuivEXT;
};

/*
������չ���ͨ����չ����glMapBuffer���buffer���������

����˵�������ݵĻ�ȡ

��ȡ��ǰVB�����ݴ�С
	GLint vb_size = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_SIZE,&vb_size);

��ȡ��ǰVB,IB������ָ�������������
����ͨ��2�ַ�ʽ��ȡ 
 
 ���淶���Ի�ȡVB����Ϊ��,IB�Ļ�ȡ��ʽ��ͬ

    1> ͨ�� glMapBufferOES ������ȡ
	char* pCopyData = new char[vb_size];
	GLint accflat = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_ACCESS_OES,&accflat);//������88ba��ʾ���Զ�д
	
	if( GL_WRITE_ONLY_OES != GL_BUFFER_ACCESS_OES ) //��ѯ��ȡȨ��
	{

		void* pData  = glMapBufferOES(GL_ARRAY_BUFFER,accflat);
		memcpy(pCopyData,pData,vb_size);
		glUnmapBufferOES(GL_ARRAY_BUFFER);
	}

	delete [] pCopyData;

	2> ͨ�� glGetBufferPointervOES ��ȡ���ݵ�ַ
	GLint canmap = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_MAPPED_OES,&canmap);
	void* pVBData = NULL;//����ָ��ĵ�ַ
	if( canmap == GL_TRUE )
	{
	   glGetBufferPointervOE( GL_ARRAY_BUFFER , GL_BUFFER_MAP_POINTER_OES , &pVBData );

	   	if( pVBData != NULL )
		{
			//��ȡ���ݵ�ַ�ɹ�
		}
	}

	if( pVBData != NULL )
	{
	    //���û�������򷵻�NULL
	}

*/

#endif // _DEVICE_CAPS_H


