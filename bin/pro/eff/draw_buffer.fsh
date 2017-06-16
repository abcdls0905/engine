uniform MEDIUMP sampler2D tex_BaseMap;

psin vec2 oUV;

void main(void)
{
	vec2 iUV = oUV;

#if defined FLAG_PORTRAIT_UPSIDE
	#if defined IOS
		iUV.x = clamp(1.0 - oUV.x, 0.0, 1.0);
	#endif
#endif 

#if defined FLAG_PORTRAIT_UPSIDE_DOWN
	iUV.y = clamp(1.0 - oUV.y, 0.0, 1.0);
#endif

#if defined FLAG_PORTRAIT_LEFT
	#if defined IOS
		iUV.x = clamp(1.0 - oUV.x, 0.0, 1.0);
	#endif
#endif 

#if defined FLAG_PORTRAIT_RIGHT

#endif

	vec4 oColor = tex2D(tex_BaseMap, iUV);

	OutputColor0(oColor);
}