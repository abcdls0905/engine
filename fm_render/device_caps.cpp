//--------------------------------------------------------------------
// 文件名:		device_caps.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "device_caps.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/var_list.h"

#include "render_utils.h"

#ifdef FX_SYSTEM_LINUX
#include "nv_syscaps.h"
#endif

 
/// \file device_caps.cpp
/// \brief 显示设备属性

/// entity: DeviceCaps
/// \brief 显示设备属性实体
DECLARE_ENTITY(DeviceCaps, 0, IEntity);

/// entity: DeviceCaps
//  \brief 获取设备信息
DECLARE_PROPERTY_GET(result_string, DeviceCaps, DeviceInfoString, GetDeviceInfoString);

/// entity: DeviceCaps
//  \brief 是否支持pvrt硬解 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsPVRTCSupported,IsPVRTCSupported);

/// entity: DeviceCaps
//  \brief 是否支持etc硬解 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsETCSupported,IsETCSupported);

/// entity: DeviceCaps
//  \brief 是否支持ATC格式
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsATCSupported,IsATCSupported);

/// entity: DeviceCaps
//  \brief  是否支持DDS格式
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDDSSupported,IsDDSSupported);

/// entity: DeviceCaps
//  \brief 是否支持BGRA8888 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsBGRA8888Supported,IsBGRA8888Supported);

/// entity: DeviceCaps
//  \brief 是否支持RGB888
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsRGB888_RGBA8888Supported,IsRGB888_RGBA8888Supported);

/// entity: DeviceCaps
//  \brief 是否支持RGBA8888
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsRGBA8888Supported,IsRGBA8888Supported);

/// 是否支持深度图采样
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDepthTextureSupported,IsDepthTextureSupported);

/// entity: DeviceCaps
//  \brief 是否支持深度模板
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDepth24Stencil8Supported,IsDepth24Stencil8Supported);

/// entity: DeviceCaps
//  \brief 是否支持纹理各项异性
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFilterAnisotropicSupported,IsTextureFilterAnisotropicSupported);

/// entity: DeviceCaps
//  \brief 是否支持texture2DLodExt,texture2DProjLodEXT,texture2DProjLodEXT,textureCubeLodEXT 采样.（主要是开启异性过滤后需要使用这种采样方式）
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsShaderTextureLodSupported,IsShaderTextureLodSupported);

/// entity: DeviceCaps
//  \brief 是否支持16位浮点纹理 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat16Supported,IsTextureFloat16Supported);

/// entity: DeviceCaps
//  \brief 是否支持16位浮点纹理线性插值 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat16LinearSupported,IsTextureFloat16LinearSupported);

/// entity: DeviceCaps
//  \brief 是否支持32位浮点纹理线性插值 
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat32LinearSupported,IsTextureFloat32LinearSupported);

/// entity: DeviceCaps
//  \brief 是否支持16位浮点纹理作为ColorRT
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat16ColorBufferSupported,IsTextureFloat16ColorBufferSupported);

/// entity: DeviceCaps
//  \brief 是否支持32位浮点纹理作为ColorRT
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat32ColorBufferSupported,IsTextureFloat32ColorBufferSupported);

/// entity: DeviceCaps
//  \brief 是否支持RG双通道纹理(主要用于法线)
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureRGSupported,IsTextureRGSupported);

/// entity: DeviceCaps
//  \brief 是否支持32位浮点纹理
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTextureFloat32Supported,IsTextureFloat32Supported);

/// entity: DeviceCaps
//  \brief 是否支持24位深度
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDepth24Supported,IsDepth24Supported);

/// entity: DeviceCaps
//  \brief 是否支持8位模板
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsStencil1Supported,IsStencil1Supported);

/// entity: DeviceCaps
//  \brief 是否支持8位模板
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsStencil4Supported,IsStencil4Supported);

/// entity: DeviceCaps
//  \brief 是否支持shadow2d2DEXT 和shadow2DProjExt采样
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsShadowSamplersSupported,IsShadowSamplersSupported);

/// entity: DeviceCaps
//  \brief 是否支持遮挡查询
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsOcclusionQuerySupported,IsOcclusionQuerySupported);

/// entity: DeviceCaps
//  \brief 是否支持FBO Render MipMap 将FBO渲染到一张带MipMap的贴图中
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsFboRenderMipMap,IsFboRenderMipMap);

/// entity: DeviceCaps
//  \brief 是否支持32位深度
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDepth32Supported,IsDepth32Supported);

/// entity: DeviceCaps
//  \brief 是否支持FrameBuffer丢弃
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsDiscardFrameBufferSupported,IsDiscardFrameBufferSupported);

/// entity: DeviceCaps
//  \brief是否支持16位浮点顶点数据
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsVertexFloat16Supported,IsVertexFloat16Supported);

/// entity: DeviceCaps
//  \brief是否支持是否支持32位顶点索引
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsVertex32BitIndexSupported,IsVertex32BitIndexSupported);

