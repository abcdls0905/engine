
vsin HIGHP vec4 iPos;
vsout HIGHP vec3 oNormal;

uniform HIGHP vec3 c_vPos;
uniform HIGHP vec3 c_vReferPos;
uniform HIGHP mat4 c_mtxViewProj;

void main(void)
{
	HIGHP vec4 oPos;
		
	oNormal = iPos.xyz;
		
	vec3 i_pos = c_vPos - c_vReferPos;

	oPos.w = 1.0;
	oPos.xyz = iPos.xyz * 0.5 + i_pos;
	
	oPos = c_mtxViewProj * oPos;
	
	gl_Position = oPos;
}
