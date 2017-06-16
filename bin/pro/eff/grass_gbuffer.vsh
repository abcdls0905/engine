vsin MEDIUMP vec4 iPos;
vsin MEDIUMP vec2 iTex0;
vsin MEDIUMP vec2 iHeightAngle;

//cbPerfGroup
uniform MEDIUMP vec3  c_vWinDir;
uniform MEDIUMP float c_fWinForce;
uniform HIGHP float c_fTimer;
uniform MEDIUMP vec2  c_FadeParam;
uniform MEDIUMP vec3  c_vPlayerPoint;
uniform MEDIUMP vec4  c_MaterialDiffuse;
uniform MEDIUMP vec4  c_MaterialAmbient;

// cbPerObject
uniform MEDIUMP mat4 c_Detail1Param;
uniform MEDIUMP mat4 c_Detail2Param;
uniform MEDIUMP mat4 c_Detail3Param;
uniform MEDIUMP mat4 c_Detail4Param;
uniform MEDIUMP vec3 c_vChunkPos;
uniform MEDIUMP vec4 c_ZoneOffsetAndSize;

uniform MEDIUMP float c_fBaseSpecularLevel;
uniform MEDIUMP float c_fBaseSpecularPower;
uniform MEDIUMP float c_fBaseHeightScale;
uniform MEDIUMP vec4  c_BlendSpecularLevel;
uniform MEDIUMP vec4  c_BlendSpecularPower;
uniform MEDIUMP vec4  c_BlendHeightScale;


// cbPerGroup
uniform MEDIUMP mat4 c_mtxViewProj;
uniform MEDIUMP mat4 c_mtxView;
uniform MEDIUMP mat4 c_mtxProj;
uniform MEDIUMP mat4 c_mtxViewProjInverse;
uniform MEDIUMP mat4 c_mtxViewInverse;
uniform MEDIUMP mat4 c_mtxProjInverse;
uniform MEDIUMP vec3 c_vViewPos;
uniform MEDIUMP vec3 c_vLightDir;
uniform MEDIUMP vec3 c_LightAmbient;
uniform MEDIUMP float c_fReflectFactor;
uniform MEDIUMP vec3 c_LightDiffuse;
uniform MEDIUMP vec4 c_FogColor;
uniform MEDIUMP vec4 c_FogParam;
uniform MEDIUMP vec4 c_FogExpParam;
uniform MEDIUMP vec2 c_PixelSize;
uniform MEDIUMP float c_fStereoConvergence;
uniform MEDIUMP float c_fCameraNearZ;
uniform MEDIUMP vec2  c_HalfPixelSize;
uniform MEDIUMP vec2  c_BlendHalfPixel;
uniform MEDIUMP vec3  c_vReferPos;
uniform MEDIUMP vec2  c_DepthParam;
uniform MEDIUMP float c_fFarClipDistance;
uniform MEDIUMP float c_fAlphaRef;
uniform MEDIUMP vec3  c_vPointLightPos;
uniform MEDIUMP float c_fPointLightRange;
uniform MEDIUMP vec4  c_PointLightDiffuse;
uniform MEDIUMP vec4  c_CameraLightDiffuse;
uniform MEDIUMP vec4  c_ClipPlane;

uniform MEDIUMP vec4  c_ZoneOffset;

vsout MEDIUMP vec4 oDiffuse;
vsout MEDIUMP vec2 oTex0;

#if defined(FOGLINEAR)||defined(FOGEXP)
	vsout MEDIUMP float oFogDepth;
#endif

#if defined(LIGHTMAP)
	vsout MEDIUMP vec2 oLightTexcoord;
#endif
vsout MEDIUMP float oHeight;
vsout MEDIUMP vec3 oNormal;
vsout MEDIUMP vec4 oPos;

HIGHP vec3 Wind(in MEDIUMP vec3 Pos, in MEDIUMP vec3 WindDirection,in MEDIUMP float WindForce,in HIGHP float Time,in MEDIUMP  float GrassHeight)
{
	MEDIUMP float Distancez = dot(Pos, WindDirection);
	MEDIUMP float Distancex = distance(Pos, Distancez * WindDirection);
	MEDIUMP float Swing = (1.0 - abs(sin(Distancex + Time * 0.4))) * GrassHeight;
	MEDIUMP float Wave = sin(Distancez * 10.0 + Time * WindForce * 6.0) + 
		sin(Distancez * 30.0 + Time * WindForce * 3.0);
	
	Distancez += sin(Time * 0.5 + Distancex) + sin(Distancex * 2.0) + 
		sin(Distancez) * 0.5;
	
	MEDIUMP float Wave2 = sin(Distancez + Time * WindForce * 5.0);
	
	return Swing * (Wave * WindForce / 2.0 + WindForce) * WindDirection + 
		Wave2 * WindDirection * pow(max(WindForce, 0.000001), 5.0);
}


void main(void)
{
   MEDIUMP vec3 vGrassBottom = iPos.xyz ; 
   
   vGrassBottom.y -= iHeightAngle.x;
   
    HIGHP vec3 vPosInWind = iPos.xyz + Wind(iPos.xyz, c_vWinDir, c_fWinForce, c_fTimer, iHeightAngle.x);
   
    HIGHP vec3 posdir = vPosInWind - vec3(c_vPlayerPoint.x - c_vChunkPos.x, c_vPlayerPoint.y, c_vPlayerPoint.z - c_vChunkPos.z);
   
	posdir.y -= 1.0;

	vPosInWind.xz += posdir.xz / dot(posdir, posdir) * 0.5;

	vPosInWind = normalize(vPosInWind - vGrassBottom) * iHeightAngle.x + vGrassBottom;

	HIGHP vec4 pos_in_world = vec4(vPosInWind, 1.0);
	
	pos_in_world.xz = pos_in_world.xz + c_vChunkPos.xz - c_vReferPos.xz;
	oPos = pos_in_world * c_mtxViewProj;   
	gl_Position = oPos;
	
	MEDIUMP vec4 Diffuse = vec4(1.0,1.0,1.0,1.0);
	oTex0 = iTex0;
	
	float fDepth = length(pos_in_world.xyz - c_vViewPos);

	Diffuse.w *= clamp((c_FadeParam.x - fDepth) * c_FadeParam.y,0.0,1.0); 	
	
	// Open Fog
#if defined(FOGLINEAR) || defined(FOGEXP)
	oFogDepth = fDepth;
#endif

#if defined(LIGHTMAP)
	Diffuse.x = clamp((iHeightAngle.x * 100.0) * (1.25 + sin(iHeightAngle.y) * 0.25),0.0,1.0);
	oLightTexcoord.xy = iPos.xz / 256.0 + c_ZoneOffset.xy / 2.0;
#endif
	//oHeight = clamp(iHeightAngle.x * 100.0,0.0,1.0);
	oHeight = clamp(iHeightAngle.x * 100.0,0.0,1.0) * 0.5 + 0.5;
	oNormal.y = 0.0;
	oNormal.x = sin(iHeightAngle.y);
	oNormal.z = cos(iHeightAngle.y);
	oNormal *= iHeightAngle.x;
	oNormal += (vPosInWind - vGrassBottom) * 6.0;
	oNormal.y += 1.0;
    oNormal = normalize(oNormal);	

   	oDiffuse = Diffuse;
}