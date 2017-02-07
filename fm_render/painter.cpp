//--------------------------------------------------------------------
// 文件名:		painter.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "painter.h"
#include "render.h"
#include "helper.h"
#include "font.h"
#include "font_manager.h"
#include "dynamic_tex.h"
#include "tex_font.h"
#include "texture.h"
#include "../visual/i_font.h"
#include "../visual/i_texture.h"
#include "../visual/i_vis_base.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/var_list.h"
#include "../visual/i_resource.h"
#include "../visual/i_scene.h"

DECLARE_FUNCTION_INTERFACE(Painter);




#define INIT_VERTEX_LINE(v, x1, y1, color1) \
	{ v->x = float(x1); v->y = float(y1); v->z = m_fDepthZ; v->w = m_fDepthW; \
	v->color = color1; } 

#ifdef _COMPRESSED_VERTEX
	#define INIT_VERTEX_PICT(v, x1, y1, u1, v1) \
	{ v->x = Vis_ConvertFloatToHalfFloat((float)x1); v->y = Vis_ConvertFloatToHalfFloat((float)y1); v->z = Vis_ConvertFloatToHalfFloat(m_fDepthZ); v->w = Vis_ConvertFloatToHalfFloat(m_fDepthW); \
	v->color = 0xFFFFFFFF; v->tu = Vis_ConvertFloatToHalfFloat((float)u1); v->tv = Vis_ConvertFloatToHalfFloat((float)v1); } 
	#define INIT_VERTEX_ADD_PICT(v, x1, y1, color, u1, v1) \
		{ v->x = Vis_ConvertFloatToHalfFloat(float(x1)); v->y = Vis_ConvertFloatToHalfFloat(float(y1)); v->z = Vis_ConvertFloatToHalfFloat(m_fDepthZ); v->w = Vis_ConvertFloatToHalfFloat(m_fDepthW); \
		v->color = color; v->tu = Vis_ConvertFloatToHalfFloat(u1); v->tv = Vis_ConvertFloatToHalfFloat(v1); } 
	#define INIT_VERTEX_BW_PICT(v, x1, y1, color1, u1, v1) \
		{ v->x = Vis_ConvertFloatToHalfFloat(float(x1)); v->y = Vis_ConvertFloatToHalfFloat(float(y1)); v->z = Vis_ConvertFloatToHalfFloat(m_fDepthZ); v->w = Vis_ConvertFloatToHalfFloat(m_fDepthW); \
		v->color = color1; v->tu = Vis_ConvertFloatToHalfFloat(u1); v->tv = Vis_ConvertFloatToHalfFloat(v1); };
#else
	#define INIT_VERTEX_PICT(v, x1, y1, u1, v1) \
		{ v->x = float(x1); v->y = float(y1); v->z = m_fDepthZ; v->w = m_fDepthW; \
		v->color = 0xFFFFFFFF; v->tu = u1; v->tv = v1; } 
	#define INIT_VERTEX_ADD_PICT(v, x1, y1, color, u1, v1) \
		{ v->x = float(x1); v->y = float(y1); v->z = m_fDepthZ; v->w = m_fDepthW; \
		v->color = color; v->tu = u1; v->tv = v1; } 
	#define INIT_VERTEX_BW_PICT(v, x1, y1, color1, u1, v1) \
		{ v->x = float(x1); v->y = float(y1); v->z = m_fDepthZ; v->w = m_fDepthW; \
		v->color = color1; v->tu = u1; v->tv = v1; };
#endif

static inline float get_tex_uv(int value, int maximum)
{
	return (float(value)) / float(maximum);
}

static inline unsigned int color_get_alpha(unsigned int color)
{
	return (color >> 24) & 0xFF;
}

// Painter

Painter::Painter()
{
	m_pRender = NULL;
	m_pPaintBatchs = NULL;
	m_pCurFont = NULL;
	m_pDefFont = NULL;
	m_pRotateMatrix = NULL;
	m_nBlendAlpha = 255;
	m_fDepthZ = 0.0F;
	m_fDepthW = 1.0F;
	m_bScaleEnable = false;
	m_fScaleRatio = 1.1F;
	m_nRenderWidth = 0;
	m_nRenderHeight = 0;
	m_nCursorHotSpotX = 0;
	m_nCursorHotSpotY = 0;
    m_pFontManager = NULL;
	m_paintmode = 0;
}

Painter::~Painter()
{
	font_container_t::iterator it = m_Fonts.Begin();

	for (; it != m_Fonts.End(); ++it)
	{
		IFont* pFont = it.GetData();

		pFont->Release();
	}

	SAFE_RELEASE(m_pFontManager);
	//SAFE_RELEASE(m_pPaintBatchs);
}

bool Painter::Init()
{
	m_pRender = (Render*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		CORE_TRACE("(Painter::Init)no render");
		return false;
	}

	m_pFontManager = CFontManager::NewInstance(m_pRender);
	m_pPaintBatchs = m_pRender->GetPaintBatchs();
	//m_pPaintBatchs = CPaintBatchs::NewInstance(m_pRender);
	m_pPaintBatchs->Initialize();
	return true;
}

bool Painter::Shut()
{
    font_container_t::iterator it = m_Fonts.Begin();

    for (; it != m_Fonts.End(); ++it)
    {
        IFont* pFont = it.GetData();

        pFont->Shutdown();
    }

	SAFE_RELEASE(m_pRender);

	return true;
}

bool Painter::UpdateWindowSize()
{
	return m_pRender->UpdateWindowSize();
}

bool Painter::BeginPaint()
{
	//m_pPaintBatchs = m_pRender->GetPaintBatchs();
	m_pPaintBatchs->SetScaleEnable(m_bScaleEnable);

	return true;
}

bool Painter::EndPaint()
{
//	m_pPaintBatchs->RenderFlat();
	return true;
}

int Painter::GetRenderWidth()
{
	if (m_bScaleEnable)
	{
		return m_nRenderWidth;
	}
	else
	{
		return m_pRender->GetDeviceWidth();
	}
}

int Painter::GetRenderHeight()
{
	if (m_bScaleEnable)
	{
		return m_nRenderHeight;
	}
	else
	{
		return m_pRender->GetDeviceHeight();
	}
}

int Painter::GetDeviceWidth()
{
	return m_pRender->GetDeviceWidth();
}

int Painter::GetDeviceHeight()
{
	return m_pRender->GetDeviceHeight();
}

int Painter::GetWinWidth()
{
	return m_pRender->GetWinWidth();
}

int Painter::GetWinHeight()
{
	return m_pRender->GetWinHeight();
}

bool Painter::GetNeedScaleX()
{
	if (m_bScaleEnable)
	{
		return m_nRenderWidth != m_pRender->GetWinWidth();
	}
	else
	{
		return m_pRender->GetNeedScaleX();
	}
}

bool Painter::GetNeedScaleY()
{
	if (m_bScaleEnable)
	{
		return m_nRenderHeight != m_pRender->GetWinHeight();
	}
	else
	{
		return m_pRender->GetNeedScaleY();
	}
}

float Painter::GetScaleFactorX()
{
	if (m_bScaleEnable)
	{
		return float(m_nRenderWidth) / float(m_pRender->GetWinWidth());
	}
	else
	{
		return m_pRender->GetScaleFactorX();
	}
}

float Painter::GetScaleFactorY()
{
	if (m_bScaleEnable)
	{
		return float(m_nRenderHeight) / float(m_pRender->GetWinHeight());
	}
	else
	{
		return m_pRender->GetScaleFactorY();
	}
}

void Painter::SetDefaultPath(const char* value)
{
	Assert(value != NULL);

	m_strDefaultPath = value;
}

const char* Painter::GetDefaultPath()
{
	if (m_strDefaultPath.empty())
	{
		return GetCore()->GetResourcePath();
	}
	else
	{
		return m_strDefaultPath.c_str();
	}
}

inline int Painter::ScaledValue(int value)
{
	Assert(m_bScaleEnable);

	return int(value * m_fScaleRatio+0.5);
}

bool Painter::SetViewport(int x1, int y1, int x2, int y2, bool is_in_parent)
{
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return m_pPaintBatchs->SetViewport(x1, y1, x2, y2, is_in_parent);
}

bool Painter::ResetViewport()
{
	return m_pPaintBatchs->ResetViewport();
}

void Painter::SetRotateTransform(const FmMat4* mat)
{
	if (mat)
	{
		m_mtxRotate = *mat;
		m_pRotateMatrix = &m_mtxRotate;
	}
	else
	{
		m_pRotateMatrix = NULL;
	}
}

bool Painter::DrawScene(IScene* pScene)
{
	if (NULL == pScene)
	{
		return false;
	}

	pScene->Realize();

	m_pPaintBatchs->AddBatch(0.0F, 1.0F, false, false, false, false, 0, -1, 
		(IRenderDrawOp::DRAW_MODE)-1, 0, NULL, pScene, NULL);
	return true;
}

