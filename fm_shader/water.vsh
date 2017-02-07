//#define WAVELESS 1
//#define USE_REFLECTION 1
//#define USE_REFRACTION 1
//#define USE_DEPTHMAP 1
//#define SUN_REFLECTION 1
//#define BORDER 1
//#define UNDERWATER 1
//#define WHITEWATER 1
//#define FOGLINEAR 1
//#define FOGEXP 1
//#define USE_INTZ 1
//#define EASY 1


attribute mediump vec4 iPos;
attribute mediump vec4 iDiffuseNormal;

// cbPerObject
uniform mediump mat4 c_Detail1Param;
uniform mediump mat4 c_Detail2Param;
uniform mediump mat4 c_Detail3Param;
uniform mediump mat4 c_Detail4Param;
uniform mediump vec3 c_vChunkPos;
uniform mediump vec4 c_ZoneOffsetAndSize;

uniform mediump float c_fBaseSpecularLevel;
uniform mediump float c_fBaseSpecularPower;
uniform mediump float c_fBaseHeightScale;
uniform mediump vec4  c_BlendSpecularLevel;
uniform mediump vec4  c_BlendSpecularPower;
uniform mediump vec4  c_BlendHeightScale;


// cbPerGroup
uniform mediump mat4  c_mtxViewProj;
uniform mediump float c_fTime;
uniform mediump vec4  c_ViewPosition;
uniform mediump vec4  c_WaveParam0;
uniform mediump vec4  c_WaveParam1;
uniform mediump vec4  c_WaveParam2;
uniform mediump vec4  c_WaveParam3;
uniform mediump vec4  c_WaveParam4;
uniform mediump vec4  c_WaveParam5;
uniform mediump vec4  c_WaveParam6;
uniform mediump vec4  c_WaveParam7;
uniform mediump float c_NormalmapScale0;
uniform mediump vec2  c_NormalmapSpeed0;
uniform mediump float c_NormalmapScale1;
uniform mediump vec2  c_NormalmapSpeed1;
uniform mediump float c_NormalmapScale2;
uniform mediump vec2  c_NormalmapSpeed2;
uniform mediump float c_NormalmapScale3;
uniform mediump vec2  c_NormalmapSpeed3;
uniform mediump vec3  c_SunDirection;
uniform mediump vec4  c_UnderWater;
uniform mediump vec4  c_vReferPos; 
uniform mediump vec3  c_vLightDir; 
uniform mediump vec4  c_OceanColor; 
uniform mediump float c_DistanceScale;
uniform mediump float c_ReflectionInten;
uniform mediump float c_RefractionInten;
uniform mediump float c_ReflectionWeight;
uniform mediump float c_RefractionWeight;
uniform mediump vec4  c_SunColorStart;
uniform mediump vec4  c_SunColorEnd; 
uniform mediump float c_SunLevel;
uniform mediump vec4  c_SkyColor;
uniform mediump vec4  c_DepthParam; 
uniform mediump float c_ReflectionSunInten;
uniform mediump vec4  c_FogColor;
uniform mediump vec4  c_FogParam;
uniform mediump vec4  c_FogExpParam;
uniform mediump vec2  c_PixelSize;

varying mediump vec4 oNormalMapCoord0;
varying mediump vec4 oNormalMapCoord1;
varying mediump vec4 oViewToVertex;
varying mediump vec3 oSunToVertex;
varying mediump vec4 oFovCoord;
varying mediump vec4 oWorldPos;
varying mediump vec3 oNormal;

varying mediump vec4 oPos;

