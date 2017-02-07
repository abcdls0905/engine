uniform sampler2D tex_Diffuse;
uniform sampler2D tex_Bump;
uniform sampler2D tex_SpecularMap;
uniform samplerCube tex_ReflectionMap;
uniform sampler2D tex_LightMap;
uniform sampler2D tex_EmissiveMap;
uniform sampler2D tex_FilterMap;
uniform sampler2D tex_KeepoutMap;
uniform sampler2D tex_Shadow;

varying mediump vec4 oTex0;
varying mediump vec3 oNormal;

#ifdef FILTERMAP
	varying mediump vec2 oTexFilter;
#endif

#if defined NORMALMAP
varying mediump vec3 oTangent;
varying mediump vec3 oBinormal;
#endif

varying mediump vec4 oDiffuse;
varying mediump vec4 oViewToVertex;

varying mediump float oClipDistance;

#ifdef SHADOWMAP
varying mediump vec4 oTexCoordShadow;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
varying mediump vec4 oFog;
#endif

// PS Constants
uniform mediump vec4 c_FallOffParam;
uniform mediump vec4 c_MaterialAmbient;
uniform mediump vec4 c_MaterialAmbientEX;
uniform mediump vec4 c_SubSurfaceParam;

uniform mediump float c_fBumpScale;

//scene
uniform mediump vec3 c_vLightDir;
uniform mediump vec4 c_LightAmbient;
uniform mediump vec4 c_LightDiffuse;

//µ„π‚‘¥
uniform mediump vec4 c_PointLightPosition;
uniform mediump vec4 c_PointLightDiffuse;

uniform mediump mat4 c_mtxViewInverse;
uniform mediump vec4 c_MaterialDiffuse;
uniform mediump vec4 c_MaterialEmissive;
uniform mediump vec4 c_MaterialSpecular;
uniform mediump vec4 c_AppendColor;

uniform mediump float c_fSpecularPower;
uniform mediump float c_fAlphaRefValue;

uniform mediump vec3 c_vLightMapColorScale;

uniform mediump vec4 c_FilterParam;

mediump vec4 lerp(mediump vec4 u, mediump vec4 v, mediump float t)
{
	return u + (v-u)*t;
}

mediump vec3 lerp(mediump vec3 u, mediump vec3 v, mediump float t)
{
	return u + (v-u)*t;
}

mediump vec3 normal_calc(mediump vec3 map_normal, mediump float bump_scale)
{
	mediump vec3 v = vec3(0.5, 0.5, 1.0);
	map_normal = lerp(v, map_normal, bump_scale);
	map_normal = (map_normal * 2.0) - 1.0;
	return map_normal;
}

