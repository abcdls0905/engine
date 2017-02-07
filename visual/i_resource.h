//--------------------------------------------------------------------
// 文件名:		i_resource.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_RESOURCE_H
#define _VISUAL_I_RESOURCE_H

#include "../public/macros.h"
#include "../visual/fm_define.h"

// 显示资源基类

class IResource
{
public:
	// 资源类型
	enum
	{
		TYPE_VERTEX_BUFFER,
		TYPE_STATIC_VB,
		TYPE_DYNAMIC_VB,
		TYPE_INDEX_BUFFER,
		TYPE_STATIC_IB,
		TYPE_DYNAMIC_IB,
		TYPE_SHADER_TEX,
		TYPE_RENDER_TEX,
		TYPE_DEPTH_TEX,
		TYPE_COLOR_TEX,
		TYPE_STENCIL_TEX,
		TYPE_DEPTH_STENCIL_TEX,
		TYPE_STATIC_TEX,
		TYPE_DYNAMIC_TEX,
		TYPE_STATIC_CUBE,
		TYPE_COLOR_RT,
		TYPE_DEPTH_RT,
		TYPE_DEPTH_STENCIL_RT,
		TYPE_FRAME_RT,
		TYPE_VERTEX_SHADER,
		TYPE_PIXEL_SHADER,
		TYPE_SHADER_PROGRAM,
		TYPE_TEXTURE_SAMPLER,
	};

public:
	virtual ~IResource() = 0;

	// 释放
	virtual void Release() = 0;
	// 获得资源类型
	virtual int GetResourceType() = 0;

private:
	// 销毁
	virtual void Destroy() = 0;

	
#ifdef PROCESS_LOST_DEVICE
	// 设备就绪时的处理
	virtual bool Restore(){return true;};
	// 设备丢失时的处理
	virtual bool Invalidate(){return true;};
#endif
	friend class Render;
};

inline IResource::~IResource() {}

// 顶点缓冲

class IVertexBuffer: public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_VERTEX_BUFFER; }

	// 获得缓冲区 
	fm_uint GetBuffer() const { return m_nBuffer; }
	// 获得缓冲区长度
	unsigned int GetSize() const { return m_nSize; }

protected:
	fm_uint m_nBuffer;
	unsigned int m_nSize;
#ifdef PROCESS_LOST_DEVICE
	void* m_pData;
#endif
};

// 静态顶点缓冲

class IStaticVB: public IVertexBuffer
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_STATIC_VB; }
};

// 动态顶点缓冲

class IDynamicVB: public IVertexBuffer
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_DYNAMIC_VB; }

	// 返回创建VB的时候的标识串
	virtual const char* GetIdent() = 0;
};

// 索引缓冲

class IIndexBuffer: public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_INDEX_BUFFER; }

	// 获得缓冲区 
	fm_uint GetBuffer() const { return m_nBuffer; }
	// 获得缓冲区长度
	unsigned int GetSize() const { return m_nSize; }
	// 获得格式
	int GetFormat() const { return m_nFormat; }

protected:
	fm_uint m_nBuffer;
#ifdef PROCESS_LOST_DEVICE
	void* m_pData;
#endif
	unsigned int m_nSize;
	int m_nFormat;
};

// 静态索引缓冲

class IStaticIB: public IIndexBuffer
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_STATIC_IB; }
};

// 动态索引缓冲

class IDynamicIB: public IIndexBuffer
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_DYNAMIC_IB; }
};


// 设置贴图采样器
class ITextureSampler: public IResource
{
public:
	// TextureMagFilter
	enum TEX_SAMPLER_MAG_FILTER
	{
		TS_MAG_NEAREST,
		TS_MAG_LINEAR,
	};

