
vsin HIGHP vec4 iPos;

void main(void)
{
	vec4 oPos;
	
	oPos = vec4(iPos.xy, 0.0, 1.0);
	
	gl_Position = oPos;
}