struct Wave 
{
	mediump float fFreq; // 频率 (2PI / 波长)
	mediump float fAmp; // 振幅
	mediump float fPhase; // 相伴 (速度 * 2PI / 波长)
	mediump float fK; // 
	mediump vec2 vDir; // 方向 
};

 
void main(void)
{
   mediump vec4 posWorld = vec4(iPos.xyz,1.0) ; 
   posWorld.xz = posWorld.xz + (c_vChunkPos.xz - c_vReferPos.xz);
  
   
   mediump vec4 pos = vec4(0.0,0.0,0.0,1.0);
   mediump float ddx = 0.0;
   mediump float ddy = 0.0;  
   posWorld = posWorld + c_vReferPos;  
#if !defined(EASY)   
   mediump float wavedistance = distance(posWorld.xyz, c_ViewPosition.xyz);
 
   Wave Waves[5];
   
   Waves[0].fFreq = c_WaveParam0.x;Waves[0].fAmp = c_WaveParam0.y;Waves[0].fPhase = c_WaveParam0.z;Waves[0].fK = c_WaveParam0.w;Waves[0].vDir = vec2(c_WaveParam5.xy);
   Waves[1].fFreq = c_WaveParam1.x;Waves[1].fAmp = c_WaveParam1.y;Waves[1].fPhase = c_WaveParam1.z;Waves[1].fK = c_WaveParam1.w;Waves[1].vDir = vec2(c_WaveParam5.zw);
   Waves[2].fFreq = c_WaveParam2.x;Waves[2].fAmp = c_WaveParam2.y;Waves[2].fPhase = c_WaveParam2.z;Waves[2].fK = c_WaveParam2.w;Waves[2].vDir = vec2(c_WaveParam6.xy);
   Waves[3].fFreq = c_WaveParam3.x;Waves[3].fAmp = c_WaveParam3.y;Waves[3].fPhase = c_WaveParam3.z;Waves[3].fK = c_WaveParam3.w;Waves[3].vDir = vec2(c_WaveParam6.zw);
   Waves[4].fFreq = c_WaveParam4.x;Waves[4].fAmp = c_WaveParam4.y;Waves[4].fPhase = c_WaveParam4.z;Waves[4].fK = c_WaveParam4.w;Waves[4].vDir = vec2(c_WaveParam7.xy);
  
   for(int i = 0 ; i < 5; i++ )
   {
      #if !defined(WAVELESS)
        // pos.y += EvaluateWaveSharp(Waves[i], posWorld.xz, c_fTime);
         pos.y = pos.y + Waves[i].fAmp * pow(sin(dot(Waves[i].vDir, posWorld.xz) * Waves[i].fFreq + c_fTime * Waves[i].fPhase) * 0.5 + 0.500001, Waves[i].fK);
      #endif
 
 	  mediump float diff = Waves[i].fK * Waves[i].fFreq * Waves[i].fAmp 
		        * pow(sin(dot(Waves[i].vDir, posWorld.xz) * Waves[i].fFreq + c_fTime * Waves[i].fPhase) * 0.5 + 0.500001, Waves[i].fK - 1.0) 
		        * cos(dot(Waves[i].vDir, posWorld.xz) * Waves[i].fFreq + c_fTime * Waves[i].fPhase)
            	* clamp(20.0 / Waves[i].fFreq / wavedistance,0.0,1.0);      
      
 		ddx = ddx + diff * Waves[i].vDir.x;
 		ddy = ddy + diff * Waves[i].vDir.y;      
   }
   
#endif
   
   oNormal = vec3(-ddx, 1.0, -ddy) * c_UnderWater.x;
   pos.y = pos.y + c_ViewPosition.w;
   pos.xz = pos.xz + iPos.xz + (c_vChunkPos.xz - c_vReferPos.xz);   
   
   oPos = pos*c_mtxViewProj;
   // 水扰动速度
   oNormalMapCoord0.xy = posWorld.xz * c_NormalmapScale0 + c_fTime * c_NormalmapSpeed0;
   oNormalMapCoord0.zw = posWorld.xz * c_NormalmapScale1 + c_fTime * c_NormalmapSpeed1;
   oNormalMapCoord1.xy = posWorld.xz * c_NormalmapScale2 + c_fTime * c_NormalmapSpeed2;
   oNormalMapCoord1.zw = posWorld.xz * c_NormalmapScale3 + c_fTime * c_NormalmapSpeed3;   
   
   mediump vec4 pos_in_world = pos;
   mediump vec3 vecViewToVertex = pos_in_world.xyz - c_ViewPosition.xyz;
   
   oViewToVertex.w = length(vecViewToVertex);
   oViewToVertex.xyz = vecViewToVertex.xyz;
   oSunToVertex = c_SunDirection;
   oWorldPos = vec4(pos_in_world.xyz, iPos.y);
   
   oFovCoord = oPos;
   oPos.z = oPos.z * c_UnderWater.y;
   gl_Position = oPos;
   
   oPos.x = (oPos.w + oPos.x) * 0.5;
   oPos.y = (oPos.w - oPos.y) * 0.5;
 
}