	// TextureMinFilter 
	enum TEX_SAMPLER_MIN_FILTER
	{
		TS_MIN_NEAREST,
		TS_MIN_LINEAR,
		TS_MIN_NEAREST_MIPMAP_NEAREST,
		TS_MIN_LINEAR_MIPMAP_NEAREST,
		TS_MIN_NEAREST_MIPMAP_LINEAR,
		TS_MIN_LINEAR_MIPMAP_LINEAR,
	};

	// UV type 
	enum TEX_WRAP_TYPE
	{
		TWT_U,
		TWT_V,
		TWT_MAX,
	};

	// TextureWrapMode 纹理包装方式
	enum TEX_WRAP_MODE
	{
		TWM_REPEAT,
		TWM_CLAMP_TO_EDGE,
		TWM_MIRRORED_REPEAT,
	};

	// TextureType 纹理类型
	enum TEX_TYPE
	{
		TEX_2D,
		TEX_3D,
		TEX_CUBE,
	};

	// 设置纹理过滤方式
	virtual void SetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min) = 0;
	virtual void GetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER& tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER& tex_min) = 0;

	// 设置纹理包装
	virtual void SetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode) = 0;
	virtual void GetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE& u_wrap_type,ITextureSampler::TEX_WRAP_MODE& v_wrap_mode) = 0;

	virtual ITextureSampler::TEX_TYPE GetTextureType() = 0;

	// 获得资源类型
	virtual int GetResourceType() { return TYPE_TEXTURE_SAMPLER; }
};


// 不可采样贴图
class IRenderTex:public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_RENDER_TEX; }

	// 获得可采样对象 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;

	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;
protected:
	fm_uint m_nRenderTex;
};

// 不可采样的深度
class IDepthTex:public IRenderTex
{
public:
	// 获得资源的类型
	virtual int GetResourceType() { return TYPE_DEPTH_TEX;};

	// 获得可采样对象 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;
	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;
};

// 不可以采样的深度
class IColorTex:public IRenderTex
{
public:
	// 获得资源的类型
	virtual int GetResourceType() { return TYPE_COLOR_TEX; };

	// 获得可采样对象 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;
	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;
};

// 不可采样的模板
class IStencilTex:public IRenderTex
{
public:
	// 获得资源的类型
	virtual int GetResourceType() { return TYPE_STENCIL_TEX ;};

	// 获得可采样对象 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;
	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;
};

// 具有深度和模板的一种特殊不可采样
class IDepthStencilTex:public IRenderTex
{
public:
	// 获得资源的类型
	virtual int GetResourceType() { return TYPE_DEPTH_STENCIL_TEX ;};

	// 获得可采样对象 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;
	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;
};

// 可采样贴图

class IShaderTex: public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_SHADER_TEX; }

	// 获得可采样对象 
	fm_uint GetTexture() const { return m_nTexture; }

	// 贴图宽度
	virtual unsigned int GetWidth() const = 0;

	// 贴图高度
	virtual unsigned int GetHeight() const = 0;
	// 贴图格式
	virtual TEX_FORMAT GetFormat() const = 0;

	// 获取贴图采样器操作
	virtual ITextureSampler* GetTextureSampler() const = 0;
protected:
	fm_uint m_nTexture;
};

// 静态贴图

class IStaticTex: public IShaderTex
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_STATIC_TEX; }
};

// 动态贴图

class IDynamicTex: public IShaderTex
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_DYNAMIC_TEX; }
};

// 静态立方体贴图

class IStaticCube: public IShaderTex
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_STATIC_CUBE; }
};

// 颜色渲染目标

class IColorRT: public IShaderTex
{
public:
	// 宽度
	virtual unsigned int GetWidth() const = 0;
	// 高度
	virtual unsigned int GetHeight() const = 0;
	// 格式
	virtual TEX_FORMAT GetFormat() const = 0;
	// 是否自动生成层级
	virtual bool GetAutoGenMipmap() const = 0;

	// 获得资源类型
	virtual int GetResourceType() { return TYPE_COLOR_RT; }
};

// 深度渲染目标

