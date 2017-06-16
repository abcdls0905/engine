psin vec2 oUV;

#if defined(BLUR)
psin vec2 oUV1;
psin vec2 oUV2;
psin vec2 oUV3;
psin vec2 oUV4;
psin vec2 oUV5;
psin vec2 oUV6;
psin vec2 oUV7;
psin vec2 oUV8;
#endif

uniform MEDIUMP sampler2D texColor;
uniform MEDIUMP sampler2D texBlurColor;
uniform vec2 c_vTexelOffset;
uniform float c_fBloomIntensity;
uniform float c_fMaxBrightness;

uniform float c_psOffsets[8];
uniform float c_psWeights[8];

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fLumScale;
#endif

vec3 brightness_overflow(vec3 cr, float crMax)
{
	float brightnessmax = max(cr.r, cr.g);
	
	brightnessmax = max(brightnessmax, cr.b);
	
	return cr * (brightnessmax - crMax);
}

void main()
{
	vec4 oColor = vec4(0.0, 0.0, 0.0, 0.0);
#if defined(DOWN_FILTER)
	vec3 color = tex2D(texColor, oUV.xy + c_vTexelOffset.xy).rgb;
	color += tex2D(texColor, oUV.xy - c_vTexelOffset.xy).rgb;
	color += tex2D(texColor, oUV.xy + vec2(c_vTexelOffset.x, -c_vTexelOffset.y)).rgb;
	color += tex2D(texColor, oUV.xy + vec2(-c_vTexelOffset.x, c_vTexelOffset.y)).rgb;
	
	color *= 0.25;
	
	oColor.xyz = brightness_overflow(color, c_fMaxBrightness);
		
#elif defined(BLUR)
	vec3 color = c_psWeights[0] * tex2D(texColor, vec2(oUV1)).rgb;
  color += c_psWeights[1] * tex2D(texColor, vec2(oUV2)).rgb;
  color += c_psWeights[2] * tex2D(texColor, vec2(oUV3)).rgb;
  color += c_psWeights[3] * tex2D(texColor, vec2(oUV4)).rgb;
  color += c_psWeights[4] * tex2D(texColor, vec2(oUV5)).rgb;
  color += c_psWeights[5] * tex2D(texColor, vec2(oUV6)).rgb;
  color += c_psWeights[6] * tex2D(texColor, vec2(oUV7)).rgb;
  color += c_psWeights[7] * tex2D(texColor, vec2(oUV8)).rgb;

	oColor.xyz = color;
	
#elif defined(BLUR_SIMPLE)
	vec3 color = tex2D(texColor, vec2(oUV.x + c_vTexelOffset.x, oUV.y + c_vTexelOffset.y)).rgb;
	color += tex2D(texColor, vec2(oUV.x + c_vTexelOffset.x * 2.0, oUV.y + c_vTexelOffset.y * 2.0)).rgb;
	color += tex2D(texColor, oUV).rgb;
	color += tex2D(texColor, vec2(oUV.x - c_vTexelOffset.x * 2.0, oUV.y - c_vTexelOffset.y * 2.0)).rgb;
	color += tex2D(texColor, vec2(oUV.x - c_vTexelOffset.x, oUV.y - c_vTexelOffset.y)).rgb;

	oColor.xyz = color * 0.2;

#elif defined(BLOOM_FINAL)
	vec3 color  = tex2D(texColor, oUV.xy).xyz;
	
	#ifdef LUM_SCALE
		color *= c_fLumScale;
	#endif
	
	color += c_fBloomIntensity * tex2D(texBlurColor ,oUV.xy).xyz;

	oColor.xyz = color;
#else
	oColor.xyz = vec3(1.0, 0.0, 0.0);
#endif

	oColor.w = 1.0;
	OutputColor0(oColor);
}
