//--------------------------------------------------------------------
// 文件名:		shader_program.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月6日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _RENDER_STATE_OP_H_
#define _RENDER_STATE_OP_H_



#include "../visual/i_resource.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "render.h"


// 颜色写入状态控制
class CWriteColorMaskOp
{
public:
	CWriteColorMaskOp():m_bRed(true),m_bGreen(true),m_bBlue(true),m_bAlpha(true)
	{
	}

	void SetWriteColorMaskOp(bool bRed,bool bGreen,bool bBlue,bool bAlpha)
	{
        m_bRed = bRed;
		m_bGreen = bGreen;
		m_bBlue = bBlue;
		m_bAlpha = bAlpha;
	}

	bool Equal(bool bRed,bool bGreen,bool bBlue,bool bAlpha)
	{
		return (bRed == m_bRed) &&(bGreen == m_bGreen) && (bBlue == m_bBlue) && (bAlpha == m_bAlpha);
	}

	bool GetEnableWriteColorMask()
	{
		return (m_bRed == false) &&(m_bGreen == false) && (m_bBlue == false) && (m_bAlpha == false);
	}

	void GetWriteColorMaskOp(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha)
	{
        bRed = m_bRed;
		bGreen = m_bGreen;
		bBlue = m_bBlue;
		bAlpha = m_bAlpha;
	}
private:
	bool m_bRed;
	bool m_bGreen;
	bool m_bBlue;
	bool m_bAlpha;
};

// 阿法混合状态控制
class CBlendColorOp
{
public:
	//混合颜色
	void SetBlendColor(const FmVec4& color)
	{
		m_vec4BlendColor = color;
	}

	FmVec4 GetBlendColor()
	{
		return m_vec4BlendColor;
	}

	bool EqualBlendColor(const FmVec4& color)
	{
		return color == m_vec4BlendColor;
	}

    //混合方程
	void SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha)
	{
        m_modeRGB = modeRGB;
        m_modeAlpha = modeAlpha;
	}

	void GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha)
	{
        modeRGB = m_modeRGB;
		modeAlpha = m_modeAlpha;
	}

	bool EqualBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha)
	{
		return (modeRGB == m_modeRGB) && (modeAlpha == m_modeAlpha);
	}

	void SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha)
	{
		m_srcRGB = srcRGB;
		m_srcAlpha = srcAlpha;
		m_dstRGB = dstRGB;
		m_dstAlpha = dstAlpha;
	}

	void GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC &srcRGB,IRenderStateOp::BLEND_COLOR_FUNC &srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC &dstRGB,IRenderStateOp::BLEND_COLOR_FUNC &dstAlpha)
	{
		srcRGB = m_srcRGB;
		srcAlpha = m_srcAlpha;
		dstRGB = m_dstRGB;
		dstAlpha = m_dstAlpha;
	}

	bool EqualBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha)
	{
		return (srcRGB == m_srcRGB) && (srcAlpha == m_srcAlpha) && (dstRGB == m_dstRGB) && (dstAlpha == m_dstAlpha);
	}

    bool IsEnableBlend()
	{
		return m_bEnableBlend;
	}

    void EnableBlend(bool bEnable)
	{
		m_bEnableBlend = bEnable;
	}

private:
	FmVec4 m_vec4BlendColor;
    IRenderStateOp::BLEND_FUNC_MODE m_modeRGB;
    IRenderStateOp::BLEND_FUNC_MODE m_modeAlpha;
    IRenderStateOp::BLEND_COLOR_FUNC m_srcRGB;
	IRenderStateOp::BLEND_COLOR_FUNC m_srcAlpha;
	IRenderStateOp::BLEND_COLOR_FUNC m_dstRGB;
	IRenderStateOp::BLEND_COLOR_FUNC m_dstAlpha;
	bool m_bEnableBlend;
};

