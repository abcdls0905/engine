uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_Bump;
uniform MEDIUMP sampler2D tex_SpecularMap;
uniform MEDIUMP samplerCube tex_ReflectionMap;
uniform MEDIUMP sampler2D tex_LightMap;
uniform MEDIUMP sampler2D tex_EmissiveMap;
uniform MEDIUMP sampler2D tex_FilterMap;
uniform MEDIUMP sampler2D tex_FilterAlpha;
uniform MEDIUMP sampler2D tex_SceneRefract;

#if defined DEPTHMAP && defined ES3
		uniform HIGHP sampler2D tex_DepthMap;
#endif

#ifdef USE_PCF
	uniform HIGHP sampler2DShadow tex_Shadow;
#else
	uniform MEDIUMP sampler2D tex_Shadow;
#endif

#if defined DYNAMIC_RIPPLE
uniform MEDIUMP sampler2D tex_DynamicRipple;
#endif

//uniform MEDIUMP sampler2D tex_WarFog;
uniform MEDIUMP sampler2D tex_LightMap1;
uniform MEDIUMP sampler2D tex_LightMap2;
uniform MEDIUMP sampler2D tex_AmbientOcclusion;

#ifdef SHADOWMAP
	uniform HIGHP float c_ShadowParam;
#endif

#ifdef CAMERALIGHT
	uniform HIGHP vec4 c_CameraLightDiffuse;
#endif

// PS Constants
uniform HIGHP vec4 c_FallOffParam;
uniform HIGHP vec4 c_MaterialAmbient;
uniform HIGHP vec4 c_MaterialAmbientEx;
uniform HIGHP vec4 c_SubSurfaceParam;

uniform HIGHP float c_fBumpScale;
uniform HIGHP vec2 c_ReflectScale;

//scene
uniform HIGHP vec3 c_vLightDir;
uniform HIGHP vec4 c_LightAmbient;
uniform HIGHP vec4 c_LightDiffuse;

//???
uniform HIGHP vec4 c_PointLightPosition;
uniform HIGHP vec4 c_PointLightDiffuse;

uniform HIGHP mat4 c_mtxViewInverse;
uniform HIGHP vec4 c_MaterialDiffuse;
uniform HIGHP vec4 c_MaterialEmissive;
uniform HIGHP vec3 c_MaterialSpecular;
uniform HIGHP vec4 c_AppendColor;

uniform HIGHP float c_fSpecularPower;
uniform HIGHP float c_fAlphaRefValue;
uniform HIGHP float c_fTime;

#ifdef DEPTHMAP
	uniform HIGHP vec4  c_DepthParam;
#endif	

#if defined VOLUME_WATER || defined VOLUME_WATER_FOG
uniform HIGHP vec2 c_PixelSize;
#endif

uniform HIGHP vec3 c_vLightMapColorScale;

uniform HIGHP vec4 c_FilterParam;

uniform HIGHP vec4 c_HDRScaleInfo[6];

#if defined EMISSIVE_SPECIAL
	uniform float c_fEmissiveIntensity;
	uniform float c_fEmissiveAlphaMultiplier;
#endif

//#if defined WARFOG
//	uniform HIGHP vec4 c_vWarFogRange;
//#endif

#if defined WATER || defined VOLUME_WATER
	#if defined FOAM
		uniform MEDIUMP sampler2D tex_Foam;
		
		uniform MEDIUMP vec4 c_WaveParam; // [Foam] x:speed;y:tilling;z:intersect_factor;w:amount
	#endif
#endif

#ifdef LUM_SCALE
	uniform HIGHP float c_fInvLumScale;
#endif

psin HIGHP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin HIGHP vec3 oNormal;

	#if defined LIGHTMAP
		psin HIGHP vec4 oTex1;
	#endif
#endif

#ifdef FILTERMAP
	psin HIGHP vec2 oTexFilter;
#endif

#if (defined SABER_ARC && defined REFRACTION) || defined NORMALMAP
	psin HIGHP vec3 oTangent;
	psin HIGHP vec3 oBinormal;
#endif

psin HIGHP vec4 oDiffuse;
psin HIGHP vec4 oViewToVertex;
psin HIGHP vec2 oClipDistance;

#ifdef DEPTHMAP
	psin HIGHP vec4 oFovCoord;
#endif

#ifdef SHADOWMAP
	psin HIGHP vec4 oTexCoordShadow;
#endif

#if !defined SABER_ARC && (defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG)
	psin HIGHP vec4 oFog;
#endif

#ifdef CAMERALIGHT
	psin HIGHP	float oCameraLightInten;
#endif

//#if defined WARFOG
//	psin HIGHP vec2 oTexWarFog;
//#endif

#if defined EMISSIVE_SPECIAL
	psin HIGHP vec2 oTexEmissiveSpecial;
#endif

#if defined SABER_ARC
	#if defined FOGLINEAR || defined FOGEXP || defined REFRACTION
		psin HIGHP float oFogDepth;
	#endif

	uniform HIGHP vec4 c_FogColor;
	uniform HIGHP vec4 c_FogParam;
	uniform HIGHP vec4 c_FogExpParam;
