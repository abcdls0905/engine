// model refraction shader

precision mediump float;
precision mediump int;

uniform sampler2D tex_Diffuse;
uniform sampler2D tex_Bump;

uniform mediump vec4 c_MaterialAmbient;
uniform mediump float c_fBumpScale;
uniform mediump vec4 c_AppendColor;

#if defined DIFFUSEMAP || defined NORMALMAP
	varying mediump vec2 oTex0;
#endif

#ifdef VERTEX_NORMAL
	varying mediump vec3 oNormal;
#endif

#ifdef NORMALMAP
	varying mediump vec3 oTangent;
	varying mediump vec3 oBinormal;
#endif
#ifdef VERTEX_COLOR
	varying mediump vec4 oDiffuse;
#endif
	varying mediump vec2 oClipDistance;


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

void main(void)
{
	mediump vec3 nor;
	
	#ifdef VERTEX_NORMAL
		#ifdef NORMALMAP
			nor = normal_calc(texture2D(tex_Bump, oTex0.xy).xyz, c_fBumpScale);	
			nor = nor.x * oTangent + nor.y * oBinormal + nor.z * oNormal;
			nor = normalize(nor);
		#else
			nor = normalize(oNormal);
		#endif
	#endif

	vec4 crRefraction = vec4(0.5, 0.5, 0.5, 0.5);

	#ifdef  DIFFUSEMAP
		crRefraction = texture2D(tex_Diffuse, oTex0.xy);
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
	
	gl_FragColor = finalColor;
}
