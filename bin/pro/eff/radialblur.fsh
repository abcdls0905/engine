psin MEDIUMP vec2 oUV;
psin MEDIUMP vec2 oUV1;
psin MEDIUMP vec2 oUV2;
psin MEDIUMP vec2 oUV3;
psin MEDIUMP float blur;

uniform MEDIUMP sampler2D tex_color;
uniform MEDIUMP sampler2D tex_lastcolor;

void main()
{
#if defined(FLAG_ENABLE)
    MEDIUMP vec4 oColor = tex2D( tex_color, oUV )*0.25;
    oColor += tex2D( tex_color, oUV1 )*0.25;
    oColor += tex2D( tex_color, oUV2 )*0.25;
    oColor += tex2D( tex_color, oUV3 )*0.25;
#else
   MEDIUMP vec4 oColor = mix(tex2D(tex_lastcolor,oUV),tex2D(tex_color,oUV),vec4(blur,blur,blur,blur));
#endif

   OutputColor0(oColor);
}