layout (location = 0) out MEDIUMP vec4 outColor;

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
uniform HIGHP sampler2DShadow tex_Shadow1;
#else
uniform MEDIUMP sampler2D tex_Shadow1;
uniform MEDIUMP sampler2D tex_Shadow2;
uniform MEDIUMP sampler2D tex_Shadow3;
uniform MEDIUMP sampler2D tex_Shadow4;
#endif
uniform MEDIUMP sampler2D tex_WarFog;

in MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
in MEDIUMP vec3 oNormal;
#endif

#ifdef FILTERMAP
	in MEDIUMP vec2 oTexFilter;
#endif

#if defined NORMALMAP
in MEDIUMP vec3 oTangent;
in MEDIUMP vec3 oBinormal;
#endif

in MEDIUMP vec4 oDiffuse;
in MEDIUMP vec4 oViewToVertex;

in MEDIUMP float oClipDistance;

#ifdef SHADOWMAP
in MEDIUMP vec4 oViewPos;
uniform MEDIUMP float c_ShadowParam;
uniform MEDIUMP vec4 c_farDistance;
uniform MEDIUMP int c_ShadowMapCount;
in MEDIUMP vec4 v_shadowPos1;
in MEDIUMP vec4 v_shadowPos2;
in MEDIUMP vec4 v_shadowPos3;
in MEDIUMP vec4 v_shadowPos4;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
in MEDIUMP vec4 oFog;
#endif

#ifdef CAMERALIGHT
in MEDIUMP	float oCameraLightInten;
uniform MEDIUMP vec4 c_CameraLightDiffuse;
#endif

// PS Constants
uniform MEDIUMP vec4 c_FallOffParam;
uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP vec4 c_MaterialAmbientEX;
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
uniform MEDIUMP vec4 c_MaterialSpecular;
uniform MEDIUMP vec4 c_AppendColor;

uniform MEDIUMP float c_fSpecularPower;
uniform MEDIUMP float c_fAlphaRefValue;

uniform MEDIUMP vec3 c_vLightMapColorScale;

uniform MEDIUMP vec4 c_FilterParam;

#if defined WARFOG
	in MEDIUMP vec2 oTexWarFog;
	uniform MEDIUMP vec4 c_vWarFogRange;
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
	float shadow_lookup ( float x, float y )
	{
	   float pixelSize = 0.0009765625;
	   vec4 offset = vec4 ( x * pixelSize,
	                        y * pixelSize * 2.0,
	                        0, 0
	                        );
	   //MEDIUMP vec4 tex_uv = oTexCoordShadow * 0.5 + 0.5;
	   return textureProj ( tex_Shadow1, v_shadowPos1 + offset);
	}
	#endif
MEDIUMP vec4 final_shadow_color(in MEDIUMP float shadow)
{
	MEDIUMP float v  = 1.0 - clamp((1.0 - shadow) * c_ShadowParam, 0.0, 1.0);

	MEDIUMP vec4 fShadow = vec4(v, v, v, v);
	return fShadow;
}
#endif

