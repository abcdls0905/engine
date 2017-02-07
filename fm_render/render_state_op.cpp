#include "render_state_op.h"
#include "../public/core_log.h"
#include "../visual/vis_utils.h"


IRenderStateOp* CRenderStateOp::NewInstance(Render* pRender)
{
	IRenderStateOp* p = (IRenderStateOp*)CORE_ALLOC(sizeof(CRenderStateOp));

	new (p) CRenderStateOp(pRender);

	return p;
}

// 释放
void CRenderStateOp::Release()
{
	CORE_DELETE(this);
}

//这里是初始化设备的渲染状态
CRenderStateOp::CRenderStateOp(Render* pRender):m_pRender(pRender),m_bCullFace(true)
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// 开启深度写入
	glDepthMask(GL_TRUE);
	m_cDepthTestOp.EnableDepthWrite(true);
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	m_cDepthTestOp.EnableDepthTest(true);

	// 初始化Alpha混合方式

	m_cBlendColorOp.SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_ADD,IRenderStateOp::BLEND_FUNC_ADD);
	m_cBlendColorOp.SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE,IRenderStateOp::BLEND_COLOR_ONE);
	FmVec4 color(0.5,0.5,0.5,0.5);
	m_cBlendColorOp.SetBlendColor(color);
	glBlendColor(0.5,0.5,0.5,0.5);
	glBlendFunc(GL_ONE,GL_ONE);
	glBlendEquation(GL_FUNC_ADD);


	// 关闭ALPHA混合
	glDisable(GL_BLEND);
	m_cBlendColorOp.EnableBlend(false);
	// 默认混合方式
	// 关闭模板测试
	glDisable(GL_STENCIL_TEST);
	m_cStencilTestOp.EnableStencilTest(false);

	glScissor(0,0,0,0);
	m_cScissorTestOp.SetScissorTest(0,0,0,0);
	glDisable(GL_SCISSOR_TEST);
	m_cScissorTestOp.EnableScissorTest(false);

	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xFFFFFFFF);
	m_cStencilTestOp.SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK,IRenderStateOp::STENCIL_TEST_ALWAYS,0,0xFFFFFFFF);
	m_cStencilTestOp.SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK,IRenderStateOp::STENCIL_FUNC_KEEP,IRenderStateOp::STENCIL_FUNC_KEEP,IRenderStateOp::STENCIL_FUNC_KEEP);
	m_cStencilTestOp.SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK,0xFFFFFFFF);

	glDepthFunc(GL_LEQUAL);
	m_cDepthTestOp.SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_LEQUAL);


	m_cull_face_model = IRenderStateOp::CULL_BACK;
	glCullFace(GL_FRONT);
	glColorMask( GL_TRUE , GL_TRUE , GL_TRUE ,GL_TRUE );

	// 关于深度偏置相关的
	glPolygonOffset(0.0f,0.0f);
	m_PolygonOffset_factor = 0.0;
	m_PolygonOffset_units = 0.0;
	m_bPolygonOffsetFill = false;
	glDisable(GL_POLYGON_OFFSET_FILL);

	m_pRenderStateRecordOp = (CRenderStateRecordOp*)m_pRender->GetRenderStateRecordOp();
}

//混合开关
bool CRenderStateOp::IsEnableBlend()
{
	return m_cBlendColorOp.IsEnableBlend();
}

void CRenderStateOp::EnableBlend(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
	m_pRender->GetPerformance()->nRenderBlendSumNum++;
#endif
	if( bEnable == m_cBlendColorOp.IsEnableBlend() )
	{
		return;
	}
	if(bEnable)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}	
	m_cBlendColorOp.EnableBlend( bEnable );
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
	m_pRender->GetPerformance()->nRenderBlendUsedNum++;
#endif

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_COLOR_BLEND_B;
}

//深度写入管理
bool CRenderStateOp::IsEnableDepthWrite()
{
	return m_cDepthTestOp.IsEnableDepthWrite();
}

void CRenderStateOp::EnableDepthWrite(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if( bEnable == m_cDepthTestOp.IsEnableDepthWrite() )
	{
		return;
	}
	if(bEnable)
	{
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDepthMask(GL_FALSE);
	}

	m_cDepthTestOp.EnableDepthWrite(bEnable);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif
	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_DEPTH_WRITE_B;
}

//是否进行深度测试
bool CRenderStateOp::IsEnableDepthTest()
{
	return m_cDepthTestOp.IsEnableDepthTest();
}

