
/****************************************************************
  Copyright (C), 2009-2010 YEFENG
  File name   :  primitive_3d_drawer.cpp
  Author      :  叶峰
  Version     :  1.0a
  Create Date :  2010/09/11   
  Description :  三维图形的绘制
*****************************************************************/

// INCLUDES -----------------------------------------------------------------------------

#include <assert.h>
#include <float.h>
#include "../public/macros.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/module.h"
#include "../visual/i_scene_view.h"
#include "primitive_3d_drawer.h"

// --------------------------------------------------------------------------------------

#define MIN_SLICES          10
#define DEFAULT_SLICES      24

// --------------------------------------------------------------------------------------

DECLARE_FUNCTION_INTERFACE(Primitive3DDrawer);

// --------------------------------------------------------------------------------------

IRender*			Primitive3DDrawer::s_pRender = NULL;
IShaderProgram*	    Primitive3DDrawer::s_pPrimitiveShader = NULL;
IDynamicVB*		    Primitive3DDrawer::s_pPrimitiveVB = NULL;
fm_int			    Primitive3DDrawer::s_hMatWVP = -1;

// --------------------------------------------------------------------------------------

Primitive3DDrawer::Primitive3DDrawer()
{
    m_pSceneView = NULL;

	m_nSlices = 0;
    m_pSinList = NULL;
    m_pCosList = NULL;
    m_pVerticesBuffer = NULL;

	m_listBatchData = NULL;
	memset(m_batchData, 0, sizeof(m_batchData));

    FmMat4Identity(&m_mtxWorld);
    m_bWireMode = false;
    m_bZEnable = true;
    m_bCullFace = true;
}

Primitive3DDrawer::~Primitive3DDrawer()
{
    ClearInternal();

	BatchData* ptr = m_listBatchData;
	BatchData* pNext;
	while (ptr)
	{
		pNext = ptr->pNext;
		assert(ptr->nUsed == 0);
		CORE_DELETE(ptr);
		ptr = pNext;
	}
    m_listBatchData = NULL;

    SAFE_RELEASE(s_pPrimitiveVB);
    SAFE_RELEASE(s_pPrimitiveShader);
}

bool                    Primitive3DDrawer::Init()
{
	s_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == s_pRender)
	{
		return false;
	}

    assert(!s_pPrimitiveShader);
	IVertexShader* pVS = s_pRender->LoadVertexShader("design_line.vsh", "main", "");
	IPixelShader* pPS = s_pRender->LoadPixelShader("design_line.fsh", "main", "");
	const char* vb[] = {"iPos", "iDiffuse"};
	s_pPrimitiveShader = s_pRender->CreateShaderProgram(pVS, pPS, vb, 2);
    assert(s_pPrimitiveShader);
	s_hMatWVP = s_pPrimitiveShader->GetParamOp()->FindParamIdByName("c_mtxWVP");

	s_pPrimitiveVB = s_pRender->CreateDynamicVB(sizeof(PrimitiveVertex)*MAX_BATCH_VERTICES_NUM, "Primitive3DDrawer::s_pPrimitiveVB");

    m_pSceneView = s_pRender->GetSceneView();

    BuildInternal(DEFAULT_SLICES);

    return true;
}

bool                    Primitive3DDrawer::Shut()
{
    ClearInternal();

    BatchData* ptr = m_listBatchData;
    while (ptr)
    {
        ptr->nUsed = 0;
        ptr = ptr->pNext;
    }

    SAFE_RELEASE(s_pPrimitiveVB);
    SAFE_RELEASE(s_pPrimitiveShader);

    return true;
}

void                    Primitive3DDrawer::BuildInternal(unsigned int slices)
{
    m_pSinList = (float*)CORE_ALLOC(sizeof(float) * (slices + 1));
    m_pCosList = (float*)CORE_ALLOC(sizeof(float) * (slices + 1));

    float _angle;
    for (unsigned int i = 0; i < slices; i++)
    {
        _angle = FM_TWO_PI * i / slices;
        m_pSinList[i] = sinf(_angle);
        m_pCosList[i] = cosf(_angle);
    }
    m_pSinList[slices] = m_pSinList[0];
    m_pCosList[slices] = m_pCosList[0];

    m_pVerticesBuffer = (FmVec3*)CORE_ALLOC(sizeof(FmVec3) * (2*slices + 2));

    m_nSlices = slices;
}

void                    Primitive3DDrawer::ClearInternal()
{
	if (m_pSinList)
	{
		CORE_FREE(m_pSinList, sizeof(float) * (m_nSlices + 1));  
		m_pSinList = NULL;
	}
	if (m_pCosList)
	{
		CORE_FREE(m_pCosList, sizeof(float) * (m_nSlices + 1));  
		m_pCosList = NULL;
	}
	if (m_pVerticesBuffer)
	{
		CORE_FREE(m_pVerticesBuffer, sizeof(FmVec3) * (2*m_nSlices + 2));  
		m_pVerticesBuffer = NULL;
	}

    m_nSlices = 0;
}

// 设置当前场景视图
void                    Primitive3DDrawer::SetSceneView(ISceneView* pSceneView)
{
    m_pSceneView = pSceneView;
}

// 获得当前场景视图
ISceneView*             Primitive3DDrawer::GetSceneView()
{
    return m_pSceneView;
}

bool                    Primitive3DDrawer::IsReady() const
{
    return (s_pRender != NULL && m_pSceneView != NULL && m_nSlices >= MIN_SLICES);
}

bool                    Primitive3DDrawer::SetSlices(unsigned int slices)
{
    if (slices < MIN_SLICES)
    {
        slices = MIN_SLICES;
    }
    ClearInternal();
    BuildInternal(slices);
    return IsReady();
}

// 世界矩阵
void                    Primitive3DDrawer::SetMtxWorld(const FmMat4& mtxWorld)
{
    Flush();
    m_mtxWorld = mtxWorld;
}

void                    Primitive3DDrawer::SetMtxWorldIdentity()
{
    Flush();
    FmMat4Identity(&m_mtxWorld);
}

const FmMat4&           Primitive3DDrawer::GetMtxWorld() const
{
    return m_mtxWorld;
}

// 是否画线模式
void                    Primitive3DDrawer::SetWireMode(bool value)
{
    if (m_bWireMode != value)
    {
        Flush();
        m_bWireMode = value;
    }
}

bool                    Primitive3DDrawer::GetWireMode() const
{
    return m_bWireMode;
}

// 是否背面剔除
void                    Primitive3DDrawer::SetCullFace(bool value)
{
    if (m_bCullFace != value)
    {
        Flush();
        m_bCullFace = value;
    }
}

bool                    Primitive3DDrawer::GetCullFace() const
{
    return m_bCullFace;
}

// 是否使用深度测试
void                    Primitive3DDrawer::SetZEnable(bool value)
{
    if (m_bZEnable != value)
    {
        Flush();
        m_bZEnable = value;
    }
}

bool                    Primitive3DDrawer::GetZEnable() const
{
    return m_bZEnable;
}

