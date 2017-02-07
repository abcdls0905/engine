// pixel refraction shader


attribute mediump vec2 iPos;
attribute mediump vec2 iTex;

varying mediump vec2 oTex;


void main(void)
{
   oTex = iTex;
   gl_Position = vec4(iPos.xy, 0.0, 1.0);
}


