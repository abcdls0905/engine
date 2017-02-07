
// render like Billboard wire

//#define KEY_FRAME

attribute mediump vec3 iPos;
#if defined(KEY_FRAME)
attribute mediump vec2 iTexCoord;
#endif

uniform mediump mat4 c_mtxWVP;
uniform mediump vec4 c_vDiffuse;
#if defined(KEY_FRAME)
uniform mediump vec4 c_vTexelSize;
#endif

varying mediump vec4 oDiffuse;

void main(void)
{
	oDiffuse = c_vDiffuse;
	gl_Position = vec4(iPos, 1.0) * c_mtxWVP;
#if defined(KEY_FRAME)
	gl_Position.x += iTexCoord.x * gl_Position.w * c_vTexelSize.x;
	gl_Position.y += iTexCoord.y * gl_Position.w * c_vTexelSize.y;
#endif
}

