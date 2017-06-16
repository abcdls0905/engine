
vsin HIGHP vec4 iPos;
vsout HIGHP vec4 oViewScale;
vsout HIGHP vec4 oScreenPos;

uniform HIGHP vec3 c_vViewPos;
uniform HIGHP mat4 c_mtxWorld;
uniform HIGHP mat4 c_mtxViewProj;

void main(void)
{
	HIGHP vec4 oPos;
	
	oPos = c_mtxWorld * iPos;
	oViewScale = oPos;
	oPos = c_mtxViewProj * oPos;
	oPos.z = min(oPos.z, oPos.w);
	oScreenPos = oPos;

	oViewScale.xyz -= c_vViewPos;
	oViewScale.w = oPos.w;
	
	gl_Position = oPos;
}