CPaintBatchs::vertex_pict_t* Painter::NewPictBatch(
	IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, IShaderTex* pTexture)
{
	Assert((primitive_type == IRenderDrawOp::DRAW_TRIANGLES)
		|| (primitive_type == IRenderDrawOp::DRAW_TRIANGLE_STRIP));
	
	return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, true, true, false, false, m_nBlendAlpha, -1, primitive_type, 
		primitive_count, pTexture, NULL, m_pRotateMatrix);
}

CPaintBatchs::vertex_pict_t* Painter::NewPictA8Batch(
	IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, IShaderTex* pTexture)
{
	Assert(primitive_type == IRenderDrawOp::DRAW_TRIANGLES);
	
	return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, true, true, false, true, m_nBlendAlpha, -1, primitive_type, 
		primitive_count, pTexture, NULL, m_pRotateMatrix);
}

CPaintBatchs::vertex_pict_t* Painter::NewPictOutlineBatch(IRenderDrawOp::DRAW_MODE primitive_type, 
	int primitive_count, IShaderTex* pTexture, const FmVec3* pOutColor, bool use_big_texture, bool fadeing)
{
	Assert(primitive_type == IRenderDrawOp::DRAW_TRIANGLES);
	if(use_big_texture)
		return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, true, true, false, false, m_nBlendAlpha, -1, primitive_type, 
		primitive_count, pTexture, NULL, m_pRotateMatrix, pOutColor, fadeing);
	else
		return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
			m_fDepthW, true, true, false, true, m_nBlendAlpha, -1, primitive_type, 
			primitive_count, pTexture, NULL, m_pRotateMatrix, pOutColor);
}

CPaintBatchs::vertex_pict_t* Painter::NewBWPictBatch(
	IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, IShaderTex* pTexture)
{
	Assert(primitive_type == IRenderDrawOp::DRAW_TRIANGLES);

	return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, true, true, true, false, m_nBlendAlpha, -1, primitive_type, 
		primitive_count, pTexture, NULL, m_pRotateMatrix);
}

CPaintBatchs::vertex_pict_t* Painter::NewAddPictBatch(
	IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, IShaderTex* pTexture,
	int blend_mode)
{
	Assert(primitive_type == IRenderDrawOp::DRAW_TRIANGLES); 

	return (CPaintBatchs::vertex_pict_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, true, true, false, false, m_nBlendAlpha, blend_mode, 
		primitive_type, primitive_count, pTexture, NULL, m_pRotateMatrix);
}

CPaintBatchs::vertex_line_t* Painter::NewLineBatch(
	IRenderDrawOp::DRAW_MODE primitive_type, int primitive_count, unsigned int alpha)
{
	return (CPaintBatchs::vertex_line_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, alpha < 255, false, false, false, m_nBlendAlpha, -1, 
		primitive_type, primitive_count, NULL, NULL, m_pRotateMatrix);
}

CPaintBatchs::vertex_point_t* Painter::NewPointBatch(
	int primitive_count, unsigned int alpha)
{
	return (CPaintBatchs::vertex_point_t*)m_pPaintBatchs->AddBatch(m_fDepthZ,
		m_fDepthW, alpha < 255, false, false, false, m_nBlendAlpha, -1, 
		IRenderDrawOp::DRAW_POINTS, primitive_count, NULL, NULL, m_pRotateMatrix);
}

inline void Painter::DrawVertexPict(CPaintBatchs::vertex_pict_t*& pv, 
	int x1, int y1, int x2, int y2, float u1, float v1, float u2, float v2)
{
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y1), u1, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y1), u2, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y2), u1, v2);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y1), u2, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y2), u2, v2);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y2), u1, v2);
	pv++;
}

bool Painter::DrawPict(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height, 
	int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}


	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);

	if(m_paintmode & PAINT_VERTICAL_FLIP)
	{
		v1 = get_tex_uv(bottom, tex_height);
		v2 = get_tex_uv(top, tex_height);
	}

	if(m_paintmode & PAINT_HORIZONTAL_FLIP)
	{
		u1 = get_tex_uv(right, tex_width);
		u2 = get_tex_uv(left, tex_width);
	}

	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		2, pTex);
	
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y1), u1, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y1), u2, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y2), u1, v2);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y1), u2, v1);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x1), SETTEXY(y2), u1, v2);
	pv++;
	INIT_VERTEX_PICT(pv, SETTEXX(x2), SETTEXY(y2), u2, v2);
	pv++;
	
	return true;
}

bool Painter::DrawBWPict(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height, 
	unsigned int blendcolor)
{
	float u1 = get_tex_uv(left, tex_width);
    float v1 = get_tex_uv(top, tex_height);
    float u2 = get_tex_uv(right, tex_width);
    float v2 = get_tex_uv(bottom, tex_height);

	CPaintBatchs::vertex_pict_t* pv = NewBWPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 2, pTex);

    INIT_VERTEX_BW_PICT(pv, SETTEXX(x1), SETTEXY(y1), blendcolor, u1, v1);
    pv++;
    INIT_VERTEX_BW_PICT(pv, SETTEXX(x2), SETTEXY(y1), blendcolor, u2, v1);
    pv++;
    INIT_VERTEX_BW_PICT(pv, SETTEXX(x1), SETTEXY(y2), blendcolor, u1, v2);
    pv++;
	INIT_VERTEX_BW_PICT(pv, SETTEXX(x2), SETTEXY(y1), blendcolor, u2, v1);
	pv++;
	INIT_VERTEX_BW_PICT(pv, SETTEXX(x1), SETTEXY(y2), blendcolor, u1, v2);
	pv++;
    INIT_VERTEX_BW_PICT(pv, SETTEXX(x2), SETTEXY(y2), blendcolor, u2, v2);
    pv++;

    return true;
}

bool Painter::DrawBWPictRound(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height, 
	int fan_num, unsigned int blendcolor)
{
	Assert(fan_num > 0);
	
	if (NULL == pTex)
	{
		return false;
	}

	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);
	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;
	float cu = float(u1 + u2) * 0.5F;
	float cv = float(v1 + v2) * 0.5F;
	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	// 扇面角度
	float fan_angle = FLOAT_PI * 2.0F / float(fan_num);
	TAutoMem<CPaintBatchs::vertex_pict_t, 32, TCoreAlloc> auto_buf(fan_num + 1);
	CPaintBatchs::vertex_pict_t* points = auto_buf.GetBuffer();
	
	for (int i = 0; i <= fan_num; ++i)
	{
		float theta = fan_angle * i;
		
		points[i].x = cx + rx * cosf(theta);
		points[i].y = cy + ry * sinf(theta);
		points[i].tu = u1 + (points[i].x - x1) / (x2 - x1) * (u2 - u1);
		points[i].tv = v1 + (points[i].y - y1) / (y2 - y1) * (v2 - v1);
	}

	CPaintBatchs::vertex_pict_t* pv = NewBWPictBatch(IRenderDrawOp::DRAW_TRIANGLES, fan_num, 
		pTex);
	
	for (int k = 0; k < fan_num; ++k)
	{
		CPaintBatchs::vertex_pict_t& v1 = points[k];
		CPaintBatchs::vertex_pict_t& v2 = points[k + 1];
		
		INIT_VERTEX_BW_PICT(pv, SETTEXX(cx), SETTEXY(cy),  
			blendcolor, cu, cv);	
		pv++;
		INIT_VERTEX_BW_PICT(pv, SETTEXX(v1.x), SETTEXY(v1.y),  
			blendcolor, v1.tu, v1.tv);	
		pv++;
		INIT_VERTEX_BW_PICT(pv, SETTEXX(v2.x), SETTEXY(v2.y),  
			blendcolor, v2.tu, v2.tv);	
		pv++;
	}

	return true;
}

bool Painter::DrawAddPict(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height, 
	int blend_mode, unsigned int blendcolor)
{
	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);
	
	CPaintBatchs::vertex_pict_t* pv = NewAddPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 2, pTex, 
		blend_mode);
    unsigned int color = blendcolor;

	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x1), SETTEXY(y1), color, u1, v1);
	pv++;
	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x2), SETTEXY(y1), color, u2, v1);
	pv++;
	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x1), SETTEXY(y2), color, u1, v2);
	pv++;
	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x2), SETTEXY(y1), color, u2, v1);
	pv++;
	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x1), SETTEXY(y2), color, u1, v2);
	pv++;
	INIT_VERTEX_ADD_PICT(pv, SETTEXX(x2), SETTEXY(y2), color, u2, v2);
	pv++;

	return true;
}

