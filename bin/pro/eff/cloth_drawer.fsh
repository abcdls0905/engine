psin MEDIUMP vec2 oUV;

uniform MEDIUMP sampler2D tex_color;

void main(void)
{
    OutputColor0(tex2DGamma(tex_color, oUV));
}