class IDepthRT: public IShaderTex
{
public:
	// 宽度
	virtual unsigned int GetWidth() const = 0;
	// 高度
	virtual unsigned int GetHeight() const = 0;
	// 格式
	virtual TEX_FORMAT GetFormat() const = 0;
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_DEPTH_RT; }
};

// 可采样的深度模板格式
class IDepthStencilRT: public IShaderTex
{
public:
	// 宽度
	virtual unsigned int GetWidth() const = 0;
	// 高度
	virtual unsigned int GetHeight() const = 0;
	// 格式
	virtual TEX_FORMAT GetFormat() const = 0;
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_DEPTH_STENCIL_RT; }
};

// 创建一个FrameBuffer
class IFrameRT : public IResource
{
public:
	//绑定状态
	enum BIND_STATE
	{
		BIND_NONE,//未绑定
		BIND_TEX,//绑定不可读贴图
		BIND_RT,//绑定可读纹理
	};


	// 获得资源类型
	virtual int GetResourceType() { return TYPE_FRAME_RT; }
	// Framebuffer对象
	fm_uint GetFrame() const { return m_nFrame; }

	// 获取IColorRT
	virtual IColorRT* GetColorRT() = 0;

	// 获取IDepthRT
	virtual IDepthRT* GetDepthRT() = 0;

	// 获取绑定的IColorTex
	virtual IColorTex* GetColorTex() = 0;

	// 获取绑定的DepthTex
	virtual IDepthTex* GetDepthTex() = 0;

	// 获取绑定的模版
	virtual IStencilTex* GetStencilTex() = 0;

	// 获取绑定的深度模板
	virtual IDepthStencilTex* GetDepthStencilTex() = 0 ;

	// 获取绑定的深度模板
	virtual IDepthStencilRT* GetDepthStencilRT() = 0;

	// 设置IColorRT
	virtual bool SetColorRT(IColorRT* pColorRT) = 0;

	// 设置IDepthRT
	virtual bool SetDepthRT(IDepthRT* pDepthRT) = 0;

	virtual bool SetColorTex(IColorTex* pColorTex) = 0;

	virtual bool SetDepthTex(IDepthTex* pDepthTex) = 0;

	virtual bool SetStencilTex(IStencilTex* pStencil) = 0;

	virtual bool SetDepthStencilTex(IDepthStencilTex* pDepthStencilTex) = 0;

	virtual bool SetDepthStencilRT(IDepthStencilRT* pDepthStencilRT) = 0;

	// 查询绑Frame当前的绑定的状态
	virtual IFrameRT::BIND_STATE GetColorState() = 0;
	virtual IFrameRT::BIND_STATE GetDepthState() = 0;
	virtual IFrameRT::BIND_STATE GetStencilState() = 0;

	// 是否绑定了DepthStencil
	virtual bool IsBindDepthStencil() = 0;

	// 使用当前FBO作为渲染对象
	virtual void UsedFrameRT() = 0;
protected:
	fm_uint m_nFrame;
};

// 顶点着色器

class IVertexShader: public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_VERTEX_SHADER; }

	// 获得着色器
	fm_uint GetShader() const { return m_nShader; }

	virtual const char* GetFileName() const = 0;

	virtual const char* GetDefine() const = 0;

	virtual const char* GetFuncName() const = 0;
protected:
	fm_uint m_nShader;
};

// 像素着色器

class IPixelShader: public IResource
{
public:
	// 获得资源类型
	virtual int GetResourceType() { return TYPE_PIXEL_SHADER; }

	// 获得着色器
	fm_uint GetShader() const { return m_nShader; }

	virtual const char* GetFileName() const = 0;

	virtual const char* GetDefine() const = 0;

	virtual const char* GetFuncName() const = 0;

protected:
	fm_uint m_nShader;
};


// shader常量操作
class IShaderParamOp
{
public:
	//根据寄存器变量名字获取索引值
	virtual fm_int FindParamIdByName(const char* pstrName) = 0;
	// bAuto  是否需要引擎底层自动管理设置

