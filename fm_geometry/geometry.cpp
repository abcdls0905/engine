//--------------------------------------------------------------------
// �ļ���:		geometry.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��11��25��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include <assert.h>
#include "geometry_builder.h"
#include "geometry.h"

/// entity: Geometry
/// \brief ����ͼ�ε���Ⱦ
/// \see �̳���IVisBase
DECLARE_ENTITY(Geometry, 0, IVisBase);

/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, Geometry, Visible, GetVisible, SetVisible);
/// property: bool ZEnable
/// \brief �Ƿ���Ȳ���
DECLARE_PROPERTY(bool, Geometry, ZEnable, GetZEnable, SetZEnable);
/// property: bool LineMode
/// \brief �Ƿ���ģʽ
DECLARE_PROPERTY(bool, Geometry, LineMode, GetLineMode, SetLineMode);

#define VERTEX_ARRAY	0
#define NORMAL_ARRAY	1

const char* geometry_vertex_t_name[] = 
{
	"iPos",
	"iNor",
};

// Geometry

Geometry::Geometry()
{
	m_pRender = NULL;
    m_pSceneView = NULL;

	m_pVB = NULL;
	m_pTrianglesIB = NULL;
	m_pLinesIB = NULL;

	m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_vAngle = FmVec3(0.0F, 0.0F, 0.0F);
	m_vScale = FmVec3(1.0F, 1.0F, 1.0F);

	m_pShaderProgram = NULL;

	m_vColor.x = 1.0f;
	m_vColor.y = 1.0f;
	m_vColor.z = 1.0f;
	m_vColor.w = 1.0f;

	m_bVisible = false;
	m_bLineMode = true;
	m_bZEnable = true;

    m_listBatchData = NULL;
    m_nBatchCount = 0;
}

Geometry::~Geometry()
{
    BatchData* ptr = m_listBatchData;
    BatchData* pNext;
    while (ptr)
    {
        pNext = ptr->pNext;
        assert(ptr->pShader == 0);
        CORE_DELETE(ptr);
        ptr = pNext;
    }
    m_listBatchData = NULL;
    m_nBatchCount = 0;

	Unbuild();
}

bool Geometry::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	}
    m_pSceneView = m_pRender->GetSceneView();

	IVertexShader* pVS = m_pRender->LoadVertexShader("geometry.vsh", 
		"vs_main_geometry", "");
	IPixelShader* pPS = m_pRender->LoadPixelShader("geometry.fsh", 
		"ps_main_geometry", "");
	m_pShaderProgram = m_pRender->CreateShaderProgram(
		pVS, pPS, geometry_vertex_t_name, 2);

	m_hDiffuse = m_pShaderProgram->GetParamOp()->FindParamIdByName("c_vDiffuse");
	m_hLightDirection = m_pShaderProgram->GetParamOp()->FindParamIdByName("c_vLightDirection");
	m_hMatWVP = m_pShaderProgram->GetParamOp()->FindParamIdByName("c_mtxWVP");
	m_hMatWorld = m_pShaderProgram->GetParamOp()->FindParamIdByName("c_mtxWorld");

	return true;
}

bool Geometry::Shut()
{    
    BatchData* ptr = m_listBatchData;
    while (ptr)
    {
        ptr->pShader = 0;
        ptr = ptr->pNext;
    }

	SAFE_RELEASE(m_pShaderProgram);
	SAFE_RELEASE(m_pRender);
	
	return true;
}

// ���õ�ǰ������ͼ
void Geometry::SetSceneView(ISceneView* pSceneView)
{
    m_pSceneView = pSceneView;
}

// ��õ�ǰ������ͼ
ISceneView* Geometry::GetSceneView()
{
    return m_pSceneView;
}

bool Geometry::SetPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;

	UpdateTransform();
	return true;
}

FmVec3 Geometry::GetPosition() const
{ 
	return m_vPosition; 
}

bool Geometry::SetAngle(float x, float y, float z)
{
	m_vAngle.x = x;
	m_vAngle.y = y;
	m_vAngle.z = z;

	UpdateTransform();
	return true;
}

FmVec3 Geometry::GetAngle() const
{ 
	return m_vAngle; 
}

// ����
bool Geometry::SetScale(float x, float y, float z)
{
	m_vScale.x = x;
	m_vScale.y = y;
	m_vScale.z = z;

	UpdateTransform();
	return true;
}

// �任����
bool Geometry::SetTransform(const FmMat4& mtx)
{
	m_matWorld = mtx;
	FmQuat quat;
	FmMat4Decompose(&m_vScale,&quat,&m_vPosition,&m_matWorld);
	FmQuatRotationToYawPitchRoll(&quat,&m_vAngle.y,&m_vAngle.x,&m_vAngle.z);
	return true;
}

const FmMat4& Geometry::GetTransform() const
{
	return m_matWorld;
}

FmVec3 Geometry::GetScale() const
{
	return m_vScale;
}

void Geometry::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Geometry::GetVisible() const
{
	return m_bVisible;
}

