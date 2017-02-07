//--------------------------------------------------------------------
// �ļ���:		particle_data.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "particle_data.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/vis_utils.h"

// CParticleData

CParticleData* CParticleData::NewInstance()
{
	return CORE_NEW(CParticleData);
}

CParticleData::CParticleData()
{
	m_nHash = 0;
	m_nRefs = 1;

	m_Visibility[0].time = -1;
	m_ParticleDir[0].time = -1;
	m_ParticleSize[0].time = -1;
	m_ParticleSpeed[0].time = -1;
	m_ParticleSpin[0].time = -1;
	m_ParticleAngle[0].time = -1;
	m_WindEffect[0].time = -1;
	m_GravityEffect[0].time = -1;
	m_EmitRadiusX[0].time = -1;
	m_EmitRadiusY[0].time = -1;
	m_EmitRadiusZ[0].time = -1;
	//m_ColorRed[0].time = -1;
	//m_ColorGreen[0].time = -1;
	//m_ColorBlue[0].time = -1;
	
	m_fDirectionDeviation = 0.0F;
	m_fWindEffect = 0.0F;
	m_fGravity = 0.0F;
	m_fInitSpeed = 0.0F;
	m_fSpeedDeviation = 0.0F;
	m_fInitSize = 1.0F;
	m_fSizeDeviation = 0.0F;
	m_fSpin = 0.0F;
	m_fSpinDeviation = 0.0F;
	m_fWeight = 0.0F;
	m_fWeightDeviation = 0.0F;
	
	m_fEmitRadiusX = 0.0F;
	m_fEmitRadiusY = 0.0F;
	m_fEmitRadiusZ = 0.0F;
	m_fEmitRadiusXRand = 0.0F;
	m_fEmitRadiusYRand = 0.0F;
	m_fEmitRadiusZRand = 0.0F;
	
	m_fTrackPointRadius = 1.0F;
	m_fSurfaceOffset = 0.0F;
	m_fEmissionTime = 1.0F;
	m_fEmissionTimeRand = 0.0F;
	m_fChaosVal = 0.0F;
	m_fSpeedChaos = 0.0F;
	m_fAspectRatio = 1.0F;
	m_fLuminance = 1.0F;
	m_fDistortionIntensity = 1.0F;

	m_nTextureFrame = 1;
	m_nParticleNum = 1;
	m_nParticleLife = 1000;
	m_nTailLife = 100;
	m_nOneEmitNumber = 1;
	m_nParticleFrame = 1;
	m_nOneAxisNotRand = 0;
	m_nParticleColor = 0xFFFFFFFF;
	
	m_bRepeat = false;
	m_bTrackPointReturn = false;
	m_bTrackPointDisappear = false;
	m_bLocalSpaceEmitter = false;
	m_bLocalAngleEmitter = false;
	m_bAngleLinkSpeed = false;
	m_bAngleLinkEmitter = false;
	m_bRandomDirection = false;
	m_bFlashTail = false;
	m_bFixupTailSize = false;
	m_bDelayUnload = true;
	m_bNotDownLevel = false;
	m_bColorAni = false;
	m_bStaticBoundBox = false;

	m_vBoxMin = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoxMax = FmVec3(0.0F, 0.0F, 0.0F);
}

CParticleData::~CParticleData()
{
}

int CParticleData::GetRefs() const
{
	return m_nRefs;
}

int CParticleData::IncRefs()
{
	return ++m_nRefs;
}

void CParticleData::Release()
{
	if (--m_nRefs == 0)
	{
		CORE_DELETE(this);
	}
}

