psin vec2 oUV;

#if defined(BLUR_HORIZ) || defined(BLUR_VERT)
	psin vec2 oUV1;
	psin vec2 oUV2;
	psin vec2 oUV3;
	psin vec2 oUV4;
	psin vec2 oUV5;
	psin vec2 oUV6;
	psin vec2 oUV7;
	psin vec2 oUV8;
#endif

#if defined(EXTRACT_HIGHLIGHTS) || defined(TONE_MAP)
	uniform vec2 c_vCutOffAndPower;
#endif

uniform vec2 c_vTexelOffset;

#if defined(BLUR_HORIZ) || defined(BLUR_VERT)
	uniform float c_psOffsets[8];
	uniform float c_psWeights[8];
#endif

#ifdef ADAPT_LUM
	uniform float c_fLumDelay;
#endif

#ifdef TONE_MAP
	uniform vec3 c_vHDRClampAndExposure;
#endif

#ifdef FILM_MAPPING
	uniform vec4 c_HDRFilmCurve;
#endif

uniform MEDIUMP sampler2D texColor;

#if defined(ADAPT_LUM) || defined(TONE_MAP)
	uniform MEDIUMP sampler2D texColor1;
#endif

#ifdef TONE_MAP
	uniform MEDIUMP sampler2D texColor2;
	uniform MEDIUMP sampler2D texColor3;
#endif

#ifdef DOF
	uniform MEDIUMP sampler2D texColor4;
	uniform MEDIUMP sampler2D texColor5;
#endif

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fLumScale;
#endif

#define LUMINANCE(rgb) (0.21*rgb.x + 0.72*rgb.y + 0.07*rgb.z)

vec3 brightness_overflow(vec3 cr, float crMax)
{
	vec3 ori_color = cr;

	#ifdef LUM_SCALE
		ori_color *= c_fLumScale;
	#endif
	
	float brightnessmax = max(ori_color.r, ori_color.g);
	
	brightnessmax = max(brightnessmax, ori_color.b);
	
	return cr * (brightnessmax - crMax);
}

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

#if defined DOWN_FILTER

	color.xyz = tex2D(texColor, oUV.xy).xyz;
	
	OutputColor0(color);
#elif defined QUART_DOWN_FILTER

	color.xyz = tex2D(texColor, oUV.xy + c_vTexelOffset.xy).rgb;
	color.xyz += tex2D(texColor, oUV.xy - c_vTexelOffset.xy).rgb;
	color.xyz += tex2D(texColor, oUV.xy + vec2(c_vTexelOffset.x, -c_vTexelOffset.y)).rgb;
	color.xyz += tex2D(texColor, oUV.xy + vec2(-c_vTexelOffset.x, c_vTexelOffset.y)).rgb;
	
	color *= 0.25;

	OutputColor0(color);
#elif defined EXTRACT_HIGHLIGHTS
	
	vec3 curColor = tex2D(texColor, oUV.xy).xyz;

	color.xyz = brightness_overflow(curColor, c_vCutOffAndPower.x);

	OutputColor0(color);
#elif defined(BLUR_HORIZ) || defined(BLUR_VERT)
	
	color.xyz = c_psWeights[0] * tex2D(texColor, vec2(oUV1)).rgb;
  color.xyz += c_psWeights[1] * tex2D(texColor, vec2(oUV2)).rgb;
  color.xyz += c_psWeights[2] * tex2D(texColor, vec2(oUV3)).rgb;
  color.xyz += c_psWeights[3] * tex2D(texColor, vec2(oUV4)).rgb;
  color.xyz += c_psWeights[4] * tex2D(texColor, vec2(oUV5)).rgb;
  color.xyz += c_psWeights[5] * tex2D(texColor, vec2(oUV6)).rgb;
  color.xyz += c_psWeights[6] * tex2D(texColor, vec2(oUV7)).rgb;
  color.xyz += c_psWeights[7] * tex2D(texColor, vec2(oUV8)).rgb;
  
	OutputColor0(color);
#elif defined(BLUR_SIMPLE)
	color.xyz = tex2D(texColor, vec2(oUV.x + c_vTexelOffset.x, oUV.y + c_vTexelOffset.y)).rgb;
	color.xyz += tex2D(texColor, vec2(oUV.x + c_vTexelOffset.x * 2.0, oUV.y + c_vTexelOffset.y * 2.0)).rgb;
	color.xyz += tex2D(texColor, oUV).rgb;
	color.xyz += tex2D(texColor, vec2(oUV.x - c_vTexelOffset.x * 2.0, oUV.y - c_vTexelOffset.y * 2.0)).rgb;
	color.xyz += tex2D(texColor, vec2(oUV.x - c_vTexelOffset.x, oUV.y - c_vTexelOffset.y)).rgb;

	color.xyz *= 0.2;

	OutputColor0(color);
#elif defined ADAPT_LUM

	float preLum  = tex2D(texColor, oUV.xy).x;
	color.xyz = tex2D(texColor1, oUV.xy).xyz;

	float curLum = LUMINANCE(color.xyz);
	color.x = max(0.0, preLum + ((curLum - preLum) * c_fLumDelay));
	
	OutputColor0(color);
#elif defined TONE_MAP

	vec3 hdrColor = tex2D(texColor, oUV.xy).xyz;

	#ifdef LUM_SCALE
		hdrColor *= c_fLumScale;
	#endif
	
	#ifdef BLOOM
		hdrColor += tex2D(texColor2, oUV.xy).xyz * c_vCutOffAndPower.y;
	#endif

	//if average is 0, the statement "fSceneKey / average" below will result in black screen in metal with fast-math enabled
	float average = max(tex2D(texColor1, oUV.xy).x, 0.001);

	#ifdef LUM_SCALE
		average *= c_fLumScale;
	#endif
		
	#ifdef FILM_MAPPING
		float fSceneKey = c_vHDRClampAndExposure.z - 2.0 / (2.0 + log2(average + 1.0));
		float fExposure = clamp(fSceneKey / average, c_vHDRClampAndExposure.x, c_vHDRClampAndExposure.y);
		hdrColor.rgb = fExposure * hdrColor.rgb;
		
		vec4 HDRFilmCurve = c_HDRFilmCurve;
		vec4 c = vec4(max(hdrColor.rgb, vec3(0.0, 0.0, 0.0)), HDRFilmCurve.w);
		float ShoStren = 0.22 * HDRFilmCurve.x;
		float LinStren = 0.3 * HDRFilmCurve.y;
		float LinAngle = 0.1;
		float ToeStren = 0.2;
		float ToeNum = 0.01 * HDRFilmCurve.z;
		float ToeDenom = 0.3;
		vec4 compressedCol = ((c * (ShoStren * c + LinAngle*LinStren) + ToeStren*ToeNum) / (c * (ShoStren * c + LinStren) + ToeStren*ToeDenom)) - (ToeNum/ToeDenom);
		color.xyz = min(max(compressedCol.rgb / compressedCol.w, vec3(0.0, 0.0, 0.0)), vec3(1.0, 1.0, 1.0));
	#else
		float overScreenLum = c_vHDRClampAndExposure.z / average;

		// apply tone mapping function
		float hdrLum = LUMINANCE(hdrColor);
		float lumScaled = clamp(hdrLum * overScreenLum, c_vHDRClampAndExposure.x, c_vHDRClampAndExposure.y);
		color.xyz = hdrColor * lumScaled;
	#endif

	OutputColor0Gamma(color);

#elif defined ONLY_SOFT_GAMMA
	color.xyzw = tex2D(texColor, oUV.xy).xyzw;

	OutputColor0Gamma(color);
#endif
}
