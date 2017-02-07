//--------------------------------------------------------------------
// 文件名:		weather.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WEATHER_H
#define _WEATHER_H

#include "../visual/i_weather.h"
// 天气

class IRenderContext;

class Weather: public IWeather
{
public:
	Weather();
	virtual ~Weather();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 是否开启指数雾
	void SetFogExp(bool value);
	bool GetFogExp() const;

	// 是否开启线性雾
	void SetFogLinear(bool value);
	bool GetFogLinear() const;

	// 指数雾颜色
	void SetFogExpColorString(const char* value);
	result_string GetFogExpColorString() const;

	// 线性雾颜色
	void SetFogColorString(const char* value);
	result_string GetFogColorString() const;
	
	// 指数雾浓度
	void SetFogDensity(float value);
	float GetFogDensity() const;

	// 线性雾的起始距离
	void SetFogStart(float value);
	float GetFogStart() const;

	// 线性雾的结束距离
	void SetFogEnd(float value);
	float GetFogEnd() const;
	
	// 线性雾高度
	void SetFogHeight(float value);
	float GetFogHeight() const;

	// 是否开启高度雾
	void SetHeightFog(bool value);
	bool GetHeightFog() const;

	// 高度雾颜色
	void SetHeightFogColorString(const char* value);
	result_string GetHeightFogColorString() const;

	// 高度雾的起始距离
	void SetHeightFogStart(float value);
	float GetHeightFogStart() const;

	// 高度雾的结束距离
	void SetHeightFogEnd(float value);
	float GetHeightFogEnd() const;

	// 主光源的方向
	void SetSunDirectionString(const char* value);
	result_string GetSunDirectionString() const;

	// 主光源颜色
	void SetDiffuseColorString(const char* value);
	result_string GetDiffuseColorString() const;
	
	// 主光源亮度系数
	void SetDiffuseIntensity(float value);
	float GetDiffuseIntensity() const;

	// 环境光颜色
	void SetAmbientColorString(const char* value);
	result_string GetAmbientColorString() const;

	// 环境镜面反射光颜色
	void SetSpecularColorString(const char* value);
	result_string GetSpecularColorString() const;

	// 环境光亮度系数
	void SetAmbientIntensity(float value);
	float GetAmbientIntensity() const;

	// 阳光光亮度系数
	void SetSunGlowIntensity(float value);
	float GetSunGlowIntensity() const;

	// 环境镜面反射光亮度系数
	void SetSpecularIntensity(float value);
	float GetSpecularIntensity() const;

	// 风向
	void SetWindAngle(float value);
	float GetWindAngle() const;

	// 风速
	void SetWindSpeed(float value);
	float GetWindSpeed() const;

	// 用高度角和方位角生成阳光方向
	bool MakeSunDirection(float height_angle, float azimuth_angle);
	
private:
	IRenderContext* m_pContext;
};

#endif // _WEATHER_H