/// entity: DeviceCaps
//  \brief是否支持gamma correction
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsSRGBSupported,IsSRGBSupported);

/// entity: DeviceCaps
//  \brief获取片段纹理采样器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,FragmentTextureSamplerMaxNum,GetFragmentTextureSamplerMaxNum);

/// entity: DeviceCaps
//  \brief获取顶点纹理采样器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,VertexTextureSamplerMaxNum,GetVertexTextureSamplerMaxNum);

/// entity: DeviceCaps
//  \brief获取可变向量寄存器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,VaryingVectorsMaxNum,GetVaryingVectorsMaxNum);

/// entity: DeviceCaps
//  \brief获取片段向量寄存器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,FragmentUniformVectorsMaxNum,GetFragmentUniformVectorsMaxNum);

/// entity: DeviceCaps
//  \brief获取顶点向量寄存器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,VertexUniformVectorsMaxNum,GetVertexUniformVectorsMaxNum);

/// entity: DeviceCaps
//  \brief获取顶点属性最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,VertexAttribsMaxNum,GetVertexAttribsMaxNum);

/// entity: DeviceCaps
//  \brief获取纹理最大尺寸
DECLARE_PROPERTY_GET(int,DeviceCaps,TextureMaxSize,GetTextureMaxSize);

/// entity: DeviceCaps
//  \brief获取组合纹理采样器最大数目
DECLARE_PROPERTY_GET(int,DeviceCaps,CombindTextureSamplerMaxNum,GetCombindTextureSamplerMaxNum);

// 开始标记是否支持各个GPU厂商个识别串
/// entity: DeviceCaps
//  \brief 是否为Tegra 芯片
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsTegraGPU,IsTegraGPU);

//  \brief 是否为Adreno 芯片
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsAdrenoGPU,IsAdrenoGPU);

//  \brief 是否为PowerVR 芯片
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsPowerVRGPU,IsPowerVRGPU);

//  \brief 是否为Mail 芯片
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsMaliGPU,IsMaliGPU);

//  \brief 是否为合作伙伴的GPU
DECLARE_PROPERTY_GET(bool,DeviceCaps,IsPartnerGPU,IsPartnerGPU);

DeviceCaps::DeviceCaps()
{
	m_bTegraGPU = false;
//#ifndef FX_SYSTEM_IOS
	_glDiscardFramebufferEXT = NULL;
//#endif

	_glMapBufferOES = NULL;
	_glUnmapBufferOES = NULL;
	_glGetBufferPointervOES = NULL;

	_glGenQueriesEXT = NULL;
	_glDeleteQueriesEXT = NULL;
	_glIsQueryEXT = NULL;
	_glBeginQueryEXT = NULL;
	_glEndQueryEXT = NULL;
	_glGetQueryivEXT = NULL;
	_glGetQueryObjectuivEXT = NULL;
}

DeviceCaps::~DeviceCaps()
{
}


// 是否支持PVR格式
bool DeviceCaps::IsPVRTCSupported()
{
	return m_bPVRSupported;
}

// 是否支持ETC1格式
bool DeviceCaps::IsETCSupported()
{
	return m_bETCSupported;
}

// 是否支持ATC格式
bool DeviceCaps::IsATCSupported()
{
	return m_bATCSupported;
}

// 是否支持DDS格式
bool DeviceCaps::IsDDSSupported()
{
	return m_bDDSSupported;
}

// 是否支持BGRA8888格式
bool DeviceCaps::IsBGRA8888Supported()
{
	return m_bBGRA8888Supported;
}

bool DeviceCaps::IsRGB888_RGBA8888Supported()
{
	return m_bRGB888_RGBA8888Supported;
}

// 是否支持RGBA8
bool DeviceCaps::IsRGBA8888Supported()
{
	return m_bRGBA8888Supported;
}

// 是否支持16位浮点纹理
bool DeviceCaps::IsTextureFloat16Supported()
{
	return m_bTextureFloat16Supported;
}

// 是否支持16位浮点纹理线性插值
bool DeviceCaps::IsTextureFloat16LinearSupported()
{
	return m_bTextureFloat16LinearSupported;
}

// 是否支持32位浮点纹理
bool DeviceCaps::IsTextureFloat32Supported()
{
	return m_bTextureFloat32Supported;
}

// 是否支持32位浮点纹理线性过滤方式
bool DeviceCaps::IsTextureFloat32LinearSupported()
{
	return m_bTextureFloat32LinearSupported;
}

// 是否支持32位顶点索引
bool DeviceCaps::IsVertex32BitIndexSupported()
{
	return m_bVertex32BitIndexSupported;
}

// 是否支持浮点纹理的COLOR_HALF_FLOAT(确定浮点纹理是否支持FBO COLOR挂点)
bool DeviceCaps::IsTextureFloat16ColorBufferSupported()
{
	return m_bTextureFloat16ColorBufferSupported;
}

