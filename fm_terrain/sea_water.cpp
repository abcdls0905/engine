//--------------------------------------------------------------------
// 文件名:		sea_water.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月11日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "sea_water.h"
#include "terrain.h"
#include "sea_base.h"
#include "../visual/vis_utils.h"
#include "../public/var_list.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"

/// \file sea_water.cpp
/// \brief 地形水面类型

/// entity: SeaWater
/// \brief 地形水面类型实体
DECLARE_ENTITY(SeaWater, 0, IEntity);

/// readonly: string Name
/// \brief 水面类型名称
DECLARE_PROPERTY_GET(const char*, SeaWater, Name, GetName);
/// property: bool StopRefresh
/// \brief 停止刷新，调试用
//DECLARE_PROPERTY(bool, SeaWater, StopRefresh, GetStopRefresh, SetStopRefresh);
/// property: string SkyColor
/// \brief 反映天空的颜色
DECLARE_PROPERTY(result_string, SeaWater, SkyColor, GetSkyColorString, 
	SetSkyColorString);
/// property: string WaterColor
/// \brief 水面颜色
DECLARE_PROPERTY(result_string, SeaWater, WaterColor, GetWaterColorString, 
	SetWaterColorString);
/// property float ReflectionRatio
/// \brief 渲染水面环境贴图时的剪裁距离与当前视距的比例（0.0到1.0）
DECLARE_PROPERTY(float, SeaWater, ReflectionRatio, GetReflectionRatio, 
	SetReflectionRatio);
/// property: bool CubeMapStatic
/// \brief 环境贴图是否不需要刷新
DECLARE_PROPERTY(bool, SeaWater, CubeMapStatic, GetCubeMapStatic, 
	SetCubeMapStatic);
/// property: bool Reflect
/// \brief 是否开启反射
DECLARE_PROPERTY(bool, SeaWater, Reflect, GetReflect, SetReflect);
/// property: float BaseHeight
/// \brief 水面基准高度
DECLARE_PROPERTY(float, SeaWater, BaseHeight, GetBaseHeight, SetBaseHeight);
/// property: float MaxSeaHeight
/// \brief 最大水面高度差
DECLARE_PROPERTY(float, SeaWater, MaxSeaHeight, GetMaxSeaHeight, 
	SetMaxSeaHeight);

/// property: float WaveGradient0 WaveGradient1 WaveGradient2 WaveGradient3 
///           float WaveGradient4
/// \brief 波陡度
DECLARE_PROPERTY(float, SeaWater, WaveGradient0, GetWaveGradient0, 
	SetWaveGradient0);
DECLARE_PROPERTY(float, SeaWater, WaveGradient1, GetWaveGradient1, 
	SetWaveGradient1);
DECLARE_PROPERTY(float, SeaWater, WaveGradient2, GetWaveGradient2, 
	SetWaveGradient2);
DECLARE_PROPERTY(float, SeaWater, WaveGradient3, GetWaveGradient3, 
	SetWaveGradient3);
DECLARE_PROPERTY(float, SeaWater, WaveGradient4, GetWaveGradient4, 
	SetWaveGradient4);
/// property: float WaveSwing0 WaveSwing1 WaveSwing2 WaveSwing3 WaveSwing4
/// \brief 振幅
DECLARE_PROPERTY(float, SeaWater, WaveSwing0, GetWaveSwing0, SetWaveSwing0);
DECLARE_PROPERTY(float, SeaWater, WaveSwing1, GetWaveSwing1, SetWaveSwing1);
DECLARE_PROPERTY(float, SeaWater, WaveSwing2, GetWaveSwing2, SetWaveSwing2);
DECLARE_PROPERTY(float, SeaWater, WaveSwing3, GetWaveSwing3, SetWaveSwing3);
DECLARE_PROPERTY(float, SeaWater, WaveSwing4, GetWaveSwing4, SetWaveSwing4);
/// property: float WaveAngleFreq0 WaveAngleFreq1 WaveAngleFreq2 
///           float WaveAngleFreq3 WaveAngleFreq4
/// \brief 角频率
DECLARE_PROPERTY(float, SeaWater, WaveAngleFreq0, GetWaveAngleFreq0, 
	SetWaveAngleFreq0);
DECLARE_PROPERTY(float, SeaWater, WaveAngleFreq1, GetWaveAngleFreq1, 
	SetWaveAngleFreq1);
DECLARE_PROPERTY(float, SeaWater, WaveAngleFreq2, GetWaveAngleFreq2, 
	SetWaveAngleFreq2);
DECLARE_PROPERTY(float, SeaWater, WaveAngleFreq3, GetWaveAngleFreq3, 
	SetWaveAngleFreq3);
DECLARE_PROPERTY(float, SeaWater, WaveAngleFreq4, GetWaveAngleFreq4, 
	SetWaveAngleFreq4);
