//--------------------------------------------------------------------
// �ļ���:		weather.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "weather.h"
#include "scene.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"
#include "../visual/i_context.h"

/// \file weather.cpp
/// \brief ����

/// entity: Weather
/// \brief ����ʵ��
DECLARE_ENTITY(Weather, 0, IEntity);

/// property: bool FogExp
/// \brief �Ƿ���ָ����
DECLARE_PROPERTY(bool, Weather, FogExp, GetFogExp, SetFogExp);
/// property: bool FogLinear
/// \brief �Ƿ���������
DECLARE_PROPERTY(bool, Weather, FogLinear, GetFogLinear, SetFogLinear);
/// property: string FogExpColor
/// \brief ָ������ɫ
DECLARE_PROPERTY(result_string, Weather, FogExpColor, GetFogExpColorString, 
	SetFogExpColorString);
/// property: string FogColor
/// \brief ��������ɫ
DECLARE_PROPERTY(result_string, Weather, FogColor, GetFogColorString, 
	SetFogColorString);
/// property: float FogDensity
/// \brief ָ����Ũ��
DECLARE_PROPERTY(float, Weather, FogDensity, GetFogDensity, SetFogDensity);
/// property: float FogStart
/// \brief ���������ʼ����
DECLARE_PROPERTY(float, Weather, FogStart, GetFogStart, SetFogStart);
/// property: float FogEnd
/// \brief ������Ľ�������
DECLARE_PROPERTY(float, Weather, FogEnd, GetFogEnd, SetFogEnd);
/// property: float FogHeight
/// \brief ������߶�
DECLARE_PROPERTY(float, Weather, FogHeight, GetFogHeight, SetFogHeight);

/// �߶���
/// property: bool HeightFog
/// \brief �Ƿ����߶���
DECLARE_PROPERTY(bool, Weather, HeightFog, GetHeightFog, SetHeightFog);
/// property: string HeightFogColor
/// \brief �߶�����ɫ
DECLARE_PROPERTY(result_string, Weather, HeightFogColor, GetHeightFogColorString, SetHeightFogColorString);
/// property: float HeightFogStart
/// \brief �߶������ʼ����
DECLARE_PROPERTY(float, Weather, HeightFogStart, GetHeightFogStart, SetHeightFogStart);
/// property: float HeightFogEnd
/// \brief �߶���Ľ�������
DECLARE_PROPERTY(float, Weather, HeightFogEnd, GetHeightFogEnd, SetHeightFogEnd);

/// property: string SunDirection
/// \brief ����Դ�ķ���
DECLARE_PROPERTY(result_string, Weather, SunDirection, GetSunDirectionString, 
	SetSunDirectionString)
/// property: string DiffuseColor
/// \brief ����Դ��ɫ
DECLARE_PROPERTY(result_string, Weather, DiffuseColor, GetDiffuseColorString, 
	SetDiffuseColorString);
// property: float DiffuseIntensity
/// \brief ����Դ����ϵ��
DECLARE_PROPERTY(float, Weather, DiffuseIntensity, GetDiffuseIntensity, 
	SetDiffuseIntensity);
/// property: string AmbientColor
/// \brief ��������ɫ
DECLARE_PROPERTY(result_string, Weather, AmbientColor, GetAmbientColorString, 
	SetAmbientColorString);
/// property: string SpecularColor
/// \brief �������淴�����ɫ
DECLARE_PROPERTY(result_string, Weather, SpecularColor, GetSpecularColorString, 
	SetSpecularColorString);

// property: float AmbientIntensity
/// \brief ����������ϵ��
DECLARE_PROPERTY(float, Weather, AmbientIntensity, GetAmbientIntensity, 
	SetAmbientIntensity);

/// \brief ��������ϵ��
DECLARE_PROPERTY(float, Weather, SunGlowIntensity, GetSunGlowIntensity, 
	SetSunGlowIntensity);
// property: float SunGlowIntensity
/// \brief ��������ϵ��
DECLARE_PROPERTY(float, Weather, SpecularIntensity, GetSpecularIntensity, 
	SetSpecularIntensity);

/// property: float WindAngle
/// \brief ����
DECLARE_PROPERTY(float, Weather, WindAngle, GetWindAngle, SetWindAngle);
/// property: float WindSpeed
/// \brief ����
DECLARE_PROPERTY(float, Weather, WindSpeed, GetWindSpeed, SetWindSpeed);