/*
#define	PSKEY_TEXTURE			"texture"
#define PSKEY_TRACKPOINTRETURN	"track_point_return"
#define PSKEY_ONEEMITPSNUM		"one_emit_particles_num"
#define PSKEY_PFRAMENUM			"particles_frame_num"
#define PSKEY_TEX_FRM			"texture_frame"
#define	PSKEY_PNUM				"particles_num"
#define PSKEY_EMISSIONTIME		"emissiontime"
#define PSKEY_EMISSIONTIMERAND	"emissiontime_rand"
#define PSKEY_TECHNIQUE			"technique"
#define PSKEY_SURFACEOFFSET		"surfaceoffset"
#define PSKEY_WINDEFFECT		"windeffect"
#define PSKEY_DDEVIATION		"ddeviation"
#define PSKEY_GRAVITY			"gravity"
#define PSKEY_INISPEED			"speed"
#define PSKEY_SDEVIATION		"speed_deviation"
#define PSKEY_SPEEDCHAOS		"speed_chaos"
#define PSKEY_LIFETIME			"lifetime"
#define PSKEY_TAILTIME			"tailtime"
#define PSKEY_SPIN				"spin"
#define PSKEY_SPINDEV			"spin_deviation"
#define PSKEY_EMITTERINITIME	"emitter_initime"
#define PSKEY_WEIGHT			"weight"
#define PSKEY_WEIGHTDEVIATION	"weight_deviation"
#define PSKEY_RANDOMDIRECTION	"randomdirection"
#define PSKEY_NONSTOPEMIT		"nonstopemit"
#define PSKEY_FLASHTAIL			"flashtail"
#define PSKEY_FIXUPTAILSIZE		"fixuptailsize"
#define PSKEY_EMITRADIUS		"emit_radius"
#define PSKEY_TRACKPOINTRADIUS	"trackpoint_radius"
#define PSKEY_ALPHAKEY			"key_alpha"
#define PSKEY_PSIZEKEY			"key_psize"
#define PSKEY_PSPEEDKEY			"key_pspeed"
#define PSKEY_PSPINKEY			"key_spin"
#define PSKEY_PANGLEKEY			"key_angle"
#define PSKEY_WINDEFFECTKEY		"key_windeffect"
#define PSKEY_CHAOSVAL			"chaos_val"
*/