// 是否支持浮点纹理的COLOR_HALF_FLOAT(确定浮点纹理是否支持FBO COLOR挂点)
bool DeviceCaps::IsTextureFloat32ColorBufferSupported()
{
	return m_bTextureFloat32ColorBufferSupported;
}

// 是否支持加载2进制shader
bool DeviceCaps::IsBinaryShaderSupported()
{
	return m_bBinaryShaderSupported;
}

// 是否支持把深度当作贴图
bool DeviceCaps::IsDepthTextureSupported()
{
	return m_bDepthTextureSupported;
}

// 是否支持深度和模板当作深度
bool DeviceCaps::IsDepth24Stencil8Supported()
{
	return m_bDepth24Stencil8Supported;
}

// 是否支持1位模板
bool DeviceCaps::IsStencil1Supported()
{
	return m_bStencil1Supported;
}

// 是否支持4位模板
bool DeviceCaps::IsStencil4Supported()
{
	return m_bStencil4Supported;
}

// 是否支持3D贴图
bool DeviceCaps::IsTexture3DSupported()
{
	return m_bTexture3DSupported;
}

// 是否支持贴图2指数大小 non-power-of-two (npot)
bool DeviceCaps::IsTextureNPOTSupported()
{
	return m_bTextureNPOTSupported;
}

// 是否支持RG双通道纹理(主要用于法线)
bool DeviceCaps::IsTextureRGSupported()
{
	return m_bTextureRGSupported;
}

// 是否支持24位深度
bool DeviceCaps::IsDepth24Supported()
{
	return m_bDepth24Supported;
}

// 是否支持32位深度
bool DeviceCaps::IsDepth32Supported()
{
	return m_bDepth32Supported;
}

// 是否支持FBO Render MipMap 将FBO渲染到一张带MipMap的贴图中
bool DeviceCaps::IsFboRenderMipMap()
{
	return m_bFboRenderMipMap;
}

// 是否为Tegra 芯片
bool DeviceCaps::IsTegraGPU()
{
	return m_bTegraGPU;
}

// 是否为Adreno 芯片
bool DeviceCaps::IsAdrenoGPU()
{
	return m_bAdrenoGPU;
}

// 是否为PowerVR 芯片
bool DeviceCaps::IsPowerVRGPU()
{
	return m_bPowerVRGPU;
}

// 是否为Mail 芯片
bool DeviceCaps::IsMaliGPU()
{
	return m_bMaliGPU;
}

// 是否为合作伙伴的GPU
bool DeviceCaps::IsPartnerGPU()
{
	return m_bPartnerGPU;
}


// 是否支持16位浮点顶点数据
bool DeviceCaps::IsVertexFloat16Supported()
{
	return m_bVertexFloat16Supported;
}

// 是否支持shadow2d2DEXT 和shadow2DProjExt采样
bool DeviceCaps::IsShadowSamplersSupported()
{
	return m_bShadowSamplersSupported;
}

// 是否支持纹理各项异性
bool DeviceCaps::IsTextureFilterAnisotropicSupported()
{
	return m_bTextureFilterAnisotropicSupported;
}

// 是否支持texture2DLodExt,texture2DProjLodEXT,texture2DProjLodEXT,textureCubeLodEXT 采样.（主要是开启异性过滤后需要使用这种采样方式）
bool DeviceCaps::IsShaderTextureLodSupported()
{
	return m_bShaderTextureLodSupported;
}

// 是否支持遮挡查询
bool DeviceCaps::IsOcclusionQuerySupported()
{
	return m_bOcclusionQuerySupported;
}

// 是否支持glMapBuffer
bool DeviceCaps::IsMapBufferSupported()
{
	return m_bMapBufferSupported;
}

bool DeviceCaps::IsSRGBSupported()
{
	return m_bSRGBSupported;
}

// 是否支持丢弃FrameBuffer绑定数据
bool DeviceCaps::IsDiscardFrameBufferSupported()
{
	return m_bDiscardFrameBufferSupported;
}

// 获取纹理最大尺寸
int DeviceCaps::GetTextureMaxSize() const
{
	return m_nTextureMaxSize;
}

// 获取顶点属性最大数目
int DeviceCaps::GetVertexAttribsMaxNum() const
{
	return m_nVertexAttribsMaxNum;
}

// 获取顶点向量寄存器最大数目
int DeviceCaps::GetVertexUniformVectorsMaxNum() const
{
	return m_nVertexUniformVectorsMaxNum;
}

// 获取片段向量寄存器最大数目
int DeviceCaps::GetFragmentUniformVectorsMaxNum() const
{
	return m_nFragmentUniformVectorsMaxNum;
}

// 获取可变向量寄存器最大数目
int DeviceCaps::GetVaryingVectorsMaxNum() const
{
	return m_nVaryingVectorsMaxNum;
}

