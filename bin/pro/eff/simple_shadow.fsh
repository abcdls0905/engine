uniform mediump mat4  c_mtxViewProj;
uniform mediump mat4  c_mtxLightMVP;
uniform mediump float c_fLightDispersion;
uniform mediump vec4  c_TopColor;
uniform mediump vec4  c_BottomColor;

varying mediump vec4 oPos;
varying mediump vec4 oLightDepth;


void main()
{
   mediump vec4 posLight = oLightDepth / oLightDepth.w;
   mediump vec2 texCoord = posLight.xy * 0.5 + 0.5;
   
   texCoord.y = 1.0 - texCoord.y;
   
   mediump vec4 diffuse = mix( c_TopColor , c_BottomColor , texCoord.y * texCoord.y );
   
   
   mediump float r = clamp( length( texCoord - 0.5 ) * 4.0 ,0.0,1.0);
   gl_FragColor = ( 1.0 - r ) * ( vec4(1.0,1.0,1.0,1.0) - diffuse);
}