static inline int read_ini_integer(IIniFile* pIni, 
	size_t sect_index, const char* key, int def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return atoi(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline float read_ini_float(IIniFile* pIni, 
	size_t sect_index, const char* key, float def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return (float)atof(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline const char* read_ini_string(IIniFile* pIni, 
	size_t sect_index, const char* key, const char* def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return pIni->GetSectionItemValue(sect_index, item_index);
}

// У�����ݣ���һ֡�����ݵ�w��ʱ�䣩���ܴ���0.0F
static inline void verify_track_data(FmVec4* data, int data_size)
{
	if (data[0].w > 0.0F)
	{
		for (int i = data_size - 1; i > 0; i--)
		{
			data[i] = data[i - 1];
		}

		data[0].w = 0.0F;
	}
}

bool CParticleData::LoadIni(IIniFile* ini, const char* psname,
	const char* tex_path)
{
	Assert(ini != NULL);
	Assert(psname != NULL);
	Assert(tex_path != NULL);

	size_t sect_index;

	if (!ini->FindSectionIndex(psname, sect_index))
	{
		CORE_TRACE("(CParticleData::LoadIni)section not found");
		CORE_TRACE(psname);
		CORE_TRACE(ini->GetFileName());
		return false;
	}

	// ��ͼ
	const char* tex_name = read_ini_string(ini, sect_index, "texture", "");
	// �����ɫ��ͼ
	const char* rnd_tex = read_ini_string(ini, sect_index, "rand_color_tex", 
		"");
	// ������ͼ
	const char* refraction_tex = read_ini_string(ini, sect_index, 
		"refraction", "");
	// ��ʾЧ��
	m_strEffectName = read_ini_string(ini, sect_index, "technique", "");
	// �����ģʽ
	m_strBillboardType = read_ini_string(ini, sect_index, "billboard", 
		"Billboard");
	// ���Ӽ���ģʽ
	m_strCalculateMode = read_ini_string(ini, sect_index, "calculate", 
		"Simple");

	if (tex_name[0] != 0)
	{
		m_strTextureName = tex_path;
		m_strTextureName += tex_name;
	}

	if (rnd_tex[0] != 0)
	{
		m_strRandomColorTex = tex_path;
		m_strRandomColorTex += rnd_tex;
	}

	if (refraction_tex[0] != 0)
	{
		m_strRefractionTex = tex_path;
		m_strRefractionTex += refraction_tex;
	}

	// ������������
	m_strNextParticle = read_ini_string(ini, sect_index, "NextParticle", "");
	
	size_t item_index;

	// �ظ�����
	m_bRepeat = ini->FindSectionItemIndex(sect_index, "NonStopEmit", 
		item_index);
	
	// ��������
	m_nParticleNum = read_ini_integer(ini, sect_index, "particles_num", 1);

	if (m_nParticleNum < 1)
	{
		m_nParticleNum = 1;
	}

	// ��������ʱ��
	m_nParticleLife = read_ini_integer(ini, sect_index, "lifetime", 0);

	if (m_nParticleLife < 1)
	{
		m_nParticleLife = 1;
	}

	// ÿ�εķ�������
	m_nOneEmitNumber = read_ini_integer(ini, sect_index, 
		"one_emit_particles_num", 1);

	if (m_nOneEmitNumber < 1)
	{
		m_nOneEmitNumber = 1;
	}

	// ������ɫ
	const char* par_color = read_ini_string(ini, sect_index, "color", 
		"255,255,255,255");

	if (strchr(par_color, ',') != NULL)
	{
		m_nParticleColor = VisUtil_ColorToInt(par_color);
	}
	else
	{
		// Ҳ����������ʽ����ɫ����
		m_nParticleColor = atoi(par_color);
	}

	// ����ʱ��
	m_fEmissionTime = FM_MAX(read_ini_float(ini, sect_index, "EmissionTime", 
		1.0F), 1.0F);
	m_fEmissionTimeRand = read_ini_float(ini, sect_index, "emissiontime_rand", 
		0.0F);

	// ����Ӱ��
	m_fWindEffect = read_ini_float(ini, sect_index, "windeffect", 0.0F);
	// ����Ӱ��
	m_fGravity = read_ini_float(ini, sect_index, "gravity", 0.0F);
	// �����ľ���
	m_fSurfaceOffset = read_ini_float(ini, sect_index, "surfaceoffset", 0.0F);

	// �������
	m_bRandomDirection = ini->FindSectionItemIndex(sect_index, 
		"RandomDirection", item_index);
	// ����ƫ��
	m_fDirectionDeviation = read_ini_float(ini, sect_index, "ddeviation", 0.0F);

	// ���ӳߴ�
	m_fInitSize = read_ini_float(ini, sect_index, "size", 1.0F);
	// ���ӳߴ�ƫ��ֵ
	m_fSizeDeviation = read_ini_float(ini, sect_index, "size_deviation", 0.0F);
	// �������
	m_fAspectRatio = read_ini_float(ini, sect_index, "AspectRatio", 1.0F);

	// �����ٶ�
	m_fInitSpeed = read_ini_float(ini, sect_index, "speed", 0.0F);
	// �����ٶ�ƫ��ֵ
	m_fSpeedDeviation = read_ini_float(ini, sect_index, "speed_deviation", 0.0F);
	// �����ٶ����ֵ
	m_fSpeedChaos = read_ini_float(ini, sect_index, "speed_chaos", 0.0F);

	// ��ת
	m_fSpin = read_ini_float(ini, sect_index, "spin", 0.0F);
	// ��תƫ��ֵ
	m_fSpinDeviation = read_ini_float(ini, sect_index, "spin_deviation", 0.0F);

	// ����
	m_fWeight = read_ini_float(ini, sect_index, "weight", 0.0F);
	// ����ƫ��ֵ
	m_fWeightDeviation = read_ini_float(ini, sect_index, "weight_deviation", 0.0F);

	// �Ƿ������β
	m_bFlashTail = ini->FindSectionItemIndex(sect_index, "FlashTail", item_index);
	// ��βʱ��
	m_nTailLife = read_ini_integer(ini, sect_index, "TailTime", 0);
	// ������β��С
	m_bFixupTailSize = ini->FindSectionItemIndex(sect_index, "FixUpTailSize", 
		item_index);
	
	// ���ӳ���λ�õ����ֵ
	m_fChaosVal = read_ini_float(ini, sect_index, "chaos_val", 0.0F);

	// ���䷶Χ
	float emit_space = read_ini_float(ini, sect_index, "emit_radius", 0.0F);

	// ����뾶
	m_fEmitRadiusX = read_ini_float(ini, sect_index, "emit_radiusx", emit_space);
	m_fEmitRadiusY = read_ini_float(ini, sect_index, "emit_radiusy", emit_space);
	m_fEmitRadiusZ = read_ini_float(ini, sect_index, "emit_radiusz", emit_space);
	// ����뾶���ֵ
	m_fEmitRadiusXRand = read_ini_float(ini, sect_index, "emit_radiusx_rand", 0.0F);
	m_fEmitRadiusYRand = read_ini_float(ini, sect_index, "emit_radiusy_rand", 0.0F);
	m_fEmitRadiusZRand = read_ini_float(ini, sect_index, "emit_radiusz_rand", 0.0F);
	
	// ���ձ�־
	m_bTrackPointReturn = ini->FindSectionItemIndex(sect_index, 
		"track_point_return", item_index);
	// �������ʧ
	m_bTrackPointDisappear = ini->FindSectionItemIndex(sect_index, 
		"track_point_disappear", item_index);
	// ���հ뾶
	m_fTrackPointRadius = read_ini_float(ini, sect_index, 
		"trackpoint_radius", 0.0F);

	// ���������������λ��
	m_bLocalSpaceEmitter = ini->FindSectionItemIndex(sect_index, 
		"LocalSpaceEmitter", item_index);

	// ��������������Է���
	m_bLocalAngleEmitter = ini->FindSectionItemIndex(sect_index, 
		"LocalAngleEmitter", item_index);

	// �������������ٶȷ���
	m_bAngleLinkSpeed = ini->FindSectionItemIndex(sect_index, 
		"AngleLinkSpeed", item_index);

	// �����������ķ���������
	m_bAngleLinkEmitter = ini->FindSectionItemIndex(sect_index, 
		"AngleLinkEmitter", item_index);

	// ������ͼUV����
	m_nParticleFrame = read_ini_integer(ini, sect_index, 
		"particles_frame_num", 1);

	if (m_nParticleFrame < 1)
	{
		m_nParticleFrame = 1;
	}

	// ��ͼ�ֿ�����
	m_nTextureFrame = read_ini_integer(ini, sect_index, "texture_frame", 1);

	if (m_nTextureFrame < 1)
	{
		m_nTextureFrame = 1;
	}

	// ����һ����ķ��䷽��
	const char* one_axis = read_ini_string(ini, sect_index, 
		"OneAxisNotRand", "");

	if (stricmp(one_axis, "XAxis") == 0)
	{
		m_nOneAxisNotRand = AXIS_X;
	}
	else if (stricmp(one_axis, "YAxis") == 0)
	{
		m_nOneAxisNotRand = AXIS_Y;
	}
	else if (stricmp(one_axis, "ZAxis") == 0)
	{
		m_nOneAxisNotRand = AXIS_Z;
	}
	else
	{
		m_nOneAxisNotRand = AXIS_NONE;
	}

	// ��Χ�гߴ�
	size_t size_x_index;
	size_t size_y_index;
	size_t size_z_index;

	if (ini->FindSectionItemIndex(sect_index, "BoxSizeX", size_x_index)
		&& ini->FindSectionItemIndex(sect_index, "BoxSizeY", size_y_index)
		&& ini->FindSectionItemIndex(sect_index, "BoxSizeZ", size_z_index))
	{
		float box_size_x = (float)atof(
			ini->GetSectionItemValue(sect_index, size_x_index));
		float box_size_y = (float)atof(
			ini->GetSectionItemValue(sect_index, size_y_index));
		float box_size_z = (float)atof(
			ini->GetSectionItemValue(sect_index, size_z_index));
		//float box_size_x = read_ini_float(ini, sect_index, "BoxSizeX", 0.001F);
		//float box_size_y = read_ini_float(ini, sect_index, "BoxSizeY", 0.001F);
		//float box_size_z = read_ini_float(ini, sect_index, "BoxSizeZ", 0.001F);

		if ((box_size_x > 0.0F) && (box_size_y > 0.0F) && (box_size_z > 0.0F))
		{
			m_vBoxMin.x = -box_size_x * 0.5F;
			m_vBoxMin.y = 0.0F;
			m_vBoxMin.z = -box_size_z * 0.5F;
			m_vBoxMax.x = box_size_x * 0.5F;
			m_vBoxMax.y = box_size_y;
			m_vBoxMax.z = box_size_z * 0.5F;
			m_bStaticBoundBox = true;
		}
	}

	// �ؼ�֡����
	BuildTrack(ini, m_Visibility, psname, sect_index, "key_alpha");
	BuildTrack(ini, m_ParticleDir, psname, sect_index, "key_ddeviation");
	BuildTrack(ini, m_ParticleSize, psname, sect_index, "key_psize");
	BuildTrack(ini, m_ParticleSpeed, psname, sect_index, "key_pspeed");
	BuildTrack(ini, m_ParticleSpin, psname, sect_index, "key_spin");
	BuildTrack(ini, m_WindEffect, psname, sect_index, "key_windeffect");
	BuildTrack(ini, m_GravityEffect, psname, sect_index, "key_gravity");
	BuildTrack(ini, m_EmitRadiusX, psname, sect_index, "key_emit_radiusx");
	BuildTrack(ini, m_EmitRadiusY, psname, sect_index, "key_emit_radiusy");
	BuildTrack(ini, m_EmitRadiusZ, psname, sect_index, "key_emit_radiusz");
	BuildTrack(ini, m_ParticleAngle, psname, sect_index, "key_angle");

	//// ��ɫ�ؼ�֡
	//bool track_red = BuildTrack(ini, m_ColorRed, psname, sect_index, 
	//	"key_red");
	//bool track_green = BuildTrack(ini, m_ColorGreen, psname, sect_index, 
	//	"key_green");
	//bool track_blue = BuildTrack(ini, m_ColorBlue, psname, sect_index, 
	//	"key_blue");

	// ��������
	m_fLuminance = read_ini_float(ini, sect_index, 
		"luminance", 1.0F);
	// Ť������
	m_fDistortionIntensity = read_ini_float(ini, sect_index, 
		"DistortionIntensity", 1.0F);

	// �ӳ�ж������
	//const char* val = ini->ReadString(sect_index, "DelayUnload", "true");
	//m_bDelayUnload = stricmp(val, "true") == 0;
	m_bDelayUnload = ini->FindSectionItemIndex(sect_index, "DelayUnload", 
		item_index);

	// ��������ʾ
	m_bNotDownLevel = ini->FindSectionItemIndex(sect_index, "NotDownLevel", 
		item_index);

	// ��ɫ����
	m_bColorAni = ini->FindSectionItemIndex(sect_index, "ColorAni", 
		item_index);

	// �����ʺ�GPU����Ĺ켣����
	InitTrackData();

	return true;
}

void CParticleData::InitTrackData()
{
	float particle_life = (float)m_nParticleLife;
	
	// �ɼ���
	track_event_t* track = m_Visibility;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				m_vVisibilityTrack[i] = FmVec4(1.0F, 1.0F, 1.0F, 
					particle_life);
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vVisibilityTrack[i] = m_vVisibilityTrack[i - 1];
				m_vVisibilityTrack[i].w = particle_life;
			}

			break;
		}
		else
		{
			m_vVisibilityTrack[i] = FmVec4(track[i].value, 0.0F, 0.0F, 
				(float)track_time);
		}
	}

	verify_track_data(m_vVisibilityTrack, TRACK_EVENT_MAX);

	// �ٶ�
	track = m_ParticleSpeed;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				m_vParticleSpeedTrack[i] = FmVec4(0.0F, 0.0F, 0.0F, 
					particle_life);
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vParticleSpeedTrack[i] = m_vParticleSpeedTrack[i - 1];
				m_vParticleSpeedTrack[i].w = particle_life;
			}

			break;
		}
		else
		{
			m_vParticleSpeedTrack[i] = FmVec4(track[i].value, 1.0F, 1.0F, 
				(float)track_time);
		}
	}

	verify_track_data(m_vParticleSpeedTrack, TRACK_EVENT_MAX);

	// �ٶȶ����¹ؼ�֡��λ��
	m_vParticleSpeedDelta[0] = FmVec4(0.0F, 0.0F, 0.0F, 
		m_vParticleSpeedTrack[0].w);

	for (int i = 1; i < TRACK_EVENT_MAX; i++)
	{
		m_vParticleSpeedDelta[i] = m_vParticleSpeedDelta[i - 1] 
			+ (m_vParticleSpeedTrack[i - 1] + m_vParticleSpeedTrack[i]) / 2.0F 
			* (m_vParticleSpeedTrack[i].w - m_vParticleSpeedTrack[i - 1].w);
	}

	// ����Ӱ��
	track = m_WindEffect;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				m_vWindEffectTrack[i] = FmVec4(1.0F, 1.0F, 1.0F, 
					particle_life);
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vWindEffectTrack[i] = m_vWindEffectTrack[i - 1];
				m_vWindEffectTrack[i].w = particle_life;
			}

			break;
		}
		else
		{
			m_vWindEffectTrack[i] = FmVec4(track[i].value, 1.0F, 1.0F,
				(float)track_time);
		}
	}

	// ����Ӱ��
	track = m_GravityEffect;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vWindEffectTrack[i].y = m_vWindEffectTrack[i - 1].y;
			}

			break;
		}
		else
		{
			m_vWindEffectTrack[i].y = track[i].value;
		}
	}

	verify_track_data(m_vWindEffectTrack, TRACK_EVENT_MAX);

	// ����������Ӱ���¹ؼ�֡��λ��
	m_vWindEffectDelta[0] = FmVec4(0.0F, 0.0F, 0.0F, 
		m_vWindEffectTrack[0].w);
	
	for (int i = 1; i < TRACK_EVENT_MAX; i++)
	{
		m_vWindEffectDelta[i] = m_vWindEffectDelta[i - 1] 
			+ (m_vWindEffectTrack[i - 1] + m_vWindEffectTrack[i]) / 2.0F 
			* (m_vWindEffectTrack[i].w - m_vWindEffectTrack[i - 1].w);
	}

	// ��С
	track = m_ParticleSize;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				m_vParticleSizeTrack[i] = FmVec4(0.0F, 0.0F, 0.0F, 
					particle_life);
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vParticleSizeTrack[i] = m_vParticleSizeTrack[i - 1];
				m_vParticleSizeTrack[i].w = particle_life;
			}

			break;
		}
		else
		{
			m_vParticleSizeTrack[i] = FmVec4(track[i].value, 0.0F, 0.0F, 
				(float)track_time);
		}
	}

	verify_track_data(m_vParticleSizeTrack, TRACK_EVENT_MAX);

	// ����ת��
	track = m_ParticleSpin;

	for (int i = 0; i < TRACK_EVENT_MAX; i++)
	{
		int track_time = track[i].time;

		if (track_time < 0)
		{
			if (0 == i)
			{
				m_vParticleSpinTrack[i] = FmVec4(0.0F, 0.0F, 0.0F, 
					particle_life);
				i = 1;
			}

			for (; i < TRACK_EVENT_MAX; ++i)
			{
				m_vParticleSpinTrack[i] = m_vParticleSpinTrack[i - 1];
				m_vParticleSpinTrack[i].w = particle_life;
			}

			break;
		}
		else
		{
			m_vParticleSpinTrack[i] = FmVec4(track[i].value, 1.0F, 1.0F,
				(float)track_time);
		}
	}

	verify_track_data(m_vParticleSpinTrack, TRACK_EVENT_MAX);

	// ���ӽǶ�
	m_vParticleSpinDelta[0] = FmVec4(0.0F, 0.0F, 0.0F, 
		m_vParticleSpinTrack[0].w);

	for (int i = 1; i < TRACK_EVENT_MAX; i++)
	{
		m_vParticleSpinDelta[i] = m_vParticleSpinDelta[i - 1] 
			+ (m_vParticleSpinTrack[i - 1] + m_vParticleSpinTrack[i]) / 2.0F 
			* (m_vParticleSpinTrack[i].w - m_vParticleSpinTrack[i - 1].w);
	}
}

