precision mediump float;
precision mediump int;

attribute mediump vec2 iPos;
attribute mediump vec2 iUV;


#if defined(GRADUAL) 
uniform highp mat4  c_matViewProjI;
#endif

varying mediump vec2 oUV;

#if defined(GRADUAL) 
varying highp vec3 oPixelDir;
#endif



void main(void)
{
   highp vec4 pos = vec4(iPos,0.0,1.0) ; 
   gl_Position = pos;
   oUV = iUV ;
   
#if defined(GRADUAL) 
	highp vec4 temp = pos * c_matViewProjI;
	highp vec4 temp2 = vec4(0.0, 0.0, -1.0, 0.0)*c_matViewProjI;
	oPixelDir = temp.xyz / temp.w - temp2.xyz / temp2.w;
#endif   
   
}