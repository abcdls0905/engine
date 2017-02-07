//#define ADJUST 1
//#define GRADUAL 1
//#define ANGLE 1


attribute mediump vec2 iPos;
attribute mediump vec2 iUV;


#if defined(GRADUAL) 
uniform mediump mat4  c_matViewProjI;
#endif

varying mediump vec2 oUV;

#if defined(GRADUAL) 
varying mediump vec3 oPixelDir;
#endif



void main(void)
{
   mediump vec4 pos = vec4(iPos,0.0,1.0) ; 
   gl_Position = pos;
   oUV = iUV ;
   
#if defined(GRADUAL) 
	vec4 temp = pos * c_matViewProjI;
	vec4 temp2 = vec4(0.0, 0.0, -1.0, 0.0)*c_matViewProjI;
	oPixelDir = temp.xyz / temp.w - temp2.xyz / temp2.w;
#endif   
   
}