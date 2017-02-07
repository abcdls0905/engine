precision mediump float;
precision mediump int;

#ifdef ES3

in mediump vec3 iPos;
in mediump vec2 iUV;

#else

attribute mediump vec3 iPos;
attribute mediump vec2 iUV;

#endif

uniform mediump mat4 mat4PMV;

#ifdef ES3

out mediump vec2 oUV;
out mediump vec3 oPos;


#else

varying mediump vec2 oUV;
varying mediump vec3 oPos;

#endif//ES3

#ifdef FOG

#ifdef ES3

/* 
layout(std140) uniform fog
{
  mediump vec4 c_FogColor;
  mediump vec4 c_FogHeight;
};
*/

uniform mediump vec4 c_FogColor;
uniform mediump vec4 c_FogHeight;

out mediump vec4 oFog;


#else

uniform mediump vec4 c_FogColor;
uniform mediump vec4 c_FogHeight;

varying mediump vec4 oFog;

#endif//ES3



#endif//FOG

void main(void)
{
   mediump vec4 pos = vec4(iPos, 1.0); 
   gl_Position = pos* mat4PMV ;
   oUV = iUV ;
   oPos = pos.xyz;
   gl_Position.z = gl_Position.w;

#ifdef FOG
    vec3 ViewDir = normalize(pos.xyz);

	float fogValue = clamp(ViewDir.y / c_FogHeight.x, 0.0, 1.0);
	oFog.w = pow(max(fogValue, 0.00001), c_FogHeight.y);
	
	oFog.xyz = c_FogColor.xyz * (1.0 - oFog.w);
#endif

}