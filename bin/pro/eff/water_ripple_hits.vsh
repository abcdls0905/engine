vsin MEDIUMP vec4 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 uv;

uniform HIGHP mat4 c_MtxVP;

void main(void)
{
	uv = iUV;
	
	vec4 pos = iPos;
	pos *= c_MtxVP;
	
	gl_Position = vec4(pos.xy, 0.0, 1.0);
}