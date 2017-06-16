uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_Bump;
uniform MEDIUMP sampler2D tex_SpecularMap;
uniform MEDIUMP samplerCube tex_ReflectionMap;
uniform MEDIUMP sampler2D tex_LightMap;
uniform MEDIUMP sampler2D tex_EmissiveMap;
uniform MEDIUMP sampler2D tex_FilterMap;
uniform MEDIUMP sampler2D tex_FilterAlpha;
uniform MEDIUMP sampler2D tex_KeepoutMap;
uniform MEDIUMP sampler2D tex_GlossinessMap;

#if defined(ES3) && !defined(IOS_METAL)
	#ifdef USE_PCF
		uniform HIGHP sampler2DArrayShadow tex_Shadow1;
	#else
		uniform HIGHP sampler2DArray tex_Shadow1;
	#endif
#else
	#ifdef USE_PCF
		uniform HIGHP sampler2DShadow tex_Shadow1;
	#else
		uniform MEDIUMP sampler2D tex_Shadow1;
	#endif

	#ifdef PSSM2
		#ifdef USE_PCF
			uniform HIGHP sampler2DShadow tex_Shadow2;
		#else
			uniform MEDIUMP sampler2D tex_Shadow2;
		#endif
	#endif

	#ifdef PSSM3
		#ifdef USE_PCF
			uniform HIGHP sampler2DShadow tex_Shadow3;
		#else
			uniform MEDIUMP sampler2D tex_Shadow3;
		#endif
	#endif

	#ifdef PSSM4
		#ifdef USE_PCF
			uniform HIGHP sampler2DShadow tex_Shadow4;
		#else
			uniform MEDIUMP sampler2D tex_Shadow4;
		#endif
	#endif
#endif


uniform MEDIUMP sampler2D tex_WarFog;
uniform MEDIUMP sampler2D tex_LightMap1;
uniform MEDIUMP sampler2D tex_LightMap2;
uniform MEDIUMP sampler2D tex_AmbientOcclusion;

#ifdef SHADOWMAP
	uniform MEDIUMP float c_ShadowParam;
	uniform MEDIUMP float c_fShadowDistance;
	uniform MEDIUMP vec4 c_farDistance;
	uniform MEDIUMP int c_ShadowMapCount;
#endif

#ifdef CAMERALIGHT
	uniform MEDIUMP vec4 c_CameraLightDiffuse;
#endif

// PS Constants
uniform MEDIUMP vec4 c_FallOffParam;
uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP vec4 c_MaterialAmbientEx;
uniform MEDIUMP vec4 c_SubSurfaceParam;

uniform MEDIUMP float c_fBumpScale;
uniform MEDIUMP vec2 c_ReflectScale;

//scene
uniform MEDIUMP vec3 c_vLightDir;
uniform MEDIUMP vec4 c_LightAmbient;
uniform MEDIUMP vec4 c_LightDiffuse;

//???
uniform MEDIUMP vec4 c_PointLightPosition;
uniform MEDIUMP vec4 c_PointLightDiffuse;

#ifdef POINTLIGHT2
uniform MEDIUMP vec4 c_PointLightPosition2;
uniform MEDIUMP vec4 c_PointLightDiffuse2;
#endif

#ifdef POINTLIGHT3
uniform MEDIUMP vec4 c_PointLightPosition3;
uniform MEDIUMP vec4 c_PointLightDiffuse3;
#endif

#ifdef POINTLIGHT4
uniform MEDIUMP vec4 c_PointLightPosition4;
uniform MEDIUMP vec4 c_PointLightDiffuse4;
#endif

uniform MEDIUMP mat4 c_mtxViewInverse;
uniform MEDIUMP vec4 c_MaterialDiffuse;
uniform MEDIUMP vec4 c_MaterialEmissive;
uniform MEDIUMP vec3 c_MaterialSpecular;
uniform MEDIUMP vec4 c_AppendColor;

uniform HIGHP float c_fSpecularPower;
uniform MEDIUMP float c_fAlphaRefValue;

uniform MEDIUMP vec3 c_vLightMapColorScale;

