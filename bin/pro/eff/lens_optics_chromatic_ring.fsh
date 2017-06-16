uniform MEDIUMP sampler2D tex_BrightnessMap;
uniform MEDIUMP sampler2D tex_SpectrumMap;

psin MEDIUMP vec2 uv;
psin MEDIUMP vec4 color;

void main(void)
{
	#define thou uv.x
	
	vec4 HDRParams = vec4(6.2, 0.5, 0.09, 1.0);
	
	float dist = (thou * 2.0 - 1.0);
	float alpha = exp(-4.0 * dist * dist) - exp(-4.0);
	vec3 map_color =  tex2D(tex_SpectrumMap, vec2(0.5, 1.0 - uv.x)).rgb;
	
	map_color *= color.rgb * color.a * alpha;
	
	OutputColor0(vec4((map_color.rgb * (HDRParams.x * map_color.rgb + HDRParams.y)) / (map_color.rgb * (HDRParams.x * map_color.rgb + 1.7) + HDRParams.z), 1.0));
}