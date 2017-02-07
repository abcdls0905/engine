precision mediump float;
precision mediump int;

varying vec2 oUV;	

uniform sampler2D texColor;
uniform sampler2D texBlurColor;
uniform vec2 c_vTexelOffset;
uniform float c_fBloomIntensity;

void main()
{
#if defined(DOWN_FILTER)
	vec3 color = texture2D(texColor, oUV.xy + c_vTexelOffset.xy).xyz;
	color += texture2D(texColor, oUV.xy - c_vTexelOffset.xy).xyz;
	color += texture2D(texColor, oUV.xy + vec2(c_vTexelOffset.x, -c_vTexelOffset.y)).xyz;
	color += texture2D(texColor, oUV.xy + vec2(-c_vTexelOffset.x, c_vTexelOffset.y)).xyz;

	float luminance = dot(color.xyz,vec3(0.2126,0.7152,0.0722));
	gl_FragColor.xyz = color * (luminance - 0.25) * 0.25;

#elif defined(BLUR)
	vec2 sample_texcoord = oUV.xy;

	sample_texcoord -= c_vTexelOffset.xy * 3.0;
	vec3 color  = texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 3.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor ,sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor, sample_texcoord).xyz;

	color /= 11.0;

	gl_FragColor.xyz = color;

#elif defined(BLOOM_FINAL)
	vec3 color  = texture2D(texColor, oUV.xy).xyz;
	color += c_fBloomIntensity * texture2D(texBlurColor ,oUV.xy).xyz;

	gl_FragColor.xyz = color;
#else
	gl_FragColor.xyz = vec3(1.0, 0.0, 0.0);
#endif

	gl_FragColor.w = 1.0;
}
