//--------------------------------------------------------------------
// 文件名:		paint_batch.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年7月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "paint_batch.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/i_scene.h"

struct GuiShaderData
{
    IShaderProgram* pShaderProgram;
    fm_int hTwoPixelSize;
    fm_int hMtxRotate;
    fm_int hAlphaValue;
    fm_int hTexBaseMap;
	fm_int hOutline;
};

struct GuiShaderKey
{
	void * ps;
	void * vs;
};

template<>
class TPodTraits<GuiShaderKey>
{
public:
	static size_t GetHash(const GuiShaderKey& value)
	{
		return size_t(value.ps) + size_t(value.vs);
	}

	static bool Equal(const GuiShaderKey& s1, const GuiShaderKey& s2)
	{
		return s1.ps == s2.ps && s1.vs == s2.vs;
	}
};

class GuiShaderManager
{
public:
	GuiShaderData GetShader(Render* prender, IVertexShader* pVS, IPixelShader* pPS, const char** name, int len)
	{
	
		GuiShaderKey key;
		key.ps = pPS;;
		key.vs = pVS;

		if(m_ShaderPrograms.Exists(key))
		{
			return m_ShaderPrograms.Find(key).GetData();
		}
		
        GuiShaderData shaderData;
        shaderData.pShaderProgram = prender->CreateShaderProgram(pVS, pPS, name, len);
        Assert(shaderData.pShaderProgram);

        IShaderParamOp* pShaderParamOp = shaderData.pShaderProgram->GetParamOp();
        shaderData.hTwoPixelSize = pShaderParamOp->FindParamIdByName("c_TwoPixelSize");
        shaderData.hMtxRotate = pShaderParamOp->FindParamIdByName("c_mtxRotate");
        shaderData.hAlphaValue = pShaderParamOp->FindParamIdByName("c_fAlphaValue");
        shaderData.hTexBaseMap = pShaderParamOp->FindParamIdByName("tex_BaseMap");
		shaderData.hOutline = pShaderParamOp->FindParamIdByName("c_outline_color");
        
		m_ShaderPrograms.Add(key, shaderData);
		return shaderData;
	//	return m_ShaderPrograms[key];
	}

	void Clear()
	{
		for(TPodHashMap<GuiShaderKey, GuiShaderData, TPodTraits<GuiShaderKey>, TPaintBatchAlloc> ::iterator it = m_ShaderPrograms.Begin(); it!= m_ShaderPrograms.End(); it++)
		{
			it.GetData().pShaderProgram->Release();
		}
	
		m_ShaderPrograms.Clear();
		if(s_inst)
		{
			delete(s_inst);
			s_inst = 0;
		}
	}

	static GuiShaderManager& Inst()
	{
		if(s_inst == 0)
			s_inst = new GuiShaderManager;
		return *s_inst;
	}

private:
	static GuiShaderManager* s_inst;
	TPodHashMap<GuiShaderKey, GuiShaderData, TPodTraits<GuiShaderKey>, TPaintBatchAlloc> m_ShaderPrograms;
};

GuiShaderManager* GuiShaderManager::s_inst = 0;

// CPaintBatchs

CPaintBatchs* CPaintBatchs::NewInstance(Render* pRender)
{
	CPaintBatchs* p = (CPaintBatchs*)CORE_ALLOC(sizeof(CPaintBatchs));

	new (p) CPaintBatchs(pRender);

	return p;
}

CPaintBatchs::CPaintBatchs(Render* pRender)
{
	Assert(pRender != NULL);

	m_pFrameRT = NULL;
	m_pRender = pRender;
	m_bScaleEnable = false;
	m_nBatchUsed = 0;
	m_CurViewport.nLeft = 0;
	m_CurViewport.nTop = 0;
	m_CurViewport.nWidth = 0;
	m_CurViewport.nHeight = 0;
	m_nLineVBSize = 0;
	m_nPictVBSize = 0;
	m_nPointVBSize = 0;
	m_pLineVB = NULL;
	m_pPictVB = NULL;
	m_pPointVB = NULL;
	m_debug_batch_num = 0xffffffff;
}

