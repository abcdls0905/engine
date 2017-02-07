//--------------------------------------------------------------------
// �ļ���:		particle_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PARTICLE_INFO_H
#define _PARTICLE_INFO_H

#include "../public/module.h"
#include "particle_data.h"

// ���Ӳ���

class ParticleInfo: public IEntity
{
public:
	ParticleInfo();
	virtual ~ParticleInfo();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �����������
	CParticleData* GetData() const { return m_pData; }

	// ��Ӧ�����������ļ���
	void SetIniName(const char* value);
	const char* GetIniName() const;
	
	// ��ͼ�ļ���
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// �����ɫ��ͼ�ļ���
	void SetRandomColorTex(const char* value);
	const char* GetRandomColorTex() const;

	// ������ͼ��
	void SetRefractionTex(const char* value);
	const char* GetRefractionTex() const;

	// ��ʾЧ��
	void SetEffectName(const char* value);
	const char* GetEffectName() const;

	// �����ģʽ
	void SetBillboardType(const char* value); 
	const char* GetBillboardType() const;

	// ���Ӽ���ģʽ
	void SetCalculateMode(const char* value); 
	const char* GetCalculateMode() const;

	// ������������
	void SetNextParticle(const char* value);
	const char* GetNextParticle() const;
	
	// ����ƫ��
	void SetDirectionDeviation(float value);
	float GetDirectionDeviation() const;

	// ���Ӱ��ϵ��
	void SetWindEffect(float value);
	float GetWindEffect() const;

	// ����Ӱ��
	void SetGravity(float value);
	float GetGravity() const;

	// ��ʼ��С
	void SetInitSize(float value);
	float GetInitSize() const;

	// ��Сƫ��
	void SetSizeDeviation(float value);
	float GetSizeDeviation() const;

	// �������
	void SetAspectRatio(float value);
	float GetAspectRatio() const;

	// ��ʼ�ٶ�
	void SetInitSpeed(float value);
	float GetInitSpeed() const;

	// �ٶ�ƫ��
	void SetSpeedDeviation(float value);
	float GetSpeedDeviation() const;

	// ��ת
	void SetSpin(float value);
	float GetSpin() const;

	// ��תƫ��
	void SetSpinDeviation(float value);
	float GetSpinDeviation() const;

	// ����
	void SetWeight(float value);
	float GetWeight() const;

	// ����ƫ��
	void SetWeightDeviation(float value);
	float GetWeightDeviation() const;

	// ����뾶X
	void SetEmitRadiusX(float value);
	float GetEmitRadiusX() const;

	// ����뾶Y
	void SetEmitRadiusY(float value);
	float GetEmitRadiusY() const;

	// ����뾶Z
	void SetEmitRadiusZ(float value);
	float GetEmitRadiusZ() const;

	// ����뾶X�����Χ
	void SetEmitRadiusXRand(float value);
	float GetEmitRadiusXRand() const;

	// ����뾶Y�����Χ
	void SetEmitRadiusYRand(float value);
	float GetEmitRadiusYRand() const;

	// ����뾶Z�����Χ
	void SetEmitRadiusZRand(float value);
	float GetEmitRadiusZRand() const;

	// ���հ뾶
	void SetTrackPointRadius(float value);
	float GetTrackPointRadius() const;

	// ��ƽ��ľ���
	void SetSurfaceOffset(float value);
	float GetSurfaceOffset() const;

	// ����ʱ��
	void SetEmissionTime(float value);
	float GetEmissionTime() const;
	
	// ����ʱ�����ֵ
	void SetEmissionTimeRand(float value);
	float GetEmissionTimeRand() const;
	
	// λ�ö���ֵ
	void SetChaosValue(float value);
	float GetChaosValue() const;
	
	// �ٶȶ���ֵ
	void SetSpeedChaos(float value);
	float GetSpeedChaos() const;

	// ��ͼ�ֿ�����
	void SetTextureFrame(int value);
	int GetTextureFrame() const;

	// ��������
	void SetParticleNum(int value);
	int GetParticleNum() const;
	
	// ��������ʱ��
	void SetParticleLife(int value);
	int GetParticleLife() const;
	
	// ��β����ʱ��
	void SetTailLife(int value);
	int GetTailLife() const;

	// һ�η������������
	void SetOneEmitNumber(int value);
	int GetOneEmitNumber() const;

	// ������ͼ������
	void SetParticleFrame(int value);
	int GetParticleFrame() const;

	// ������ɫ
	void SetParticleColor(unsigned int value);
	unsigned int GetParticleColor() const;

	// ������ɫ
	void SetParticleColorString(const char* value);
	result_string GetParticleColorString() const;

	// �ظ�����
	void SetRepeat(bool value);
	bool GetRepeat() const;

	// ���ձ�־
	void SetTrackPointReturn(bool value);
	bool GetTrackPointReturn() const;

	// �������ʧ
	void SetTrackPointDisappear(bool value);
	bool GetTrackPointDisappear() const;

	// ��������������λ��
	void SetLocalSpaceEmitter(bool value);
	bool GetLocalSpaceEmitter() const;

	// �������������Է���
	void SetLocalAngleEmitter(bool value);
	bool GetLocalAngleEmitter() const;

	// �������������ٶȷ���
	void SetAngleLinkSpeed(bool value);
	bool GetAngleLinkSpeed() const;

	// �����������ķ���������
	void SetAngleLinkEmitter(bool value);
	bool GetAngleLinkEmitter() const;

	// ��������־
	void SetRandomDirection(bool value);
	bool GetRandomDirection() const;

	// ����һ����ķ��䷽��
	void SetOneAxisNotRand(const char* value);
	result_string GetOneAxisNotRand() const;

	// ������β��־
	void SetFlashTail(bool value);
	bool GetFlashTail() const;

	// ������β�ߴ��Ƿ�̶�
	void SetFixupTailSize(bool value);
	bool GetFixupTailSize() const;

	// �켣��Ϣ
	bool AddTrack(const char* type, int time, float value);
	bool SetTrack(const char* type, int time, float value);
	bool SetTrackByIndex(const char* type, int index, int time, float value);
	bool ClearTrack(const char* type);
	int GetTrackCount(const char* type);
	int GetTrackTime(const char* type, int index);
	float GetTrackValue(const char* type, int index);

	// ��������
	void SetLuminance(float value);
	float GetLuminance() const;

	// Ť��ǿ��
	void SetDistortionIntensity(float value);
	float GetDistortionIntensity() const;

	// �ӳ�ж������
	void SetDelayUnload(bool value);
	bool GetDelayUnload() const;

	// ʹ����ɫ����
	void SetColorAni(bool value);
	bool GetColorAni() const;

	// ��������ʾ
	void SetNotDownLevel(bool value);
	bool GetNotDownLevel() const;

private:
	CParticleData::track_event_t* GetTrackEvent(const char* type);

private:
	CParticleData* m_pData;
};

#endif // _PARTICLE_INFO_H
