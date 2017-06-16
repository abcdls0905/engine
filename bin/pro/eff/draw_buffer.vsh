uniform MEDIUMP vec2 c_PixelSize;

vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;

void main(void)
{
	oUV = iUV;
	
	vec2 pos = (iPos * c_PixelSize * 2.0) + vec2(-1.0, 1.0);
	
	gl_Position = vec4(pos, 0.0, 1.0);
}