void                    Primitive3DDrawer::PushVertex
(
    const FmVec3&       vertex, 
    unsigned int        color, 
    IRenderDrawOp::DRAW_MODE     type
)
{
	BatchData* pBatchData = GetBatchData(type, 1);
	if (!pBatchData)
	{
		return;
	}

    pBatchData->verticesBuf[pBatchData->nUsed]._x = vertex.x;
    pBatchData->verticesBuf[pBatchData->nUsed]._y = vertex.y;
    pBatchData->verticesBuf[pBatchData->nUsed]._z = vertex.z;
    pBatchData->verticesBuf[pBatchData->nUsed]._color = color;

    pBatchData->nUsed++;
}

void                    Primitive3DDrawer::PushVertices
(
    const FmVec3*       pVertices, 
    unsigned int        numVertices,
    unsigned int        color, 
    IRenderDrawOp::DRAW_MODE     type
)
{
	BatchData* pBatchData = GetBatchData(type, numVertices);
	if (!pBatchData)
	{
		return;
	}

    for (unsigned int i = 0; i < numVertices; i++)
    {
        pBatchData->verticesBuf[pBatchData->nUsed + i]._x = pVertices[i].x;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._y = pVertices[i].y;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._z = pVertices[i].z;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._color= color;
    }

    pBatchData->nUsed += numVertices;
}

void                    Primitive3DDrawer::PushVertices
(
    const FmVec3*       pVertices, 
    const unsigned int* pColors, 
    unsigned int        numVertices,
    IRenderDrawOp::DRAW_MODE     type
)
{
	BatchData* pBatchData = GetBatchData(type, numVertices);
	if (!pBatchData)
	{
		return;
	}

    for (unsigned int i = 0; i < numVertices; i++)
    {
        pBatchData->verticesBuf[pBatchData->nUsed + i]._x = pVertices[i].x;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._y = pVertices[i].y;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._z = pVertices[i].z;
        pBatchData->verticesBuf[pBatchData->nUsed + i]._color= pColors[i];
    }

	pBatchData->nUsed += numVertices;
}

 void                   Primitive3DDrawer::Flush()
{
	FlushBatchData(IRenderDrawOp::DRAW_POINTS);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_LOOP);
	FlushBatchData(IRenderDrawOp::DRAW_LINES);
	FlushBatchData(IRenderDrawOp::DRAW_TRIANGLE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_TRIANGLE_FAN);
	FlushBatchData(IRenderDrawOp::DRAW_TRIANGLES);
}

void				    Primitive3DDrawer::DrawBatchData(void* pdata)
{
    BatchData* pBatchData = (BatchData*)pdata;
    if (!pBatchData->nUsed)
    {
        return;
    }

    s_pRender->UpdateDynamicVB(s_pPrimitiveVB, 0, pBatchData->verticesBuf, sizeof(PrimitiveVertex)*pBatchData->nUsed);

    s_pPrimitiveShader->UsedShader();

    IRenderDrawOp* pRenderDrawOp = s_pRender->GetRenderDrawOp();
    pRenderDrawOp->SetVB(s_pPrimitiveVB->GetBuffer());

    pRenderDrawOp->EnableVertexAttribArray(0, true);
    pRenderDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(PrimitiveVertex), (void*)(0));
    pRenderDrawOp->EnableVertexAttribArray(1, true);
    pRenderDrawOp->SetVertexAttribPointer(1, 4, IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE, sizeof(PrimitiveVertex), (void*)(3 * sizeof(float)));

    IShaderParamOp* pShaderParamOp = s_pPrimitiveShader->GetParamOp();
    pShaderParamOp->SetParamValue(s_hMatWVP, pBatchData->mtxWVP);

    IRenderStateOp* pStateOp = s_pPrimitiveShader->GetRenderStateOp();
    pStateOp->EnableDepthWrite(pBatchData->bZEnable);
    pStateOp->EnableDepthTest(pBatchData->bZEnable);
    pStateOp->EnableCullFace(!pBatchData->bCullFace);

    s_pRender->GetRenderDrawOp()->DrawArrays(pBatchData->drawType, 0, pBatchData->nUsed);

    /*
    pRenderDrawOp->SetVB( 0 );
    pRenderDrawOp->EnableVertexAttribArray(0, false);
    pRenderDrawOp->EnableVertexAttribArray(1, false);
    */

    pBatchData->nUsed = 0;
}

void                    Primitive3DDrawer::UpdateBatchData(BatchData* pBatchData, IRenderDrawOp::DRAW_MODE drawMode)
{
    pBatchData->drawType = drawMode;

    // 设置WVP
    FmMat4 mtxWorld = m_mtxWorld;    
    IRenderContext* pContext = s_pRender->GetContext();
    const FmVec3& refer_pos = pContext->GetReferPosition();
    mtxWorld._41 -= refer_pos.x;
    mtxWorld._42 -= refer_pos.y;
    mtxWorld._43 -= refer_pos.z;
    FmMat4Multiply(&pBatchData->mtxWVP, &mtxWorld, &pContext->GetCamera().mtxViewProjRelative);
    FmMat4Transpose(&pBatchData->mtxWVP, &pBatchData->mtxWVP);

    pBatchData->bWireMode = m_bWireMode;
    pBatchData->bCullFace = m_bCullFace;
    pBatchData->bZEnable = m_bZEnable;
}

Primitive3DDrawer::BatchData* Primitive3DDrawer::GetBatchData(IRenderDrawOp::DRAW_MODE drawMode, unsigned int verticesNum)
{
    if (!m_pSceneView || verticesNum > MAX_BATCH_VERTICES_NUM)
    {
        return NULL;
    }

    if (m_batchData[drawMode])
    {
        if (m_batchData[drawMode]->nUsed + verticesNum <= MAX_BATCH_VERTICES_NUM)
        {
            return m_batchData[drawMode];
        }
        else
        {
            FlushBatchData(drawMode);
        }
    }

    if (!m_listBatchData)
    {
        m_listBatchData = CORE_NEW(BatchData);
        UpdateBatchData(m_listBatchData, drawMode);
        m_batchData[drawMode] = m_listBatchData;

        return m_batchData[drawMode];
    }

    BatchData* ptr = m_listBatchData;
    while (ptr)
    {
        if (ptr->nUsed == 0)
        {
            UpdateBatchData(ptr, drawMode);
            m_batchData[drawMode] = ptr;
            break;
        }

        if (ptr->pNext)
        {
            ptr = ptr->pNext;
        }
        else
        {
            ptr->pNext = CORE_NEW(BatchData);
            UpdateBatchData(ptr->pNext, drawMode);
            m_batchData[drawMode] = ptr->pNext;
            break;
        }
    }

    return m_batchData[drawMode];
}

void					Primitive3DDrawer::FlushBatchData(IRenderDrawOp::DRAW_MODE drawMode)
{
    if (!m_batchData[drawMode] || !m_pSceneView)
    {
        return;
    }

    if (m_batchData[drawMode]->nUsed == 0)
    {
        m_batchData[drawMode] = NULL;
        return;
    }

    m_pSceneView->AddSolidBatch(DrawBatchData, m_batchData[drawMode], "Primitive3DDrawer::FlushBatchData");

    m_batchData[drawMode] = NULL;
}

