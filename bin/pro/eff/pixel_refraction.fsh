// pixel refraction shader
uniform MEDIUMP float c_RefractionInten;

psin MEDIUMP vec2 oTex;

uniform MEDIUMP sampler2D tex_Background;
uniform MEDIUMP sampler2D tex_RefractionMap;

void main(void)
{
	HIGHP vec2 normal = tex2D(tex_RefractionMap, oTex.xy).xy;
#ifdef RGB565RT
	normal.x -= 123.0 / 255.0;
	normal.y -= 125.0 / 255.0;
#else
	normal.xy -= 127.0 / 255.0;
#endif
	
	MEDIUMP vec2 tcBkg = oTex + normal * c_RefractionInten;
	OutputColor0(tex2D(tex_Background, tcBkg));
}
