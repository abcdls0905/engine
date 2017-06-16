vsin MEDIUMP vec2 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 oPos;

void main(void)
{
   oPos = iUV;
   gl_Position = vec4(iPos.xy, 0.0, 1.0);
}