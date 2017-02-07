//--------------------------------------------------------------------
// 文件名:		sea_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SEA_DATA_H
#define _SEA_DATA_H

#include "../public/macros.h"


#include "../visual/i_render.h"

// 海水数据
struct sea_data_t
{
	float fReflectionRatio;	// 倒影距离比例（0.0-1.0）

	unsigned int nSkyColor;
	unsigned int nWaterColor;
	
	bool bCubeMapStatic;
	bool bCubeMapReady;
	bool bReflect;

	float fWaveGradient[5];
	float fWaveSwing[5];
	float fWaveAngleFreq[5];
	float fWavePhaX[5];
	float fWavePhaZ[5];
	float fWaveDirectionX[5];
	float fWaveDirectionZ[5];

	float fNormalMapScale[4];
	float fNormalMapSpeedX[4];
	float fNormalMapSpeedZ[4];
	
	float fDistanceScale;
	
	float fReflectionInten;
	float fRefractionInten;
	float fReflectionWeight;
	float fRefractionWeight;

	float fSunLevel;
	float fReflectionSunInten;
	unsigned int nSunColorStart;
	unsigned int nSunColorEnd;

	char strNormalMapFileName1[256];
	char strNormalMapFileName2[256];
	char strNormalMapFileName3[256];
	char strNormalMapFileName4[256];
	char strWhiteWaterMap[256];
	char strNoiseMap[256];

	float fCausticSpeed;
	float fCausticScale;
	float fCausticBright;
	float fCausticFocus;
	char strCausticMap[256];
};

#endif // _SEA_DATA_H
