psin vec2 oUV;

uniform MEDIUMP sampler2D texColor;
uniform MEDIUMP sampler2D texReraction;
uniform vec4 c_vRefractionInten;

void main()
{
//#ifdef FLOATRT
	//vec2 normal = tex2D(texReraction, oUV.xy).xy;
//#else
	vec2 normal = tex2D(texReraction, oUV.xy).xy - 127.0 / 255.0;
//#endif

	vec2 tcBkg = oUV.xy + abs(normal) * c_vRefractionInten.x;

	OutputColor0(tex2D(texColor, tcBkg.xy));
}
