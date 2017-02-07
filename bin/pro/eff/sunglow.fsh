
varying mediump vec4 oDiffuse;
varying mediump vec2 oTexCoord;

uniform sampler2D tex_BaseMap;

void main()
{
   mediump vec4 oColor = texture2D( tex_BaseMap , oTexCoord ) ;
   oColor.xyz*=oDiffuse.xyz;
   oColor.w = 1.0;  

   gl_FragColor = oColor;
//   gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}