//模板测试控制
class CStencilTestOp
{
public:
	// 模板写入方法
	class CStencilTestFuncSeparate
	{
	public:
		// 模板写入方法
		void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask)
		{
			if( (IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK == face) || (IRenderStateOp::STENCIL_TEST_FRONT == face ) )
			{
				m_face[IRenderStateOp::STENCIL_TEST_FRONT] = IRenderStateOp::STENCIL_TEST_FRONT;
				m_func[IRenderStateOp::STENCIL_TEST_FRONT] = func;
                m_ref[IRenderStateOp::STENCIL_TEST_FRONT] = ref;
				m_mask[IRenderStateOp::STENCIL_TEST_FRONT] = mask;
			}

			if( (IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK == face) || (IRenderStateOp::STENCIL_TEST_BACK == face ) )
			{
				m_face[IRenderStateOp::STENCIL_TEST_BACK] = IRenderStateOp::STENCIL_TEST_BACK;
				m_func[IRenderStateOp::STENCIL_TEST_BACK] = func;
                m_ref[IRenderStateOp::STENCIL_TEST_BACK] = ref;
				m_mask[IRenderStateOp::STENCIL_TEST_BACK] = mask;
			}
		}

		// 模板写入方法
		bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask)
		{
			if(( IRenderStateOp::STENCIL_TEST_FRONT == face ) || ( IRenderStateOp::STENCIL_TEST_BACK == face  ) )
			{
				func = m_func[face];
				ref = m_ref[face];
                mask = m_mask[face];
				return true;
			}

			//如果双模板测试属于状态不一致则直接返回
			if(  (m_func[IRenderStateOp::STENCIL_TEST_BACK] == m_func[IRenderStateOp::STENCIL_TEST_FRONT] ) && 
				(m_ref[IRenderStateOp::STENCIL_TEST_BACK] == m_ref[IRenderStateOp::STENCIL_TEST_FRONT] ) &&
				(m_mask[IRenderStateOp::STENCIL_TEST_BACK] == m_mask[IRenderStateOp::STENCIL_TEST_FRONT]) )
			{
				return false;
			}
			func = m_func[IRenderStateOp::STENCIL_TEST_FRONT];
			ref = m_ref[IRenderStateOp::STENCIL_TEST_FRONT];
			mask = m_mask[IRenderStateOp::STENCIL_TEST_FRONT];
			return true;
		}

		bool EqualStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask)
		{
			if(( IRenderStateOp::STENCIL_TEST_FRONT == face  ) || ( IRenderStateOp::STENCIL_TEST_BACK == face  ) )
			{
				return (m_func[face] == func) && (m_ref[face] == ref) && (m_mask[face] == mask);
			}

			//如果双模板测试属于状态不一致则直接返回
			if(  (m_func[IRenderStateOp::STENCIL_TEST_BACK] == m_func[IRenderStateOp::STENCIL_TEST_FRONT] ) && 
				(m_ref[IRenderStateOp::STENCIL_TEST_BACK] == m_ref[IRenderStateOp::STENCIL_TEST_FRONT] ) &&
				(m_mask[IRenderStateOp::STENCIL_TEST_BACK] == m_mask[IRenderStateOp::STENCIL_TEST_FRONT]) )
			{
				return false;
			}
			
			return (m_func[IRenderStateOp::STENCIL_TEST_FRONT] == func) && (m_ref[IRenderStateOp::STENCIL_TEST_FRONT] == ref) && (m_mask[IRenderStateOp::STENCIL_TEST_FRONT] == mask);
		}

	private:
		IRenderStateOp::STENCIL_TEST_FACE m_face[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
        IRenderStateOp::STENCIL_TEST_FUNC m_func[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
        int               m_ref[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
		unsigned int      m_mask[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
	};

	// 模板测试操作
	class CStencilTestOpSeparate
	{
	public:
		// 模板写入的操作方法
		void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK) || ( face == IRenderStateOp::STENCIL_TEST_FRONT) )
			{
				m_face[IRenderStateOp::STENCIL_TEST_FRONT] = IRenderStateOp::STENCIL_TEST_FRONT;
				m_sfail[IRenderStateOp::STENCIL_TEST_FRONT] = sfail;
				m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT] = dpfail;
				m_dppass[IRenderStateOp::STENCIL_TEST_FRONT] = dppass;
			}

			if( (face == IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK) || ( face == IRenderStateOp::STENCIL_TEST_BACK) )
			{
				m_face[IRenderStateOp::STENCIL_TEST_BACK] = IRenderStateOp::STENCIL_TEST_BACK;
				m_sfail[IRenderStateOp::STENCIL_TEST_BACK] = sfail;
				m_dpfail[IRenderStateOp::STENCIL_TEST_BACK] = dpfail;
				m_dppass[IRenderStateOp::STENCIL_TEST_BACK] = dppass;
			}
		}

		// 模板写入值获取
		bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_BACK) || ( face == IRenderStateOp::STENCIL_TEST_FRONT) )
			{
                sfail = m_sfail[face];
                dpfail = m_dpfail[face];
                dppass = m_dppass[face];
				return true;
			}

			// 如果双面模板测试参数一样的
			if( ( m_sfail[IRenderStateOp::STENCIL_TEST_FRONT] == m_sfail[IRenderStateOp::STENCIL_TEST_BACK]) &&
				(m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT] == m_dpfail[IRenderStateOp::STENCIL_TEST_BACK]) &&
				(m_dppass[IRenderStateOp::STENCIL_TEST_FRONT] == m_dppass[IRenderStateOp::STENCIL_TEST_BACK]) )
			{
				sfail = m_sfail[IRenderStateOp::STENCIL_TEST_FRONT];
				dpfail = m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT];
				dppass = m_dppass[IRenderStateOp::STENCIL_TEST_FRONT];
				return true;
			}

            return false;
		}

		bool EqualStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_BACK) || ( face == IRenderStateOp::STENCIL_TEST_FRONT) )
			{
				return ( m_sfail[face] == sfail) &&(m_dpfail[face] == dpfail) && (m_dppass[face] == dppass);
			}

			if( ( m_sfail[IRenderStateOp::STENCIL_TEST_FRONT] == m_sfail[IRenderStateOp::STENCIL_TEST_BACK]) &&
				(m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT] == m_dpfail[IRenderStateOp::STENCIL_TEST_BACK]) &&
				(m_dppass[IRenderStateOp::STENCIL_TEST_FRONT] == m_dppass[IRenderStateOp::STENCIL_TEST_BACK]) )
			{
				return ( m_sfail[IRenderStateOp::STENCIL_TEST_FRONT] == sfail) &&
					(m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT] == dpfail) &&
					(m_dppass[IRenderStateOp::STENCIL_TEST_FRONT] == dppass);
			}

			return false;
		}
	private:
		IRenderStateOp::STENCIL_TEST_FACE m_face[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
		IRenderStateOp::STENCIL_FUNC_MODE m_sfail[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
		IRenderStateOp::STENCIL_FUNC_MODE m_dpfail[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
		IRenderStateOp::STENCIL_FUNC_MODE m_dppass[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
	};

	// 模板这写入遮罩值控制
	class CStencilTestMask
	{
	public:
		// 模板写入遮罩值
		void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK) || ( face == IRenderStateOp::STENCIL_TEST_FRONT) )
			{
                m_face[IRenderStateOp::STENCIL_TEST_FRONT] = IRenderStateOp::STENCIL_TEST_FRONT;
				m_mask[IRenderStateOp::STENCIL_TEST_FRONT] = mask;
			}

			if( (face == IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK) || ( face == IRenderStateOp::STENCIL_TEST_BACK) )
			{
                m_face[IRenderStateOp::STENCIL_TEST_BACK] = IRenderStateOp::STENCIL_TEST_BACK;
				m_mask[IRenderStateOp::STENCIL_TEST_BACK] = mask;
			}
		}

		// 相等判断
		bool EqualStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT ) || ( face == IRenderStateOp::STENCIL_TEST_BACK ) )
			{
				return m_mask[face] == mask;
			}

			// STENCIL_TEST_FRONT_AND_BACK
			return (m_mask[IRenderStateOp::STENCIL_TEST_FRONT] == mask) && (m_mask[IRenderStateOp::STENCIL_TEST_BACK] == mask);
		}

		// 这个函数有一处陷阱地方需要注意STENCIL_TEST_FRONT_AND_BACK 可能出现双面剔除不一致性
		bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT) || (face == IRenderStateOp::STENCIL_TEST_BACK ) )
			{
                mask = m_mask[face];
				return true;
			}

			//如果当存取的不一致的时候返回错误
			if( m_mask[IRenderStateOp::STENCIL_TEST_BACK] != m_mask[IRenderStateOp::STENCIL_TEST_FRONT])
			{
				return false;
			}

			mask = m_mask[IRenderStateOp::STENCIL_TEST_BACK];
			return true;
		}
	private:
        IRenderStateOp::STENCIL_TEST_FACE m_face[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
		unsigned int m_mask[IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK];
	};
