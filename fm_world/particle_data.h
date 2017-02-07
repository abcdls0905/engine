//--------------------------------------------------------------------
// �ļ���:		particle_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PARTICLE_DATA_H
#define _PARTICLE_DATA_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"


#include "../visual/fm_header.h"

// ��������

class IIniFile;

class CParticleData
{
public:
	// ������ʵ��
	static CParticleData* NewInstance();

	enum { TRACK_EVENT_MAX = 16 };

	enum
	{
		AXIS_NONE,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	struct track_event_t
	{
		int time;
		float value;
	};
	
	track_event_t m_Visibility[TRACK_EVENT_MAX];
	track_event_t m_ParticleDir[TRACK_EVENT_MAX];
	track_event_t m_ParticleSize[TRACK_EVENT_MAX];
	track_event_t m_ParticleSpeed[TRACK_EVENT_MAX];
	track_event_t m_ParticleSpin[TRACK_EVENT_MAX];
	track_event_t m_ParticleAngle[TRACK_EVENT_MAX];
	track_event_t m_WindEffect[TRACK_EVENT_MAX];
	track_event_t m_GravityEffect[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusX[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusY[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusZ[TRACK_EVENT_MAX];
	//track_event_t m_ColorRed[TRACK_EVENT_MAX];
	//track_event_t m_ColorGreen[TRACK_EVENT_MAX];
	//track_event_t m_ColorBlue[TRACK_EVENT_MAX];

	// ���ɸı����±��������˳��(GPU���ݶ����崫��)
	FmVec4 m_vVisibilityTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpeedTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpeedDelta[TRACK_EVENT_MAX];
	FmVec4 m_vWindEffectTrack[TRACK_EVENT_MAX];
	FmVec4 m_vWindEffectDelta[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSizeTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpinTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpinDelta[TRACK_EVENT_MAX];
	
	core_string m_strTextureName;	// ��ͼ�ļ���
	core_string m_strRandomColorTex; // �����ɫ��ͼ
	core_string m_strRefractionTex; // ������ͼ
	core_string m_strEffectName; // ��ʾЧ��
	core_string m_strNextParticle; // ������ɺ��������ŵ���һ������
	core_string m_strBillboardType; // �����ģʽ
	core_string m_strCalculateMode; // ���Ӽ���ģʽ
	
	float m_fDirectionDeviation;	// ����ƫ��
	float m_fWindEffect;			// ���Ӱ��ϵ��
	float m_fGravity;				// ����Ӱ��
	float m_fInitSpeed;				// ��ʼ�ٶ�
	float m_fSpeedDeviation;		// �ٶ�ƫ��
	float m_fInitSize;				// ��ʼ��С
	float m_fSizeDeviation;			// ��Сƫ��
	float m_fSpin;					// ��ת
	float m_fSpinDeviation;			// ��תƫ��
	float m_fWeight;				// ����
	float m_fWeightDeviation;		// ����ƫ��

	float m_fEmitRadiusX;			// ���䷶Χ
	float m_fEmitRadiusY;
	float m_fEmitRadiusZ;
	float m_fEmitRadiusXRand;		// ���䷶Χ���ƫ��
	float m_fEmitRadiusYRand;
	float m_fEmitRadiusZRand;
	
	float m_fTrackPointRadius;		// ���հ뾶
	float m_fSurfaceOffset;			// ��ƽ��ľ���
	float m_fEmissionTime;			// ����ʱ��
	float m_fEmissionTimeRand;		// ����ʱ�����ֵ
	float m_fChaosVal;				// λ�ö���ֵ
	float m_fSpeedChaos;			// �ٶȶ���ֵ
	float m_fAspectRatio;			// ������ͼ�������
	float m_fLuminance;				// ��������
	float m_fDistortionIntensity;	// Ť��ǿ��
	
	int m_nTextureFrame;			// ��ͼ�ֿ�����
	int m_nParticleNum;				// ��������
	int m_nParticleLife;			// ��������ʱ��
	int m_nTailLife;				// ��β����ʱ��
	int m_nOneEmitNumber;			// һ�η������������
	int m_nParticleFrame;			// ������ͼ������
	int m_nOneAxisNotRand;			// ����һ����ķ��䷽��
	unsigned int m_nParticleColor;	// ������ɫ
	
	bool m_bRepeat;					// �ظ�����
	bool m_bTrackPointReturn;		// ���ձ�־
	bool m_bTrackPointDisappear;	// �������ʧ
	bool m_bLocalSpaceEmitter;		// ���������������λ��
	bool m_bLocalAngleEmitter;		// ��������������Է���
	bool m_bAngleLinkSpeed;			// �������������ٶȷ���
	bool m_bAngleLinkEmitter;		// �����������ķ���������
	bool m_bRandomDirection;		// ��������־
	bool m_bFlashTail;				// ������β��־
	bool m_bFixupTailSize;			// ������β�ߴ�
	bool m_bDelayUnload;			// �ӳ�ж������
	bool m_bNotDownLevel;			// ��������ʾ
	bool m_bColorAni;				// ��ɫ����

	bool m_bStaticBoundBox;			// �Ƿ�̶���Χ��
	FmVec3 m_vBoxMin;			// ��Χ��
	FmVec3 m_vBoxMax;

public:
	CParticleData();
	~CParticleData();

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

	// �������ļ���������
	bool LoadIni(IIniFile* ini, const char* psname, const char* tex_path);
	
	// ��ʼ��������Ⱦ�Ĺ켣����
	void InitTrackData();

	// ������ӹ켣ֵ
	float GetTrackValue(track_event_t* track, float time);
	
private:
	// �������ӹ켣����
	bool BuildTrack(IIniFile* ini, track_event_t* track, 
		const char* psname, size_t sect_index, const char* key_name);
	
private:
	core_string m_strIniName;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nRefs;
};

#endif // _PARTICLE_DATA_H