void CRenderStateOp::EnableDepthTest(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if( bEnable == m_cDepthTestOp.IsEnableDepthTest() )
	{
		return;
	}

	if(bEnable)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	m_cDepthTestOp.EnableDepthTest(bEnable);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif
	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_DEPTH_TEST_B;
}

//是否颜色写入
bool CRenderStateOp::IsColorWrite()
{
	return m_cWriteColorOp.GetEnableWriteColorMask();
}

void CRenderStateOp::EnableColorWrite(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif

	if(!bEnable)
	{
		SetColorWriteMask(false,false,false,false);
	}
	else
	{
		SetColorWriteMask(true,true,true,true);
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif
	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_COLOR_WRITE_B;
}

//是否进行模板测试
bool CRenderStateOp::IsStencilTestEnable()
{
	return m_cStencilTestOp.IsStencilTestEnable();
}

void CRenderStateOp::EnableStencilTest(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if( bEnable == m_cStencilTestOp.IsStencilTestEnable() )
	{
		return;
	}

	if( bEnable )
	{
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		// 关闭模板测试
		glDisable(GL_STENCIL_TEST);
	}
	m_cStencilTestOp.EnableStencilTest( bEnable );
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_STENCIL_TEST_B;
}

//是否开启拣选面
bool CRenderStateOp::IsCullFaceEnable()
{
	return m_bCullFace;
}

void CRenderStateOp::EnableCullFace(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif

	if( bEnable == m_bCullFace )
	{
		return;
	}

	if( bEnable )
	{
		glEnable(GL_CULL_FACE);
		if(m_cull_face_model == IRenderStateOp::CULL_FRONT)
		{  
			glCullFace(GL_BACK);
			m_cull_face_model = IRenderStateOp::CULL_FRONT;
		}
		else// if ( m_cull_face_model == IRenderStateOp::CULL_FRONT || m_cull_face_model ==  IRenderStateOp::CULL_NONE)
		{
			glCullFace(GL_FRONT);
			m_cull_face_model = IRenderStateOp::CULL_BACK;
		}
	}
	else
	{
		glDisable(GL_CULL_FACE);
		//m_cull_face_model = IRenderStateOp::CULL_NONE;
	}
	m_bCullFace = bEnable;
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_CULL_FACE_B;
}

//是否启动深度偏置(深度修正)
bool CRenderStateOp::IsPolygonOffsetFillEnable()
{
	return m_bPolygonOffsetFill;	
}

void CRenderStateOp::EnablePolygonOffsetFill(bool bEnable)
{
	if( m_bPolygonOffsetFill == bEnable )
		return;

	if(bEnable)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	m_bPolygonOffsetFill = bEnable;

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_POLYGON_OFFSET_FILL_B;
}

//深度偏置参考值
void CRenderStateOp::SetPolygonOffset(float factor,float units)
{
	if( (factor == m_PolygonOffset_factor) && (units == m_PolygonOffset_units) )
		return;
	glPolygonOffset(factor,units);
	m_PolygonOffset_factor = factor;
	m_PolygonOffset_units = units;

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_POLYGON_OFFSET_C;
}

void CRenderStateOp::GetPolygonOffset(float& factor,float& units)
{
	factor = m_PolygonOffset_factor;
	units = m_PolygonOffset_units;
}


//剪裁测试
bool CRenderStateOp::IsScissorTestEnable()
{
	return m_cScissorTestOp.IsScissorTestEnable();
}

void CRenderStateOp::EnableScissorTest(bool bEnable)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if( bEnable == m_cScissorTestOp.IsScissorTestEnable())
	{
		return;
	}

	if( bEnable )
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
	m_cScissorTestOp.EnableScissorTest(bEnable);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_SCISSOR_TEST_B;
}

void CRenderStateOp::SetCullFaceModel(IRenderStateOp::CULL_FACE_MODE cull_face_model)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif

	if(m_cull_face_model == cull_face_model )
	{
		return;
	}

	m_cull_face_model = cull_face_model;
	switch(cull_face_model)
	{
	case IRenderStateOp::CULL_FRONT:
		//EnableCullFace(true);
		glCullFace(GL_BACK);         
		break;
	case IRenderStateOp::CULL_BACK:
		//EnableCullFace(true);
		glCullFace(GL_FRONT);
		break;
		//	case IRenderStateOp::CULL_NONE:
		//		EnableCullFace(false);
		//		break;
	default:
		;
	};

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	 

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_CULL_FACE_MODEL_B;
}

IRenderStateOp::CULL_FACE_MODE CRenderStateOp::GetCullFaceModel()
{
	return m_cull_face_model;
}

