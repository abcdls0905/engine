uniform MEDIUMP sampler2D tex_BrightnessMap;
uniform MEDIUMP sampler2D tex_GhostMap;

psin MEDIUMP vec2 uv;
psin MEDIUMP vec4 color;

void main(void)
{
	vec4 HDRParams = vec4(6.2, 0.5, 0.09, 1.0);

	vec4 map_color = tex2D(tex_GhostMap, uv);
	
	map_color.rgb *= map_color.a * 32.0;
	
	map_color.rgb *= color.rgb * color.a * map_color.a;
	
	OutputColor0(vec4((map_color.rgb * (HDRParams.x * map_color.rgb + HDRParams.y)) / (map_color.rgb * (HDRParams.x * map_color.rgb + 1.7) + HDRParams.z), 1.0));
}