bool Painter::DrawPictRound(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, 
	int tex_height, int fan_num)
{
	Assert(fan_num > 0);
	
	if (NULL == pTex)
	{
		return false;
	}

	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);
	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;
	float cu = float(u1 + u2) * 0.5F;
	float cv = float(v1 + v2) * 0.5F;
	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	// 扇面角度
	float fan_angle = FLOAT_PI * 2.0F / float(fan_num);
	TAutoMem<CPaintBatchs::vertex_pict_t, 32, TCoreAlloc> auto_buf(fan_num + 1);
	CPaintBatchs::vertex_pict_t* points = auto_buf.GetBuffer();
	
	for (int i = 0; i <= fan_num; ++i)
	{
		float theta = fan_angle * i;
		
		points[i].x = cx + rx * cosf(theta);
		points[i].y = cy + ry * sinf(theta);
		points[i].tu = u1 + (points[i].x - x1) / (x2 - x1) * (u2 - u1);
		points[i].tv = v1 + (points[i].y - y1) / (y2 - y1) * (v2 - v1);
	}

	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, fan_num, 
		pTex);
	
	for (int k = 0; k < fan_num; ++k)
	{
		CPaintBatchs::vertex_pict_t& v1 = points[k];
		CPaintBatchs::vertex_pict_t& v2 = points[k + 1];
		
		INIT_VERTEX_PICT(pv, SETTEXX(cx), SETTEXY(cy), cu, cv);	
		pv++;
		INIT_VERTEX_PICT(pv, SETTEXX(v1.x), SETTEXY(v1.y), v1.tu, v1.tv);	
		pv++;
		INIT_VERTEX_PICT(pv, SETTEXX(v2.x), SETTEXY(v2.y), v2.tu, v2.tv);	
		pv++;
	}

	return true;
}

bool Painter::DrawPictExpandTile(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, 
	int tex_height, int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}
	
	// 窗口分块尺寸
	int w = (x2 - x1) / 3;
	int h = (y2 - y1) / 3;
	// 贴图分块尺寸
	int tw = (right - left) / 3; 
	int th = (bottom - top) / 3;

	if (tw > w)
	{
		tw = w;
	}

	if (th > h)
	{
		th = h;
	}

	if ((tw == 0) || (th == 0))
	{
		return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			tex_width, tex_height, orign_width, orign_height);
	}
	
	// 边的尺寸
	int edge_w = (x2 - x1) - tw * 2;
	int edge_h = (y2 - y1) - th * 2;
	// 边的图块数量
	int num_x = (edge_w + tw - 1) / tw;
	int num_y = (edge_h + th - 1) / th;
	// 总的分块数量
	int block_num = 4 + num_x * 2 + num_y * 2 + num_x * num_y;
	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		block_num * 2, pTex);
	
	float u1 = get_tex_uv(left, tex_width);
	float u2 = get_tex_uv(right, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float v2 = get_tex_uv(bottom, tex_height);
	float u3 = get_tex_uv(left + tw, tex_width);
	float u4 = get_tex_uv(right - tw, tex_width);
	float v3 = get_tex_uv(top + th, tex_height);
	float v4 = get_tex_uv(bottom - th, tex_height);

	// 画四个角
	// 左上
	DrawVertexPict(pv, x1, y1, x1 + tw, y1 + th, u1, v1, u3, v3);

	// 左下
	DrawVertexPict(pv, x1, y2 - th, x1 + tw, y2, u1, v4, u3, v2);

	// 右上
	DrawVertexPict(pv, x2 - tw, y1, x2, y1 + th, u4, v1, u2, v3);
	// 右下
	DrawVertexPict(pv, x2 - tw, y2 - th, x2, y2, u4, v4, u2, v2);
	
	// 画四条边
	int count = 0;
	int i;
	
	for (i = 0; i < num_x; ++i)
	{
		int size = tw;
		float u5 = u4;
		
		if ((count + size) > edge_w)
		{
			size = edge_w - count;
			u5 = get_tex_uv(left + tw + size, tex_width);
		}

		// 上边
		DrawVertexPict(pv, 	x1 + tw + count, y1, 
			x1 + tw + count + size, y1 + th, u3, v1, u5, v3);
		// 下边
		DrawVertexPict(pv, x1 + tw + count, y2 - th, 
			x1 + tw + count + size, y2, u3, v4, u5, v2);
		count += size;
	}
	
	count = 0;
	
	for (i = 0; i < num_y; ++i)
	{
		int size = th;
		float v5 = v4;
		
		if ((count + size) > edge_h)
		{
			size = edge_h - count;
			v5 = get_tex_uv(top + th + size, tex_height);
		}
		
		// 左边
		DrawVertexPict(pv, x1, y1 + th + count, 
			x1 + tw, y1 + th + count + size, u1, v3, u3, v5);
		// 右边
		DrawVertexPict(pv, x2 - tw, y1 + th + count, 
			x2, y1 + th + count + size, u4, v3, u2, v5);
		count += size;
	}
	
    int countw = 0;

    for (i = 0; i < num_x; ++i)
    {
        int sizew = tw;
        float u5 = u4;

        if ((countw + sizew) > edge_w)
        {
            sizew = edge_w - countw;
            u5 = get_tex_uv(left + tw + sizew, tex_width);
        }

        int counth = 0;

        for (int j = 0; j < num_y; ++j)
        {
            int sizeh = th;
            float v5 = v4;

            if ((counth + sizeh) > edge_h)
            {
                sizeh = edge_h - counth;
                v5 = get_tex_uv(top + th + sizeh, tex_height);
            }

            // 画中心部分,不拉伸
            DrawVertexPict(pv, x1 + tw + countw, y1 + th + counth, 
                x1 + tw + countw + sizew, y1 + th + counth + sizeh, 
                u3, v3, u5, v5);
            counth += sizeh;
        }

        countw += sizew;
    }

	return true;
}

bool Painter::DrawPictExpand(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, 
	int tex_height, int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}
	
	// 窗口分块尺寸
	int w = (x2 - x1) / 3;
	int h = (y2 - y1) / 3;
	// 贴图分块尺寸
	int tw = (right - left) / 3; 
	int th = (bottom - top) / 3;

	if (tw > w)
	{
		tw = w;
	}

	if (th > h)
	{
		th = h;
	}

	if ((tw == 0) || (th == 0))
	{
		return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			tex_width, tex_height, orign_width, orign_height);
	}
	
	// 边的尺寸
	int edge_w = (x2 - x1) - tw * 2;
	int edge_h = (y2 - y1) - th * 2;
	// 边的图块数量
	//int num_x = (edge_w + tw - 1) / tw;
	//int num_y = (edge_h + th - 1) / th;
	// 总的分块数量
	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		9 * 2, pTex);
	
	float u1 = get_tex_uv(left, tex_width);
	float u2 = get_tex_uv(right, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float v2 = get_tex_uv(bottom, tex_height);
	float u3 = get_tex_uv(left + tw, tex_width);
	float u4 = get_tex_uv(right - tw, tex_width);
	float v3 = get_tex_uv(top + th, tex_height);
	float v4 = get_tex_uv(bottom - th, tex_height);

	// 画四个角
	// 左上
	DrawVertexPict(pv, x1, y1, x1 + tw, y1 + th, u1, v1, u3, v3);

	// 左下
	DrawVertexPict(pv, x1, y2 - th, x1 + tw, y2, u1, v4, u3, v2);

	// 右上
	DrawVertexPict(pv, x2 - tw, y1, x2, y1 + th, u4, v1, u2, v3);
	// 右下
	DrawVertexPict(pv, x2 - tw, y2 - th, x2, y2, u4, v4, u2, v2);
	

	//画四条边，使用拉伸的方式
	
	//水平边
	DrawVertexPict(pv, x1+tw, y1,		x2-tw, y1 + th, u3, v1, u4, v3);
	DrawVertexPict(pv, x1+tw, y2 - th,	x2-tw, y2,		u3, v4, u4, v2);

	//垂直边
	DrawVertexPict(pv, x1, y1+th,	x1+tw, y2-th,		u1, v3, u3, v4);
	DrawVertexPict(pv, x2-tw, y1+th,	x2, y2-th,		u4, v3, u2, v4);
	
	DrawVertexPict(pv, x1+tw, y1+th, x2-tw, y2-th, u3, v3, u4,v4);
	
	return true;
}


