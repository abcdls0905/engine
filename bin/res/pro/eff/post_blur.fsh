precision mediump float;
precision mediump int;

varying vec2 oUV;	

uniform sampler2D texColor;
uniform vec2 c_vTexelOffset;

void main()
{
#if defined(DOWN_FILTER)
	vec3 color = texture2D(texColor, oUV.xy + c_vTexelOffset.xy).xyz;
	color += texture2D(texColor, oUV.xy - c_vTexelOffset.xy).xyz;
	color += texture2D(texColor, oUV.xy + vec2(c_vTexelOffset.x, -c_vTexelOffset.y)).xyz;
	color += texture2D(texColor, oUV.xy + vec2(-c_vTexelOffset.x, c_vTexelOffset.y)).xyz;

	gl_FragColor.xyz = color.xyz * 0.25;

#elif defined(LOW)
	vec2 sample_texcoord = oUV.xy;

	sample_texcoord -= c_vTexelOffset.xy * 2.0;
	vec3 color  = texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 3.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor ,sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor, sample_texcoord).xyz;

	color /= 9.0;

	gl_FragColor.xyz = color;

#elif defined(MIDDLE)
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

#elif defined(HIGH)
	vec2 sample_texcoord = oUV.xy;

	sample_texcoord -= c_vTexelOffset.xy * 4.0;
	vec3 color  = texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 3.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += 2.0 * texture2D(texColor, sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor ,sample_texcoord).xyz;

	sample_texcoord += c_vTexelOffset.xy;
	color += texture2D(texColor, sample_texcoord).xyz;

	color /= 15.0;

	gl_FragColor.xyz = color;
#elif defined(BLUR_FINAL)
	vec3 color  = texture2D(texColor, oUV.xy).xyz;
	gl_FragColor.xyz = color;
#else
	gl_FragColor.xyz = vec3(1.0, 0.0, 0.0);
#endif

	gl_FragColor.w = 1.0;
}