//基本图形
void                    Primitive3DDrawer::DrawVertex
(
    const FmVec3&       vPos, 
    unsigned int        color
)
{
    PushVertex(vPos, color, IRenderDrawOp::DRAW_POINTS);
}

void                    Primitive3DDrawer::DrawLine
(
    const FmVec3&       vStart, 
    const FmVec3&       vEnd, 
    unsigned int        color
)
{
    PushVertex(vStart, color, IRenderDrawOp::DRAW_LINES);
    PushVertex(vEnd, color, IRenderDrawOp::DRAW_LINES);
}

void                    Primitive3DDrawer::DrawLine
(
    const FmVec3&       vStart, 
    const FmVec3&       vEnd, 
    unsigned int        colStart, 
    unsigned int        colEnd
)
{
    PushVertex(vStart, colStart, IRenderDrawOp::DRAW_LINES);
    PushVertex(vEnd, colEnd, IRenderDrawOp::DRAW_LINES);
}

void                    Primitive3DDrawer::DrawLineList
(
    const FmVec3*       pVertices, 
    const unsigned int* pColors,
    unsigned int        numPoints,
    float               depth
)
{
    if (numPoints % 2 == 1)
    {
        numPoints--;
    }
    if (numPoints < 2)
    {
        return;
    }

    PushVertices(pVertices, pColors, numPoints, IRenderDrawOp::DRAW_LINES);
}

void                    Primitive3DDrawer::DrawLineList
(
    const FmVec3*       pVertices, 
    unsigned int        color,
    unsigned int        numPoints,
    float               depth
)
{
    if (numPoints % 2 == 1)
    {
        numPoints--;
    }
    if (numPoints < 2)
    {
        return;
    }
    PushVertices(pVertices, numPoints, color, IRenderDrawOp::DRAW_LINES);
}

void                    Primitive3DDrawer::DrawLineStrip
(
    const FmVec3*       pVertices, 
    const unsigned int* pColors,
    unsigned int        numPoints,
    float               depth
)
{
    PushVertices(pVertices, pColors, numPoints, IRenderDrawOp::DRAW_LINE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_STRIP);
}

void                    Primitive3DDrawer::DrawLineStrip
(
    const FmVec3*       pVertices, 
    unsigned int        color,
    unsigned int        numPoints,
    float               depth
)                       
{
	PushVertices(pVertices, numPoints, color, IRenderDrawOp::DRAW_LINE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_STRIP);
}

void                    Primitive3DDrawer::DrawLineLoop
(
    const FmVec3*       pVertices, 
    const unsigned int* pColors,
    unsigned int        numPoints,
    float               depth
)
{
	PushVertices(pVertices, pColors, numPoints, IRenderDrawOp::DRAW_LINE_LOOP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_LOOP);
}

void                    Primitive3DDrawer::DrawLineLoop
(
    const FmVec3*       pVertices, 
    unsigned int        color,
    unsigned int        numPoints,
    float               depth
)
{
	PushVertices(pVertices, numPoints, color, IRenderDrawOp::DRAW_LINE_LOOP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_LOOP);
}

void                    Primitive3DDrawer::DrawTriangle
(
    const FmVec3&       v1, 
    const FmVec3&       v2,  
    const FmVec3&       v3, 
    unsigned int        color
)
{
    if (m_bWireMode)
    {
        PushVertex(v1, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v2, color, IRenderDrawOp::DRAW_LINES);

        PushVertex(v2, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v3, color, IRenderDrawOp::DRAW_LINES);

        PushVertex(v3, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v1, color, IRenderDrawOp::DRAW_LINES);
    }
    else
    {
        PushVertex(v1, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v2, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color, IRenderDrawOp::DRAW_TRIANGLES);
    }
}

void                    Primitive3DDrawer::DrawTriangle
(
    const FmVec3&       v1, 
    const FmVec3&       v2,  
    const FmVec3&       v3, 
    unsigned int        color1, 
    unsigned int        color2,
    unsigned int        color3
)
{
    if (m_bWireMode)
    {
        PushVertex(v1, color1, IRenderDrawOp::DRAW_LINES);
        PushVertex(v2, color2, IRenderDrawOp::DRAW_LINES);

        PushVertex(v2, color2, IRenderDrawOp::DRAW_LINES);
        PushVertex(v3, color3, IRenderDrawOp::DRAW_LINES);

        PushVertex(v3, color3, IRenderDrawOp::DRAW_LINES);
        PushVertex(v1, color1, IRenderDrawOp::DRAW_LINES);
    }
    else
    {
        PushVertex(v1, color1, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v2, color2, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color3, IRenderDrawOp::DRAW_TRIANGLES);
    }
}

void                    Primitive3DDrawer::DrawQuadrangle
(
    const FmVec3&       v1, 
    const FmVec3&       v2,  
    const FmVec3&       v3,  
    const FmVec3&       v4, 
    unsigned int        color
)
{
    if (m_bWireMode)
    {
        PushVertex(v1, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v2, color, IRenderDrawOp::DRAW_LINES);
                                                  
        PushVertex(v2, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v3, color, IRenderDrawOp::DRAW_LINES);

        PushVertex(v3, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v4, color, IRenderDrawOp::DRAW_LINES);
                                                  
        PushVertex(v4, color, IRenderDrawOp::DRAW_LINES);
        PushVertex(v1, color, IRenderDrawOp::DRAW_LINES);
    }
    else
    {
        PushVertex(v1, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v2, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color, IRenderDrawOp::DRAW_TRIANGLES);

        PushVertex(v1, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v4, color, IRenderDrawOp::DRAW_TRIANGLES);
    }
}

void                    Primitive3DDrawer::DrawQuadrangle
(
    const FmVec3&       v1, 
    const FmVec3&       v2,  
    const FmVec3&       v3,  
    const FmVec3&       v4, 
    unsigned int        color1, 
    unsigned int        color2,
    unsigned int        color3,
    unsigned int        color4
)
{
    if (m_bWireMode)
    {
        PushVertex(v1, color1, IRenderDrawOp::DRAW_LINES);
        PushVertex(v2, color2, IRenderDrawOp::DRAW_LINES);

        PushVertex(v2, color2, IRenderDrawOp::DRAW_LINES);
        PushVertex(v3, color3, IRenderDrawOp::DRAW_LINES);

        PushVertex(v3, color3, IRenderDrawOp::DRAW_LINES);
        PushVertex(v4, color4, IRenderDrawOp::DRAW_LINES);

        PushVertex(v4, color4, IRenderDrawOp::DRAW_LINES);
        PushVertex(v1, color1, IRenderDrawOp::DRAW_LINES);
    }
    else
    {
        PushVertex(v1, color1, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v2, color2, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color3, IRenderDrawOp::DRAW_TRIANGLES);

        PushVertex(v1, color1, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v3, color3, IRenderDrawOp::DRAW_TRIANGLES);
        PushVertex(v4, color4, IRenderDrawOp::DRAW_TRIANGLES);
    }
}

