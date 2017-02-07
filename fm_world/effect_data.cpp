//--------------------------------------------------------------------
// �ļ���:		effect_data.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2011��10��18��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "effect_data.h"
#include "../visual/i_vis_base.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/i_ini_file.h"

// CEffectData

CEffectData* CEffectData::NewInstance(IRender* pRender)
{
	CEffectData* p = (CEffectData*)CORE_ALLOC(sizeof(CEffectData));

	new (p) CEffectData(pRender);

	return p;
}

CEffectData::CEffectData(IRender* pRender)
{
	m_nHash = 0;
	m_nRefs = 1;
	m_bNoCache = false;
	m_fUnuseTime = 0.0F;
	m_pRender = pRender;
}

CEffectData::~CEffectData()
{
	for (size_t i = 0; i < m_ParticleInfo.size(); ++i)
	{
		CORE_DELETE(m_ParticleInfo[i]);
	}

	for (size_t j = 0; j < m_SaberArcInfo.size(); ++j)
	{
		CORE_DELETE(m_SaberArcInfo[j]);
	}

	for (size_t k = 0; k < m_LightInfo.size(); ++k)
	{
		CORE_DELETE(m_LightInfo[k]);
	}
}

int CEffectData::GetRefs() const
{
	return m_nRefs;
}

int CEffectData::IncRefs()
{
	return ++m_nRefs;
}

void CEffectData::Release()
{
	if (--m_nRefs == 0)
	{
		CORE_DELETE(this);
	}
}

static inline int read_ini_integer(IIniFile* pIni, size_t sect_index, 
	const char* key, int def_value)
{
	size_t item_index;

	if (!pIni->FindSectionItemIndex(sect_index, key, item_index))
	{
		return def_value;
	}

	return atoi(pIni->GetSectionItemValue(sect_index, item_index));
}

static inline float read_ini_float(IIniFile* pIni, size_t sect_index, 
	const char* key, float def_value)
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

