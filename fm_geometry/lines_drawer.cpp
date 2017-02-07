//--------------------------------------------------------------------
// 文件名:		lines_drawer.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2014年03月05日
// 创建人:		叶峰
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include <assert.h>
#include "lines_drawer.h"

/// entity: LinesDrawer
/// \brief 几何图形的渲染
/// \see 继承自IVisBase
DECLARE_ENTITY(LinesDrawer, 0, IVisBase);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, LinesDrawer, Visible, GetVisible, SetVisible);
/// property: bool ZEnable
/// \brief 是否深度测试
DECLARE_PROPERTY(bool, LinesDrawer, ZEnable, GetZEnable, SetZEnable);

// LinesDrawer

LinesDrawer::LinesDrawer()
{
	m_pRender = NULL;
    m_pShaderProgram = NULL;
    m_pVB = NULL;
    m_pSceneView = NULL;

    m_hMatWVP = -1;

    memset(m_listLinePoints, 0, sizeof(m_listLinePoints));
    memset(m_listLineFrames, 0, sizeof(m_listLineFrames));
	m_linesNum = 0;

	m_bVisible = true;
	m_bZEnable = true;
}

LinesDrawer::~LinesDrawer()
{

}

bool LinesDrawer::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}
    m_pSceneView = m_pRender->GetSceneView();

    IVertexShader* pVS = m_pRender->LoadVertexShader("design_line.vsh", "main", "");
    IPixelShader* pPS = m_pRender->LoadPixelShader("design_line.fsh", "main", "");
    const char* vb[] = {"iPos", "iDiffuse"};
    m_pShaderProgram = m_pRender->CreateShaderProgram(pVS, pPS, vb, 2);
    assert(m_pShaderProgram);
    m_hMatWVP = m_pShaderProgram->GetParamOp()->FindParamIdByName("c_mtxWVP");

    m_pVB = m_pRender->CreateDynamicVB(sizeof(LinePoint)*MAX_LINES_NUM*2, "LinesDrawer::m_pVB");

	return true;
}

bool LinesDrawer::Shut()
{    
    m_linesNum = 0;
    SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pShaderProgram);
	SAFE_RELEASE(m_pRender);
	
	return true;
}

// 设置当前场景视图
void LinesDrawer::SetSceneView(ISceneView* pSceneView)
{
    m_pSceneView = pSceneView;
}

// 获得当前场景视图
ISceneView* LinesDrawer::GetSceneView()
{
    return m_pSceneView;
}

void LinesDrawer::SetVisible(bool value)
{
	m_bVisible = value;
}

bool LinesDrawer::GetVisible() const
{
	return m_bVisible;
}

void LinesDrawer::SetZEnable(bool value)
{
	m_bZEnable = value;
}

bool LinesDrawer::GetZEnable() const
{
	return m_bZEnable;
}

void LinesDrawer::DrawLine(const LinePoint& ptStart, const LinePoint& ptEnd, int numFrames)
{
    if (!m_bVisible || m_linesNum >= MAX_LINES_NUM)
    {
        return;
    }

    m_listLinePoints[m_linesNum*2] = ptStart;
    m_listLinePoints[m_linesNum*2 + 1] = ptEnd;
    m_listLineFrames[m_linesNum] = numFrames;
    m_linesNum++;
}

void LinesDrawer::DrawLines(const LinePoint* listPoints, unsigned int numLines, int numFrames)
{
    if (!m_bVisible)
    {
        return;
    }

    if (m_linesNum + numLines >= MAX_LINES_NUM)
    {
        numLines = MAX_LINES_NUM - m_linesNum;
    }

    if (numLines > 0)
    {
        memcpy(m_listLinePoints+m_linesNum*2, listPoints, numLines*2*sizeof(LinePoint));
        for (unsigned int i = 0; i < numLines; i++)
        {
            m_listLineFrames[m_linesNum + i] = numFrames;
        }
        m_linesNum += numLines;
    }
}

void LinesDrawer::DrawLine(const FmVec3& ptStart, const FmVec3& ptEnd, unsigned int color, int numFrames)
{
    if (!m_bVisible || m_linesNum >= MAX_LINES_NUM)
    {
        return;
    }

    m_listLinePoints[m_linesNum*2].vPos = ptStart;
    m_listLinePoints[m_linesNum*2].diffuse = color;
    m_listLinePoints[m_linesNum*2 + 1].vPos = ptEnd;
    m_listLinePoints[m_linesNum*2 + 1].diffuse = color;
    m_listLineFrames[m_linesNum] = numFrames;
    m_linesNum++;
}

