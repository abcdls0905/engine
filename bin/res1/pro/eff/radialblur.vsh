precision mediump float;
precision mediump int;

attribute mediump vec2 iPos;
attribute mediump vec2 iUV;

uniform mediump float c_vRadialBlurAmount;

varying mediump vec2 oUV;
varying mediump vec2 oUV1;
varying mediump vec2 oUV2;
varying mediump vec2 oUV3;
varying mediump float blur;

void main(void)
{
   highp vec4 pos = vec4(iPos,0.0,1.0); 
   gl_Position = pos;
   oUV = iUV;

#if defined(FLAG_ENABLE)
   mediump vec2 uv =  (vec2(pos.x, - pos.y) + 1.0) / 2.0;
   //uv.y = 1.0 - uv.y;
   uv = vec2(uv.x-0.5,0.5-uv.y);

   oUV = uv * (1.0 + 0.00 * c_vRadialBlurAmount) + vec2(0.5,0.5);
   oUV1 = uv * (1.0 + 0.25 * c_vRadialBlurAmount) + vec2(0.5,0.5);
   oUV2 = uv * (1.0 + 0.50 * c_vRadialBlurAmount) + vec2(0.5,0.5);
   oUV3 = uv * (1.0 + 0.75 * c_vRadialBlurAmount) + vec2(0.5,0.5);
#endif

	blur = 1.0 - c_vRadialBlurAmount;
}