public:
	// 模板写入的测试方法
	void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask)
	{
		m_stenciltestfunc.SetStencilTestFuncSeparate(face,func,ref,mask);
	}

	bool EqualStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask)
	{
		return m_stenciltestfunc.EqualStencilTestFuncSeparate(face,func,ref,mask);
	}

	bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask)
	{
		return m_stenciltestfunc.GetStencilTestFuncSeparate(face,func,ref,mask);
	}

	// 模板写入的操作方法
	void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass)
	{
		m_stenciltestop.SetStencilTestOpSeparate(face,sfail,dpfail,dppass);
	}

	bool EqualStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass)
	{
		return m_stenciltestop.EqualStencilTestOpSeparate(face,sfail,dpfail,dppass);
	}

	bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass)
	{
		return m_stenciltestop.GetStencilTestOpSeparate(face,sfail,dpfail,dppass);
	}

	// 模板写入遮罩值控制
	void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
	{
       m_stenciltestmask.SetStencilTestMaskSeparate(face,mask);
	}

	bool EqualStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
	{
		return m_stenciltestmask.EqualStencilTestMaskSeparate(face,mask);
	}

	bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask)
	{
		return m_stenciltestmask.GetStencilTestMaskSeparate(face,mask);
	}

	//是否进行模板测试
	bool IsStencilTestEnable()
	{
		return m_bStencilTest;
	}

	void EnableStencilTest(bool bEnable)
	{
		m_bStencilTest = bEnable;
	}
