//--------------------------------------------------------------------
// �ļ���:		shader_program.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��6��6��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef _RENDER_STATE_OP_H_
#define _RENDER_STATE_OP_H_



#include "../visual/i_resource.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "render.h"


// ��ɫд��״̬����
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

// �������״̬����
class CBlendColorOp
{
public:
	//�����ɫ
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

    //��Ϸ���
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

//ģ����Կ���
class CStencilTestOp
{
public:
	// ģ��д�뷽��
	class CStencilTestFuncSeparate
	{
	public:
		// ģ��д�뷽��
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

		// ģ��д�뷽��
		bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask)
		{
			if(( IRenderStateOp::STENCIL_TEST_FRONT == face ) || ( IRenderStateOp::STENCIL_TEST_BACK == face  ) )
			{
				func = m_func[face];
				ref = m_ref[face];
                mask = m_mask[face];
				return true;
			}

			//���˫ģ���������״̬��һ����ֱ�ӷ���
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

			//���˫ģ���������״̬��һ����ֱ�ӷ���
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

	// ģ����Բ���
	class CStencilTestOpSeparate
	{
	public:
		// ģ��д��Ĳ�������
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

		// ģ��д��ֵ��ȡ
		bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_BACK) || ( face == IRenderStateOp::STENCIL_TEST_FRONT) )
			{
                sfail = m_sfail[face];
                dpfail = m_dpfail[face];
                dppass = m_dppass[face];
				return true;
			}

			// ���˫��ģ����Բ���һ����
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

	// ģ����д������ֵ����
	class CStencilTestMask
	{
	public:
		// ģ��д������ֵ
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

		// ����ж�
		bool EqualStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT ) || ( face == IRenderStateOp::STENCIL_TEST_BACK ) )
			{
				return m_mask[face] == mask;
			}

			// STENCIL_TEST_FRONT_AND_BACK
			return (m_mask[IRenderStateOp::STENCIL_TEST_FRONT] == mask) && (m_mask[IRenderStateOp::STENCIL_TEST_BACK] == mask);
		}

		// ���������һ������ط���Ҫע��STENCIL_TEST_FRONT_AND_BACK ���ܳ���˫���޳���һ����
		bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask)
		{
			if( (face == IRenderStateOp::STENCIL_TEST_FRONT) || (face == IRenderStateOp::STENCIL_TEST_BACK ) )
			{
                mask = m_mask[face];
				return true;
			}

			//�������ȡ�Ĳ�һ�µ�ʱ�򷵻ش���
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
	// ģ��д��Ĳ��Է���
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

	// ģ��д��Ĳ�������
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

	// ģ��д������ֵ����
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

	//�Ƿ����ģ�����
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

//��ȱȽ�
class CDepthTestOp
{
public:
	// ������Ȳ��Եķ���
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

// �ӿڲ���
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

// ����ü�
class CScissorTestOp
{
public:
	// ���òü�����
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

	//���ò���
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

// ��Ⱦ״̬����
class CRenderStateOp:public IRenderStateOp
{
public:
	static IRenderStateOp* NewInstance(Render* pRender);  
	// �ͷ�
	void Release();

	CRenderStateOp(){};

	CRenderStateOp(Render* pRender);

	//��Ͽ���
	virtual bool IsEnableBlend();
    virtual void EnableBlend(bool bEnable);
    
	//���д�����
	virtual bool IsEnableDepthWrite();
	virtual void EnableDepthWrite(bool bEnable);

	//�Ƿ������Ȳ���
	virtual bool IsEnableDepthTest();
	virtual void EnableDepthTest(bool bEnable);

	//�Ƿ���ɫд��
	virtual bool IsColorWrite();
	virtual void EnableColorWrite(bool bEnable);

	//�Ƿ����ģ�����
	virtual bool IsStencilTestEnable();
	virtual void EnableStencilTest(bool bEnable);

	//�Ƿ�����ѡ��
	virtual bool IsCullFaceEnable();
	virtual void EnableCullFace(bool bEnable);

	//���ò���
	virtual bool IsScissorTestEnable();
	virtual void EnableScissorTest(bool bEnable);

	//�Ƿ��������ƫ��(�������)
	virtual bool IsPolygonOffsetFillEnable();
	virtual void EnablePolygonOffsetFill(bool bEnable);

	//���ƫ�òο�ֵ
	virtual void SetPolygonOffset(float factor,float units);
	virtual void GetPolygonOffset(float& factor,float& units);
    
	//�����μ�ѡ�淽ʽ
	virtual void SetCullFaceModel(IRenderStateOp::CULL_FACE_MODE cull_face_model = CULL_BACK );
	virtual IRenderStateOp::CULL_FACE_MODE GetCullFaceModel() ;

	//��ɫд�����
	virtual void SetColorWriteMask(bool bRed,bool bGreen,bool bBlue,bool bAlpha) ;
	virtual void GetColorWriteMask(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha) ;

	//������Ͽ��� 
	virtual void SetBlendColor(const FmVec4& blendcolor) ;
	virtual FmVec4 GetBlendColor( ) ;

	virtual	void SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha) ;
	virtual void GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha);
	virtual void SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha);
	virtual void GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC& srcRGB,IRenderStateOp::BLEND_COLOR_FUNC& srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC& dstRGB,IRenderStateOp::BLEND_COLOR_FUNC& dstAlpha);

	virtual void SetBlendEquation(IRenderStateOp::BLEND_FUNC_MODE mode);
	virtual void SetBlendFunc(IRenderStateOp::BLEND_COLOR_FUNC src,IRenderStateOp::BLEND_COLOR_FUNC dst);

	// ���д��Ĳ��Է���
	virtual void SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func);

	virtual IRenderStateOp::DEPTH_TEST_FUNC GetDepthTestFunc();

	// ģ��д��Ĳ��Է���
	virtual void SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask) ;
	virtual bool GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask);

	// ģ��д��Ĳ�������
	virtual void SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass);
	virtual bool GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass);

	// ģ��д������ֵ
	virtual void SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask);
	// ��ȡģ�������洦��
	virtual bool GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask);

    // ����ü�
	virtual void SetScissorTest(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) ;
	virtual void GetScissorTest(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height) ;

	// �����ӿڴ�С
	virtual void SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height) ;
	// ��ȡ��ǰ�ӿڴ�С
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