CPaintBatchs::~CPaintBatchs()
{
	GuiShaderManager::Inst().Clear();
	ClearData();

	for (size_t i = 0; i < m_BatchCache.size(); ++i)
	{
		CORE_FREE(m_BatchCache[i], sizeof(batch_t));
	}

	SAFE_RELEASE(m_pLineVB);
	SAFE_RELEASE(m_pPictVB);
	SAFE_RELEASE(m_pPointVB);
}

void CPaintBatchs::Release()
{
	CORE_DELETE(this);
}

bool CPaintBatchs::Initialize()
{
	m_GuiVSList.SetRender(m_pRender);
	m_GuiVSList.SetShaderName("gui.vsh", "main");
	m_GuiVSList.SetDefine(GUI_VS_USE_POINT, "#define USE_POINT 1\n");
	m_GuiVSList.SetDefine(GUI_VS_USE_PICTURE, "#define USE_PICTURE 1\n");
	m_GuiVSList.SetDefine(GUI_VS_USE_ROTATE, "#define USE_ROTATE 1\n");
	m_GuiVSList.SetDefine(GUI_VS_USE_DEPTH, "#define USE_DEPTH 1\n");
	m_GuiVSList.GetShader(0);

	m_GuiPSList.SetRender(m_pRender);
	m_GuiPSList.SetShaderName("gui.fsh", "main");
	m_GuiPSList.SetDefine(GUI_PS_USE_PICTURE, "#define USE_PICTURE 1\n");
	m_GuiPSList.SetDefine(GUI_PS_USE_PICTUREA8, "#define USE_PICTUREA8 1\n");
	m_GuiPSList.SetDefine(GUI_PS_USE_FADE, "#define USE_FADE 1\n");
	m_GuiPSList.SetDefine(GUI_PS_USE_BLACKWHITE, "#define USE_BLACKWHITE 1\n");
	m_GuiPSList.SetDefine(GUI_PS_USE_OUTLINE, "#define USE_OUTLINE 1\n");
	m_GuiPSList.GetShader(0);

	return true;
}

