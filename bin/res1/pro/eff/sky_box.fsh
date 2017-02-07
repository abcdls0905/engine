precision mediump float;
precision mediump int;

#ifdef ES3

in mediump vec2 oUV;
in mediump vec3 oPos;

#else

varying mediump vec2 oUV;
varying mediump vec3 oPos;

#endif


#ifdef CUBEMAP
uniform lowp samplerCube tex_sky;
#else
uniform lowp sampler2D tex_sky;
#endif

#ifdef FOG

#ifdef ES3

in mediump vec4 oFog;

#else

varying mediump vec4 oFog;

#endif

#endif

#ifdef ES3
layout (location = 0) out lowp vec4 Out_Color;
#endif


void main()
{
#ifdef ES3

#ifdef CUBEMAP
   mediump vec4 oColor = texture(tex_sky , normalize(oPos));
#else
   mediump vec4 oColor = texture(tex_sky , oUV);
#endif

#else

#ifdef CUBEMAP
   mediump vec4 oColor = textureCube(tex_sky , normalize(oPos));
#else
   mediump vec4 oColor = texture2D(tex_sky , oUV);
#endif

#endif


#ifdef FOG
	oColor.xyz = oColor.xyz * oFog.w + oFog.xyz;
#endif

#ifdef ES3
   Out_Color = oColor;
#else
   gl_FragColor = oColor ;
#endif   
   
}