//颜色写入控制
void CRenderStateOp::SetColorWriteMask(bool bRed,bool bGreen,bool bBlue,bool bAlpha)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cWriteColorOp.Equal(bRed,bGreen,bBlue,bAlpha))
	{
		return; 
	}
	glColorMask( bRed?GL_TRUE:GL_FALSE , bGreen?GL_TRUE:GL_FALSE , bBlue?GL_TRUE:GL_FALSE , bAlpha?GL_TRUE:GL_FALSE );
	m_cWriteColorOp.SetWriteColorMaskOp(bRed,bGreen,bBlue,bAlpha);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	 

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_COLOR_WRITE_MASK_B;
}

void CRenderStateOp::GetColorWriteMask(bool &bRed,bool &bGreen,bool &bBlue,bool &bAlpha)
{
	return m_cWriteColorOp.GetWriteColorMaskOp(bRed,bGreen,bBlue,bAlpha);
}

//阿法混合控制 
void CRenderStateOp::SetBlendColor(const FmVec4& blendcolor)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cBlendColorOp.EqualBlendColor(blendcolor))
		return;
	m_cBlendColorOp.SetBlendColor(blendcolor);
	glBlendColor(blendcolor.x,blendcolor.y,blendcolor.z,blendcolor.w);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	 

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_BLEND_COLOR_C;
}

FmVec4 CRenderStateOp::GetBlendColor( )
{
	return m_cBlendColorOp.GetBlendColor();
}

void CRenderStateOp::SetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE modeRGB,IRenderStateOp::BLEND_FUNC_MODE modeAlpha)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cBlendColorOp.EqualBlendEquationSeparate(modeRGB,modeAlpha))
		return;
	m_cBlendColorOp.SetBlendEquationSeparate(modeRGB,modeAlpha);

	GLenum tmodeRGB = BlendFuncModeToGLBlendFuncMode(modeRGB);
	GLenum tmodeAlpha = BlendFuncModeToGLBlendFuncMode(modeAlpha);
	glBlendEquationSeparate(tmodeRGB,tmodeAlpha);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	 

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_BLEND_EQUATION_SEPARATE_C;
}

void CRenderStateOp::GetBlendEquationSeparate(IRenderStateOp::BLEND_FUNC_MODE& modeRGB,IRenderStateOp::BLEND_FUNC_MODE& modeAlpha)
{
	m_cBlendColorOp.GetBlendEquationSeparate(modeRGB,modeAlpha);
}

void CRenderStateOp::SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC srcRGB,IRenderStateOp::BLEND_COLOR_FUNC srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC dstRGB,IRenderStateOp::BLEND_COLOR_FUNC dstAlpha) 
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cBlendColorOp.EqualBlendFuncSeparate(srcRGB,dstRGB,srcAlpha,dstAlpha))
		return;
	m_cBlendColorOp.SetBlendFuncSeparate(srcRGB,srcAlpha,dstRGB,dstAlpha);
	GLenum tsrcRGB = BlendColorFuncToGLBlendColorFunc(srcRGB);
	GLenum tsrcAlpha = BlendColorFuncToGLBlendColorFunc(srcAlpha);
	GLenum tdstRGB = BlendColorFuncToGLBlendColorFunc(dstRGB);
	GLenum tdstAlpha = BlendColorFuncToGLBlendColorFunc(dstAlpha);
	glBlendFuncSeparate(tsrcRGB,tdstRGB,tsrcAlpha,tdstAlpha);
	//glBlendFunc(tsrcRGB,tdstRGB);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	 

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_BLEND_FUNC_SEPARATE_C;
}

void CRenderStateOp::GetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_FUNC& srcRGB,IRenderStateOp::BLEND_COLOR_FUNC& srcAlpha,IRenderStateOp::BLEND_COLOR_FUNC& dstRGB,IRenderStateOp::BLEND_COLOR_FUNC& dstAlpha)
{
	m_cBlendColorOp.GetBlendFuncSeparate(srcRGB,srcAlpha,dstRGB,dstAlpha);
}

void CRenderStateOp::SetBlendEquation(IRenderStateOp::BLEND_FUNC_MODE mode)
{
	SetBlendEquationSeparate(mode,mode);
}

void CRenderStateOp::SetBlendFunc(IRenderStateOp::BLEND_COLOR_FUNC src,IRenderStateOp::BLEND_COLOR_FUNC dst)
{
	SetBlendFuncSeparate(src,src,dst,dst);
}

