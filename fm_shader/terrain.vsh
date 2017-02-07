//#define BLEND0 1
//#define BLEND1 1
//#define BLEND2 1
//#define BLEND3 1
//#define BLEND4 1
//#define LIGHTMAP 1
//#define SHADOWEDMAP 1
//#define GRADUALLY_DISAPPEAR 1
//#define FOGLINEAR 1
//#define NORMALMAP 1
//#define SPECULAR 1
//#define PRELIGHT 1
//#define SMOOTHCLIP 1
//#define FIXSHADOW 1
//#define FOGEXP 1
//#define SPHEREAMBIENT 1
//#define CLIPPLANE 1


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

// cbPerScene
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
uniform mediump mat4  c_mtxShadowViewProj;

// cbPerGroup
uniform mediump float c_fTerrainBlockSize;
uniform mediump vec2 c_GraduallyDist;
uniform mediump vec4 c_TerrainBlockSize;
uniform mediump vec2 c_Detail0Param;
uniform mediump vec4 c_MaterialDiffuse;
uniform mediump vec4 c_MaterialAmbient;
uniform mediump vec4 c_MaterialSpecular;
uniform mediump vec4 c_TessellationFactor;

#if defined(SHADOWEDMAP)
varying mediump vec4 oTexCoordShadow;
#endif

#if !defined(NORMALMAP)&&!defined(SPECULAR)
varying mediump float oDinten;
#endif

#if defined(BLEND0)
varying mediump vec4 oTexCoord01;
#endif

#if defined(BLEND1)||defined(BLEND2)
varying mediump vec4 oTexCoord23;
#endif

#if defined(BLEND3)||defined(BLEND4)
varying mediump vec4 oTexCoord45;
#endif

#if defined(LIGHTMAP)
varying mediump vec2 oTexCoordLB;
#endif

#if defined(GRADUALLY_DISAPPEAR)||defined(SPHEREAMBIENT)
varying mediump vec2 oGraduallyAmbient;
#endif

#if defined(FOGLINEAR)||defined(FOGEXP)||defined(SMOOTHCLIP)||defined(FIXSHADOW)
varying mediump vec2 oDepth;
#endif

#if defined(SPECULAR)
varying mediump vec3 oNor;
#endif

#if defined(NORMALMAP)||defined(SPECULAR)
varying mediump vec3 oViewDirection;
#endif

#if defined(NORMALMAP)
varying mediump vec3 oLightDirection;
#endif 

varying mediump vec4 oPos;

#if defined(DEBUG)
varying mediump vec4 oTest;
#endif

#if defined(CLIPPLANE)
varying mediump float oClipDistance;
#endif

#ifdef HEIGHT_FOG
varying mediump vec3 oVertexPos;
#endif

void main(void)
{   
   mediump vec3 input_normal = iDiffuseNormal.xyz;
   input_normal.xz = input_normal.xz*2.0 - 1.0;
   input_normal = normalize(input_normal);
   
   mediump vec4 pos_in_world = iPos; 
   pos_in_world.xz = pos_in_world.xz + c_vChunkPos.xz;
   
#if defined(SHADOWEDMAP)
	oTexCoordShadow = pos_in_world * c_mtxShadowViewProj;
#endif  

	pos_in_world.xz = pos_in_world.xz - c_vReferPos.xz;
   
   oPos = pos_in_world*c_mtxViewProj;
   gl_Position = oPos;
   
#ifdef HEIGHT_FOG
	oVertexPos = pos_in_world.xyz;
#endif	

#if defined(SPECULAR)
   oNor = input_normal;
#endif   

#if defined(NORMALMAP)
   mediump vec3 nor =  input_normal;
   mediump vec3 vTangent = normalize(vec3(nor.y ,-nor.x, 0.0));
   mediump vec3 vBinormal = -normalize(vec3(0.0,-nor.z,nor.y));
   
   oLightDirection.x = dot(vTangent,c_vLightDir);
   oLightDirection.y = dot(vBinormal,c_vLightDir);
   oLightDirection.z = dot(nor,c_vLightDir);
 
#if defined(DEBUG)   
   oTest = vec4(vBinormal,1.0);
#endif
   
   mediump vec3 vViewDir = pos_in_world.xyz - c_vViewPos;
   oViewDirection.x = dot(vTangent, vViewDir);
   oViewDirection.y = dot(vBinormal, vViewDir);
   oViewDirection.z = dot(nor, vViewDir);   
#else
 #if defined(SPECULAR)
   oViewDirection.xyz = pos_in_world.xyz - c_vViewPos;
 #else
   oDinten = max(dot(input_normal,c_vLightDir.xyz), 0.0);
 #endif
#endif
   mediump vec3 tc = iPos.xyz;
   mediump vec3 tcd = pos_in_world.xyz;
   tcd = tcd + c_vReferPos.xyz;
   
#if defined(BLEND0)
   oTexCoord01.zw = tcd.xz * c_Detail0Param.xy;
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

#if defined(LIGHTMAP)
    oTexCoordLB.x = (tcd.x - c_ZoneOffsetAndSize.x) / c_ZoneOffsetAndSize.z;
    oTexCoordLB.y = (tcd.z - c_ZoneOffsetAndSize.y) / c_ZoneOffsetAndSize.w;
#endif

#if defined(GRADUALLY_DISAPPEAR) || defined(SPHEREAMBIENT)
    oGraduallyAmbient = vec2(0.0,0.0);
#endif

#if defined(GRADUALLY_DISAPPEAR)
    mediump float distViewToVertex = length(pos_in_world.xyz - c_vViewPos);
    oGraduallyAmbient.x = clamp( (distViewToVertex - c_GraduallyDist.x) / c_GraduallyDist.y ,0.0,1.0);
#endif

#if defined(SPHEREAMBIENT)
    oGraduallyAmbient.y = 0.75 + iDiffuseNormal.y * 0.25;
#endif

#if defined(FOGLINEAR)||defined(FOGEXP)||defined(SMOOTHCLIP)||defined(FIXSHADOW)
    oDepth = vec2(0.0,0.0);
#endif

#if defined(FOGLINEAR)||defined(FOGEXP)||defined(SMOOTHCLIP)
    oDepth.x = length(pos_in_world.xyz - c_vViewPos);
#endif

#if defined(FIXSHADOW)
    oDepth.y = oPos.w / 1024.0;
#endif

#if defined(CLIPPLANE)
    oClipDistance = dot(oPos, c_ClipPlane);   
#endif 
}