void main (void)
{
#ifdef KEEPOUT
	gl_FragColor = texture2D(tex_KeepoutMap, oTex0.xy);
#else
#ifdef ONLYZPASS
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif
		//gl_FragColor = vec4(1,1,1,1);
		mediump vec4 diffuse_color = vec4(1,1,1,1);
		mediump vec3 specular_color = vec3(0, 0, 0);
	#ifdef NORMALMAP
		mediump vec3 nor = normal_calc(texture2D(tex_Bump, oTex0.xy).xyz, c_fBumpScale);	
		nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
		nor = normalize(nor);
	#else
		mediump vec3 nor = normalize(oNormal);
	#endif
	
	
		mediump vec3 light_dir = c_vLightDir;
	
	#ifdef DYNAMICLIGHTING
		#ifdef DIFFUSE
			#ifdef OPACITY
				diffuse_color.w *= oDiffuse.w;
			#endif
		#endif
	#else
		#ifdef LIGHTMAP
			diffuse_color.xyz *= texture2D(tex_LightMap, oTex0.zw).xyz * c_vLightMapColorScale.xyz;
		#endif
	#endif
		
	#ifdef DIFFUSEMAP
		#ifndef VOLUMETEXTURE
			mediump vec4 crTexDiffuse = texture2D(tex_Diffuse, oTex0.xy);
		#endif
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
 
		
	gl_FragColor = diffuse_color;
#else
	#ifdef CLIPPLANE
		if(oClipDistance < 0.0)
		{
			discard;
		}
	#endif


	mediump vec4 diffuse_color = vec4(1,1,1,1);
	mediump vec3 specular_color = vec3(0, 0, 0);
	#ifdef NORMALMAP
		mediump vec3 nor = normal_calc(texture2D(tex_Bump, oTex0.xy).xyz, c_fBumpScale);	
		nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
		nor = normalize(nor);
	#else
		mediump vec3 nor = normalize(oNormal);
	#endif
	
	#ifdef SHADOWMAP
    //mediump vec2 tex_uv = (oPos.xy + 0.5) * c_PixelSize;
	mediump vec2 tex_uv = (vec2(0.5, 0.5) * (oTexCoordShadow.xy / oTexCoordShadow.w)) + vec2(0.5, 0.5);
    mediump vec4 shadow_Inten = texture2D(tex_Shadow, tex_uv);
	#endif  
	
	mediump vec3 light_dir = c_vLightDir;
	
	#ifdef DYNAMICLIGHTING
		mediump vec3 ambient_light = c_LightAmbient.xyz;
		#ifdef SPHEREAMBIENT
		 	// hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
			#ifdef SKELETON
				ambient_light *= 1.0 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
			#else
				ambient_light *= 0.75 + dot(nor, c_mtxViewInverse[1].xyz) * 0.25;
			#endif
		#endif
		
		#ifdef SHADOWMAP
			diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz * shadow_Inten.x;
		#else 
			diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz;
		#endif
		
		diffuse_color.xyz *= c_MaterialDiffuse.xyz;
		
		#ifdef SHADOWMAP
			diffuse_color.xyz += c_MaterialAmbient.xyz * ambient_light * shadow_Inten.x;
		#else 
			diffuse_color.xyz += c_MaterialAmbient.xyz * ambient_light;
		#endif
		
		#ifdef POINTLIGHT
			mediump vec3 to_point_vector = c_PointLightPosition.xyz - oViewToVertex.xyz;
			mediump float to_point_distance = length(to_point_vector);
			mediump float LdotN = dot(to_point_vector / to_point_distance, nor);
			mediump float point_light_inten = max(LdotN, 0.0);
			mediump float point_light_rang = max(0.0, 1.0 - to_point_distance / c_PointLightPosition.w);
			
			diffuse_color.xyz += point_light_inten * point_light_rang
				* c_MaterialDiffuse.xyz * c_PointLightDiffuse.xyz;
		#endif
	
		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz * 2.0;
			#ifdef OPACITY
				diffuse_color.w *= oDiffuse.w;
			#endif
		#endif
	#else
		#ifdef LIGHTMAP		
			diffuse_color.xyz *= texture2D(tex_LightMap, oTex0.zw).xyz * c_vLightMapColorScale.xyz;
		#endif
		
		#ifdef DIFFUSE
			diffuse_color.xyz *= oDiffuse.xyz * 2.0;
		#endif

		#ifdef SHADOWMAP
			diffuse_color.xyz *= shadow_Inten.x;
		#endif
	
	#endif
		
	#ifdef DIFFUSEMAP
		#ifdef VOLUMETEXTURE
			//float4 crTexDiffuse = tex_Diffuse.Sample(sam_Diffuse, float3(input.iTex01.xy, c_fTime));
		#else
			mediump vec4 crTexDiffuse = texture2D(tex_Diffuse, oTex0.xy);
			//float4 crTexDiffuse = tex_Diffuse.Sample(sam_Diffuse, input.iTex01.xy);
		#endif
	
		#ifdef SATURATION
			mediump float lum = dot(crTexDiffuse.xyz, vec3(0.299, 0.587, 0.114));
			crTexDiffuse.xyz = max((crTexDiffuse.xyz - lum) * c_MaterialEmissive.w + lum, 0.0);
		#endif
	
		#ifdef FILTERMAP
			mediump vec4 crBaseColor = vec4(c_FilterParam.yyy, c_FilterParam.w);
			mediump vec4 crTexFilter = texture2D(tex_FilterMap, oTexFilter) * vec4(c_FilterParam.xxx, c_FilterParam.z);
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
		mediump vec4 specular_tex = texture2D(tex_SpecularMap, oTex0.xy);
		mediump vec3 specular_material = c_MaterialSpecular.xyz * specular_tex.xyz;
		mediump float gloss = max(c_fSpecularPower, 0.000001) * specular_tex.w;
	#endif
	
	#if defined SPECULAR && !defined SPECULARMAP
		mediump vec3 specular_material = c_MaterialSpecular.xyz;
		mediump float gloss = max(c_fSpecularPower, 0.000001);
	#endif
	
	#if defined REFLECTION	
		mediump vec3 reflect_inten = vec3(c_MaterialDiffuse.w, c_MaterialDiffuse.w, c_MaterialDiffuse.w);
		#if defined SPECULARMAP
			reflect_inten *= specular_material.xyz;
		#endif
	#endif
	
	#if defined SPECULAR || defined SPECULARMAP
		mediump float specular_inten = pow(max(dot(reflect(normalize(oViewToVertex.xyz), nor), light_dir), 0.000001), gloss);
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
			mediump vec4 crTexSubSurface = texture2D(tex_EmissiveMap, oTex0.xy);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			mediump float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			#else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			#endif
		#else
			#ifdef EMISSIVE
				mediump vec3 crTexEmissive = c_MaterialEmissive.xyz * texture2D(tex_EmissiveMap, oTex0.xy).xyz;
				diffuse_color.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			mediump float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			#ifdef SHADOWMAP
				diffuse_color.xyz += max(sss_inten, 0.0F) * c_SubSurfaceParam.y
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
		mediump float reflect_factor = c_MaterialDiffuse.w;
		mediump vec3 vReflect = reflect(normalize(oViewToVertex.xyz), nor);
		mediump vec4 crTexReflect = textureCube (tex_ReflectionMap, vReflect);
		specular_color += crTexReflect.xyz * reflect_inten.xyz * reflect_factor;
	#endif
	
	#if defined DIFFUSEMAP && defined SPECULAR && !defined SPECULARMAP
		specular_color.xyz *= crTexDiffuse.w;
	#endif
	
	#if defined SPECULAR || defined SPECULARMAP || defined REFLECTION
		diffuse_color.xyz += specular_color.xyz;
	#endif
	
	#if defined FALLOFF
		mediump float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);		
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
		gl_FragColor = diffuse_color;
#endif
#endif

}