// 获取顶点纹理采样器最大数目
int DeviceCaps::GetVertexTextureSamplerMaxNum() const
{
	return m_nVertexTextureSamplerMaxNum;
}

// 获取片段纹理采样器最大数目
int DeviceCaps::GetFragmentTextureSamplerMaxNum() const
{
	return m_nFragmentTextureSamplerMaxNum;
}

// 获取组合纹理采样器最大数目
int DeviceCaps::GetCombindTextureSamplerMaxNum() const
{
	return m_nCombindTextureSamplerMaxNum;
}


result_string DeviceCaps::GetDeviceInfoString()
{
	result_string strInfo = "\r\n[GPU Info]\r\n";
	strInfo+=(char*)glGetString(GL_RENDERER);//GPU 名字
	strInfo+="\r\n";
	strInfo+=(char*)glGetString(GL_VENDOR);//GPU 厂商
	strInfo+="\r\n";
	strInfo+=(char*)glGetString(GL_VERSION);//GPU 驱动版本号
#define PRINT_INFO_BOOL(info) strInfo+=#info;\
	                           strInfo+=":";\
							   if(m_b##info)\
                               strInfo+="true\r\n";\
							   else\
                               strInfo+="false\r\n";
	                          

	strInfo += "\r\n[DeviceCapsInfo]\r\n";
	PRINT_INFO_BOOL(PVRSupported);
	PRINT_INFO_BOOL(ETCSupported);
	PRINT_INFO_BOOL(ATCSupported);
	PRINT_INFO_BOOL(DDSSupported);
	PRINT_INFO_BOOL(BGRA8888Supported);
	PRINT_INFO_BOOL(TextureFloat16Supported);
	PRINT_INFO_BOOL(TextureFloat32Supported);
	PRINT_INFO_BOOL(TextureFloat16LinearSupported);
	PRINT_INFO_BOOL(TextureFloat32LinearSupported);

	PRINT_INFO_BOOL(TextureFloat16ColorBufferSupported);
	PRINT_INFO_BOOL(TextureFloat32ColorBufferSupported);

	PRINT_INFO_BOOL(BinaryShaderSupported);
	PRINT_INFO_BOOL(DepthTextureSupported);

	PRINT_INFO_BOOL(Texture3DSupported);
	PRINT_INFO_BOOL(TextureNPOTSupported);
	PRINT_INFO_BOOL(TextureFilterAnisotropicSupported);
	PRINT_INFO_BOOL(TextureRGSupported);

	PRINT_INFO_BOOL(Depth24Stencil8Supported);
	PRINT_INFO_BOOL(Stencil1Supported);
	PRINT_INFO_BOOL(Stencil4Supported);
	PRINT_INFO_BOOL(RGB888_RGBA8888Supported);
	PRINT_INFO_BOOL(RGBA8888Supported);

	PRINT_INFO_BOOL(Depth24Supported);
	PRINT_INFO_BOOL(Depth32Supported);

    PRINT_INFO_BOOL(FboRenderMipMap);

	PRINT_INFO_BOOL(VertexFloat16Supported);
	PRINT_INFO_BOOL(ShadowSamplersSupported);
	PRINT_INFO_BOOL(ShaderTextureLodSupported);
	PRINT_INFO_BOOL(OcclusionQuerySupported);

	PRINT_INFO_BOOL(SRGBSupported);
	PRINT_INFO_BOOL(DiscardFrameBufferSupported);

	PRINT_INFO_BOOL(PartnerGPU);


	char mbzTemp[256];
#define PRINT_INFO_INT(info) strInfo+=#info;\
	                         strInfo+=":";\
							 SafeSprintf(mbzTemp,sizeof(mbzTemp),"%d\r\n",m_n##info);\
							 strInfo+=mbzTemp;

	PRINT_INFO_INT(TextureMaxSize); 
	PRINT_INFO_INT(FragmentTextureSamplerMaxNum);   
	PRINT_INFO_INT(VertexTextureSamplerMaxNum); 
	PRINT_INFO_INT(CombindTextureSamplerMaxNum);
	PRINT_INFO_INT(VaryingVectorsMaxNum); 
	PRINT_INFO_INT(FragmentUniformVectorsMaxNum); 
	PRINT_INFO_INT(VertexUniformVectorsMaxNum); 
	PRINT_INFO_INT(VertexAttribsMaxNum); 

 
#undef PRINT_INFO_INT
#undef PRINT_INFO_BOOL
	return strInfo;
}

// 打印出设备信息
void DeviceCaps::PrintDeviceInfoString()
{
	result_string strInfo = "\r\n[GPU Info]\r\n";
	strInfo+=(char*)glGetString(GL_RENDERER);//GPU 名字
	strInfo+="\r\n";
	strInfo+=(char*)glGetString(GL_VENDOR);//GPU 厂商
	strInfo+="\r\n";
	strInfo+=(char*)glGetString(GL_VERSION);//GPU 驱动版本号
#define PRINT_INFO_BOOL(info) strInfo+=#info;\
	                           strInfo+=":";\
							   if(m_b##info)\
                               strInfo+="true\r\n";\
							   else\
                               strInfo+="false\r\n";
	                          

	strInfo += "\r\n[DeviceCapsInfo]\r\n";
	PRINT_INFO_BOOL(PVRSupported);
	PRINT_INFO_BOOL(ETCSupported);
	PRINT_INFO_BOOL(ATCSupported);
	PRINT_INFO_BOOL(DDSSupported);
	PRINT_INFO_BOOL(BGRA8888Supported);
	PRINT_INFO_BOOL(TextureFloat16Supported);
	PRINT_INFO_BOOL(TextureFloat32Supported);
	PRINT_INFO_BOOL(TextureFloat16LinearSupported);
	PRINT_INFO_BOOL(TextureFloat32LinearSupported);

	PRINT_INFO_BOOL(TextureFloat16ColorBufferSupported);
	PRINT_INFO_BOOL(TextureFloat32ColorBufferSupported);

	PRINT_INFO_BOOL(BinaryShaderSupported);
	PRINT_INFO_BOOL(DepthTextureSupported);

	PRINT_INFO_BOOL(Texture3DSupported);
	PRINT_INFO_BOOL(TextureNPOTSupported);
	PRINT_INFO_BOOL(TextureFilterAnisotropicSupported);
	PRINT_INFO_BOOL(TextureRGSupported);

	PRINT_INFO_BOOL(Depth24Stencil8Supported);
	PRINT_INFO_BOOL(Stencil1Supported);
	PRINT_INFO_BOOL(Stencil4Supported);
	PRINT_INFO_BOOL(RGB888_RGBA8888Supported);
	PRINT_INFO_BOOL(RGBA8888Supported);

	PRINT_INFO_BOOL(Depth24Supported);
	PRINT_INFO_BOOL(Depth32Supported);

    PRINT_INFO_BOOL(FboRenderMipMap);

	PRINT_INFO_BOOL(VertexFloat16Supported);
	PRINT_INFO_BOOL(ShadowSamplersSupported);
	PRINT_INFO_BOOL(ShaderTextureLodSupported);
	PRINT_INFO_BOOL(OcclusionQuerySupported);

	PRINT_INFO_BOOL(SRGBSupported);
	PRINT_INFO_BOOL(DiscardFrameBufferSupported);

	PRINT_INFO_BOOL(PartnerGPU);

	// 打印一部分
	CORE_TRACE( strInfo.c_str() );

	strInfo = "";

	char mbzTemp[256];
#define PRINT_INFO_INT(info) strInfo+=#info;\
	                         strInfo+=":";\
							 SafeSprintf(mbzTemp,sizeof(mbzTemp),"%d\r\n",m_n##info);\
							 strInfo+=mbzTemp;

	PRINT_INFO_INT(TextureMaxSize); 
	PRINT_INFO_INT(FragmentTextureSamplerMaxNum);   
	PRINT_INFO_INT(VertexTextureSamplerMaxNum); 
	PRINT_INFO_INT(CombindTextureSamplerMaxNum);
	PRINT_INFO_INT(VaryingVectorsMaxNum); 
	PRINT_INFO_INT(FragmentUniformVectorsMaxNum); 
	PRINT_INFO_INT(VertexUniformVectorsMaxNum); 
	PRINT_INFO_INT(VertexAttribsMaxNum); 

	CORE_TRACE( strInfo.c_str() );
 
#undef PRINT_INFO_INT
#undef PRINT_INFO_BOOL

	//打印出所有属性信息
	CORE_TRACE("[Extensions]");
	const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
	CORE_TRACE(extensions);
}

// 获取当前color depth stencil 
result_string DeviceCaps::GetCurrentColor_Depth_Stencil_BitInfoString()
{
	result_string strInfo ;
	fm_int depth_bits;
	fm_int stencil_bits;
	fm_int color_r_bits;
	fm_int color_g_bits;
	fm_int color_b_bits;
	fm_int color_a_bits;

	// 查询当前最大贴图支持大小
	//glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	//查询当前深度宽位度 和当前模板位宽度
	glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
	glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
	glGetIntegerv(GL_RED_BITS, &color_r_bits);
	glGetIntegerv(GL_GREEN_BITS, &color_g_bits);
	glGetIntegerv(GL_BLUE_BITS, &color_b_bits);
	glGetIntegerv(GL_ALPHA_BITS, &color_a_bits);

	// 输出当前framebuffer的深度和模板信息
	char mbzTemp[64];
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"ColorBufferBit: R:%d G:%d B:%d A:%d\r\n",color_r_bits,color_g_bits,color_b_bits,color_a_bits);
    strInfo+=mbzTemp;
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"DepthBufferBit: %d\r\n",depth_bits);
    strInfo+=mbzTemp;
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"StencilBufferBit: %d\r\n",stencil_bits);
    strInfo+=mbzTemp;
    return strInfo;
}

// 丢弃FrameBuffer绑定的数据,优化的特殊处理
void DeviceCaps::DiscardFramebufferEXT(fm_sizei numAttachments, const fm_enum *attachments)
{
	if( m_bDiscardFrameBufferSupported )
	{
//#ifdef FX_SYSTEM_IOS
//		glDiscardFramebufferEXT(GL_FRAMEBUFFER,numAttachments,attachments);	
//#else
		if(_glDiscardFramebufferEXT)
			_glDiscardFramebufferEXT(GL_FRAMEBUFFER,numAttachments,attachments);
//#endif
		TestErr("DiscardFramebufferEXT");
	}
}

void* DeviceCaps::MapBufferOES(fm_enum target, fm_enum access)
{
	if(!m_bMapBufferSupported || _glMapBufferOES == NULL)
		return NULL;

	return _glMapBufferOES(target,access);
}

fm_boolean DeviceCaps::UnmapBufferOES(fm_enum target)
{
	if(!m_bMapBufferSupported || _glUnmapBufferOES == NULL)
		return false;

	return _glUnmapBufferOES( target );
}

void DeviceCaps::GetBufferPointervOES(fm_enum target, fm_enum pname, fm_void **params)
{
	if(!m_bMapBufferSupported || _glGetBufferPointervOES == NULL)
		return;

	_glGetBufferPointervOES( target , pname , params);
}

// 查询当前
bool DeviceCaps::IsMapBufferReadAccess(fm_enum target,fm_int& access)
{
#ifdef RENDER_ES_3_0
//	glGetBufferParameteriv(target,GL_BUFFER_ACCESS_FLAGS,&access);//返回是88ba表示可以读写
//	return GL_WRITE_ONLY != access;//如是只是可写权限
	return false;
#else
	glGetBufferParameteriv(target,GL_BUFFER_ACCESS_OES,&access);//返回是88ba表示可以读写
	return GL_WRITE_ONLY_OES != access;//如是只是可写权限
#endif
}

// 是否支持BUFFER_MAPPED 指针数据
bool DeviceCaps::IsMapBufferPointerSupported()
{
	fm_int canmap = 0;
#ifdef RENDER_ES_3_0
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_MAPPED,&canmap);
#else
	glGetBufferParameteriv(GL_ARRAY_BUFFER,GL_BUFFER_MAPPED_OES,&canmap);
#endif
	return canmap == GL_TRUE;
}

