//#define FOGLINEAR 1
//#define FOGEXP 1
//#define SHADOWEDMAP 1
//#define PRELIGHT 1
//#define LIGHTMAP 1
//#define EARLYZ 1
 
  

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

 
uniform sampler2D tex_Diffuse;
uniform sampler2D tex_Shadow;
uniform sampler2D tex_Prelight; 

void main()
{
    mediump vec4 oColor;
    mediump float flerpvalue2; 
#if defined( LIGHTMAP )
	mediump vec4 Ambient = vec4(c_LightAmbient.xyz,0.0) * c_MaterialAmbient * 0.5;
	
	oColor = texture2D(tex_Prelight, oLightTexcoord) * 2.0;
	oColor = texture2D(tex_Diffuse, oTex0) * oColor * (0.5 + 0.5 * oDiffuse.x);
#else
	mediump vec4 crDTex = texture2D(tex_Diffuse, oTex0);


#ifndef EARLYZ
	if( (crDTex.w - c_fAlphaRef)<0.0)
	{
	  discard;
	}
#endif

	//mediump vec3 vNormal = normalize(oNormal);
	mediump vec3 vNormal = oNormal;

	#if defined(SHADOWEDMAP) || defined(PRELIGHT)
		mediump vec2 sc = iPos.xy * c_PixelSize.xy;
	#endif

	#if defined(SHADOWEDMAP)
		mediump vec4 shadowInten = texture2D(tex_Shadow), sc);
	#endif

	#if defined(PRELIGHT)
		mediump vec4 prelight = texture2D(tex_Prelight, sc) * 2.0;
	#endif
	
	mediump vec3 ambient_light = c_LightAmbient.xyz;
	//mediump float light = 0.5 + oHeight * 0.5;
	mediump float light = oHeight ;
	mediump float dinten = max(dot(vNormal, c_vLightDir), 0.0);

	#if defined(SHADOWEDMAP)
		mediump vec3 model_color = (c_MaterialAmbient.xyz * ambient_light + 
			c_MaterialDiffuse.xyz * c_LightDiffuse.xyz * dinten * shadowInten.x) * light;
	#else
		mediump vec3 model_color = (c_MaterialAmbient.xyz * ambient_light + 
			c_MaterialDiffuse.xyz * c_LightDiffuse.xyz * dinten) * light;
	#endif
		
	#if defined(PRELIGHT)
		model_color.xyz += c_MaterialDiffuse.xyz * prelight.xyz;
	#endif
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
	model_color *= crDTex.xyz;

	oColor = vec4(model_color, crDTex.w);
#endif  

	oColor.w *= oDiffuse.w;
	
#if defined(FOGEXP)
    mediump float flerpvalue1 = 1.0 / exp(oFogDepth * c_FogExpParam.w);
	oColor.xyz = c_FogExpParam.xyz*(1.0 - flerpvalue1) +  oColor.xyz * flerpvalue1,
#endif

#if defined(FOGLINEAR)
    flerpvalue2 = clamp( (oFogDepth - c_FogParam.x) * c_FogParam.y,0.0,1.0);
  	oColor.xyz = oColor.xyz*(1.0 - flerpvalue2) + c_FogColor.xyz*flerpvalue2;
#endif 

   gl_FragColor = oColor;
}