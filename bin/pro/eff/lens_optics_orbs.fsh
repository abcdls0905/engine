uniform MEDIUMP sampler2D tex_orbMap;
uniform MEDIUMP float c_LensTexStrength;

psin MEDIUMP vec2 uv;
psin MEDIUMP vec4 color;

void main(void)
{
	vec4 HDRParams = vec4(6.2, 0.5, 0.09, 1.0);

	vec4 map_color = tex2D(tex_orbMap, uv);
	
	map_color *= color;
	
	map_color.rgb *= map_color.a * c_LensTexStrength;
	
	OutputColor0(vec4((map_color.rgb * (HDRParams.x * map_color.rgb + HDRParams.y)) / (map_color.rgb * (HDRParams.x * map_color.rgb + 1.7) + HDRParams.z), 1.0));
}