/*
bool Painter::DrawPictExpandH(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height,
	int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}

	// 窗口分块尺寸
	int w = (x2 - x1) / 3;
	// 贴图分块尺寸
	int tw = orign_width / 3; 

	if ((0 == w) || (0 == (right - left)))
	{
		return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			tex_width, tex_height, orign_width, orign_height);
	}

    // 全部绘制
    if (right - left == orign_width)
    {
        int draw_width = tw;

        if (draw_width > w)
        {
            draw_width = w;
        }

	    // 边的尺寸
	    int edge_w = (x2 - x1) - draw_width * 2;
	    // 边的图块数量
	    int num_x = (edge_w + draw_width - 1) / draw_width;

	    float u1 = get_tex_uv(left, tex_width);
	    float u2 = get_tex_uv(right, tex_width);
	    float v1 = get_tex_uv(top, tex_height);
	    float v2 = get_tex_uv(bottom, tex_height);

	    float u3 = get_tex_uv(left + tw, tex_width);
	    float u4 = get_tex_uv(right - tw, tex_width);

	    // 总的分块数量
	    int block_num = 2 + num_x;

	    CPaintBatchs::vertex_pict_t* pv = NewPictBatch(
			IRenderDrawOp::DRAW_TRIANGLES, block_num * 2, pTex);

	    // 左
	    DrawVertexPict(pv, x1, y1, x1 + draw_width, y2, u1, v1, u3, v2);

	    // 右
	    DrawVertexPict(pv, x2 - draw_width, y1, x2, y2, u4, v1, u2, v2);

	    // 画中间部分
	    int count = 0;

	    for (int i = 0; i < num_x; ++i)
	    {
		    int size = draw_width;

		    float u5 = u4;

		    if ((count + size) > edge_w)
		    {
			    size = edge_w - count;

			    u5 = get_tex_uv(left + draw_width + size, tex_width);
		    }

		    DrawVertexPict(pv, x1 + draw_width + count, y1, 
				x1 + draw_width + count + size, y2, u3, v1, u5, v2);

		    count += size;
	    }
    }
    // 局部绘制
    else
    {

	    // 实际条的宽度
        int dest_max_width = (x2 - x1) * orign_width / (right - left);

        if (dest_max_width <= orign_width)
        {
		    return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			    tex_width, tex_height, orign_width, orign_height);
        }

        // 如果宽度小于贴图宽度2/3，则直接绘制不等分
        if (x2 - x1 < orign_width * 2 / 3)
        {
            if (left == 0)
            {
		        return DrawPict(x1, y1, x2, y2, left, top, left + x2 - x1, 
					bottom, pTex, tex_width, tex_height, orign_width, orign_height);
            }
            else if (right == orign_width)
            {
		        return DrawPict(x1, y1, x2, y2, right - (x2 - x1), top, right, 
					bottom, pTex, tex_width, tex_height, orign_width, orign_height);
            }
        }

        // 从左开始
        if (left == 0)
        {
            // 由左往右画
	        // 不显示左图块
            bool show_right = false;
            int right_leave = tw - (dest_max_width - (x2 - x1));

            if (right_leave > 0)
            {
                show_right = true;
            }
            else
            {
                right_leave = 0;
            }

            // 不显示右图块
            int edge_w = (x2 - x1) - tw - right_leave;
    	    
            // 边的图块数量
            int num_x = (edge_w + tw - 1) / tw;

            float u1 = 0.0F;
            //float u2 = 1.0F;
            float v1 = 0.0F;
            float v2 = 1.0F;
            float u3 = get_tex_uv(tw, tex_width);
            float u4 = get_tex_uv(tex_width - tw, tex_width);

            // 总的分块数量
            int block_num = 1 + num_x;

            if (show_right)
            {
                block_num += 1;
            }

            CPaintBatchs::vertex_pict_t* pv = NewPictBatch(
				IRenderDrawOp::DRAW_TRIANGLES, block_num * 2, pTex);

            // 左
            DrawVertexPict(pv, x1, y1, x1 + tw, y2, u1, v1, u3, v2);

            if (show_right)
            {
                float u5 = get_tex_uv(
					right - ((int)dest_max_width - (x2 - x1)), tex_width);

                // 右
                DrawVertexPict(pv, x1 + (int)dest_max_width - tw, y1, x2, y2, 
					u4, v1, u5, v2);
            }

            // 画中间部分
            int count = 0;

            for (int i = 0; i < num_x; ++i)
            {
	            int size = tw;
	            float u5 = u4;

	            if ((count + size) > edge_w)
	            {
		            size = edge_w - count;
		            u5 = get_tex_uv(left + tw + size, tex_width);
	            }

	            DrawVertexPict(pv, x1 + tw + count, y1, 
					x1 + tw + count + size, y2, u3, v1, u5, v2);
	            count += size;
            }
        }
        // 从右开始
        else if (right == tex_width)
        {
            // 由右往左画
            // 实体条的宽度
            int dest_max_width = (x2 - x1) * tex_width / (right - left);
	        // 不显示左图块
            bool show_left = false;
            int left_leave = tw - (dest_max_width - (x2 - x1));

            if (left_leave > 0)
            {
                show_left = true;
            }
            else
            {
                left_leave = 0;
            }

            int edge_w = (x2 - x1) - tw - left_leave;
    	    
            // 边的图块数量
            int num_x = (edge_w + tw - 1) / tw;
            //float u1 = 0.0F;
            float u2 = 1.0F;
            float v1 = 0.0F;
            float v2 = 1.0F;
            float u3 = get_tex_uv(tw, tex_width);
            float u4 = get_tex_uv(tex_width - tw, tex_width);

            // 总的分块数量
            int block_num = 1 + num_x;

            if (show_left)
            {
                block_num += 1;
            }

            CPaintBatchs::vertex_pict_t* pv = NewPictBatch(
				IRenderDrawOp::DRAW_TRIANGLES, block_num * 2, pTex);

            if (show_left)
            {
                // 左
                float u5 = get_tex_uv(tw + tw - left_leave, tex_width);
                
				DrawVertexPict(pv, x1, y1, x1 + left_leave, y2, u5, v1, u3, v2);
            }

            // 右
            DrawVertexPict(pv, x2 - tw, y1, x2, y2, u4, v1, u2, v2);

            // 画中间部分
            int count = x2 - x1 - tw;

            if (show_left)
            {
                count -= left_leave;
            }

	        for (int i = 0; i < num_x; ++i)
            {
	            float u5 = u4;

	            if (count < tw)
	            {
		            u5 = get_tex_uv(count, tex_width);

                    int end = x2 - tw - i * tw;
                    int begin = end - count;

	                DrawVertexPict(pv, begin, y1, end , y2, 
		                u5, v1, u4, v2);
	            }
                else
                {
                    int begin = x2 - tw - (i + 1) * tw;

	                DrawVertexPict(pv, begin, y1, begin + tw, y2, 
		                u3, v1, u4, v2);
                }

	            count -= tw;
            }
        }
    }

	return true;
}
*/




bool Painter::DrawPictExpandH(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height,
	int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}

	// 窗口分块尺寸
	int w = (x2 - x1) / 3;
	
	// 贴图分块尺寸
	int tw = (right-left) / 3; 
	
	if(tw > w )
		tw = w;

	if ((0 == w) || (0 == (right - left)))
	{
		return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			tex_width, tex_height, orign_width, orign_height);
	}

	float u1 = get_tex_uv(left, tex_width);
	float u2 = get_tex_uv(right, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float v2 = get_tex_uv(bottom, tex_height);

	float u3 = get_tex_uv(left + tw, tex_width);
	float u4 = get_tex_uv(right - tw, tex_width);


	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(
			IRenderDrawOp::DRAW_TRIANGLES,  3*2, pTex);

	
	//左
	DrawVertexPict(pv, x1, y1, x1 + tw, y2, 
					u1, v1, u3, v2);



	//右
	DrawVertexPict(pv, x2-tw, y1, x2, y2, 
					u4, v1, u2, v2);



	//中间
	DrawVertexPict(pv, x1+tw, y1, x2-tw, y2, 
					u3, v1, u4, v2);

	return true;
}


bool Painter::DrawPictExpandV(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height,
	int orign_width, int orign_height)
{
	if (NULL == pTex)
	{
		return false;
	}

	// 窗口分块尺寸
	int h = (y2 - y1) / 3;
	// 贴图分块尺寸
	int th = (bottom - top) / 3;

	if (th > h)
	{
		th = h;
	}

	if (th == 0)
	{
		return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
			tex_width, tex_height, orign_width, orign_height);
	}

	// 边的尺寸
	int edge_h = (y2 - y1) - th * 2;

	// 边的图块数量
	int num_y = (edge_h + th - 1) / th;
	float u1 = get_tex_uv(left, tex_width);
	float u2 = get_tex_uv(right, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float v2 = get_tex_uv(bottom, tex_height);
	float v3 = get_tex_uv(top + th, tex_height);
	float v4 = get_tex_uv(bottom - th, tex_height);

	// 总的分块数量
	int block_num = 2 + num_y;
	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		block_num * 2, pTex);

	// 上
	DrawVertexPict(pv, x1, y1, x2, y1 + th, u1, v1, u2, v3);
	// 下
	DrawVertexPict(pv, x1, y2 - th, x2, y2, u1, v4, u2, v2);

	// 中间部分
	int count = 0;

	for (int i = 0; i < num_y; ++i)
	{
		int size = th;
		float v5 = v4;

		if ((count + size) > edge_h)
		{
			size = edge_h - count;
			v5 = get_tex_uv(top + th + size, tex_height);
		}

		DrawVertexPict(pv, x1, y1 + th + count, x2, y1 + th + count + size, 
			u1, v3, u2, v5);
		count += size;
	}

	return true;
}

