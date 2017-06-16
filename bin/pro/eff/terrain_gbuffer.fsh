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

// cbPerGroup
uniform MEDIUMP float c_fTerrainBlockSize;
uniform MEDIUMP vec2 c_GraduallyDist;
uniform MEDIUMP vec4 c_TerrainBlockSize;
uniform MEDIUMP vec4 c_MaterialDiffuse;
uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP vec4 c_MaterialSpecular;
uniform MEDIUMP vec4 c_TessellationFactor;

uniform MEDIUMP vec3 c_vLightMapColorScale;

#if defined(SHADOWMAP)
	uniform MEDIUMP float c_ShadowParam;
	uniform MEDIUMP float c_fShadowDistance;
	uniform MEDIUMP vec4 c_farDistance;
#endif

uniform MEDIUMP sampler2D tex_Detailmap0;
uniform MEDIUMP sampler2D tex_Detailmap1;
uniform MEDIUMP sampler2D tex_Detailmap2;
uniform MEDIUMP sampler2D tex_Detailmap3;
uniform MEDIUMP sampler2D tex_Detailmap4;
uniform MEDIUMP sampler2D tex_Blendmap;
uniform MEDIUMP sampler2D tex_Lightmap;
#ifdef USE_PCF
	uniform HIGHP sampler2DArrayShadow tex_Shadow1;
#else
	uniform HIGHP sampler2DArray tex_Shadow1;
#endif
uniform MEDIUMP sampler2D tex_Bumpmap0;
uniform MEDIUMP sampler2D tex_Bumpmap1;
uniform MEDIUMP sampler2D tex_Bumpmap2;
uniform MEDIUMP sampler2D tex_Bumpmap3;
uniform MEDIUMP sampler2D tex_Bumpmap4;

uniform MEDIUMP sampler2D tex_SpecularMap0;
uniform MEDIUMP sampler2D tex_SpecularMap1;
uniform MEDIUMP sampler2D tex_SpecularMap2;
uniform MEDIUMP sampler2D tex_SpecularMap3;
uniform MEDIUMP sampler2D tex_SpecularMap4;

#ifdef ES3
	layout(location = 1) out HIGHP vec4 DiffuseColor;
	layout(location = 2) out HIGHP vec4 SpecularColor;
	#ifdef LIGHTMAP
		layout(location = 3) out HIGHP vec4 LightMapColor;
	#endif
#endif//ES3

#if defined(BLEND0)
	psin MEDIUMP vec4 oTexCoord01;
#endif

#if defined(BLEND1)||defined(BLEND2)
	psin MEDIUMP vec4 oTexCoord23;
#endif

#if defined(BLEND3)||defined(BLEND4)
	psin MEDIUMP vec4 oTexCoord45;
#endif

#if defined(LIGHTMAP) || defined(WARFOG)
	#if defined(LIGHTMAP) && defined(WARFOG)
		psin MEDIUMP vec4 oTexCoordLB;
	#else
		psin MEDIUMP vec2 oTexCoordLB;
	#endif
#endif

#if defined(GRADUALLY_DISAPPEAR)
	psin MEDIUMP float oGradually;
#endif

#if defined(SMOOTHCLIP)
	psin MEDIUMP vec2 oDepth;
#endif

psin MEDIUMP vec3 oNor;

#if defined(NORMALMAP0) || defined(NORMALMAP1) || defined(NORMALMAP2) || defined(NORMALMAP3) || defined(NORMALMAP4)
	psin MEDIUMP vec3 oTangent;			// gbuffer add
	psin MEDIUMP vec3 oBinormal;		// gbuffer add
#endif

#if defined(SHADOWMAP)
	//vsout MEDIUMP vec4 oTexCoordShadow;
	psin HIGHP vec4 oViewPos;

	#ifdef PSSM4
		psin HIGHP vec4 oShadowPos[4];
	#elif defined(PSSM3)
		psin HIGHP vec4 oShadowPos[3];
	#elif defined(PSSM2)
		psin HIGHP vec4 oShadowPos[2];
	#else
		psin HIGHP vec4 oShadowPos;
	#endif
#endif

psin MEDIUMP vec4 oPos;

#if defined(DEBUG)
	psin MEDIUMP vec4 oTest;
#endif