void* CPaintBatchs::AddBatch(float depth_z, float depth_w, bool use_blend, 
	bool use_texture, bool use_black_white, bool use_texture_a8, 
	int blend_alpha, int blend_mode, IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, 
	IShaderTex* pTexture, IScene* pScene, const FmMat4* pRotate, const FmVec3* pOutLineColor, bool outline_fade)
{
	int vertex_num = 0;
	int start_vertex = 0;
	void* pdata = NULL;

	switch (primitive_type)
	{
	case IRenderDrawOp::DRAW_POINTS:
		{
			vertex_num = primitive_count * 2;
			start_vertex = (int)m_VertexPoint.size();
			m_VertexPoint.resize(start_vertex + vertex_num);
			pdata = &m_VertexPoint[start_vertex];
		}
		break;
	case IRenderDrawOp::DRAW_LINES:
		{
			vertex_num = primitive_count * 2;
			start_vertex = (int)m_VertexLine.size();
			m_VertexLine.resize(start_vertex + vertex_num);
			pdata = &m_VertexLine[start_vertex];
		}
		break;
	case IRenderDrawOp::DRAW_LINE_STRIP:
		{
			vertex_num = primitive_count + 1;
			start_vertex = (int)m_VertexLine.size();
			m_VertexLine.resize(start_vertex + vertex_num);
			pdata = &m_VertexLine[start_vertex];
		}
		break;
	case IRenderDrawOp:: DRAW_TRIANGLES:
		{
			vertex_num = primitive_count * 3;
			
			if (use_texture)
			{
				start_vertex = (int)m_VertexPict.size();
				m_VertexPict.resize(start_vertex + vertex_num);
				pdata = &m_VertexPict[start_vertex];
			}
			else
			{
				start_vertex = (int)m_VertexLine.size();
				m_VertexLine.resize(start_vertex + vertex_num);
				pdata = &m_VertexLine[start_vertex];
			}
		}
		break;
	case IRenderDrawOp::DRAW_TRIANGLE_STRIP:
		{
			vertex_num = primitive_count + 2;
			
			if (use_texture)
			{
				start_vertex = (int)m_VertexPict.size();
				m_VertexPict.resize(start_vertex + vertex_num);
				pdata = &m_VertexPict[start_vertex];
			}
			else
			{
				start_vertex = (int)m_VertexLine.size();
				m_VertexLine.resize(start_vertex + vertex_num);
				pdata = &m_VertexLine[start_vertex];
			}
		}
		break;
	default:
		if (NULL == pScene)
		{
			Assert(0);
		}
		break;
	}

	// 获得当前的视口
	int view_left = m_CurViewport.nLeft;
	int view_top = m_CurViewport.nTop;
	int view_width = m_CurViewport.nWidth;
	int view_height = m_CurViewport.nHeight;
	// 生成批次标识
	unsigned int flags = 0;
	bool use_rotate = false;
	bool use_depth = false;
	bool use_fade = false;
	bool use_outline = false;

	if (blend_alpha < 255)
	{
		use_blend = true;
		flags |= USE_FADE;
		use_fade = true;
	}

	if(pOutLineColor != 0)
	{
		flags |= USE_OUTLINE;
		use_outline = true;
	}

	if (depth_z > 0.0F)
	{
		flags |= USE_DEPTH;
		use_depth = true;
	}

	if (use_blend)
	{
		flags |= USE_BLEND;
	}

	if (use_texture)
	{
		flags |= USE_TEXTURE;
	}

	if (use_black_white)
	{
		flags |= USE_BLACKWHITE;
	}

	if (pRotate)
	{
		flags |= USE_ROTATE;
		use_rotate = true;
	}

    if (use_texture_a8)
    {
        flags |= USE_TEXTUREA8;
    }

	if(outline_fade)
		flags |= USE_OUTLINE_FADE;

	if (!m_Batchs.empty())
	{
		// 合并类似的批次
		batch_t* pPrevBatch = m_Batchs.back();

		if ((NULL == pScene) && (NULL == pRotate)
			&& (flags == pPrevBatch->nFlags)
			&& (primitive_type != GL_LINE_STRIP)
			&& (primitive_type != GL_TRIANGLE_STRIP)
			&& (primitive_type == pPrevBatch->nPrimitiveType)
			&& (blend_alpha == pPrevBatch->nBlendAlpha)
			&& (blend_mode == pPrevBatch->nBlendMode)
			&& (pTexture == pPrevBatch->pTexture)
			&& (depth_z == pPrevBatch->fDepthZ)
			&& (view_left == pPrevBatch->nViewLeft)
			&& (view_top == pPrevBatch->nViewTop)
			&& (view_width == pPrevBatch->nViewWidth)
			&& (view_height == pPrevBatch->nViewHeight)
			&& m_pFrameRT == pPrevBatch->pFrameRT)
		{
			pPrevBatch->nPrimitiveCount += primitive_count;
			pPrevBatch->nVertexCount += vertex_num;
			return pdata;
		}
	}

	batch_t* pBatch;

	if (m_nBatchUsed == m_BatchCache.size())
	{
		pBatch = (batch_t*)CORE_ALLOC(sizeof(batch_t));
		m_BatchCache.push_back(pBatch);
	}
	else
	{
		pBatch = m_BatchCache[m_nBatchUsed];
	}

	++m_nBatchUsed;

	pBatch->nFlags = flags;
	pBatch->nPrimitiveType = primitive_type;
	pBatch->nPrimitiveCount = primitive_count;
	pBatch->nStartVertex = start_vertex;
	pBatch->nVertexCount = vertex_num;
	pBatch->nBlendAlpha = blend_alpha;
	pBatch->nBlendMode = blend_mode;
	pBatch->pFrameRT = m_pFrameRT;
	pBatch->pTexture = pTexture;
	// 记录当前的视口
	pBatch->nViewLeft = view_left;
	pBatch->nViewTop = view_top;
	pBatch->nViewWidth = view_width;
	pBatch->nViewHeight = view_height;
	pBatch->fDepthZ = depth_z;
	pBatch->pScene = pScene;
	
	if(use_outline)
		pBatch->nOutlineColor = *pOutLineColor;

	if(pScene)
		pBatch->pSceneView = pScene->GetSceneView();
	else
		pBatch->pSceneView = 0;
	
	if (pRotate)
	{
		pBatch->mtxRotate = *pRotate;
	}

	pBatch->pVertexShader = GetGuiVS(primitive_type == GL_POINTS, use_texture, use_rotate, use_depth);
	pBatch->pPixelShader = GetGuiPS(use_texture, use_texture_a8, use_fade, use_black_white, use_outline);

	/*
	if (primitive_type == GL_POINTS)
	{
		pBatch->pVertexShader = GetGuiPointVS(use_rotate, use_depth);
		pBatch->pPixelShader = GetGuiPointPS(use_fade);
	}
	else if (use_texture)
	{
		if (use_black_white)
		{
			pBatch->pVertexShader = GetGuiBWPictVS(use_rotate, use_depth);
			pBatch->pPixelShader = GetGuiBWPictPS(use_fade);
		}
        else if (use_texture_a8)
        {
			pBatch->pVertexShader = GetGuiA8PictVS(use_rotate, use_depth);
			pBatch->pPixelShader = GetGuiA8PictPS(use_fade);
        }
		else
		{
			pBatch->pVertexShader = GetGuiPictVS(use_rotate, use_depth);
			pBatch->pPixelShader = GetGuiPictPS(use_fade);
		}
	}
	else
	{
		pBatch->pVertexShader = GetGuiLineVS(use_rotate, use_depth);
		pBatch->pPixelShader = GetGuiLinePS(use_fade);
	}*/

	m_Batchs.push_back(pBatch);
	
	return pdata;
}

