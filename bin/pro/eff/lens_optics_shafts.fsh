uniform MEDIUMP sampler2D tex_BrightnessMap;
uniform MEDIUMP sampler2D tex_IrisMap;

psin MEDIUMP vec2 uv;
psin MEDIUMP vec4 color;

void main(void)
{
	vec4 HDRParams = vec4(6.2, 0.5, 0.09, 1.0);

	vec3 map_color = tex2D(tex_IrisMap, vec2(uv.x, 1.0 - uv.y)).rgb;
	
	map_color *= color.rgb * color.a;
	
	OutputColor0(vec4((map_color.rgb * (HDRParams.x * map_color.rgb + HDRParams.y)) / (map_color.rgb * (HDRParams.x * map_color.rgb + 1.7) + HDRParams.z), 1.0));
}