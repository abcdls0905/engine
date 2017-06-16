uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_Bump;
uniform MEDIUMP sampler2D tex_SpecularMap;
uniform MEDIUMP samplerCube tex_ReflectionMap;
uniform MEDIUMP sampler2D tex_EmissiveMap;
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

#ifdef SHADOWMAP
	uniform MEDIUMP float c_ShadowParam;
	uniform MEDIUMP float c_fShadowDistance;
	uniform MEDIUMP vec4 c_farDistance;
	uniform MEDIUMP int c_ShadowMapCount;
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
uniform MEDIUMP vec4 c_MaterialSecondarySpecular;
uniform MEDIUMP vec4 c_AppendColor;

uniform HIGHP float c_fSpecularPower;
uniform MEDIUMP float c_fAlphaRefValue;

uniform MEDIUMP vec3 c_vLightMapColorScale;

uniform MEDIUMP vec4 c_FilterParam;

uniform MEDIUMP vec4 c_HDRScaleInfo[6];

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

uniform MEDIUMP vec4 c_HairParam;

psin MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin HIGHP vec3 oNormal;
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

MEDIUMP vec3 Fresnel(vec3 N, vec3 V, vec3 SpecCol, float Gloss)
{
	return mix(SpecCol, vec3(1.0, 1.0, 1.0), pow((1.0 - dot(N, V)) * max(Gloss, 0.7), 5.0));
}

HIGHP float DiffuseHair(vec3 nor, vec3 light_dir)
{
	float diffuse_wrap = c_HairParam.y;
	float wrap_denom = 1.0 + diffuse_wrap;
	return clamp((dot(nor, light_dir) + diffuse_wrap) / (wrap_denom * wrap_denom), 0.0, 1.0);
	//return saturate(0.75 * dot(nor, light_dir) + 0.25);
}

HIGHP float HairSingleSpecularTerm(vec3 T, vec3 H, float exponent)
{
	HIGHP float dotTH = dot(T, H);
	HIGHP float sinTH = sqrt(max(1.0 - dotTH * dotTH, 0.01));
	return pow(sinTH, exponent);
}

HIGHP vec3 ShiftTangent(vec3 T, vec3 N, float shiftAmount)
{
	return normalize(T + shiftAmount * N);
}

HIGHP vec3 SpecularHair(vec3 nor, vec3 light_dir, vec3 view_dir,
	float specular_power, vec3 specular_color, vec3 secondary_specular_color, vec3 tangent)
{
	specular_power = clamp(specular_power, 0.5, 0.9);

	HIGHP vec2 specularShift = vec2(0.0, c_HairParam.w);

	HIGHP float specularWidth = c_HairParam.z;
	
	HIGHP vec2 specularExp = vec2(
		exp2(10.0 * specular_power + 1.0),
		exp2(10.0 * max(specular_power / specularWidth, 0.4) + 1.0));
	
	HIGHP vec3 specularColor0 = specular_color;
	HIGHP vec3 specularColor1 = secondary_specular_color;
	
	HIGHP vec3 T1 = ShiftTangent(tangent, nor, specularShift.x);
	HIGHP vec3 T2 = ShiftTangent(tangent, nor, specularShift.y);
	
	HIGHP vec3 H = normalize(light_dir + view_dir);
	HIGHP vec3 specular = specularColor0 *
		HairSingleSpecularTerm(T1, H, specularExp.x);

	HIGHP vec3 specular2 = specularColor1 *
		HairSingleSpecularTerm(T2, H, specularExp.y);

	// modulate secondary specular term with noise
	//float specularMask = tex2D(tSpecularMask, i.texCoord * 10.0f);
	//specular2 *= specularMask;

	// specular attenuation for hair facing away from light
	HIGHP float specularAttenuation = clamp(1.75 * dot(nor, light_dir) + 0.25, 0.0, 1.0);

	specular = (specular + specular2) * specularAttenuation;

	return specular;
}

