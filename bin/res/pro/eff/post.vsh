
attribute highp vec2 iPos;
attribute highp vec2 iUV;
 
varying mediump vec2 oUV;
varying highp vec3 oPos;

void main(void)
{
   highp vec4 pos = vec4(iPos,0.0,1.0) ; 
   gl_Position = pos;
   oUV = iUV ;
   oPos = pos.xyz ;
}