//#define FOGLINEAR 1
//#define FOGEXP 1
//#define SHADOWEDMAP 1
//#define PRELIGHT 1
//#define LIGHTMAP 1
//#define EARLYZ 1

attribute mediump vec4 iPos;
attribute mediump vec2 iTex0;
attribute mediump vec2 iHeightAngle;


//cbPerfGroup
uniform mediump vec3  c_vWinDir;
uniform mediump float c_fWinForce;
uniform mediump float c_fTimer;
uniform mediump vec2  c_FadeParam;
uniform mediump vec3  c_vPlayerPoint;
uniform mediump vec4  c_MaterialDiffuse;
uniform mediump vec4  c_MaterialAmbient;

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
uniform mediump mat4 c_mtxViewProj;
uniform mediump mat4 c_mtxView;
uniform mediump mat4 c_mtxProj;
uniform mediump mat4 c_mtxViewProjInverse;
uniform mediump mat4 c_mtxViewInverse;
uniform mediump mat4 c_mtxProjInverse;
uniform mediump vec3 c_vViewPos;
uniform mediump vec3 c_vLightDir;
uniform mediump vec3 c_LightAmbient;
uniform mediump float c_fReflectFactor;
uniform mediump vec3 c_LightDiffuse;
uniform mediump vec4 c_FogColor;
uniform mediump vec4 c_FogParam;
uniform mediump vec4 c_FogExpParam;
uniform mediump vec2 c_PixelSize;
uniform mediump float c_fStereoConvergence;
uniform mediump float c_fCameraNearZ;
uniform mediump vec2  c_HalfPixelSize;
uniform mediump vec2  c_BlendHalfPixel;
uniform mediump vec3  c_vReferPos;
uniform mediump vec2  c_DepthParam;
uniform mediump float c_fFarClipDistance;
uniform mediump float c_fAlphaRef;
uniform mediump vec3  c_vPointLightPos;
uniform mediump float c_fPointLightRange;
uniform mediump vec4  c_PointLightDiffuse;
uniform mediump vec4  c_CameraLightDiffuse;
uniform mediump vec4  c_ClipPlane;


varying mediump vec4 oDiffuse;
varying mediump vec2 oTex0;

#if defined(FOGLINEAR)||defined(FOGEXP)
varying mediump float oFogDepth;
#endif

#if defined(LIGHTMAP)
varying mediump vec2 oLightTexcoord;
#else
varying mediump float oHeight;
varying mediump vec3 oNormal;
#endif
varying mediump vec4 oPos;

mediump vec3 Wind(in mediump vec3 Pos, in mediump vec3 WindDirection,in mediump float WindForce,in mediump float Time,in mediump  float GrassHeight)
{
	mediump float Distancez = dot(Pos, WindDirection);
	mediump float Distancex = distance(Pos, Distancez * WindDirection);
	mediump float Swing = (1.0 - abs(sin(Distancex + Time * 0.4))) * GrassHeight;
	mediump float Wave = sin(Distancez * 10.0 + Time * WindForce * 6.0) + 
		sin(Distancez * 30.0 + Time * WindForce * 3.0);
	
	Distancez += sin(Time * 0.5 + Distancex) + sin(Distancex * 2.0) + 
		sin(Distancez) * 0.5;
	
	mediump float Wave2 = sin(Distancez + Time * WindForce * 5.0);
	
	return Swing * (Wave * WindForce / 2.0 + WindForce) * WindDirection + 
		Wave2 * WindDirection * pow(max(WindForce, 0.000001), 5.0);
}


void main(void)
{
   mediump vec3 vGrassBottom = iPos.xyz ; 
   
   vGrassBottom.y -= iHeightAngle.x;
   
    mediump vec3 vPosInWind = iPos.xyz + Wind(iPos.xyz, c_vWinDir, c_fWinForce, c_fTimer, iHeightAngle.x);
   
    mediump vec3 posdir = vPosInWind - vec3(c_vPlayerPoint.x - c_vChunkPos.x, c_vPlayerPoint.y, c_vPlayerPoint.z - c_vChunkPos.z);
   
	posdir.y -= 1.0;

	vPosInWind.xz += posdir.xz / dot(posdir, posdir) * 0.5;

	vPosInWind = normalize(vPosInWind - vGrassBottom) * iHeightAngle.x + vGrassBottom;

	mediump vec4 pos_in_world = vec4(vPosInWind, 1.0);
	
	pos_in_world.xz = pos_in_world.xz + c_vChunkPos.xz - c_vReferPos.xz;
	oPos = pos_in_world * c_mtxViewProj;   
	gl_Position = oPos;
	
	mediump vec4 Diffuse = vec4(1.0,1.0,1.0,1.0);
	oTex0 = iTex0;
	
	float fDepth = length(pos_in_world.xyz - c_vViewPos);

	Diffuse.w *= clamp((c_FadeParam.x - fDepth) * c_FadeParam.y,0.0,1.0); 	
	
	// Open Fog
#if defined(FOGLINEAR) || defined(FOGEXP)
	oFogDepth = fDepth;
#endif

#if defined(LIGHTMAP)
	Diffuse.x = clamp((iHeightAngle.x * 100.0) * (1.25 + sin(iHeightAngle.y) * 0.25),0.0,1.0);
	oLightTexcoord = iPos.xz / 256.0 + c_ZoneOffsetAndSize.xy / 8.0;
#else
	//oHeight = clamp(iHeightAngle.x * 100.0,0.0,1.0);
	oHeight = clamp(iHeightAngle.x * 100.0,0.0,1.0) * 0.5 + 0.5;
	oNormal.y = 0.0;
	oNormal.x = sin(iHeightAngle.y);
	oNormal.z = cos(iHeightAngle.y);
	oNormal *= iHeightAngle.x;
	oNormal += (vPosInWind - vGrassBottom) * 6.0;
	oNormal.y += 1.0;
#endif
    oNormal = normalize(oNormal);
   	oDiffuse = Diffuse;
}