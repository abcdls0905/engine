#ifndef _I_WEATHER_H
#define _I_WEATHER_H

#include "../public/module.h"

class IWeather: public IEntity
{
public:
	// 是否开启指数雾
	virtual void SetFogExp(bool value) = 0;
	virtual bool GetFogExp() const = 0;

	// 是否开启线性雾
	virtual void SetFogLinear(bool value) = 0;
	virtual bool GetFogLinear() const = 0;

	// 指数雾颜色
	virtual void SetFogExpColorString(const char* value) = 0;
	virtual result_string GetFogExpColorString() const = 0;

	// 线性雾颜色
	virtual void SetFogColorString(const char* value) = 0;
	virtual result_string GetFogColorString() const = 0;
	
	// 指数雾浓度
	virtual void SetFogDensity(float value) = 0;
	virtual float GetFogDensity() const = 0;

	// 线性雾的起始距离
	virtual void SetFogStart(float value) = 0;
	virtual float GetFogStart() const = 0;

	// 线性雾的结束距离
	virtual void SetFogEnd(float value) = 0;
	virtual float GetFogEnd() const = 0;
	
	// 线性雾高度
	virtual void SetFogHeight(float value) = 0;
	virtual float GetFogHeight() const = 0;

	// 主光源的方向
	virtual void SetSunDirectionString(const char* value) = 0;
	virtual result_string GetSunDirectionString() const = 0;

	// 主光源颜色
	virtual void SetDiffuseColorString(const char* value) = 0;
	virtual result_string GetDiffuseColorString() const = 0;
	
	// 主光源亮度系数
	virtual void SetDiffuseIntensity(float value) = 0;
	virtual float GetDiffuseIntensity() const = 0;

	// 环境光颜色
	virtual void SetAmbientColorString(const char* value) = 0;
	virtual result_string GetAmbientColorString() const = 0;

	// 环境镜面反射光颜色
	virtual void SetSpecularColorString(const char* value) = 0;
	virtual result_string GetSpecularColorString() const = 0;

	// 环境光亮度系数
	virtual void SetAmbientIntensity(float value) = 0;
	virtual float GetAmbientIntensity() const = 0;

	// 阳光光亮度系数
	virtual void SetSunGlowIntensity(float value) = 0;
	virtual float GetSunGlowIntensity() const = 0;

	// 环境镜面反射光亮度系数
	virtual void SetSpecularIntensity(float value) = 0;
	virtual float GetSpecularIntensity() const = 0;

	// 风向
	virtual void SetWindAngle(float value) = 0;
	virtual float GetWindAngle() const = 0;

	// 风速
	virtual void SetWindSpeed(float value) = 0;
	virtual float GetWindSpeed() const = 0;

	// 用高度角和方位角生成阳光方向
	virtual bool MakeSunDirection(float height_angle, float azimuth_angle) = 0;
};
#endif
