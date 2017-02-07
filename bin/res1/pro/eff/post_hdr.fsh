precision mediump float;
precision mediump int;

varying vec2 oUV;

#ifdef EXTRACT_HIGHLIGHTS
uniform vec2 c_vCutOffAndPower;
#endif 

#if defined(BLUR_HORIZ) || defined(BLUR_VERT)
uniform float c_fBlurOffset;
#endif

#if defined(DOF)
uniform vec4 c_vTexSizeAndBlurDir;
#endif

#ifdef ADAPT_LUM
uniform float c_fLumDelay;
#endif

#ifdef TONE_MAP
uniform vec3 c_vHDRClampAndExposure;
#endif

uniform sampler2D texColor;

#if defined(DOF) || defined(ADAPT_LUM) || defined(TONE_MAP)
uniform sampler2D texColor1;
#endif

#ifdef TONE_MAP
uniform sampler2D texColor2;
uniform sampler2D texColor3;
#endif

#ifdef DOF
uniform sampler2D texColor4;
uniform sampler2D texColor5;
#endif

#define LUMINANCE(rgb) (0.21*rgb.x + 0.72*rgb.y + 0.07*rgb.z)

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

#if defined QUART_DOWN_FILTER

	color.xyz = texture2D(texColor, oUV.xy).xyz;

#elif defined EXTRACT_HIGHLIGHTS

	vec3 curColor = texture2D(texColor, oUV.xy).xyz;

	float lum = LUMINANCE(curColor.xyz);

	if (lum >= c_vCutOffAndPower.x)
	{
		color.xyz = curColor.xyz;
		color.xyz *= c_vCutOffAndPower.y;
	}

#elif defined BLUR_HORIZ

	color.xyz  = texture2D(texColor, oUV.xy + vec2(-c_fBlurOffset, 0.0)).xyz * 0.29;
	color.xyz += texture2D(texColor, oUV.xy).xyz * 0.42;
	color.xyz += texture2D(texColor, oUV.xy + vec2(c_fBlurOffset, 0.0)).xyz * 0.29;

#elif defined BLUR_VERT

	color.xyz  = texture2D(texColor, oUV.xy + vec2(0.0, -c_fBlurOffset)).xyz * 0.29;
	color.xyz += texture2D(texColor, oUV.xy).xyz * 0.42;
	color.xyz += texture2D(texColor, oUV.xy + vec2(0.0, c_fBlurOffset)).xyz * 0.29;

#elif defined DOF

	float dx = 1.0 / c_vTexSizeAndBlurDir.x;
	float dy = 1.0 / c_vTexSizeAndBlurDir.y;

	vec2 before = vec2(-dx, -dy) * c_vTexSizeAndBlurDir.zw;
	vec2 after = vec2(dx, dy) * uDirection;
	vec2 offset1 = before * 2.0;
	vec2 offset2 = before * 1.0;
	vec2 offset3 = after * 1.0;
	vec2 offset4 = after * 2.0;
	
	vec3 color = texture2D(texColor, oUV.xy).xyz;

	float numPixels = 1.0;

	vec3 blurPixel = texture2D(texColor, oUV.xy + offset1).xyz;
	color += blurPixel;
	numPixels = numPixels + 1.0;

	blurPixel = texture2D(texColor1, oUV.xy + offset2).xyz;
	color += blurPixel;
	numPixels = numPixels + 1.0;

    blurPixel = texture2D(texColor1, oUV.xy + offset3).xyz;
	color += blurPixel;
	numPixels = numPixels + 1.0;

	blurPixel = texture2D(texColor1, oUV.xy + offset4).xyz;
	color += blurPixel;
	numPixels = numPixels + 1.0;

	color /= numPixels;

#elif defined ADAPT_LUM

	float preLum  = texture2D(texColor, oUV.xy).x;
	color.xyz = texture2D(texColor1, oUV.xy).xyz;

	float curLum = LUMINANCE(color.xyz);
	color.x = max(0.0, preLum + ((curLum - preLum) * c_fLumDelay));

#elif defined TONE_MAP

	vec3 hdrColor = texture2D(texColor, oUV.xy).xyz;

#ifdef BLOOM
	hdrColor += texture2D(texColor2, oUV.xy).xyz;
	hdrColor += texture2D(texColor3, oUV.xy).xyz;
#endif

#ifdef DOF

#endif

	float overScreenLum = c_vHDRClampAndExposure.z / texture2D(texColor1, oUV.xy).x;

	// apply tone mapping function
	float hdrLum = LUMINANCE(hdrColor);
	float lumScaled = clamp(hdrLum * overScreenLum, c_vHDRClampAndExposure.x, c_vHDRClampAndExposure.y);
	color.xyz = hdrColor * lumScaled;

#endif

	gl_FragColor = color;
}
