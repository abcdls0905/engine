precision highp float;
precision highp int;

attribute highp vec2 iPos;
attribute highp vec2 iUV;
 
varying highp vec2 oUV;


void main(void)
{
   highp vec4 pos = vec4(iPos,0.0,1.0) ; 
   gl_Position = pos;
   oUV = iUV ;
}