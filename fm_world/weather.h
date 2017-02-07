//--------------------------------------------------------------------
// �ļ���:		weather.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WEATHER_H
#define _WEATHER_H

#include "../visual/i_weather.h"
// ����

class IRenderContext;

class Weather: public IWeather
{
public:
	Weather();
	virtual ~Weather();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ƿ���ָ����
	void SetFogExp(bool value);
	bool GetFogExp() const;

	// �Ƿ���������
	void SetFogLinear(bool value);
	bool GetFogLinear() const;

	// ָ������ɫ
	void SetFogExpColorString(const char* value);
	result_string GetFogExpColorString() const;

	// ��������ɫ
	void SetFogColorString(const char* value);
	result_string GetFogColorString() const;
	
	// ָ����Ũ��
	void SetFogDensity(float value);
	float GetFogDensity() const;

	// ���������ʼ����
	void SetFogStart(float value);
	float GetFogStart() const;

	// ������Ľ�������
	void SetFogEnd(float value);
	float GetFogEnd() const;
	
	// ������߶�
	void SetFogHeight(float value);
	float GetFogHeight() const;

	// �Ƿ����߶���
	void SetHeightFog(bool value);
	bool GetHeightFog() const;

	// �߶�����ɫ
	void SetHeightFogColorString(const char* value);
	result_string GetHeightFogColorString() const;

	// �߶������ʼ����
	void SetHeightFogStart(float value);
	float GetHeightFogStart() const;

	// �߶���Ľ�������
	void SetHeightFogEnd(float value);
	float GetHeightFogEnd() const;

	// ����Դ�ķ���
	void SetSunDirectionString(const char* value);
	result_string GetSunDirectionString() const;

	// ����Դ��ɫ
	void SetDiffuseColorString(const char* value);
	result_string GetDiffuseColorString() const;
	
	// ����Դ����ϵ��
	void SetDiffuseIntensity(float value);
	float GetDiffuseIntensity() const;

	// ��������ɫ
	void SetAmbientColorString(const char* value);
	result_string GetAmbientColorString() const;

	// �������淴�����ɫ
	void SetSpecularColorString(const char* value);
	result_string GetSpecularColorString() const;

	// ����������ϵ��
	void SetAmbientIntensity(float value);
	float GetAmbientIntensity() const;

	// ���������ϵ��
	void SetSunGlowIntensity(float value);
	float GetSunGlowIntensity() const;

	// �������淴�������ϵ��
	void SetSpecularIntensity(float value);
	float GetSpecularIntensity() const;

	// ����
	void SetWindAngle(float value);
	float GetWindAngle() const;

	// ����
	void SetWindSpeed(float value);
	float GetWindSpeed() const;

	// �ø߶ȽǺͷ�λ���������ⷽ��
	bool MakeSunDirection(float height_angle, float azimuth_angle);
	
private:
	IRenderContext* m_pContext;
};

#endif // _WEATHER_H