/// property: float WavePhaX0 WavePhaX1 WavePhaX2 WavePhaX3 WavePhaX4
///           float WavePhaZ0 WavePhaZ1 WavePhaZ2 WavePhaZ3 WavePhaZ4
/// \brief 相位
DECLARE_PROPERTY(float, SeaWater, WavePhaX0, GetWavePhaX0, SetWavePhaX0);
DECLARE_PROPERTY(float, SeaWater, WavePhaX1, GetWavePhaX1, SetWavePhaX1);
DECLARE_PROPERTY(float, SeaWater, WavePhaX2, GetWavePhaX2, SetWavePhaX2);
DECLARE_PROPERTY(float, SeaWater, WavePhaX3, GetWavePhaX3, SetWavePhaX3);
DECLARE_PROPERTY(float, SeaWater, WavePhaX4, GetWavePhaX4, SetWavePhaX4);

DECLARE_PROPERTY(float, SeaWater, WavePhaZ0, GetWavePhaZ0, SetWavePhaZ0);
DECLARE_PROPERTY(float, SeaWater, WavePhaZ1, GetWavePhaZ1, SetWavePhaZ1);
DECLARE_PROPERTY(float, SeaWater, WavePhaZ2, GetWavePhaZ2, SetWavePhaZ2);
DECLARE_PROPERTY(float, SeaWater, WavePhaZ3, GetWavePhaZ3, SetWavePhaZ3);
DECLARE_PROPERTY(float, SeaWater, WavePhaZ4, GetWavePhaZ4, SetWavePhaZ4);
/// property: float WaveDirectionX0 WaveDirectionX1 WaveDirectionX2 
///           float WaveDirectionX3 WaveDirectionX4
///           float WaveDirectionZ0 WaveDirectionZ1 WaveDirectionZ2 
///           float WaveDirectionZ3 WaveDirectionZ4
/// \brief 垂直于波阵面的水平方向
DECLARE_PROPERTY(float, SeaWater, WaveDirectionX0, GetWaveDirectionX0, 
	SetWaveDirectionX0);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionX1, GetWaveDirectionX1, 
	SetWaveDirectionX1);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionX2, GetWaveDirectionX2, 
	SetWaveDirectionX2);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionX3, GetWaveDirectionX3, 
	SetWaveDirectionX3);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionX4, GetWaveDirectionX4, 
	SetWaveDirectionX4);

DECLARE_PROPERTY(float, SeaWater, WaveDirectionZ0, GetWaveDirectionZ0, 
	SetWaveDirectionZ0);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionZ1, GetWaveDirectionZ1, 
	SetWaveDirectionZ1);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionZ2, GetWaveDirectionZ2, 
	SetWaveDirectionZ2);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionZ3, GetWaveDirectionZ3, 
	SetWaveDirectionZ3);
DECLARE_PROPERTY(float, SeaWater, WaveDirectionZ4, GetWaveDirectionZ4, 
	SetWaveDirectionZ4);
/// property: float NormalMapScale0 NormalMapScale1 NormalMapScale2 
///           float NormalMapScale3
/// \brief Normalmap Scale
DECLARE_PROPERTY(float, SeaWater, NormalMapScale0, GetNormalMapScale0, 
	SetNormalMapScale0);
DECLARE_PROPERTY(float, SeaWater, NormalMapScale1, GetNormalMapScale1, 
	SetNormalMapScale1);
DECLARE_PROPERTY(float, SeaWater, NormalMapScale2, GetNormalMapScale2, 
	SetNormalMapScale2);
DECLARE_PROPERTY(float, SeaWater, NormalMapScale3, GetNormalMapScale3, 
	SetNormalMapScale3);
/// property: float NormalMapSpeedX0 NormalMapSpeedX1 NormalMapSpeedX2 
///           flaot NormalMapSpeedX3
///           float NormalMapSpeedZ0 NormalMapSpeedZ1 NormalMapSpeedZ2 
///           float NormalMapSpeedZ3
/// \brief Normalmap Speed
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedX0, GetNormalMapSpeedX0, 
	SetNormalMapSpeedX0);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedX1, GetNormalMapSpeedX1, 
	SetNormalMapSpeedX1);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedX2, GetNormalMapSpeedX2, 
	SetNormalMapSpeedX2);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedX3, GetNormalMapSpeedX3, 
	SetNormalMapSpeedX3);

DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedZ0, GetNormalMapSpeedZ0, 
	SetNormalMapSpeedZ0);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedZ1, GetNormalMapSpeedZ1, 
	SetNormalMapSpeedZ1);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedZ2, GetNormalMapSpeedZ2, 
	SetNormalMapSpeedZ2);
DECLARE_PROPERTY(float, SeaWater, NormalMapSpeedZ3, GetNormalMapSpeedZ3, 
	SetNormalMapSpeedZ3);

/// property: float DistanceScale
/// \brief Distance Scale
DECLARE_PROPERTY(float, SeaWater, DistanceScale, GetDistanceScale, 
	SetDistanceScale);
/// property: float ReflectionInten
/// \brief Reflection Inten
DECLARE_PROPERTY(float, SeaWater, ReflectionInten, GetReflectionInten, 
	SetReflectionInten);
