uniform MEDIUMP sampler2D tex_LensOpticsMap;
uniform MEDIUMP sampler2D tex_BrightnessMap;
uniform MEDIUMP sampler2D tex_ColorMap;

psin MEDIUMP vec2 oPos;

void main(void)
{
	vec2 tex_uv = oPos;
	
	vec4 oColor = tex2D(tex_ColorMap, tex_uv);
	
	float brightness = tex2D(tex_BrightnessMap, vec2(0.5, 0.5)).x;
	vec3 lens = clamp(tex2D(tex_LensOpticsMap, tex_uv).rgb * brightness, 0.0, 1.0);
	
	oColor.rgb += lens * (vec3(1.0) - oColor.rgb);
	
	OutputColor0(oColor);
}