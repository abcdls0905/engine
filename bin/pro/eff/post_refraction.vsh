vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;

uniform vec2 c_vViewport;

void main()
{
	vec4 pos = vec4(iPos, 0.0,1.0);

	oUV.xy = iUV.xy;

	gl_Position = pos;
}