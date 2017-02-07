attribute mediump vec3 iPos;
attribute mediump vec3 iNor;

uniform mediump vec4 c_vDiffuse;
uniform mediump vec4 c_vLightDirection;
uniform mediump mat4 c_mtxWVP;
uniform mediump mat4 c_mtxWorld;

varying mediump vec4 oDiffuse;
varying mediump vec4 oNormal;

void main(void)
{
	oDiffuse = c_vDiffuse;
    oNormal = vec4(iNor, 0.0) * c_mtxWorld;
	oNormal = normalize(oNormal);
	oNormal.w = clamp(dot(oNormal.xyz, c_vLightDirection.xyz), 0.0, 1.0);
    oNormal.w = sqrt(oNormal.w);
	gl_Position = vec4(iPos, 1.0) * c_mtxWVP;
}
