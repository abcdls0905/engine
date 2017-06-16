// dizzy vertex shader

uniform MEDIUMP vec2 u_vViewportDimensions;

vsin MEDIUMP vec4 a_vPosition;
vsin MEDIUMP vec2 a_vTexCoord;

vsout MEDIUMP vec2 v_vTexCoord;

void main(void)
{
	gl_Position = vec4(a_vPosition.xy, 0.0, 1.0);
	v_vTexCoord = (vec2(gl_Position.x, gl_Position.y)
		+ 1.0 + u_vViewportDimensions) * 0.5;
}
