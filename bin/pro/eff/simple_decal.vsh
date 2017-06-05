// simple decal vertex shader

//#define TEXANI

uniform MEDIUMP mat4 u_matWorld;	// not used.
uniform MEDIUMP vec3 u_vCameraPos;	// not used. 
uniform MEDIUMP mat4 u_matWVP;
uniform MEDIUMP float u_fDisplayBias;
uniform MEDIUMP mat4 u_matTexture;

vsin MEDIUMP vec4 a_vPosition;
vsin MEDIUMP vec2 a_vTexCoord;

vsout MEDIUMP vec2 v_vTexCoord;

void main(void)
{
	gl_Position = a_vPosition * u_matWVP;
	gl_Position.z -= u_fDisplayBias;

#if defined(TEXANI)
	v_vTexCoord = (vec4(a_vTexCoord, 0.0, 1.0) * u_matTexture).xy;
#else
	v_vTexCoord =  a_vTexCoord;
#endif
}
