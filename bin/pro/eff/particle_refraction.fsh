psin HIGHP vec4 oScreenPos;
psin MEDIUMP vec2 oUV;
psin MEDIUMP vec4 oColor;

uniform MEDIUMP sampler2D tex_BaseMap;
uniform MEDIUMP sampler2D tex_BaseAlphaMap;

void main()
{
	MEDIUMP vec4 Color = vec4(0.5, 0.5, 0.5, 0.5);

	Color *= tex2DGamma(tex_BaseMap, oUV);

	Color.xyz -= 0.5;
	
#ifdef PAR_BASE_MAP_ALPHA
	Color.a = oColor.a * tex2D(tex_BaseAlphaMap, oUV).r;
#endif

	Color.xy += 0.5;
	//Color.w *= 0.5;

	OutputColor0(Color);
}
