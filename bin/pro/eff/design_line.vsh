attribute mediump vec3 iPos;
attribute mediump vec4 iDiffuse;
uniform mediump mat4 c_mtxWVP;

varying mediump vec4 oDiffuse;

void main(void)
{
	oDiffuse = (iDiffuse/255.0).zyxw;
	gl_Position = vec4(iPos, 1.0) * c_mtxWVP;
}