// Weather

Weather::Weather()
{
	m_pContext = NULL;
}

Weather::~Weather()
{
}

bool Weather::Init(const IVarList& args)
{
	PERSISTID scene_id = args.ObjectVal(0);
	Scene* pScene = (Scene*)GetCore()->GetEntity(scene_id);

	if (NULL == pScene)
	{
		CORE_TRACE("(Weather::Init)no scene entity");
		return false;
	}

	if (!pScene->GetEntInfo()->IsKindOf("Scene"))
	{
		CORE_TRACE("(Weather::Init)scene entity type error");
		return false;
	}

	m_pContext = pScene->GetContext();
    m_pContext->SetFloat(IRenderContext::F_SPECULAR_INTENSITY, 2.0);
    m_pContext->SetFloat(IRenderContext::F_AMBIENT_INTENSITY, 2.0);
    m_pContext->SetFloat(IRenderContext::F_DIFFUSE_INTENSITY, 2.0);	
	return true;
}

bool Weather::Shut()
{
	return true;
}

void Weather::SetFogExp(bool value)
{
	if (value)
	{
		m_pContext->SetInt(IRenderContext::I_FOG_EXP, 1);
	}
	else
	{
		m_pContext->SetInt(IRenderContext::I_FOG_EXP, 0);
	}
}

bool Weather::GetFogExp() const
{
	return (m_pContext->GetInt(IRenderContext::I_FOG_EXP) != 0);
}

void Weather::SetFogLinear(bool value)
{
	if (value)
	{
		m_pContext->SetInt(IRenderContext::I_FOG_LINEAR, 1);
	}
	else
	{
		m_pContext->SetInt(IRenderContext::I_FOG_LINEAR, 0);
	}
}

bool Weather::GetFogLinear() const
{
	return (m_pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0);
}

void Weather::SetFogExpColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_FOG_EXP_COLOR, color);
}

result_string Weather::GetFogExpColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_FOG_EXP_COLOR);

	return VisUtil_IntToColor(color);
}

void Weather::SetFogColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_FOG_COLOR_GROUND, color);
}

result_string Weather::GetFogColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_FOG_COLOR_GROUND);

	return VisUtil_IntToColor(color);
}

void Weather::SetFogDensity(float value)
{
	m_pContext->SetFloat(IRenderContext::F_FOG_DENSITY_GROUND, value);
}

float Weather::GetFogDensity() const
{
	return m_pContext->GetFloat(IRenderContext::F_FOG_DENSITY_GROUND);
}

void Weather::SetFogStart(float value)
{
	m_pContext->SetFloat(IRenderContext::F_FOG_START_GROUND, value);
}

float Weather::GetFogStart() const
{
	return m_pContext->GetFloat(IRenderContext::F_FOG_START_GROUND);
}

void Weather::SetFogEnd(float value)
{
	m_pContext->SetFloat(IRenderContext::F_FOG_END_GROUND, value);
}

float Weather::GetFogEnd() const
{
	return m_pContext->GetFloat(IRenderContext::F_FOG_END_GROUND);
}

void Weather::SetFogHeight(float value)
{
	m_pContext->SetFloat(IRenderContext::F_FOG_HEIGHT, value);
}

float Weather::GetFogHeight() const
{
	return m_pContext->GetFloat(IRenderContext::F_FOG_HEIGHT);
}

void Weather::SetHeightFog(bool value)
{
	if (value)
	{
		m_pContext->SetInt(IRenderContext::I_HEIGHT_FOG, 1);
	}
	else
	{
		m_pContext->SetInt(IRenderContext::I_HEIGHT_FOG, 0);
	}
}

bool Weather::GetHeightFog() const
{
	return (m_pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0);
}

void Weather::SetHeightFogColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_HEIGHTFOG_COLOR, color);
}

result_string Weather::GetHeightFogColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_HEIGHTFOG_COLOR);

	return VisUtil_IntToColor(color);
}

void Weather::SetHeightFogStart(float value)
{
	m_pContext->SetFloat(IRenderContext::F_HEIGHT_FOG_START, value);
}

float Weather::GetHeightFogStart() const
{
	return m_pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_START);
}

void Weather::SetHeightFogEnd(float value)
{
	m_pContext->SetFloat(IRenderContext::F_HEIGHT_FOG_END, value);
}

