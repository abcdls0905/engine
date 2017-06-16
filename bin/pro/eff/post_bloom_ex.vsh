uniform vec2 c_vTexelOffset;
uniform float c_psOffsets[8];

vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 oUV;

#if defined(BLUR)
vsout vec2 oUV1;
vsout vec2 oUV2;
vsout vec2 oUV3;
vsout vec2 oUV4;
vsout vec2 oUV5;
vsout vec2 oUV6;
vsout vec2 oUV7;
vsout vec2 oUV8;
#endif

void main()
{
  oUV = iUV;
  gl_Position = vec4(iPos, 0.0,1.0);
  
#if defined(BLUR)
	oUV1 = oUV + c_vTexelOffset * c_psOffsets[0];
	oUV2 = oUV + c_vTexelOffset * c_psOffsets[1];
	oUV3 = oUV + c_vTexelOffset * c_psOffsets[2];
	oUV4 = oUV + c_vTexelOffset * c_psOffsets[3];
	oUV5 = oUV + c_vTexelOffset * c_psOffsets[4];
	oUV6 = oUV + c_vTexelOffset * c_psOffsets[5];
	oUV7 = oUV + c_vTexelOffset * c_psOffsets[6];
	oUV8 = oUV + c_vTexelOffset * c_psOffsets[7];
#endif
}