void DeviceCaps::GenQueriesEXT(fm_sizei n, fm_uint *ids)
{
	if( !m_bOcclusionQuerySupported || _glGenQueriesEXT == NULL )
		return;
	_glGenQueriesEXT(n,ids);
}

void DeviceCaps::DeleteQueriesEXT(fm_sizei n, const fm_uint *ids)
{
	if( !m_bOcclusionQuerySupported || _glDeleteQueriesEXT == NULL )
		return;
	_glDeleteQueriesEXT(n,ids);
}

fm_boolean DeviceCaps::IsQueryEXT(fm_uint id)
{
	if( !m_bOcclusionQuerySupported || _glIsQueryEXT == NULL )
		return false;
	return _glIsQueryEXT(id);
}

void DeviceCaps::BeginQueryEXT(fm_enum target, fm_uint id)
{
	if( !m_bOcclusionQuerySupported || _glBeginQueryEXT == NULL )
		return ;
	 _glBeginQueryEXT(target,id);
}

void DeviceCaps::EndQueryEXT(fm_enum target)
{
	if( !m_bOcclusionQuerySupported || _glEndQueryEXT == NULL )
		return ;
	_glEndQueryEXT( target );
}

void DeviceCaps::GetQueryivEXT(fm_enum target, fm_enum pname, fm_int *params)
{
	if( !m_bOcclusionQuerySupported || _glGetQueryivEXT == NULL )
		return ;
	_glGetQueryivEXT( target , pname , params );
}

