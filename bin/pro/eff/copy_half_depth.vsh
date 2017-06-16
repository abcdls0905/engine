vsin MEDIUMP vec2 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 oUV;

void main(void)
{
	oUV = iUV;
	gl_Position = vec4(iPos, 0.0, 1.0);
}