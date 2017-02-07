//--------------------------------------------------------------------
// �ļ���:		i_resource.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_RESOURCE_H
#define _VISUAL_I_RESOURCE_H

#include "../public/macros.h"
#include "../visual/fm_define.h"

// ��ʾ��Դ����

class IResource
{
public:
	// ��Դ����
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

	// �ͷ�
	virtual void Release() = 0;
	// �����Դ����
	virtual int GetResourceType() = 0;

private:
	// ����
	virtual void Destroy() = 0;

	
#ifdef PROCESS_LOST_DEVICE
	// �豸����ʱ�Ĵ���
	virtual bool Restore(){return true;};
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate(){return true;};
#endif
	friend class Render;
};

inline IResource::~IResource() {}

// ���㻺��

class IVertexBuffer: public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_VERTEX_BUFFER; }

	// ��û����� 
	fm_uint GetBuffer() const { return m_nBuffer; }
	// ��û���������
	unsigned int GetSize() const { return m_nSize; }

protected:
	fm_uint m_nBuffer;
	unsigned int m_nSize;
#ifdef PROCESS_LOST_DEVICE
	void* m_pData;
#endif
};

// ��̬���㻺��

class IStaticVB: public IVertexBuffer
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_STATIC_VB; }
};

// ��̬���㻺��

class IDynamicVB: public IVertexBuffer
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_DYNAMIC_VB; }

	// ���ش���VB��ʱ��ı�ʶ��
	virtual const char* GetIdent() = 0;
};

// ��������

class IIndexBuffer: public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_INDEX_BUFFER; }

	// ��û����� 
	fm_uint GetBuffer() const { return m_nBuffer; }
	// ��û���������
	unsigned int GetSize() const { return m_nSize; }
	// ��ø�ʽ
	int GetFormat() const { return m_nFormat; }

protected:
	fm_uint m_nBuffer;
#ifdef PROCESS_LOST_DEVICE
	void* m_pData;
#endif
	unsigned int m_nSize;
	int m_nFormat;
};

// ��̬��������

class IStaticIB: public IIndexBuffer
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_STATIC_IB; }
};

// ��̬��������

class IDynamicIB: public IIndexBuffer
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_DYNAMIC_IB; }
};


// ������ͼ������
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

	// TextureWrapMode �����װ��ʽ
	enum TEX_WRAP_MODE
	{
		TWM_REPEAT,
		TWM_CLAMP_TO_EDGE,
		TWM_MIRRORED_REPEAT,
	};

	// TextureType ��������
	enum TEX_TYPE
	{
		TEX_2D,
		TEX_3D,
		TEX_CUBE,
	};

	// ����������˷�ʽ
	virtual void SetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min) = 0;
	virtual void GetTextureSamplerFilter(ITextureSampler::TEX_SAMPLER_MAG_FILTER& tex_mag,ITextureSampler::TEX_SAMPLER_MIN_FILTER& tex_min) = 0;

	// ���������װ
	virtual void SetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE u_wrap_mode,ITextureSampler::TEX_WRAP_MODE v_wrap_mode) = 0;
	virtual void GetTextureUVWrapMode(ITextureSampler::TEX_WRAP_MODE& u_wrap_type,ITextureSampler::TEX_WRAP_MODE& v_wrap_mode) = 0;

	virtual ITextureSampler::TEX_TYPE GetTextureType() = 0;

	// �����Դ����
	virtual int GetResourceType() { return TYPE_TEXTURE_SAMPLER; }
};


// ���ɲ�����ͼ
class IRenderTex:public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_RENDER_TEX; }

	// ��ÿɲ������� 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;

	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
protected:
	fm_uint m_nRenderTex;
};

// ���ɲ��������
class IDepthTex:public IRenderTex
{
public:
	// �����Դ������
	virtual int GetResourceType() { return TYPE_DEPTH_TEX;};

	// ��ÿɲ������� 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
};

// �����Բ��������
class IColorTex:public IRenderTex
{
public:
	// �����Դ������
	virtual int GetResourceType() { return TYPE_COLOR_TEX; };

	// ��ÿɲ������� 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
};

