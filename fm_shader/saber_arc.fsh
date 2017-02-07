
varying mediump vec4 oColor;
varying mediump vec2 oTexcoord;

#if defined(REFRACTION)
varying mediump vec2 oTangent;
varying mediump vec2 oBinormal;
#endif

#if defined(FOGLINEAR)||defined(FOGEXP)||defined(REFRACTION)
varying mediump float oFogDepth;
#endif

uniform mediump vec4 c_FogColor;
uniform mediump vec4 c_FogParam;
uniform mediump vec4 c_FogExpParam;

uniform sampler2D tex_Diffuse;

mediump vec3 lerp(mediump vec3 u, mediump vec3 v, mediump float t)
{
	return u + (v-u)*t;
}

void main(void)
{
	gl_FragColor = oColor;
	
	mediump vec2 tc = oTexcoord;
	
#if defined(REFRACTION)
	mediump vec4 crTex = texture2D( tex_Diffuse , tc ) - vec4(0.5, 0.5, 0.5, 0.0);
	gl_FragColor = vec4((crTex.x * oBinormal + crTex.y * oTangent) * vec2(2.0, -2.0), 0, 1.0);
	
	gl_FragColor.xy += 0.5;
	gl_FragColor.w = oColor.w * crTex.w;
#else
  #if defined(DIFFUSEMAP)
    mediump vec4 crTex = texture2D( tex_Diffuse , tc );
	gl_FragColor *= crTex;
  #endif

  #if defined(FOGEXP)
	gl_FragColor.xyz = lerp(c_FogExpParam.xyz, gl_FragColor.xyz,
		1.0 / exp(oFogDepth * c_FogExpParam.x));
  #endif

  #if defined(FOGLINEAR)
	gl_FragColor.xyz = lerp( gl_FragColor.xyz, c_FogColor.xyz, 
		clamp((oFogDepth - c_FogParam.x) / c_FogParam.y, 0.0, 1.0) );
  #endif
#endif
}