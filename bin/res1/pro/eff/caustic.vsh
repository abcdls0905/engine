//#define SHADOW 1

attribute mediump vec4 iPos;

// cbPerObject
uniform mediump mat4 c_mtxViewInverse;
uniform mediump mat4 c_mtxProjInverse;
uniform mediump vec2 c_HalfPixelSize;
uniform mediump float c_fWaterHeight;
uniform mediump float c_fTimes;
uniform mediump vec2 c_BrightAndFocus;
uniform mediump vec3 c_vScaling;
uniform mediump vec4 c_LightColor;
uniform mediump vec3 c_vLightDir;

varying mediump vec2 oTex0;
varying mediump vec4 oViewScale;

void main()
{
   mediump vec4 oPos = vec4(iPos.xy,0.0,1.0);
   oTex0 = (iPos.xy * vec2(1.0, -1.0) + 1.0) * 0.5;
   oViewScale = oPos*c_mtxProjInverse;
   gl_Position = oPos;
}