uniform MEDIUMP vec4 c_FilterParam;

uniform MEDIUMP vec4 c_HDRScaleInfo[6];

#if defined WARFOG
	uniform MEDIUMP vec4 c_vWarFogRange;
#endif

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

psin MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin HIGHP vec3 oNormal;

	#ifdef LIGHTMAP
		psin MEDIUMP vec4 oTex1;
		#ifdef LIGHTMAP_AO_COLOR
			psin MEDIUMP vec4 oTex2;
		#endif 
	#endif
#endif

#ifdef FILTERMAP
	psin MEDIUMP vec2 oTexFilter;
#endif

#if defined NORMALMAP
	psin MEDIUMP vec3 oTangent;
	psin MEDIUMP vec3 oBinormal;
#endif

psin MEDIUMP vec4 oDiffuse;
psin MEDIUMP vec4 oViewToVertex;

psin MEDIUMP float oClipDistance;

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
	psin MEDIUMP vec4 oFog;
#endif

#ifdef SHADOWMAP
	psin HIGHP vec3 oWorldPos;
	uniform MEDIUMP vec2 c_PixelSize;
	uniform MEDIUMP mat4 c_mtxView;
	uniform MEDIUMP mat4 c_mtxLightViewProj1;
	#ifdef PSSM2
		uniform MEDIUMP mat4 c_mtxLightViewProj2;
	#endif
	#ifdef PSSM3
		uniform MEDIUMP mat4 c_mtxLightViewProj3;
	#endif
	#ifdef PSSM4
		uniform MEDIUMP mat4 c_mtxLightViewProj4;
	#endif
#endif

#if defined(ES3) && defined(SHADOWMAP) && !defined(IOS_METAL)
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

#if defined CAMERALIGHT && defined VERTEX_NORMAL
	psin MEDIUMP	float oCameraLightInten;
#endif

#if defined WARFOG
	psin MEDIUMP vec2 oTexWarFog;
#endif

MEDIUMP vec4 lerp(MEDIUMP vec4 u, MEDIUMP vec4 v, MEDIUMP float t)
{
	return u + (v-u)*t;
}

MEDIUMP vec3 lerp(MEDIUMP vec3 u, MEDIUMP vec3 v, MEDIUMP float t)
{
	return u + (v-u)*t;
}

MEDIUMP vec3 normal_calc(MEDIUMP vec3 map_normal, MEDIUMP float bump_scale)
{
	MEDIUMP vec3 v = vec3(0.5, 0.5, 1.0);
	map_normal = lerp(v, map_normal, bump_scale);
	map_normal = (map_normal * 2.0) - 1.0;
	return map_normal;
}

MEDIUMP vec3 Fresnel(vec3 N, vec3 V, vec3 SpecCol, float Gloss)
{
	return mix(SpecCol, vec3(1.0, 1.0, 1.0), pow((1.0 - dot(N, V)) * max(Gloss, 0.7), 5.0));
}

// º∆À„∑¯…‰Õºπ‚’’
MEDIUMP vec3 RNM_calc(vec3 normalmap,
	vec3 lightmap1, vec3 lightmap2, vec3 lightmap3,
	vec3 lightMapMin, vec3 lightMapRange,
	vec3 lightMapMin1,vec3 lightMapRange1,
	vec3 lightMapMin2,vec3 lightMapRange2)
{
	const MEDIUMP vec3 bumpBasis = vec3(0.8165, 0.0, 0.577);
	const MEDIUMP vec3 bumpBasis1 = vec3(-0.408, -0.707, 0.577);
	const MEDIUMP vec3 bumpBasis2 = vec3(-0.408, 0.707, 0.577);

	MEDIUMP vec3 dp;

	dp.x = dot(normalmap, bumpBasis);
	dp.y = dot(normalmap, bumpBasis1);
	dp.z = dot(normalmap, bumpBasis2);
	dp = clamp(dp, 0.0, 1.0);

	return dp.x * (lightmap1 * lightMapRange + lightMapMin)
		+ dp.y * (lightmap2 * lightMapRange1 + lightMapMin1)
		+ dp.z * (lightmap3 * lightMapRange2 + lightMapMin2);
}

