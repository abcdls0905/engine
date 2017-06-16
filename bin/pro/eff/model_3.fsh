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

// 计算辐射图光照
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

HIGHP vec3 PhysicalBRDF(vec3 N, vec3 V, vec3 L, float Gloss, vec3 SpecCol)
{
	HIGHP vec3 H = normalize( V + L );
	
	// Compute perceptually linear roughness parameter (squared)
	HIGHP float m = pow( (1.0 - Gloss * 0.7), 6.0 );
	HIGHP float m2 = m * m;
	
	// Prevent highlights from getting too tiny since we don't have real area lights yet
	m2 = max( m2, 0.00001 );
	
	// GGX NDF
	HIGHP float NdotH = clamp( dot( N, H ), 0.0, 1.0 );
	HIGHP float spec = (NdotH * NdotH) * (m2 - 1.0) + 1.0;
	spec = m2 / (spec * spec);
	
	// Schlick-Smith Visibility Term
	HIGHP float k = (0.8 + m * 0.5) * (0.8 + m * 0.5) / 2.0;
	HIGHP float Gv = clamp( dot( N, V ), 0.0, 1.0 ) * (1.0 - k) + k;
	HIGHP float Gl = clamp( dot( N, L ), 0.0, 1.0 ) * (1.0 - k) + k;
	spec /= max( Gv * Gl, 1e-6 );
		
	// Schlick approximation for Fresnel
	HIGHP vec3 fresnel = mix( SpecCol, vec3(1.0, 1.0, 1.0), pow( (1.0 - dot(L, H)) * max(Gloss, 0.7), 5.0 ) ); // 乘以光泽度max(Gloss,0.7)是避免迎光平视时高光过亮的问题，模拟粗糙材质的效果

	return (fresnel * spec) / 4.0;
}

MEDIUMP vec3 Fresnel(vec3 N, vec3 V, vec3 SpecCol, float Gloss)
{
	return mix(SpecCol, vec3(1.0, 1.0, 1.0), pow((1.0 - dot(N, V)) * max(Gloss, 0.7), 5.0));
}

HIGHP float DiffuseBRDF(vec3 N, vec3 V, vec3 L, float Gloss, float NdotL)
{
	#define PI 3.1415926
	// TODO: Merge with Specular BRDF to save a few instructions

	Gloss = max(Gloss, 0.7); // 避免光泽度低的材质出现迎光和背光面亮度相差过大的情况，尤其是地表
	
	HIGHP float m = pow( (1.0 - Gloss * 0.7), 6.0 );
	
	// Approximation of the full quality Oren-Nayar model
	HIGHP float s = dot(L, V) - dot(N, L) * dot(N, V);
	HIGHP float t = s <= 0.0 ? 1.0 : max(max(dot(N, L), dot(N, V)), 1e-6);
	HIGHP float A = 1.0 / (1.0 + (0.5 - 2.0 / (3.0 * PI)) * m);
	HIGHP float B = m * A;
	
	return NdotL * max(A + B * (s / t), 0.0);
}

