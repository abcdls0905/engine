vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;

void main(void)
{
	oUV = iUV;
	gl_Position = vec4(iPos, 0.0, 1.0);
}
