// model refraction shader

uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_Bump;

uniform MEDIUMP vec4 c_MaterialAmbient;
uniform MEDIUMP float c_fBumpScale;
uniform MEDIUMP vec4 c_AppendColor;

#if defined DIFFUSEMAP || defined NORMALMAP
	psin MEDIUMP vec2 oTex0;
#endif

#ifdef VERTEX_NORMAL
	psin MEDIUMP vec3 oNormal;
#endif

#ifdef NORMALMAP
	psin MEDIUMP vec3 oTangent;
	psin MEDIUMP vec3 oBinormal;
#endif

#ifdef VERTEX_COLOR
	psin MEDIUMP vec4 oDiffuse;
#endif

psin MEDIUMP vec2 oClipDistance;

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

void main(void)
{
	MEDIUMP vec3 nor;
	
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			nor = normal_calc(tex2D(tex_Bump, oTex0.xy).xyz, c_fBumpScale);	
			nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
			nor = normalize(nor);
		#else
			if (length(oNormal) > 0.0)
				nor = normalize(oNormal);
		#endif
	#endif

	vec4 crRefraction = vec4(0.5, 0.5, 0.5, 0.5);

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

	finalColor.xy += 0.5;
	finalColor.w = crRefraction.w / exp(oClipDistance.y * 0.05);
	
	OutputColor0(finalColor);
}
