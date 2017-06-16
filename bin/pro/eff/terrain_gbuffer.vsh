vsin MEDIUMP vec4 iPos;
#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuseNormal;
#else
vsin MEDIUMP vec4 iDiffuseNormal;
#endif

// cbPerObject
uniform MEDIUMP mat4 c_Detail1Param;
uniform MEDIUMP mat4 c_Detail2Param;
uniform MEDIUMP mat4 c_Detail3Param;
uniform MEDIUMP mat4 c_Detail4Param;
uniform MEDIUMP mat4 c_mtxWV;		// gbuffer add
uniform MEDIUMP vec3 c_vChunkPos;
uniform MEDIUMP vec4 c_ZoneOffsetAndSize;

uniform MEDIUMP float c_fBaseSpecularLevel;
uniform MEDIUMP float c_fBaseSpecularPower;
uniform MEDIUMP float c_fBaseHeightScale;
uniform MEDIUMP vec4  c_BlendSpecularLevel;
uniform MEDIUMP vec4  c_BlendSpecularPower;
uniform MEDIUMP vec4  c_BlendHeightScale;

// cbPerScene
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
uniform MEDIUMP mat4  c_mtxShadowViewProj;
uniform MEDIUMP vec4  c_WarFogOffsetAndSize;

#ifdef SHADOWMAP
	uniform HIGHP mat4 c_mtxLightViewProj1;
	#ifdef PSSM2
		uniform HIGHP mat4 c_mtxLightViewProj2;
	#endif
	#ifdef PSSM3
		uniform HIGHP mat4 c_mtxLightViewProj3;
	#endif
	#ifdef PSSM4
		uniform HIGHP mat4 c_mtxLightViewProj4;
	#endif
#endif

// cbPerGroup
uniform MEDIUMP float c_fTerrainBlockSize;
uniform MEDIUMP vec2 c_GraduallyDist;
uniform MEDIUMP vec4 c_TerrainBlockSize;
uniform MEDIUMP vec4 c_MaterialDiffuse;
uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP vec4 c_MaterialSpecular;
uniform MEDIUMP vec4 c_TessellationFactor;
uniform MEDIUMP mat4 c_Detail0Param;

#if defined(SHADOWMAP)
	//vsout MEDIUMP vec4 oTexCoordShadow;
	vsout HIGHP vec4 oViewPos;

	#ifdef PSSM4
		vsout HIGHP vec4 oShadowPos[4];
	#elif defined(PSSM3)
		vsout HIGHP vec4 oShadowPos[3];
	#elif defined(PSSM2)
		vsout HIGHP vec4 oShadowPos[2];
	#else
		vsout HIGHP vec4 oShadowPos;
	#endif
#endif

#if defined(BLEND0)
	vsout MEDIUMP vec4 oTexCoord01;
#endif

#if defined(BLEND1)||defined(BLEND2)
	vsout MEDIUMP vec4 oTexCoord23;
#endif

#if defined(BLEND3)||defined(BLEND4)
	vsout MEDIUMP vec4 oTexCoord45;
#endif

#if defined(LIGHTMAP) || defined(WARFOG)
	#if defined(LIGHTMAP) && defined(WARFOG)
		vsout MEDIUMP vec4 oTexCoordLB;
	#else
		vsout MEDIUMP vec2 oTexCoordLB;
	#endif
#endif

#if defined(GRADUALLY_DISAPPEAR)
	vsout MEDIUMP float oGradually;
#endif

#if defined(SMOOTHCLIP)
	vsout MEDIUMP vec2 oDepth;
#endif

vsout MEDIUMP vec3 oNor;

#if defined(NORMALMAP0) || defined(NORMALMAP1) || defined(NORMALMAP2) || defined(NORMALMAP3) || defined(NORMALMAP4)
	vsout MEDIUMP vec3 oTangent;		// gbuffer add
	vsout MEDIUMP vec3 oBinormal;		// gbuffer add
#endif

vsout HIGHP vec4 oPos;

#if defined(DEBUG)
	vsout MEDIUMP vec4 oTest;
#endif

#if defined(CLIPPLANE)
	vsout MEDIUMP float oClipDistance;
#endif