// ״̬�����¼��
class CRenderStateRecordOp:public IRenderStateRecordOp
{
public:
	// ��¼״̬�Ľӿڲ������λ
	enum FLAG_RENDER_STATE_RECORD
	{
		FLAG_RENDER_STATE_NONE,//���κ�״̬���
		// ��Ⱦ����״̬
		FLAG_COLOR_BLEND_B = 0x1, //��Ȼ�Ͽ��ر��
		FLAG_COLOR_WRITE_B = (0x1<<1), //��ɫд����
		FLAG_DEPTH_WRITE_B = (0x1<<2), //���д����
        FLAG_DEPTH_TEST_B = (0x1<<3),//��Ȳ��Ա��
        FLAG_STENCIL_TEST_B = (0x1<<4),//ģ����Ա��
		FLAG_CULL_FACE_B = (0x1<<5),//�ü����ر��
		FLAG_SCISSOR_TEST_B = (0x1<<6),//���ֲ��Ա��
		FLAG_POLYGON_OFFSET_FILL_B = (0x1<<7),//���ƫ�Ʊ��
		FLAG_CULL_FACE_MODEL_B = (0x1<<8),//�ü�˳����
		FLAG_COLOR_WRITE_MASK_B = (0x1<<9),//��ɫ����д����

		// ��ɫ��ϲ���λ
		FLAG_BLEND_COLOR_C = (0x1<<10),
        FLAG_BLEND_EQUATION_SEPARATE_C =  (0x1<<11),
        FLAG_BLEND_FUNC_SEPARATE_C = (0x1<<12),

		// ��Ȳ��Է�ʽ���
        FLAG_DEPTH_TEST_FUNC_C = (0x1<<13),

		// ģ����Ա��
        FLAG_STENCIL_TEST_FUNC_SEPARATE_C = (0x1<<14),
        FLAG_STENCIL_TEST_OP_SEPARATE_C = (0x1<<15),
        FLAG_STENCIL_TEST_MASK_SEPARATE_C = (0x1<<16),

		// �����������ñ��
		FLAG_SCISSOR_TEST_C = (0x1<<17),
		// �ӿڵ������
		FLAG_VIEWPORT_C = (0x1<<18),

		// ƫ�ƺ�������
		FLAG_POLYGON_OFFSET_C = (0x1<<19),
	};

	// ���ݲ���������з���
	enum FLAG_RENDER_STATE_GROUPS
	{
		//����޵������ʾû��״̬�仯
		FLAG_RENDER_STATE_GROUP_NO_CHANGE = 0,
		//��Ȳ���
		FLAG_RENDER_STATE_GROUP_DEPTH_OP = FLAG_DEPTH_WRITE_B | FLAG_DEPTH_TEST_B | FLAG_DEPTH_TEST_FUNC_C,
		//ģ�����
		FLAG_RENDER_STATE_GROUP_STENCIL_OP = FLAG_STENCIL_TEST_B | FLAG_STENCIL_TEST_FUNC_SEPARATE_C | FLAG_STENCIL_TEST_OP_SEPARATE_C | FLAG_STENCIL_TEST_MASK_SEPARATE_C,
		//��ɫд�����
		FLAG_RENDER_STATE_GROUP_COLOR_OP = FLAG_COLOR_WRITE_B | FLAG_COLOR_WRITE_MASK_B,
		//��ɫ��ϲ���
		FLAG_RENDER_STATE_GROUP_COLOR_BLEND_OP = FLAG_COLOR_BLEND_B | FLAG_BLEND_COLOR_C | FLAG_BLEND_EQUATION_SEPARATE_C | FLAG_BLEND_FUNC_SEPARATE_C,
		//�ü�����
		FLAG_RENDER_STATE_GROUP_CULL_OP = FLAG_CULL_FACE_B |  FLAG_CULL_FACE_MODEL_B,
		//���ƫ�Ʋ���
		FLAG_RENDER_STATE_GROUP_POLYGON_OFFSET_OP = FLAG_POLYGON_OFFSET_FILL_B | FLAG_POLYGON_OFFSET_C,
		//���ֲ���
		FLAG_RENDER_STATE_GROUP_SCISSOR_OP = FLAG_SCISSOR_TEST_B | FLAG_SCISSOR_TEST_C,
		//�ӿڵ���
		FLAG_RENDER_STATE_VIEWPORT_OP = FLAG_VIEWPORT_C,
	};

public:
	static IRenderStateRecordOp* NewInstance(Render* pRender);  

	~CRenderStateRecordOp();
	// �ͷ�
	void Release();

	CRenderStateRecordOp(Render* pRender);

	//��ʼ��¼״̬
	virtual void Store();

	//��ʼ�ָ���Ⱦ״̬
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