float Weather::GetHeightFogEnd() const
{
	return m_pContext->GetFloat(IRenderContext::F_HEIGHT_FOG_END);
}

void Weather::SetSunDirectionString(const char* value)
{
	FmVec3 v = VisUtil_StringToVector(value);

	FmVec3Normalize(&v, &v);

	m_pContext->SetVector4(IRenderContext::V_LIGHT_DIRECTION, FmVec4(v, 0.0F));
}

result_string Weather::GetSunDirectionString() const
{
	FmVec3 v = m_pContext->GetVector(IRenderContext::V_LIGHT_DIRECTION);

	return VisUtil_VectorToString(v);
}

void Weather::SetDiffuseColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);
	
	m_pContext->SetColor(IRenderContext::C_DIFFUSE_COLOR, color);
}

result_string Weather::GetDiffuseColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_DIFFUSE_COLOR);
	
	return VisUtil_IntToColor(color);
}

void Weather::SetDiffuseIntensity(float value)
{
	m_pContext->SetFloat(IRenderContext::F_DIFFUSE_INTENSITY, value);
}

float Weather::GetDiffuseIntensity() const
{
	return m_pContext->GetFloat(IRenderContext::F_DIFFUSE_INTENSITY);
}

void Weather::SetAmbientColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_AMBIENT_COLOR, color);
}

result_string Weather::GetAmbientColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_AMBIENT_COLOR);

	return VisUtil_IntToColor(color);
}


void Weather::SetSpecularColorString(const char* value)
{
	unsigned int color = VisUtil_ColorToInt(value);

	m_pContext->SetColor(IRenderContext::C_SPECULAR_COLOR, color);
}

result_string Weather::GetSpecularColorString() const
{
	unsigned int color = m_pContext->GetColor(IRenderContext::C_SPECULAR_COLOR);

	return VisUtil_IntToColor(color);
}


void Weather::SetAmbientIntensity(float value)
{
	m_pContext->SetFloat(IRenderContext::F_AMBIENT_INTENSITY, value);
}

float Weather::GetAmbientIntensity() const
{
	return m_pContext->GetFloat(IRenderContext::F_AMBIENT_INTENSITY);
}

void Weather::SetSunGlowIntensity(float value)
{
	m_pContext->SetFloat(IRenderContext::F_DIFFUSE_INTENSITY, value);
}

float Weather::GetSunGlowIntensity() const
{
	return m_pContext->GetFloat(IRenderContext::F_DIFFUSE_INTENSITY);
}

void Weather::SetSpecularIntensity(float value)
{
	m_pContext->SetFloat(IRenderContext::F_SPECULAR_INTENSITY, value);
}

float Weather::GetSpecularIntensity() const
{
	return m_pContext->GetFloat(IRenderContext::F_SPECULAR_INTENSITY);
}

void Weather::SetWindAngle(float value)
{
	m_pContext->SetFloat(IRenderContext::F_WIND_ANGLE, value);
}

float Weather::GetWindAngle() const
{
	return m_pContext->GetFloat(IRenderContext::F_WIND_ANGLE);
}

void Weather::SetWindSpeed(float value)
{
	m_pContext->SetFloat(IRenderContext::F_WIND_SPEED, value);
}

float Weather::GetWindSpeed() const
{
	return m_pContext->GetFloat(IRenderContext::F_WIND_SPEED);
}

static inline void rotate_around_y(float& x, float& z, float cos, float sin)
{
	float xx = x * cos + z * sin;
	float zz = z * cos - x * sin;
	
	x = xx;
	z = zz;
}

bool Weather::MakeSunDirection(float height_angle, float azimuth_angle)
{
	FmVec3 v(5000.0F, 0.0F, 0.0F);
	
	rotate_around_y(v.x, v.y, cosf(-height_angle), sinf(-height_angle));
	v.z = 0.0F;

	// -X���߽磬����270��
	// +X���߽磬����90��
	// -Z���߽磬����180��
	// +Z�ϱ߽磬����0��
	azimuth_angle -= FLOAT_PI * 0.5F;
	rotate_around_y(v.x, v.z, cosf(azimuth_angle), sinf(azimuth_angle));
	FmVec3Normalize(&v, &v);
	m_pContext->SetVector4(IRenderContext::V_LIGHT_DIRECTION, FmVec4(v, 0.0F));
	
	return true;
}