void CPaintBatchs::ClearData()
{
	// 尝试释放一些内存
	if (m_BatchCache.size() > (m_nBatchUsed * 2 + 16))
	{
		for (size_t j = m_nBatchUsed; j < m_BatchCache.size(); ++j)
		{
			CORE_FREE(m_BatchCache[j], sizeof(batch_t));
		}

		m_BatchCache.resize(m_nBatchUsed);
	}

	for (size_t i = 0; i < m_nBatchUsed; ++i)
	{
		m_BatchCache[i]->pTexture = NULL;
	}

	m_CurViewport.nLeft = 0;
	m_CurViewport.nTop = 0;
	m_CurViewport.nWidth = m_pRender->GetDeviceWidth();
	m_CurViewport.nHeight = m_pRender->GetDeviceHeight();

	m_ViewportStack.clear();
	m_Batchs.clear();
	m_VertexLine.clear();
	m_VertexPict.clear();
	m_VertexPoint.clear();

	m_bScaleEnable = false;
	m_nBatchUsed = 0;
}

void CPaintBatchs::SetPaintState()
{
	if (m_bScaleEnable)
	{
		//ID3D11SamplerState* pLinearSampler = m_pLinearSamplerState->GetState();

		//m_pDeviceContext->PSSetSamplers(0, 1, &pLinearSampler);
	}
	else
	{
		// 无缩放时使用点过滤
		//ID3D11SamplerState* pPointSampler = m_pPointSamplerState->GetState();

		//m_pDeviceContext->PSSetSamplers(0, 1, &pPointSampler);
	}

//	m_pDeviceContext->RSSetState(m_pRasterState->GetState());
//	m_pDeviceContext->OMSetDepthStencilState(
//		m_pNoTestDepthState->GetState(), 0);
//	m_pDeviceContext->OMSetBlendState(m_pNoBlendState->GetState(), NULL, 
//		0xFFFFFFFF);

//	ID3D11Buffer* pCBuffers[2] = { 
//		m_pObjectCB->GetBuffer(), m_pScreenCB->GetBuffer() };
//
//	m_pDeviceContext->VSSetConstantBuffers(0, 2, pCBuffers);
//	m_pDeviceContext->PSSetConstantBuffers(0, 2, pCBuffers);
}

void CPaintBatchs::SetConstValue(GuiShaderData& shaderData, CPaintBatchs::batch_t* pBatch)
{
	IShaderParamOp* pShaderParamOp = shaderData.pShaderProgram->GetParamOp();
	if (pBatch->nFlags & (USE_ROTATE + USE_FADE))
	{
		pShaderParamOp->SetParamValue(shaderData.hMtxRotate, pBatch->mtxRotate);
		pShaderParamOp->SetParamValue(shaderData.hAlphaValue, pBatch->nBlendAlpha / 255.0F);
	}

	if(pBatch->nFlags & USE_OUTLINE)
	{
		pShaderParamOp->SetParamValue(shaderData.hOutline, pBatch->nOutlineColor);
	}

	int device_width;
	int device_height;

	//如果有FrameRT,画布大小取rt的大小
	if(pBatch->pFrameRT)
	{
		IColorRT* pColorRT = pBatch->pFrameRT->GetColorRT();
		device_width = pColorRT->GetWidth();
		device_height = pColorRT->GetHeight();
	}
	else
	{
		device_width = m_pRender->GetDeviceWidth();
		device_height = m_pRender->GetDeviceHeight();
	}

	pShaderParamOp->SetParamValue(shaderData.hTwoPixelSize, 2.0f/ (float)device_width, 2.0f/(float)device_height, 1.0f, 1.0f);
}