// 面图形
void                    Primitive3DDrawer::DrawPlane
(
    const FmVec3&       vCenter, 
    float               fWidth,
    float               fLength,
    unsigned int        color,
    YeAxis3DType        axisPlane
)
{
    FmVec3* v = m_pVerticesBuffer;
    float fHalfW = fWidth * 0.5f;
    float fHalfL = fLength * 0.5f;
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            v[0] = FmVec3(-fHalfW, -fHalfL, 0.0f);
            v[1] = FmVec3(-fHalfW,  fHalfL, 0.0f);
            v[2] = FmVec3( fHalfW,  fHalfL, 0.0f);
            v[3] = FmVec3( fHalfW, -fHalfL, 0.0f);
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            v[0] = FmVec3(0.0f, -fHalfW, -fHalfL);
            v[1] = FmVec3(0.0f, -fHalfW,  fHalfL);
            v[2] = FmVec3(0.0f,  fHalfW,  fHalfL);
            v[3] = FmVec3(0.0f,  fHalfW, -fHalfL);
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            v[0] = FmVec3(-fHalfW, 0.0f, -fHalfL);
            v[1] = FmVec3(-fHalfW, 0.0f,  fHalfL);
            v[2] = FmVec3( fHalfW, 0.0f,  fHalfL);
            v[3] = FmVec3( fHalfW, 0.0f, -fHalfL);
        }
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        v[i] += vCenter;
    }

    DrawQuadrangle(v[0], v[1], v[2], v[3], color);
}

#define YD_IN_RANGE(a, b, x)            ( ((x) > (a) && (x) < (b)) || ((x) > (b) && (x) < (a)) )
#define YD_IN_RANGE_EQUAL(a, b, x)      ( ((x) >= (a) && (x) <= (b)) || ((x) >= (b) && (x) <= (a)) )
#define YD_IS_ZERO(f)					((f) < FLT_EPSILON) && ((f) > -FLT_EPSILON)

inline float        YfCalculatePlaneXbyYZ(float a, float b, float c, float d, float y, float z)
{
	if (YD_IS_ZERO(a))
	{
		return ( -(b*y + c*z + d) > 0 ? FLT_MAX : -FLT_MAX );
	}
	else
	{
		return -(b*y + c*z  + d) / a;
	}
}

inline float        YfCalculatePlaneYbyXZ(float a, float b, float c, float d, float x, float z)
{
	if (YD_IS_ZERO(b))
	{
		return ( -(a*x + c*z + d) > 0 ? FLT_MAX : -FLT_MAX );
	}
	else
	{
		return -(a*x + c*z  + d) / b;
	}
}

inline float        YfCalculatePlaneZbyXY(float a, float b, float c, float d, float x, float y)
{
	if (YD_IS_ZERO(c))
	{
		return ( -(a*x + b*y + d) > 0 ? FLT_MAX : -FLT_MAX );
	}
	else
	{
		return -(a*x + b*y  + d) / c;
	}
}

bool				YfPlaneClipAABB
(
	const FmVec3&	vMin,
	const FmVec3&	vMax, 
	const FmPlane&  plane, 
	unsigned int&   numCollided, 
	FmVec3*			pVCollided
)
{
	float x, y, z;
	numCollided = 0;
	float boxX, boxY, boxZ;
	unsigned int i;

	for (i = 0; i < 4; i++)
	{
		boxX = (i % 2 == 0) ? vMin.x : vMax.x;
		boxZ = (i < 2) ? vMin.z : vMax.z;
		y = YfCalculatePlaneYbyXZ(plane.a, plane.b, plane.c, plane.d, boxX, boxZ);
		if (YD_IN_RANGE_EQUAL(vMin.y, vMax.y, y))
		{
			pVCollided[numCollided] = FmVec3(boxX, y, boxZ);
			numCollided++;
		}
	}
	if (numCollided >= 4)
	{
		return true;
	}

	for (i = 0; i < 4; i++)
	{
		boxX = (i % 2 == 0) ? vMin.x : vMax.x;
		boxY = (i < 2) ? vMin.y : vMax.y;
		z = YfCalculatePlaneZbyXY(plane.a, plane.b, plane.c, plane.d, boxX, boxY);
		if (YD_IN_RANGE(vMin.z, vMax.z, z))
		{
			pVCollided[numCollided] = FmVec3(boxX, boxY, z);
			numCollided++;
		}
	}
	if (numCollided >= 4)
	{
		return true;
	}

	for (i = 0; i < 4; i++)
	{
		boxY = (i % 2 == 0) ? vMin.y : vMax.y;
		boxZ = (i < 2) ? vMin.z : vMax.z;
		x = YfCalculatePlaneXbyYZ(plane.a, plane.b, plane.c, plane.d, boxY, boxZ);
		if (YD_IN_RANGE(vMin.x, vMax.x, x))
		{
			pVCollided[numCollided] = FmVec3(x, boxY, boxZ);
			numCollided++;
		}
	}
	if (numCollided >= 4)
	{
		return true;
	}

	return true;
}

// 平面
void                    Primitive3DDrawer::DrawPlane
(
    const FmPlane&      plane, 
    const FmVec3&       vMin, 
    const FmVec3&       vMax, 
    unsigned int        color
)
{
    FmVec3* v = m_pVerticesBuffer;
    unsigned int numCollided;
    YfPlaneClipAABB(
        vMin,
        vMax,
        plane,
        numCollided,
        v);
    
    if (numCollided == 3)
    {
        DrawTriangle(v[0], v[1], v[2], color);
    }
    else if (numCollided == 4)
    {
#if 1
        if (m_bWireMode)
        {
            DrawTetrahedron(v, 0, color);
        }
        else
        {
		    DrawTetrahedron(v, color, 0);
        }
#else
		DrawQuadrangle(v[0], v[1], v[2], v[3], color);
#endif
    }
}

void                    Primitive3DDrawer::DrawCircleRound
(
    const FmVec3&       vCenter, 
    float               fHeight,
    float               fRadius,
    unsigned int        colCenter,
    unsigned int        colOuter,
    YeAxis3DType        axisPlane
)
{
    FmVec3 vTop = vCenter;
    FmVec3* v = m_pVerticesBuffer;
    if (!v)
    {
        return;
    }
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            vTop.z += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadius, m_pCosList[i]*fRadius, 0.0f );
                v[i] += vCenter;
            }
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            vTop.x += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( 0.0f, m_pSinList[i]*fRadius, m_pCosList[i]*fRadius );
                v[i] += vCenter;
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            vTop.y += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadius, 0.0f, m_pCosList[i]*fRadius );
                v[i] += vCenter;
            }
        }
        break;
    }

    if (m_bWireMode)
    {
        PushVertices(v, m_nSlices, colOuter, IRenderDrawOp::DRAW_LINE_LOOP);
        FlushBatchData(IRenderDrawOp::DRAW_LINE_LOOP);

        for (unsigned int i = 0; i < m_nSlices; i++)
        {
            PushVertex(vTop, colCenter, IRenderDrawOp::DRAW_LINES);
            PushVertex(v[i], colCenter, IRenderDrawOp::DRAW_LINES);            
        }
    }
    else
    {
	    PushVertex(vTop, colCenter, IRenderDrawOp::DRAW_TRIANGLE_FAN);
        PushVertices(v, m_nSlices + 1, colOuter, IRenderDrawOp::DRAW_TRIANGLE_FAN);
        FlushBatchData(IRenderDrawOp::DRAW_TRIANGLE_FAN);
    }
}