bool Painter::DrawGlyph(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, const PERSISTID& glyph_id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}
	
	IShaderTex* pTex = pGlyph->GetShaderTex();

	if (NULL == pTex)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph->GetOriginWidth(), pGlyph->GetOriginHeight());
}

bool Painter::DrawBWGlyph(int x1, int y1, int x2, int y2, int left, int top, 
	int right, int bottom, const PERSISTID& glyph_id, unsigned int blendcolor)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

    if (NULL == pGlyph)
    {
        return false;
    }

	IShaderTex* pTex = pGlyph->GetShaderTex();

    if (NULL == pTex)
    {
        return false;
    }

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawBWPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), blendcolor);
}

bool Painter::DrawBWGlyphRound(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id, 
    int fan_num, unsigned int blendcolor)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);
	
	if (NULL == pGlyph)
	{
		return false;
	}
	
	IShaderTex* pTex = pGlyph->GetShaderTex();
	
	if (NULL == pTex)
	{
		return false;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawBWPictRound(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), fan_num, blendcolor);
}

// 画叠加混合效果的界面图片
bool Painter::DrawAddGlyph(int x1, int y1, int x2, int y2, int left, int top, 
    int right, int bottom, const PERSISTID& glyph_id, int blendmode,
    unsigned int blendcolor)
{
	if (((blendmode <= 0) || (blendmode > 4)) && blendcolor == 0xffffffff)
	{
		return DrawGlyph(x1, y1, x2, y2, left, top, right, bottom, glyph_id);
	}

	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	IShaderTex* pTex = pGlyph->GetShaderTex();

	if (NULL == pTex)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawAddPict(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), blendmode, blendcolor);
}

bool Painter::DrawGlyphRound(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id, int fan_num)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);
	
	if (NULL == pGlyph)
	{
		return false;
	}
	
	IShaderTex* pTex = pGlyph->GetShaderTex();
	
	if (NULL == pTex)
	{
		return false;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawPictRound(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), fan_num);
}

bool Painter::DrawGlyphFan(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id, int fan_num, 
	float begin_angle, float end_angle)
{
	Assert(fan_num > 0);

	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	int tex_width = pGlyph->GetWidth();
	int tex_height = pGlyph->GetHeight();
	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);
	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;
	float cu = float(u1 + u2) * 0.5F;
	float cv = float(v1 + v2) * 0.5F;
	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	// 扇面角度
	float fan_angle = FLOAT_PI * 2.0F / float(fan_num);

	// 保证end肯定比begin大, 顺时针处理
	if (end_angle - begin_angle > 0.001F)
	{
		// 顺时针
	}
	else if (end_angle - begin_angle < -0.001F)
	{
		// 逆时针
		fan_angle = -fan_angle;
	}
	else
	{
		// 角度太小不用绘制
		return true;
	}

	int real_fan_num = (int)((end_angle - begin_angle + fan_angle * 0.5F) / fan_angle);

	real_fan_num = abs(real_fan_num);

	if (real_fan_num == 0)
	{
		// 角度太小不用绘制
		return true;
	}

	TAutoMem<CPaintBatchs::vertex_pict_t, 32, TCoreAlloc> auto_buf(real_fan_num + 1);
	CPaintBatchs::vertex_pict_t* points = auto_buf.GetBuffer();

	for (int i = 0; i <= real_fan_num; ++i)
	{
		float theta = begin_angle + fan_angle * i;

		points[i].x = cx + rx * cosf(theta);
		points[i].y = cy + ry * sinf(theta);
		points[i].tu = u1 + (points[i].x - x1) / (x2 - x1) * (u2 - u1);
		points[i].tv = v1 + (points[i].y - y1) / (y2 - y1) * (v2 - v1);
	}

	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		real_fan_num, pGlyph->GetShaderTex());

	for (int k = 0; k < real_fan_num; ++k)
	{
		CPaintBatchs::vertex_pict_t& v1 = points[k];
		CPaintBatchs::vertex_pict_t& v2 = points[k + 1];

		INIT_VERTEX_PICT(pv, SETTEXX(cx), SETTEXY(cy), cu, cv);	
		pv++;
		INIT_VERTEX_PICT(pv, SETTEXX(v1.x), SETTEXY(v1.y), v1.tu, v1.tv);	
		pv++;
		INIT_VERTEX_PICT(pv, SETTEXX(v2.x), SETTEXY(v2.y), v2.tu, v2.tv);	
		pv++;
	}

	return true;
}

bool Painter::DrawGlyphExpand(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id) 
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);
	
	if (NULL == pGlyph)
	{
		return false;
	}
	
	IShaderTex* pTex = pGlyph->GetShaderTex();
	
	if (NULL == pTex)
	{
		return false;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawPictExpand(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph->GetOriginWidth(), pGlyph->GetOriginHeight());
}

bool Painter::DrawGlyphExpandH(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	IShaderTex* pTex = pGlyph->GetShaderTex();

	if (NULL == pTex)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawPictExpandH(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph->GetOriginWidth(), pGlyph->GetOriginHeight());
}

bool Painter::DrawGlyphExpandV(int x1, int y1, int x2, int y2, 
	int left, int top, int right, int bottom, const PERSISTID& glyph_id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	IShaderTex* pTex = pGlyph->GetShaderTex();

	if (NULL == pTex)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	return DrawPictExpandV(x1, y1, x2, y2, left, top, right, bottom, pTex, 
		pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph->GetOriginWidth(), pGlyph->GetOriginHeight());
}

static inline bool get_rotate_pos(FmVec2 vBasePos, FmVec2& vDestPos, 
	float rotate, float scale)
{
	FmVec2Scale(&vDestPos, &vBasePos, scale);

	FmVec3 vBase(vDestPos.x, vDestPos.y, 0.0F);
	FmVec3 vOut;
	FmMat4 mat_angle;

	FmMatrixRotationZ(&mat_angle, rotate);
	FmVec3TransformCoord(&vOut, &vBase, &mat_angle);
	vDestPos.x = vOut.x;
	vDestPos.y = vOut.y;

	return true;
}

bool Painter::DrawGlyphList(float* x, float* y, float* orient, 
	float* scale, float* alpha, int count, const PERSISTID& glyph_id, 
	int blend_mode, unsigned int blend_color)
{
	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	float u1 = 0.0F;
	float v1 = 0.0F;
	float u2 = 1.0F;
	float v2 = 1.0F;
	int image_width = pGlyph->GetWidth();
	int image_height = pGlyph->GetHeight();

	CPaintBatchs::vertex_pict_t* pv = NewAddPictBatch(
		IRenderDrawOp::DRAW_TRIANGLES, count * 2, pGlyph->GetShaderTex(), blend_mode);

	FmVec2 vBaseLeftTop = FmVec2(-image_width / 2.0F, 
		-image_height / 2.0F);
	FmVec2 vBaseRightTop = FmVec2(image_width / 2.0F, 
		-image_height / 2.0F);
	FmVec2 vBaseLeftBottom = FmVec2(-image_width / 2.0F, 
		image_height / 2.0F);
	FmVec2 vBaseRightBottom = FmVec2(image_width / 2.0F, 
		image_height / 2.0F);
	FmVec2 vLeftTop;
	FmVec2 vRightTop;
	FmVec2 vLeftBottom;
	FmVec2 vRightBottom;
	FmMat4 mat;

	int rgb = blend_color & 0x00FFFFFF;
	int blend_alpha = (blend_color & 0xFF000000) >> 24;

	for (int i = 0; i < count; ++i)
	{
		int color = ((int(blend_alpha * alpha[i])) << 24) | rgb;

		get_rotate_pos(vBaseLeftTop, vLeftTop, orient[i], scale[i]);
		vLeftTop.x += x[i];
		vLeftTop.y += y[i];

		get_rotate_pos(vBaseRightTop, vRightTop, orient[i], scale[i]);
		vRightTop.x += x[i];
		vRightTop.y += y[i];

		get_rotate_pos(vBaseLeftBottom, vLeftBottom, orient[i], scale[i]);
		vLeftBottom.x += x[i];
		vLeftBottom.y += y[i];

		get_rotate_pos(vBaseRightBottom, vRightBottom, orient[i], scale[i]);
		vRightBottom.x += x[i];
		vRightBottom.y += y[i];

		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vLeftTop.x), SETTEXY(vLeftTop.y),
			color, u1, v1);
		pv++;
		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vRightTop.x), SETTEXY(vRightTop.y),
			color, u2, v1);
		pv++;
		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vLeftBottom.x), SETTEXY(vLeftBottom.y),
			color, u1, v2);
		pv++;
		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vRightTop.x), SETTEXY(vRightTop.y),
			color, u2, v1);
		pv++;
		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vLeftBottom.x), SETTEXY(vLeftBottom.y),
			color, u1, v2);
		pv++;
		INIT_VERTEX_ADD_PICT(pv, SETTEXX(vRightBottom.x), SETTEXY(vRightBottom.y),    
			color, u2, v2);
		pv++;
	}
	
	return true;
}

