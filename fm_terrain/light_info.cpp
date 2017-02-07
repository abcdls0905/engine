//--------------------------------------------------------------------
// 文件名:		light_info.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "light_info.h"

/// \file light_info.cpp
/// \brief 光源参数

/// entity: LightInfo
/// \brief 光源参数实体
DECLARE_ENTITY("", LightInfo, IEntity);

/// property: string Name
/// \brief 名称
DECLARE_PROPERTY(const char*, LightInfo, Name, GetName, SetName);
/// property: bool TurnOn
/// \brief 是否使用
DECLARE_PROPERTY(bool, LightInfo, TurnOn, GetTurnOn, SetTurnOn);
/// property: string LightType
/// \brief 光源类型（"point", "spot", "box"）
DECLARE_PROPERTY(const char*, LightInfo, LightType, GetLightTypeString, 
	SetLightTypeString);
/// property: float Intensity
/// \brief 光强度
DECLARE_PROPERTY(float, LightInfo, Intensity, GetIntensity, SetIntensity);
/// property: float ColorRed
/// \brief 红色分量
DECLARE_PROPERTY(float, LightInfo, ColorRed, GetColorRed, SetColorRed);
/// property: float ColorGreen
/// \brief 绿色分量
DECLARE_PROPERTY(float, LightInfo, ColorGreen, GetColorGreen, SetColorGreen);
/// property: float ColorBlue
/// \brief 蓝色分量
DECLARE_PROPERTY(float, LightInfo, ColorBlue, GetColorBlue, SetColorBlue);
/// property: float Cosine
/// \brief 角度余弦
DECLARE_PROPERTY(float, LightInfo, Cosine, GetCosine, SetCosine);
/// property: float Shadow
/// \brief 投影强度
DECLARE_PROPERTY(float, LightInfo, Shadow, GetShadow, SetShadow);
/// property: float Bright
/// \brief 亮度
DECLARE_PROPERTY(float, LightInfo, Bright, GetBright, SetBright);
/// property: float Contrast
/// \brief 对比度
DECLARE_PROPERTY(float, LightInfo, Contrast, GetContrast, SetContrast);
/// property: float Gamma
/// \brief Gamma修正
DECLARE_PROPERTY(float, LightInfo, Gamma, GetGamma, SetGamma);
/// property: float Attenu0
/// \brief 衰减系数0
DECLARE_PROPERTY(float, LightInfo, Attenu0, GetAttenu0, SetAttenu0);
/// property: float Attenu1
/// \brief 衰减系数1
DECLARE_PROPERTY(float, LightInfo, Attenu1, GetAttenu1, SetAttenu1);
/// property: float Attenu2
/// \brief 衰减系数2
DECLARE_PROPERTY(float, LightInfo, Attenu2, GetAttenu2, SetAttenu2);
/// property: float Range
/// \brief 影响范围
DECLARE_PROPERTY(float, LightInfo, Range, GetRange, SetRange);
/// property: float InnerDegree
/// \brief 聚光灯内角
DECLARE_PROPERTY(float, LightInfo, InnerDegree, GetInnerDegree, 
	SetInnerDegree);
/// property: float OuterDegree
/// \brief 聚光灯外角
DECLARE_PROPERTY(float, LightInfo, OuterDegree, GetOuterDegree, 
	SetOuterDegree);
/// property: float Falloff
/// \brief 聚光灯FALLOFF
DECLARE_PROPERTY(float, LightInfo, Falloff, GetFalloff, SetFalloff);
/// property: float PosiX
/// \brief 位置坐标X
DECLARE_PROPERTY(float, LightInfo, PosiX, GetPosiX, SetPosiX);
/// property: float PosiY
/// \brief 位置坐标Y
DECLARE_PROPERTY(float, LightInfo, PosiY, GetPosiY, SetPosiY);
/// property: float PosiZ
/// \brief 位置坐标Z
DECLARE_PROPERTY(float, LightInfo, PosiZ, GetPosiZ, SetPosiZ);
/// property: float AngleX
/// \brief 角度X
DECLARE_PROPERTY(float, LightInfo, AngleX, GetAngleX, SetAngleX);
/// property: float AngleY
/// \brief 角度Y
DECLARE_PROPERTY(float, LightInfo, AngleY, GetAngleY, SetAngleY);
/// property: float AngleZ
/// \brief 角度Z
DECLARE_PROPERTY(float, LightInfo, AngleZ, GetAngleZ, SetAngleZ);
/// property: float SizeX
/// \brief 盒子尺寸X
DECLARE_PROPERTY(float, LightInfo, SizeX, GetSizeX, SetSizeX);
/// property: float SizeY
/// \brief 盒子尺寸Y
DECLARE_PROPERTY(float, LightInfo, SizeY, GetSizeY, SetSizeY);
/// property: float SizeZ
/// \brief 盒子尺寸Z
DECLARE_PROPERTY(float, LightInfo, SizeZ, GetSizeZ, SetSizeZ);

// LightInfo

LightInfo::LightInfo()
{
	memset(&m_Data, 0, sizeof(m_Data));

	m_Data.bTurnOn = true;
	m_Data.nType = light_t::t_point;
}

LightInfo::~LightInfo()
{
}

bool LightInfo::Init(const IVarList& args)
{
	return true;
}

bool LightInfo::Shut()
{
	return true;
}

void LightInfo::SetName(const char* value)
{
	Assert(value != NULL);

	m_strName = value;
}

const char* LightInfo::GetName() const
{
	return m_strName.c_str();
}

void LightInfo::SetLightTypeString(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "point") == 0)
	{
		m_Data.nType = light_t::t_point;
	}
	else if (stricmp(value, "spot") == 0)
	{
		m_Data.nType = light_t::t_spot;
	}
	else if (stricmp(value, "box") == 0)
	{
		m_Data.nType = light_t::t_box;
	}
}

const char* LightInfo::GetLightTypeString() const
{
	switch (m_Data.nType)
	{
	case light_t::t_point:
		return "point";
	case light_t::t_spot:
		return "spot";
	case light_t::t_box:
		return "box";
	default:
		Assert(0);
		break;
	}

	return "";
}
*/