void                    Primitive3DDrawer::DrawEllipseRound
(
    const FmVec3&       vCenter, 
    float               fHeight,
    float               fRadiusW,
    float               fRadiusH,
    unsigned int        colCenter,
    unsigned int        colOuter,
    YeAxis3DType        axisPlane
)
{
    FmVec3 vTop = vCenter;
    FmVec3* v = m_pVerticesBuffer;
    if (!v)
    {
        return;
    }
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            vTop.z += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadiusW, m_pCosList[i]*fRadiusH, 0.0f );
                v[i] += vCenter;
            }
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            vTop.x += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( 0.0f, m_pSinList[i]*fRadiusW, m_pCosList[i]*fRadiusH );
                v[i] += vCenter;
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            vTop.y += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadiusW, 0.0f, m_pCosList[i]*fRadiusH );
                v[i] += vCenter;
            }
        }
        break;
    }

    if (m_bWireMode)
    {
        PushVertices(v, m_nSlices, colOuter, IRenderDrawOp::DRAW_LINE_LOOP);
        FlushBatchData(IRenderDrawOp::DRAW_LINE_LOOP);

        for (unsigned int i = 0; i < m_nSlices; i++)
        {
            PushVertex(vTop, colCenter, IRenderDrawOp::DRAW_LINES);
            PushVertex(v[i], colCenter, IRenderDrawOp::DRAW_LINES);            
        }
    }
    else
    {
        PushVertex(vTop, colCenter, IRenderDrawOp::DRAW_TRIANGLE_FAN);
	    PushVertices(v, m_nSlices + 1, colOuter, IRenderDrawOp::DRAW_TRIANGLE_FAN);
	    FlushBatchData(IRenderDrawOp::DRAW_TRIANGLE_FAN);
    }
}

void                    Primitive3DDrawer::DrawCone
(
    const FmVec3&       vCenter, 
    float               fHeight,
    float               fRadius,
    unsigned int        colTop,
    unsigned int        colBottom,
    YeAxis3DType        axisPlane
)
{
    DrawCircleRound(vCenter, fHeight, fRadius, colTop, colBottom, axisPlane);
    DrawCircleRound(vCenter, 0.0f, fRadius, colBottom, colBottom, axisPlane);
}

void                    Primitive3DDrawer::DrawPyramid
(
    const FmVec3&       vCenter, 
    float               fWidth,
    float               fLength,
    float               fHeight,
    unsigned int        colTop,
    unsigned int        colBottom,
    YeAxis3DType        axisPlane
)
{
    FmVec3* v = m_pVerticesBuffer;
    float fHalfW = fWidth * 0.5f;
    float fHalfL = fLength * 0.5f;
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            v[0] = FmVec3( 0, 0, fHeight);
            v[1] = FmVec3(-fHalfW, -fHalfL, 0.0f);
            v[2] = FmVec3(-fHalfW,  fHalfL, 0.0f);
            v[3] = FmVec3( fHalfW,  fHalfL, 0.0f);
            v[4] = FmVec3( fHalfW, -fHalfL, 0.0f);
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            v[0] = FmVec3(fHeight, 0, 0);
            v[1] = FmVec3(0.0f, -fHalfW, -fHalfL);
            v[2] = FmVec3(0.0f, -fHalfW,  fHalfL);
            v[3] = FmVec3(0.0f,  fHalfW,  fHalfL);
            v[4] = FmVec3(0.0f,  fHalfW, -fHalfL);
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            v[0] = FmVec3(0, fHeight, 0);
            v[1] = FmVec3(-fHalfW, 0.0f, -fHalfL);
            v[2] = FmVec3(-fHalfW, 0.0f,  fHalfL);
            v[3] = FmVec3( fHalfW, 0.0f,  fHalfL);
            v[4] = FmVec3( fHalfW, 0.0f, -fHalfL);
        }
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        v[i] += vCenter;
    }

    DrawQuadrangle(v[1], v[2], v[3], v[4], colBottom);

    DrawTriangle(v[0], v[1], v[2], colTop, colBottom, colBottom);
    DrawTriangle(v[0], v[2], v[3], colTop, colBottom, colBottom);
    DrawTriangle(v[0], v[3], v[4], colTop, colBottom, colBottom);
    DrawTriangle(v[0], v[4], v[1], colTop, colBottom, colBottom);
}

void                    Primitive3DDrawer::DrawLoop
(
    const FmVec3&       vPosition, 
    float               fHeight,
    float               fInRadius,
    float               fOutRadius,
    unsigned int        colIn,
    unsigned int        colOut,
    YeAxis3DType        axisPlane
)
{
    if (fInRadius < FLT_EPSILON)
    {
        DrawCircleRound(vPosition, fHeight, fOutRadius, colIn, colOut, axisPlane);
        return;
    }

    FmVec3 vInCenter = vPosition;
    FmVec3 vOutCenter = vPosition;

    FmVec3* v = m_pVerticesBuffer;
    if (!v)
    {
        return;
    }
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            vInCenter.z += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i*2] = FmVec3( m_pSinList[i]*fInRadius, m_pCosList[i]*fInRadius, 0.0f );
                v[i*2] += vInCenter;
                v[i*2 + 1] = FmVec3( m_pSinList[i]*fOutRadius, m_pCosList[i]*fOutRadius, 0.0f );
                v[i*2 + 1] += vOutCenter;
            }
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            vInCenter.x += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i*2] = FmVec3( 0.0f, m_pSinList[i]*fInRadius, m_pCosList[i]*fInRadius );
                v[i*2] += vInCenter;
                v[i*2 + 1] = FmVec3( 0.0f, m_pSinList[i]*fOutRadius, m_pCosList[i]*fOutRadius );
                v[i*2 + 1] += vOutCenter;
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            vInCenter.y += fHeight;
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i*2] = FmVec3( m_pSinList[i]*fInRadius, 0.0f, m_pCosList[i]*fInRadius );
                v[i*2] += vInCenter;
                v[i*2 + 1] = FmVec3( m_pSinList[i]*fOutRadius, 0.0f, m_pCosList[i]*fOutRadius );
                v[i*2 + 1] += vOutCenter;
            }
        }
        break;
    }


    if (m_bWireMode)
    {
        for (unsigned int i = 0; i < m_nSlices; i++)
        {
            PushVertex(v[i*2], colIn, IRenderDrawOp::DRAW_LINES);
            PushVertex(v[i*2 + 1], colOut, IRenderDrawOp::DRAW_LINES);     

            PushVertex(v[i*2], colIn, IRenderDrawOp::DRAW_LINES);
            PushVertex(v[i*2 + 2], colIn, IRenderDrawOp::DRAW_LINES);  

            PushVertex(v[i*2 + 1], colOut, IRenderDrawOp::DRAW_LINES);  
            PushVertex(v[i*2 + 3], colOut, IRenderDrawOp::DRAW_LINES);  
        }
    }
    else
    {
        for (unsigned int i = 0; i <= m_nSlices; i++)
        {
            PushVertex(v[i*2], colIn, IRenderDrawOp::DRAW_TRIANGLE_STRIP);
            PushVertex(v[i*2 + 1], colOut, IRenderDrawOp::DRAW_TRIANGLE_STRIP);
        }
        FlushBatchData(IRenderDrawOp::DRAW_TRIANGLE_STRIP);
    }
}