private:
    CStencilTestFuncSeparate m_stenciltestfunc;
    CStencilTestOpSeparate   m_stenciltestop;
	CStencilTestMask         m_stenciltestmask;
	bool m_bStencilTest;
};

//深度比较
class CDepthTestOp
{
public:
	// 设置深度测试的方法
	void SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC depthtestfunc)
	{
		m_depthtestfunc = depthtestfunc;
	}
	
	bool EqualDepthTest(IRenderStateOp::DEPTH_TEST_FUNC depthtestfunc)
	{
		return m_depthtestfunc == depthtestfunc;
	}

    IRenderStateOp::DEPTH_TEST_FUNC GetDepthTestFunc() const
	{
		return m_depthtestfunc;
	}

	bool IsEnableDepthWrite()
	{
		return m_bDepthWrite;
	}

	void EnableDepthWrite(bool bEnable)
	{
        m_bDepthWrite = bEnable;
	}

	bool IsEnableDepthTest() const
	{
		return m_bDepthTest;
	}

	void EnableDepthTest(bool bEnable)
	{
		m_bDepthTest = bEnable;
	}
private:
    IRenderStateOp::DEPTH_TEST_FUNC m_depthtestfunc;
	bool m_bDepthWrite;
	bool m_bDepthTest;
};