/// property: float RefractionInten
/// \brief Refraction Inten
DECLARE_PROPERTY(float, SeaWater, RefractionInten, GetRefractionInten, 
	SetRefractionInten);
/// property: float ReflectionWeight
/// \brief Reflection Weight
DECLARE_PROPERTY(float, SeaWater, ReflectionWeight, GetReflectionWeight, 
	SetReflectionWeight);
/// property: float RefractionWeight
/// \brief Refraction Weight
DECLARE_PROPERTY(float, SeaWater, RefractionWeight, GetRefractionWeight, 
	SetRefractionWeight);
/// property: float SunLevel
/// \brief Sun Level
DECLARE_PROPERTY(float, SeaWater, SunLevel, GetSunLevel, SetSunLevel);
/// property: float SetReflectionSunInten
/// \brief Sun Level
DECLARE_PROPERTY(float, SeaWater, ReflectionSunInten, GetReflectionSunInten, 
	SetReflectionSunInten);
/// property: string SunColorStart
/// \brief 太阳中心颜色
DECLARE_PROPERTY(result_string, SeaWater, SunColorStart, GetSunColorStartString, 
	SetSunColorStartString);
/// property: string SunColorEndString
/// \brief 太阳边缘颜色
DECLARE_PROPERTY(result_string, SeaWater, SunColorEnd, GetSunColorEndString, 
	SetSunColorEndString);

/// property: string NormalMapFileName
/// \brief 水面波纹法线贴图
DECLARE_PROPERTY(const char*, SeaWater, NormalMapFileName1, 
	GetNormalMapFileName1, SetNormalMapFileName1);
DECLARE_PROPERTY(const char*, SeaWater, NormalMapFileName2, 
	GetNormalMapFileName2, SetNormalMapFileName2);
DECLARE_PROPERTY(const char*, SeaWater, NormalMapFileName3, 
	GetNormalMapFileName3, SetNormalMapFileName3);
DECLARE_PROPERTY(const char*, SeaWater, NormalMapFileName4, 
	GetNormalMapFileName4, SetNormalMapFileName4);

/// property: string WhiteWaterMap
/// \brief 白色水域贴图
DECLARE_PROPERTY(const char*, SeaWater, WhiteWaterMap, GetWhiteWaterMap, 
	SetWhiteWaterMap);

/// property: string NoiseMap
/// \brief 噪声贴图
DECLARE_PROPERTY(const char*, SeaWater, NoiseMap, GetNoiseMap, SetNoiseMap);

/// property: float CausticSpeed
/// \brief 水下刻蚀变化速度
DECLARE_PROPERTY(float, SeaWater, CausticSpeed, GetCausticSpeed, 
	SetCausticSpeed);
/// property: float CausticScale
/// \brief 水下刻蚀缩放
DECLARE_PROPERTY(float, SeaWater, CausticScale, GetCausticScale, 
	SetCausticScale);
/// property: float CausticBright
/// \brief 水下刻蚀亮度
DECLARE_PROPERTY(float, SeaWater, CausticBright, GetCausticBright, 
	SetCausticBright);
/// property: float CausticFocus
/// \brief 水下刻蚀焦距
DECLARE_PROPERTY(float, SeaWater, CausticFocus, GetCausticFocus, 
	SetCausticFocus);
/// property: string CausticMap
/// \brief 水下刻蚀贴图
DECLARE_PROPERTY(const char*, SeaWater, CausticMap, GetCausticMap, 
	SetCausticMap);

/*
/// method: bool RefreshNormalMap(string pszFileName1, 
///		string pszFileName2, string pszFileName3, string pszFileName4)
/// \brief 刷新NormalMap
/// \param pszFile1 资源名称
/// \param pszFile2 资源名称
DECLARE_METHOD_4(bool, SeaWater, RefreshNormalMap, const char*, const char*, 
	const char*, const char*);
/// method: bool RefreshWhiteWaterMap(string pszFileName)
/// \brief 刷新白色水域贴图
/// \param pszFileName 贴图资源名称
DECLARE_METHOD_1(bool, SeaWater, RefreshWhiteWaterMap, const char*);
/// method: bool RefreshCausticMap(string pszFileName)
/// \brief 刷新水下刻蚀贴图
/// \param pszFileName 贴图资源名称
DECLARE_METHOD_1(bool, SeaWater, RefreshCausticMap, const char*);

/// method: bool UpdateSeaData()
/// \brief 立即更新海水参数
DECLARE_METHOD_0(bool, SeaWater, UpdateSeaData);
/// method: bool Start()
/// \brief 开始显示
DECLARE_METHOD_0(bool, SeaWater, Start);
*/

// SeaWater

SeaWater::SeaWater()
{
	m_pTerrain = NULL;
	m_pSeaBase = NULL;
	m_nHash = 0;
	// 数据结构中不能包含任何非POD成员
	memset(&m_SeaData, 0, sizeof(m_SeaData));
}

SeaWater::~SeaWater()
{
	if (m_pSeaBase)
	{
		CORE_DELETE(m_pSeaBase);
	}
}

