precision mediump float;
precision mediump int;

varying mediump vec2 oUV;
varying mediump vec2 oUV1;
varying mediump vec2 oUV2;
varying mediump vec2 oUV3;
varying mediump float blur;
 
uniform sampler2D tex_color;
uniform sampler2D tex_lastcolor;

void main()
{
#if defined(FLAG_ENABLE)
    mediump vec4 oColor = texture2D( tex_color, oUV )*0.25;
    oColor += texture2D( tex_color, oUV1 )*0.25;
    oColor += texture2D( tex_color, oUV2 )*0.25;
    oColor += texture2D( tex_color, oUV3 )*0.25;
#else
   mediump vec4 oColor = mix(texture2D(tex_lastcolor,oUV),texture2D(tex_color,oUV),vec4(blur,blur,blur,blur));
#endif

   gl_FragColor = oColor;
}