bool CEffectData::LoadIni(IIniFile* pIni, const char* em_name)
{
	Assert(pIni != NULL);
	Assert(em_name != NULL);

	size_t sect_index;

	if (!pIni->FindSectionIndex(em_name, sect_index))
	{
		CORE_TRACE("(CEffectData::LoadIni)section not found");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(em_name);
		return false;
	}

	// ģ���ļ�
	const char* model_file = read_ini_string(pIni, sect_index, "Model", "");

	if (StringEmpty(model_file))
	{
		CORE_TRACE("(CEffectData::LoadIni)no model file");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(em_name);
		return false;
	}

	m_strModelFile = model_file;

	size_t item_index;

	if (pIni->FindSectionItemIndex(sect_index, "@Model", item_index))
	{
		m_strMaterialFile = pIni->GetSectionItemValue(sect_index, item_index);
	}

	// ��ȡ��������

	// �����ļ���
	m_strParticleFile = read_ini_string(pIni, sect_index, "particle_file", "");

	// ��������
	int particle_number = read_ini_integer(pIni, sect_index, "number", 0);

	if ((m_strParticleFile.empty() && (particle_number > 0)) 
		|| (!m_strParticleFile.empty() && (particle_number <= 0)))
	{
		CORE_TRACE("(CEffectData::LoadIni)Particle config error");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(em_name);
		return false;
	}

	if (particle_number > 0)
	{
		char sKeyName[128] = { 0 };

		for (int i = 0; i < particle_number; ++i)
		{
			// ������
			SafeSprintf(sKeyName, sizeof(sKeyName), "particle%d", i);

			const char* par_name = read_ini_string(pIni, sect_index, 
				sKeyName, "");

			// ��������
			SafeSprintf(sKeyName, sizeof(sKeyName), "idhelper%d", i);

			const char* helper_name = read_ini_string(pIni, sect_index, 
				sKeyName, "");

			// ���ӷ�������λ��
			SafeSprintf(sKeyName, sizeof(sKeyName), "EmitOpen%d", i);

			int emit_open = read_ini_integer(pIni, sect_index, sKeyName, -1);

			// ���ӷ������ر�λ��
			SafeSprintf(sKeyName, sizeof(sKeyName), "EmitClose%d", i);

			int emit_close = read_ini_integer(pIni, sect_index, sKeyName, -1);

			// ��¼������Ϣ
			particle_info_t* par_info = CORE_NEW(particle_info_t);

			par_info->open_emit = emit_open;
			par_info->close_emit = emit_close;
			par_info->particle_name = par_name;
			par_info->helper_name = helper_name;

			m_ParticleInfo.push_back(par_info);
		}
	}

	// ��ȡ��β����

	// ���������ļ�
	m_strSaberArcFile = read_ini_string(pIni, sect_index, "saberarc_file", "");

	// ��������
	int saberarc_num = read_ini_integer(pIni, sect_index, "saberarc_number", 0);

	if ((m_strSaberArcFile.empty() && (saberarc_num > 0)) 
		|| (!m_strSaberArcFile.empty() && (saberarc_num <= 0)))
	{
		CORE_TRACE("(CEffectData::LoadIni)Saber config error");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(em_name);
		return false;
	}

	if (saberarc_num > 0)
	{
		char strName[32] = { 0 };

		for (int i = 0; i < saberarc_num; ++i)
		{
			// ��������
			SafeSprintf(strName, sizeof(strName), "saberarc%d_Point1", i);

			const char* helper_point1 = read_ini_string(pIni, sect_index, 
				strName, "");

			SafeSprintf(strName, sizeof(strName), "saberarc%d_Point2", i);

			const char* helper_point2 = read_ini_string(pIni, sect_index, 
				strName, "");

			SafeSprintf(strName, sizeof(strName), "saberarc%d_Name", i);

			const char* saberarc_name = read_ini_string(pIni, sect_index, 
				strName, "");

			SafeSprintf(strName, sizeof(strName), "saberarc%d_IsStart", i);

			bool is_start = pIni->FindSectionItemIndex(sect_index, strName,
				item_index);

			saberarc_info_t* sa_info = CORE_NEW(saberarc_info_t);

			sa_info->is_saber_start = is_start;
			sa_info->saberarc_name = saberarc_name;
			sa_info->helper_name0 = helper_point1;
			sa_info->helper_name1 = helper_point2;

			m_SaberArcInfo.push_back(sa_info);
		}
	}

	// ��Դ�����ļ�
	m_strLightFile = read_ini_string(pIni, sect_index, "light_file", "");

	// ��Դ����
	int light_num = read_ini_integer(pIni, sect_index, "light_number", 0);

	if ((m_strLightFile.empty() && (light_num > 0)) 
		|| (!m_strLightFile.empty() && (light_num <= 0)))
	{
		CORE_TRACE("(CEffectData::LoadIni)Light config error");
		CORE_TRACE(pIni->GetFileName());
		CORE_TRACE(em_name);
		return false;
	}

	if (light_num > 0)
	{
		char strName[32];

		for (int i = 0; i < light_num; ++i)
		{
			// ��������
			SafeSprintf(strName, sizeof(strName), "light_name%d", i);

			const char* light_name = read_ini_string(pIni, sect_index, 
				strName, "");

			SafeSprintf(strName, sizeof(strName), "light_helper%d", i);

			const char* helper_name = read_ini_string(pIni, sect_index, 
				strName, "");

			// ��Դ��������λ��
			SafeSprintf(strName, sizeof(strName), "light_visible_%d", i);

			int emit_open = read_ini_integer(pIni, sect_index, strName, -1);

			// ��Դ�������ر�λ��
			SafeSprintf(strName, sizeof(strName), "light_invisible_%d", i);

			int emit_close = read_ini_integer(pIni, sect_index, strName, -1);

			light_info_t* light_info = CORE_NEW(light_info_t);

			light_info->light_name = light_name;
			light_info->helper_name = helper_name;
			light_info->open_emit = emit_open;
			light_info->close_emit = emit_close;

			m_LightInfo.push_back(light_info);
		}
	}

	// ��ȡ���Ӳü�
	if (pIni->FindSectionItemIndex(sect_index, "ClipPlaneEnable", item_index))
	{
		m_bClipPlaneEnable = (atoi(pIni->GetSectionItemValue(sect_index, item_index)) == 1);
	}
	else
	{
		m_bClipPlaneEnable = false;
	}

	return true;
}