bool SeaWater::Init(const IVarList& args)
{
	return true;
}

bool SeaWater::Shut()
{
	return true;
}

void SeaWater::SetIndex(size_t value)
{
	m_pSeaBase->SetIndex(value);
}

void SeaWater::SetSkyColorString(const char* value)
{
	m_SeaData.nSkyColor = VisUtil_ColorToInt(value);
}

result_string SeaWater::GetSkyColorString() const
{
	return VisUtil_IntToColor(m_SeaData.nSkyColor);
}

void SeaWater::SetWaterColorString(const char* value)
{
	m_SeaData.nWaterColor = VisUtil_ColorToInt(value);
}

result_string SeaWater::GetWaterColorString() const
{
	return VisUtil_IntToColor(m_SeaData.nWaterColor);
}

void SeaWater::SetCubeMapStatic(bool value)
{
	m_SeaData.bCubeMapStatic = value;

	m_pSeaBase->GetSeaData()->bCubeMapStatic = value;
	m_pSeaBase->GetSeaData()->bCubeMapReady = false;
}

bool SeaWater::GetCubeMapStatic() const
{
	return m_SeaData.bCubeMapStatic;
}

void SeaWater::SetReflect(bool value)
{
	m_SeaData.bReflect = value;

	m_pSeaBase->GetSeaData()->bReflect = value;
}

bool SeaWater::GetReflect() const
{
	return m_SeaData.bReflect;
}

void SeaWater::SetBaseHeight(float value)
{
	m_pSeaBase->SetBaseHeight(value);
}

float SeaWater::GetBaseHeight() const
{
	return m_pSeaBase->GetBaseHeight();
}

float SeaWater::GetCurrentHeight(float x, float y)
{
	return m_pSeaBase->GetCurrentHeight(x, y);
}

void SeaWater::SetMaxSeaHeight(float value)
{
	m_pSeaBase->SetMaxSeaHeight(value);
}

float SeaWater::GetMaxSeaHeight() const
{
	return m_pSeaBase->GetMaxSeaHeight();
}

void SeaWater::SetReflectionRatio(float value)
{
	m_SeaData.fReflectionRatio = value;
}

float SeaWater::GetReflectionRatio() const
{
	return m_SeaData.fReflectionRatio;
}

void SeaWater::SetWaveGradient0(float value)
{
	m_SeaData.fWaveGradient[0] = value;
}

float SeaWater::GetWaveGradient0() const
{
	return m_SeaData.fWaveGradient[0];
}

void SeaWater::SetWaveGradient1(float value)
{
	m_SeaData.fWaveGradient[1] = value;
}

float SeaWater::GetWaveGradient1() const
{
	return m_SeaData.fWaveGradient[1];
}

void SeaWater::SetWaveGradient2(float value)
{
	m_SeaData.fWaveGradient[2] = value;
}

float SeaWater::GetWaveGradient2() const
{
	return m_SeaData.fWaveGradient[2];
}

void SeaWater::SetWaveGradient3(float value)
{
	m_SeaData.fWaveGradient[3] = value;
}

float SeaWater::GetWaveGradient3() const
{
	return m_SeaData.fWaveGradient[3];
}

void SeaWater::SetWaveGradient4(float value)
{
	m_SeaData.fWaveGradient[4] = value;
}

float SeaWater::GetWaveGradient4() const
{
	return m_SeaData.fWaveGradient[4];
}

void SeaWater::SetWaveSwing0(float value)
{
	m_SeaData.fWaveSwing[0] = value;
}

float SeaWater::GetWaveSwing0() const
{
	return m_SeaData.fWaveSwing[0];
}

void SeaWater::SetWaveSwing1(float value)
{
	m_SeaData.fWaveSwing[1] = value;
}

float SeaWater::GetWaveSwing1() const
{
	return m_SeaData.fWaveSwing[1];
}

void SeaWater::SetWaveSwing2(float value)
{
	m_SeaData.fWaveSwing[2] = value;
}

float SeaWater::GetWaveSwing2() const
{
	return m_SeaData.fWaveSwing[2];
}

void SeaWater::SetWaveSwing3(float value)
{
	m_SeaData.fWaveSwing[3] = value;
}

float SeaWater::GetWaveSwing3() const
{
	return m_SeaData.fWaveSwing[3];
}

void SeaWater::SetWaveSwing4(float value)
{
	m_SeaData.fWaveSwing[4] = value;
}

float SeaWater::GetWaveSwing4() const
{
	return m_SeaData.fWaveSwing[4];
}

void SeaWater::SetWaveAngleFreq0(float value)
{
	m_SeaData.fWaveAngleFreq[0] = value;
}

float SeaWater::GetWaveAngleFreq0() const
{
	return m_SeaData.fWaveAngleFreq[0];
}

void SeaWater::SetWaveAngleFreq1(float value)
{
	m_SeaData.fWaveAngleFreq[1] = value;
}

float SeaWater::GetWaveAngleFreq1() const
{
	return m_SeaData.fWaveAngleFreq[1];
}