void DeviceCaps::GetQueryObjectuivEXT(fm_uint id, fm_enum pname, fm_uint *params)
{
	if( !m_bOcclusionQuerySupported || _glGetQueryObjectuivEXT == NULL )
		return ;
	_glGetQueryObjectuivEXT( id , pname , params );
}

bool DeviceCaps::GetDeviceInfo()
{
	GetGpuInfo();
	m_bPVRSupported = IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
	m_bETCSupported = IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");

    m_bATCSupported = IsGLExtensionSupported("GL_AMD_compressed_ATC_texture");
	if(!m_bATCSupported)
		m_bATCSupported = IsGLExtensionSupported("GL_ATI_texture_compression_atitc");
 

	m_bDDSSupported = IsGLExtensionSupported("GL_EXT_texture_compression_s3tc");
	if(!m_bDDSSupported)
		m_bDDSSupported = IsGLExtensionSupported("GL_EXT_texture_compression_dxt1");

#ifdef RENDER_ES_3_0
	m_bBGRA8888Supported = true;
#else
	#ifdef FX_SYSTEM_IOS
		m_bBGRA8888Supported = IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
	#else
		m_bBGRA8888Supported = IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");	 
	#endif
#endif

    
#ifdef RENDER_ES_3_0
	m_bTextureFloat16Supported = true;
	m_bTextureFloat32Supported = true;
#else
	m_bTextureFloat16Supported =  IsGLExtensionSupported("GL_OES_texture_half_float");	 
	m_bTextureFloat32Supported =  IsGLExtensionSupported("GL_OES_texture_float");	
#endif

#ifdef RENDER_ES_3_0
	m_bTextureFloat16ColorBufferSupported = true;
	m_bTextureFloat32ColorBufferSupported = true;
#else
	m_bTextureFloat16ColorBufferSupported = IsGLExtensionSupported("GL_EXT_color_buffer_half_float");
	m_bTextureFloat32ColorBufferSupported = IsGLExtensionSupported("GL_EXT_color_buffer_float");
#endif

#ifdef RENDER_ES_3_0
	m_bBinaryShaderSupported = true;
#else
	m_bBinaryShaderSupported = IsGLExtensionSupported("GL_OES_get_program_binary");
#endif

#ifdef RENDER_ES_3_0
	m_bDepthTextureSupported = true;
#else
	m_bDepthTextureSupported = IsGLExtensionSupported("GL_OES_depth_texture");
#endif

#ifdef RENDER_ES_3_0
	m_bTexture3DSupported = true;
#else
	m_bTexture3DSupported = IsGLExtensionSupported("GL_OES_texture_3D");
#endif

	m_bTextureNPOTSupported = IsGLExtensionSupported("GL_OES_texture_npot");

	m_bTextureRGSupported = IsGLExtensionSupported("GL_EXT_texture_rg");

#ifdef RENDER_ES_3_0
	m_bDepth24Stencil8Supported = true;
#else
	m_bDepth24Stencil8Supported = IsGLExtensionSupported("GL_OES_packed_depth_stencil");
#endif

#ifdef RENDER_ES_3_0
	m_bRGB888_RGBA8888Supported = true;
#else
	m_bRGB888_RGBA8888Supported = IsGLExtensionSupported("GL_OES_rgb8_rgba8");
#endif

//#ifdef FX_SYSTEM_WINDOWS
//	m_bRGBA8888Supported = true;
//#else
	if(m_bRGB888_RGBA8888Supported)
	{
		m_bRGBA8888Supported = true;	
	}
	else
	{
		m_bRGBA8888Supported = IsGLExtensionSupported("GL_ARM_rgba8");
	}
//#endif

	m_bVertex32BitIndexSupported = IsGLExtensionSupported("GL_OES_element_index_uint");

	m_bDiscardFrameBufferSupported = IsGLExtensionSupported("GL_EXT_discard_framebuffer");

	if(m_bDiscardFrameBufferSupported)
	{ 
#ifdef FX_SYSTEM_IOS
		_glDiscardFramebufferEXT = glDiscardFramebufferEXT;
#else
		_glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXT)eglGetProcAddress("glDiscardFramebufferEXT");
#endif
	}