#endif

#if defined WATER
	psin HIGHP vec4 oTex1;  //if rendering water， xy = bumpUv1; zw= v_foamUv
	psin HIGHP vec4 oDarkColor;// xyz = dark color, w = reflection power
	psin HIGHP vec4 oLightColor;
	psin HIGHP vec4 oLightMap;
#endif

#if defined FOAM
	psin HIGHP vec2 oFoamTex;
#endif

#if defined DYNAMIC_RIPPLE
	psin HIGHP vec2 oRipplesCoord;
#endif

HIGHP vec4 lerp(HIGHP vec4 u, HIGHP vec4 v, HIGHP float t)
{
	return u + (v-u)*t;
}

HIGHP vec3 lerp(HIGHP vec3 u, HIGHP vec3 v, HIGHP float t)
{
	return u + (v-u)*t;
}

HIGHP vec3 normal_calc(HIGHP vec3 map_normal, HIGHP float bump_scale)
{
	HIGHP vec3 v = vec3(0.5, 0.5, 1.0);
	map_normal = lerp(v, map_normal, bump_scale);
	map_normal = (map_normal * 2.0) - 1.0;
	return map_normal;
}

// 计算辐射图光照
HIGHP vec3 RNM_calc(vec3 normalmap, 
	vec3 lightmap1, vec3 lightmap2, vec3 lightmap3, 
	vec3 lightMapMin, vec3 lightMapRange, 
	vec3 lightMapMin1,vec3 lightMapRange1, 
	vec3 lightMapMin2,vec3 lightMapRange2)
{
	const HIGHP vec3 bumpBasis = vec3(0.8165, 0.0, 0.577);
	const HIGHP vec3 bumpBasis1 = vec3(-0.408, -0.707, 0.577);
	const HIGHP vec3 bumpBasis2 = vec3(-0.408, 0.707, 0.577);

	HIGHP vec3 dp;

	dp.x = dot(normalmap, bumpBasis);
	dp.y = dot(normalmap, bumpBasis1);
	dp.z = dot(normalmap, bumpBasis2);
	dp = clamp(dp, 0.0, 1.0);

	return dp.x * (lightmap1 * lightMapRange + lightMapMin) 
		+ dp.y * (lightmap2 * lightMapRange1 + lightMapMin1) 
		+ dp.z * (lightmap3 * lightMapRange2 + lightMapMin2);
}

HIGHP vec3 PhysicalBRDF(vec3 N, vec3 V, vec3 L, float Gloss, vec3 SpecCol)
{
	HIGHP vec3 H = normalize( V + L );
	
	// Compute perceptually linear roughness parameter (squared)
	HIGHP float m = pow( (1.0 - Gloss * 0.7), 6.0 );
	HIGHP float m2 = m * m;
	
	// Prevent highlights from getting too tiny since we don't have real area lights yet
	m2 = max( m2, 0.00001 );
	
	// GGX NDF
	HIGHP float NdotH = max(dot( N, H ), 0.0);
	HIGHP float spec = (NdotH * NdotH) * (m2 - 1.0) + 1.0;
	spec = m2 / (spec * spec);
	
	/*
	// Schlick-Smith Visibility Term
	HIGHP float k = (0.8 + m * 0.5) * (0.8 + m * 0.5) / 2.0;
	HIGHP float Gv = clamp( dot( N, V ), 0.0, 1.0 ) * (1.0 - k) + k;
	HIGHP float Gl = clamp( dot( N, L ), 0.0, 1.0 ) * (1.0 - k) + k;
	spec /= max( Gv * Gl, 1e-6 );
	*/
		
	// Schlick approximation for Fresnel
	HIGHP vec3 fresnel = mix( SpecCol, vec3(1.0, 1.0, 1.0), pow( (1.0 - dot(L, H)), 5.0 ) );

	return (fresnel * spec) / 4.0;
}

HIGHP vec3 Fresnel(vec3 N, vec3 V, vec3 SpecCol)
{
	return mix(SpecCol, vec3(1.0, 1.0, 1.0), pow((1.0 - dot(N, V)), 5.0));
}

