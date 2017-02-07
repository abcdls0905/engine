varying mediump vec2 oUV;

 
uniform mediump sampler2D tex_color;

void main()
{
   mediump vec2 UV = oUV;
   gl_FragColor = texture2D( tex_color , UV )  ;
}