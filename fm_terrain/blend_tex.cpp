//--------------------------------------------------------------------
// 文件名:		blend_tex.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "blend_tex.h"
#include "terrain.h"
#include "../visual/i_texture.h"

// CBlendTex

CBlendTex::CBlendTex()
{
	m_nHash = 0;
	m_pTex = NULL;
	m_pNormalMap = NULL;
	m_pHeightMap = NULL;
	m_fSpecularLevel = 0.0F;
	m_fSpecularPower = 0.0F;
	m_fHeightScale = 0.0F;
}

CBlendTex::~CBlendTex()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pNormalMap);
	SAFE_RELEASE(m_pHeightMap);
}

bool CBlendTex::AddFormat(unsigned int scale_u, unsigned int scale_v, 
	unsigned int angle_x, unsigned int angle_y, unsigned int angle_z)
{
	if (m_Formats.size() >= 255)
	{
		return false;
	}
	
	format_t data;
	
	data.nScaleU = scale_u;
	data.nScaleV = scale_v;
	data.nAngleX = angle_x;
	data.nAngleY = angle_y;
	data.nAngleZ = angle_z;

	m_Formats.push_back(data);

	//UpdateMatrix(m_Formats.size() - 1);
	
	return true;
}

bool CBlendTex::UpdateMatrix(size_t index, Terrain* pTerrain)
{
	Assert(index < m_Formats.size());
	
	format_t* p = &m_Formats[index];
	
	FmMat4 mtxRotate;
	FmMat4 mtxScale;

	FmMat4RotationYawPitchRoll(&mtxRotate, 
		((float)p->nAngleX + 90.0F) / 180.0F * FLOAT_PI, 
		((float)p->nAngleY + 90.0F) / 180.0F * FLOAT_PI, 
		((float)p->nAngleZ + 90.0F) / 180.0F * FLOAT_PI);

	float fUnit = pTerrain->GetTextureUnits() * pTerrain->GetUnitSize();

	FmMat4Scaling(&mtxScale, 
		(float)p->nScaleU * 0.01F / fUnit, 
		(float)p->nScaleV * 0.01F / fUnit, 
		1.0F);
	FmMat4Multiply(&p->mtxTex, &mtxRotate, &mtxScale);

	/*
	// 角度
	float angle_x = float(p->nAngleX) / 360.0F * FLOAT_PI * 2.0F; 
	float angle_y = float(p->nAngleY) / 360.0F * FLOAT_PI * 2.0F; 
	float angle_z = float(p->nAngleZ) / 360.0F * FLOAT_PI * 2.0F; 
	
	// 密度
	float scale_u = float(p->nScaleU) * 0.1F; 
	float scale_v = float(p->nScaleV) * 0.1F;
	
	// 偏移
	float offset_u = 0.0F;
	float offset_v = 0.0F;

	FmMat4 rotate_x;
	FmMat4 rotate_y;
	FmMat4 rotate_z;

	FmMat4 scale;
	FmMat4 translate;
	
	FmMat4Identity(&scale);
	FmMat4Identity(&translate);
	
	FmMat4RotationX(&rotate_x, angle_x);
	FmMat4RotationY(&rotate_y, angle_y);
	FmMat4RotationZ(&rotate_z, angle_z);
	
	scale._11 = scale_u; 
	scale._22 = scale_v;

	translate._31 = offset_u; 
	translate._32 = offset_v;

	FmMat4Multiply(&p->matTex, &rotate_x, &rotate_y);
	FmMat4Multiply(&p->matTex, &p->matTex, &rotate_z);
	FmMat4Multiply(&p->matTex, &p->matTex, &scale);
	FmMat4Multiply(&p->matTex, &p->matTex, &translate);
	*/
	
	return true;
}

