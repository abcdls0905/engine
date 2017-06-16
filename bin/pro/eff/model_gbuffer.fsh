uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_Bump;
uniform MEDIUMP sampler2D tex_SpecularMap;
uniform MEDIUMP samplerCube tex_ReflectionMap;
uniform MEDIUMP sampler2D tex_LightMap;
uniform MEDIUMP sampler2D tex_EmissiveMap;
uniform MEDIUMP sampler2D tex_FilterMap;
uniform MEDIUMP sampler2D tex_KeepoutMap;

#ifdef USE_PCF
	uniform HIGHP sampler2DArrayShadow tex_Shadow1;
#else
	uniform HIGHP sampler2DArray tex_Shadow1;
#endif

uniform MEDIUMP sampler2D tex_LightMap1;
uniform MEDIUMP sampler2D tex_LightMap2;
uniform MEDIUMP sampler2D tex_AmbientOcclusion;

// PS Constants
uniform MEDIUMP vec4 c_FallOffParam;
uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP vec4 c_MaterialAmbientEx;
uniform MEDIUMP vec4 c_SubSurfaceParam;

uniform MEDIUMP float c_fBumpScale;

//scene
uniform MEDIUMP vec3 c_vLightDir;
uniform MEDIUMP vec4 c_LightAmbient;
uniform MEDIUMP vec4 c_LightDiffuse;

//???
uniform MEDIUMP vec4 c_PointLightPosition;
uniform MEDIUMP vec4 c_PointLightDiffuse;

uniform MEDIUMP mat4 c_mtxViewInverse;
uniform MEDIUMP vec4 c_MaterialDiffuse;
uniform MEDIUMP vec4 c_MaterialEmissive;
uniform MEDIUMP vec3 c_MaterialSpecular;
uniform MEDIUMP vec4 c_AppendColor;

uniform MEDIUMP float c_fSpecularPower;
uniform MEDIUMP float c_fAlphaRefValue;

uniform MEDIUMP vec3 c_vLightMapColorScale;

uniform MEDIUMP vec4 c_FilterParam;

uniform MEDIUMP vec4 c_HDRScaleInfo[6];

#ifdef SHADOWMAP
	uniform MEDIUMP float c_ShadowParam;
	uniform MEDIUMP float c_fShadowDistance;
	uniform MEDIUMP vec4 c_farDistance;
	uniform MEDIUMP int c_ShadowMapCount;
#endif

psin MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin MEDIUMP vec3 oNormal;

	#ifdef LIGHTMAP
		psin MEDIUMP vec4 oTex1;
		psin MEDIUMP vec4 oTex2;
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

#ifdef SHADOWMAP
	uniform MEDIUMP vec2 c_PixelSize;
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

#ifdef ES3
	layout(location = 1) out HIGHP vec4 DiffuseColor;
	layout(location = 2) out HIGHP vec4 SpecularColor;
	#ifdef LIGHTMAP
		layout(location = 3) out HIGHP vec4 LightMapColor;
	#endif
#endif//ES3

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