void main (void)
{
	HIGHP vec3 nor;
	MEDIUMP vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	HIGHP vec3 specular_color = vec3(0.0, 0.0, 0.0);
	HIGHP vec3 secondary_specular_color = vec3(0.0, 0.0, 0.0);
	MEDIUMP vec3 light_dir = c_vLightDir;
	float bias = -1.0;
	
	diffuse_color.xyz *= c_MaterialDiffuse.xyz;

	#ifdef DIFFUSE
		diffuse_color.xyz *= oDiffuse.xyz;
		diffuse_color.w *= oDiffuse.w;
	#endif
	
	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2DBias(tex_DiffuseAlpha, oTex0.xy, bias).r;
		#endif
		
		diffuse_color *= crTexDiffuse;
	#endif

	#ifdef APPENDCOLOR
		diffuse_color *= c_AppendColor;
	#endif

	#ifdef DISAPPEAR
		diffuse_color.w *= 1.0 - clamp((oViewToVertex.w - 25.0) / 50.0, 0.0, 1.0);
	#endif

	#ifdef ALPHATEST
		// 实心部分
		if(diffuse_color.w - c_fAlphaRefValue < 0.0)
		{
			discard;
		}
	#endif
	
	vec3 mat_diffuse = diffuse_color.rgb;
	
	float alpha_blend_multiplier = c_HairParam.x;
	diffuse_color.w = clamp(diffuse_color.w * alpha_blend_multiplier, 0.0, 1.0);
	
	MEDIUMP vec4 shadow_Inten = vec4(1.0, 0.5, 0.0, 0.0);

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
	secondary_specular_color = c_MaterialSecondarySpecular.rgb;
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
		HIGHP vec4 gloss_tex = tex2DBiasGamma(tex_GlossinessMap, oTex0.xy, bias);
		gloss_power = gloss_tex.x;
	#endif
	
	specular_power *= gloss_power;
	
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
		HIGHP float diffuseTerm = DiffuseHair(nor, light_dir);

		#ifdef SHADOWMAP
			diffuseTerm *= shadow_Inten.x;
		#endif
		
		diffuse_light.rgb = diffuseTerm * c_LightDiffuse.rgb;
	#endif
	
	HIGHP vec3 specular_light = vec3(0.0);

	#if defined SPECULAR || defined SPECULARMAP
		float dinten = dotNL;
		
		#ifdef SHADOWMAP
			dinten *= shadow_Inten.x;
		#endif
		
		#ifdef NORMALMAP
			vec3 specular_tangent = normalize(oBinormal);
		
			specular_light = SpecularHair(nor, light_dir, view_dir, specular_power,
				specular_color, secondary_specular_color * mat_diffuse, 
				specular_tangent) * dinten * c_LightDiffuse.rgb;
		#endif
	#endif
	
	#ifdef POINTLIGHT
		MEDIUMP vec3 to_light = c_PointLightPosition.xyz - oViewToVertex.xyz;
		MEDIUMP float dist = length(to_light);
		to_light /= dist;
		MEDIUMP float point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition.w);
		MEDIUMP vec3 point_light_color = point_light_rang * c_PointLightDiffuse.xyz;
		
		diffuse_light.xyz += DiffuseHair(nor, to_light) * point_light_color;
		
		#ifdef NORMALMAP
			MEDIUMP float point_dotNL = max(dot(to_light, nor), 0.0);
			
			specular_light.xyz += SpecularHair(nor, to_light, view_dir, specular_power,
				specular_color, secondary_specular_color, 
				specular_tangent) * point_dotNL * point_light_color;
		#endif
		
		#ifdef POINTLIGHT2
			to_light = c_PointLightPosition2.xyz - oViewToVertex.xyz;
			dist = length(to_light);
			to_light /= dist;
			point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition2.w);
			point_light_color = point_light_rang * c_PointLightDiffuse2.xyz;
			
			diffuse_light.xyz += DiffuseHair(nor, to_light) * point_light_color;
			
			#ifdef NORMALMAP
				point_dotNL = max(dot(to_light, nor), 0.0);
				
				specular_light.xyz += SpecularHair(nor, to_light, view_dir, specular_power,
					specular_color, secondary_specular_color, 
					specular_tangent) * point_dotNL * point_light_color;
			#endif
		#endif
		
		#ifdef POINTLIGHT3
			to_light = c_PointLightPosition3.xyz - oViewToVertex.xyz;
			dist = length(to_light);
			to_light /= dist;
			point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition3.w);
			point_light_color = point_light_rang * c_PointLightDiffuse3.xyz;
			
			diffuse_light.xyz += DiffuseHair(nor, to_light) * point_light_color;
			
			#ifdef NORMALMAP
				point_dotNL = max(dot(to_light, nor), 0.0);
				
				specular_light.xyz += SpecularHair(nor, to_light, view_dir, specular_power,
					specular_color, secondary_specular_color, 
					specular_tangent) * point_dotNL * point_light_color;
			#endif
		#endif
		
		#ifdef POINTLIGHT4
			to_light = c_PointLightPosition4.xyz - oViewToVertex.xyz;
			dist = length(to_light);
			to_light /= dist;
			point_light_rang = max(0.0, 1.0 - dist / c_PointLightPosition4.w);
			point_light_color = point_light_rang * c_PointLightDiffuse4.xyz;
			
			diffuse_light.xyz += DiffuseHair(nor, to_light) * point_light_color;
			
			#ifdef NORMALMAP
				point_dotNL = max(dot(to_light, nor), 0.0);
				
				specular_light.xyz += SpecularHair(nor, to_light, view_dir, specular_power,
					specular_color, secondary_specular_color, 
					specular_tangent) * point_dotNL * point_light_color;
			#endif
		#endif
	#endif
	
	diffuse_light.rgb += ambient_light;
	diffuse_color.xyz *= diffuse_light;

	#if defined EMISSIVEMAP
		#ifdef EMISSIVE
			MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
			diffuse_color.xyz += crTexEmissive.xyz;
		#endif
	#else
		#ifdef EMISSIVE
			diffuse_color.xyz += c_MaterialEmissive.xyz;
		#endif
	#endif

	#if defined REFLECTION
		MEDIUMP float reflect_factor = c_ReflectScale.x;
		MEDIUMP vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		
		#if defined IOS
			vReflect.y = -vReflect.y;
		#endif
		
		MEDIUMP vec4 crTexReflect = texCubeBias(tex_ReflectionMap, vReflect, (1.0 - c_fSpecularPower) * c_ReflectScale.y);
		
		MEDIUMP vec3 reflect_color = Fresnel(nor, view_dir, specular_color, specular_power);
		
		specular_light += crTexReflect.xyz * reflect_color * reflect_factor;
	#endif

	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		diffuse_color.xyz += specular_light.xyz;
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

	#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
		diffuse_color.xyz = diffuse_color.xyz * oFog.w + oFog.xyz;
	#endif

	diffuse_color.xyz = diffuse_color.xyz + c_MaterialAmbientEx.xyz;

	#if defined(SHADOWMAP) && defined(PSSM_DEBUG)
		diffuse_color *= shadow_color_debug;
	#endif
	
	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(diffuse_color);
}