
//#define FOG

varying mediump vec2 oUV;
varying mediump vec3 oPos;

#ifdef FOG
uniform mediump vec4 c_FogColor;
uniform mediump float c_FogHeight;
#endif

uniform sampler2D tex_sky;
 
void main()
{
   mediump vec4 oColor = texture2D( tex_sky , oUV ) ;

#ifdef FOG
   mediump float fogValue = normalize( oPos ).y / c_FogHeight;
   oColor = c_FogColor*(1.0 - fogValue) + oColor * fogValue;
#endif

   gl_FragColor = oColor ;
}