GLenum CRenderStateOp::BlendFuncModeToGLBlendFuncMode(IRenderStateOp::BLEND_FUNC_MODE mode)
{
	GLenum tmode = GL_FUNC_ADD;
	switch(mode)
	{
	case BLEND_FUNC_ADD:
		tmode = GL_FUNC_ADD;
		break;
	case BLEND_FUNC_SUBTRACT:
		tmode = GL_FUNC_SUBTRACT;
		break;
	case BLEND_FUNC_REVERSE_SUBTRACT:
		tmode = GL_FUNC_REVERSE_SUBTRACT;
		break;
	default:
		break;
	};
	return tmode;
}

GLenum  CRenderStateOp::BlendColorFuncToGLBlendColorFunc(IRenderStateOp::BLEND_COLOR_FUNC func)
{
	GLenum tfunc = GL_ZERO;
	switch(func)
	{
	case BLEND_COLOR_ZERO:
		tfunc = GL_ZERO;
		break;
	case BLEND_COLOR_ONE:
		tfunc = GL_ONE;
		break;
	case BLEND_COLOR_SRC_COLOR:
		tfunc = GL_SRC_COLOR;
		break;
	case BLEND_COLOR_ONE_MINUS_SRC_COLOR:
		tfunc = GL_ONE_MINUS_SRC_COLOR;
		break;
	case BLEND_COLOR_DST_COLOR:
		tfunc = GL_DST_COLOR;
		break;
	case BLEND_COLOR_ONE_MINUS_DST_COLOR:
		tfunc = GL_ONE_MINUS_DST_COLOR;
		break;
	case BLEND_COLOR_SRC_ALPHA:
		tfunc = GL_SRC_ALPHA;
		break;
	case BLEND_COLOR_ONE_MINUS_SRC_ALPHA:
		tfunc = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case BLEND_COLOR_DST_ALPHA:
		tfunc = GL_DST_ALPHA;
		break;
	case BLEND_COLOR_ONE_MINUS_DST_ALPHA:
		tfunc = GL_ONE_MINUS_DST_ALPHA;
		break;
	case BLEND_COLOR_CONSTANT_COLOR:
		tfunc = GL_CONSTANT_COLOR;
		break;
	case BLEND_COLOR_ONE_MINUS_CONSTANT_COLOR:
		tfunc = GL_ONE_MINUS_CONSTANT_COLOR;
		break;
	case BLEND_COLOR_CONSTANT_ALPHA:
		tfunc = GL_CONSTANT_ALPHA;
		break;
	case BLEND_COLOR_ONE_MINUS_CONSTANT_ALPHA:
		tfunc = GL_ONE_MINUS_CONSTANT_ALPHA;
		break;
	case BLEND_COLOR_SRC_ALPHA_SATURATE:
		tfunc = GL_SRC_ALPHA_SATURATE;
		break;
	default:
		break;
	};
	return tfunc;
}

void CRenderStateOp::SetDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if( m_cDepthTestOp.EqualDepthTest(func) )
	{
		return;
	}

	glDepthFunc( DepthTestFuncToGLDepthTestFunc(func) );

	m_cDepthTestOp.SetDepthTestFunc(func);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_DEPTH_TEST_FUNC_C;
}

// 获取当前深度测试的方法
IRenderStateOp::DEPTH_TEST_FUNC CRenderStateOp::GetDepthTestFunc()
{
	return m_cDepthTestOp.GetDepthTestFunc();
}

GLenum CRenderStateOp::DepthTestFuncToGLDepthTestFunc(IRenderStateOp::DEPTH_TEST_FUNC func)
{
	GLenum tfunc = GL_NEVER;
	switch(func)
	{
	case IRenderStateOp::DEPTH_TEST_NEVER:
		tfunc = GL_NEVER;
		break;
	case IRenderStateOp::DEPTH_TEST_ALWAYS:
		tfunc = GL_ALWAYS;
		break;
	case IRenderStateOp::DEPTH_TEST_LESS:
		tfunc = GL_LESS;
		break;
	case IRenderStateOp::DEPTH_TEST_LEQUAL:
		tfunc = GL_LEQUAL;
		break;
	case IRenderStateOp::DEPTH_TEST_EQUAL:
		tfunc = GL_EQUAL;
		break;
	case IRenderStateOp::DEPTH_TEST_GREATER:
		tfunc = GL_GREATER;
		break;
	case IRenderStateOp::DEPTH_TEST_GEQUAL:
		tfunc = GL_GEQUAL;
		break;
	case IRenderStateOp::DEPTH_TEST_NOTEQUAL:
		tfunc = GL_NOTEQUAL;
		break;
	default:
		break;
	};

	return tfunc;
}

