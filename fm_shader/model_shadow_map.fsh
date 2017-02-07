uniform mediump sampler2D tex_Diffuse;

#ifdef ALPHATEST
varying mediump vec2 oTex0;
#endif

//varying mediump float oDepth;

mediump vec4 EncodeFloatRGB(mediump float v) 
{
	mediump vec4 enc = vec4(1.0, 255.0, 65025.0,16581375.0) * v;
	enc = fract(enc);
	enc.xyz -= enc.yzw * vec3(1.0 / 255.0, 1.0/255.0, 1.0/255.0);
    return enc;
}

uniform mediump float c_fAlphaRefValue;

void main (void)
{
#ifdef ALPHATEST
	mediump float alpha = texture2D(tex_Diffuse, oTex0.xy).w;

	if(alpha - c_fAlphaRefValue <0.0)
	{
		discard;
	}
#endif

	//gl_FragColor = EncodeFloatRGB(oDepth / 1024.0);
	gl_FragColor = vec4(0.5, 1.0, 1.0, 1.0);
}