	virtual bool SetParamValue(fm_int Id,fm_float x,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_int x,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,fm_float w,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,fm_int w,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const fm_float* v,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const fm_int* v,bool bAuto = true) = 0;

	// 扩展引擎结合的数学库进行包装的
	virtual bool SetParamValue(fm_int Id,FmVec2& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmVec3& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmVec4& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmMat3& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmMat4& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec2* value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec3* value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec4* value,bool bAuto = true) = 0;

	// 设置纹理
	// 常量ID         纹理ID
	virtual bool SetTexture2D(fm_int Id,fm_int TexID) = 0;
	virtual bool SetTexture3D(fm_int Id,fm_int TexID) = 0;
	virtual bool SetTextureCube(fm_int Id,fm_int TexID) = 0;
};


// 渲染状态管理器(目前主要进行一些粗略的渲染状态设置和获取管理)
class IRenderStateOp
{
public:
	// CullFaceMode
	enum CULL_FACE_MODE
	{
		CULL_FRONT,
		CULL_BACK,
//		CULL_NONE,
	};

	// BlendColorMode mode rgb
	enum BLEND_FUNC_MODE
	{
		BLEND_FUNC_ADD,
		BLEND_FUNC_SUBTRACT,
		BLEND_FUNC_REVERSE_SUBTRACT,
	};

	// BlendColorFunc  src and dst
	enum BLEND_COLOR_FUNC
	{
		BLEND_COLOR_ZERO,
		BLEND_COLOR_ONE,
		BLEND_COLOR_SRC_COLOR,
		BLEND_COLOR_ONE_MINUS_SRC_COLOR,
		BLEND_COLOR_DST_COLOR,
		BLEND_COLOR_ONE_MINUS_DST_COLOR,
		BLEND_COLOR_SRC_ALPHA,
		BLEND_COLOR_ONE_MINUS_SRC_ALPHA,
		BLEND_COLOR_DST_ALPHA,
		BLEND_COLOR_ONE_MINUS_DST_ALPHA,
		BLEND_COLOR_CONSTANT_COLOR,
		BLEND_COLOR_ONE_MINUS_CONSTANT_COLOR,
		BLEND_COLOR_CONSTANT_ALPHA,
		BLEND_COLOR_ONE_MINUS_CONSTANT_ALPHA,
		BLEND_COLOR_SRC_ALPHA_SATURATE,//only src, srcRGB, srcAlpha:
	};

	// Depth Test Func 
	enum DEPTH_TEST_FUNC
	{
		DEPTH_TEST_NEVER,
		DEPTH_TEST_ALWAYS,
		DEPTH_TEST_LESS,
		DEPTH_TEST_LEQUAL,
		DEPTH_TEST_EQUAL,
		DEPTH_TEST_GREATER,
		DEPTH_TEST_GEQUAL,
		DEPTH_TEST_NOTEQUAL,
	};

	// Stencil Test Face
	enum STENCIL_TEST_FACE
	{
		STENCIL_TEST_FRONT,
		STENCIL_TEST_BACK,
 		STENCIL_TEST_FRONT_AND_BACK,//虽然底层支持双模板测试，但是底层对于双面模板测试的状态管理较为逻辑，对于返回值可能会false.(虽然支持，但是不推荐使用)
	};

	// Stencil Test Mode
	enum STENCIL_FUNC_MODE
	{
		STENCIL_FUNC_KEEP,
		STENCIL_FUNC_ZERO,
		STENCIL_FUNC_REPLACE,
		STENCIL_FUNC_INCR,
		STENCIL_FUNC_DECR,
		STENCIL_FUNC_INVERT,
		STENCIL_FUNC_INCR_WRAP,
		STENCIL_FUNC_DECR_WRAP,
	};