GLenum CRenderStateOp::StencilFaceToGLStencilFace(IRenderStateOp::STENCIL_TEST_FACE face)
{
	GLenum tface = GL_FRONT;
	switch(face)
	{
	case IRenderStateOp::STENCIL_TEST_FRONT:
		tface = GL_FRONT;
		break;
	case IRenderStateOp::STENCIL_TEST_BACK:
		tface = GL_BACK;
		break;
	case IRenderStateOp::STENCIL_TEST_FRONT_AND_BACK:
		tface = GL_FRONT_AND_BACK;
		break;
	default:
		break;
	}
	return tface;
}

GLenum CRenderStateOp::StencilFuncToGLStencilFunc(IRenderStateOp::STENCIL_TEST_FUNC func)
{
	GLenum tfunc = GL_NEVER;
	switch(func)
	{
	case IRenderStateOp::STENCIL_TEST_NEVER:
		tfunc = GL_NEVER;
		break;
	case IRenderStateOp::STENCIL_TEST_ALWAYS:
		tfunc = GL_ALWAYS;
		break;
	case IRenderStateOp::STENCIL_TEST_LESS:
		tfunc = GL_LESS;
		break;
	case IRenderStateOp::STENCIL_TEST_LEQUAL:
		tfunc = GL_LEQUAL;
		break;
	case IRenderStateOp::STENCIL_TEST_EQUAL:
		tfunc = GL_EQUAL;
		break;
	case IRenderStateOp::STENCIL_TEST_GREATER:
		tfunc = GL_GREATER;
		break;
	case IRenderStateOp::STENCIL_TEST_GEQUAL:
		tfunc = GL_GEQUAL;
		break;
	case IRenderStateOp::STENCIL_TEST_NOTEQUAL:
		tfunc = GL_NOTEQUAL;
		break;
	default:
		break;
	}
	return tfunc;
}

GLenum CRenderStateOp::StencilModeToGLStencilMode(IRenderStateOp::STENCIL_FUNC_MODE mode)
{
	GLenum tmode = GL_KEEP;

	switch(mode)
	{
	case IRenderStateOp::STENCIL_FUNC_KEEP:
		tmode = GL_KEEP;
		break;
	case IRenderStateOp::STENCIL_FUNC_ZERO:
		tmode = GL_ZERO;
		break;
	case IRenderStateOp::STENCIL_FUNC_REPLACE:
		tmode = GL_REPLACE;
		break;
	case IRenderStateOp::STENCIL_FUNC_INCR:
		tmode = GL_INCR;
		break;
	case IRenderStateOp::STENCIL_FUNC_DECR:
		tmode = GL_DECR;
		break;
	case IRenderStateOp::STENCIL_FUNC_INVERT:
		tmode = GL_INVERT;
		break;
	case IRenderStateOp::STENCIL_FUNC_INCR_WRAP:
		tmode = GL_INCR_WRAP;
		break;
	case IRenderStateOp::STENCIL_FUNC_DECR_WRAP:
		tmode = GL_DECR_WRAP;
		break;
	default:
		break;
	};

	return tmode;
}


// 模板写入的测试方法
void CRenderStateOp::SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC func,int ref,unsigned int mask)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cStencilTestOp.EqualStencilTestFuncSeparate(face,func,ref,mask))
	{
		return;
	}

	glStencilFuncSeparate( StencilFaceToGLStencilFace(face),StencilFuncToGLStencilFunc(func),ref,mask);
	m_cStencilTestOp.SetStencilTestFuncSeparate(face,func,ref,mask);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_STENCIL_TEST_FUNC_SEPARATE_C;
}

bool CRenderStateOp::GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_TEST_FUNC& func,int& ref,unsigned int& mask)
{
	return m_cStencilTestOp.GetStencilTestFuncSeparate(face,func,ref,mask);
}

// 模板写入的操作方法
void CRenderStateOp::SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE sfail,IRenderStateOp::STENCIL_FUNC_MODE dpfail,IRenderStateOp::STENCIL_FUNC_MODE dppass)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cStencilTestOp.EqualStencilTestOpSeparate(face,sfail,dpfail,dppass))
	{
		return;
	}

	glStencilOpSeparate( StencilFaceToGLStencilFace(face),StencilModeToGLStencilMode(sfail),StencilModeToGLStencilMode(dpfail),StencilModeToGLStencilMode(dppass));
	m_cStencilTestOp.SetStencilTestOpSeparate(face,sfail,dpfail,dppass);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_STENCIL_TEST_OP_SEPARATE_C;
}

