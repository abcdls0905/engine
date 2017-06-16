vsin MEDIUMP vec4 iPos;
vsin MEDIUMP vec4 iColor;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 uv;
vsout MEDIUMP vec4 color;

uniform MEDIUMP vec4 c_LightProjPos;
uniform MEDIUMP float c_IllumRange;
uniform MEDIUMP vec4 c_LightColor;

void main(void)
{
	vec2 distDiff = c_LightProjPos.xy - iPos.xy;

	float distSq = dot(distDiff, distDiff) / c_IllumRange;
	float illum = exp2(-10.0 * (distSq));

	uv = iUV;
	color = iColor * vec4(c_LightColor.xyz * illum, 1.0);

	gl_Position = vec4(iPos.xy, 0.0, 1.0);
}