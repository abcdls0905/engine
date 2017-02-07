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

// cbPerGroup
uniform mediump float c_fTerrainBlockSize;
uniform mediump vec2 c_GraduallyDist;
uniform mediump vec4 c_TerrainBlockSize;
uniform mediump vec2 c_Detail0Param;
uniform mediump vec4 c_MaterialDiffuse;
uniform mediump vec4 c_MaterialAmbient;
uniform mediump vec4 c_MaterialSpecular;
uniform mediump vec4 c_TessellationFactor;

uniform mediump vec3 c_vLightMapColorScale;

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

#if defined(SHADOWEDMAP)
varying mediump vec4 oTexCoordShadow;
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

uniform mediump vec4 c_HeightFogColor;
uniform mediump vec4 c_HeightFogParam;
#endif

uniform sampler2D tex_Detailmap0;
uniform sampler2D tex_Detailmap1;
uniform sampler2D tex_Detailmap2;
uniform sampler2D tex_Detailmap3;
uniform sampler2D tex_Detailmap4;
uniform sampler2D tex_Blendmap;
uniform sampler2D tex_Lightmap;
uniform sampler2D tex_Shadow;
uniform sampler2D tex_Prelight;
uniform sampler2D tex_Depthmap;
uniform sampler2D tex_Bumpmap0;
uniform sampler2D tex_Bumpmap1;
uniform sampler2D tex_Bumpmap2;
uniform sampler2D tex_Bumpmap3;
uniform sampler2D tex_Bumpmap4;


