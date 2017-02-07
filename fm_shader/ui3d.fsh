uniform sampler2D tex_BaseMap;
varying highp vec2 oTexCoord;
//uniform highp float c_alpha;

void main()
{
#ifdef USE_TEX
	gl_FragColor = texture2D(tex_BaseMap,  vec2(oTexCoord.x, oTexCoord.y));
#else
	gl_FragColor = vec4(1, 0, 0, 1);
#endif
	//gl_FragColor = vec4(1, 1, 1, 1);
	//gl_FragColor.a = gl_FragColor.a * c_alpha;
}