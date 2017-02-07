//--------------------------------------------------------------------
// �ļ���:		effect_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��10��18��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _EFFECT_DATA_H
#define _EFFECT_DATA_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/tick_timer.h"
#include "../utils/array_pod.h"

// ��Ч���ݻ���

class IRender;
class IIniFile;

class CEffectData
{
public:
	struct particle_info_t
	{
		core_string helper_name; // ��������
		core_string particle_name; // ������
		int open_emit;				// �����ӷ��俪��
		int close_emit;				// �ر����ӷ��俪��
	};

	struct saberarc_info_t
	{
		core_string helper_name0;	// ������0
		core_string helper_name1;	// ������1
		core_string saberarc_name; // ������
		bool is_saber_start;
	};

	struct light_info_t
	{
		core_string helper_name; // ��������
		core_string light_name; // �ƹ���
		int open_emit;				// �򿪵ƹⷢ�俪��
		int close_emit;				// �رյƹⷢ�俪��
	};

	TArrayPod<particle_info_t*, 10, TCoreAlloc> m_ParticleInfo;	// ��������
	TArrayPod<saberarc_info_t*, 2, TCoreAlloc> m_SaberArcInfo;	// �˶���β
	TArrayPod<light_info_t*, 1, TCoreAlloc> m_LightInfo;		// �ƹ�

	core_string m_strModelFile; // ��Чģ���ļ�
	core_string m_strMaterialFile; // ��Чģ�Ͳ���
	core_string m_strParticleFile; // �����ļ���
	core_string m_strSaberArcFile; // �����ļ���
	core_string m_strLightFile; // ��Դ�ļ���
	bool m_bClipPlaneEnable;					// �ü��濪��

public:
	// ������ʵ��
	static CEffectData* NewInstance(IRender* pRender);

	CEffectData(IRender* pRender);
	~CEffectData();
	
	// �����ļ���
	void SetIniName(const char* value) { m_strIniName = value; }
	const char* GetIniName() const { return m_strIniName.c_str(); }

	// ����
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// ��ϣֵ
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// ������ü���
	int GetRefs() const;
	// ��������
	int IncRefs();
	// �ͷ�����
	void Release();

	// ����Ҫ���ݻ������������
	void SetNoCache(bool value) { m_bNoCache = value; }
	bool GetNoCache() const { return m_bNoCache; }

	// δʹ��ʱ��
	void SetUnuseTime(float value) { m_fUnuseTime = value; }
	float GetUnuseTime() const { return m_fUnuseTime; }

	// �������ļ���������
	bool LoadIni(IIniFile* pIni, const char* em_name);

private:
	IRender* m_pRender;
	core_string m_strIniName;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nRefs;
	bool m_bNoCache;
	float m_fUnuseTime;
};

#endif // _EFFECT_DATA_H
