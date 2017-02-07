precision mediump float;
precision mediump int;

varying vec2 oUV;

uniform sampler2D texColor;
uniform sampler2D texReraction;
uniform vec4 c_vRefractionInten;

void main()
{
//#ifdef FLOATRT
	//vec2 normal = texture2D(texReraction, oUV.xy).xy;
//#else
	vec2 normal = texture2D(texReraction, oUV.xy).xy - 127.0 / 255.0;
//#endif

	vec2 tcBkg = oUV.xy + abs(normal) * c_vRefractionInten.x;

	gl_FragColor = texture2D(texColor, tcBkg.xy);
}