	// Stencil Test Func
	enum STENCIL_TEST_FUNC
	{
		STENCIL_TEST_NEVER,
		STENCIL_TEST_ALWAYS,
		STENCIL_TEST_LESS,
		STENCIL_TEST_LEQUAL,
		STENCIL_TEST_EQUAL,
		STENCIL_TEST_GREATER,
		STENCIL_TEST_GEQUAL,
		STENCIL_TEST_NOTEQUAL,
	};
public:
	//混合开关
	virtual bool IsEnableBlend() = 0;
	virtual void EnableBlend(bool bEnable) = 0;

	//深度写入
	virtual bool IsEnableDepthWrite() = 0;
	virtual void EnableDepthWrite(bool bEnable) = 0;

	//是否进行深度测试
	virtual bool IsEnableDepthTest() = 0;
	virtual void EnableDepthTest(bool bEnable) = 0;

	//是否颜色写入
	virtual bool IsColorWrite() = 0;
	virtual void EnableColorWrite(bool bEnable) = 0;

	//是否进行模板测试
	virtual bool IsStencilTestEnable() = 0;
	virtual void EnableStencilTest(bool bEnable) = 0;

	//是否开启拣选面
	virtual bool IsCullFaceEnable() = 0;
	virtual void EnableCullFace(bool bEnable) = 0;

	//剪裁测试
	virtual bool IsScissorTestEnable() = 0;
	virtual void EnableScissorTest(bool bEnable) = 0;

	//是否启动深度偏置(深度修正)
	virtual bool IsPolygonOffsetFillEnable() = 0;
	virtual void EnablePolygonOffsetFill(bool bEnable) = 0;

	//深度偏置参考值
	virtual void SetPolygonOffset(float factor,float units) = 0;
	virtual void GetPolygonOffset(float& factor,float& units) = 0;

	//三角形拣选面方式
	virtual void SetCullFaceModel(IRenderStateOp::CULL_FACE_MODE cull_face_model = CULL_BACK ) = 0;
	virtual IRenderStateOp::CULL_FACE_MODE GetCullFaceModel() = 0;

	//颜色写入控制
	virtual void SetColorWriteMask(bool bRed,bool bGreen,bool bBlue,bool bAlpha) = 0;
	virtual void GetColorWriteMask(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha) = 0;

	//阿法混合控制
	virtual void SetBlendColor(const FmVec4& blendcolor) = 0;
	virtual FmVec4 GetBlendColor( ) = 0;

	virtual	void SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha) = 0;
	virtual void GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha) = 0;
	virtual void SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha) = 0;
	virtual void GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC& srcRGB,IRenderStateOp::BLEND_COLOR_FUNC& srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC& dstRGB,IRenderStateOp::BLEND_COLOR_FUNC& dstAlpha) = 0;
	// 混合参数本质和上面是一样的只是为了用户习惯而已
	virtual void SetBlendEquation(IRenderStateOp::BLEND_FUNC_MODE mode) = 0;
	virtual void SetBlendFunc(IRenderStateOp::BLEND_COLOR_FUNC src,IRenderStateOp::BLEND_COLOR_FUNC dst) = 0;

	// 深度写入的测试方法
	virtual void SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func) = 0;

	// 获取当前深度测试的方法
	virtual IRenderStateOp::DEPTH_TEST_FUNC GetDepthTestFunc() = 0;

	// 模板写入的测试方法
	virtual void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask) = 0;
	virtual bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask) = 0;

	// 模板写入的操作方法
	virtual void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass) = 0;
	virtual bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass) = 0;

	// 模板写入遮罩值
	virtual void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask) = 0;
	virtual bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask) = 0;

	// 区域裁剪
	virtual void SetScissorTest(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) = 0;
	virtual void GetScissorTest(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) = 0;

	// 设置视口大小
	virtual void SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) = 0;
	// 获取当前视口大小
	virtual void GetViewPort(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) = 0;
};


