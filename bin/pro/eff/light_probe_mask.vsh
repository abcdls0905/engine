
vsin HIGHP vec4 iPos;

uniform HIGHP mat4 c_mtxWorld;
uniform HIGHP mat4 c_mtxViewProj;

void main(void)
{
	vec4 oPos;
	
	oPos = c_mtxWorld * iPos;
	oPos = c_mtxViewProj * oPos;
	oPos.z = min(oPos.z, oPos.w);
	
	gl_Position = oPos;
}