// ���ɲ�����ģ��
class IStencilTex:public IRenderTex
{
public:
	// �����Դ������
	virtual int GetResourceType() { return TYPE_STENCIL_TEX ;};

	// ��ÿɲ������� 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
};

// ������Ⱥ�ģ���һ�����ⲻ�ɲ���
class IDepthStencilTex:public IRenderTex
{
public:
	// �����Դ������
	virtual int GetResourceType() { return TYPE_DEPTH_STENCIL_TEX ;};

	// ��ÿɲ������� 
	fm_uint GetRenderTex() const { return m_nRenderTex; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
};

// �ɲ�����ͼ

class IShaderTex: public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_SHADER_TEX; }

	// ��ÿɲ������� 
	fm_uint GetTexture() const { return m_nTexture; }

	// ��ͼ���
	virtual unsigned int GetWidth() const = 0;

	// ��ͼ�߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ͼ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;

	// ��ȡ��ͼ����������
	virtual ITextureSampler* GetTextureSampler() const = 0;
protected:
	fm_uint m_nTexture;
};

// ��̬��ͼ

class IStaticTex: public IShaderTex
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_STATIC_TEX; }
};

// ��̬��ͼ

class IDynamicTex: public IShaderTex
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_DYNAMIC_TEX; }
};

// ��̬��������ͼ

class IStaticCube: public IShaderTex
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_STATIC_CUBE; }
};

// ��ɫ��ȾĿ��

class IColorRT: public IShaderTex
{
public:
	// ���
	virtual unsigned int GetWidth() const = 0;
	// �߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
	// �Ƿ��Զ����ɲ㼶
	virtual bool GetAutoGenMipmap() const = 0;

	// �����Դ����
	virtual int GetResourceType() { return TYPE_COLOR_RT; }
};

// �����ȾĿ��

class IDepthRT: public IShaderTex
{
public:
	// ���
	virtual unsigned int GetWidth() const = 0;
	// �߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
	// �����Դ����
	virtual int GetResourceType() { return TYPE_DEPTH_RT; }
};

// �ɲ��������ģ���ʽ
class IDepthStencilRT: public IShaderTex
{
public:
	// ���
	virtual unsigned int GetWidth() const = 0;
	// �߶�
	virtual unsigned int GetHeight() const = 0;
	// ��ʽ
	virtual TEX_FORMAT GetFormat() const = 0;
	// �����Դ����
	virtual int GetResourceType() { return TYPE_DEPTH_STENCIL_RT; }
};

// ����һ��FrameBuffer
class IFrameRT : public IResource
{
public:
	//��״̬
	enum BIND_STATE
	{
		BIND_NONE,//δ��
		BIND_TEX,//�󶨲��ɶ���ͼ
		BIND_RT,//�󶨿ɶ�����
	};


	// �����Դ����
	virtual int GetResourceType() { return TYPE_FRAME_RT; }
	// Framebuffer����
	fm_uint GetFrame() const { return m_nFrame; }

	// ��ȡIColorRT
	virtual IColorRT* GetColorRT() = 0;

	// ��ȡIDepthRT
	virtual IDepthRT* GetDepthRT() = 0;

	// ��ȡ�󶨵�IColorTex
	virtual IColorTex* GetColorTex() = 0;

	// ��ȡ�󶨵�DepthTex
	virtual IDepthTex* GetDepthTex() = 0;

	// ��ȡ�󶨵�ģ��
	virtual IStencilTex* GetStencilTex() = 0;

	// ��ȡ�󶨵����ģ��
	virtual IDepthStencilTex* GetDepthStencilTex() = 0 ;

	// ��ȡ�󶨵����ģ��
	virtual IDepthStencilRT* GetDepthStencilRT() = 0;

	// ����IColorRT
	virtual bool SetColorRT(IColorRT* pColorRT) = 0;

	// ����IDepthRT
	virtual bool SetDepthRT(IDepthRT* pDepthRT) = 0;

	virtual bool SetColorTex(IColorTex* pColorTex) = 0;

	virtual bool SetDepthTex(IDepthTex* pDepthTex) = 0;

	virtual bool SetStencilTex(IStencilTex* pStencil) = 0;

