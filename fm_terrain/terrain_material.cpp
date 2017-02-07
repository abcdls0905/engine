//--------------------------------------------------------------------
// 文件名:		terrain_material.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月28日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_material.h"
#include "terrain.h"

/// \file terrain_material.cpp
/// \brief 地形材质

/// entity: TerrainMaterial
/// \brief 地形材质实体
DECLARE_ENTITY(TerrainMaterial, 0, IEntity);

DECLARE_PROPERTY(float, TerrainMaterial, DiffuseAlpha, 
	GetDiffuseAlpha, SetDiffuseAlpha);
DECLARE_PROPERTY(float, TerrainMaterial, DiffuseRed, 
	GetDiffuseRed, SetDiffuseRed);
DECLARE_PROPERTY(float, TerrainMaterial, DiffuseGreen, 
	GetDiffuseGreen, SetDiffuseGreen);
DECLARE_PROPERTY(float, TerrainMaterial, DiffuseBlue, 
	GetDiffuseBlue, SetDiffuseBlue);

DECLARE_PROPERTY(float, TerrainMaterial, AmbientAlpha, 
	GetAmbientAlpha, SetAmbientAlpha);
DECLARE_PROPERTY(float, TerrainMaterial, AmbientRed, 
	GetAmbientRed, SetAmbientRed);
DECLARE_PROPERTY(float, TerrainMaterial, AmbientGreen, 
	GetAmbientGreen, SetAmbientGreen);
DECLARE_PROPERTY(float, TerrainMaterial, AmbientBlue, 
	GetAmbientBlue, SetAmbientBlue);

DECLARE_PROPERTY(float, TerrainMaterial, SpecularAlpha, 
	GetSpecularAlpha, SetSpecularAlpha);
DECLARE_PROPERTY(float, TerrainMaterial, SpecularRed, 
	GetSpecularRed, SetSpecularRed);
DECLARE_PROPERTY(float, TerrainMaterial, SpecularGreen, 
	GetSpecularGreen, SetSpecularGreen);
DECLARE_PROPERTY(float, TerrainMaterial, SpecularBlue, 
	GetSpecularBlue, SetSpecularBlue);

DECLARE_PROPERTY(float, TerrainMaterial, EmissiveAlpha, 
	GetEmissiveAlpha, SetEmissiveAlpha);
DECLARE_PROPERTY(float, TerrainMaterial, EmissiveRed, 
	GetEmissiveRed, SetEmissiveRed);
DECLARE_PROPERTY(float, TerrainMaterial, EmissiveGreen, 
	GetEmissiveGreen, SetEmissiveGreen);
DECLARE_PROPERTY(float, TerrainMaterial, EmissiveBlue, 
	GetEmissiveBlue, SetEmissiveBlue);

/// property: bool SpecularPower
/// \brief 镜面高光强度
DECLARE_PROPERTY(float, TerrainMaterial, SpecularPower, GetSpecularPower, 
	SetSpecularPower);
/// property: bool SpecularEnable
/// \brief 是否开启镜面高光
DECLARE_PROPERTY(bool, TerrainMaterial, SpecularEnable, GetSpecularEnable,
	SetSpecularEnable);

// TerrainMaterial

TerrainMaterial::TerrainMaterial()
{
	m_pTerrain = NULL;
}

TerrainMaterial::~TerrainMaterial()
{
}

bool TerrainMaterial::Init(const IVarList& args)
{
	return true;
}

bool TerrainMaterial::Shut()
{
	return true;
}

void TerrainMaterial::SetDiffuseAlpha(float value)
{
	m_pTerrain->GetMaterial().vDiffuse.w = value;
}

float TerrainMaterial::GetDiffuseAlpha() const
{
	return m_pTerrain->GetMaterial().vDiffuse.w;
}

void TerrainMaterial::SetDiffuseRed(float value)
{
	m_pTerrain->GetMaterial().vDiffuse.x = value;
}

float TerrainMaterial::GetDiffuseRed() const
{
	return m_pTerrain->GetMaterial().vDiffuse.x;
}

void TerrainMaterial::SetDiffuseGreen(float value)
{
	m_pTerrain->GetMaterial().vDiffuse.y = value;
}