#if defined(CLIPPLANE)
	psin MEDIUMP float oClipDistance;
#endif

vec3 EncodeSpecColor( vec3 specCol )
{
	vec3 encodedSpecCol = vec3( 0, 127.0/255.0, 127.0/255.0 );
	
	// Y'CbCr
	specCol = sqrt( specCol );
	encodedSpecCol.x = dot( vec3(0.299, 0.587, 0.114), specCol.rgb );
	encodedSpecCol.y += dot( vec3(-0.1687, -0.3312, 0.5), specCol.rgb );
	encodedSpecCol.z += dot( vec3(0.5, -0.4186, -0.0813), specCol.rgb );
	
	return encodedSpecCol;
}

vec3 EncodeNormal(vec3 n)
{
	n = -n;
	vec3 abs_n = abs(n);
	vec2 p = abs_n.xy * (1.0 / (abs_n.x + abs_n.y + abs_n.z));
	vec2 v = (n.z <= 0.0) ? (1.0 - p.yx) : p;
	v = vec2((n.x >= 0.0) ? v.x : -v.x, (n.y >= 0.0) ? v.y: -v.y);
	vec2 enc1023 = v * 511.5 + 511.5;
	
	vec2 residual;
	residual.x = float(fract(enc1023.x) > 0.5) / 3.0;
	residual.y = float(fract(enc1023.y) > 0.5) / 3.0;
	return vec3(floor(enc1023) / 1023.0, residual.x * 2.0 + residual.y);
}

