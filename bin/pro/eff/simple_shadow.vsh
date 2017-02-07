attribute mediump vec4 iPos;
 
uniform mediump mat4  c_mtxViewProj;
uniform mediump mat4  c_mtxLightMVP;
uniform mediump float c_fLightDispersion;
uniform mediump vec4  c_TopColor;
uniform mediump vec4  c_BottomColor;

varying mediump vec4 oPos;
varying mediump vec4 oLightDepth;



void main(void)
{
   mediump vec4 pos  = iPos * c_mtxViewProj ;
   pos.z = pos.z - 0.003 * (1.0 - pos.z / pos.w) * pos.w; 
   oPos = pos;
   
   oLightDepth = iPos * c_mtxLightMVP;
   gl_Position = pos;
}