void main (void)
{
	MEDIUMP vec3 nor;
#ifdef KEEPOUT
	outColor = texture(tex_KeepoutMap, oTex0.xy);
#else
#ifdef ONLYZPASS
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif
		//outColor = vec4(1,1,1,1);
		MEDIUMP vec4 diffuse_color = vec4(1,1,1,1);
		MEDIUMP vec3 specular_color = vec3(0, 0, 0);
		#ifdef VERTEX_NORMAL
			#ifdef NORMALMAP
				nor = normal_calc(texture(tex_Bump, oTex0.xy).xyz, c_fBumpScale);
				nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
				nor = normalize(nor);
			#else
				nor = normalize(oNormal);
			#endif
		#endif


		MEDIUMP vec3 light_dir = c_vLightDir;

	#ifdef DYNAMICLIGHTING
		#ifdef DIFFUSE
			diffuse_color.w *= oDiffuse.w;
		#endif
	#else
		#ifdef LIGHTMAP
			diffuse_color.xyz *= texture(tex_LightMap, oTex0.zw).xyz * c_vLightMapColorScale.xyz;
		#endif
	#endif

	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = texture(tex_Diffuse, oTex0.xy);
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


	outColor = diffuse_color;
#else
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif


	MEDIUMP vec4 diffuse_color = vec4(1,1,1,1);
	MEDIUMP vec3 specular_color = vec3(0, 0, 0);
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			nor = normal_calc(texture(tex_Bump, oTex0.xy).xyz, c_fBumpScale);
			nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
			nor = normalize(nor);
		#else
			nor = normalize(oNormal);
		#endif
	#endif

	#ifdef SHADOWMAP
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
			MEDIUMP vec4 shadow_Inten = vec4(sum);
			shadow_Inten = final_shadow_color(shadow_Inten.x);
		#else

            HIGHP float shadow_depth = 1.0;

            if(c_ShadowMapCount == 1)
            {
                shadow_depth = texture(tex_Shadow1, v_shadowPos1.xy).x;

                if(shadow_depth > 0.0 && shadow_depth < 1.0)
                    shadow_depth = 0.5;
            }
            else
            {
                if(oViewPos.z < c_farDistance.x)
                {
                    shadow_depth = texture(tex_Shadow1, v_shadowPos1.xy).x;
                    if(shadow_depth < v_shadowPos1.z)
                        shadow_depth = 0.5;
                    else
                        shadow_depth = 1.0;
                }
                else if(oViewPos.z < c_farDistance.y)
                {
                    shadow_depth = texture(tex_Shadow2, v_shadowPos2.xy).x;
                    if(shadow_depth < v_shadowPos2.z)
                        shadow_depth = 0.5;
                    else
                        shadow_depth = 1.0;
                }
                else if(oViewPos.z < c_farDistance.z)
                {
                    shadow_depth = texture(tex_Shadow3, v_shadowPos3.xy).x;
                    if(shadow_depth < v_shadowPos3.z)
                        shadow_depth = 0.5;
                    else
                        shadow_depth = 1.0;
                }
                else if(oViewPos.z < c_farDistance.w)
                {
                    shadow_depth = texture(tex_Shadow4, v_shadowPos4.xy).x;
                    if(shadow_depth < v_shadowPos4.z)
                        shadow_depth = 0.5;
                    else
                        shadow_depth = 1.0;
                }
            }
            MEDIUMP vec4 shadow_Inten = final_shadow_color(shadow_depth);


		#endif
	#endif

	MEDIUMP vec3 light_dir = c_vLightDir;

	#ifdef DYNAMICLIGHTING
		MEDIUMP vec3 ambient_light = c_LightAmbient.xyz;
		#ifdef SPHEREAMBIENT
		 	// hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
			#ifdef SKELETON
				ambient_light *= 1.0 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
			#else
				ambient_light *= 0.75 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
			#endif
		#endif

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
			MEDIUMP vec3 to_point_vector = c_PointLightPosition.xyz - oViewToVertex.xyz;
			MEDIUMP float to_point_distance = length(to_point_vector);
			MEDIUMP float LdotN = dot(to_point_vector / to_point_distance, nor);
			MEDIUMP float point_light_inten = max(LdotN, 0.0);
			MEDIUMP float point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition.w);

			diffuse_color.xyz += point_light_inten * point_light_rang
				* c_MaterialDiffuse.xyz * c_PointLightDiffuse.xyz;
		#endif

		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
			diffuse_color.w *= oDiffuse.w;
		#endif
	#else
		#ifdef LIGHTMAP
			diffuse_color.xyz *= texture(tex_LightMap, oTex0.zw).xyz * c_vLightMapColorScale.xyz;
		#endif

		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz;
		#endif

		#ifdef SHADOWMAP
			diffuse_color.xyz *= shadow_Inten.x;
		#endif

	#endif

	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = texture(tex_Diffuse, oTex0.xy);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = texture(tex_DiffuseAlpha, oTex0.xy).r;
		#endif

		#ifdef SATURATION
			MEDIUMP float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
		#endif

		#ifdef FILTERMAP
			MEDIUMP vec4 crBaseColor = vec4(c_FilterParam.yyy, c_FilterParam.w);
			MEDIUMP vec4 crTexFilter = texture(tex_FilterMap, oTexFilter) * vec4(c_FilterParam.xxx, c_FilterParam.z);
			crTexFilter += crBaseColor;
			crTexDiffuse *= crTexFilter;
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

	#ifdef SPECULARMAP
		MEDIUMP vec4 specular_tex = texture(tex_SpecularMap, oTex0.xy);
		MEDIUMP vec3 specular_material = c_MaterialSpecular.xyz * specular_tex.xyz;
		MEDIUMP float gloss = max(c_fSpecularPower, 0.000001) * specular_tex.w;
	#endif

	#if defined SPECULAR && !defined SPECULARMAP
		MEDIUMP vec3 specular_material = c_MaterialSpecular.xyz;
		MEDIUMP float gloss = max(c_fSpecularPower, 0.000001);
	#endif

	#if defined REFLECTION
		MEDIUMP vec3 reflect_inten = vec3(c_MaterialDiffuse.w, c_MaterialDiffuse.w, c_MaterialDiffuse.w);
		#if defined SPECULARMAP
			reflect_inten *= specular_material.xyz;
		#endif
	#endif

	#if defined SPECULAR || defined SPECULARMAP
		MEDIUMP float specular_inten = pow(max(dot(reflect(normalize(oViewToVertex.xyz), nor), light_dir), 0.000001), gloss);
		#ifdef SHADOWMAP
			specular_inten *= shadow_Inten.x;
		#endif
	#endif

	#if defined SPECULAR || defined SPECULARMAP
		specular_color = specular_inten * specular_material.xyz * c_LightDiffuse.xyz;
		//#if PRELIGHT && SHADOWMAP
			//specular_color += specular_material.xyz * prelight.xyz * prelight.w;
		//#endif
	#endif

	#if defined EMISSIVEMAP
		#ifdef SKINEFFECT
			MEDIUMP vec4 crTexSubSurface = texture(tex_EmissiveMap, oTex0.xy);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			#else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * texture(tex_EmissiveMap, oTex0.xy).xyz;
				diffuse_color.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz * max(shadow_Inten.x, 0.7F);
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
		MEDIUMP float reflect_factor = c_MaterialDiffuse.w;
		MEDIUMP vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		MEDIUMP vec4 crTexReflect = texCube(tex_ReflectionMap, vReflect);
		specular_color += crTexReflect.xyz * reflect_inten.xyz * reflect_factor;
	#endif

	#if defined DIFFUSEMAP && defined SPECULAR && !defined SPECULARMAP
		specular_color.xyz *= crTexDiffuse.w;
	#endif

	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		diffuse_color.xyz += specular_color.xyz;
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

		diffuse_color.xyz = diffuse_color.xyz + c_MaterialAmbientEX.xyz;

	#ifdef WARFOG
		MEDIUMP vec2 vTexCoord = oTexWarFog.xy;
		MEDIUMP vec4 crWarFog = texture(tex_WarFog, vTexCoord);

		diffuse_color.xyz = diffuse_color.xyz * mix(c_vWarFogRange.x, c_vWarFogRange.y, crWarFog.w);
	#endif

	#ifdef BWCOLOR
		MEDIUMP float bwdiffuse = (diffuse_color.x + diffuse_color.y + diffuse_color.z) * 0.3;
		diffuse_color.xyz = vec3(bwdiffuse,bwdiffuse,bwdiffuse);
	#endif

		outColor = diffuse_color;
#endif
#endif
}
