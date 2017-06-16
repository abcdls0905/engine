vsin MEDIUMP vec2 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 uv;
vsout MEDIUMP vec4 color;

uniform MEDIUMP vec2 c_SizeBright;
uniform MEDIUMP vec4 c_LightProjPos;
uniform MEDIUMP vec4 c_ExternTint;
uniform MEDIUMP vec2 c_AspectRatio;
uniform MEDIUMP mat4 c_mtxForm;

void main(void)
{
	color = vec4(c_ExternTint.rgb * c_SizeBright.y, c_ExternTint.a);
	uv = iUV;
	
	vec4 pos = vec4(iPos, 0.0, 1.0);
	pos.xy *= c_SizeBright.x;
	
	pos *= c_mtxForm;
 	pos.xy *= c_AspectRatio;
	
	gl_Position = vec4(pos.xy + c_LightProjPos.xy, 0.0, 1.0);
}