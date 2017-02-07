

#ifdef MASK
attribute mediump vec2 iPos;

uniform mediump float c_fShadowMaxZ;
#else
attribute mediump vec4 iPos;

varying mediump vec4 oViewScale;
varying mediump vec4 oScreenPos;

uniform mediump mat4 c_mtxWVP;
uniform mediump mat4 c_mtxProjInverse;
#endif

void main(void)
{
#ifdef MASK
	gl_Position = vec4(iPos.xy, c_fShadowMaxZ, 1.0);
#else
	mediump vec4 oPos =  c_mtxWVP * iPos;

	oViewScale = c_mtxProjInverse * oPos;
	oScreenPos = oPos;
	gl_Position = oPos;
#endif
}