// 视口操作
class CViewPortOp
{
public:
	void SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height)
	{
        m_x = x;
		m_y = y;
        m_width = width;
        m_height = height;
	}

	bool EqualViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height)
	{
		return (m_x == x) && (m_y == y) && (m_width == width) && (m_height == height);
	}

	void GetViewPort(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height)
	{
		x = m_x;
		y = m_y;
		width = m_width;
		height = m_height;
	}
private:
	unsigned int m_x;
	unsigned int m_y;
	fm_sizei m_width;
	fm_sizei m_height;
};

// 区域裁剪
class CScissorTestOp
{
public:
	// 设置裁剪区域
	void SetScissorTest(unsigned int left,unsigned int bottom,fm_sizei width,fm_sizei height)
	{
		m_left = left;
        m_bottom = bottom;
        m_width = width;
        m_height = height;
	}

	bool EqualScissorTest(unsigned int left,unsigned int bottom,fm_sizei width,fm_sizei height)
	{
		return (m_left == left) && (m_bottom == bottom) && (m_width == width) && (m_height == height);
	}

	void GetScissorTest(unsigned int& left,unsigned int& bottom,fm_sizei& width,fm_sizei& height)
	{
        left = m_left;
		bottom = m_bottom;
		width = m_width;
		height = m_height;
	}

	//剪裁测试
	bool IsScissorTestEnable()
	{
		return m_bStencilTest;
	}

	void EnableScissorTest(bool bEnable)
	{
		m_bStencilTest = bEnable;
	}
	
private:
	unsigned m_left;
	unsigned m_bottom;
    fm_sizei m_width;
	fm_sizei m_height;
	bool m_bStencilTest;
};


class CRenderStateRecordOp;

// 渲染状态管理
class CRenderStateOp:public IRenderStateOp
{
public:
	static IRenderStateOp* NewInstance(Render* pRender);  
	// 释放
	void Release();

	CRenderStateOp(){};

	CRenderStateOp(Render* pRender);

	//混合开关
	virtual bool IsEnableBlend();
    virtual void EnableBlend(bool bEnable);
    
	//深度写入管理
	virtual bool IsEnableDepthWrite();
	virtual void EnableDepthWrite(bool bEnable);

	//是否进行深度测试
	virtual bool IsEnableDepthTest();
	virtual void EnableDepthTest(bool bEnable);

	//是否颜色写入
	virtual bool IsColorWrite();
	virtual void EnableColorWrite(bool bEnable);

	//是否进行模板测试
	virtual bool IsStencilTestEnable();
	virtual void EnableStencilTest(bool bEnable);

	//是否开启拣选面
	virtual bool IsCullFaceEnable();
	virtual void EnableCullFace(bool bEnable);

	//剪裁测试
	virtual bool IsScissorTestEnable();
	virtual void EnableScissorTest(bool bEnable);

	//是否启动深度偏置(深度修正)
	virtual bool IsPolygonOffsetFillEnable();
	virtual void EnablePolygonOffsetFill(bool bEnable);

	//深度偏置参考值
	virtual void SetPolygonOffset(float factor,float units);
	virtual void GetPolygonOffset(float& factor,float& units);
    
	//三角形拣选面方式
	virtual void SetCullFaceModel(IRenderStateOp::CULL_FACE_MODE cull_face_model = CULL_BACK );
	virtual IRenderStateOp::CULL_FACE_MODE GetCullFaceModel() ;

	//颜色写入控制
	virtual void SetColorWriteMask(bool bRed,bool bGreen,bool bBlue,bool bAlpha) ;
	virtual void GetColorWriteMask(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha) ;

	//阿法混合控制 
	virtual void SetBlendColor(const FmVec4& blendcolor) ;
	virtual FmVec4 GetBlendColor( ) ;