#ifdef SHADOWMAP
	#ifdef USE_PCF
		HIGHP float tex2DProj(HIGHP sampler2DShadow tex, HIGHP vec4 pos)
		{
			#ifdef ES2
				return shadow2DProjEXT(tex, pos);
			#else
				return textureProj(tex, pos);
			#endif//ES2
		}
		
		#if defined(ES3) && !defined(IOS_METAL)
		float shadow_lookup(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float x, float y, float shadowmap_index)
		#else
		float shadow_lookup(HIGHP sampler2DShadow tex, HIGHP vec4 shadow_pos, float x, float y )
		#endif
		{
			HIGHP vec4 offset = vec4 ( x * c_PixelSize.x * shadow_pos.w,
		                        y * c_PixelSize.y * shadow_pos.w,
								0.0, 0.0);
			#if defined(ES3) && !defined(IOS_METAL)
				shadow_pos = shadow_pos + offset;
				return texture(tex, vec4(shadow_pos.xy, shadowmap_index, shadow_pos.z));
			#else
				return tex2DProj(tex, shadow_pos + offset);
			#endif
		}
	#endif

	MEDIUMP vec4 final_shadow_color(in MEDIUMP float shadow, in HIGHP float distance)
	{
		float v = clamp((1.0 - shadow) * c_ShadowParam, 0.0, 1.0);
		float r = clamp(distance / c_fShadowDistance, 0.0, 1.0);
		return vec4(1.0 - v * (1.0 - r * r), 0.5, 1.0, 1.0);
	}
	