bool CRenderStateOp::GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FACE face,IRenderStateOp::STENCIL_FUNC_MODE& sfail,IRenderStateOp::STENCIL_FUNC_MODE& dpfail,IRenderStateOp::STENCIL_FUNC_MODE& dppass)
{
	return m_cStencilTestOp.GetStencilTestOpSeparate(face,sfail,dpfail,dppass);
}

// 模板写入遮罩值
void CRenderStateOp::SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int mask)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cStencilTestOp.EqualStencilTestMaskSeparate(face,mask))
	{
		return;
	}
	glStencilMaskSeparate(StencilFaceToGLStencilFace(face),mask);
	m_cStencilTestOp.SetStencilTestMaskSeparate(face,mask);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_STENCIL_TEST_MASK_SEPARATE_C;
}

bool CRenderStateOp::GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FACE face,unsigned int& mask)
{
	return m_cStencilTestOp.GetStencilTestMaskSeparate(face,mask);
}

// 区域裁剪
void CRenderStateOp::SetScissorTest(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif
	if(m_cScissorTestOp.EqualScissorTest(x,y,width,height))
	{
		return;
	}
	glScissor(x,y,width,height);
	m_cScissorTestOp.SetScissorTest(x,y,width,height);
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_SCISSOR_TEST_C;
}

void CRenderStateOp::GetScissorTest(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height)
{
	m_cScissorTestOp.GetScissorTest(x,y,width,height);
}

// 设置视口大小
void CRenderStateOp::SetViewPort(unsigned int x,unsigned int y,fm_sizei width,fm_sizei height)
{
#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateSumNum++;
#endif

	if( m_cViewPortOp.EqualViewPort(x,y,width,height) )
	{
		return;
	}

	glViewport(x,y,width,height);
	m_cViewPortOp.SetViewPort(x,y,width,height);

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nRenderStateUsedNum++;
#endif	

	unsigned int& flag = m_pRenderStateRecordOp->GetRenderStateRecordFlag();
	flag = flag | CRenderStateRecordOp::FLAG_VIEWPORT_C;
}


// 获取当前视口大小
void CRenderStateOp::GetViewPort(unsigned int& x,unsigned int& y,fm_sizei& width,fm_sizei& height)
{
	m_cViewPortOp.GetViewPort(x,y,width,height);
}


IRenderStateRecordOp* CRenderStateRecordOp::NewInstance(Render* pRender)
{
	IRenderStateRecordOp* p = (IRenderStateRecordOp*)CORE_ALLOC(sizeof(CRenderStateRecordOp));

	new (p) CRenderStateRecordOp(pRender);

	return p;
}

CRenderStateRecordOp::CRenderStateRecordOp(Render* pRender):m_pRender(pRender),m_RenderStateFlag(FLAG_RENDER_STATE_NONE)
{
	m_pRenderStateOp = (CRenderStateOp*)CORE_ALLOC(sizeof(CRenderStateOp));
}

CRenderStateRecordOp::~CRenderStateRecordOp()
{  
	CORE_DELETE(m_pRenderStateOp);
}
// 释放
void CRenderStateRecordOp::Release()
{
	CORE_DELETE(this);
}

//开始记录状态
void CRenderStateRecordOp::Store()
{
	//拷贝一份状态
	memcpy(m_pRenderStateOp,m_pRender->GetRenderStateOp(),sizeof(CRenderStateOp));

	//清楚标记
	m_RenderStateFlag = FLAG_RENDER_STATE_NONE;
}

//开始恢复渲染状态
void CRenderStateRecordOp::Restore()
{
	//恢复状态逻辑
	if(FLAG_RENDER_STATE_GROUP_NO_CHANGE == m_RenderStateFlag)
		return;

	//进行状态恢复
	IRenderStateOp* pRenderStateOp = m_pRender->GetRenderStateOp();

	//是否有深度操作
	if(FLAG_RENDER_STATE_GROUP_DEPTH_OP & m_RenderStateFlag)
	{
		RestoreDepthOp(pRenderStateOp);
	}
	//是否有模版操作
	if(FLAG_RENDER_STATE_GROUP_STENCIL_OP & m_RenderStateFlag)
	{
		RestoreStencilOp(pRenderStateOp);
	}

	//是否有颜色写入操作
	if(FLAG_RENDER_STATE_GROUP_COLOR_OP &  m_RenderStateFlag)
	{
		RestoreColorOp(pRenderStateOp);
	}

	//是否颜色混合操作
	if(FLAG_RENDER_STATE_GROUP_COLOR_BLEND_OP & m_RenderStateFlag)
	{
		RestoreColorBlendOp(pRenderStateOp);
	}

	//裁剪操作
	if(FLAG_RENDER_STATE_GROUP_CULL_OP & m_RenderStateFlag)
	{
		RestoreCullOp(pRenderStateOp);
	}

	//是否深度偏移操作
	if(FLAG_RENDER_STATE_GROUP_POLYGON_OFFSET_OP & m_RenderStateFlag)
	{
		RestorePolyGonOffsetOp(pRenderStateOp);
	}

	//是否遮罩操作
	if(FLAG_RENDER_STATE_GROUP_SCISSOR_OP & m_RenderStateFlag)
	{
		RestoreScissor(pRenderStateOp);
	}

	//视口调整
	if(FLAG_RENDER_STATE_VIEWPORT_OP & m_RenderStateFlag)
	{
		RestoreViewPortOp(pRenderStateOp);
	}
}