void                    Primitive3DDrawer::DrawCylinder
(
    const FmVec3&       vPosition, 
    float               fHeight,
    float               fTopRadius,
    float               fBottomRadius,
    unsigned int        colTop,
    unsigned int        colBottom,
    YeAxis3DType        axisPlane
)
{
    if (fTopRadius < FLT_EPSILON)
    {
        DrawCone(vPosition, fHeight, fBottomRadius, colTop, colBottom, axisPlane);
        return;
    }

    DrawLoop(vPosition, fHeight, fTopRadius, fBottomRadius, colTop, colBottom, axisPlane);

    FmVec3 vTop = vPosition;
    FmVec3 vBottom = vPosition;
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            vTop.z += fHeight;
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            vTop.x += fHeight;
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            vTop.y += fHeight;
        }
        break;
    }
    DrawCircleRound(vTop, 0.0f, fTopRadius, colTop, colTop, axisPlane);
    DrawCircleRound(vBottom, 0.0f, fBottomRadius, colBottom, colBottom, axisPlane);    
}

void                    Primitive3DDrawer::DrawBox
(
    const FmVec3&       vMin,
    const FmVec3&       vMax,
    unsigned int        color
)
{
    FmVec3* v = m_pVerticesBuffer;
    v[0] = FmVec3(vMin.x, vMin.y, vMin.z);
    v[1] = FmVec3(vMax.x, vMin.y, vMin.z);
    v[2] = FmVec3(vMax.x, vMin.y, vMax.z);
    v[3] = FmVec3(vMin.x, vMin.y, vMax.z);
    v[4] = FmVec3(vMin.x, vMax.y, vMin.z);
    v[5] = FmVec3(vMax.x, vMax.y, vMin.z);
    v[6] = FmVec3(vMax.x, vMax.y, vMax.z);
    v[7] = FmVec3(vMin.x, vMax.y, vMax.z);

    DrawQuadrangle(v[0], v[1], v[2], v[3], color);
    DrawQuadrangle(v[4], v[5], v[6], v[7], color);

    DrawQuadrangle(v[0], v[1], v[5], v[4], color);
    DrawQuadrangle(v[2], v[3], v[7], v[6], color);

    DrawQuadrangle(v[1], v[2], v[6], v[5], color);
    DrawQuadrangle(v[3], v[0], v[4], v[7], color);
}

void                    Primitive3DDrawer::DrawBindingWall
(
    const FmVec3&       vMin,
    const FmVec3&       vMax,
    float               fWallHeight,
    unsigned int        colTop,
    unsigned int        colBottom
)
{
    float minY, maxY;
    if (fWallHeight > 0.0f)
    {
        minY = 0.0f;
        maxY = fWallHeight;
    }
    else
    {
        minY = vMin.y;
        maxY = vMax.y;
    }

    FmVec3* v = m_pVerticesBuffer;
    v[0] = FmVec3(vMin.x, minY, vMin.z);
    v[1] = FmVec3(vMax.x, minY, vMin.z);
    v[2] = FmVec3(vMax.x, minY, vMax.z);
    v[3] = FmVec3(vMin.x, minY, vMax.z);
    v[4] = FmVec3(vMin.x, maxY, vMin.z);
    v[5] = FmVec3(vMax.x, maxY, vMin.z);
    v[6] = FmVec3(vMax.x, maxY, vMax.z);
    v[7] = FmVec3(vMin.x, maxY, vMax.z);

    DrawQuadrangle(v[0], v[1], v[5], v[4], colBottom, colBottom, colTop, colTop);
    DrawQuadrangle(v[2], v[3], v[7], v[6], colBottom, colBottom, colTop, colTop);

    DrawQuadrangle(v[1], v[2], v[6], v[5], colBottom, colBottom, colTop, colTop);
    DrawQuadrangle(v[3], v[0], v[4], v[7], colBottom, colBottom, colTop, colTop);
}

void                    Primitive3DDrawer::DrawSphere
(
    const FmVec3&       vCenter,
    float               fRadius,
    unsigned int        color
)
{
    unsigned int stacks = m_nSlices / 2;
    if (stacks < 3)
    {
        return;
    }

    float lastRadius = 0.0f;
    float lastHeight = fRadius;
    float currentRadius;
    float currentHeight;
    FmVec3 vPosition;
    
    for (unsigned int i = 1; i <= stacks; i++)
    {
        currentRadius = fRadius * m_pSinList[i];
        currentHeight = fRadius * m_pCosList[i];

        vPosition = vCenter;
        vPosition.y += currentHeight;

        DrawLoop(vPosition, lastHeight - currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_XOZ);

        lastRadius = currentRadius;
        lastHeight = currentHeight;
    }

    if (lastHeight + fRadius > 0.01f)
    {
        DrawCircleRound(vPosition, -fRadius - lastHeight, lastRadius, color, color, YE_AXIS_3D_PLANE_XOZ);
    }
}

void                    Primitive3DDrawer::DrawHemisphere
(
    const FmVec3&       vCenter,
    float               fRadius,
    unsigned int        color,
    YeAxis3DType        axisDir
)
{
    unsigned int stacks = m_nSlices / 4;
    if (stacks < 2)
    {
        return;
    }

    float lastRadius = 0.0f;
    float lastHeight = fRadius;
    float currentRadius;
    float currentHeight;
    FmVec3 vPosition;

    switch(axisDir)
    {
    case YE_AXIS_3D_POSITIVE_X:
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.x += currentHeight;

                DrawLoop(vPosition, lastHeight - currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_YOZ);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_YOZ);
            }
        }
        break;
    case YE_AXIS_3D_NEGATIVE_X:
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.x -= currentHeight;

                DrawLoop(vPosition, -lastHeight + currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_YOZ);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, -lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_YOZ);
            }
        }
        break;
    case YE_AXIS_3D_POSITIVE_Z:
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.z += currentHeight;

                DrawLoop(vPosition, lastHeight - currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_XOY);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_XOY);
            }
        }
        break;
    case YE_AXIS_3D_NEGATIVE_Z:
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.z -= currentHeight;

                DrawLoop(vPosition, -lastHeight + currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_XOY);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, -lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_XOY);
            }
        }
        break;
    case YE_AXIS_3D_NEGATIVE_Y:
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.y -= currentHeight;

                DrawLoop(vPosition, -lastHeight + currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_XOZ);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, -lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_XOZ);
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_POSITIVE_Y
        {
            for (unsigned int i = 1; i <= stacks; i++)
            {
                currentRadius = fRadius * m_pSinList[i];
                currentHeight = fRadius * m_pCosList[i];

                vPosition = vCenter;
                vPosition.y += currentHeight;

                DrawLoop(vPosition, lastHeight - currentHeight, lastRadius, currentRadius, color, color, YE_AXIS_3D_PLANE_XOZ);

                lastRadius = currentRadius;
                lastHeight = currentHeight;
            }

            if (lastHeight > 0.01f)
            {
                DrawLoop(vCenter, lastHeight, lastRadius, fRadius, color, color, YE_AXIS_3D_PLANE_XOZ);
            }
        }
        break;
    }
}

