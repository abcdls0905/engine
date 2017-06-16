vsin HIGHP vec4 iPos;

uniform HIGHP mat4 c_mtxWVP;

void main(void)
{
	vec4 oPos;
	
	oPos = c_mtxWVP * iPos;
	oPos.z = min(oPos.z, oPos.w);
	gl_Position = oPos;
}