void main (void)
{
	MEDIUMP vec3 nor;
	MEDIUMP vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	MEDIUMP vec3 specular_color = vec3(0.0, 0.0, 0.0);
	//MEDIUMP vec3 light_dir = c_vLightDir;
	float bias = -1.0;

	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif

	//MEDIUMP vec3 diffuse_light = vec3(0.0, 0.0, 0.0);
	//MEDIUMP vec3 view_dir = -normalize(oViewToVertex.xyz);
	#ifdef SPECULAR
		specular_color = c_MaterialSpecular.rgb;
	#endif
	
	MEDIUMP float specular_power = c_fSpecularPower;
	
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			MEDIUMP vec3 nor_calc = normal_calc(tex2DBias(tex_Bump, oTex0.xy, bias).xyz, c_fBumpScale);
			nor = nor_calc.x * oTangent + nor_calc.y * oBinormal + nor_calc.z * oNormal;
			nor = normalize(nor);
		#else
			nor = normalize(oNormal);
		#endif
	#else
		nor = vec3(0.0, 0.0, 0.0);
	#endif

	//MEDIUMP float dotNL = clamp(dot(nor, light_dir), 0.0, 1.0);
	
	#ifdef SPECULARMAP
		MEDIUMP vec4 spec_tex = tex2DBias(tex_SpecularMap, oTex0.xy, bias);
		specular_color *= spec_tex.rgb;
		specular_power *= spec_tex.a;
	#endif
	
	#ifndef LIGHTMAP
		diffuse_color.xyz *= c_MaterialDiffuse.xyz;
	#endif
	
	#ifdef DIFFUSE
		diffuse_color.xyz *= oDiffuse.xyz;
		diffuse_color.w *= oDiffuse.w;
	#endif
			
	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = tex2DBias(tex_Diffuse, oTex0.xy, bias);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2DBias(tex_DiffuseAlpha, oTex0.xy, bias).r;
		#endif

		#ifdef SATURATION
			MEDIUMP float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
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

	//#if defined FALLOFF
	//	MEDIUMP float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);
	//	falloff_inten = 1.0 - max(falloff_inten, 0.0);
	//	falloff_inten = pow(max(falloff_inten, 0.000001), c_FallOffParam.w);
	//
	//	#ifdef DIFFUSEMAP
	//		diffuse_color.xyz += crTexDiffuse.xyz * falloff_inten * c_FallOffParam.xyz;
	//	#else
	//		diffuse_color.xyz += falloff_inten * c_FallOffParam.xyz;
	//	#endif
	//#endif

	//diffuse_color.xyz = diffuse_color.xyz + c_MaterialAmbientEx.xyz;

	#ifdef BWCOLOR
		MEDIUMP float bwdiffuse = (diffuse_color.x + diffuse_color.y + diffuse_color.z) * 0.3;
		diffuse_color.xyz = vec3(bwdiffuse,bwdiffuse,bwdiffuse);
	#endif
	
	vec3 encode_normal = EncodeNormal(nor);
	
	OutputColor0(
		vec4(encode_normal.x, encode_normal.y, 0.0, encode_normal.z));

	#ifdef GAMMA_POWER
		DiffuseColor.rgb = pow(diffuse_color.rgb, vec3(1.0/2.2, 1.0/2.2, 1.0/2.2));	
	#else
		DiffuseColor.rgb = diffuse_color.rgb;
	#endif
	DiffuseColor.a = diffuse_color.a;
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
			MEDIUMP vec4 shadow_Inten = final_shadow_color(shadow_depth, oViewPos.z);
		#endif
		
		MEDIUMP vec3 lightmap_color = tex2DBiasGamma(tex_LightMap, oTex0.zw, bias).xyz;

		#ifdef VERTEX_NORMAL
			#ifdef NORMALMAP
				#ifdef SPHEREAMBIENT // RNM
					MEDIUMP vec3 lightmap_color1 = tex2DBiasGamma(tex_LightMap1, oTex1.xy, bias).xyz;
					MEDIUMP vec3 lightmap_color2 = tex2DBiasGamma(tex_LightMap2, oTex1.zw, bias).xyz;

					lightmap_color.xyz = RNM_calc(nor_calc,
						lightmap_color.xyz, lightmap_color1.xyz, lightmap_color2.xyz,
						c_HDRScaleInfo[0].xyz,c_HDRScaleInfo[1].xyz,
						c_HDRScaleInfo[2].xyz,c_HDRScaleInfo[3].xyz,
						c_HDRScaleInfo[4].xyz,c_HDRScaleInfo[5].xyz);

					//MEDIUMP vec3 ao_color = tex2DBiasGamma(tex_AmbientOcclusion, oTex2.xy, bias).xyz;
					//lightmap_color.xyz += ao_color;
					
					#ifdef LIGHTMAP_AO
						MEDIUMP vec3 ao_color = tex2DBiasGamma(tex_AmbientOcclusion, oTex2.xy, bias).xyz;
						lightmap_color.xyz += ao_color;
					#else
						lightmap_color.xyz += c_LightAmbient.xyz*0.5;
					#endif
					
				#else
					lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
				#endif
			#else
				lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
			#endif
		#else
			lightmap_color.xyz = lightmap_color.xyz * c_vLightMapColorScale.xyz;
		#endif
		
		#ifdef SHADOWMAP
			LightMapColor.rgb = lightmap_color * shadow_Inten.x;
		#else
			LightMapColor.rgb = lightmap_color;
		#endif
		
		LightMapColor.a = 1.0;
	#endif
}