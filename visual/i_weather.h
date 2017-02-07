#ifndef _I_WEATHER_H
#define _I_WEATHER_H

#include "../public/module.h"

class IWeather: public IEntity
{
public:
	// �Ƿ���ָ����
	virtual void SetFogExp(bool value) = 0;
	virtual bool GetFogExp() const = 0;

	// �Ƿ���������
	virtual void SetFogLinear(bool value) = 0;
	virtual bool GetFogLinear() const = 0;

	// ָ������ɫ
	virtual void SetFogExpColorString(const char* value) = 0;
	virtual result_string GetFogExpColorString() const = 0;

	// ��������ɫ
	virtual void SetFogColorString(const char* value) = 0;
	virtual result_string GetFogColorString() const = 0;
	
	// ָ����Ũ��
	virtual void SetFogDensity(float value) = 0;
	virtual float GetFogDensity() const = 0;

	// ���������ʼ����
	virtual void SetFogStart(float value) = 0;
	virtual float GetFogStart() const = 0;

	// ������Ľ�������
	virtual void SetFogEnd(float value) = 0;
	virtual float GetFogEnd() const = 0;
	
	// ������߶�
	virtual void SetFogHeight(float value) = 0;
	virtual float GetFogHeight() const = 0;

	// ����Դ�ķ���
	virtual void SetSunDirectionString(const char* value) = 0;
	virtual result_string GetSunDirectionString() const = 0;

	// ����Դ��ɫ
	virtual void SetDiffuseColorString(const char* value) = 0;
	virtual result_string GetDiffuseColorString() const = 0;
	
	// ����Դ����ϵ��
	virtual void SetDiffuseIntensity(float value) = 0;
	virtual float GetDiffuseIntensity() const = 0;

	// ��������ɫ
	virtual void SetAmbientColorString(const char* value) = 0;
	virtual result_string GetAmbientColorString() const = 0;

	// �������淴�����ɫ
	virtual void SetSpecularColorString(const char* value) = 0;
	virtual result_string GetSpecularColorString() const = 0;

	// ����������ϵ��
	virtual void SetAmbientIntensity(float value) = 0;
	virtual float GetAmbientIntensity() const = 0;

	// ���������ϵ��
	virtual void SetSunGlowIntensity(float value) = 0;
	virtual float GetSunGlowIntensity() const = 0;

	// �������淴�������ϵ��
	virtual void SetSpecularIntensity(float value) = 0;
	virtual float GetSpecularIntensity() const = 0;

	// ����
	virtual void SetWindAngle(float value) = 0;
	virtual float GetWindAngle() const = 0;

	// ����
	virtual void SetWindSpeed(float value) = 0;
	virtual float GetWindSpeed() const = 0;

	// �ø߶ȽǺͷ�λ���������ⷽ��
	virtual bool MakeSunDirection(float height_angle, float azimuth_angle) = 0;
};
#endif