void SeaWater::SetWaveAngleFreq2(float value)
{
	m_SeaData.fWaveAngleFreq[2] = value;
}

float SeaWater::GetWaveAngleFreq2() const
{
	return m_SeaData.fWaveAngleFreq[2];
}

void SeaWater::SetWaveAngleFreq3(float value)
{
	m_SeaData.fWaveAngleFreq[3] = value;
}

float SeaWater::GetWaveAngleFreq3() const
{
	return m_SeaData.fWaveAngleFreq[3];
}

void SeaWater::SetWaveAngleFreq4(float value)
{
	m_SeaData.fWaveAngleFreq[4] = value;
}

float SeaWater::GetWaveAngleFreq4() const
{
	return m_SeaData.fWaveAngleFreq[4];
}

void SeaWater::SetWavePhaX0(float value)
{
	m_SeaData.fWavePhaX[0] = value;
}

float SeaWater::GetWavePhaX0() const
{
	return m_SeaData.fWavePhaX[0];
}

void SeaWater::SetWavePhaX1(float value)
{
	m_SeaData.fWavePhaX[1] = value;
}

float SeaWater::GetWavePhaX1() const
{
	return m_SeaData.fWavePhaX[1];
}

void SeaWater::SetWavePhaX2(float value)
{
	m_SeaData.fWavePhaX[2] = value;
}

float SeaWater::GetWavePhaX2() const
{
	return m_SeaData.fWavePhaX[2];
}

void SeaWater::SetWavePhaX3(float value)
{
	m_SeaData.fWavePhaX[3] = value;
}

float SeaWater::GetWavePhaX3() const
{
	return m_SeaData.fWavePhaX[3];
}

void SeaWater::SetWavePhaX4(float value)
{
	m_SeaData.fWavePhaX[4] = value;
}

float SeaWater::GetWavePhaX4() const
{
	return m_SeaData.fWavePhaX[4];
}

void SeaWater::SetWavePhaZ0(float value)
{
	m_SeaData.fWavePhaZ[0] = value;
}

float SeaWater::GetWavePhaZ0() const
{
	return m_SeaData.fWavePhaZ[0];
}

void SeaWater::SetWavePhaZ1(float value)
{
	m_SeaData.fWavePhaZ[1] = value;
}

float SeaWater::GetWavePhaZ1() const
{
	return m_SeaData.fWavePhaZ[1];
}

void SeaWater::SetWavePhaZ2(float value)
{
	m_SeaData.fWavePhaZ[2] = value;
}

float SeaWater::GetWavePhaZ2() const
{
	return m_SeaData.fWavePhaZ[2];
}

void SeaWater::SetWavePhaZ3(float value)
{
	m_SeaData.fWavePhaZ[3] = value;
}

float SeaWater::GetWavePhaZ3() const
{
	return m_SeaData.fWavePhaZ[3];
}

void SeaWater::SetWavePhaZ4(float value)
{
	m_SeaData.fWavePhaZ[4] = value;
}

float SeaWater::GetWavePhaZ4() const
{
	return m_SeaData.fWavePhaZ[4];
}

void SeaWater::SetWaveDirectionX0(float value)
{
	m_SeaData.fWaveDirectionX[0] = value;
}

float SeaWater::GetWaveDirectionX0() const
{
	return m_SeaData.fWaveDirectionX[0];
}

void SeaWater::SetWaveDirectionX1(float value)
{
	m_SeaData.fWaveDirectionX[1] = value;
}

float SeaWater::GetWaveDirectionX1() const
{
	return m_SeaData.fWaveDirectionX[1];
}

void SeaWater::SetWaveDirectionX2(float value)
{
	m_SeaData.fWaveDirectionX[2] = value;
}

float SeaWater::GetWaveDirectionX2() const
{
	return m_SeaData.fWaveDirectionX[2];
}

void SeaWater::SetWaveDirectionX3(float value)
{
	m_SeaData.fWaveDirectionX[3] = value;
}

float SeaWater::GetWaveDirectionX3() const
{
	return m_SeaData.fWaveDirectionX[3];
}

void SeaWater::SetWaveDirectionX4(float value)
{
	m_SeaData.fWaveDirectionX[4] = value;
}

float SeaWater::GetWaveDirectionX4() const
{
	return m_SeaData.fWaveDirectionX[4];
}

void SeaWater::SetWaveDirectionZ0(float value)
{
	m_SeaData.fWaveDirectionZ[0] = value;
}

float SeaWater::GetWaveDirectionZ0() const
{
	return m_SeaData.fWaveDirectionZ[0];
}

void SeaWater::SetWaveDirectionZ1(float value)
{
	m_SeaData.fWaveDirectionZ[1] = value;
}

float SeaWater::GetWaveDirectionZ1() const
{
	return m_SeaData.fWaveDirectionZ[1];
}

void SeaWater::SetWaveDirectionZ2(float value)
{
	m_SeaData.fWaveDirectionZ[2] = value;
}

float SeaWater::GetWaveDirectionZ2() const
{
	return m_SeaData.fWaveDirectionZ[2];
}