float TerrainMaterial::GetDiffuseGreen() const
{
	return m_pTerrain->GetMaterial().vDiffuse.y;
}

void TerrainMaterial::SetDiffuseBlue(float value)
{
	m_pTerrain->GetMaterial().vDiffuse.z = value;
}

float TerrainMaterial::GetDiffuseBlue() const
{
	return m_pTerrain->GetMaterial().vDiffuse.z;
}

void TerrainMaterial::SetAmbientAlpha(float value)
{
	m_pTerrain->GetMaterial().vAmbient.w = value;
}

float TerrainMaterial::GetAmbientAlpha() const
{
	return m_pTerrain->GetMaterial().vAmbient.w;
}

void TerrainMaterial::SetAmbientRed(float value)
{
	m_pTerrain->GetMaterial().vAmbient.x = value;
}

float TerrainMaterial::GetAmbientRed() const
{
	return m_pTerrain->GetMaterial().vAmbient.x;
}

void TerrainMaterial::SetAmbientGreen(float value)
{
	m_pTerrain->GetMaterial().vAmbient.y = value;
}

float TerrainMaterial::GetAmbientGreen() const
{
	return m_pTerrain->GetMaterial().vAmbient.y;
}

void TerrainMaterial::SetAmbientBlue(float value)
{
	m_pTerrain->GetMaterial().vAmbient.z = value;
}

float TerrainMaterial::GetAmbientBlue() const
{
	return m_pTerrain->GetMaterial().vAmbient.z;
}

void TerrainMaterial::SetSpecularAlpha(float value)
{
	m_pTerrain->GetMaterial().vSpecular.w = value;
}

float TerrainMaterial::GetSpecularAlpha() const
{
	return m_pTerrain->GetMaterial().vSpecular.w;
}

void TerrainMaterial::SetSpecularRed(float value)
{
	m_pTerrain->GetMaterial().vSpecular.x = value;
}

float TerrainMaterial::GetSpecularRed() const
{
	return m_pTerrain->GetMaterial().vSpecular.x;
}

void TerrainMaterial::SetSpecularGreen(float value)
{
	m_pTerrain->GetMaterial().vSpecular.y = value;
}

float TerrainMaterial::GetSpecularGreen() const
{
	return m_pTerrain->GetMaterial().vSpecular.y;
}

void TerrainMaterial::SetSpecularBlue(float value)
{
	m_pTerrain->GetMaterial().vSpecular.z = value;
}

float TerrainMaterial::GetSpecularBlue() const
{
	return m_pTerrain->GetMaterial().vSpecular.z;
}

void TerrainMaterial::SetEmissiveAlpha(float value)
{
	m_pTerrain->GetMaterial().vEmissive.w = value;
}

float TerrainMaterial::GetEmissiveAlpha() const
{
	return m_pTerrain->GetMaterial().vEmissive.w;
}

void TerrainMaterial::SetEmissiveRed(float value)
{
	m_pTerrain->GetMaterial().vEmissive.x = value;
}

float TerrainMaterial::GetEmissiveRed() const
{
	return m_pTerrain->GetMaterial().vEmissive.x;
}

void TerrainMaterial::SetEmissiveGreen(float value)
{
	m_pTerrain->GetMaterial().vEmissive.y = value;
}

float TerrainMaterial::GetEmissiveGreen() const
{
	return m_pTerrain->GetMaterial().vEmissive.y;
}

void TerrainMaterial::SetEmissiveBlue(float value)
{
	m_pTerrain->GetMaterial().vEmissive.z = value;
}

float TerrainMaterial::GetEmissiveBlue() const
{
	return m_pTerrain->GetMaterial().vEmissive.z;
}

void TerrainMaterial::SetSpecularPower(float value)
{
	m_pTerrain->GetMaterial().fSpecularPower = value;
}

float TerrainMaterial::GetSpecularPower() const
{
	return m_pTerrain->GetMaterial().fSpecularPower;
}

void TerrainMaterial::SetSpecularEnable(bool value)
{
	m_pTerrain->GetMaterial().bSpecularEnable = value;
}

bool TerrainMaterial::GetSpecularEnable() const
{
	return m_pTerrain->GetMaterial().bSpecularEnable;
}
 