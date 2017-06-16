vsin HIGHP vec4 iPos;

vsout HIGHP vec4 oScreenPos;
vsout HIGHP vec4 oViewScale;
vsout HIGHP vec4 oPos;

uniform HIGHP mat4 c_mtxWVP;
uniform HIGHP mat4 c_mtxProjInverse;

void main(void)
{	
	HIGHP vec4 pos = c_mtxWVP * iPos;
	pos.z = min(pos.z, pos.w);
	oScreenPos = pos;
	oViewScale = c_mtxProjInverse * pos;
	oPos = pos;
	gl_Position = pos;
}
