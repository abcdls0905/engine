vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;

#define FLT_MIN 1.175494351e-38

void main(void)
{
	oUV = iUV;
	gl_Position = vec4(iPos, 1.0 - FLT_MIN, 1.0);
}