bool CPaintBatchs::RenderFlat()
{
    IRenderStateOp* pRenderStateOp = m_pRender->GetRenderStateOp();	

	pRenderStateOp->EnableCullFace(false);
    pRenderStateOp->EnableDepthTest(false);
	pRenderStateOp->EnableDepthWrite(false);
	pRenderStateOp->EnableScissorTest(true);

	IFrameRT* pOldFrame = m_pRender->GetUsedFrameRT();

	if (!m_VertexLine.empty())
	{
		size_t size = m_VertexLine.size() * sizeof(vertex_line_t);
		
		if (size > m_nLineVBSize)
		{
			SAFE_RELEASE(m_pLineVB);
			m_pLineVB = m_pRender->CreateDynamicVB((unsigned int)size,"CPaintBatchs::RenderFlat VertexLine");
			m_nLineVBSize = size;
		}

		if(m_pLineVB)
			m_pRender->UpdateDynamicVB(m_pLineVB, 0, &m_VertexLine[0], size);
	}

	if (!m_VertexPict.empty())
	{
		size_t size = m_VertexPict.size() * sizeof(vertex_pict_t);

		if (size > m_nPictVBSize)
		{
			SAFE_RELEASE(m_pPictVB);
			m_pPictVB = m_pRender->CreateDynamicVB((unsigned int)size,"CPaintBatchs::RenderFlat VertexPict");
			m_nPictVBSize = size;
		}

		if(m_pPictVB)
			m_pRender->UpdateDynamicVB(m_pPictVB, 0, &m_VertexPict[0], size);
	}

	if (!m_VertexPoint.empty())
	{
		size_t size = m_VertexPoint.size() * sizeof(vertex_point_t);

		if (size > m_nPointVBSize)
		{
			SAFE_RELEASE(m_pPointVB);
			m_pPointVB = m_pRender->CreateDynamicVB((unsigned int)size,"CPaintBatchs::RenderFlat VertexPoint");
			m_nPointVBSize = size;
		}

		if(m_pPointVB)
			m_pRender->UpdateDynamicVB(m_pPointVB, 0, &m_pPointVB[0], size);
	}

	int device_width = m_pRender->GetDeviceWidth();
	int device_height = m_pRender->GetDeviceHeight();

	int _device_width = device_width;
	int _device_height = device_height;
	SetPaintState();

	int cur_left = 0;
	int cur_top = 0;
	int cur_width = device_width;
	int cur_height = device_height;
	int prev_flags = -1;
	int prev_blend_alpha = -1;
	float prev_depth = -1.0F;
	IVertexShader* prev_vs = NULL;
	IPixelShader* prev_ps = NULL;

	size_t batch_num = FM_MIN(m_Batchs.size(), m_debug_batch_num);

	IFrameRT* pFrameRT = NULL;
	
	for (size_t i = 0; i < batch_num; ++i)
	{
		batch_t* pBatch = m_Batchs[i];
		
		if(pFrameRT != pBatch->pFrameRT)
		{
			pFrameRT = pBatch->pFrameRT;
			if(pFrameRT)
			{
				IColorRT* pColorRT = pFrameRT->GetColorRT();

				cur_width = (int)pColorRT->GetWidth();
				cur_height = (int)pColorRT->GetHeight();
			
				_device_width = cur_width;
				_device_height = cur_height;

				pFrameRT->UsedFrameRT();
				pRenderStateOp->SetViewPort(0, 0, cur_width, cur_height);
				IRenderDrawOp* pRenderDrawOp = m_pRender->GetRenderDrawOp();
				pRenderDrawOp->SetClearColor(FmVec4(0.0, 0.0, 0.0, 0.0));
				pRenderDrawOp->SetClearDepth(1.0F);
				pRenderDrawOp->SetClear(IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER);
			}
			else
			{
				cur_width = device_width;
				cur_height = device_height;
				_device_width = device_width;
				_device_height = device_height;
				m_pRender->UsedFrameRT(pOldFrame);
				pRenderStateOp->SetViewPort(0, 0, device_width, device_height);
			}
		}

		//开启一个framert
		if ((pBatch->nViewLeft != cur_left) 
			|| (pBatch->nViewTop != cur_top)
			|| (pBatch->nViewWidth != cur_width) 
			|| (pBatch->nViewHeight != cur_height))
		{
			cur_left = pBatch->nViewLeft;
			cur_top = pBatch->nViewTop;
			cur_width = pBatch->nViewWidth;
			cur_height = pBatch->nViewHeight;

			pRenderStateOp->SetScissorTest(cur_left, _device_height - cur_top - cur_height, cur_width, cur_height);
		}

		
		//drawscene
		if(pBatch->pSceneView != NULL)
		{
			unsigned int x, y;
			fm_sizei w, h;
			pRenderStateOp->GetViewPort(x, y, w, h);
			pRenderStateOp->SetViewPort(cur_left, _device_height - cur_top - cur_height, cur_width, cur_height);

			//画scene前置下状态
			pRenderStateOp->EnableBlend(false);
			pRenderStateOp->EnableDepthTest(true);
			pRenderStateOp->EnableCullFace(true);
			pRenderStateOp->EnableDepthWrite(true);

			//画scene
			pBatch->pSceneView->DrawScene();

			//画完scene后，把gui的渲染状态重置
			pRenderStateOp->EnableCullFace(false);
			pRenderStateOp->EnableDepthTest(false);
			pRenderStateOp->EnableDepthWrite(false);
			pRenderStateOp->EnableScissorTest(true);

			pRenderStateOp->SetViewPort(x, y, w, h);
			pBatch->pSceneView->ClearBatchs();

		}

		// 尽量减少渲染状态的切换
		if ((pBatch->nFlags != prev_flags)
			|| (pBatch->nBlendAlpha != prev_blend_alpha)
			|| (pBatch->fDepthZ != prev_depth))
		{
			if (pBatch->nFlags & USE_DEPTH)
			{
				//m_pDeviceContext->OMSetDepthStencilState(
				//	m_pTestDepthState->GetState(), 0);
			}
			else
			{
				//m_pDeviceContext->OMSetDepthStencilState(
				//	m_pNoTestDepthState->GetState(), 0);
			}

			if (pBatch->nFlags & USE_BLEND)
			{
			//	glEnable(GL_BLEND);
                pRenderStateOp->EnableBlend(true);
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				pRenderStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_SRC_ALPHA,IRenderStateOp::BLEND_COLOR_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				//glDisable(GL_BLEND);
				pRenderStateOp->EnableBlend(false);
				//m_pDeviceContext->OMSetBlendState(
				//	m_pNoBlendState->GetState(), NULL, 0xFFFFFFFF);
			}

			prev_flags = pBatch->nFlags;
			prev_blend_alpha = pBatch->nBlendAlpha;
			prev_depth = pBatch->fDepthZ;
		}

		if (pBatch->nBlendMode >= 0)
		{
			// 画叠加混合效果的界面图片
			switch (pBatch->nBlendMode)
			{
			case 2:
				//glEnable(GL_BLEND);
				pRenderStateOp->EnableBlend(true);
				//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
				pRenderStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_SRC_ALPHA, IRenderStateOp::BLEND_COLOR_DST_ALPHA);
				break;
			case 1:
			case 3:
				//glEnable(GL_BLEND);
				pRenderStateOp->EnableBlend(true);
				//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
				pRenderStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_SRC_ALPHA, IRenderStateOp::BLEND_COLOR_DST_ALPHA);
				break;
			default:
				break;
			}
		}

		if (pBatch->pVertexShader != prev_vs)
		{
			prev_vs = pBatch->pVertexShader;
		}

		if (pBatch->pPixelShader != prev_ps)
		{
			prev_ps = pBatch->pPixelShader;
		}
		
		if (pBatch->nPrimitiveType == GL_POINTS)
		{
			enum {POS, POINT_SIZE, COLOR};
			static const char* element_name[] = {
				"iPos", "iPSize", "iDiffuse"
			};

            GuiShaderData shaderData = GuiShaderManager::Inst().GetShader(
                m_pRender, pBatch->pVertexShader, pBatch->pPixelShader, element_name, 3);
			IShaderProgram* pShaderProgram = shaderData.pShaderProgram;
			pShaderProgram->UsedShader();

			IRenderDrawOp* pRenderDrawOp = pShaderProgram->GetRenderDrawOp();

			int offset = 0;
			//VertexAttribPointer(POS, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_point_t), (fm_void*)offset);
			pRenderDrawOp->EnableVertexAttribArray(POS,true);
			pRenderDrawOp->SetVertexAttribPointer(POS, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(vertex_point_t), (fm_void*)offset);
			offset += sizeof(float)*4;
			//VertexAttribPointer(POINT_SIZE, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_point_t), (fm_void*)offset);
			pRenderDrawOp->EnableVertexAttribArray(POINT_SIZE ,true);
			pRenderDrawOp->SetVertexAttribPointer(POINT_SIZE,1, IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(vertex_point_t), (fm_void*)offset);
			offset += sizeof(float);
			//VertexAttribPointer(COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(vertex_point_t), (fm_void*)offset);
			pRenderDrawOp->EnableVertexAttribArray(COLOR ,true);
			pRenderDrawOp->SetVertexAttribPointer(COLOR , 4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE,sizeof(vertex_point_t), (fm_void*)offset);

			offset += sizeof(float);
			SetConstValue(shaderData, pBatch);
			pShaderProgram->GetRenderDrawOp()->DrawArrays(pBatch->nPrimitiveType, pBatch->nStartVertex, pBatch->nVertexCount);						
		}
		else if (pBatch->nFlags & USE_TEXTURE)
		{
			enum {POS, COLOR, UV};
			static const char* element_name[] = {
				"iPos", "iDiffuse", "iTexCoord"
			};

            GuiShaderData shaderData = GuiShaderManager::Inst().GetShader(
                m_pRender, pBatch->pVertexShader, pBatch->pPixelShader, element_name, 3);
            IShaderProgram* pShaderProgram = shaderData.pShaderProgram;
			
            IShaderParamOp* pShaderParamOp = pShaderProgram->GetParamOp();
			IRenderDrawOp* pRenderDrawOp = pShaderProgram->GetRenderDrawOp();
			pShaderProgram->UsedShader();

			SetConstValue(shaderData, pBatch);
			vertex_pict_t* pict_t = (vertex_pict_t*)&m_VertexPict[0];

			pShaderParamOp->SetTexture2D(shaderData.hTexBaseMap, pBatch->pTexture->GetTexture());
			pShaderProgram->GetRenderDrawOp()->SetVB( m_pPictVB->GetBuffer() );

			int offset = 0;

			pRenderDrawOp->EnableVertexAttribArray(POS,true);
			pRenderDrawOp->EnableVertexAttribArray(COLOR,true);
			pRenderDrawOp->EnableVertexAttribArray(UV,true);

#ifdef _COMPRESSED_VERTEX
			//VertexAttribPointer(POS, 4, GL_HALF_FLOAT_OES, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(POS, 4,IRenderDrawOp::VERTEX_DATA_HALF_FLOAT,sizeof(vertex_pict_t), (fm_void*)offset);
			offset += sizeof(short)*4;
			//VertexAttribPointer(COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(COLOR, 4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE, sizeof(vertex_pict_t), (fm_void*)offset);
			offset += sizeof(long);
			//VertexAttribPointer(UV, 2, GL_HALF_FLOAT_OES, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(UV, 2,IRenderDrawOp::VERTEX_DATA_HALF_FLOAT, sizeof(vertex_pict_t), (fm_void*)offset);
#else
			//VertexAttribPointer(POS, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(POS, 4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(vertex_pict_t), (fm_void*)offset);
			offset += sizeof(float)*4;
			//VertexAttribPointer(COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(COLOR, 4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE, sizeof(vertex_pict_t), (fm_void*)offset);
			offset += sizeof(long);
			//VertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_pict_t), (fm_void*)offset);
			pRenderDrawOp->SetVertexAttribPointer(UV, 2,IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(vertex_pict_t), (fm_void*)offset);
#endif
			pShaderProgram->GetRenderDrawOp()->DrawArrays(pBatch->nPrimitiveType, pBatch->nStartVertex, pBatch->nVertexCount);	
			pShaderProgram->GetRenderDrawOp()->SetVB( 0 );

			pRenderDrawOp->EnableVertexAttribArray(POS,false);
			pRenderDrawOp->EnableVertexAttribArray(COLOR,false);
			pRenderDrawOp->EnableVertexAttribArray(UV,false);
		}
		else
        {
            enum {POS, COLOR};
			static const char* element_name[] = {
				"iPos", "iDiffuse"
			};

            GuiShaderData shaderData = GuiShaderManager::Inst().GetShader(
                m_pRender, pBatch->pVertexShader, pBatch->pPixelShader, element_name, 2);
            IShaderProgram* pShaderProgram = shaderData.pShaderProgram;

            IRenderDrawOp* pRenderDrawOp = pShaderProgram->GetRenderDrawOp();
            pShaderProgram->UsedShader();

			pRenderDrawOp->SetVB( m_pLineVB->GetBuffer() );

			int offset = 0;
			pRenderDrawOp->EnableVertexAttribArray(POS,true);
			pRenderDrawOp->SetVertexAttribPointer(POS,4,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(vertex_line_t), (fm_void*)offset);
			offset += sizeof(float)*4;
			pRenderDrawOp->EnableVertexAttribArray(COLOR,true);
			pRenderDrawOp->SetVertexAttribPointer(COLOR,4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE,sizeof(vertex_line_t), (fm_void*)offset);

			SetConstValue(shaderData, pBatch);
			pShaderProgram->GetRenderDrawOp()->DrawArrays(pBatch->nPrimitiveType, pBatch->nStartVertex, pBatch->nVertexCount);
			pShaderProgram->GetRenderDrawOp()->SetVB( 0 );
		}
	}

	// 恢复视口
	pRenderStateOp->SetViewPort(0, 0, device_width, device_height);
	pRenderStateOp->SetScissorTest(0, 0, device_width, device_height);
	pRenderStateOp->EnableScissorTest(false);
	ClearData();
	pRenderStateOp->EnableBlend(false);
	pRenderStateOp->EnableDepthTest(true);
    pRenderStateOp->EnableCullFace(true);
	pRenderStateOp->EnableDepthWrite(true);
	m_pRender->UsedFrameRT(pOldFrame);
	return true;
}