#if defined(ES3) && !defined(IOS_METAL)
	#ifdef USE_PCF
	float calculate_shadow(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#else
	float calculate_shadow(HIGHP sampler2DArray tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#endif
#else
	#ifdef USE_PCF
	float calculate_shadow(HIGHP sampler2DShadow tex, HIGHP vec4 shadow_pos)
	#else
	float calculate_shadow(HIGHP sampler2D tex, HIGHP vec4 shadow_pos)
	#endif
#endif
	{
		#ifdef IOS_METAL
			shadow_pos.y = 1.0 - shadow_pos.y;
		#endif
		
		#ifdef USE_PCF
			#if defined(ES3) && !defined(IOS_METAL)
				HIGHP float sum = shadow_lookup(tex, shadow_pos, -0.5, -0.5, shadowmap_index);
				sum += shadow_lookup(tex, shadow_pos, -0.5, 0.5, shadowmap_index);
				sum += shadow_lookup(tex, shadow_pos, 0.5, 0.5, shadowmap_index);
				sum += shadow_lookup(tex, shadow_pos, 0.5, -0.5, shadowmap_index);
			#else
				HIGHP float sum = shadow_lookup(tex, shadow_pos, -0.5, -0.5);
				sum += shadow_lookup(tex, shadow_pos, -0.5, 0.5);
				sum += shadow_lookup(tex, shadow_pos, 0.5, 0.5);
				sum += shadow_lookup(tex, shadow_pos, 0.5, -0.5);
			#endif
			sum = sum * 0.25;
			return sum;
		#else
			#if defined(ES3) && !defined(IOS_METAL)
			HIGHP float shadow_depth = texture(tex, vec3(shadow_pos.xy, shadowmap_index)).x;
			#else
			HIGHP float shadow_depth = tex2D(tex, shadow_pos.xy).x;
			#endif
			return (shadow_depth < shadow_pos.z) && (shadow_depth < 1.0) ? 0.0 : 1.0;
		#endif
	}
#endif

HIGHP float GetLuminance(HIGHP vec3 color)
{
	// This seems to work better (less same luma ghost trails).
	// CCIR 601 function for luma.
	return dot(color, vec3(0.299, 0.587, 0.114));

	// Rec 709 function for luma.
	//return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

#define MSAA 1
HIGHP float stepAA(float y, float x)
{
#if defined MSAA && defined ES3
	float diff = x - y;
	vec2 dd = vec2(dFdx(diff), dFdy(diff));
	return clamp(diff / length(dd), 0.0, 1.0);
#else
	return step(y, x);
#endif
}

void main (void)
{
	HIGHP vec3 nor;
	MEDIUMP vec4 oColor = vec4(1.0, 1.0, 1.0, 1.0);
	HIGHP vec3 specular_color = vec3(0.0, 0.0, 0.0);
	MEDIUMP vec3 light_dir = c_vLightDir;
	float bias = -1.0;
	
	MEDIUMP vec4 shadow_Inten = vec4(1.0, 0.5, 0.0, 0.0);

#ifdef KEEPOUT
	vec4 color = tex2DBias(tex_KeepoutMap, oTex0.xy, bias);
	
	#ifdef LUM_SCALE
		color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(color);
#else
#ifdef ONLYZPASS
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif

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
		
	#ifdef DYNAMICLIGHTING
		#ifdef DIFFUSE
			oColor.w *= oDiffuse.w;
		#endif
	#else
		#ifdef LIGHTMAP
			oColor.xyz *= tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz * c_vLightMapColorScale.xyz;
		#endif
	#endif

	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);
		oColor *= crTexDiffuse;
	#endif

	#ifdef APPENDCOLOR
		oColor *= c_AppendColor;
	#endif

	#ifdef DISAPPEAR
		oColor.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif


	#ifdef CLIPBLEND
		if(oColor.w - 0.9 <0.0)
		{
			discard;
		}
		//clip(oColor.w - 0.9);
	#else
		#ifdef CLIPSOLID
			if(0.9 - oColor.w <0.0)
			{
				discard;
			}
			//clip(0.9 - oColor.w);
		#else
			if(oColor.w - c_fAlphaRefValue <0.0)
			{
				discard;
			}
		#endif
	#endif

	OutputColor0(oColor);
#else
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif

	#ifdef SHADOWMAP
		HIGHP float shadow_depth = 1.0;
		#ifdef PSSM_DEBUG
			vec4 shadow_color_debug = vec4(0.5, 0.5, 0.5, 1.0);
		#endif
		
		HIGHP vec4 world_pos = vec4(oWorldPos, 1.0);
		vec4 oViewPos = c_mtxView * world_pos;
		
		
		#if defined(ES3) && !defined(IOS_METAL)
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
		#else
			#if !defined(PSSM2) && !defined(PSSM3) && !defined(PSSM4)
				HIGHP vec4 v_shadowPos1 = c_mtxLightViewProj1 * world_pos;
				shadow_depth = calculate_shadow(tex_Shadow1, v_shadowPos1);
			#else
				if(oViewPos.z < c_farDistance.x)
				{
					HIGHP vec4 v_shadowPos1 = c_mtxLightViewProj1 * world_pos;
					shadow_depth = calculate_shadow(tex_Shadow1, v_shadowPos1);
				}
				#ifdef PSSM2
					else if(oViewPos.z < c_farDistance.y)
					{
						HIGHP vec4 v_shadowPos2 = c_mtxLightViewProj2 * world_pos;
						shadow_depth = calculate_shadow(tex_Shadow2, v_shadowPos2);
					}
				#endif
				#ifdef PSSM3
					else if(oViewPos.z < c_farDistance.z)
					{
						HIGHP vec4 v_shadowPos3 = c_mtxLightViewProj3 * world_pos;
						shadow_depth = calculate_shadow(tex_Shadow3, v_shadowPos3);
					}
				#endif
				#ifdef PSSM4
					else if(oViewPos.z < c_farDistance.w)
					{
						HIGHP vec4 v_shadowPos4 = c_mtxLightViewProj4 * world_pos;
						shadow_depth = calculate_shadow(tex_Shadow4, v_shadowPos4);
					}
				#endif
			#endif
		#endif
		shadow_Inten = final_shadow_color(shadow_depth, oViewPos.z);
	#endif

	MEDIUMP vec3 diffuse_light = vec3(0.0, 0.0, 0.0);
	MEDIUMP vec3 view_dir = -normalize(oViewToVertex.xyz);
	specular_color = c_MaterialSpecular.rgb;
	HIGHP float specular_power = c_fSpecularPower;
	
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
		MEDIUMP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2DBias(tex_DiffuseAlpha, oTex0.xy, bias).r;
		#endif

		#ifdef SATURATION
			MEDIUMP float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
		#endif

		#ifdef FILTERMAP
			MEDIUMP vec4 crBaseColor = vec4(c_FilterParam.yyy, c_FilterParam.w);
			MEDIUMP vec4 crTexFilter = tex2DBias(tex_FilterMap, oTexFilter, bias) * vec4(c_FilterParam.xxx, c_FilterParam.z);
			crTexFilter += crBaseColor;
			#ifdef FILTERMAP_ALPHA
				crTexFilter.a = tex2DBias(tex_FilterAlpha, oTexFilter.xy, bias).r;
			#endif
			crTexDiffuse *= crTexFilter;
		#endif
		
		oColor *= crTexDiffuse;// *  max(dot(normalize(light_dir), nor), 0.0);
	#endif

	#ifdef APPENDCOLOR
		oColor *= c_AppendColor;
	#endif

	#ifdef DISAPPEAR
		oColor.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif
	
	#ifdef DIFFUSE
		oColor.xyz *= oDiffuse.xyz;
		
		#ifdef DYNAMICLIGHTING
			oColor.w *= oDiffuse.w;
		#endif
	#endif

	#ifdef CLIPBLEND
		if(oColor.w - 0.9 <0.0)
		{
			discard;
		}
		//clip(oColor.w - 0.9);
	#else
		#ifdef CLIPSOLID
			if(0.9 - oColor.w <0.0)
			{
				discard;
			}
			//clip(0.9 - oColor.w);
		#else
			#ifdef ALPHATEST
				#ifndef ONLYCOLORPASS
					#ifdef ALPHATEST_GREATERQUAL
						if(oColor.w - c_fAlphaRefValue >= 0.0)
						{
							discard;
						}
					#else
						if(oColor.w - c_fAlphaRefValue <0.0)
						{
							discard;
						}
					#endif
				#endif
				//clip(oColor.w - c_fAlphaRefValue);
			#endif
		#endif
	#endif

	//HIGHP float dotNL = clamp(dot(nor, light_dir), 0.0, 1.0) > (1.0 - oColor.w);
	HIGHP float dotNL = stepAA((1.0 - oColor.w), dot(nor, light_dir));
	
	float gloss_power = 1.0;
	#ifdef SPECULARMAP
        HIGHP vec4 spec_tex = tex2DBiasGamma(tex_SpecularMap, oTex0.xy, bias);
		specular_color *= spec_tex.xyz;
		gloss_power = spec_tex.w;
	#endif
	
	#ifdef GLOSSINESSMAP
		HIGHP vec4 gloss_tex = tex2DBiasGamma(tex_GlossinessMap, oTex0.xy, bias);
		gloss_power = gloss_tex.x;
	#endif
	
	HIGHP vec3 specular_light = vec3(0.0);
	specular_power *= gloss_power;
	
	#ifdef DYNAMICLIGHTING
		MEDIUMP vec3 ambient_light = c_LightAmbient.xyz;

		//#ifdef SPHEREAMBIENT
		// 	// hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
		//	#ifdef SKELETON
		//		ambient_light *= 1.0 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
		//	#else
		//		ambient_light *= 0.75 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
		//	#endif
		//#endif

		#ifdef VERTEX_NORMAL
			HIGHP float diffuseTerm = dotNL;
	
			#ifdef SHADOWMAP
				diffuseTerm *= shadow_Inten.x;
			#endif
		
			diffuse_light.rgb = diffuseTerm * c_LightDiffuse.rgb;
		#endif
	#else
		#ifdef LIGHTMAP
			//MEDIUMP vec3 lightmap_color = tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz;
			MEDIUMP vec4 lightmap_color = tex2DGamma(tex_LightMap, oTex0.zw);
			
			#ifdef VERTEX_NORMAL
				#ifdef NORMALMAP
					nor = normal_calc(tex2DBias(tex_Bump, oTex0.xy, bias).xyz, c_fBumpScale);

					#ifdef SPHEREAMBIENT // RNM
						MEDIUMP vec3 lightmap_color1 = tex2DBiasGamma(tex_LightMap1, oTex1.xy, bias).xyz;
						MEDIUMP vec3 lightmap_color2 = tex2DBiasGamma(tex_LightMap2, oTex1.zw, bias).xyz;

						lightmap_color.xyz = RNM_calc(nor,
							lightmap_color.xyz, lightmap_color1.xyz, lightmap_color2.xyz,
							c_HDRScaleInfo[0].xyz,c_HDRScaleInfo[1].xyz,
							c_HDRScaleInfo[2].xyz,c_HDRScaleInfo[3].xyz,
							c_HDRScaleInfo[4].xyz,c_HDRScaleInfo[5].xyz);
						
						#ifdef LIGHTMAP_AO_COLOR
							MEDIUMP vec3 ao_color = tex2DBiasGamma(tex_AmbientOcclusion, oTex2.xy, bias).xyz;
							lightmap_color.xyz += ao_color;
						#else
							lightmap_color.xyz += c_LightAmbient.xyz*0.5;
						#endif
						
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

			oColor.xyz = oColor.xyz * lightmap_color.xyz;

			lightmap_color.w = (1.0 - lightmap_color.w) * 0.5;
		
			#if defined(ALPHASHADOWMAP)
				MEDIUMP float shadow = min(lightmap_color.w / max(dot(light_dir.xyz, nor), 0.0), 1.0);
				
				#if defined(NORMALMAP)
					MEDIUMP float diffuseTerm = dotNL;
					lightmap_color.w = shadow * diffuseTerm;
				#endif

				#if defined(SHADOWMAP)
					lightmap_color.w *= shadow_Inten.x;
				#endif
				
				#if defined(SPECULAR)
					shadow_Inten.x *= shadow;
				#endif
				
				diffuse_light.xyz += c_LightDiffuse.xyz * lightmap_color.w;
			#endif
			
		#endif

		#if defined(SHADOWMAP) && !defined(ALPHASHADOWMAP)
			oColor.xyz *= shadow_Inten.x;
		#endif
	
		#ifdef POINTLIGHT
			MEDIUMP vec3 to_light = c_PointLightPosition.xyz - oViewToVertex.xyz;
			MEDIUMP float dist = length(to_light);
			to_light /= dist;
			MEDIUMP float LdotN = stepAA((1.0 - oColor.w), dot(nor, to_light));
			MEDIUMP float point_light_inten = max(LdotN, 0.0);
			MEDIUMP float point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition.w);
			MEDIUMP vec3 point_light_color = point_light_rang * c_PointLightDiffuse.xyz;
			
			diffuse_light.xyz += point_light_inten * c_MaterialDiffuse.xyz * point_light_color;
			
			#if defined SPECULAR || defined SPECULARMAP
				specular_light.xyz += stepAA(specular_power, dot(nor, normalize(view_dir + to_light))) 
					* point_light_inten * specular_color * point_light_color;
			#endif
		#endif
	#endif

	//#if defined REFLECTION
		//MEDIUMP vec3 reflect_inten = vec3(c_MaterialDiffuse.w, c_MaterialDiffuse.w, c_MaterialDiffuse.w);
		//#if defined SPECULARMAP
			//reflect_inten *= specular_material.xyz;
		//#endif
	//#endif

	#if defined SPECULAR || defined SPECULARMAP
		// specular
		//specular_light = (dot(nor, normalize(view_dir + light_dir)) > specular_power) * dotNL * specular_color * c_LightDiffuse.rgb;
		specular_light += stepAA(specular_power, dot(nor, normalize(view_dir + light_dir))) * dotNL * specular_color * c_LightDiffuse.rgb;
	#endif
	
	#ifdef DYNAMICLIGHTING
		#ifdef POINTLIGHT
			MEDIUMP vec3 to_light = c_PointLightPosition.xyz - oViewToVertex.xyz;
			MEDIUMP float dist = length(to_light);
			to_light /= dist;
			MEDIUMP float LdotN = stepAA((1.0 - oColor.w), dot(nor, to_light));
			MEDIUMP float point_light_inten = max(LdotN, 0.0);
			MEDIUMP float point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition.w);
			MEDIUMP vec3 point_light_color = point_light_rang * c_PointLightDiffuse.xyz;
			
			diffuse_light.xyz += point_light_inten * point_light_color;
			
			#if defined SPECULAR || defined SPECULARMAP
				specular_light.xyz += stepAA(specular_power, dot(nor, normalize(view_dir + to_light))) 
					* point_light_inten * specular_color * point_light_color;
			#endif
		#endif
	
		oColor.xyz *= c_MaterialDiffuse.xyz * diffuse_light + c_MaterialAmbient.xyz * ambient_light;
	#endif

	#if defined EMISSIVEMAP
		#ifdef SKINEFFECT
			MEDIUMP vec4 crTexSubSurface = tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				oColor.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			#else
				oColor.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
				oColor.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				oColor.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz * max(shadow_Inten.x, 0.7);
			#else
				oColor.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				oColor.xyz += c_MaterialEmissive.xyz;
			#endif
		#endif
	#endif

	#if defined(REFLECTION) && defined(ES3)
		MEDIUMP float reflect_factor = c_ReflectScale.x;
		MEDIUMP vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		
		#if defined IOS
			vReflect.y = -vReflect.y;
		#endif
		
		MEDIUMP vec4 crTexReflect = texCubeLod(tex_ReflectionMap, vReflect, (1.0 - c_fSpecularPower) * c_ReflectScale.y);
		
		MEDIUMP vec3 reflect_color = Fresnel(nor, view_dir, specular_color, specular_power);
		
		specular_light += crTexReflect.xyz * reflect_color * reflect_factor;
	#endif

	// #if defined DIFFUSEMAP && defined SPECULAR && !defined SPECULARMAP
		// specular_color.xyz *= crTexDiffuse.w;
	// #endif

	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		#if defined ALPHASHADOWMAP
			oColor.xyz += specular_light.xyz * shadow_Inten.x;
		#else
			oColor.xyz += specular_light.xyz;
		#endif
	#endif

	#if defined FALLOFF
		MEDIUMP float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);
		falloff_inten = 1.0 - max(falloff_inten, 0.0);
		falloff_inten = pow(max(falloff_inten, 0.000001), c_FallOffParam.w);

		#ifdef DIFFUSEMAP
			oColor.xyz += crTexDiffuse.xyz * falloff_inten * c_FallOffParam.xyz;
		#else
			oColor.xyz += falloff_inten * c_FallOffParam.xyz;
		#endif
	#endif

	#ifdef BLEND_ENHANCE
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			oColor.xyz = oColor.xyz * oFog.w;
		#endif
	#else
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			oColor.xyz = oColor.xyz * oFog.w + oFog.xyz;
		#endif
	#endif

		oColor.xyz = oColor.xyz + c_MaterialAmbientEx.xyz;

	#ifdef WARFOG
		MEDIUMP vec2 vTexCoord = oTexWarFog.xy;
		MEDIUMP vec4 crWarFog = tex2DBias(tex_WarFog, vTexCoord, bias);

		oColor.xyz = oColor.xyz * mix(c_vWarFogRange.x, c_vWarFogRange.y, crWarFog.w);
	#endif

	#ifdef BWCOLOR
		MEDIUMP float bwdiffuse = (oColor.x + oColor.y + oColor.z) * 0.3;
		oColor.xyz = vec3(bwdiffuse,bwdiffuse,bwdiffuse);
	#endif

	#if defined(SHADOWMAP) && defined(PSSM_DEBUG)
		oColor *= shadow_color_debug;
	#endif
	
	#ifdef LUM_SCALE
		oColor.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(oColor);
#endif
#endif
}
