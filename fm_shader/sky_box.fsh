
//#define CUBEMAP
//#define FOG

varying mediump vec2 oUV;
varying mediump vec3 oPos;

#ifdef FOG
uniform mediump vec4 c_FogColor;
uniform mediump float c_FogHeight;
#endif

#ifdef CUBEMAP
uniform samplerCube tex_sky;
#else
uniform sampler2D tex_sky;
#endif

void main()
{
#ifdef CUBEMAP
   mediump vec4 oColor = textureCube( tex_sky , -normalize( oPos ) );
#else
   mediump vec4 oColor = texture2D( tex_sky , oUV ) ;
#endif

#ifdef FOG
   mediump float fogValue = clamp(normalize( oPos ).y / c_FogHeight,0.0,1.0);
   oColor = c_FogColor*(1.0 - fogValue) + oColor * fogValue;
#endif

   gl_FragColor = oColor ;
}