// 渲染绘制接口
// 将所有和DrawCall 相关的函数直接关联起来统一管理 其中包括顶点数据和IB的数据监控和管理
// 渲染函数不提供人查询功能，但是底层 会自动过滤一些无效设置数据或者重复的数据存放。
class IRenderDrawOp
{
public:
	//Clear Buffer Flat
	enum CLEAR_BUFFER
	{
		CLEAR_COLOR_BUFFER = 1,
		CLEAR_DEPTH_BUFFER = 1<<1,
		CLEAR_STENCIL_BUFFER = 1<<2,
	};

	//Vertex Data Type
	enum VERTEX_DATA_TYPE
	{
		VERTEX_DATA_BYTE,
		VERTEX_DATA_UNSIGNED_BYTE,
		VERTEX_DATA_SHORT,
		VERTEX_DATA_UNSIGNED_SHORT,
		VERTEX_DATA_FIXED,
		VERTEX_DATA_FLOAT,
		VERTEX_DATA_HALF_FLOAT,//16位浮点 需要检测硬件是否支持
	};

	//Vertex Index Type
	enum VERTEX_INDEX_TYPE
	{
		VERTEX_INDEX_UNSIGNED,
		VERTEX_INDEX_UNSIGNED_SHORT,
	};

	//Draw Mode Type
	enum DRAW_MODE
	{
		DRAW_POINTS,
		DRAW_LINE_STRIP,
		DRAW_LINE_LOOP,
		DRAW_LINES,
		DRAW_TRIANGLE_STRIP,
		DRAW_TRIANGLE_FAN,
		DRAW_TRIANGLES,
	};
public:
	// clear buffer op
	virtual void SetClearColor(const FmVec4& vec4Color) = 0;
	virtual void SetClearDepth(float fDepth) = 0;
	virtual void SetClearStencil(int s) = 0;
	virtual void SetClear(fm_uint Flat) = 0;

	// vertex op
	// Set Vertex Attrib Format Info
	virtual void SetVertexAttribPointer(unsigned int index,int size,IRenderDrawOp::VERTEX_DATA_TYPE type, fm_sizei stride, const void* ptr ) = 0;

	virtual void EnableVertexAttribArray(unsigned int index,bool bEnable = true) = 0;
	virtual bool IsEnableVertexAttribArray(unsigned int index) = 0;

	// draw call func
	virtual void DrawArrays(IRenderDrawOp::DRAW_MODE draw_mode,int first,fm_sizei count) = 0;
	virtual void DrawIndex(IRenderDrawOp::DRAW_MODE draw_mode,fm_sizei count,IRenderDrawOp::VERTEX_INDEX_TYPE index_type,void* indices) = 0;

	// set vb
	virtual void SetVB(fm_uint vb) = 0;
	// set ib
	virtual void SetIB(fm_uint ib) = 0;
};


// 渲染状态记录器
class IRenderStateRecordOp
{
public:
	//开始记录状态
	virtual void Store() = 0;

	//开始恢复渲染状态
	virtual void Restore() = 0;
};

// 着色器程序

class IShaderProgram: public IResource
{
public:
	//获取shader的常量操作
	virtual IShaderParamOp* GetParamOp() const = 0;

	//获取渲染器状态操作
	virtual IRenderStateOp* GetRenderStateOp() const = 0;

	//渲染的顶点相关的操作
	virtual IRenderDrawOp*  GetRenderDrawOp() const =  0;

	virtual IVertexShader* GetVertexShader() const = 0;

    virtual IPixelShader*  GetPixelShader() const = 0;


	//渲染状态记录器
//	virtual IRenderStateRecordOp* GetRenderStateRecordOp() const = 0;

	//使用改shader
	virtual void UsedShader() = 0;

	// 获得资源类型
	virtual int GetResourceType() { return TYPE_SHADER_PROGRAM; }
};



#endif // _VISUAL_I_RESOURCE_H
