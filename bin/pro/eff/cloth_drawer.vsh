vsin MEDIUMP vec3 iPos;
vsin MEDIUMP vec3 iNor;
vsin MEDIUMP vec2 iUV;

uniform MEDIUMP mat4 c_mtxWVP;

vsout MEDIUMP vec2 oUV;

void main(void)
{
    oUV = iUV;
    vec4 pos = vec4(iPos, 1.0);
    
    gl_Position = c_mtxWVP * pos;
}
