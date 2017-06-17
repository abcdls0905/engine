varying mediump vec2 oUV;
varying highp vec3 oPos;
 
uniform sampler2D tex_color;
uniform sampler2D tex_depth;
 
void main()
{
   mediump vec4 oColor = texture2D( tex_color , oUV ) ;
//   mediump vec4 oColor = vec4(oUV,0.0,1.0);
//   oColor.rg = vec2(0.0,0.0);
   gl_FragColor = oColor ;
}