void main(void)
{   
#ifdef IOS_METAL
	MEDIUMP vec3 input_normal;
	input_normal.x = float((iDiffuseNormal & uint(0x00FF0000)) >> 16) / 255.0;
	input_normal.y = float((iDiffuseNormal & uint(0x0000FF00)) >> 8) / 255.0;
	input_normal.z = float(iDiffuseNormal & uint(0x000000FF)) / 255.0;
#else
	MEDIUMP vec3 input_normal = iDiffuseNormal.zyx;
#endif

	input_normal.xz = input_normal.xz*2.0 - 1.0;
	input_normal = normalize(input_normal);
   
	HIGHP vec4 pos_in_world = iPos; 
	pos_in_world.xz = pos_in_world.xz + c_vChunkPos.xz;

	pos_in_world.xz = pos_in_world.xz - c_vReferPos.xz;
   
#ifdef SHADOWMAP
    oViewPos = c_mtxView * pos_in_world;

	#ifdef PSSM4
		oShadowPos[3] = c_mtxLightViewProj4 * pos_in_world;
	#endif
	
	#ifdef PSSM3
		oShadowPos[2] = c_mtxLightViewProj3 * pos_in_world;
	#endif
	
	#ifdef PSSM2
		oShadowPos[1] = c_mtxLightViewProj2 * pos_in_world;
	#endif
	
	#if defined(PSSM2) || defined(PSSM3) || defined(PSSM4)
		oShadowPos[0] = c_mtxLightViewProj1 * pos_in_world;
	#else
		oShadowPos = c_mtxLightViewProj1 * pos_in_world;
	#endif
#endif

	oPos = pos_in_world*c_mtxViewProj;
	gl_Position = oPos;

	oNor = vec4(vec4(input_normal, 0.0) * c_mtxWV).xyz;

#if defined(NORMALMAP0) || defined(NORMALMAP1) || defined(NORMALMAP2) || defined(NORMALMAP3) || defined(NORMALMAP4)
	MEDIUMP vec3 nor = input_normal;
	MEDIUMP vec3 vTangent = normalize(vec3(nor.y, -nor.x, 0.0));
	MEDIUMP vec3 vBinormal = normalize(vec3(0.0, nor.z, -nor.y));
	
	oTangent = vec4(vec4(vTangent, 0.0) * c_mtxWV).xyz;
	oBinormal = vec4(vec4(vBinormal, 0.0) * c_mtxWV).xyz;
#endif
	MEDIUMP vec3 tc = iPos.xyz;
	MEDIUMP vec3 tcd = pos_in_world.xyz;
	tcd = tcd + c_vReferPos.xyz;
   
#if defined(BLEND0)
	oTexCoord01.zw = (vec4(tcd,0.0)*c_Detail0Param).xy;
	oTexCoord01.xy = tc.xz * c_fTerrainBlockSize;
#endif   

#if defined(BLEND1)
	oTexCoord23.xy = (vec4(tcd,0.0)*c_Detail1Param).xy;
#endif

#if defined(BLEND2)
	oTexCoord23.zw = (vec4(tcd, 0.0)*c_Detail2Param).xy;    
#endif

#if defined(BLEND3)
	oTexCoord45.xy = (vec4(tcd, 0.0) * c_Detail3Param).xy;
#endif

#if defined(BLEND4)
	oTexCoord45.zw = (vec4(tcd,0.0) * c_Detail4Param).xy;
#endif

#if defined(LIGHTMAP) || defined(WARFOG)
	#if defined(LIGHTMAP) && defined(WARFOG)
		oTexCoordLB.z = (c_WarFogOffsetAndSize.x - tcd.x) / c_WarFogOffsetAndSize.z;
		oTexCoordLB.w = (tcd.z - c_WarFogOffsetAndSize.y) / c_WarFogOffsetAndSize.w;
		oTexCoordLB.xy = ((tcd.xz - c_ZoneOffsetAndSize.xy) / c_ZoneOffsetAndSize.zw - 0.5) * c_TerrainBlockSize.w + 0.5;
   #else
		#if defined(LIGHTMAP)
			oTexCoordLB.xy = ((tcd.xz - c_ZoneOffsetAndSize.xy) / c_ZoneOffsetAndSize.zw - 0.5) * c_TerrainBlockSize.w + 0.5;
		#else
			oTexCoordLB.x = (c_WarFogOffsetAndSize.x - tcd.x) / c_WarFogOffsetAndSize.z;
			oTexCoordLB.y = (tcd.z - c_WarFogOffsetAndSize.y) / c_WarFogOffsetAndSize.w;
		#endif
   #endif
#endif

#if defined(GRADUALLY_DISAPPEAR)
    MEDIUMP float distViewToVertex = length(pos_in_world.xyz - c_vViewPos);
    oGradually = clamp( (distViewToVertex - c_GraduallyDist.x) / c_GraduallyDist.y ,0.0,1.0);
#endif

#if defined(SMOOTHCLIP)
    oDepth = vec2(length(pos_in_world.xyz - c_vViewPos));
#endif

#if defined(CLIPPLANE)
    oClipDistance = dot(oPos, c_ClipPlane);
#endif
}