	virtual	void SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha) ;
	virtual void GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha);
	virtual void SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha);
	virtual void GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC& srcRGB,IRenderStateOp::BLEND_COLOR_FUNC& srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC& dstRGB,IRenderStateOp::BLEND_COLOR_FUNC& dstAlpha);

	virtual void SetBlendEquation(IRenderStateOp::BLEND_FUNC_MODE mode);
	virtual void SetBlendFunc(IRenderStateOp::BLEND_COLOR_FUNC src,IRenderStateOp::BLEND_COLOR_FUNC dst);

	// 深度写入的测试方法
	virtual void SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func);

	virtual IRenderStateOp::DEPTH_TEST_FUNC GetDepthTestFunc();

	// 模板写入的测试方法
	virtual void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask) ;
	virtual bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask);

	// 模板写入的操作方法
	virtual void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass);
	virtual bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass);

	// 模板写入遮罩值
	virtual void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask);
	// 获取模板正反面处理
	virtual bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask);

    // 区域裁剪
	virtual void SetScissorTest(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) ;
	virtual void GetScissorTest(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) ;

	// 设置视口大小
	virtual void SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) ;
	// 获取当前视口大小
	virtual void GetViewPort(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) ;
private:
	GLenum  BlendFuncModeToGLBlendFuncMode(IRenderStateOp::BLEND_FUNC_MODE mode);
	GLenum  BlendColorFuncToGLBlendColorFunc(IRenderStateOp::BLEND_COLOR_FUNC func);
	GLenum  DepthTestFuncToGLDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func);
	GLenum  StencilFaceToGLStencilFace(IRenderStateOp::STENCIL_TEST_FACE face);
	GLenum  StencilFuncToGLStencilFunc(IRenderStateOp::STENCIL_TEST_FUNC func);
	GLenum  StencilModeToGLStencilMode(IRenderStateOp::STENCIL_FUNC_MODE mode);
private:
	bool m_bCullFace;
	bool m_bPolygonOffsetFill;
	float m_PolygonOffset_factor;
	float m_PolygonOffset_units;

	IRenderStateOp::CULL_FACE_MODE m_cull_face_model;
	CWriteColorMaskOp m_cWriteColorOp;
	CBlendColorOp     m_cBlendColorOp;
	CDepthTestOp      m_cDepthTestOp;
	CStencilTestOp    m_cStencilTestOp;
	CScissorTestOp    m_cScissorTestOp;
	CViewPortOp       m_cViewPortOp;

	Render* m_pRender;
	CRenderStateRecordOp* m_pRenderStateRecordOp;
};


// 状态管理记录器
class CRenderStateRecordOp:public IRenderStateRecordOp
{
public:
	// 记录状态的接口操作标记位
	enum FLAG_RENDER_STATE_RECORD
	{
		FLAG_RENDER_STATE_NONE,//无任何状态标记
		// 渲染开关状态
		FLAG_COLOR_BLEND_B = 0x1, //深度混合开关标记
		FLAG_COLOR_WRITE_B = (0x1<<1), //颜色写入标记
		FLAG_DEPTH_WRITE_B = (0x1<<2), //深度写入标记
        FLAG_DEPTH_TEST_B = (0x1<<3),//深度测试标记
        FLAG_STENCIL_TEST_B = (0x1<<4),//模版测试标记
		FLAG_CULL_FACE_B = (0x1<<5),//裁剪开关标记
		FLAG_SCISSOR_TEST_B = (0x1<<6),//遮罩测试标记
		FLAG_POLYGON_OFFSET_FILL_B = (0x1<<7),//深度偏移标记
		FLAG_CULL_FACE_MODEL_B = (0x1<<8),//裁剪顺序标记
		FLAG_COLOR_WRITE_MASK_B = (0x1<<9),//颜色遮罩写入标记

		// 颜色混合操作位
		FLAG_BLEND_COLOR_C = (0x1<<10),
        FLAG_BLEND_EQUATION_SEPARATE_C =  (0x1<<11),
        FLAG_BLEND_FUNC_SEPARATE_C = (0x1<<12),