bool Painter::DrawGlyphConvexPolygon(int x1, int y1, int x2, int y2, int left, 
	int top, int right, int bottom, const PERSISTID& glyph_id, float* point, 
	int* index, int count, int blend_mode, unsigned int blend_color)
{
	Assert(count > 2);

	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	IShaderTex* pTex = pGlyph->GetShaderTex();

	if (NULL == pTex)
	{
		return false;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	int tex_width = pGlyph->GetWidth();
	int tex_height = pGlyph->GetHeight();
	float u1 = get_tex_uv(left, tex_width);
	float v1 = get_tex_uv(top, tex_height);
	float u2 = get_tex_uv(right, tex_width);
	float v2 = get_tex_uv(bottom, tex_height);
	TAutoMem<CPaintBatchs::vertex_pict_t, 32, TCoreAlloc> auto_buf(count);
	CPaintBatchs::vertex_pict_t* points = auto_buf.GetBuffer();

	for (int i = 0; i < count; ++i)
	{
		if (m_bScaleEnable)
		{
			points[i].x = float(x1 + ScaledValue((int)point[i * 2]));
			points[i].y = float(y1 + ScaledValue((int)point[i * 2 + 1]));
		}
		else
		{
			points[i].x = float(x1) + point[i * 2];
			points[i].y = float(y1) + point[i * 2 + 1];
		}

		points[i].tu = u1 + (points[i].x - x1) / (x2 - x1) * (u2 - u1);
		points[i].tv = v1 + (points[i].y - y1) / (y2 - y1) * (v2 - v1);
	}

	CPaintBatchs::vertex_pict_t* pv = NewAddPictBatch(IRenderDrawOp::DRAW_TRIANGLES, count - 2, 
		pTex, blend_mode);
	unsigned int color = blend_color;
	int index_count = (count - 2) * 3;

	for (int k = 0; k < index_count; ++k)
	{
		CPaintBatchs::vertex_pict_t& v = points[index[k]];

		INIT_VERTEX_ADD_PICT(pv, SETTEXX(v.x), SETTEXY(v.y), color, 0.9375F, 
			0.5F);
		pv++;
	}

	return true;
}

bool Painter::DrawGlyphConvexPolygonBorder(int x1, int y1, int x2, int y2, 
	const PERSISTID& glyph_id, float points[], float normals[], int point_num)
{
	int line_num = point_num - 1;

	if (line_num <= 0)
	{
		return true;
	}

	CTexture* pGlyph = m_pRender->GetTextureById(glyph_id);

	if (NULL == pGlyph)
	{
		return false;
	}

	IShaderTex* pTex = pGlyph->GetShaderTex();
	CPaintBatchs::vertex_pict_t* pv = NewPictBatch(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 
		point_num * 2, pTex);

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);

		int point0_x = x1 + ScaledValue((int)points[line_num * 2]);
		int point0_y = y1 + ScaledValue((int)points[line_num * 2 + 1]);
		int point1_x = point0_x + ScaledValue((int)normals[line_num * 2]);
		int point1_y = point0_y + ScaledValue((int)normals[line_num * 2 + 1]);

		INIT_VERTEX_PICT(pv, SETX(point0_x), SETY(point0_y), 0, 0);
		pv++;
		INIT_VERTEX_PICT(pv, SETX(point1_x), SETY(point1_y), 0.9375F, 0);
		pv++;

		for (int i = 0; i < point_num; ++i)
		{
			point0_x = x1 + ScaledValue((int)points[i * 2]);
			point0_y = y1 + ScaledValue((int)points[i * 2 + 1]);
			point1_x = point0_x + ScaledValue((int)normals[i * 2]);
			point1_y = point0_y + ScaledValue((int)normals[i * 2 + 1]);
			
			INIT_VERTEX_PICT(pv, SETX(point0_x), SETY(point0_y), 0, 0);
			pv++;
			INIT_VERTEX_PICT(pv, SETX(point1_x), SETY(point1_y), 0.9375F, 0);
			pv++;
		}
	}
	else
	{
		int point_x = x1 + (int)points[line_num * 2];
		int point_y = y1 + (int)points[line_num * 2 + 1];
		int normal_x = (int)normals[line_num * 2];
		int normal_y = (int)normals[line_num * 2 + 1];

		INIT_VERTEX_PICT(pv, SETX(point_x + normal_x), 
			SETY(point_y + normal_y), 0, 0);
		pv++;
		INIT_VERTEX_PICT(pv, SETX(point_x), SETY(point_y), 0.9375F, 0);
		pv++;

		for (int i = 0; i < point_num; ++i)
		{
			point_x = x1 + (int)points[i * 2];
			point_y = y1 + (int)points[i * 2 + 1];
			normal_x = (int)normals[i * 2];
			normal_y = (int)normals[i * 2 + 1];

			INIT_VERTEX_PICT(pv, SETX(point_x + normal_x), 
				SETY(point_y + normal_y), 0, 0);
			pv++;
			INIT_VERTEX_PICT(pv, SETX(point_x), SETY(point_y), 0.9375F, 0);
			pv++;
		}
	}

	return true;
}

bool Painter::DrawRect(int x1, int y1, int x2, int y2, unsigned int color)
{
	int alpha = color_get_alpha(color);
	
	if (alpha == 0)
	{
		return true;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_LINES, 4, alpha);

	// 上
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y1), color);	
	pv++;

	// 左
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y2), color);	
	pv++;

	// 线的末端不会画出

	// 下
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y2 - 1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2 - 1), color);	
	pv++;
	
	// 右
	INIT_VERTEX_LINE(pv, SETX(x2 - 1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2 - 1), SETY(y2), color);	
	pv++;

	/*
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y2), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1 - 1), color);	
	pv++;*/
	
	return true;
}

bool Painter::FillRect(int x1, int y1, int x2, int y2, unsigned int color)
{
	int alpha = color_get_alpha(color);
	
	if (0 == alpha)
	{
		return true;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_TRIANGLES, 2, alpha);
	
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y2), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y2), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
	pv++;

	return true;
}

bool Painter::FillRound(int x1, int y1, int x2, int y2, unsigned int color, 
	int fan_num)
{
	Assert(fan_num > 0);
	
	int alpha = color_get_alpha(color);
	
	if (0 == alpha)
	{
		return true;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;
	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	// 扇面角度
	float fan_angle = FLOAT_PI * 2.0F / float(fan_num);
	TAutoMem<FmVec3, 32, TCoreAlloc> auto_buf(fan_num  + 1);
	FmVec3* points = auto_buf.GetBuffer();

	for (int i = 0; i <= fan_num; ++i)
	{
		float theta = fan_angle * i;

		points[i].x = cx + rx * cosf(theta);
		points[i].y = cy + ry * sinf(theta);
		points[i].z = 0.0F;
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_TRIANGLES, fan_num, 
		alpha);
	
	for (int k = 0; k < fan_num; ++k)
	{
		FmVec3& v1 = points[k];
		FmVec3& v2 = points[k + 1];
		
		INIT_VERTEX_LINE(pv, SETX(cx), SETY(cy), color);	
		pv++;
		INIT_VERTEX_LINE(pv, SETX(v1.x), SETY(v1.y), color);	
		pv++;
		INIT_VERTEX_LINE(pv, SETX(v2.x), SETY(v2.y), color);	
		pv++;
	}

	return true;
}

bool Painter::FillRoundPart(int x1, int y1, int x2, int y2, 
	unsigned int color, int show_fan_num, int total_fan_num)
{
	Assert(total_fan_num > 0);

    if (show_fan_num == 0)
    {
        return true;
    }

	int alpha = color_get_alpha(color);
	
	if (0 == alpha)
	{
		return true;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	// 中心点
	float cx = float(x1 + x2) * 0.5F;
	float cy = float(y1 + y2) * 0.5F;
	// 半径
	float rx = float(x2 - x1) * 0.5F;
	float ry = float(y2 - y1) * 0.5F;
	// 扇面角度
	float fan_angle = FLOAT_PI * 2.0F / float(total_fan_num);
	TAutoMem<FmVec3, 32, TCoreAlloc> auto_buf(show_fan_num  + 1);
	FmVec3* points = auto_buf.GetBuffer();

	for (int i = 0; i <= show_fan_num; ++i)
	{
		float theta = -fan_angle * i - FLOAT_PI / 2.0F;

		points[i].x = cx + rx * cosf(theta);
		points[i].y = cy + ry * sinf(theta);
		points[i].z = 0.0F;
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_TRIANGLES, 
		show_fan_num, alpha);
	
	for (int k = 0; k < show_fan_num; ++k)
	{
		FmVec3& v1 = points[k];
		FmVec3& v2 = points[k + 1];
		
		INIT_VERTEX_LINE(pv, SETX(cx), SETY(cy), color);	
		pv++;
		INIT_VERTEX_LINE(pv, SETX(v1.x), SETY(v1.y), color);	
		pv++;
		INIT_VERTEX_LINE(pv, SETX(v2.x), SETY(v2.y), color);	
		pv++;
	}

	return true;
}

bool Painter::DrawLine(int x1, int y1, int x2, int y2, unsigned int color)
{
	int alpha = color_get_alpha(color);
	
	if (alpha == 0)
	{
		return true;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_LINES, 1, alpha);
	
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
	pv++;

	return true;
}

bool Painter::DrawLinkedLines(int points[], int point_num, unsigned int color)
{
	int alpha = color_get_alpha(color);

	if (alpha == 0)
	{
		return true;
	}

	int line_num = point_num - 1;

	if (line_num <= 0)
	{
		return true;
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_LINES, line_num, alpha);

	if (m_bScaleEnable)
	{
		for (int i = 0; i < line_num; ++i)
		{
			int x1 = ScaledValue(points[i * 2]);
			int y1 = ScaledValue(points[i * 2 + 1]);
			int x2 = ScaledValue(points[i * 2 + 2]);
			int y2 = ScaledValue(points[i * 2 + 3]);
			
			INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
			pv++;
			INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
			pv++;
		}
	}
	else
	{
		for (int i = 0; i < line_num; ++i)
		{
			int x1 = points[i * 2];
			int y1 = points[i * 2 + 1];
			int x2 = points[i * 2 + 2];
			int y2 = points[i * 2 + 3];

			INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
			pv++;
			INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
			pv++;
		}
	}

	return true;
}

bool Painter::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
	unsigned int color)
{
	int alpha = color_get_alpha(color);
	
	if (alpha == 0)
	{
		return true;
	}
	
	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
		x3 = ScaledValue(x3);
		y3 = ScaledValue(y3);
	}

	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_TRIANGLES, 1, alpha);
	
	INIT_VERTEX_LINE(pv, SETX(x1), SETY(y1), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x2), SETY(y2), color);	
	pv++;
	INIT_VERTEX_LINE(pv, SETX(x3), SETY(y3), color);	
	pv++;

	return true;
}

