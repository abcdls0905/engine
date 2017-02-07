//--------------------------------------------------------------------
// �ļ���:		light_info.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#include "light_info.h"

/// \file light_info.cpp
/// \brief ��Դ����

/// entity: LightInfo
/// \brief ��Դ����ʵ��
DECLARE_ENTITY("", LightInfo, IEntity);

/// property: string Name
/// \brief ����
DECLARE_PROPERTY(const char*, LightInfo, Name, GetName, SetName);
/// property: bool TurnOn
/// \brief �Ƿ�ʹ��
DECLARE_PROPERTY(bool, LightInfo, TurnOn, GetTurnOn, SetTurnOn);
/// property: string LightType
/// \brief ��Դ���ͣ�"point", "spot", "box"��
DECLARE_PROPERTY(const char*, LightInfo, LightType, GetLightTypeString, 
	SetLightTypeString);
/// property: float Intensity
/// \brief ��ǿ��
DECLARE_PROPERTY(float, LightInfo, Intensity, GetIntensity, SetIntensity);
/// property: float ColorRed
/// \brief ��ɫ����
DECLARE_PROPERTY(float, LightInfo, ColorRed, GetColorRed, SetColorRed);
/// property: float ColorGreen
/// \brief ��ɫ����
DECLARE_PROPERTY(float, LightInfo, ColorGreen, GetColorGreen, SetColorGreen);
/// property: float ColorBlue
/// \brief ��ɫ����
DECLARE_PROPERTY(float, LightInfo, ColorBlue, GetColorBlue, SetColorBlue);
/// property: float Cosine
/// \brief �Ƕ�����
DECLARE_PROPERTY(float, LightInfo, Cosine, GetCosine, SetCosine);
/// property: float Shadow
/// \brief ͶӰǿ��
DECLARE_PROPERTY(float, LightInfo, Shadow, GetShadow, SetShadow);
/// property: float Bright
/// \brief ����
DECLARE_PROPERTY(float, LightInfo, Bright, GetBright, SetBright);
/// property: float Contrast
/// \brief �Աȶ�
DECLARE_PROPERTY(float, LightInfo, Contrast, GetContrast, SetContrast);
/// property: float Gamma
/// \brief Gamma����
DECLARE_PROPERTY(float, LightInfo, Gamma, GetGamma, SetGamma);
/// property: float Attenu0
/// \brief ˥��ϵ��0
DECLARE_PROPERTY(float, LightInfo, Attenu0, GetAttenu0, SetAttenu0);
/// property: float Attenu1
/// \brief ˥��ϵ��1
DECLARE_PROPERTY(float, LightInfo, Attenu1, GetAttenu1, SetAttenu1);
/// property: float Attenu2
/// \brief ˥��ϵ��2
DECLARE_PROPERTY(float, LightInfo, Attenu2, GetAttenu2, SetAttenu2);
/// property: float Range
/// \brief Ӱ�췶Χ
DECLARE_PROPERTY(float, LightInfo, Range, GetRange, SetRange);
/// property: float InnerDegree
/// \brief �۹���ڽ�
DECLARE_PROPERTY(float, LightInfo, InnerDegree, GetInnerDegree, 
	SetInnerDegree);
/// property: float OuterDegree
/// \brief �۹�����
DECLARE_PROPERTY(float, LightInfo, OuterDegree, GetOuterDegree, 
	SetOuterDegree);
/// property: float Falloff
/// \brief �۹��FALLOFF
DECLARE_PROPERTY(float, LightInfo, Falloff, GetFalloff, SetFalloff);
/// property: float PosiX
/// \brief λ������X
DECLARE_PROPERTY(float, LightInfo, PosiX, GetPosiX, SetPosiX);
/// property: float PosiY
/// \brief λ������Y
DECLARE_PROPERTY(float, LightInfo, PosiY, GetPosiY, SetPosiY);
/// property: float PosiZ
/// \brief λ������Z
DECLARE_PROPERTY(float, LightInfo, PosiZ, GetPosiZ, SetPosiZ);
/// property: float AngleX
/// \brief �Ƕ�X
DECLARE_PROPERTY(float, LightInfo, AngleX, GetAngleX, SetAngleX);
/// property: float AngleY
/// \brief �Ƕ�Y
DECLARE_PROPERTY(float, LightInfo, AngleY, GetAngleY, SetAngleY);
/// property: float AngleZ
/// \brief �Ƕ�Z
DECLARE_PROPERTY(float, LightInfo, AngleZ, GetAngleZ, SetAngleZ);
/// property: float SizeX
/// \brief ���ӳߴ�X
DECLARE_PROPERTY(float, LightInfo, SizeX, GetSizeX, SetSizeX);
/// property: float SizeY
/// \brief ���ӳߴ�Y
DECLARE_PROPERTY(float, LightInfo, SizeY, GetSizeY, SetSizeY);
/// property: float SizeZ
/// \brief ���ӳߴ�Z
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