		// 深度测试方式标记
        FLAG_DEPTH_TEST_FUNC_C = (0x1<<13),

		// 模版测试标记
        FLAG_STENCIL_TEST_FUNC_SEPARATE_C = (0x1<<14),
        FLAG_STENCIL_TEST_OP_SEPARATE_C = (0x1<<15),
        FLAG_STENCIL_TEST_MASK_SEPARATE_C = (0x1<<16),

		// 遮罩区域设置标记
		FLAG_SCISSOR_TEST_C = (0x1<<17),
		// 视口调整标记
		FLAG_VIEWPORT_C = (0x1<<18),

		// 偏移函数设置
		FLAG_POLYGON_OFFSET_C = (0x1<<19),
	};

	// 根据操作种类进行分类
	enum FLAG_RENDER_STATE_GROUPS
	{
		//如果无调整则表示没有状态变化
		FLAG_RENDER_STATE_GROUP_NO_CHANGE = 0,
		//深度操作
		FLAG_RENDER_STATE_GROUP_DEPTH_OP = FLAG_DEPTH_WRITE_B | FLAG_DEPTH_TEST_B | FLAG_DEPTH_TEST_FUNC_C,
		//模板操作
		FLAG_RENDER_STATE_GROUP_STENCIL_OP = FLAG_STENCIL_TEST_B | FLAG_STENCIL_TEST_FUNC_SEPARATE_C | FLAG_STENCIL_TEST_OP_SEPARATE_C | FLAG_STENCIL_TEST_MASK_SEPARATE_C,
		//颜色写入操作
		FLAG_RENDER_STATE_GROUP_COLOR_OP = FLAG_COLOR_WRITE_B | FLAG_COLOR_WRITE_MASK_B,
		//颜色混合操作
		FLAG_RENDER_STATE_GROUP_COLOR_BLEND_OP = FLAG_COLOR_BLEND_B | FLAG_BLEND_COLOR_C | FLAG_BLEND_EQUATION_SEPARATE_C | FLAG_BLEND_FUNC_SEPARATE_C,
		//裁剪操作
		FLAG_RENDER_STATE_GROUP_CULL_OP = FLAG_CULL_FACE_B |  FLAG_CULL_FACE_MODEL_B,
		//深度偏移操作
		FLAG_RENDER_STATE_GROUP_POLYGON_OFFSET_OP = FLAG_POLYGON_OFFSET_FILL_B | FLAG_POLYGON_OFFSET_C,
		//遮罩操作
		FLAG_RENDER_STATE_GROUP_SCISSOR_OP = FLAG_SCISSOR_TEST_B | FLAG_SCISSOR_TEST_C,
		//视口调整
		FLAG_RENDER_STATE_VIEWPORT_OP = FLAG_VIEWPORT_C,
	};

public:
	static IRenderStateRecordOp* NewInstance(Render* pRender);  

	~CRenderStateRecordOp();
	// 释放
	void Release();

	CRenderStateRecordOp(Render* pRender);

	//开始记录状态
	virtual void Store();

	//开始恢复渲染状态
	virtual void Restore();

	unsigned int& GetRenderStateRecordFlag();
private:
	void RestoreDepthOp(IRenderStateOp* pRenderStateOp);
	void RestoreStencilOp(IRenderStateOp* pRenderStateOp);
	void RestoreColorOp(IRenderStateOp* pRenderStateOp);
	void RestoreColorBlendOp(IRenderStateOp* pRenderStateOp);
	void RestoreCullOp(IRenderStateOp* pRenderStateOp);
	void RestorePolyGonOffsetOp(IRenderStateOp* pRenderStateOp);
	void RestoreScissor(IRenderStateOp* pRenderStateOp);
	void RestoreViewPortOp(IRenderStateOp* pRenderStateOp);
private:

	Render* m_pRender;
    CRenderStateOp* m_pRenderStateOp;
	unsigned int m_RenderStateFlag;
};




#endif