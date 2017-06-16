vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;
#ifdef DEPTH
	vsout vec4 oPosWS;
#endif

#ifdef DEPTH
	uniform mat4 c_matViewProjI;
	uniform vec4 c_vViewPos;
#endif

uniform vec2 c_vViewport;

void main()
{
  vec4 pos = vec4(iPos, 0.0,1.0);
  
  oUV = vec2(iUV.x + 0.5 / c_vViewport.x, iUV.y + 0.5 / c_vViewport.y);
	
  gl_Position = pos;
  
#ifdef DEPTH
	pos = pos * c_matViewProjI;
	oPosWS = pos - c_vViewPos * pos.w;
#endif 
}