void CRenderStateRecordOp::RestoreDepthOp(IRenderStateOp* pRenderStateOp)
{
	if(m_RenderStateFlag & FLAG_DEPTH_WRITE_B)
	{
		bool bEnableDepthWrite = m_pRenderStateOp->IsEnableDepthWrite();
		pRenderStateOp->EnableDepthWrite(bEnableDepthWrite);
	}

	if(m_RenderStateFlag & FLAG_DEPTH_TEST_B)
	{
		bool bEnableDepthTest = m_pRenderStateOp->IsEnableDepthTest();
		pRenderStateOp->EnableDepthTest(bEnableDepthTest);
	}

	if(m_RenderStateFlag & FLAG_DEPTH_TEST_FUNC_C)
	{
		IRenderStateOp::DEPTH_TEST_FUNC func = m_pRenderStateOp->GetDepthTestFunc();
		pRenderStateOp->SetDepthTestFunc(func);
	}
}

void CRenderStateRecordOp::RestoreStencilOp(IRenderStateOp* pRenderStateOp)
{ 
	//模板开关
	if(m_RenderStateFlag & FLAG_STENCIL_TEST_B)
	{
		bool bStencilTestEnable = m_pRenderStateOp->IsStencilTestEnable();
		pRenderStateOp->EnableStencilTest(bStencilTestEnable);
	}

	if(m_RenderStateFlag & FLAG_STENCIL_TEST_FUNC_SEPARATE_C )
	{
		IRenderStateOp::STENCIL_TEST_FUNC func;
		int ref;
		unsigned int mask;
		m_pRenderStateOp->GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FRONT,func,ref,mask);
		pRenderStateOp->SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_FRONT,func,ref,mask);

		m_pRenderStateOp->GetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_BACK,func,ref,mask);
		pRenderStateOp->SetStencilTestFuncSeparate(IRenderStateOp::STENCIL_TEST_BACK,func,ref,mask);
	}

	if(m_RenderStateFlag & FLAG_STENCIL_TEST_OP_SEPARATE_C)
	{
		IRenderStateOp::STENCIL_FUNC_MODE sfail;
		IRenderStateOp::STENCIL_FUNC_MODE dpfail;
		IRenderStateOp::STENCIL_FUNC_MODE dppass;
		m_pRenderStateOp->GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FRONT,sfail,dpfail,dppass);
		pRenderStateOp->SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_FRONT,sfail,dpfail,dppass);

		m_pRenderStateOp->GetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_BACK,sfail,dpfail,dppass);
		pRenderStateOp->SetStencilTestOpSeparate(IRenderStateOp::STENCIL_TEST_BACK,sfail,dpfail,dppass);
	}

	if(m_RenderStateFlag & FLAG_STENCIL_TEST_MASK_SEPARATE_C)
	{
		unsigned int mask;
		m_pRenderStateOp->GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FRONT,mask);
		pRenderStateOp->SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_FRONT,mask);

		m_pRenderStateOp->GetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_BACK,mask);
		pRenderStateOp->SetStencilTestMaskSeparate(IRenderStateOp::STENCIL_TEST_BACK,mask);
	}
}

void CRenderStateRecordOp::RestoreColorOp(IRenderStateOp* pRenderStateOp)
{
	if(m_RenderStateFlag & FLAG_COLOR_WRITE_B)
	{
		bool bColorWrite = m_pRenderStateOp->IsColorWrite();
		pRenderStateOp->EnableColorWrite(bColorWrite);
	}

	if(m_RenderStateFlag & FLAG_COLOR_WRITE_MASK_B)
	{
		bool bRed;
		bool bGreen;
		bool bBlue;
		bool bAlpha;
		m_pRenderStateOp->GetColorWriteMask(bRed,bGreen,bBlue,bAlpha);
		pRenderStateOp->SetColorWriteMask(bRed,bGreen,bBlue,bAlpha);
	}

}

