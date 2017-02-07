//--------------------------------------------------------------------
// 文件名:		Pyramid.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年8月6日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "pyramid.h"
#include "helper.h"
#include "../visual/i_camera.h"

Pyramid::Pyramid()
{
	m_bShowPyramid = false;
	m_uLineColor = 0xffffffff;
	m_fDepth = 0.5f;
	m_fFovy = FM_PIf / 4;
	m_fAspect = 800.0f / 600.0f;
	FmMatrixIdentity(&m_mtxWorld);
}

bool Pyramid::Update()
{
	return false;
}

void Pyramid::Realize(IRender* pRender)
{
	if (NULL == pRender)
	{
		return;
	}

	// 根据摄像机坐标和朝向计算世界矩阵
	ICamera* pCamera = GetCamera(m_camera);
	if (NULL == pCamera)
	{
		return;
	}

	FmVec3 cam_pos = pCamera->GetPosition();
	FmVec3 cam_angle = pCamera->GetAngle();

    FmMat4RotationYawPitchRoll(&m_mtxWorld, cam_angle.y, cam_angle.x, cam_angle.z);
	m_mtxWorld._41 = cam_pos.x;
	m_mtxWorld._42 = cam_pos.y;
	m_mtxWorld._43 = cam_pos.z;

	if (m_bShowPyramid)
	{
		pRender->DrawDesignLine(m_mtxWorld, PRIMITIVE_LINELIST, PYRAMID_LINE_COUNT - 1, 
			m_vertexBuffer + 2, sizeof(vertex_t));
	}

	pRender->DrawDesignLine(m_mtxWorld, PRIMITIVE_LINELIST, 1, 
		m_vertexBuffer, sizeof(vertex_t));

}

void Pyramid::SetCamera(const PERSISTID& value)
{
	m_camera = value;
	UpdateVertex();
}

PERSISTID Pyramid::GetCamera() const
{
	return m_camera;
}

void Pyramid::SetFocusDepth(float value)
{
	m_fDepth = value;
	UpdateVertex();
}

ICamera* Pyramid::GetCamera(const PERSISTID& id) const
{
	IEntity* pEntity = g_pCore->GetEntity(id);

	if (NULL == pEntity)
	{
		return NULL;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Camera"))
	{
		return NULL;
	}

	return (ICamera*)pEntity;
}

bool Pyramid::UpdateVertex()
{
	ICamera* pCamera = GetCamera(m_camera);
	if (NULL == pCamera)
	{
		return false;
	}

	FmVec3 target_local_pos;
	target_local_pos.x = 0;
	target_local_pos.y = 0;
	target_local_pos.z = m_fDepth;

	// 计算视锥侧面4条线与目标平面的4个交点

	float tan_half_fovy = tan(m_fFovy / 2);
	float fTargetPlaneHeightHalf = m_fDepth * tan_half_fovy;
	float fTargetPlaneWidthHalf = fTargetPlaneHeightHalf * m_fAspect;

	FmVec3 pos_left_top = target_local_pos;
	pos_left_top.x -= fTargetPlaneWidthHalf;
	pos_left_top.y += fTargetPlaneHeightHalf;
	FmVec3 pos_right_top = target_local_pos;
	pos_right_top.x += fTargetPlaneWidthHalf;
	pos_right_top.y += fTargetPlaneHeightHalf;
	FmVec3 pos_left_down = target_local_pos;
	pos_left_down.x -= fTargetPlaneWidthHalf;
	pos_left_down.y -= fTargetPlaneHeightHalf;
	FmVec3 pos_right_down = target_local_pos;
	pos_right_down.x += fTargetPlaneWidthHalf;
	pos_right_down.y -= fTargetPlaneHeightHalf;

	FmVec3 cam_local_pos;
	cam_local_pos.x = 0;
	cam_local_pos.y = 0;
	cam_local_pos.z = 0;

	// 设置目标线的顶点
	m_vertexBuffer[0].pos = cam_local_pos;
	m_vertexBuffer[0].color = m_uLineColor;
	m_vertexBuffer[1].pos = target_local_pos;
	m_vertexBuffer[1].color = m_uLineColor;

	// 设置视锥体的顶点
	m_vertexBuffer[2].pos = cam_local_pos;
	m_vertexBuffer[2].color = m_uLineColor;
	m_vertexBuffer[3].pos = pos_left_top;
	m_vertexBuffer[3].color = m_uLineColor;

	m_vertexBuffer[4].pos = cam_local_pos;
	m_vertexBuffer[4].color = m_uLineColor;
	m_vertexBuffer[5].pos = pos_right_top;
	m_vertexBuffer[5].color = m_uLineColor;

	m_vertexBuffer[6].pos = cam_local_pos;
	m_vertexBuffer[6].color = m_uLineColor;
	m_vertexBuffer[7].pos = pos_left_down;
	m_vertexBuffer[7].color = m_uLineColor;

	m_vertexBuffer[8].pos = cam_local_pos;
	m_vertexBuffer[8].color = m_uLineColor;
	m_vertexBuffer[9].pos = pos_right_down;
	m_vertexBuffer[9].color = m_uLineColor;

	m_vertexBuffer[10].pos = pos_left_top;
	m_vertexBuffer[10].color = m_uLineColor;
	m_vertexBuffer[11].pos = pos_right_top;
	m_vertexBuffer[11].color = m_uLineColor;

	m_vertexBuffer[12].pos = pos_left_top;
	m_vertexBuffer[12].color = m_uLineColor;
	m_vertexBuffer[13].pos = pos_left_down;
	m_vertexBuffer[13].color = m_uLineColor;

	m_vertexBuffer[14].pos = pos_right_top;
	m_vertexBuffer[14].color = m_uLineColor;
	m_vertexBuffer[15].pos = pos_right_down;
	m_vertexBuffer[15].color = m_uLineColor;

	m_vertexBuffer[16].pos = pos_left_down;
	m_vertexBuffer[16].color = m_uLineColor;
	m_vertexBuffer[17].pos = pos_right_down;
	m_vertexBuffer[17].color = m_uLineColor;

    // 两条目标点的交叉线
    const float r = 0.05f;
    pos_left_top = target_local_pos;
    pos_left_top.x -= fTargetPlaneWidthHalf*r;
    pos_left_top.y += fTargetPlaneHeightHalf*r;
    pos_right_top = target_local_pos;
    pos_right_top.x += fTargetPlaneWidthHalf*r;
    pos_right_top.y += fTargetPlaneHeightHalf*r;
    pos_left_down = target_local_pos;
    pos_left_down.x -= fTargetPlaneWidthHalf*r;
    pos_left_down.y -= fTargetPlaneHeightHalf*r;
    pos_right_down = target_local_pos;
    pos_right_down.x += fTargetPlaneWidthHalf*r;
    pos_right_down.y -= fTargetPlaneHeightHalf*r;

    m_vertexBuffer[18].pos = pos_left_top;
    m_vertexBuffer[18].color = m_uLineColor;
    m_vertexBuffer[19].pos = pos_right_down;
    m_vertexBuffer[19].color = m_uLineColor;

    m_vertexBuffer[20].pos = pos_left_down;
    m_vertexBuffer[20].color = m_uLineColor;
    m_vertexBuffer[21].pos = pos_right_top;
    m_vertexBuffer[21].color = m_uLineColor;

	return true;
}

void Pyramid::SetFovy(float value)
{
	if (m_fFovy != value)
	{
		m_fFovy = value;
		UpdateVertex();
	}
}

void Pyramid::SetAspect(float value)
{
	if (m_fAspect != value)
	{
		m_fAspect = value;
		UpdateVertex();
	}
}