#ifdef RENDER_ES_3_0
	m_bDepth24Supported = true;
#else
	m_bDepth24Supported = IsGLExtensionSupported("GL_OES_depth24");
#endif

#ifdef RENDER_ES_3_0
	m_bDepth24Supported = true;
#else
	m_bDepth32Supported = IsGLExtensionSupported("GL_OES_depth32");
#endif

	m_bTextureFloat16LinearSupported = IsGLExtensionSupported("GL_OES_texture_half_float_linear");
	m_bTextureFloat32Supported = IsGLExtensionSupported("GL_OES_texture_float_linear");

	m_bFboRenderMipMap = IsGLExtensionSupported("GL_OES_fbo_render_mipmap");

#ifdef RENDER_ES_3_0
	m_bStencil4Supported = false;
	m_bStencil1Supported = false;
#else
	// 是否支持模板位数
	m_bStencil4Supported = IsGLExtensionSupported("GL_OES_stencil4");

	m_bStencil1Supported = IsGLExtensionSupported("GL_OES_stencil1");
#endif

	m_bVertexFloat16Supported =  IsGLExtensionSupported("GL_OES_vertex_half_float");

	m_bShadowSamplersSupported = IsGLExtensionSupported("GL_EXT_shadow_samplers");

	m_bTextureFilterAnisotropicSupported = IsGLExtensionSupported("GL_EXT_texture_filter_anisotropic");

	m_bShaderTextureLodSupported = IsGLExtensionSupported("GL_EXT_shader_texture_lod");