void SeaWater::SetWaveDirectionZ3(float value)
{
	m_SeaData.fWaveDirectionZ[3] = value;
}

float SeaWater::GetWaveDirectionZ3() const
{
	return m_SeaData.fWaveDirectionZ[3];
}

void SeaWater::SetWaveDirectionZ4(float value)
{
	m_SeaData.fWaveDirectionZ[4] = value;
}

float SeaWater::GetWaveDirectionZ4() const
{
	return m_SeaData.fWaveDirectionZ[4];
}

void SeaWater::SetNormalMapScale0(float value)
{
	m_SeaData.fNormalMapScale[0] = value;
}

float SeaWater::GetNormalMapScale0() const
{
	return m_SeaData.fNormalMapScale[0];
}

void SeaWater::SetNormalMapScale1(float value)
{
	m_SeaData.fNormalMapScale[1] = value;
}

float SeaWater::GetNormalMapScale1() const
{
	return m_SeaData.fNormalMapScale[1];
}

void SeaWater::SetNormalMapScale2(float value)
{
	m_SeaData.fNormalMapScale[2] = value;
}

float SeaWater::GetNormalMapScale2() const
{
	return m_SeaData.fNormalMapScale[2];
}

void SeaWater::SetNormalMapScale3(float value)
{
	m_SeaData.fNormalMapScale[3] = value;
}

float SeaWater::GetNormalMapScale3() const
{
	return m_SeaData.fNormalMapScale[3];
}

void SeaWater::SetNormalMapSpeedX0(float value)
{
	m_SeaData.fNormalMapSpeedX[0] = value;
}

float SeaWater::GetNormalMapSpeedX0() const
{
	return m_SeaData.fNormalMapSpeedX[0];
}

void SeaWater::SetNormalMapSpeedX1(float value)
{
	m_SeaData.fNormalMapSpeedX[1] = value;
}

float SeaWater::GetNormalMapSpeedX1() const
{
	return m_SeaData.fNormalMapSpeedX[1];
}

void SeaWater::SetNormalMapSpeedX2(float value)
{
	m_SeaData.fNormalMapSpeedX[2] = value;
}

float SeaWater::GetNormalMapSpeedX2() const
{
	return m_SeaData.fNormalMapSpeedX[2];
}

void SeaWater::SetNormalMapSpeedX3(float value)
{
	m_SeaData.fNormalMapSpeedX[3] = value;
}

float SeaWater::GetNormalMapSpeedX3() const
{
	return m_SeaData.fNormalMapSpeedX[3];
}


void SeaWater::SetNormalMapSpeedZ0(float value)
{
	m_SeaData.fNormalMapSpeedZ[0] = value;
}

float SeaWater::GetNormalMapSpeedZ0() const
{
	return m_SeaData.fNormalMapSpeedZ[0];
}

void SeaWater::SetNormalMapSpeedZ1(float value)
{
	m_SeaData.fNormalMapSpeedZ[1] = value;
}

float SeaWater::GetNormalMapSpeedZ1() const
{
	return m_SeaData.fNormalMapSpeedZ[1];
}

void SeaWater::SetNormalMapSpeedZ2(float value)
{
	m_SeaData.fNormalMapSpeedZ[2] = value;
}

float SeaWater::GetNormalMapSpeedZ2() const
{
	return m_SeaData.fNormalMapSpeedZ[2];
}

void SeaWater::SetNormalMapSpeedZ3(float value)
{
	m_SeaData.fNormalMapSpeedZ[3] = value;
}

float SeaWater::GetNormalMapSpeedZ3() const
{
	return m_SeaData.fNormalMapSpeedZ[3];
}

void SeaWater::SetDistanceScale(float value)
{
	m_SeaData.fDistanceScale = value;
}

float SeaWater::GetDistanceScale() const
{
	return m_SeaData.fDistanceScale;
}

void SeaWater::SetReflectionInten(float value)
{
	m_SeaData.fReflectionInten = value;
}

float SeaWater::GetReflectionInten() const
{
	return m_SeaData.fReflectionInten;
}

void SeaWater::SetRefractionInten(float value)
{
	m_SeaData.fRefractionInten = value;
}

float SeaWater::GetRefractionInten() const
{
	return m_SeaData.fRefractionInten;
}

void SeaWater::SetReflectionWeight(float value)
{
	m_SeaData.fReflectionWeight = value;
}

float SeaWater::GetReflectionWeight() const
{
	return m_SeaData.fReflectionWeight;
}

void SeaWater::SetRefractionWeight(float value)
{
	m_SeaData.fRefractionWeight = value;
}

float SeaWater::GetRefractionWeight() const
{
	return m_SeaData.fRefractionWeight;
}

void SeaWater::SetSunLevel(float value)
{
	m_SeaData.fSunLevel = value;
}

float SeaWater::GetSunLevel() const
{
	return m_SeaData.fSunLevel;
}

void SeaWater::SetReflectionSunInten(float value)
{
	m_SeaData.fReflectionSunInten = value;
}