void LinesDrawer::DrawLines(const FmVec3* listPoints, unsigned int numLines, unsigned int color, int numFrames)
{
    if (!m_bVisible)
    {
        return;
    }

    if (m_linesNum + numLines >= MAX_LINES_NUM)
    {
        numLines = MAX_LINES_NUM - m_linesNum;
    }

    if (numLines > 0)
    {
        memcpy(m_listLinePoints+m_linesNum*2, listPoints, numLines*2*sizeof(LinePoint));
        for (unsigned int i = 0; i < numLines; i++)
        {
            m_listLinePoints[(m_linesNum + i)*2].vPos = listPoints[i*2];
            m_listLinePoints[(m_linesNum + i)*2].diffuse = color;
            m_listLinePoints[(m_linesNum + i)*2 + 1].vPos = listPoints[i*2 + 1];
            m_listLinePoints[(m_linesNum + i)*2 + 1].diffuse = color;
            m_listLineFrames[m_linesNum + i] = numFrames;
        }
        m_linesNum += numLines;
    }
}

void LinesDrawer::Realize()
{
	if (!m_bVisible || !m_linesNum)
	{
		return;
	}

    m_pSceneView->AddSolidBatch(RenderLines, this, "LinesDrawer::Realize");
}

void LinesDrawer::RenderLines(void* pdata)
{
	LinesDrawer* pthis = (LinesDrawer*)pdata;
    if (!pthis->m_bVisible || !pthis->m_linesNum)
    {
        pthis->m_linesNum = 0;
        return;
    }

    pthis->m_pRender->UpdateDynamicVB(pthis->m_pVB, 0, pthis->m_listLinePoints, sizeof(LinePoint)*pthis->m_linesNum*2);

    pthis->m_pShaderProgram->UsedShader();

    IRenderDrawOp* pRenderDrawOp = pthis->m_pRender->GetRenderDrawOp();
    pRenderDrawOp->SetVB(pthis->m_pVB->GetBuffer());

    pRenderDrawOp->EnableVertexAttribArray(0, true);
    pRenderDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(LinePoint), (void*)(0));
    pRenderDrawOp->EnableVertexAttribArray(1, true);
    pRenderDrawOp->SetVertexAttribPointer(1, 4, IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE, sizeof(LinePoint), (void*)(3 * sizeof(float)));

    IRenderContext* pContext = pthis->m_pRender->GetContext();	
    FmMat4 matWorld;
    FmMatrixIdentity(&matWorld);
    const FmVec3& refer_pos = pContext->GetReferPosition();
    matWorld._41 = -refer_pos.x;
    matWorld._42 = -refer_pos.y;
    matWorld._43 = -refer_pos.z;
    FmMat4 mtxWVP;
    FmMat4Multiply(&mtxWVP, &matWorld, 
        &pContext->GetCamera().mtxViewProjRelative);
    FmMat4Transpose(&mtxWVP, &mtxWVP);

    IShaderParamOp* pShaderParamOp = pthis->m_pShaderProgram->GetParamOp();
    pShaderParamOp->SetParamValue(pthis->m_hMatWVP, mtxWVP, false);

    IRenderStateOp* pStateOp = pthis->m_pShaderProgram->GetRenderStateOp();
    pStateOp->EnableDepthWrite(pthis->m_bZEnable);
    pStateOp->EnableDepthTest(pthis->m_bZEnable);

    pRenderDrawOp->DrawArrays(IRenderDrawOp::DRAW_LINES, 0, pthis->m_linesNum*2);

    pthis->Clear();
}

void LinesDrawer::Clear()
{
    unsigned int oldLinesNum = m_linesNum;
    m_linesNum = 0;

    for (unsigned int i = 0; i < oldLinesNum; i++)
    {
        if (m_listLineFrames[i] > 1)
        {
            DrawLine(m_listLinePoints[i*2], m_listLinePoints[i*2 + 1], m_listLineFrames[i] - 1);
        }
    }
}
