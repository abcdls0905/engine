//--------------------------------------------------------------------
// 文件名:		device_caps.h
// 内  容:		
// 说  明:		
// 创建日期:	20123年3月22日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _DEVICE_CAPS_H
#define _DEVICE_CAPS_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/vis_utils.h"
#include "render_header.h"

// 系统属性

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

	// 是否支持PVR格式
	bool IsPVRTCSupported();

	// 是否支持ETC1格式
	bool IsETCSupported();

	// 是否支持ATC格式
	bool IsATCSupported();

	// 是否支持DDS格式
	bool IsDDSSupported();

	// 是否支持BGRA8888格式
	bool IsBGRA8888Supported();

	// 是否支持RGB8和RGBA8
	bool IsRGB888_RGBA8888Supported();

	// 是否支持RGBA8
	bool IsRGBA8888Supported();

	// 是否支持加载2进制shader
	bool IsBinaryShaderSupported();

	// 是否支持把深度当作贴图
	bool IsDepthTextureSupported();

	// 是否支持24位深度和8位模板
	bool IsDepth24Stencil8Supported();

	// 是否支持1位模板
	bool IsStencil1Supported();

	// 是否支持4位模板
	bool IsStencil4Supported();

	// 是否支持3D贴图
	bool IsTexture3DSupported();

	// 是否支持贴图2指数大小 non-power-of-two (npot)
	bool IsTextureNPOTSupported();

	// 是否支持RG双通道纹理(主要用于法线)
	bool IsTextureRGSupported();

	// 是否支持16位浮点纹理
	bool IsTextureFloat16Supported();

	// 是否支持16位浮点纹理线性插值
	bool IsTextureFloat16LinearSupported();

	// 是否支持浮点纹理的COLOR_HALF_FLOAT(确定浮点纹理是否支持FBO COLOR挂点)
	bool IsTextureFloat16ColorBufferSupported();

	// 是否支持浮点纹理的COLOR_HALF_FLOAT(确定浮点纹理是否支持FBO COLOR挂点)
	bool IsTextureFloat32ColorBufferSupported();

	// 是否支持32位浮点纹理
	bool IsTextureFloat32Supported();

	// 是否支持32位浮点纹理线性过滤方式
	bool IsTextureFloat32LinearSupported();

	// 是否支持32位顶点索引
	bool IsVertex32BitIndexSupported();

	// 是否支持24位深度
	bool IsDepth24Supported();

	// 是否支持32位深度
	bool IsDepth32Supported();

	// 是否支持FBO Render MipMap 将FBO渲染到一张带MipMap的贴图中
	bool IsFboRenderMipMap();

	// 是否为Tegra 芯片
	bool IsTegraGPU();

	// 是否为Adreno 芯片
	bool IsAdrenoGPU();

	// 是否为PowerVR 芯片
	bool IsPowerVRGPU();

	// 是否为Mail 芯片
	bool IsMaliGPU();

	// 是否为合作伙伴的GPU
	bool IsPartnerGPU();

	// 是否支持16位浮点顶点数据
	bool IsVertexFloat16Supported();

    // 是否支持shadow2d2DEXT 和shadow2DProjExt采样
	bool IsShadowSamplersSupported();

	// 是否支持纹理各项异性
	bool IsTextureFilterAnisotropicSupported();

	// 是否支持texture2DLodExt,texture2DProjLodEXT,texture2DProjLodEXT,textureCubeLodEXT 采样.（主要是开启异性过滤后需要使用这种采样方式）
	bool IsShaderTextureLodSupported();

	// 是否支持遮挡查询
	bool IsOcclusionQuerySupported();

	// 是否支持glMapBuffer
	bool IsMapBufferSupported();

	// 是否支持gamma correction
	bool IsSRGBSupported();

	// 是否支持丢弃FrameBuffer绑定数据
	bool IsDiscardFrameBufferSupported();

	// 获取纹理最大尺寸
	int GetTextureMaxSize() const;

	// 获取顶点属性最大数目
	int GetVertexAttribsMaxNum() const;

	// 获取顶点向量寄存器最大数目
	int GetVertexUniformVectorsMaxNum() const;

	// 获取片段向量寄存器最大数目
	int GetFragmentUniformVectorsMaxNum() const;

	// 获取可变向量寄存器最大数目
	int GetVaryingVectorsMaxNum() const;

    // 获取顶点纹理采样器最大数目
	int GetVertexTextureSamplerMaxNum() const;

	// 获取片段纹理采样器最大数目
	int GetFragmentTextureSamplerMaxNum() const;

	// 获取组合纹理采样器最大数目
	int GetCombindTextureSamplerMaxNum() const;

	// 获取设备支持信息
	result_string GetDeviceInfoString();

	// 打印出设备信息
	void PrintDeviceInfoString();

    // 获取当前color depth stencil 
	result_string GetCurrentColor_Depth_Stencil_BitInfoString();

	// 丢弃FrameBuffer绑定的数据,优化的特殊处理
	void DiscardFramebufferEXT(fm_sizei numAttachments, const fm_enum *attachments);

	void* MapBufferOES(fm_enum target, fm_enum access);

    fm_boolean UnmapBufferOES(fm_enum target);

	void GetBufferPointervOES(fm_enum target, fm_enum pname, fm_void **params);

	// 查询支持当前MapBuffer数据的读权限并返回权限值
	bool IsMapBufferReadAccess(fm_enum target,fm_int& access);
    
	// 是否支持BUFFER_MAPPED 指针数据
	bool IsMapBufferPointerSupported();

    void GenQueriesEXT(fm_sizei n, fm_uint *ids);

	void DeleteQueriesEXT(fm_sizei n, const fm_uint *ids);

    fm_boolean IsQueryEXT(fm_uint id);

	void BeginQueryEXT(fm_enum target, fm_uint id);

	void EndQueryEXT(fm_enum target);

	void GetQueryivEXT(fm_enum target, fm_enum pname, fm_int *params);

	void GetQueryObjectuivEXT(fm_uint id, fm_enum pname, fm_uint *params);
 