bool CParticleData::BuildTrack(IIniFile* ini, track_event_t* track, 
	const char* psname, size_t sect_index, const char* key_name)
{
	size_t item_index;
	
	if (ini->FindSectionItemIndex(sect_index, key_name, item_index))
	{
		// �ɵĲ��淶��ʽ
		CVarList values;
		
		size_t value_num = ini->GetItemValueList(psname, key_name, values);
		size_t count = 0;

		for (size_t n = 0; n < value_num; ++n)
		{
			if ((n + 1) >= TRACK_EVENT_MAX)
			{
				break;
			}
			
			char buffer[64];
			
			CopyString(buffer, sizeof(buffer), values.StringVal(n));
			
			for (size_t i = 0; buffer[i]; i++)
			{
				if (buffer[i] == ',')
				{
					buffer[i] = 0;

					track[n].value = (float)atof(buffer);
					track[n].time = atol(&buffer[i + 1]);

					count++;
					
					track[n + 1].value = 0.0F;
					track[n + 1].time = -1;
					break;
				}
			}
		}
		
		return (count > 0);
	}
	else
	{
		// �µĹ淶�����ļ���ʽ
		bool found = false;
		
		for (int n = 0; n < TRACK_EVENT_MAX; n++)
		{
			track[n].value = 0.0F;
			track[n].time = -1;
			
			char key[64];
			
			SafeSprintf(key, sizeof(key), "%s%d", key_name, n);
			
			if (!ini->FindSectionItemIndex(sect_index, key, item_index))
			{
				track[n].time = -1; 
				return found;
			}
			
			found = true;

			const char* value = ini->GetSectionItemValue(sect_index, 
				item_index);
			
			char buffer[64];

			CopyString(buffer, sizeof(buffer), value);

			for (size_t i = 0; buffer[i]; i++)
			{
				if (buffer[i] == ',') 
				{
					buffer[i] = 0;
					track[n].value = (float)atof(buffer);
					track[n].time = atol(&buffer[i + 1]);
					break;
				}
			}
		}
		
		return found;
	}
}