HIGHP float GetLuminance(HIGHP vec3 color)
{
	// This seems to work better (less same luma ghost trails).
	// CCIR 601 function for luma.
	return dot(color, vec3(0.299, 0.587, 0.114));

	// Rec 709 function for luma.
	//return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main (void)
{
	HIGHP vec3 nor;
	MEDIUMP vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	HIGHP vec3 specular_color = vec3(0.0, 0.0, 0.0);
	HIGHP vec3 light_dir = normalize(c_vLightDir);
	float bias = -1.0;
	
	MEDIUMP vec4 shadow_Inten = vec4(1.0, 0.5, 0.0, 0.0);
	
#ifdef LIGHTMAP1
	MEDIUMP vec3 blend_lightmap_color = vec3(1.0, 1.0, 1.0);
#endif

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
			diffuse_color.w *= oDiffuse.w;
		#endif
	#else
		#ifdef LIGHTMAP
			diffuse_color.xyz *= tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz * c_vLightMapColorScale.xyz;
		#endif
	#endif

	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);
		diffuse_color *= crTexDiffuse;
	#endif

	#ifdef APPENDCOLOR
		diffuse_color *= c_AppendColor;
	#endif

	#ifdef DISAPPEAR
		diffuse_color.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif


	#ifdef CLIPBLEND
		if(diffuse_color.w - 0.9 <0.0)
		{
			discard;
		}
		//clip(diffuse_color.w - 0.9);
	#else
		#ifdef CLIPSOLID
			if(0.9 - diffuse_color.w <0.0)
			{
				discard;
			}
			//clip(0.9 - diffuse_color.w);
		#else
			if(diffuse_color.w - c_fAlphaRefValue <0.0)
			{
				discard;
			}
		#endif
	#endif

	OutputColor0(diffuse_color);
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

	HIGHP float dotNL = clamp(dot(nor, light_dir), 0.0, 1.0);
	
	float gloss_power = 1.0;
	#ifdef SPECULARMAP
        HIGHP vec4 spec_tex = tex2DBiasGamma(tex_SpecularMap, oTex0.xy, bias);
		specular_color *= spec_tex.xyz;
		gloss_power = spec_tex.w;
	#endif
	
	#ifdef GLOSSINESSMAP
		HIGHP vec4 gloss_tex = tex2DBias(tex_GlossinessMap, oTex0.xy, bias);
		gloss_power = gloss_tex.x;
	#endif
	
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
			HIGHP float diffuseTerm = DiffuseBRDF(nor, view_dir, light_dir, specular_power, dotNL);
	
			#ifdef SHADOWMAP
				diffuseTerm *= shadow_Inten.x;
			#endif
			
			
		#if defined(LIGHTMAP1)
			blend_lightmap_color.xyz = tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz * c_vLightMapColorScale.xyz;
			
			#ifdef SOFT_GAMMA
				diffuse_light.rgb = diffuseTerm * blend_lightmap_color * DECODE_TO_LINEAR(1.2);
			#else
				diffuse_light.rgb = diffuseTerm * blend_lightmap_color * DECODE_TO_LINEAR(0.8);
			#endif
		#else
			diffuse_light.rgb = diffuseTerm * c_LightDiffuse.rgb;
		#endif 
		
		#endif
		
		#if defined CAMERALIGHT && defined VERTEX_NORMAL
			diffuse_color.xyz += c_CameraLightDiffuse.xyz * oCameraLightInten;
		#endif

		diffuse_color.xyz *= c_MaterialDiffuse.xyz;

		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
			diffuse_color.w *= oDiffuse.w;
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

			diffuse_color.xyz = diffuse_color.xyz * lightmap_color.xyz;

			lightmap_color.w = (1.0 - lightmap_color.w) * 0.5;
		
			#if defined(ALPHASHADOWMAP)
				MEDIUMP float shadow = min(lightmap_color.w / max(dot(light_dir.xyz, nor), 0.0), 1.0);
				
				#if defined(NORMALMAP)
					MEDIUMP float diffuseTerm = DiffuseBRDF(nor, view_dir, light_dir, specular_power, dotNL);
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

		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
		#endif

		#if defined(SHADOWMAP) && !defined(ALPHASHADOWMAP)
			diffuse_color.xyz *= shadow_Inten.x;
		#endif

		#ifdef POINTLIGHT
			MEDIUMP vec3 to_point_vector = c_PointLightPosition.xyz - oViewToVertex.xyz;
			MEDIUMP float to_point_distance = length(to_point_vector);
			MEDIUMP float LdotN = dot(to_point_vector / to_point_distance, nor);
			MEDIUMP float point_light_inten = max(LdotN, 0.0);
			MEDIUMP float point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition.w);
			
			diffuse_color.xyz += point_light_inten * point_light_rang
				* c_MaterialDiffuse.xyz * c_PointLightDiffuse.xyz;
				
			#ifdef POINTLIGHT2
				to_point_vector = c_PointLightPosition2.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition2.w);

				diffuse_color.xyz += point_light_inten * point_light_rang
					* c_MaterialDiffuse.xyz * c_PointLightDiffuse2.xyz;
			#endif
			
			#ifdef POINTLIGHT3
				to_point_vector = c_PointLightPosition3.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition3.w);

				diffuse_color.xyz += point_light_inten * point_light_rang
					* c_MaterialDiffuse.xyz * c_PointLightDiffuse3.xyz;
			#endif
			
			#ifdef POINTLIGHT4
				to_point_vector = c_PointLightPosition4.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition4.w);

				diffuse_color.xyz += point_light_inten * point_light_rang
					* c_MaterialDiffuse.xyz * c_PointLightDiffuse4.xyz;
			#endif
		#endif
	
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
			HIGHP vec4 crTexFilter = tex2DBias(tex_FilterMap, oTexFilter, bias);

			#ifdef FILTERMAP_ALPHA
				crTexFilter.a = tex2DBias(tex_FilterAlpha, oTexFilter.xy, bias).r;
			#endif
			
			crTexDiffuse.rgb *= crTexFilter.rgb * c_FilterParam.x + vec3(c_FilterParam.yyy);
			crTexDiffuse.a *= max(crTexFilter.a * c_FilterParam.z + c_FilterParam.w, 0.0);
		#endif
		
		diffuse_color *= crTexDiffuse;// *  max(dot(normalize(light_dir), nor), 0.0);
	#endif

	#ifdef APPENDCOLOR
		diffuse_color *= c_AppendColor;
	#endif

	#ifdef DISAPPEAR
		diffuse_color.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif

	#ifdef CLIPBLEND
		if(diffuse_color.w - 0.9 <0.0)
		{
			discard;
		}
		//clip(diffuse_color.w - 0.9);
	#else
		#ifdef CLIPSOLID
			if(0.9 - diffuse_color.w <0.0)
			{
				discard;
			}
			//clip(0.9 - diffuse_color.w);
		#else
			#ifdef ALPHATEST
				#ifndef ONLYCOLORPASS
					#ifdef ALPHATEST_GREATERQUAL
						if(diffuse_color.w - c_fAlphaRefValue >= 0.0)
						{
							discard;
						}
					#else
						if(diffuse_color.w - c_fAlphaRefValue <0.0)
						{
							discard;
						}
					#endif
				#endif
				//clip(diffuse_color.w - c_fAlphaRefValue);
			#endif
		#endif
	#endif

	//#if defined REFLECTION
		//MEDIUMP vec3 reflect_inten = vec3(c_MaterialDiffuse.w, c_MaterialDiffuse.w, c_MaterialDiffuse.w);
		//#if defined SPECULARMAP
			//reflect_inten *= specular_material.xyz;
		//#endif
	//#endif
	
	HIGHP vec3 specular_light = vec3(0.0);

	#if defined SPECULAR || defined SPECULARMAP	
	
		#if defined (LIGHTMAP1)
			specular_power = DECODE_TO_LINEAR(specular_power);
			specular_power *= blend_lightmap_color.x * 0.299 + blend_lightmap_color.y * 0.587 + blend_lightmap_color.z * 0.114;
		#endif
		
		float dinten = dotNL;
		specular_light = PhysicalBRDF(nor, view_dir, normalize(light_dir), specular_power, specular_color) * dinten * c_LightDiffuse.rgb;

		#ifdef DYNAMICLIGHTING
			// Simple energy conservation between specular and diffuse
			diffuse_light *= clamp(1.0 - GetLuminance(specular_color), 0.0, 1.0);  // Sat required to handle precision issues
		#endif
	#endif
	
	#ifdef DYNAMICLIGHTING
		#if !defined(LIGHTMAP1)
			diffuse_light.rgb += ambient_light;
		#else
			#ifdef SOFT_GAMMA
				float scale_value = 1.4;
			#else
				float scale_value = 1.2;
			#endif
			
			#if defined(SHADOWMAP)
				diffuse_light.rgb += ((blend_lightmap_color * DECODE_TO_LINEAR(scale_value))) * shadow_Inten.x;
			#else
				diffuse_light.rgb += ((blend_lightmap_color * DECODE_TO_LINEAR(scale_value)));
			#endif
		#endif

		#ifdef POINTLIGHT
			MEDIUMP vec3 to_point_vector = c_PointLightPosition.xyz - oViewToVertex.xyz;
			MEDIUMP float to_point_distance = length(to_point_vector);
			MEDIUMP float LdotN = dot(to_point_vector / to_point_distance, nor);
			MEDIUMP float point_light_inten = max(LdotN, 0.0);
			MEDIUMP float point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition.w);
			
			diffuse_light.xyz += point_light_inten * point_light_rang * c_PointLightDiffuse.xyz;
				
			#ifdef POINTLIGHT2
				to_point_vector = c_PointLightPosition2.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition2.w);

				diffuse_light.xyz += point_light_inten * point_light_rang * c_PointLightDiffuse2.xyz;
			#endif
			
			#ifdef POINTLIGHT3
				to_point_vector = c_PointLightPosition3.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition3.w);

				diffuse_light.xyz += point_light_inten * point_light_rang * c_PointLightDiffuse3.xyz;
			#endif
			
			#ifdef POINTLIGHT4
				to_point_vector = c_PointLightPosition4.xyz - oViewToVertex.xyz;
				to_point_distance = length(to_point_vector);
				LdotN = dot(to_point_vector / to_point_distance, nor);
				point_light_inten = max(LdotN, 0.0);
				point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition4.w);

				diffuse_light.xyz += point_light_inten * point_light_rang * c_PointLightDiffuse4.xyz;
			#endif
		#endif
		
		diffuse_color.xyz *= diffuse_light;
	#endif

	#if defined EMISSIVEMAP
		#ifdef SKINEFFECT
			MEDIUMP vec4 crTexSubSurface = tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			#else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
				diffuse_color.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
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

	#if defined(REFLECTION)
		MEDIUMP float reflect_factor = c_ReflectScale.x;
		MEDIUMP vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		
		#if defined IOS
			vReflect.y = -vReflect.y;
		#endif
		
		#if defined ES2
			MEDIUMP vec4 crTexReflect = texCubeBias(tex_ReflectionMap, vReflect, (1.0 - specular_power) * c_ReflectScale.y);
		#else
			MEDIUMP vec4 crTexReflect = texCubeLod(tex_ReflectionMap, vReflect, (1.0 - specular_power) * c_ReflectScale.y);
		#endif
		
		MEDIUMP vec3 reflect_color = Fresnel(nor, view_dir, specular_color, specular_power);
		
		specular_light += crTexReflect.xyz * reflect_color * reflect_factor;
	#endif

	// #if defined DIFFUSEMAP && defined SPECULAR && !defined SPECULARMAP
		// specular_color.xyz *= crTexDiffuse.w;
	// #endif

	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		#if defined ALPHASHADOWMAP || defined LIGHTMAP1
			diffuse_color.xyz += specular_light.xyz * shadow_Inten.x;
		#else
			diffuse_color.xyz += specular_light.xyz;	
		#endif
	#endif

	#if defined FALLOFF
		MEDIUMP float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);
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

	#ifdef WARFOG
		MEDIUMP vec2 vTexCoord = oTexWarFog.xy;
		MEDIUMP vec4 crWarFog = tex2DBias(tex_WarFog, vTexCoord, bias);

		diffuse_color.xyz = diffuse_color.xyz * mix(c_vWarFogRange.x, c_vWarFogRange.y, crWarFog.w);
	#endif

	#ifdef BWCOLOR
		MEDIUMP float bwdiffuse = (diffuse_color.x + diffuse_color.y + diffuse_color.z) * 0.3;
		diffuse_color.xyz = vec3(bwdiffuse,bwdiffuse,bwdiffuse);
	#endif

	#if defined(SHADOWMAP) && defined(PSSM_DEBUG)
		diffuse_color *= shadow_color_debug;
	#endif
	
	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(diffuse_color);
#endif
#endif
}