bool CPaintBatchs::SetViewport(int x1, int y1, int x2, int y2, 
	bool is_in_parent)
{
	int left = x1;
	int top = y1;
	int width = x2 - x1;
	int height = y2 - y1;
	const viewport_t& vp = m_CurViewport;

	
	if (is_in_parent)
	{
		// left,top必须按照有符号数进行比较，因为可能为负值
		// 是否完全不在父窗口的范围内
		if ((left >= (vp.nLeft + vp.nWidth))
			|| (top >= (vp.nTop + vp.nHeight))
			|| ((left + width) < vp.nLeft) 
			|| ((top + height) < vp.nTop))
		{
			// 不需要渲染此窗口
			return false;
		}

		// 剪裁掉不在父窗口的部分
		if (left < vp.nLeft)
		{
			width -= vp.nLeft - left;
			left = vp.nLeft;
		}

		if ((left + width) > (vp.nLeft + vp.nWidth))
		{
			width = (vp.nLeft + vp.nWidth) - left;
		}

		if (top < vp.nTop)
		{
			height -= vp.nTop - top;
			top = vp.nTop;
		}

		if ((top + height) > (vp.nTop + vp.nHeight))
		{
			height = (vp.nTop + vp.nHeight) - top;
		}

		if ((width == 0) || (height == 0))
		{
			return false;
		}
	}

	// 保存父视窗设置
	m_ViewportStack.push_back(vp);
	m_CurViewport.nLeft = left;
	m_CurViewport.nTop = top;
	m_CurViewport.nWidth = width;
	m_CurViewport.nHeight = height;

	return true;
}

bool CPaintBatchs::ResetViewport()
{
	if (m_ViewportStack.size() == 0)
	{
		return false;
	}

	m_CurViewport = m_ViewportStack.back();
	m_ViewportStack.pop_back();

	return true;
}

void CPaintBatchs::ClearRenderResource()
{
	SAFE_RELEASE(m_pLineVB);
	SAFE_RELEASE(m_pPictVB);
	SAFE_RELEASE(m_pPointVB);
	m_nLineVBSize = 0;
	m_nPictVBSize = 0;
	m_nPointVBSize = 0;
}

