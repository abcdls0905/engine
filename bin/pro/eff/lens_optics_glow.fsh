uniform MEDIUMP sampler2D tex_BrightnessMap;

psin MEDIUMP vec2 uv;
psin MEDIUMP vec4 color;

void main(void)
{
	#define thou uv.x
	#define theta uv.y
	
	vec4 HDRParams = vec4(6.2, 0.5, 0.09, 1.0);
	
	float constArea = 1.0;
	float focusFactor = 0.01;
	float fadingArea = mix(constArea, 0.0, clamp((thou - focusFactor) / (1.0 - focusFactor), 0.0, 1.0));

	const float fadingSpan = 0.001;
	float transition = clamp(mix(1.0,  0.0, (focusFactor - thou) / fadingSpan), 0.0, 1.0);
	float finalGrad = mix(constArea, fadingArea, transition);
	
	vec4 out_color = color * finalGrad;
	
	out_color.rgb *= out_color.a;
	
	OutputColor0(vec4((out_color.rgb * (HDRParams.x * out_color.rgb + HDRParams.y)) / (out_color.rgb * (HDRParams.x * out_color.rgb + 1.7) + HDRParams.z), 1.0));
}