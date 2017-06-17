// pixel refraction shader

vsin MEDIUMP vec2 iPos;
vsin MEDIUMP vec2 iUV;

vsout MEDIUMP vec2 oTex;

void main(void)
{
   oTex = iUV;
   gl_Position = vec4(iPos.xy, 0.0, 1.0);
}

