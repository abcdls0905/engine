varying mediump vec2 oUV;

 
uniform sampler2D tex_color;

void main()
{
   highp vec2 UV = oUV;
   gl_FragColor = texture2D( tex_color , UV )  ;
}