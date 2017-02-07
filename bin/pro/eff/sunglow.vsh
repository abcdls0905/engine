
attribute mediump vec4 iPos;
attribute mediump vec4 iDiffuse;
attribute mediump vec2 iTexCoord;

uniform mediump mat4 mtxWVP;

varying mediump vec4 oDiffuse;
varying mediump vec2 oTexCoord;

void main(void)
{
   mediump vec4 pos = vec4(iPos.xyz,1.0) ; 

   gl_Position = pos* mtxWVP ;
 
   oDiffuse = iDiffuse;
   oTexCoord = iTexCoord;
}