HIGHP float GetLuminance(HIGHP vec3 color)
{
	// This seems to work better (less same luma ghost trails).
	// CCIR 601 function for luma.
	return dot(color, vec3(0.299, 0.587, 0.114));

	// Rec 709 function for luma.
	//return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

#if defined DEPTHMAP && defined ES3

//DecodeDepthTexture
MEDIUMP float DecodeFloatRGBA(MEDIUMP vec4 rgba ) 
{
	return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

HIGHP float getlinerdepth(in HIGHP sampler2D sdepth,in HIGHP vec2 UV)
{
  //HIGHP float zvalue = DecodeFloatRGBA(tex2D( sdepth , UV ));
  HIGHP float zvalue = tex2D( sdepth , UV ).x;
  
  zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
  
  return zvalue;
}

#endif

#ifdef SHADOWMAP
	#ifdef USE_PCF
	float shadow_lookup( float x, float y )                              
	{                                                              
	   HIGHP float pixelSize = 0.0009765625;                          
	   HIGHP vec4 offset = vec4 ( x * pixelSize * oTexCoordShadow.w,       
	                        y * pixelSize * 2.0 * oTexCoordShadow.w,
	                        0.0, 0.0);                            
	   HIGHP vec4 tex_uv = oTexCoordShadow * 0.5 + 0.5;
	   return shadow2DProjEXT(tex_Shadow, tex_uv + offset);
	}  
	#endif

HIGHP vec4 final_shadow_color(in HIGHP float shadow)
{
	HIGHP float v  = 1.0 - clamp((1.0 - shadow) * c_ShadowParam, 0.0, 1.0);

	HIGHP vec4 fShadow = vec4(v, v, v, v);
	return fShadow;
}
#endif

void main (void)
{
	HIGHP vec3 nor;
	HIGHP float bias = 0.0;
	
#ifdef SABER_ARC
	HIGHP vec4 output_color = oDiffuse;
	HIGHP vec2 tc = oTex0.xy;
	
	#ifdef REFRACTION
		#ifdef DIFFUSEMAP
			HIGHP vec4 crTex = tex2DGamma(tex_Diffuse , tc) - vec4(0.5, 0.5, 0.5, 0.0);
			output_color = vec4((crTex.x * oBinormal.xy + crTex.y * oTangent.xy) * vec2(2.0, -2.0), 0, 1.0);
		#endif
		
		output_color.xy += 0.5;
		output_color.w = oDiffuse.w * crTex.w;
	#else
		#ifdef DIFFUSEMAP
			HIGHP vec4 crTex = tex2DGamma(tex_Diffuse , tc);
			output_color = output_color * crTex;
		#endif

		#ifdef FOGEXP
			output_color.xyz = lerp(c_FogExpParam.xyz, output_color.xyz,
				1.0 / exp(oFogDepth * c_FogExpParam.w));
		#endif

		#ifdef FOGLINEAR
			output_color.xyz = lerp(output_color.xyz, c_FogColor.xyz,
				clamp((oFogDepth - c_FogParam.x) * c_FogParam.y, 0.0, 1.0));
		#endif
	#endif
	
	#ifdef LUM_SCALE
		output_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(output_color);
	
#elif defined REFRACTION
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			nor = normal_calc(tex2D(tex_Bump, oTex0.xy).xyz, c_fBumpScale);	
			nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
			nor = normalize(nor);
		#else
			nor = normalize(oNormal);
		#endif
	#else
		nor = vec3(0.0, 0.0, 0.0);
	#endif

	vec4 crRefraction = vec4(0.5, 0.5, 0.5, 0.5);
	vec4 specColor = vec4(0.3, 0.2, 0.1, 1.0);

	#ifdef  DIFFUSEMAP
		crRefraction = tex2DGamma(tex_Diffuse, oTex0.xy);
	#endif

	crRefraction.xyz -= 0.5;

	#ifdef VERTEX_COLOR
		crRefraction *= oDiffuse;
	#endif

	#ifdef OPACITY
		crRefraction.w *= c_AppendColor.w;
	#endif

	vec4 finalColor;

	#ifdef NORMALMAP
		finalColor.xy = crRefraction.xy * c_MaterialAmbient.xy;

		finalColor.xy += crRefraction.x * oTangent.xy
			+ finalColor.y * oBinormal.xy
			+ finalColor.z * oNormal.xy;
	#else
		finalColor.xyz = crRefraction.xyz * c_MaterialAmbient.xyz;
		finalColor.xy += finalColor.z * nor.xy;
	#endif

		finalColor.xyz += 0.5;
		finalColor.w = crRefraction.w / exp(oClipDistance.y * 0.05);

	OutputColor0(finalColor);

#elif defined WATER_SIMPLE

#elif defined VOLUME_WATER_FOG
	HIGHP vec4 output_color = vec4(0.0);
	
	vec3 fog_color = c_MaterialAmbient.rgb;
	
	#ifdef APPENDCOLOR
		fog_color *= c_AppendColor.xyz;
	#endif
	
	vec2 ProjTexcoord = gl_FragCoord.xy * c_PixelSize.xy;
	
	#if defined DEPTHMAP && defined ES3
		HIGHP float depth = getlinerdepth(tex_DepthMap, ProjTexcoord);
		depth = min(depth, oFovCoord.w);
		//depth *= length(oViewToVertex.xyz / oFovCoord.w);
	#else
		// 使用默认雾浓度
		HIGHP float depth = 10.0;
	#endif
	
	float fog_alpha = 1.0 - exp(-depth * c_AppendColor.a);
	
	#if defined OPACITY || !defined NORMALMAP
		output_color.xyz = fog_color;
		output_color.a = fog_alpha;
	#else
		vec2 normal = tex2D(tex_Bump, ProjTexcoord + c_fTime).xy;
	
		vec4 BackGroundColor = tex2D(tex_SceneRefract, ProjTexcoord + normal * 0.03);
		
		#ifdef LUM_SCALE
			// 恢复BackGround颜色
			BackGroundColor.xyz /= c_fInvLumScale;
		#endif

		output_color.xyz = fog_color * fog_alpha + BackGroundColor.xyz * (1.0 - fog_alpha);
		output_color.a = 1.0;
	#endif
	
	#ifdef LUM_SCALE
		output_color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(output_color);

#elif defined VOLUME_WATER
	HIGHP vec4 output_color;
	vec4 DiffuseCol = vec4(c_MaterialDiffuse.rgb, c_AppendColor.a);
	vec3 AmbientCol = c_MaterialAmbient.rgb;
	vec3 LightCol = c_LightDiffuse.rgb;
	vec3 Normal = vec3(0.0, 1.0, 0.0);
	vec3 LightDir = c_vLightDir;
	float BumpScale = c_fBumpScale;
	float ReflectAmount = c_ReflectScale.x;
	
	#if defined SPECULAR || defined SPECULARMAP || defined NORMALMAP
		vec3 ViewDir = normalize(-oViewToVertex.xyz);
	#endif
	
	vec3 Ambient = vec3(0.0);
	
	#if defined SPECULAR
		vec3 SpecColor = c_MaterialSpecular.rgb;
		float SpecPow = c_fSpecularPower;
	#endif
	
	float Time = c_fTime;

	#if defined DIFFUSEMAP
		DiffuseCol *= tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);
	#endif
	
	#if defined VERTEX_NORMAL
		#if defined NORMALMAP
			vec2 parm0 = vec2(1.0, 2.0);
			vec2 parm1 = vec2(3.0, -2.0);
			vec2 parm2 = vec2(-4.0, 1.0);
			vec2 parm3 = vec2(1.0, 3.0);
		
			#ifndef IOS_METAL
				vec3 NormalMap = tex2DBias(tex_Bump, oTex0.xy + Time * parm0, bias).xyz * 0.25;
			
				NormalMap += tex2DBias(tex_Bump, oTex0.xy + Time * parm1, bias).xyz * 0.25;
				NormalMap += tex2DBias(tex_Bump, oTex0.xy + Time * parm2, bias).xyz * 0.25;
				NormalMap += tex2DBias(tex_Bump, oTex0.xy + Time * parm3, bias).xyz * 0.25;
			#else
				vec3 NormalMap = textureLod(tex_Bump, oTex0.xy + Time * parm0, 0.0).xyz * 0.25;
		
				NormalMap += textureLod(tex_Bump, oTex0.xy + Time * parm1, 0.0).xyz * 0.25;
				NormalMap += textureLod(tex_Bump, oTex0.xy + Time * parm2, 0.0).xyz * 0.25;
				NormalMap += textureLod(tex_Bump, oTex0.xy + Time * parm3, 0.0).xyz * 0.25;
			#endif
		
			Normal = normal_calc(NormalMap, BumpScale);
			
			vec2 normal_bias = Normal.xy * 0.5;
			
			Normal = Normal.x * oTangent + Normal.y * oBinormal + Normal.z * oNormal;
			
			#if defined DYNAMIC_RIPPLE
				vec2 cRipples = tex2DBias(tex_DynamicRipple, oRipplesCoord, bias).xy;
				cRipples = cRipples * 2.0 - 1.0;
				
				Normal.xz += cRipples.xy;
				normal_bias += cRipples.xy * BumpScale * 0.5;
			#endif
			
			Normal = normalize(Normal);
		#else
			Normal = oNormal;
		#endif
	#endif
	
	#if defined DIFFUSE
		DiffuseCol.xyz *= oDiffuse.xyz;
	#endif
	
	float fNdotL = max(dot(Normal, LightDir), 0.0);
	output_color.rgb = fNdotL * DiffuseCol.rgb;
	
	#ifndef IOS_METAL
		if (gl_FrontFacing)
		{
			Normal *= -1.0;
		}
	#endif
	
	vec3 ReflectColor = vec3(0.0);
	
	#if defined SPECULAR
		#ifdef SPECULARMAP
			SpecColor *= tex2DGamma(tex_SpecularMap, oTex0.xy).xyz;
		#endif
		// 菲涅耳效应
		//float lrp = 1.0 - dot(ViewDir.xyz, Normal);
		//output_color.a = saturate(ReflectAmount + pow(lrp, 6.0f));
		//output_color.a = clamp(ReflectAmount + pow(max(lrp, 0.000001), 6.0), 0.0, 1.0);
		
		ReflectColor = Fresnel(Normal, ViewDir, SpecColor);
		
		#if defined REFLECTION
			vec3 ReflectDir = reflect(normalize(oViewToVertex.xyz), Normal).xyz;
			
			#if defined IOS
				ReflectDir.y = -ReflectDir.y;
			#endif
		
			vec4 env = texCube(tex_ReflectionMap, ReflectDir);
			output_color.rgb += env.rgb * ReflectAmount * ReflectColor;
		#endif
		
		//vec3 H = normalize(LightDir + ViewDir);
		//float Specular = pow(max(dot(Normal, H), 0.000001), SpecPow);
		//output_color.rgb += Specular * SpecColor.rgb * output_color.a;
		
		float dinten = clamp(dot(Normal, LightDir), 0.0, 1.0);
		
		output_color.rgb += PhysicalBRDF(Normal, ViewDir, LightDir, SpecPow, SpecColor) * dinten * LightCol;
	#endif
	
	#ifdef OPACITY
		vec4 BackGroundColor = vec4(0.0);
		
		output_color.a = GetLuminance(ReflectColor.xyz);
	#else
		vec2 ProjTexcoord = gl_FragCoord.xy * c_PixelSize.xy;
	
		#ifdef NORMALMAP
			vec4 BackGroundColor = tex2D(tex_SceneRefract, ProjTexcoord + normal_bias);
		#else
			vec4 BackGroundColor = tex2D(tex_SceneRefract, ProjTexcoord);
		#endif
		
		#ifdef LUM_SCALE
			// 恢复BackGround颜色
			BackGroundColor.xyz /= c_fInvLumScale;
		#endif
		
		output_color.a = 1.0;
	#endif
	
	// 雾计算及水下雾重叠部雾效剥离
	#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
		HIGHP vec4 FogColor = oFog;
	#endif
	
	#ifdef OPACITY
		output_color.rgb += mix(AmbientCol.rgb, BackGroundColor.rgb, exp(-DiffuseCol.a * 10.0)) * (1.0 - output_color.a);
	
		#if defined FOGLINEAR || defined FOGEXP
			output_color.rgb = FogColor.rgb * output_color.a + output_color.rgb * FogColor.a;
		#endif
	#else
		#if defined FOGLINEAR || defined FOGEXP
			FogColor.a += 0.001;
			BackGroundColor.rgb = (BackGroundColor.rgb - FogColor.rgb) / FogColor.a;
		#endif
	
		output_color.rgb += BackGroundColor.rgb * (vec3(1.0) - ReflectColor);
		
		// 边缘过渡
		#if defined DEPTHMAP && defined ES3
			HIGHP float depth = getlinerdepth(tex_DepthMap, ProjTexcoord);
			HIGHP float border = clamp((depth - oFovCoord.w) * 0.5, 0.0, 1.0);
		
			output_color.rgb = mix(BackGroundColor.rgb, output_color.rgb, border);
			
			// 泡沫
			#if defined FOAM
				float foam_tilling = c_WaveParam.y;
				HIGHP float intersect_factor = c_WaveParam.z;
				float foam_amount = c_WaveParam.w;
				HIGHP vec2 foam_uv = oFoamTex * foam_tilling;
			
				#if defined NORMALMAP
					float foam = tex2D(tex_Foam, foam_uv + normal_bias * 0.2).x;
				#else
					float foam = tex2D(tex_Foam, foam_uv).x;
				#endif
				
				HIGHP float foam_intersect = clamp((depth - oFovCoord.w) * intersect_factor, 0.0, 1.0);
				
				foam = foam * clamp(-foam_intersect + foam, 0.0, 1.0);
				foam = foam * clamp(foam_intersect - foam_intersect * foam_intersect, 0.0, 1.0);
				
				output_color.rgb += foam * (LightCol * fNdotL + c_LightAmbient.rgb) * foam_amount;
			#endif
		#endif
	
		#if defined FOGLINEAR || defined FOGEXP
			output_color.rgb = FogColor.rgb + output_color.rgb * FogColor.a;
		#endif
	#endif
	
	#ifdef LUM_SCALE
		output_color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(output_color);
	
#elif defined WATER
	vec4 normalMapValue = vec4(0.0, 0.0, 0.0, 0.0);

	#ifdef  DIFFUSEMAP
		normalMapValue = tex2D(tex_Diffuse, oTex1.xy);
	#endif

	HIGHP vec4 diffuse_color = vec4(mix(oLightColor.xyz, oDarkColor.xyz, (normalMapValue.x * oTex0.y) + (normalMapValue.y * (1.0 - oTex0.y))), oTex0.x);

	#if defined REFLECTION
		float reflectFractor = exp2(log2(((normalMapValue.z * oTex0.y) + (normalMapValue.w * (1.0 - oTex0.y))) * oTex0.z) * oTex0.w + oDarkColor.w);
		diffuse_color.xyz = diffuse_color.xyz + reflectFractor * c_LightDiffuse.xyz;
	#endif

	#ifdef USE_FOAM
		#if defined LIGHTMAP
			vec3 foamColor = tex2DGamma(tex_LightMap, v_worldPos).xyw * vec3(tex2DGamma(tex_Foam, oTex1.zw).x * 1.5, 1.3, 1.0);
		#else
			vec3 foamColor = vec3(tex2DGamma(tex_Foam, oTex1.zw).x * 1.5, 1.3, 1.0);
		#endif

			diffuse_color = mix(diffuse_color, vec4(0.92, 0.92, 0.92, foamColor.x), min(0.92, foamColor.x)) * foamColor.yyyz;
	#endif
	
	#ifdef BLEND_ENHANCE
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w;
		#endif
	#else
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w + oFog.xyz;
		#endif
	#endif

	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(diffuse_color);
	
#elif defined NOLIGHT
	HIGHP vec4 oColor;
	HIGHP vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			nor = normal_calc(tex2DBias(tex_Bump, oTex0.xy, bias).xyz, c_fBumpScale);
			nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
			nor = normalize(nor);
		#else
			nor = normalize(oNormal);
		#endif
	#else
		nor = vec3(0.0, 0.0, 0.0);
	#endif
	
	#ifdef DIFFUSEMAP
		HIGHP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2DBias(tex_DiffuseAlpha, oTex0.xy, bias).r;
		#endif

		#ifdef SATURATION
			HIGHP float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
		#endif

		#ifdef FILTERMAP
			HIGHP vec4 crTexFilter = tex2DBias(tex_FilterMap, oTexFilter, bias);

			#ifdef FILTERMAP_ALPHA
				crTexFilter.a = tex2DBias(tex_FilterAlpha, oTexFilter.xy, bias).r;
			#endif
			
			crTexDiffuse.rgb *= crTexFilter.rgb * c_FilterParam.x + vec3(c_FilterParam.yyy);
			crTexDiffuse.a *= max(crTexFilter.a * c_FilterParam.z + c_FilterParam.w, 0.0);
		#endif

		diffuse_color *= crTexDiffuse;// *  max(dot(normalize(light_dir), nor), 0.0);
	#endif
		
	#ifdef DIFFUSE
		diffuse_color.rgb *= oDiffuse.rgb;
		diffuse_color.w *= oDiffuse.a;
	#endif
	
	#ifdef ALPHATEST
		if(diffuse_color.w - c_fAlphaRefValue <0.0)
		{
			discard;
		}
	#endif
		
	#ifdef APPENDCOLOR
		diffuse_color *= c_AppendColor;
	#endif
	
	HIGHP vec3 ambient_light = c_LightAmbient.xyz;
	//#ifdef SPHEREAMBIENT
	//	// hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
	//	ambient_light *= 0.75 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
	//#endif
	
	#ifdef EMISSIVE
		#ifdef EMISSIVE_SPECIAL
			#ifdef EMISSIVEMAP
				HIGHP vec4 crTexEmissive = tex2DBiasGamma(tex_EmissiveMap, oTexEmissiveSpecial.xy, bias);
				diffuse_color *= crTexEmissive;
			#endif
			
			diffuse_color.xyz *= c_MaterialEmissive.xyz;
			diffuse_color.xyz *= vec3(c_fEmissiveIntensity);
			
			diffuse_color.a *= c_fEmissiveAlphaMultiplier;
		#endif
	#endif
	
	diffuse_color.rgb *= c_MaterialAmbient.rgb*ambient_light; // 相当于LightAmbient为1, LightDiffuse为0
	
	#ifdef BLEND_ENHANCE
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w;
		#endif
	#else
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w + oFog.xyz;
		#endif
	#endif
	
	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(diffuse_color);
#else
	#ifdef CLIPPLANE
		if(oClipDistance.x < 0.0)
		{
			discard;
		}
	#endif

	HIGHP vec4 diffuse_color = vec4(1,1,1,1);
	HIGHP vec3 specular_color = vec3(0, 0, 0);
	
	#ifdef SHADOWMAP
		HIGHP vec2 tex_uv = (vec2(0.5, 0.5) * (oTexCoordShadow.xy / oTexCoordShadow.w)) + vec2(0.5, 0.5);
		HIGHP vec4 shadow_Inten = tex2DBias(tex_Shadow, tex_uv, bias);

		#ifdef USE_PCF
			float sum = shadow_lookup(-2.0, -2.0);
			sum += shadow_lookup(-2.0, 0.0);
			sum += shadow_lookup(-2.0, 2.0);
			sum += shadow_lookup(0.0, -2.0);
			sum += shadow_lookup(0.0, 0.0);
			sum += shadow_lookup(0.0, 2.0);
			sum += shadow_lookup(2.0, -2.0);
			sum += shadow_lookup(0.0, 0.0);
			sum += shadow_lookup(2.0, 2.0);                             
			                                                                                            
			sum = sum * 0.1111; //divided by 9                                        
			HIGHP vec4 shadow_Inten = vec4(sum);
			shadow_Inten = final_shadow_color(shadow_Inten.x);
		#else
			shadow_Inten = final_shadow_color(shadow_Inten.x);
		#endif
	#endif  
	
	HIGHP vec3 light_dir = c_vLightDir;
	
	#ifdef DYNAMICLIGHTING
			
		#ifdef VERTEX_NORMAL
			#ifdef NORMALMAP
				nor = normal_calc(tex2DBias(tex_Bump, oTex0.xy, bias).xyz, c_fBumpScale);	
				nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
				nor = normalize(nor);
			#else
				nor = normalize(oNormal);
			#endif
		#else
			nor = vec3(0.0, 0.0, 0.0);		
		#endif

		HIGHP vec3 ambient_light = c_LightAmbient.xyz;
		//#ifdef SPHEREAMBIENT
		// 	// hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
		//	#ifdef SKELETON
		//		ambient_light *= 1.0 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
		//	#else
		//		ambient_light *= 0.75 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
		//	#endif
		//#endif
		
		#ifndef VERTEX_NORMAL
			diffuse_color.xyz = vec3(0.0, 0.0, 0.0);
		#else
			#ifdef SHADOWMAP
				diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz * shadow_Inten.x;
			#else 
				diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz;
			#endif
		#endif

		#ifdef  CAMERALIGHT
			diffuse_color.xyz += c_CameraLightDiffuse.xyz * oCameraLightInten;
		#endif
		
		diffuse_color.xyz *= c_MaterialDiffuse.xyz;
		
		#ifdef SHADOWMAP
			diffuse_color.xyz += c_MaterialAmbient.xyz * ambient_light * shadow_Inten.x;
		#else 
			diffuse_color.xyz += c_MaterialAmbient.xyz * ambient_light;
		#endif
		
		#ifdef POINTLIGHT
			HIGHP vec3 to_point_vector = c_PointLightPosition.xyz - oViewToVertex.xyz;
			HIGHP float to_point_distance = length(to_point_vector);
			HIGHP float LdotN = dot(to_point_vector / to_point_distance, nor);
			HIGHP float point_light_inten = max(LdotN, 0.0);
			HIGHP float point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition.w);
			
			diffuse_color.xyz += point_light_inten * point_light_rang
				* c_MaterialDiffuse.xyz * c_PointLightDiffuse.xyz;
		#endif
	
		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
			diffuse_color.w *= oDiffuse.w;
		#endif
	#else
		#ifdef LIGHTMAP	
			HIGHP vec3 lightmap_color = tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz;

			#ifdef VERTEX_NORMAL
				#ifdef NORMALMAP
					nor = normal_calc(tex2DBias(tex_Bump, oTex0.xy, bias).xyz, c_fBumpScale);
				
					#ifdef SPHEREAMBIENT // RNM
						HIGHP vec3 lightmap_color1 = tex2DBiasGamma(tex_LightMap1, oTex1.xy, bias).xyz;
						HIGHP vec3 lightmap_color2 = tex2DBiasGamma(tex_LightMap2, oTex1.zw, bias).xyz;

						lightmap_color.xyz = RNM_calc(nor, 
							lightmap_color.xyz, lightmap_color1.xyz, lightmap_color2.xyz, 
							c_HDRScaleInfo[0].xyz,c_HDRScaleInfo[1].xyz,  
							c_HDRScaleInfo[2].xyz,c_HDRScaleInfo[3].xyz,  
							c_HDRScaleInfo[4].xyz,c_HDRScaleInfo[5].xyz);

						HIGHP vec3 ao_color = tex2DBiasGamma(tex_AmbientOcclusion, oTex1.xy, bias).xyz;
						lightmap_color.xyz += ao_color * c_MaterialAmbient.xyz * c_LightAmbient.xyz;
					#else
						lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
					#endif
					
					nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
					nor = normalize(nor);
				#else
					nor = oNormal;
					lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
				#endif
			#else
				lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
			#endif

			diffuse_color.xyz = diffuse_color.xyz * lightmap_color.xyz;
			
		#endif
		
		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
		#endif

		#ifdef SHADOWMAP
			diffuse_color.xyz *= shadow_Inten.x;
		#endif
	
	#endif
			
	#ifdef DIFFUSEMAP
		HIGHP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);
		
		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2D(tex_DiffuseAlpha, oTex0.xy).r;
		#endif
			
		#ifdef SATURATION
			HIGHP float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
		#endif
	
		#ifdef FILTERMAP
			HIGHP vec4 crBaseColor = vec4(c_FilterParam.yyy, c_FilterParam.w);
			HIGHP vec4 crTexFilter = tex2DBiasGamma(tex_FilterMap, oTexFilter, bias) * vec4(c_FilterParam.xxx, c_FilterParam.z);
			crTexFilter += crBaseColor;
			#ifdef FILTERMAP_ALPHA
				crTexFilter.a = tex2DBias(tex_FilterAlpha, oTexFilter.xy, bias).r;
			#endif
			crTexDiffuse *= crTexFilter;
		#endif
	
		diffuse_color *= crTexDiffuse;
	#endif
	
	#ifdef APPENDCOLOR
		diffuse_color *= c_AppendColor;
	#endif
	
		
	#ifdef DISAPPEAR
		diffuse_color.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif
	
	
	//#ifdef CLIPBLEND
	//	if(diffuse_color.w - 0.9 <0.0)
	//	{
	//		discard;
	//	}
	//	//clip(diffuse_color.w - 0.9);
	//#else
	//	#ifdef CLIPSOLID
	//		if(0.9 - diffuse_color.w <0.0)
	//		{
	//			discard;
	//		}
	//		//clip(0.9 - diffuse_color.w);
	//	#else
	//		#ifdef ALPHATEST
	//			#ifndef ONLYCOLORPASS
	//				#ifdef ALPHATEST_GREATERQUAL
	//					if(diffuse_color.w - c_fAlphaRefValue >= 0.0)
	//					{
	//						discard;
	//					}
	//				#else
	//					if(diffuse_color.w - c_fAlphaRefValue <0.0)
	//					{
	//						discard;
	//					}
	//				#endif
	//			#endif
	//		#endif
	//	#endif
	//#endif
	
	#ifdef SPECULARMAP
		HIGHP vec4 specular_tex = tex2DBiasGamma(tex_SpecularMap, oTex0.xy, bias);
		HIGHP vec3 specular_material = c_MaterialSpecular.xyz * specular_tex.xyz;
		HIGHP float gloss = max(c_fSpecularPower, 0.000001) * specular_tex.w;
	#endif
	
	#if defined SPECULAR && !defined SPECULARMAP
		HIGHP vec3 specular_material = c_MaterialSpecular.xyz;
		HIGHP float gloss = max(c_fSpecularPower, 0.000001);
	#endif
	
	#if defined REFLECTION	
		HIGHP vec3 reflect_inten = vec3(c_MaterialDiffuse.w, c_MaterialDiffuse.w, c_MaterialDiffuse.w);
		#if defined SPECULARMAP
			reflect_inten *= specular_material.xyz;
		#endif
	#endif
	
	#if defined SPECULAR || defined SPECULARMAP
		HIGHP float specular_inten = pow(max(dot(reflect(normalize(oViewToVertex.xyz), nor), light_dir), 0.000001), gloss);
		#ifdef SHADOWMAP
			specular_inten *= shadow_Inten.x;
		#endif
	#endif
	
	#if defined SPECULAR || defined SPECULARMAP
		specular_color = specular_inten * specular_material.xyz * c_LightDiffuse.xyz;
	#endif 
	
	#if defined EMISSIVEMAP
		#ifdef SKINEFFECT
			HIGHP vec4 crTexSubSurface = tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			HIGHP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			#else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				HIGHP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
				diffuse_color.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			HIGHP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz * max(shadow_Inten.x, 0.7);
			#else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				diffuse_color.xyz += c_MaterialEmissive.xyz;
			#endif
		#endif
	#endif
	
	#if defined REFLECTION
		HIGHP float reflect_factor = c_MaterialDiffuse.w;
		HIGHP vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		
		#if defined IOS
			vReflect.y = -vReflect.y;
		#endif
			
		HIGHP vec4 crTexReflect = texCube(tex_ReflectionMap, vReflect);
		specular_color += crTexReflect.xyz * reflect_inten.xyz * reflect_factor;
	#endif
	
	#if defined DIFFUSEMAP && defined SPECULAR && !defined SPECULARMAP
		specular_color.xyz *= crTexDiffuse.w;
	#endif
	
	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		diffuse_color.xyz += specular_color.xyz;
	#endif
	
	#if defined FALLOFF
		HIGHP float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);		
		falloff_inten = 1.0 - max(falloff_inten, 0.0);
		falloff_inten = pow(max(falloff_inten, 0.000001), c_FallOffParam.w);
	
		#ifdef DIFFUSEMAP
			diffuse_color.xyz += crTexDiffuse.xyz * falloff_inten * c_FallOffParam.xyz;
		#else
			diffuse_color.xyz += falloff_inten * c_FallOffParam.xyz;
		#endif
	#endif
	
	#ifdef BLEND_ENHANCE
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w;
		#endif
	#else
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			diffuse_color.xyz = diffuse_color.xyz * oFog.w + oFog.xyz;
		#endif
	#endif
	
		diffuse_color.xyz = diffuse_color.xyz + c_MaterialAmbientEx.xyz;
		
//	#ifdef WARFOG
//		HIGHP vec2 vTexCoord = oTexWarFog.xy;
//		HIGHP vec4 crWarFog = tex2DBias(tex_WarFog, vTexCoord, bias);
//
//		diffuse_color.xyz = diffuse_color.xyz * mix(c_vWarFogRange.x, c_vWarFogRange.y, crWarFog.w);
//	#endif
	
	#ifdef BWCOLOR
		HIGHP float bwdiffuse = (diffuse_color.x + diffuse_color.y + diffuse_color.z) * 0.3;
		diffuse_color.xyz = vec3(bwdiffuse,bwdiffuse,bwdiffuse);
	#endif
	
	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(diffuse_color);
#endif

}