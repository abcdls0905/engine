
attribute mediump vec2 iPos;
attribute mediump vec2 iUV;
 
varying mediump vec2 oUV;
varying mediump vec3 oPos;

void main(void)
{
   mediump vec4 pos = vec4(iPos,0.0,1.0) ; 
   gl_Position = pos;
   oUV = iUV ;
   oPos = pos.xyz ;
}