// �Ƿ���ģʽ
void Geometry::SetLineMode(bool value)
{
	m_bLineMode = value;
}

bool Geometry::GetLineMode() const
{
	return m_bLineMode;
}

void Geometry::SetZEnable(bool value)
{
	m_bZEnable = value;
}

bool Geometry::GetZEnable() const
{
	return m_bZEnable;
}

// ��ɫ
void Geometry::SetColor(unsigned int value)
{
	m_vColor = VisUtil_ColorToVector4(value);
}

unsigned int Geometry::GetColor() const
{
	return VisUtil_Vector4ToColor(m_vColor);
}

void Geometry::UpdateTransform()
{
	// ����
	FmMat4Scaling(&m_matWorld, m_vScale.x, m_vScale.y, m_vScale.z);

	// ��ת
	FmMat4 mtxRotate;
	FmMat4RotationYawPitchRoll(&mtxRotate, m_vAngle.y, m_vAngle.x, m_vAngle.z);
	FmMat4Multiply(&m_matWorld, &m_matWorld, &mtxRotate);

	// λ��
	m_matWorld._41 = m_vPosition.x;
	m_matWorld._42 = m_vPosition.y;
	m_matWorld._43 = m_vPosition.z;
}

bool Geometry::Build(const YsGraphDesc& graphDesc)
{
	Unbuild();
	unsigned int numVertices;        // ������Ŀ
	unsigned int numTriangles;       // ��������Ŀ
	unsigned int numLines;           // �߶���Ŀ
	if (!YfCalculateGeometryBufferSize(graphDesc, numVertices, numTriangles, numLines))
	{
		return false;
	}

	void* vb = malloc(numVertices*sizeof(geometry_vertex_t));
	void* triangles_ib = malloc(3*numTriangles*sizeof(unsigned short));
	void* lines_ib = malloc(2*numLines*sizeof(unsigned short));
	YsGeometryDesc geometryDesc;
	geometryDesc.vertexStriding = sizeof(geometry_vertex_t);

	if (!YfBuildGeometry(graphDesc, geometryDesc, vb, triangles_ib, lines_ib))
	{
		free(vb);
		free(triangles_ib);
		free(lines_ib);
		return false;
	}

	YfUpdateGeometryNormal(geometryDesc, sizeof(FmVec3), vb, numVertices, triangles_ib, numTriangles);

	// �������㻺��
	m_pVB = m_pRender->CreateStaticVB(vb, numVertices*sizeof(geometry_vertex_t));

	// ������������������
	m_pTrianglesIB = m_pRender->CreateStaticIB(triangles_ib, 3*numTriangles*sizeof(unsigned short));

	// ��������������
	m_pLinesIB = m_pRender->CreateStaticIB(lines_ib, 2*numLines*sizeof(unsigned short));

	free(vb);
	free(triangles_ib);
	free(lines_ib);

	return true;
}

void Geometry::Unbuild()
{    
    BatchData* ptr = m_listBatchData;
    while (ptr)
    {
        ptr->pShader = 0;
        ptr = ptr->pNext;
    }

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pTrianglesIB);
	SAFE_RELEASE(m_pLinesIB);
}

bool Geometry::IsReady() const
{
	return (m_pSceneView && m_pVB && m_pTrianglesIB && m_pLinesIB);
}

void Geometry::Realize()
{
	if (!m_bVisible || !IsReady())
	{
		return;
	}

    BatchData* pBatchData = GetBatchData();
    UpdateBatchData(pBatchData, m_matWorld);

    if (pBatchData->c_vDiffuse.w < 0.99f)
    {
        // �ӿռ�ľ��룬���ǰ�͸������������������
        IRenderContext* pContext = m_pRender->GetContext();
        const FmMat4& mtxView = pContext->GetCamera().mtxView;
        float camera_z = mtxView._13 * m_vPosition.x + mtxView._23 * m_vPosition.y 
            + mtxView._33 * m_vPosition.z + mtxView._43;

        // ��͸����Ⱦ
        m_pSceneView->AddBlendBatch(RenderGeometry, pBatchData, camera_z, "Geometry::Realize");
    }
    else
    {
        // ��͸����Ⱦ
        m_pSceneView->AddSolidBatch(RenderGeometry, pBatchData, "Geometry::Realize");
    }
}

// ��Ⱦ
void Geometry::Draw(const FmMat4& mtxWorld)
{
    if (!IsReady())
    {
        return;
    }

    BatchData* pBatchData = GetBatchData();
    UpdateBatchData(pBatchData, mtxWorld);

    if (pBatchData->c_vDiffuse.w < 0.99f)
    {
        // �ӿռ�ľ��룬���ǰ�͸������������������
        IRenderContext* pContext = m_pRender->GetContext();
        const FmMat4& mtxView = pContext->GetCamera().mtxView;
        float camera_z = mtxView._13 * mtxWorld._41 + mtxView._23 * mtxWorld._42 
            + mtxView._33 * mtxWorld._43 + mtxView._43;

        // ��͸����Ⱦ
        m_pSceneView->AddBlendBatch(RenderGeometry, pBatchData, camera_z, "Geometry::Draw");
    }
    else
    {
        // ��͸����Ⱦ
        m_pSceneView->AddSolidBatch(RenderGeometry, pBatchData, "Geometry::Draw");
    }
}

