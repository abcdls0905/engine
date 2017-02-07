//#define HIGHLIGHT 1
//#define FOGEXP 1
//#define FOGLINEAR 1

varying mediump vec4 oScreenPos;
varying mediump vec2 oUV;
varying mediump vec4 oColor;


uniform mediump vec4 c_ParticleColor;
//#ifdef FOGEXP
uniform mediump vec4 c_FogExpParam;
//#endif

//#ifdef FOGLINEAR
uniform mediump vec4 c_FogParam;
uniform mediump vec4 c_FogColor;
//#endif

uniform sampler2D tex_BaseMap;

void main()
{
	mediump vec4 Color = oColor;

	Color *= texture2D( tex_BaseMap , oUV ) ;
	//gl_FragColor = vec4(fract(oUV).xy,0.0,0.5);
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
	//return;

//指数雾效果
#ifdef FOGEXP
	mediump float lerpvalue = 1.0 / exp(oScreenPos.w * c_FogExpParam.a);
	Color.rgb = c_FogExpParam.rgb*(1.0 - lerpvalue) + Color.rgb*lerpvalue;
#endif

//线性雾效果
#ifdef FOGLINEAR 
	mediump float fogliner_lerpvalue = clamp( (oScreenPos.w - c_FogParam.x) * c_FogParam.y,0.0,1.0);
	Color.rgb = Color.rgb * (1.0 - fogliner_lerpvalue)  + c_FogColor.rgb * fogliner_lerpvalue;
#endif

	Color.rgb *= Color.a;

#ifdef HIGHLIGHT
	Color.a = 0.0;
#endif   

	gl_FragColor = Color ;
}