	virtual bool SetDepthStencilTex(IDepthStencilTex* pDepthStencilTex) = 0;

	virtual bool SetDepthStencilRT(IDepthStencilRT* pDepthStencilRT) = 0;

	// ��ѯ��Frame��ǰ�İ󶨵�״̬
	virtual IFrameRT::BIND_STATE GetColorState() = 0;
	virtual IFrameRT::BIND_STATE GetDepthState() = 0;
	virtual IFrameRT::BIND_STATE GetStencilState() = 0;

	// �Ƿ����DepthStencil
	virtual bool IsBindDepthStencil() = 0;

	// ʹ�õ�ǰFBO��Ϊ��Ⱦ����
	virtual void UsedFrameRT() = 0;
protected:
	fm_uint m_nFrame;
};

// ������ɫ��

class IVertexShader: public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_VERTEX_SHADER; }

	// �����ɫ��
	fm_uint GetShader() const { return m_nShader; }

	virtual const char* GetFileName() const = 0;

	virtual const char* GetDefine() const = 0;

	virtual const char* GetFuncName() const = 0;
protected:
	fm_uint m_nShader;
};

// ������ɫ��

class IPixelShader: public IResource
{
public:
	// �����Դ����
	virtual int GetResourceType() { return TYPE_PIXEL_SHADER; }

	// �����ɫ��
	fm_uint GetShader() const { return m_nShader; }

	virtual const char* GetFileName() const = 0;

	virtual const char* GetDefine() const = 0;

	virtual const char* GetFuncName() const = 0;

protected:
	fm_uint m_nShader;
};


// shader��������
class IShaderParamOp
{
public:
	//���ݼĴ����������ֻ�ȡ����ֵ
	virtual fm_int FindParamIdByName(const char* pstrName) = 0;
	// bAuto  �Ƿ���Ҫ����ײ��Զ���������

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

	// ��չ�����ϵ���ѧ����а�װ��
	virtual bool SetParamValue(fm_int Id,FmVec2& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmVec3& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmVec4& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmMat3& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,FmMat4& value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec2* value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec3* value,bool bAuto = true) = 0;

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec4* value,bool bAuto = true) = 0;

	// ��������
	// ����ID         ����ID
	virtual bool SetTexture2D(fm_int Id,fm_int TexID) = 0;
	virtual bool SetTexture3D(fm_int Id,fm_int TexID) = 0;
	virtual bool SetTextureCube(fm_int Id,fm_int TexID) = 0;
};