void                    Primitive3DDrawer::DrawCapsule
(
    const FmVec3&       vCenter,
    float               fRadius,
    float               fHeight,
    unsigned int        color,
    YeAxis3DType        axisPlane
)
{
    float fHalfHeight = fHeight / 2;
    FmVec3 vTop = vCenter;
    FmVec3 vBottom = vCenter;
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            vTop.z += fHalfHeight;
            vBottom.z -= fHalfHeight;
            DrawHemisphere(vTop, fRadius, color, YE_AXIS_3D_POSITIVE_Z);
            DrawHemisphere(vBottom, fRadius, color, YE_AXIS_3D_NEGATIVE_Z);
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            vTop.x += fHalfHeight;
            vBottom.x -= fHalfHeight;
            DrawHemisphere(vTop, fRadius, color, YE_AXIS_3D_POSITIVE_X);
            DrawHemisphere(vBottom, fRadius, color, YE_AXIS_3D_NEGATIVE_X);
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            vTop.y += fHalfHeight;
            vBottom.y -= fHalfHeight;
            DrawHemisphere(vTop, fRadius, color, YE_AXIS_3D_POSITIVE_Y);
            DrawHemisphere(vBottom, fRadius, color, YE_AXIS_3D_NEGATIVE_Y);
        }
        break;
    }

    DrawLoop(vBottom, fHeight, fRadius, fRadius, color, color, axisPlane);
}

// 线图形
void                    Primitive3DDrawer::DrawQuadWire
(
    const FmVec3&       vCenter,
    float               fSize,
    unsigned int        color,
    YeAxis3DType        axisPlane
)
{
    FmVec3* v = m_pVerticesBuffer;
    float fHalf = fSize * 0.5f;
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            v[0] = FmVec3(-fHalf, -fHalf, 0.0f);
            v[1] = FmVec3(-fHalf,  fHalf, 0.0f);
            v[2] = FmVec3( fHalf,  fHalf, 0.0f);
            v[3] = FmVec3( fHalf, -fHalf, 0.0f);
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            v[0] = FmVec3(0.0f, -fHalf, -fHalf);
            v[1] = FmVec3(0.0f, -fHalf,  fHalf);
            v[2] = FmVec3(0.0f,  fHalf,  fHalf);
            v[3] = FmVec3(0.0f,  fHalf, -fHalf);
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            v[0] = FmVec3(-fHalf, 0.0f, -fHalf);
            v[1] = FmVec3(-fHalf, 0.0f,  fHalf);
            v[2] = FmVec3( fHalf, 0.0f,  fHalf);
            v[3] = FmVec3( fHalf, 0.0f, -fHalf);
        }
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        v[i] += vCenter;
    }

    DrawLine(v[0], v[1], color, color);
    DrawLine(v[1], v[2], color, color);
    DrawLine(v[2], v[3], color, color);
    DrawLine(v[3], v[0], color, color);
}

void                    Primitive3DDrawer::DrawCircleWire
(
    const FmVec3&       vCenter,
    float               fRadius,
    unsigned int        color,
    YeAxis3DType        axisPlane
)
{
    FmVec3* v = m_pVerticesBuffer;
    if (!v)
    {
        return;
    }
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadius, m_pCosList[i]*fRadius, 0.0f );
                v[i] += vCenter;
            }
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( 0.0f, m_pSinList[i]*fRadius, m_pCosList[i]*fRadius );
                v[i] += vCenter;
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadius, 0.0f, m_pCosList[i]*fRadius );
                v[i] += vCenter;
            }
        }
        break;
    }

    PushVertices(v, m_nSlices + 1, color, IRenderDrawOp::DRAW_LINE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_STRIP);
}

void                    Primitive3DDrawer::DrawEllipseWire
(
    const FmVec3&       vCenter,
    float               fRadiusW,
    float               fRadiusH,
    unsigned int        color,
    YeAxis3DType        axisPlane
)
{
    FmVec3* v = m_pVerticesBuffer;
    if (!v)
    {
        return;
    }
    switch(axisPlane)
    {
    case YE_AXIS_3D_PLANE_XOY:
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadiusW, m_pCosList[i]*fRadiusH, 0.0f );
                v[i] += vCenter;
            }
        }
        break;
    case YE_AXIS_3D_PLANE_YOZ:
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( 0.0f, m_pSinList[i]*fRadiusW, m_pCosList[i]*fRadiusH );
                v[i] += vCenter;
            }
        }
        break;
    default: // 默认为YE_AXIS_3D_PLANE_XOZ
        {
            for (unsigned int i = 0; i <= m_nSlices; i++)
            {
                v[i] = FmVec3( m_pSinList[i]*fRadiusW, 0.0f, m_pCosList[i]*fRadiusH );
                v[i] += vCenter;
            }
        }
        break;
    }

	PushVertices(v, m_nSlices + 1, color, IRenderDrawOp::DRAW_LINE_STRIP);
	FlushBatchData(IRenderDrawOp::DRAW_LINE_STRIP);
}


void                    Primitive3DDrawer::DrawBindingBox
(
    const FmVec3&       vMin,
    const FmVec3&       vMax,
    unsigned int        color
)
{
    FmVec3* v = m_pVerticesBuffer;
    v[0] = FmVec3(vMin.x, vMin.y, vMin.z);
    v[1] = FmVec3(vMax.x, vMin.y, vMin.z);
    v[2] = FmVec3(vMax.x, vMin.y, vMax.z);
    v[3] = FmVec3(vMin.x, vMin.y, vMax.z);
    v[4] = FmVec3(vMin.x, vMax.y, vMin.z);
    v[5] = FmVec3(vMax.x, vMax.y, vMin.z);
    v[6] = FmVec3(vMax.x, vMax.y, vMax.z);
    v[7] = FmVec3(vMin.x, vMax.y, vMax.z);

    DrawLine(v[0], v[1], color);
    DrawLine(v[1], v[2], color);
    DrawLine(v[2], v[3], color);
    DrawLine(v[3], v[0], color);

    DrawLine(v[4], v[5], color);
    DrawLine(v[5], v[6], color);
    DrawLine(v[6], v[7], color);
    DrawLine(v[7], v[4], color);

    DrawLine(v[0], v[4], color);
    DrawLine(v[1], v[5], color);
    DrawLine(v[2], v[6], color);
    DrawLine(v[3], v[7], color);
}