#ifdef RENDER_ES_3_0
	m_bOcclusionQuerySupported = true;
#else
	m_bOcclusionQuerySupported = IsGLExtensionSupported("GL_EXT_occlusion_query_boolean");
#endif

    if( m_bOcclusionQuerySupported )
	{
#ifdef FX_SYSTEM_IOS
		_glGenQueriesEXT = glGenQueriesEXT;
		_glDeleteQueriesEXT = glDeleteQueriesEXT;
		_glIsQueryEXT = glIsQueryEXT;
		_glBeginQueryEXT = glBeginQueryEXT;
		_glEndQueryEXT = glEndQueryEXT;
		_glGetQueryivEXT = glGetQueryivEXT;
		_glGetQueryObjectuivEXT = glGetQueryObjectuivEXT;
#else
		_glGenQueriesEXT = (PFNGLGENQUERIESEXT) eglGetProcAddress("glGenQueriesEXT");
		_glDeleteQueriesEXT = (PFNGLDELETEQUERIESEXT) eglGetProcAddress("glDeleteQueriesEXT");
		_glIsQueryEXT = (PFNGLISQUERYEXT) eglGetProcAddress("glIsQueryEXT");
		_glBeginQueryEXT = (PFNGLBEGINQUERYEXT) eglGetProcAddress("glBeginQueryEXT");
		_glEndQueryEXT = (PFNGLENDQUERYEXT) eglGetProcAddress("glEndQueryEXT");
		_glGetQueryivEXT = (PFNGLGETQUERYIVEXT) eglGetProcAddress("glGetQueryivEXT");
		_glGetQueryObjectuivEXT = (PFNGLGETQUERYOBJECTUIVEXT) eglGetProcAddress("glGetQueryObjectuivEXT");
#endif
	}

	m_bSRGBSupported = IsGLExtensionSupported("GL_EXT_sRGB");

	m_bMapBufferSupported = IsGLExtensionSupported("GL_OES_mapbuffer");

	if(m_bMapBufferSupported)
	{
#ifdef FX_SYSTEM_IOS
		_glMapBufferOES = glMapBufferOES;
		_glUnmapBufferOES = glUnmapBufferOES;
		_glGetBufferPointervOES = glGetBufferPointervOES;
#else
		_glMapBufferOES = (PFNGLMAPBUFFEROES) eglGetProcAddress("glMapBufferOES");
		_glUnmapBufferOES = (PFNGLUNMAPBUFFEROES) eglGetProcAddress("glUnmapBufferOES");
		_glGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOES) eglGetProcAddress("glGetBufferPointervOES");
#endif
	}
	// 
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_nTextureMaxSize);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_nFragmentTextureSamplerMaxNum);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &m_nVertexTextureSamplerMaxNum);

	glGetIntegerv(GL_MAX_VARYING_VECTORS, &m_nVaryingVectorsMaxNum);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &m_nFragmentUniformVectorsMaxNum);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS , &m_nVertexAttribsMaxNum);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &m_nVertexUniformVectorsMaxNum);

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_nCombindTextureSamplerMaxNum);

	return true;
}

bool DeviceCaps::Init(const IVarList& args)
{
	return GetDeviceInfo();
}

bool DeviceCaps::Shut()
{
	return true;
}

// 扫描GPU厂商信息
void DeviceCaps::GetGpuInfo()
{
	result_string strGPU_Name = (char*)glGetString(GL_RENDERER);//GPU名字
	result_string strGPU_Vendor = (char*)glGetString(GL_VENDOR);//GPU供应商名字
	result_string strGPU_DriveVer = (char*)glGetString(GL_VERSION);//GPU的驱动版本号

	m_bPartnerGPU = false;//默认所有GPU都不是合作伙伴

#ifdef FX_SYSTEM_LINUX

	if(nvVerifyTegraGpuFamilyStrict()>=0)
//	if( strGPU_Vendor == "NVIDIA Corporation")
	{
		m_bTegraGPU = true;
		m_bPartnerGPU = true;
		CORE_TRACE("this is nvidia tergra gpu");
	}
	else
	{
		m_bTegraGPU = false;
	}
#endif

	if( strGPU_Vendor == "ARM")
	{
		m_bMaliGPU = true;
	}
	else
	{
		m_bMaliGPU = false;
	}


	if( strGPU_Vendor == "Qualcomm")
	{
		m_bAdrenoGPU = true;
		m_bPartnerGPU = true;
	}
	else
	{
		m_bAdrenoGPU = false;
	}

	if( strGPU_Vendor == "Imagination Technologies")
	{
        m_bPowerVRGPU = true;
	}
	else
	{
		m_bPowerVRGPU = false;
	}

}

bool DeviceCaps::IsGLExtensionSupported(const char* const extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;

	extensions = glGetString(GL_EXTENSIONS);

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings, etc. */
	start = extensions;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}

	return false;
}