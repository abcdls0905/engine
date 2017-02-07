//#define ADJUST 1
//#define GRADUAL 1
//#define ANGLE 1


varying mediump vec2 oUV;
varying mediump vec3 oPos;

#if defined(GRADUAL) 
varying mediump vec3 oPixelDir;
#endif
 
uniform sampler2D tex_color;
uniform sampler2D tex_depth;

uniform mediump mat4  c_matViewProjI;

// color just
uniform mediump vec3  c_ColorAdjustParam;
uniform mediump vec4  c_GradualBaseColor;

uniform mediump vec4  c_GradualColor;
uniform mediump vec3  c_GradualParam;
uniform mediump vec4  c_vecViewDirection;

uniform mediump vec3  c_AngleParam;
uniform mediump vec4  c_AngleColor;

uniform mediump vec2  c_TexScaleUV;

#if defined(ADJUST) 
mediump vec3 coloradjust(in mediump vec4 inputcolor,in mediump vec3 Param)
{
    mediump float ave = (inputcolor.x + inputcolor.y + inputcolor.z) / 3.0;
    mediump float crTempC = pow( max(ave, 0.000001 ), Param.y );
	return (ave*(1.0 - Param.z) + inputcolor.xyz*crTempC*Param.z)*Param.x;
} 
#endif 

#if defined(GRADUAL)
mediump vec3 gradualcolor(in mediump vec3 dir)
{
   mediump float weight = normalize(dir).y;
   weight -= c_GradualParam.x;
   weight /= c_GradualParam.y - c_GradualParam.x;
   weight = clamp(weight,0.0,1.0);
   weight = pow(max(weight, 0.000001), c_GradualParam.z);  
   return ((c_GradualColor.xyz - 0.5) * 2.0 * weight) * (1.0 - clamp(c_vecViewDirection.y,0.0,1.0) );
} 
#endif

#if defined(ANGLE)
mediump vec4 anglecolor(in mediump vec4 Color,in mediump vec2 texcoord)
{
   mediump float len = length(texcoord - vec2(0.5,0.5));
   mediump float weight = (len - c_AngleParam.y) / (c_AngleParam.x - c_AngleParam.y);
   weight = clamp(weight,0.0,1.0);
   weight = pow(max(weight, 0.000001), c_AngleParam.z);
   return Color*(1.0 - weight) + weight;
}
#endif
 
void main()
{
   mediump vec2 UV = oUV * c_TexScaleUV;
   
   mediump vec4 oColor = texture2D( tex_color , UV ) ;
 
#if defined(GRADUAL)	
 	oColor.xyz += gradualcolor(oPixelDir);
#endif   

#if defined(ADJUST)
 	oColor.xyz = coloradjust(oColor, c_ColorAdjustParam);
	oColor *= c_GradualBaseColor;  
#endif

#if defined(ANGLE)
   	oColor *= anglecolor(c_AngleColor, UV); 
#endif
   
   gl_FragColor = oColor ;
}