void Geometry::RenderGeometry(void* pdata)
{
	BatchData* pthis = (BatchData*)pdata;
    if (!pthis->pShader)
    {
        return;
    }
    pthis->pShader->UsedShader();

    IShaderParamOp* pShader = pthis->pShader->GetParamOp();
	pShader->SetParamValue(pthis->hDiffuse,pthis->c_vDiffuse,false);
	pShader->SetParamValue(pthis->hLightDirection,pthis->c_vLightDirection,false);
	pShader->SetParamValue(pthis->hMatWVP,pthis->c_matWVP,false);
	pShader->SetParamValue(pthis->hMatWorld,pthis->c_matWorld,false);

    IRenderDrawOp*  pDrawOp = pthis->pShader->GetRenderDrawOp();
	pDrawOp->SetVB(pthis->pVBuffer->GetBuffer());
	pDrawOp->SetIB(pthis->pIBuffer->GetBuffer());
	pDrawOp->EnableVertexAttribArray(VERTEX_ARRAY,true);
	pDrawOp->SetVertexAttribPointer(VERTEX_ARRAY, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(geometry_vertex_t), 0);
	pDrawOp->EnableVertexAttribArray(NORMAL_ARRAY,true);
	pDrawOp->SetVertexAttribPointer(NORMAL_ARRAY, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(geometry_vertex_t), (void*)(3 * sizeof(float)));

    IRenderStateOp* pStateOp = pthis->pShader->GetRenderStateOp();
    pStateOp->EnableDepthWrite(pthis->bZEnable);
    pStateOp->EnableDepthTest(pthis->bZEnable);

    fm_sizei count = pthis->pIBuffer->GetSize() / sizeof(unsigned short);

	IRenderDrawOp::DRAW_MODE drawMode = pthis->bLineMode ? IRenderDrawOp::DRAW_LINES : IRenderDrawOp::DRAW_TRIANGLES;
	pDrawOp->DrawIndex(drawMode,count,IRenderDrawOp::VERTEX_INDEX_UNSIGNED_SHORT,&((fm_short*)0)[0]);

    pthis->pShader = NULL;
}

void Geometry::UpdateBatchData(BatchData* pBatchData, const FmMat4& mtxWorld)
{
    IRenderContext* pContext = m_pRender->GetContext();	 

    pBatchData->pVBuffer = m_pVB;
    pBatchData->pIBuffer = m_bLineMode ? m_pLinesIB : m_pTrianglesIB;
    pBatchData->pShader = m_pShaderProgram;

    // ����任����
    pBatchData->c_matWorld = mtxWorld;
    const FmVec3& refer_pos = pContext->GetReferPosition();    // �������ο�λ��
    pBatchData->c_matWorld._41 -= refer_pos.x;
    pBatchData->c_matWorld._42 -= refer_pos.y;
    pBatchData->c_matWorld._43 -= refer_pos.z;

    // WVP����
    FmMat4Multiply(&pBatchData->c_matWVP, &pBatchData->c_matWorld, 
        &pContext->GetCamera().mtxViewProjRelative);
    FmMat4Transpose(&pBatchData->c_matWVP, &pBatchData->c_matWVP);

    // ��ɫ
    pBatchData->c_vDiffuse = m_vColor;

    // ��ķ���
    FmVec3 lightDir = pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);	
    FmVec3Normalize(&lightDir, &lightDir);
    pBatchData->c_vLightDirection.x = lightDir.x;
    pBatchData->c_vLightDirection.y = lightDir.y;
    pBatchData->c_vLightDirection.z = lightDir.z;
    pBatchData->c_vLightDirection.w = 0.0f;

    pBatchData->hDiffuse = m_hDiffuse;
    pBatchData->hLightDirection = m_hLightDirection;
    pBatchData->hMatWVP = m_hMatWVP;
    pBatchData->hMatWorld = m_hMatWorld;

    pBatchData->bZEnable = m_bZEnable;
    pBatchData->bLineMode = m_bLineMode;
}

Geometry::BatchData* Geometry::GetBatchData()
{
    if (!m_listBatchData)
    {
        m_listBatchData = CORE_NEW(BatchData);
        m_nBatchCount = 1;
        return m_listBatchData;
    }

    BatchData* ptr = m_listBatchData;
    while (true)
    {
        if (ptr->pShader == 0)
        {
            return ptr;
        }

        if (ptr->pNext)
        {
            ptr = ptr->pNext;
        }
        else
        {
            ptr->pNext = CORE_NEW(BatchData);
            m_nBatchCount++;
            return ptr->pNext;
        }
    }
}