#if defined(SHADOWMAP)
	#ifdef USE_PCF
		HIGHP float tex2DProj(HIGHP sampler2DShadow tex, HIGHP vec4 pos)
		{
			#ifdef ES2
				return shadow2DProjEXT(tex, pos);
			#else
				return textureProj(tex, pos);
			#endif//ES2
		}
		
		float shadow_lookup(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float x, float y, float shadowmap_index)
		{
			HIGHP vec4 offset = vec4 ( x * c_PixelSize.x * shadow_pos.w,
		                        y * c_PixelSize.y * shadow_pos.w,
		                        0.0, 0.0);
			shadow_pos = shadow_pos + offset;
			return texture(tex, vec4(shadow_pos.xy, shadowmap_index, shadow_pos.z));
		}
	#endif

	MEDIUMP vec4 final_shadow_color(in MEDIUMP float shadow, in HIGHP float distance)
	{
		float v = clamp((1.0 - shadow) * c_ShadowParam, 0.0, 1.0);
		float r = clamp(distance / c_fShadowDistance, 0.0, 1.0);
		return vec4(1.0 - v * (1.0 - r * r), 0.5, 1.0, 1.0);
	}
	

	#ifdef USE_PCF
	float calculate_shadow(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#else
	float calculate_shadow(HIGHP sampler2DArray tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#endif
	{
		#ifdef USE_PCF
			HIGHP float sum = shadow_lookup(tex, shadow_pos, -0.5, -0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, -0.5, 0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, 0.5, 0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, 0.5, -0.5, shadowmap_index);
			sum = sum * 0.25;
			return sum;
		#else
			HIGHP float shadow_depth = texture(tex, vec3(shadow_pos.xy, shadowmap_index)).x;
			return (shadow_depth < shadow_pos.z) && (shadow_depth < 1.0) ? 0.0 : 1.0;
		#endif
	}
#endif

void main()
{
	MEDIUMP vec4 crTex = vec4(0.0,0.0,0.0,0.0);
	MEDIUMP vec4 oColor = vec4(0.0,0.0,0.0,0.0);
	
#if defined(SMOOTHCLIP)
	if ((c_fFarClipDistance - 1.0 - oDepth.x) <0.0 )
	{
		discard;
	}
#endif

#if defined(BLEND0)
	MEDIUMP vec4 crDtail0 = tex2D(tex_Detailmap0,oTexCoord01.zw);
#endif

#if defined(BLEND1)
	MEDIUMP vec4 crDtail1 = tex2D(tex_Detailmap1,oTexCoord23.xy);
	MEDIUMP vec4 crBlend = tex2D(tex_Blendmap,oTexCoord01.xy);
	crTex += crDtail1 * crBlend.w;   
#endif

#if defined(BLEND2)
	MEDIUMP vec4  crDtail2 = tex2D(tex_Detailmap2,oTexCoord23.zw);
	crTex += crDtail2 * crBlend.x;
#endif  

#if defined(BLEND3)
	MEDIUMP vec4  crDtail3 = tex2D(tex_Detailmap3,oTexCoord45.xy);
	crTex += crDtail3 * crBlend.y;
#endif

#if defined(BLEND4)
	MEDIUMP vec4 crDtail4 = tex2D(tex_Detailmap4,oTexCoord45.zw);
    crTex += crDtail4 * crBlend.z;
#endif

#if defined(BLEND0)
    MEDIUMP float crBlend0 = 1.0;
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
  
  	MEDIUMP float specular_power = 0.0;
	MEDIUMP vec3 specular_color = vec3(0.0, 0.0, 0.0);
  
#if defined(NORMALMAP0) || defined(NORMALMAP1) || defined(NORMALMAP2) || defined(NORMALMAP3) || defined(NORMALMAP4)
	MEDIUMP vec4 normalmap_color = vec4(0.0,0.0,0.00001,0.0);
	MEDIUMP vec4 default_normalmap_color = vec4(0.5, 0.5, 0.5, 1.0);
	 
    #if defined(BLEND0)
		#if defined(NORMALMAP0)
			normalmap_color += tex2D(tex_Bumpmap0,oTexCoord01.zw) * crBlend0;     
		#else
			normalmap_color += default_normalmap_color * crBlend0;
		#endif
    #endif
    
    #if defined(BLEND1)
		#if defined(NORMALMAP1)
			normalmap_color += tex2D(tex_Bumpmap1,oTexCoord23.xy) * crBlend.w;  	
		#else
			normalmap_color += default_normalmap_color * crBlend.w;
		#endif
    #endif
    
    #if defined(BLEND2)
		#if defined(NORMALMAP2)
			normalmap_color += tex2D(tex_Bumpmap2,oTexCoord23.zw) * crBlend.x;
		#else
			normalmap_color += default_normalmap_color * crBlend.x;
		#endif
    #endif
    
    #if defined(BLEND3)
		#if defined(NORMALMAP3)
			normalmap_color += tex2D(tex_Bumpmap3,oTexCoord45.xy) * crBlend.y;
		#else
			normalmap_color += default_normalmap_color * crBlend.y;
		#endif
    #endif
    
    #if defined(BLEND4)
		#if defined(NORMALMAP4)
			normalmap_color += tex2D(tex_Bumpmap4,oTexCoord45.zw) * crBlend.z;
		#else
			normalmap_color += default_normalmap_color * crBlend.z;
		#endif
    #endif
    
    MEDIUMP vec3 vNormal = normalmap_color.xyz * 2.0 - 1.0;
    vNormal = normalize(vNormal.x * oTangent + vNormal.y * oBinormal + vNormal.z * oNor);
#else
	MEDIUMP vec3 vNormal = normalize(oNor);
#endif   

#if defined(SPECULAR)
	#if defined(BLEND0)
		#if defined(SPECULARMAP0)
			MEDIUMP vec4 specular_tex0 = tex2D(tex_SpecularMap0, oTexCoord01.zw);
			specular_color += specular_tex0.rgb * c_fBaseSpecularLevel * crBlend0;
			specular_power += specular_tex0.a * c_fBaseSpecularPower * crBlend0;
		#else
			specular_color += c_fBaseSpecularLevel * crBlend0;
			specular_power += c_fBaseSpecularPower * crBlend0;
		#endif
    #endif
    
    #if defined(BLEND1)
		#if defined(SPECULARMAP1)
			MEDIUMP vec4 specular_tex1 = tex2D(tex_SpecularMap1, oTexCoord23.xy);
			specular_color += specular_tex1.rgb * c_BlendSpecularLevel.x * crBlend.w;
			specular_power += specular_tex1.a * c_BlendSpecularPower.x * crBlend.w;	
		#else
			specular_color += c_BlendSpecularLevel.x * crBlend.w;
			specular_power += c_BlendSpecularPower.x * crBlend.w;	
		#endif
    #endif
    
    #if defined(BLEND2)
		#if defined(SPECULARMAP2)
			MEDIUMP vec4 specular_tex2 = tex2D(tex_SpecularMap2, oTexCoord23.zw);
			specular_color += specular_tex2.rgb * c_BlendSpecularLevel.y * crBlend.x;
			specular_power += specular_tex2.a * c_BlendSpecularPower.y * crBlend.x;
		#else
			specular_color += c_BlendSpecularLevel.y * crBlend.x;
			specular_power += c_BlendSpecularPower.y * crBlend.x;
		#endif
    #endif
    
    #if defined(BLEND3)
		#if defined(SPECULARMAP3)
			MEDIUMP vec4 specular_tex3 = tex2D(tex_SpecularMap3, oTexCoord45.xy);
			specular_color += specular_tex3.rgb * c_BlendSpecularLevel.z * crBlend.y;
			specular_power += specular_tex3.a * c_BlendSpecularPower.z * crBlend.y;    
		#else
			specular_color += c_BlendSpecularLevel.z * crBlend.y;
			specular_power += c_BlendSpecularPower.z * crBlend.y;    
		#endif
    #endif
    
    #if defined(BLEND4)
		#if defined(SPECULARMAP4)
			MEDIUMP vec4 specular_tex4 = tex2D(tex_SpecularMap4, oTexCoord45.zw);
			specular_color += specular_tex4.rgb * c_BlendSpecularLevel.w * crBlend.z;
			specular_power += specular_tex4.a * c_BlendSpecularPower.w * crBlend.z;
		#else
			specular_color += c_BlendSpecularLevel.w * crBlend.z;
			specular_power += c_BlendSpecularPower.w * crBlend.z;
		#endif
    #endif
	
	specular_color *= c_MaterialSpecular.xyz;
#endif

#ifdef LIGHTMAP
	#ifdef GAMMA_POWER
		oColor.xyz = pow(crTex.xyz, vec3(1.0/2.2, 1.0/2.2, 1.0/2.2));
	#else
		oColor.xyz = crTex.xyz;	
	#endif
#else
	#ifdef GAMMA_POWER
		oColor.xyz = pow(crTex.xyz * c_MaterialDiffuse.rgb, vec3(1.0/2.2, 1.0/2.2, 1.0/2.2));
	#else
		oColor.xyz = crTex.xyz * c_MaterialDiffuse.rgb;	
	#endif
#endif
		
	oColor.w = 1.0;

	vec3 encode_normal = EncodeNormal(vNormal);
	
	OutputColor0(
		vec4(encode_normal.x, encode_normal.y, 0.0, encode_normal.z));
	
	DiffuseColor = oColor;
	specular_color = max(specular_color, 0.001);
	SpecularColor = vec4(specular_power, EncodeSpecColor(specular_color));
	
#ifdef LIGHTMAP
	#ifdef SHADOWMAP
		HIGHP float shadow_depth = 1.0;
		
		#if !defined(PSSM2) && !defined(PSSM3) && !defined(PSSM4)
			shadow_depth = calculate_shadow(tex_Shadow1, oShadowPos, 0.0);
		#else
			vec4 shadow_distance_comparison = step(c_farDistance, vec4(oViewPos.z));
			float shadowmap_index = dot(vec4(1.0,
				#ifdef PSSM2
				1.0,
				#else
				0.0,
				#endif
				
				#ifdef PSSM3
				1.0,
				#else
				0.0,
				#endif
				
				#ifdef PSSM4
				1.0
				#else
				0.0
				#endif
				),shadow_distance_comparison);
			
			HIGHP vec4 shadowPos = oShadowPos[int(shadowmap_index)];
			shadow_depth = calculate_shadow(tex_Shadow1, shadowPos, shadowmap_index);
		#endif
		MEDIUMP vec4 shadowInten = final_shadow_color(shadow_depth, oViewPos.z);
	#endif

	#if defined(SHADOWMAP)
		LightMapColor.rgb = tex2DGamma(tex_Lightmap,oTexCoordLB.xy).xyz * c_vLightMapColorScale.xyz * shadowInten.x;
	#else
		LightMapColor.rgb = tex2DGamma(tex_Lightmap,oTexCoordLB.xy).xyz * c_vLightMapColorScale.xyz;
	#endif
	
	LightMapColor.a = 1.0;
#endif
}