float SeaWater::GetReflectionSunInten() const
{
	return m_SeaData.fReflectionSunInten;
}

void SeaWater::SetSunColorStartString(const char* value)
{
	m_SeaData.nSunColorStart = VisUtil_ColorToInt(value);
}

result_string SeaWater::GetSunColorStartString() const
{
	return VisUtil_IntToColor(m_SeaData.nSunColorStart);
}

void SeaWater::SetSunColorEndString(const char* value)
{
	m_SeaData.nSunColorEnd = VisUtil_ColorToInt(value);
}

result_string SeaWater::GetSunColorEndString() const
{
	return VisUtil_IntToColor(m_SeaData.nSunColorEnd);
}

void SeaWater::SetNormalMapFileName1(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strNormalMapFileName1, 
		sizeof(m_SeaData.strNormalMapFileName1), value);
}

const char* SeaWater::GetNormalMapFileName1() const
{
	return m_SeaData.strNormalMapFileName1;
}

void SeaWater::SetNormalMapFileName2(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strNormalMapFileName2, 
		sizeof(m_SeaData.strNormalMapFileName2), value);
}

const char* SeaWater::GetNormalMapFileName2() const
{
	return m_SeaData.strNormalMapFileName2;
}

void SeaWater::SetNormalMapFileName3(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strNormalMapFileName3,
		sizeof(m_SeaData.strNormalMapFileName3), value);
}

const char* SeaWater::GetNormalMapFileName3() const
{
	return m_SeaData.strNormalMapFileName3;
}

void SeaWater::SetNormalMapFileName4(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strNormalMapFileName4,
		sizeof(m_SeaData.strNormalMapFileName4), value);
}

const char* SeaWater::GetNormalMapFileName4() const
{
	return m_SeaData.strNormalMapFileName4;
}

void SeaWater::SetWhiteWaterMap(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strWhiteWaterMap, sizeof(m_SeaData.strWhiteWaterMap),
		value);
}

const char* SeaWater::GetWhiteWaterMap() const
{
	return m_SeaData.strWhiteWaterMap;
}

void SeaWater::SetNoiseMap(const char* value)
{
	Assert(value != NULL);

	CopyString(m_SeaData.strNoiseMap, sizeof(m_SeaData.strNoiseMap), value);
}

const char* SeaWater::GetNoiseMap() const
{
	return m_SeaData.strNoiseMap;
}

void SeaWater::SetCausticSpeed(float value)
{
	m_SeaData.fCausticSpeed = value;
}

float SeaWater::GetCausticSpeed() const
{
	return m_SeaData.fCausticSpeed;
}

void SeaWater::SetCausticScale(float value)
{
	m_SeaData.fCausticScale = value;
}

float SeaWater::GetCausticScale() const
{
	return m_SeaData.fCausticScale;
}

void SeaWater::SetCausticBright(float value)
{
	m_SeaData.fCausticBright = value;
}

float SeaWater::GetCausticBright() const
{
	return m_SeaData.fCausticBright;
}

void SeaWater::SetCausticFocus(float value)
{
	m_SeaData.fCausticFocus = value;
}

float SeaWater::GetCausticFocus() const
{
	return m_SeaData.fCausticFocus;
}

void SeaWater::SetCausticMap(const char* value)
{
	Assert(value != NULL);
	
	CopyString(m_SeaData.strCausticMap, sizeof(m_SeaData.strCausticMap), 
		value);
}

const char* SeaWater::GetCausticMap() const
{
	return m_SeaData.strCausticMap;
}

bool SeaWater::RefreshNormalMap(const char* pszFileName1, 
	const char* pszFileName2, const char* pszFileName3, 
	const char* pszFileName4)
{
	return m_pSeaBase->CreateNormalMap(pszFileName1, pszFileName2, 
		pszFileName3,pszFileName4);
}

bool SeaWater::RefreshWhiteWaterMap(const char* pszFileName)
{
	return m_pSeaBase->CreateWhiteWaterMap(pszFileName);
}

bool SeaWater::RefreshCausticMap(const char* pszFileName)
{
	return m_pSeaBase->CreateCausticMap(pszFileName);
}

