
varying mediump vec4 oColor;
varying mediump vec2 oTexcoord;

#if defined(FOGLINEAR)||defined(FOGEXP)
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

//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}