private:
	// 获得设备信息
	bool GetDeviceInfo();

	bool IsGLExtensionSupported(const char* const extension);

	// 扫描GPU厂商信息
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

   //下面是一下关于GPU硬件识别的标记
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
关于扩展如何通过扩展函数glMapBuffer获得buffer对象的数据

比如说顶点数据的获取

获取当前VB的数据大小
	GLint vb_size = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_SIZE,&vb_size);

获取当前VB,IB的数据指针或者数据内容
可以通过2种方式获取 
 
 下面范例以获取VB数据为例,IB的获取方式类同

    1> 通过 glMapBufferOES 函数获取
	char* pCopyData = new char[vb_size];
	GLint accflat = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_ACCESS_OES,&accflat);//返回是88ba表示可以读写
	
	if( GL_WRITE_ONLY_OES != GL_BUFFER_ACCESS_OES ) //查询存取权限
	{

		void* pData  = glMapBufferOES(GL_ARRAY_BUFFER,accflat);
		memcpy(pCopyData,pData,vb_size);
		glUnmapBufferOES(GL_ARRAY_BUFFER);
	}

	delete [] pCopyData;

	2> 通过 glGetBufferPointervOES 获取数据地址
	GLint canmap = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_MAPPED_OES,&canmap);
	void* pVBData = NULL;//数据指向的地址
	if( canmap == GL_TRUE )
	{
	   glGetBufferPointervOE( GL_ARRAY_BUFFER , GL_BUFFER_MAP_POINTER_OES , &pVBData );

	   	if( pVBData != NULL )
		{
			//获取数据地址成功
		}
	}

	if( pVBData != NULL )
	{
	    //如果没有数据则返回NULL
	}

*/

#endif // _DEVICE_CAPS_H