bool Painter::DrawGrids(int x1, int y1, int x2, int y2, int grid_size, 
	unsigned int color)
{
	int alpha = color_get_alpha(color);
	
	if (alpha == 0)
	{
		return true;
	}

	if (m_bScaleEnable)
	{
		x1 = ScaledValue(x1);
		y1 = ScaledValue(y1);
		x2 = ScaledValue(x2);
		y2 = ScaledValue(y2);
	}

	int rows = (y2 - y1 + grid_size - 1) / grid_size;
	int cols = (x2 - x1 + grid_size - 1) / grid_size;
	
	CPaintBatchs::vertex_line_t* pv = NewLineBatch(IRenderDrawOp::DRAW_LINES, rows + cols, 
		alpha);
	int y = y1;
	
	for (int r = 0; r < rows; r++)
	{
		INIT_VERTEX_LINE(pv, SETX(x1), SETY(y), color);
		pv++;
		INIT_VERTEX_LINE(pv, SETX(x2), SETY(y), color);
		pv++;
		y += grid_size;
	}

	int x = x1;
	
	for (int c = 0; c < cols; c++)
	{
		INIT_VERTEX_LINE(pv, SETX(x), SETY(y1), color);
		pv++;
		INIT_VERTEX_LINE(pv, SETX(x), SETY(y2), color);
		pv++;
		x += grid_size;
	}

	return true;
}

bool Painter::DrawPoints(void* pdata, int point_num)
{
	CPaintBatchs::vertex_point_t* pv = NewPointBatch(point_num, 128);

	memcpy(pv, pdata, point_num * sizeof(CPaintBatchs::vertex_point_t));

	return true;
}

bool Painter::DrawTextLen(int x, int y, unsigned int color, 
	const wchar_t* text, size_t len)
{
	if (0 == len)
	{
		return true;
	}

	IFont* pFont = m_pCurFont;

	if (NULL == pFont)
	{
		return false;
	}

	float ratio = 1.0F;

	if (m_bScaleEnable)
	{
		ratio = m_fScaleRatio;
	}

	return pFont->WriteTextLen(x, y, m_fDepthZ, m_fDepthW, color, text, len, 
		ratio);
}

bool Painter::WriteText(int x, int y, unsigned int color, const wchar_t* text)
{
	if (m_bScaleEnable)
	{
		x = ScaledValue(x);
		y = ScaledValue(y);
	}

	return DrawTextLen(x, y, color, text, wcslen(text));
}

bool Painter::WriteTextLen(int x, int y, unsigned int color, 
	const wchar_t* text, size_t len)
{
	if (m_bScaleEnable)
	{
		x = ScaledValue(x);
		y = ScaledValue(y);
	}

	return DrawTextLen(x, y, color, text, len);
}

bool Painter::WriteTextCenter(int x1, int y1, int x2, int y2,
	unsigned int color, const wchar_t* text)
{
	Assert(text != NULL);

	if (WideStrEmpty(text))
	{
		return true;
	}
	
	IFont* pFont = m_pCurFont;

	if (NULL == pFont)
	{
		return false;
	}
	
	size_t len = wcslen(text);
	int h = pFont->GetTextHeight();
	int w = pFont->GetTextLenWidth(text, len);
	// 起始位置
	int x = x1 + (x2 - x1 - w) / 2;
	int y = y1 + (y2 - y1 - h) / 2;
	
	if (m_bScaleEnable)
	{
		x = ScaledValue(x);
		y = ScaledValue(y);
	}

	return DrawTextLen(x, y, color, text, len);
}

bool Painter::WriteTextLeft(int x1, int y1, int x2, int y2,
	unsigned int color, const wchar_t* text)
{
	Assert(text != NULL);

	if (WideStrEmpty(text))
	{
		return true;
	}
	
	IFont* pFont = m_pCurFont;

	if (NULL == pFont)
	{
		return false;
	}
	
	int h = pFont->GetTextHeight();
	int x = x1;
	int y = y1 + (y2 - y1 - h) / 2;

	if (m_bScaleEnable)
	{
		x = ScaledValue(x);
		y = ScaledValue(y);
	}
	
	return DrawTextLen(x, y, color, text, wcslen(text));
}

bool Painter::WriteTextRight(int x1, int y1, int x2, int y2,
	unsigned int color, const wchar_t* text)
{
	Assert(text != NULL);
	
	if (WideStrEmpty(text))
	{
		return true;
	}
	
	IFont* pFont = m_pCurFont;
	
	if (NULL == pFont)
	{
		return false;
	}
	
	size_t len = wcslen(text);
	int w = pFont->GetTextLenWidth(text, len);
	int h = pFont->GetTextHeight();
	int x = x2 - w;
	int y = y1 + (y2 - y1 - h) / 2;

	if (m_bScaleEnable)
	{
		x = ScaledValue(x);
		y = ScaledValue(y);
	}

	return DrawTextLen(x, y, color, text, len);
}

int Painter::GetTextHeight(const wchar_t* text)
{
	IFont* pFont = m_pCurFont;
	
	if (NULL == pFont)
	{
		return 0;
	}
	
	return pFont->GetTextHeight();
}

int Painter::GetTextWidth(const wchar_t* text)
{
	return GetTextLenWidth(text, wcslen(text));
}

int Painter::GetTextLenWidth(const wchar_t* text, size_t len)
{
	IFont* pFont = m_pCurFont;
	
	if (NULL == pFont)
	{
		return 0;
	}
	
	return pFont->GetTextLenWidth(text, len);
}

// 获取下一个语言字符的宽度
int Painter::GetNextLanguageCharacter(const wchar_t * text, const size_t begin)
{
	IFont* pFont = m_pCurFont;
	
	if (NULL == pFont)
	{
		return 0;
	}
	
	return pFont->GetNextLanguageCharacter(text, begin);
}

int Painter::GetTextInterval()
{
	IFont* pFont = m_pCurFont;

	if (NULL == pFont)
	{
		return 0;
	}

	return pFont->GetInterval() / 2;
}

int Painter::GetTextVisible(const wchar_t* text, int width)
{
	Assert(text != NULL);

	IFont* pFont = m_pCurFont;

	if (NULL == pFont)
	{
		return 0;
	}
	
	size_t len = wcslen(text);
	
	while (len > 0)
	{
		int w = pFont->GetTextLenWidth(text, len);

		if (w <= width)
		{
			break;
		}
		
		len--;
	}
	
	return (int)len;
}

PERSISTID Painter::FindGlyph(const char* name)
{
	ITexture* pGlyph = m_pRender->FindTexture(name);

	if (NULL == pGlyph)
	{
		return PERSISTID();
	}

	return pGlyph->GetID();
}