// ��Ⱦ״̬������(Ŀǰ��Ҫ����һЩ���Ե���Ⱦ״̬���úͻ�ȡ����)
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
 		STENCIL_TEST_FRONT_AND_BACK,//��Ȼ�ײ�֧��˫ģ����ԣ����ǵײ����˫��ģ����Ե�״̬�����Ϊ�߼������ڷ���ֵ���ܻ�false.(��Ȼ֧�֣����ǲ��Ƽ�ʹ��)
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
	//��Ͽ���
	virtual bool IsEnableBlend() = 0;
	virtual void EnableBlend(bool bEnable) = 0;

	//���д��
	virtual bool IsEnableDepthWrite() = 0;
	virtual void EnableDepthWrite(bool bEnable) = 0;

	//�Ƿ������Ȳ���
	virtual bool IsEnableDepthTest() = 0;
	virtual void EnableDepthTest(bool bEnable) = 0;

	//�Ƿ���ɫд��
	virtual bool IsColorWrite() = 0;
	virtual void EnableColorWrite(bool bEnable) = 0;

	//�Ƿ����ģ�����
	virtual bool IsStencilTestEnable() = 0;
	virtual void EnableStencilTest(bool bEnable) = 0;

	//�Ƿ�����ѡ��
	virtual bool IsCullFaceEnable() = 0;
	virtual void EnableCullFace(bool bEnable) = 0;

	//���ò���
	virtual bool IsScissorTestEnable() = 0;
	virtual void EnableScissorTest(bool bEnable) = 0;

	//�Ƿ��������ƫ��(�������)
	virtual bool IsPolygonOffsetFillEnable() = 0;
	virtual void EnablePolygonOffsetFill(bool bEnable) = 0;

	//���ƫ�òο�ֵ
	virtual void SetPolygonOffset(float factor,float units) = 0;
	virtual void GetPolygonOffset(float& factor,float& units) = 0;

	//�����μ�ѡ�淽ʽ
	virtual void SetCullFaceModel(IRenderStateOp::CULL_FACE_MODE cull_face_model = CULL_BACK ) = 0;
	virtual IRenderStateOp::CULL_FACE_MODE GetCullFaceModel() = 0;

	//��ɫд�����
	virtual void SetColorWriteMask(bool bRed,bool bGreen,bool bBlue,bool bAlpha) = 0;
	virtual void GetColorWriteMask(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha) = 0;

	//������Ͽ���
	virtual void SetBlendColor(const FmVec4& blendcolor) = 0;
	virtual FmVec4 GetBlendColor( ) = 0;

	virtual	void SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha) = 0;
	virtual void GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha) = 0;
	virtual void SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha) = 0;
	virtual void GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC& srcRGB,IRenderStateOp::BLEND_COLOR_FUNC& srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC& dstRGB,IRenderStateOp::BLEND_COLOR_FUNC& dstAlpha) = 0;
	// ��ϲ������ʺ�������һ����ֻ��Ϊ���û�ϰ�߶���
	virtual void SetBlendEquation(IRenderStateOp::BLEND_FUNC_MODE mode) = 0;
	virtual void SetBlendFunc(IRenderStateOp::BLEND_COLOR_FUNC src,IRenderStateOp::BLEND_COLOR_FUNC dst) = 0;

	// ���д��Ĳ��Է���
	virtual void SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func) = 0;

	// ��ȡ��ǰ��Ȳ��Եķ���
	virtual IRenderStateOp::DEPTH_TEST_FUNC GetDepthTestFunc() = 0;

	// ģ��д��Ĳ��Է���
	virtual void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask) = 0;
	virtual bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask) = 0;

	// ģ��д��Ĳ�������
	virtual void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass) = 0;
	virtual bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass) = 0;

	// ģ��д������ֵ
	virtual void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask) = 0;
	virtual bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask) = 0;

	// ����ü�
	virtual void SetScissorTest(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) = 0;
	virtual void GetScissorTest(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) = 0;

	// �����ӿڴ�С
	virtual void SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) = 0;
	// ��ȡ��ǰ�ӿڴ�С
	virtual void GetViewPort(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) = 0;
};


// ��Ⱦ���ƽӿ�
// �����к�DrawCall ��صĺ���ֱ�ӹ�������ͳһ���� ���а����������ݺ�IB�����ݼ�غ͹���
// ��Ⱦ�������ṩ�˲�ѯ���ܣ����ǵײ� ���Զ�����һЩ��Ч�������ݻ����ظ������ݴ�š�
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
		VERTEX_DATA_HALF_FLOAT,//16λ���� ��Ҫ���Ӳ���Ƿ�֧��
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


// ��Ⱦ״̬��¼��
class IRenderStateRecordOp
{
public:
	//��ʼ��¼״̬
	virtual void Store() = 0;

	//��ʼ�ָ���Ⱦ״̬
	virtual void Restore() = 0;
};

// ��ɫ������

class IShaderProgram: public IResource
{
public:
	//��ȡshader�ĳ�������
	virtual IShaderParamOp* GetParamOp() const = 0;

	//��ȡ��Ⱦ��״̬����
	virtual IRenderStateOp* GetRenderStateOp() const = 0;

	//��Ⱦ�Ķ�����صĲ���
	virtual IRenderDrawOp*  GetRenderDrawOp() const =  0;

	virtual IVertexShader* GetVertexShader() const = 0;

    virtual IPixelShader*  GetPixelShader() const = 0;


	//��Ⱦ״̬��¼��
//	virtual IRenderStateRecordOp* GetRenderStateRecordOp() const = 0;

	//ʹ�ø�shader
	virtual void UsedShader() = 0;

	// �����Դ����
	virtual int GetResourceType() { return TYPE_SHADER_PROGRAM; }
};



#endif // _VISUAL_I_RESOURCE_H