void                    Primitive3DDrawer::DrawSurroundBox
(
    const FmVec3&       vMin,
    const FmVec3&       vMax,
    unsigned int        color1,
    unsigned int        color2
)
{
    FmVec3* v = m_pVerticesBuffer;
    v[0] = FmVec3(vMin.x, vMin.y, vMin.z);
    v[1] = FmVec3(vMax.x, vMin.y, vMin.z);
    v[2] = FmVec3(vMax.x, vMin.y, vMax.z);
    v[3] = FmVec3(vMin.x, vMin.y, vMax.z);
    v[4] = FmVec3(vMin.x, vMax.y, vMin.z);
    v[5] = FmVec3(vMax.x, vMax.y, vMin.z);
    v[6] = FmVec3(vMax.x, vMax.y, vMax.z);
    v[7] = FmVec3(vMin.x, vMax.y, vMax.z);

    const float dec = 0.2F;
    float decX  = (vMax.x - vMin.x) * dec;
    float decY  = (vMax.y - vMin.y) * dec;
    float decZ  = (vMax.z - vMin.z) * dec;
    
    DrawLine(v[0], FmVec3(v[0].x + decX, v[0].y, v[0].z), color1, color2);
    DrawLine(v[0], FmVec3(v[0].x, v[0].y + decY, v[0].z), color1, color2);
    DrawLine(v[0], FmVec3(v[0].x, v[0].y, v[0].z + decZ), color1, color2);

    DrawLine(v[1], FmVec3(v[1].x - decX, v[1].y, v[1].z), color1, color2);
    DrawLine(v[1], FmVec3(v[1].x, v[1].y + decY, v[1].z), color1, color2);
    DrawLine(v[1], FmVec3(v[1].x, v[1].y, v[1].z + decZ), color1, color2);

    DrawLine(v[2], FmVec3(v[2].x - decX, v[2].y, v[2].z), color1, color2);
    DrawLine(v[2], FmVec3(v[2].x, v[2].y + decY, v[2].z), color1, color2);
    DrawLine(v[2], FmVec3(v[2].x, v[2].y, v[2].z - decZ), color1, color2);

    DrawLine(v[3], FmVec3(v[3].x + decX, v[3].y, v[3].z), color1, color2);
    DrawLine(v[3], FmVec3(v[3].x, v[3].y + decY, v[3].z), color1, color2);
    DrawLine(v[3], FmVec3(v[3].x, v[3].y, v[3].z - decZ), color1, color2);

    DrawLine(v[4], FmVec3(v[4].x + decX, v[4].y, v[4].z), color1, color2);
    DrawLine(v[4], FmVec3(v[4].x, v[4].y - decY, v[4].z), color1, color2);
    DrawLine(v[4], FmVec3(v[4].x, v[4].y, v[4].z + decZ), color1, color2);

    DrawLine(v[5], FmVec3(v[5].x - decX, v[5].y, v[5].z), color1, color2);
    DrawLine(v[5], FmVec3(v[5].x, v[5].y - decY, v[5].z), color1, color2);
    DrawLine(v[5], FmVec3(v[5].x, v[5].y, v[5].z + decZ), color1, color2);

    DrawLine(v[6], FmVec3(v[6].x - decX, v[6].y, v[6].z), color1, color2);
    DrawLine(v[6], FmVec3(v[6].x, v[6].y - decY, v[6].z), color1, color2);
    DrawLine(v[6], FmVec3(v[6].x, v[6].y, v[6].z - decZ), color1, color2);

    DrawLine(v[7], FmVec3(v[7].x + decX, v[7].y, v[7].z), color1, color2);
    DrawLine(v[7], FmVec3(v[7].x, v[7].y - decY, v[7].z), color1, color2);
    DrawLine(v[7], FmVec3(v[7].x, v[7].y, v[7].z - decZ), color1, color2);
}

void                    Primitive3DDrawer::DrawSurroundCircle
(
    const FmVec3&       vCenter,
    float               fRadius,
    unsigned int        colorXOZ,
    unsigned int        colorYOZ,
    unsigned int        colorXOY
)
{
    DrawCircleWire(vCenter, fRadius, colorXOZ, YE_AXIS_3D_PLANE_XOZ);
    DrawCircleWire(vCenter, fRadius, colorYOZ, YE_AXIS_3D_PLANE_YOZ);
    DrawCircleWire(vCenter, fRadius, colorXOY, YE_AXIS_3D_PLANE_XOY);
}

void                    Primitive3DDrawer::DrawSurroundEllipse
(
    const FmVec3&       vMin,
    const FmVec3&       vMax,
    unsigned int        colorXOZ,
    unsigned int        colorYOZ,
    unsigned int        colorXOY
)
{
    FmVec3 vCenter = (vMin + vMax) * 0.5f;
    FmVec3 vRadius = (vMax - vMin) * 0.5f;

    DrawEllipseWire(vCenter, vRadius.x, vRadius.z, colorXOZ, YE_AXIS_3D_PLANE_XOZ);
    DrawEllipseWire(vCenter, vRadius.y, vRadius.z, colorYOZ, YE_AXIS_3D_PLANE_YOZ);
    DrawEllipseWire(vCenter, vRadius.x, vRadius.y, colorXOY, YE_AXIS_3D_PLANE_XOY);
}

// 四面体
void                    Primitive3DDrawer::DrawTetrahedron
(
    const FmVec3*       pVertices,
    unsigned int        faceColor,
    unsigned int        wireColor
)
{
    if (!pVertices)
    {
        return;
    }
    const FmVec3* v = pVertices;

    if (faceColor)
    {
        DrawTriangle(v[0], v[1], v[2], faceColor);
        DrawTriangle(v[0], v[2], v[3], faceColor);
        DrawTriangle(v[0], v[3], v[1], faceColor);
        DrawTriangle(v[1], v[3], v[2], faceColor);
    }

    if (wireColor)
    {
        DrawLine(v[0], v[1], wireColor);
        DrawLine(v[0], v[2], wireColor);
        DrawLine(v[0], v[3], wireColor);
        DrawLine(v[1], v[2], wireColor);
        DrawLine(v[2], v[3], wireColor);
        DrawLine(v[3], v[1], wireColor);
    }
}

// 六面体
void                    Primitive3DDrawer::DrawHexahedron
(
    const FmVec3*       pVertices,
    unsigned int        faceColor,
    unsigned int        wireColor
)
{
    if (!pVertices)
    {
        return;
    }
    const FmVec3* v = pVertices;

    if (faceColor)
    {
        DrawQuadrangle(v[0], v[1], v[2], v[3], faceColor);
        DrawQuadrangle(v[4], v[5], v[6], v[7], faceColor);

        DrawQuadrangle(v[0], v[1], v[5], v[4], faceColor);
        DrawQuadrangle(v[2], v[3], v[7], v[6], faceColor);

        DrawQuadrangle(v[1], v[2], v[6], v[5], faceColor);
        DrawQuadrangle(v[3], v[0], v[4], v[7], faceColor);
    }

    if (wireColor)
    {
        DrawLine(v[0], v[1], wireColor);
        DrawLine(v[1], v[2], wireColor);
        DrawLine(v[2], v[3], wireColor);
        DrawLine(v[3], v[0], wireColor);

        DrawLine(v[4], v[5], wireColor);
        DrawLine(v[5], v[6], wireColor);
        DrawLine(v[6], v[7], wireColor);
        DrawLine(v[7], v[4], wireColor);

        DrawLine(v[0], v[4], wireColor);
        DrawLine(v[1], v[5], wireColor);
        DrawLine(v[2], v[6], wireColor);
        DrawLine(v[3], v[7], wireColor);
    }
}