PERSISTID Painter::CreateGlyph(const char* name, bool async)
{
	ITexture* pGlyph = m_pRender->CreateGlyphTexture(name, async);

	if (NULL == pGlyph)
	{
		return PERSISTID();
	}
	
	return pGlyph->GetID();
}

PERSISTID Painter::CreateNewGlyph(const char* name, size_t width, 
	size_t height)
{
	ITexture* pGlyph = m_pRender->CreateDynamicTexture(name, 
		(unsigned int)width, (unsigned int)height, 1, TEX_FORMAT_RGBA8888_UNORM);

	if (NULL == pGlyph)
	{
		return PERSISTID();
	}

	return pGlyph->GetID();
}

bool Painter::ReleaseGlyph(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);

	if (NULL == pGlyph)
	{
		return false;
	}

	pGlyph->Release();

	return true;
}

int Painter::GetGlyphWidth(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);

	if (NULL == pGlyph)
	{
		return 0;
	}
	
	return pGlyph->GetOriginWidth();
}

int Painter::GetGlyphHeight(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);
	
	if (NULL == pGlyph)
	{
		return 0;
	}
	
	return pGlyph->GetOriginHeight();
}

unsigned int Painter::GetGlyphFormat(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);

	if (NULL == pGlyph)
	{
		return 0;
	}

	return pGlyph->GetFormat();
}

bool Painter::GetGlyphValid(const PERSISTID& id)
{
	return m_pRender->GetTextureById(id) != NULL;
}

bool Painter::GetGlyphReady(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);

	if (NULL == pGlyph)
	{
		return false;
	}

	return pGlyph->IsReady();
}

bool Painter::GetGlyphLoadComplete(const PERSISTID& id)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);

	if (NULL == pGlyph)
	{
		// 如果不存在也返回成功
		return true;
	}

	return pGlyph->IsLoadComplete();
}

bool Painter::GetGlyphAlpha(const PERSISTID& id, int x, int y, int& alpha)
{
	CTexture* pGlyph = m_pRender->GetTextureById(id);
	
	if (NULL == pGlyph)
	{
		return false;
	}

	float u = (float)x / (float)pGlyph->GetOriginWidth();
	float v = (float)y / (float)pGlyph->GetOriginHeight();

	alpha = pGlyph->GetTextureAlpha(u, v);

	return true;
}

bool Painter::AddSysFont(const char* name, const char* font_file, 
	int tex_size, int height, int interval, bool bold, bool smooth, 
	bool italic, bool underline, bool strike_out, bool var_extent, 
	bool outline, int outline_num, const FmVec3* outline_color, 
	int fadetype,  
	int shadow_color, int shadow_offset_x, int shadow_offset_y)
{
	bool reset = false;
	
	// 有则删除之
	IFont* pOldFont;
	
	if (m_Fonts.GetData(name, pOldFont))
	{
		if (pOldFont == m_pCurFont)
		{
			reset = true;
		}
		
		m_Fonts.Remove(name);
		pOldFont->Release();
	}
	
	// 添加新字体
	IFont* pFont = CFont::NewInstance(this, m_pRender, m_pFontManager, 
		font_file, tex_size, height, interval, bold, smooth, italic, 
		underline, strike_out, var_extent, outline, outline_num, outline_color, fadetype, shadow_color, 
		shadow_offset_x, shadow_offset_y);

	if (!pFont->Initialize())
	{
		pFont->Release();
		return false;
	}
	
	if (!m_Fonts.Add(name, pFont))
	{
		pFont->Release();
		return false;
	}
	
	pFont->SetName(name);

	if (strcmp(name, "Default") == 0)
	{
		m_pDefFont = pFont;
	}

	// 重新设置当前字体
	if (reset)
	{
		SetFont(name);
	}
	
	return true;
}

bool Painter::AddTexFont(const char* name, const char* font_tex,
	int tex_width, int tex_height, int height, int interval, 
	const int* char_info, int char_num)
{
	bool reset = false;
	// 有则删除之
	IFont* pOldFont;
	
	if (m_Fonts.GetData(name, pOldFont))
	{
		if (pOldFont == m_pCurFont)
		{
			reset = true;
		}
		
		m_Fonts.Remove(name);
		pOldFont->Release();
	}
	
	// 添加新字体
	IFont* pFont = CTexFont::NewInstance(this, m_pRender, font_tex, 
		tex_width, tex_height, height, interval, char_info, char_num);
	
	if (!pFont->Initialize())
	{
		pFont->Release();
		return false;
	}
	
	if (!m_Fonts.Add(name, pFont))
	{
		pFont->Release();
		return false;
	}

	pFont->SetName(name);

	if (strcmp(name, "Default") == 0)
	{
		m_pDefFont = pFont;
	}

	// 重新设置当前字体
	if (reset)
	{
		SetFont(name);
	}
	
	return true;
}

const char* Painter::GetFont()
{
	if (NULL == m_pCurFont)
	{
		return "";
	}

	return m_pCurFont->GetName();
}

bool Painter::SetFont(const char* name)
{
	IFont* pFont;

	if (!m_Fonts.GetData(name, pFont))
	{
		pFont = m_pDefFont;

		if (NULL == pFont)
		{
			return false;
		}
	}
	
	m_pCurFont = pFont;

	return true;
}


IFont* Painter::GetFontPtr()
{
	return m_pCurFont;
}

IFont* Painter::GetFont(const char* name)
{
	IFont* pFont;

	if (m_Fonts.GetData(name, pFont))
	{
		return pFont;
	}

	return NULL;
}

bool Painter::SetFontShadowColor(const char* name, unsigned int color)
{
	IFont* pFont;

	if (!m_Fonts.GetData(name, pFont))
	{
		pFont = m_pDefFont;

		if (NULL == pFont)
		{
			return false;
		}
	}

	return pFont->SetShadowColor(color);
}

bool Painter::DumpAllFontTex()
{
    if (m_pFontManager != NULL)
    {
        m_pFontManager->DumpTex();
    }

    font_container_t::iterator it = m_Fonts.Begin();
    font_container_t::iterator end = m_Fonts.End();
    size_t count = 0;

    for (; it != end; ++it)
    {
	    char fname[128];

	    SafeSprintf(fname, sizeof(fname), "font_%d.dds", count);
	    it.GetData()->DumpTex(fname);
	    ++count;
    }

	return true;
}

performance_info_t* Painter::GetPerformance()
{
	return m_pRender->GetPerformance();
}

bool Painter::SetBlendAlpha(int value)
{
	if ((value < 0) || (value > 255))
	{
		return false;
	}
	
	m_nBlendAlpha = value;

	return true;
}

int Painter::GetBlendAlpha()
{
	return m_nBlendAlpha;
}

bool Painter::SetDepthValue(float depth_z, float depth_w)
{
	if ((depth_z < 0.0F) || (depth_z > 1.0F))
	{
		return false;
	}

	m_fDepthZ = depth_z;
	m_fDepthW = depth_w;
	//m_fZValue = m_pPaintBatchs->GetDepthZValue(depth_z);
	
	return true;
}

void Painter::SetScaleEnable(bool value)
{
	m_bScaleEnable = value;
	m_nRenderWidth = int(m_pRender->GetDeviceWidth() / m_fScaleRatio);
	m_nRenderHeight = int(m_pRender->GetDeviceHeight() / m_fScaleRatio);
}

bool Painter::GetScaleEnable()
{
	return m_bScaleEnable;
}

void Painter::SetScaleRatio(float value)
{
	if ((value < 0.01F) || (value > 100.0F))
	{
        if (value < 0.01F)
        {
            value = 0.01F;
        }
        else if (value > 100.0F)
        {
            value = 100.0F;
        }
	}

	m_fScaleRatio = value;
	m_nRenderWidth = int(m_pRender->GetDeviceWidth() / (double)m_fScaleRatio + 0.5f);
	m_nRenderHeight = int(m_pRender->GetDeviceHeight() / (double)m_fScaleRatio + 0.5f);
}

float Painter::GetScaleRatio()
{
	return m_fScaleRatio;
}

//设置当前的FrameRT
void Painter::SetFrameRT(IFrameRT* pFrameRT)
{
	m_pPaintBatchs->SetFrameRT(pFrameRT);
}

IFrameRT* Painter::GetFrameRT()
{
	return m_pPaintBatchs->GetFrameRT();
}

size_t Painter::GetGuiBatchSize()
{
	return m_pPaintBatchs->GetBatchSize();
}

void Painter::SetDebugBatchNum(size_t num)
{
	m_pPaintBatchs->SetDebugBatchNum(num);
}

void Painter::SetPaintMode(int mode)
{
	m_paintmode = mode;
}

	//取得绘制模式
int Painter::GetPaintMode()const
{
	return m_paintmode;
}