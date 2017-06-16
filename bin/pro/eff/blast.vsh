vsin MEDIUMP vec2 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 oLightProjPos;
vsout MEDIUMP vec2 oVertexProjPos;

uniform MEDIUMP mat4 c_mtxViewProjection;
uniform MEDIUMP vec4 c_vFocusPosition;

void main(void)
{
	vec4 tempProjPos = c_vFocusPosition * c_mtxViewProjection;
	
	oLightProjPos = tempProjPos.xy / tempProjPos.w;
	vec4 pos = vec4(iPos.xy * 2.0, 0.0, 1.0);
	oVertexProjPos = pos.xy;

	gl_Position = pos * abs(tempProjPos.w);
}