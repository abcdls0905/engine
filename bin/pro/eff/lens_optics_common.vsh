vsin MEDIUMP vec4 iPos;
vsin MEDIUMP vec4 iColor;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 uv;
vsout MEDIUMP vec4 color;

uniform MEDIUMP vec2 c_SizeBright;
uniform MEDIUMP vec4 c_LightProjPos;
uniform MEDIUMP vec4 c_ExternTint;

void main(void)
{
	color = iColor * c_SizeBright.y * c_ExternTint;
	uv = iUV;
	
	vec4 pos = iPos;
	pos.xy *= c_SizeBright.x;
	
#ifdef ROTATE_POS
	vec2 dir = c_LightProjPos.xy;
	vec2 rotSinCos = normalize(dir);
	vec2 tmp = pos.xy;
	pos.x = tmp.x * rotSinCos.x - tmp.y * rotSinCos.y;
	pos.y = tmp.y * rotSinCos.x + tmp.x * rotSinCos.y;
#endif
	
	gl_Position = vec4(pos.xy + c_LightProjPos.xy, 0.0, 1.0);
}