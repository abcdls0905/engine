precision highp float;
precision highp int;

varying highp vec2 oUV;

 
uniform highp sampler2D tex_color;

void main()
{
   highp vec2 UV = oUV;
   gl_FragColor = texture2D( tex_color , UV )  ;
}