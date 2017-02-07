// pixel refraction shader

uniform mediump float c_RefractionInten;

varying mediump vec2 oTex;

uniform sampler2D tex_Background;
uniform sampler2D tex_RefractionMap;

void main(void)
{
	mediump vec2 normal = texture2D(tex_RefractionMap, oTex.xy).xy;
#ifdef RGB565RT
	normal.x -= 123.0 / 255.0;
	normal.y -= 125.0 / 255.0;
#else
	normal.xy -= 127.0 / 255.0;
#endif
	
	mediump vec2 tcBkg = oTex + normal * c_RefractionInten;
	gl_FragColor = texture2D(tex_Background, tcBkg);
}
