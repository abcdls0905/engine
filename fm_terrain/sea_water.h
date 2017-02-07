//--------------------------------------------------------------------
// �ļ���:		sea_water.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��11��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SEA_WATER_H
#define _SEA_WATER_H

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_render.h"
#include "sea_data.h"
#include "../visual/i_sea_water.h"

// ����ˮ������

class Terrain;
class CSeaBase;

class SeaWater: public ISeaWater
{
public:
	SeaWater();
	virtual ~SeaWater();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	void SetSeaBase(CSeaBase* value) { m_pSeaBase = value; }
	CSeaBase* GetSeaBase() const { return m_pSeaBase; }

	// ��������
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// ��������ֵ
	void SetIndex(size_t value);
	
	// ��ӳ��յ���ɫ
	void SetSkyColorString(const char* value);
	result_string GetSkyColorString() const;
	
	// ˮ����ɫ
	void SetWaterColorString(const char* value);
	result_string GetWaterColorString() const;

	// ������ͼ�Ƿ���Ҫˢ��
	void SetCubeMapStatic(bool value);
	bool GetCubeMapStatic() const;

	// �Ƿ�������
	void SetReflect(bool value);
	bool GetReflect() const;

	// ��������߶�
	void SetBaseHeight(float value);
	float GetBaseHeight() const;

	// ���浱ǰ�߶�
	float GetCurrentHeight(float x, float y);
	
	// ���߶Ȳ�
	void SetMaxSeaHeight(float value);
	float GetMaxSeaHeight() const;
	
	// ˮ�浹Ӱ�ķ�Χ����
	void SetReflectionRatio(float value);
	float GetReflectionRatio() const;

	// ������
	void SetWaveGradient0(float value);
	float GetWaveGradient0() const;

	void SetWaveGradient1(float value);
	float GetWaveGradient1() const;

	void SetWaveGradient2(float value);
	float GetWaveGradient2() const;

	void SetWaveGradient3(float value);
	float GetWaveGradient3() const;

	void SetWaveGradient4(float value);
	float GetWaveGradient4() const;

	// ���
	void SetWaveSwing0(float value);
	float GetWaveSwing0() const;

	void SetWaveSwing1(float value);
	float GetWaveSwing1() const;

	void SetWaveSwing2(float value);
	float GetWaveSwing2() const;

	void SetWaveSwing3(float value);
	float GetWaveSwing3() const;

	void SetWaveSwing4(float value);
	float GetWaveSwing4() const;

	// ��Ƶ��
	void SetWaveAngleFreq0(float value);
	float GetWaveAngleFreq0() const;

	void SetWaveAngleFreq1(float value);
	float GetWaveAngleFreq1() const;

	void SetWaveAngleFreq2(float value);
	float GetWaveAngleFreq2() const;

	void SetWaveAngleFreq3(float value);
	float GetWaveAngleFreq3() const;

	void SetWaveAngleFreq4(float value);
	float GetWaveAngleFreq4() const;

	// ��λ
	void SetWavePhaX0(float value);
	float GetWavePhaX0() const;
	void SetWavePhaZ0(float value);
	float GetWavePhaZ0() const;

	void SetWavePhaX1(float value);
	float GetWavePhaX1() const;
	void SetWavePhaZ1(float value);
	float GetWavePhaZ1() const;

	void SetWavePhaX2(float value);
	float GetWavePhaX2() const;
	void SetWavePhaZ2(float value);
	float GetWavePhaZ2() const;

	void SetWavePhaX3(float value);
	float GetWavePhaX3() const;
	void SetWavePhaZ3(float value);
	float GetWavePhaZ3() const;

	void SetWavePhaX4(float value);
	float GetWavePhaX4() const;
	void SetWavePhaZ4(float value);
	float GetWavePhaZ4() const;

	// ��ֱ�ڲ������ˮƽ����
	void SetWaveDirectionX0(float value);
	float GetWaveDirectionX0() const;
	void SetWaveDirectionZ0(float value);
	float GetWaveDirectionZ0() const;

	void SetWaveDirectionX1(float value);
	float GetWaveDirectionX1() const;
	void SetWaveDirectionZ1(float value);
	float GetWaveDirectionZ1() const;

	void SetWaveDirectionX2(float value);
	float GetWaveDirectionX2() const;
	void SetWaveDirectionZ2(float value);
	float GetWaveDirectionZ2() const;

	void SetWaveDirectionX3(float value);
	float GetWaveDirectionX3() const;
	void SetWaveDirectionZ3(float value);
	float GetWaveDirectionZ3() const;

