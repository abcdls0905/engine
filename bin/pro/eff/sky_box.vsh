//#define CUBEMAP
//#define FOG


attribute mediump vec4 iPos;
attribute mediump vec2 iUV;

uniform mediump mat4 mat4PMV;

varying mediump vec2 oUV;
varying mediump vec3 oPos;

void main(void)
{
   mediump vec4 pos = iPos ; 
   gl_Position = pos* mat4PMV ;
   oUV = iUV ;
   oPos = pos.xyz ;
   gl_Position.z = gl_Position.w;
}