void main()
{
   mediump vec4 crTex = vec4(0.0,0.0,0.0,0.0);
   mediump vec4 oColor = vec4(0.0,0.0,0.0,0.0);

#if defined(SMOOTHCLIP)
   if(( c_fFarClipDistance - 1.0 - oDepth.x ) <0.0 )
   {
     discard;
   }
#endif  

#if defined(BLEND0)
   mediump vec4 crDtail0 = texture2D(tex_Detailmap0,oTexCoord01.zw);         
#endif

#if defined(BLEND1)
   mediump vec4 crDtail1 = texture2D(tex_Detailmap1,oTexCoord23.xy);
   mediump vec4 crBlend = texture2D(tex_Blendmap,oTexCoord01.xy);
   crTex += crDtail1 * crBlend.w;   
#endif

#if defined(BLEND2)
   mediump vec4  crDtail2 = texture2D(tex_Detailmap2,oTexCoord23.zw);
   crTex += crDtail2 * crBlend.x;
#endif  

#if defined(BLEND3)
   mediump vec4  crDtail3 = texture2D(tex_Detailmap3,oTexCoord45.xy);
   crTex += crDtail3 * crBlend.y;
#endif

#if defined(BLEND4)
    mediump vec4 crDtail4 = texture2D(tex_Detailmap4,oTexCoord45.zw);
    crTex += crDtail4 * crBlend.z;
#endif

#if defined(BLEND0)
    mediump float crBlend0 = 1.0;
    #if defined(BLEND1)
      crBlend0 -= crBlend.w;
    #endif
    #if defined(BLEND2)
      crBlend0 -= crBlend.x;
    #endif
    #if defined(BLEND3)
      crBlend0 -= crBlend.y;
    #endif
    #if defined(BLEND4)
      crBlend0 -= crBlend.z;
    #endif
    crTex += crDtail0 * crBlend0;
  
#endif
  
#if defined(SHADOWEDMAP)
    //mediump vec2 tex_uv = (oPos.xy + 0.5) * c_PixelSize;
	mediump vec2 tex_uv = (vec2(0.5, 0.5) * (oTexCoordShadow.xy / oTexCoordShadow.w)) + vec2(0.5, 0.5);
    mediump vec4 shadowInten = texture2D(tex_Shadow, tex_uv);

    #if defined(PRELIGHT)
     mediump vec4 prelight = texture2D(tex_Prelight,tex_uv) * 2.0;
    #endif
#endif  

#if defined(SPECULAR)
    mediump vec3 nor = normalize(oNor);
#endif
  
#if defined(NORMALMAP)
    mediump vec4 normalmap_color = vec4(0.0,0.0,0.00001,0.0);
    mediump float specular_power = 0.0;
    
    #if defined(BLEND0)
      normalmap_color += texture2D(tex_Bumpmap0,oTexCoord01.zw) * crBlend0;     
      #if defined(SPECULAR)
        specular_power += c_fBaseSpecularPower * crBlend0;
      #endif 
    #endif
    
    #if defined(BLEND1)
		normalmap_color += texture2D(tex_Bumpmap1,oTexCoord23.xy) * crBlend.w;
		#if defined(SPECULAR)
			specular_power += c_BlendSpecularPower.x * crBlend.w;
		#endif    		
    #endif
    
    #if defined(BLEND2)
        normalmap_color += texture2D(tex_Bumpmap2,oTexCoord23.zw) * crBlend.x;
        #if defined(SPECULAR)
          specular_power += c_BlendSpecularPower.y * crBlend.x;
        #endif
    #endif
    
    #if defined(BLEND3)
        normalmap_color += texture2D(tex_Bumpmap3,oTexCoord45.xy) * crBlend.y;
        #if defined(SPECULAR)
			specular_power += c_BlendSpecularPower.z * crBlend.y;          
        #endif
    #endif
    
    #if defined(BLEND4)
        normalmap_color += texture2D(tex_Bumpmap4,oTexCoord45.zw) * crBlend.z;
        #if defined(SPECULAR)
          specular_power += c_BlendSpecularPower.w * crBlend.z;
        #endif
    #endif
    
    mediump vec3 vNormal = normalize(normalmap_color.xyz * 2.0 - 1.0);
    mediump vec3 vLightDir = normalize(oLightDirection);
    
    #if defined(SPECULAR)
       mediump vec3 vReflection = reflect(normalize(oViewDirection), vNormal);
       mediump float sinten = pow(max(dot(vReflection, vLightDir), 0.000001), specular_power);
       #if defined(SHADOWEDMAP)
         sinten *= shadowInten.x;
       #endif
       mediump vec3 specular = c_MaterialSpecular.xyz * sinten * normalmap_color.w;
    #else
       mediump vec3 specular = vec3(0.0,0.0,0.0);   
    #endif
    
    mediump float dinten = max(dot(vNormal,vLightDir), 0.0);  
    
#else
    #if defined(SPECULAR)
      mediump vec3 vReflection = reflect(normalize(oViewDirection), nor);
      mediump float sinten = pow(max(dot(vReflection, c_vLightDir), 0.000001), c_fBaseSpecularPower);
      #if defined(SHADOWEDMAP)
         sinten *= shadowInten.x;
      #endif
      mediump vec3 specular = c_MaterialSpecular.xyz * sinten;
      mediump float dinten = max(dot(nor, c_vLightDir), 0.0);
    #else
      mediump float dinten = oDinten;
    #endif   
#endif   

   mediump vec4 crLight = vec4(1.0,1.0,1.0,1.0);
   
#if defined(LIGHTMAP)
   crLight = texture2D(tex_Lightmap,oTexCoordLB.xy);
	crLight.xyz = crLight.xyz * c_vLightMapColorScale.xyz;
  // #if defined(NORMALMAP)
  //   crLight.xyz = crLight.w * c_MaterialDiffuse.xyz * dinten * c_LightDiffuse 
		//+ c_MaterialAmbient.xyz * c_LightAmbient.xyz; 
  // #endif
#else
   mediump vec3 ambient_light = c_LightAmbient.xyz;
        
   #if defined(SPHEREAMBIENT)
      ambient_light *= oGraduallyAmbient.y;
   #endif
   
   #if defined(NORMALMAP)
      ambient_light *= normalmap_color.w;            
   #endif

    
   #if defined(SHADOWEDMAP)
      crLight.xyz = dinten * c_LightDiffuse.xyz * shadowInten.x;  
   #else
      crLight.xyz = dinten * c_LightDiffuse.xyz; 
   #endif
   
   #if defined(SHADOWEDMAP)&&defined(PRELIGHT)
      crLight.xyz += prelight.xyz;
   #endif
   
   crLight.xyz *= c_MaterialDiffuse.xyz;      
   
   #if defined(SHADOWEDMAP)
      crLight.xyz += c_MaterialAmbient.xyz * ambient_light * shadowInten.y;   
   #else 
      crLight.xyz += c_MaterialAmbient.xyz * ambient_light; 
   #endif
   
#endif
   
   oColor.xyz =  crLight.xyz * crTex.xyz;
   oColor.w = 1.0;
  
#if defined(GRADUALLY_DISAPPEAR)
   #if defined(BLEND0)
     oColor.xyz = oColor.xyz*(1.0 - oGraduallyAmbient.x) + crLight.xyz * crDtail0.xyz * oGraduallyAmbient.x;
   #else
     oColor.xyz = oColor.xyz*(1.0 - oGraduallyAmbient.x) + crLight.xyz * oGraduallyAmbient.x;
   #endif
#endif

#if defined(SPECULAR)
   //#if defined(LIGHTMAP)
   //  specular.xyz *= crLight.w; 
   //#endif
   
   #if defined(SHADOWEDMAP)
     specular.xyz *= shadowInten.x;
   #endif
   
   #if defined(FOGLINEAR)||defined(FOGEXP)
     specular.xyz *= 1.0 - min(oDepth.x / 150.0, 1.0);
   #endif
   
   oColor.xyz += specular.xyz * c_LightDiffuse.xyz * crTex.w;
#endif

#if defined(FOGEXP)
 	oColor.xyz = c_FogExpParam.xyz*(1.0 - 1.0 / exp(oDepth.x * c_FogExpParam.w)) + oColor.xyz*1.0 / exp(oDepth.x * c_FogExpParam.w);  
#endif
   
#if defined(FOGLINEAR)
	mediump float lerpvalue = clamp((oDepth.x - c_FogParam.x) * c_FogParam.y,0.0,1.0);
	oColor.xyz = oColor.xyz * (1.0 - lerpvalue) + c_FogColor.xyz*lerpvalue;
#endif   

#ifdef HEIGHT_FOG
	mediump float hDelta = oVertexPos.y - c_HeightFogParam.x;
	hDelta = step(hDelta, 0.0) * clamp(hDelta * c_HeightFogParam.y, 0.0, 1.0);

	oColor.xyz = oColor.xyz * (1.0 - hDelta) + c_HeightFogColor.xyz * hDelta;
#endif
   
  gl_FragColor = oColor;
}