	void SetWaveDirectionX4(float value);
	float GetWaveDirectionX4() const;
	void SetWaveDirectionZ4(float value);
	float GetWaveDirectionZ4() const;

	// NormalMap Scale
	void SetNormalMapScale0(float value);
	float GetNormalMapScale0() const;

	void SetNormalMapScale1(float value);
	float GetNormalMapScale1() const;

	void SetNormalMapScale2(float value);
	float GetNormalMapScale2() const;

	void SetNormalMapScale3(float value);
	float GetNormalMapScale3() const;

	// NormalMap Speed
	void SetNormalMapSpeedX0(float value);
	float GetNormalMapSpeedX0() const;
	void SetNormalMapSpeedZ0(float value);
	float GetNormalMapSpeedZ0() const;

	void SetNormalMapSpeedX1(float value);
	float GetNormalMapSpeedX1() const;
	void SetNormalMapSpeedZ1(float value);
	float GetNormalMapSpeedZ1() const;

	void SetNormalMapSpeedX2(float value);
	float GetNormalMapSpeedX2() const;
	void SetNormalMapSpeedZ2(float value);
	float GetNormalMapSpeedZ2() const;

	void SetNormalMapSpeedX3(float value);
	float GetNormalMapSpeedX3() const;
	void SetNormalMapSpeedZ3(float value);
	float GetNormalMapSpeedZ3() const;

	// Distance Scale
	void SetDistanceScale(float value);
	float GetDistanceScale() const;

	// Reflection Intensity
	void SetReflectionInten(float value);
	float GetReflectionInten() const;

	// Refraction Intensity
	void SetRefractionInten(float value);
	float GetRefractionInten() const;

	// Reflection Weight
	void SetReflectionWeight(float value);
	float GetReflectionWeight() const;

	// Refraction Weight
	void SetRefractionWeight(float value);
	float GetRefractionWeight() const;

	// Sun Level
	void SetSunLevel(float value);
	float GetSunLevel() const;

	// Reflection Sun Intensity
	void SetReflectionSunInten(float value);
	float GetReflectionSunInten() const;

	// ̫��������ɫ
	void SetSunColorStartString(const char* value);
	result_string GetSunColorStartString() const;

	// ̫����Ե��ɫ
	void SetSunColorEndString(const char* value);
	result_string GetSunColorEndString() const;

	// NormalMap File
	void SetNormalMapFileName1(const char* value);
	const char* GetNormalMapFileName1() const;
	void SetNormalMapFileName2(const char* value);
	const char* GetNormalMapFileName2() const;
	void SetNormalMapFileName3(const char* value);
	const char* GetNormalMapFileName3() const;
	void SetNormalMapFileName4(const char* value);
	const char* GetNormalMapFileName4() const;

	// ��ɫˮ����ͼ
	void SetWhiteWaterMap(const char* value);
	const char* GetWhiteWaterMap() const;

	// ������ͼ
	void SetNoiseMap(const char* value);
	const char* GetNoiseMap() const;

	// ˮ�¿�ʴ�仯�ٶ�
	void SetCausticSpeed(float value);
	float GetCausticSpeed() const;

	// ˮ�¿�ʴ����
	void SetCausticScale(float value);
	float GetCausticScale() const;

	// ˮ�¿�ʴ����
	void SetCausticBright(float value);
	float GetCausticBright() const;

	// ˮ�¿�ʴ����
	void SetCausticFocus(float value);
	float GetCausticFocus() const;

	// ˮ�¿�ʴ��ͼ
	void SetCausticMap(const char* value);
	const char* GetCausticMap() const;

	// ˢ��NormalMap
	bool RefreshNormalMap(const char* pszFileName1, const char* pszFileName2, 
		const char* pszFileName3, const char* pszFileName4);
	// ˢ�°�ɫˮ����ͼ
	bool RefreshWhiteWaterMap(const char* pszFileName);
	// ˢ��ˮ�¿�ʴ��ͼ
	bool RefreshCausticMap(const char* pszFileName);

	// ��ʼ����ˮ����
	bool InitSeaData();
	// ���º�ˮ����
	bool UpdateSeaData();
	// ��ʼ��ʾ
	bool Start();

private:
	SeaWater(const SeaWater&);
	SeaWater& operator=(const SeaWater&);

private:
	core_string m_strName;
	unsigned int m_nHash;
	Terrain* m_pTerrain;
	CSeaBase* m_pSeaBase;
	sea_data_t m_SeaData;
};

#endif // _SEA_WATER_H
