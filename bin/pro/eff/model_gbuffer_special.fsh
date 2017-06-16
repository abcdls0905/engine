uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_Bump;
uniform MEDIUMP sampler2D tex_SpecularMap;
uniform MEDIUMP samplerCube tex_ReflectionMap;
uniform MEDIUMP sampler2D tex_LightMap;
uniform MEDIUMP sampler2D tex_EmissiveMap;
uniform MEDIUMP sampler2D tex_FilterMap;
uniform MEDIUMP sampler2D tex_KeepoutMap;

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

#if defined EMISSIVE_SPECIAL
	uniform float c_fEmissiveIntensity;
	uniform float c_fEmissiveAlphaMultiplier;
#endif

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

psin MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin MEDIUMP vec3 oNormal;
#endif

#if defined NORMALMAP
	psin MEDIUMP vec3 oTangent;
	psin MEDIUMP vec3 oBinormal;
#endif

psin MEDIUMP vec4 oDiffuse;
psin MEDIUMP vec4 oViewToVertex;

psin MEDIUMP float oClipDistance;

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

void main (void)
{
	MEDIUMP float bias = 0.0;

#ifdef KEEPOUT
	vec4 color = tex2DBiasGamma(tex_KeepoutMap, oTex0.xy, bias);

	#ifdef LUM_SCALE
		color.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(color);
#else
	MEDIUMP vec3 nor;
	MEDIUMP vec3 light_dir = c_vLightDir;
	MEDIUMP vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);

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
	
	#if defined EMISSIVEMAP
		#ifdef SKINEFFECT
			MEDIUMP vec4 crTexSubSurface = tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias);
			crTexSubSurface.xyz *= c_LightDiffuse.xyz;
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			// #ifdef SHADOWMAP
				// diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz * shadow_Inten.x;
			// #else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y * crTexSubSurface.xyz;
			// #endif
		#else
			#ifdef EMISSIVE
				MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
				diffuse_color.xyz += crTexEmissive.xyz;
			#endif
		#endif
	#else
		#ifdef SKINEFFECT
			MEDIUMP float sss_inten = (dot(light_dir, nor) + c_SubSurfaceParam.x) / (1.0 + c_SubSurfaceParam.x);
			// #ifdef SHADOWMAP
				// diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					// * c_LightDiffuse.xyz * max(shadow_Inten.x, 0.7);
			// #else
				diffuse_color.xyz += max(sss_inten, 0.0) * c_SubSurfaceParam.y
					* c_LightDiffuse.xyz;
			// #endif
		#else
			#ifdef EMISSIVE
				diffuse_color.xyz += c_MaterialEmissive.xyz;
			#endif
		#endif
	#endif
	
	//nor = (c_mtxViewInverse * vec4(nor, 0.0)).xyz;

	MEDIUMP float falloff_inten = -dot(normalize(oViewToVertex.xyz), nor);
	falloff_inten = 1.0 - max(falloff_inten, 0.0);
	falloff_inten = pow(max(falloff_inten, 0.000001), c_FallOffParam.w);
	
	#ifdef DIFFUSEMAP
		MEDIUMP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);
		diffuse_color.xyz += crTexDiffuse.xyz * falloff_inten * c_FallOffParam.xyz;
	#else
		diffuse_color.xyz += falloff_inten * c_FallOffParam.xyz;
	#endif
	
	#ifdef LUM_SCALE
		diffuse_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(diffuse_color);
#endif
}
