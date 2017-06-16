vsin HIGHP vec4 iPos;
vsin HIGHP vec2 iUV;

vsout MEDIUMP vec2 uv;

void main(void)
{
	uv = iUV;
	gl_Position = iPos;
}