void CRenderStateRecordOp::RestoreColorBlendOp(IRenderStateOp* pRenderStateOp)
{
	if(m_RenderStateFlag & FLAG_COLOR_BLEND_B)
	{
		bool bEnableBlend = m_pRenderStateOp->IsEnableBlend();
		pRenderStateOp->EnableBlend(bEnableBlend);
	}

	if(m_RenderStateFlag & FLAG_BLEND_COLOR_C)
	{
		FmVec4 vec4Color = m_pRenderStateOp->GetBlendColor();
		pRenderStateOp->SetBlendColor(vec4Color);
	}

	if(m_RenderStateFlag & FLAG_BLEND_EQUATION_SEPARATE_C)
	{
		IRenderStateOp::BLEND_FUNC_MODE modeRGB;
		IRenderStateOp::BLEND_FUNC_MODE modeAlpha;
		m_pRenderStateOp->GetBlendEquationSeparate(modeRGB,modeAlpha);
		pRenderStateOp->SetBlendEquationSeparate(modeRGB,modeAlpha);
	}

	if(m_RenderStateFlag & FLAG_BLEND_FUNC_SEPARATE_C)
	{
		IRenderStateOp::BLEND_COLOR_FUNC srcRGB;
		IRenderStateOp::BLEND_COLOR_FUNC srcAlpha;
		IRenderStateOp::BLEND_COLOR_FUNC dstRGB;
		IRenderStateOp::BLEND_COLOR_FUNC dstAlpha;
		m_pRenderStateOp->GetBlendFuncSeparate(srcRGB,srcAlpha,dstRGB,dstAlpha );
		pRenderStateOp->SetBlendFuncSeparate(srcRGB,srcAlpha,dstRGB,dstAlpha );
	}

}

void CRenderStateRecordOp::RestoreCullOp(IRenderStateOp* pRenderStateOp)
{
	if(m_RenderStateFlag & FLAG_CULL_FACE_B)
	{
		bool bCullFace = m_pRenderStateOp->IsCullFaceEnable();
		pRenderStateOp->EnableCullFace(bCullFace);
	}

	if(m_RenderStateFlag & FLAG_CULL_FACE_MODEL_B)
	{
		IRenderStateOp::CULL_FACE_MODE cull_face_mode = m_pRenderStateOp->GetCullFaceModel();
		pRenderStateOp->SetCullFaceModel(cull_face_mode);
	}
}

void CRenderStateRecordOp::RestorePolyGonOffsetOp(IRenderStateOp* pRenderStateOp)
{
	if( m_RenderStateFlag & FLAG_POLYGON_OFFSET_FILL_B )
	{
		bool bPolygonOffsetFillEnable = m_pRenderStateOp->IsPolygonOffsetFillEnable();
		pRenderStateOp->EnablePolygonOffsetFill(bPolygonOffsetFillEnable);
	}

	if( m_RenderStateFlag & FLAG_POLYGON_OFFSET_C )
	{
		float factor;
		float units;
		m_pRenderStateOp->GetPolygonOffset(factor,units);
		pRenderStateOp->SetPolygonOffset(factor,units);
	}
}

void CRenderStateRecordOp::RestoreScissor(IRenderStateOp* pRenderStateOp)
{
	if( m_RenderStateFlag & FLAG_SCISSOR_TEST_B )
	{
		bool bScissorTestEnable = m_pRenderStateOp->IsScissorTestEnable();
		pRenderStateOp->EnableScissorTest(bScissorTestEnable);
	}

	if( m_RenderStateFlag & FLAG_SCISSOR_TEST_C )
	{
		unsigned int x;
		unsigned int y;
		fm_sizei width;
		fm_sizei height;
		m_pRenderStateOp->GetScissorTest(x,y,width,height);
		pRenderStateOp->SetScissorTest(x,y,width,height);
	}
}

void CRenderStateRecordOp::RestoreViewPortOp(IRenderStateOp* pRenderStateOp)
{
	if( m_RenderStateFlag & FLAG_VIEWPORT_C)
	{
		unsigned int x = 0;
		unsigned int y = 0;
		fm_sizei width = 0;
		fm_sizei height = 0;
		m_pRenderStateOp->GetViewPort(x,y,width,height);
		pRenderStateOp->SetViewPort(x,y,width,height);
	}
}

unsigned int& CRenderStateRecordOp::GetRenderStateRecordFlag()
{
	return m_RenderStateFlag;
}