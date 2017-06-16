vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;
vsout vec4 oViewScale;

uniform HIGHP mat4  c_mtxProjInverse;

#define FLT_MIN 1.175494351e-38

void main(void)
{
	oUV = iUV;
	oViewScale = c_mtxProjInverse * vec4(iPos.xy, 0.0, 1.0);
	gl_Position = vec4(iPos, 1.0 - FLT_MIN, 1.0);
}