#define FLOAT_EQUAL(v1, v2) \
	((v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON)))

float CParticleData::GetTrackValue(track_event_t* track, float time)
{
	if (time < 0)
	{
		return 0.0F;
	}

	track_event_t* te_end = track + TRACK_EVENT_MAX;

	for (track_event_t* te = track; te < te_end; ++te)
	{
		float track_time = te->time * 0.001F;
		
		// if time - return value
		if (FLOAT_EQUAL(time, track_time))
		{
			return te->value;
		}

		if (track_time < 0) 
		{
			// if no more keys - return previous value
			if (te == track)
			{
				return 0.0F;
			}
			else
			{
				track_event_t* prev_te = te - 1;

				return prev_te->value;
			}
		}

		// skip already processed keys
		if (time > track_time)
		{
			continue;
		}

		float v1;
		float v2;
		float t1;
		float t2;

		// set from value and time
		if (te == track)
		{
			v1 = 0;
			t1 = 0;
		}
		else
		{
			track_event_t* prev_te = te - 1;

			v1 = prev_te->value;
			t1 = prev_te->time * 0.001F;
		}

		// set to value and time
		v2 = te->value;
		t2 = track_time;

		if (FLOAT_EQUAL(t1, t2))
		{
			return te->value;	// input error, double key
		}

		return v1 + (time - t1) * (v2 - v1) / (t2 - t1);
	}

	return 0.0F;
}
