uniform MEDIUMP sampler2D tex_ColorMapLighting;
uniform MEDIUMP sampler2D tex_ColorMap;
uniform MEDIUMP vec4 c_SunColor;
uniform MEDIUMP float c_fRaysAmount;
uniform MEDIUMP vec4 c_PixelSize;

psin MEDIUMP vec2 oPos;

void main(void)
{
	vec4 oColor;
	
	vec2 tex_uv = oPos;
	
	vec4 sceneColor = tex2D(tex_ColorMap, tex_uv);
	
	vec4 light = tex2D(tex_ColorMapLighting, tex_uv);
	
	oColor.rgb = sceneColor.rgb + light.rgb * c_fRaysAmount * c_SunColor.rgb * (vec3(1.0, 1.0, 1.0) - sceneColor.rgb);
	
	oColor.a = 1.0;
	
	OutputColor0(oColor);
}