bool SeaWater::InitSeaData()
{
	m_SeaData.fReflectionRatio = 0.5F;

	m_SeaData.nSkyColor = COLOR_ARGB(255, 102, 178, 255);
	m_SeaData.nWaterColor = COLOR_ARGB(255, 0, 0, 0);

	m_SeaData.bCubeMapStatic = false;
	m_SeaData.bCubeMapReady = false;
	m_SeaData.bReflect = true;

	m_SeaData.fWaveGradient[0] = 0.001F;
	m_SeaData.fWaveGradient[1] = 0.001F;
	m_SeaData.fWaveGradient[2] = 0.001F;
	m_SeaData.fWaveGradient[3] = 0.001F;
	m_SeaData.fWaveGradient[4] = 0.001F;

	m_SeaData.fWaveSwing[0] = 0.01F;
	m_SeaData.fWaveSwing[1] = 0.01F;
	m_SeaData.fWaveSwing[2] = 0.01F;
	m_SeaData.fWaveSwing[3] = 0.01F;
	m_SeaData.fWaveSwing[4] = 0.01F;

	m_SeaData.fWaveAngleFreq[0] = 0.01F;
	m_SeaData.fWaveAngleFreq[1] = 0.01F;
	m_SeaData.fWaveAngleFreq[2] = 0.01F;
	m_SeaData.fWaveAngleFreq[3] = 0.01F;
	m_SeaData.fWaveAngleFreq[4] = 0.01F;

	m_SeaData.fWavePhaX[0] = 0.01F;
	m_SeaData.fWavePhaX[1] = 0.01F;
	m_SeaData.fWavePhaX[2] = 0.01F;
	m_SeaData.fWavePhaX[3] = 0.01F;
	m_SeaData.fWavePhaX[4] = 0.01F;

	m_SeaData.fWavePhaZ[0] = 0.01F;
	m_SeaData.fWavePhaZ[1] = 10.01F;
	m_SeaData.fWavePhaZ[2] = 0.01F;
	m_SeaData.fWavePhaZ[3] = 0.01F;
	m_SeaData.fWavePhaZ[4] = 0.01F;

	m_SeaData.fWaveDirectionX[0] = 0.01F;
	m_SeaData.fWaveDirectionX[1] = 0.01F;
	m_SeaData.fWaveDirectionX[2] = 0.01F;
	m_SeaData.fWaveDirectionX[3] = 0.01F;
	m_SeaData.fWaveDirectionX[4] = 0.01F;

	m_SeaData.fWaveDirectionZ[0] = 0.01F;
	m_SeaData.fWaveDirectionZ[1] = 0.01F;
	m_SeaData.fWaveDirectionZ[2] = 0.01F;
	m_SeaData.fWaveDirectionZ[3] = 0.01F;
	m_SeaData.fWaveDirectionZ[4] = 0.01F;

	m_SeaData.fNormalMapScale[0] = 0.01F;
	m_SeaData.fNormalMapScale[1] = 0.01F;
	m_SeaData.fNormalMapScale[2] = 0.01F;
	m_SeaData.fNormalMapScale[3] = 0.01F;

	m_SeaData.fNormalMapSpeedX[0] = 0.01F;
	m_SeaData.fNormalMapSpeedX[1] = 0.01F;
	m_SeaData.fNormalMapSpeedX[2] = 0.01F;
	m_SeaData.fNormalMapSpeedX[3] = 0.01F;

	m_SeaData.fNormalMapSpeedZ[0] = 0.01F;
	m_SeaData.fNormalMapSpeedZ[1] = 0.01F;
	m_SeaData.fNormalMapSpeedZ[2] = 0.01F;
	m_SeaData.fNormalMapSpeedZ[3] = 0.01F;

	m_SeaData.fDistanceScale = 40.0F;

	m_SeaData.fReflectionInten = 0.01F;
	m_SeaData.fRefractionInten = 0.01F;
	m_SeaData.fReflectionWeight = 0.5F;
	m_SeaData.fRefractionWeight = 0.5F;

	m_SeaData.fSunLevel = 60.0F;
	m_SeaData.fReflectionSunInten = 0.01F;
	m_SeaData.nSunColorStart = COLOR_ARGB(255, 0, 0, 0);
	m_SeaData.nSunColorEnd = COLOR_ARGB(255, 0, 0, 0);

	CopyString(m_SeaData.strNormalMapFileName1, 
		sizeof(m_SeaData.strNormalMapFileName1), 
		"tex\\water\\waternormalmap2.dds");
	CopyString(m_SeaData.strNormalMapFileName2, 
		sizeof(m_SeaData.strNormalMapFileName2), 
		"tex\\water\\waternormalmap2.dds");
	CopyString(m_SeaData.strNormalMapFileName3, 
		sizeof(m_SeaData.strNormalMapFileName3), 
		"tex\\water\\waternormalmap2.dds");
	CopyString(m_SeaData.strNormalMapFileName4, 
		sizeof(m_SeaData.strNormalMapFileName4), 
		"tex\\water\\waternormalmap2.dds");

	// 默认不开白色水域效果
	CopyString(m_SeaData.strWhiteWaterMap, sizeof(m_SeaData.strWhiteWaterMap),
		"");
	CopyString(m_SeaData.strNoiseMap, sizeof(m_SeaData.strNoiseMap), 
		"tex\\water\\noise.dds");
	m_SeaData.fCausticSpeed = 1.0F;
	m_SeaData.fCausticScale = 4.0F;
	m_SeaData.fCausticBright = 1.0F;
	m_SeaData.fCausticFocus = 5.0F;
	CopyString(m_SeaData.strCausticMap, sizeof(m_SeaData.strCausticMap),
		"tex\\water\\caust.dds");
	
	return true;
}

bool SeaWater::UpdateSeaData()
{
	*(m_pSeaBase->GetSeaData()) = m_SeaData;
	
	return true;
}

bool SeaWater::Start()
{
  //m_pSeaBase->SetVisible(true);
  return true;
}
