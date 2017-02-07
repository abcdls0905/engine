precision mediump float;
precision mediump int;

varying vec2 v_vec2UV0;	

uniform sampler2D tex_color;
uniform vec2 u_vec2TexelScale;
uniform vec2 u_vec2AmountAndPower;

void main()
{
	vec2 uv0 = v_vec2UV0.xy  * u_vec2TexelScale;

	vec3 color = texture2D(tex_color, uv0.xy).xyz;

#ifdef LOWEST
	vec4 sum = vec4(0);
	vec3 bloom;

	for(int i= -3 ;i < 3; i++)
	{
		sum += texture2D(tex_color, uv0 + vec2(-1, i) * 0.004) * u_vec2AmountAndPower.x;
		sum += texture2D(tex_color, uv0 + vec2(0, i) * 0.004) * u_vec2AmountAndPower.x;
		sum += texture2D(tex_color, uv0 + vec2(1, i) * 0.004) * u_vec2AmountAndPower.x;
	}

	if (color.x < 0.3 && color.y < 0.3 && color.z < 0.3)
	{
		bloom = sum.xyz*sum.xyz * 0.012 + color;
	}
	else
	{
		if (color.x < 0.5 && color.y < 0.5 && color.z < 0.5)
		{
			bloom = sum.xyz*sum.xyz * 0.009 + color;
		}
		else
		{
			bloom = sum.xyz*sum.xyz * 0.0075 + color;
		}
	}

	color.xyz = mix(color, bloom, u_vec2AmountAndPower.y);
#endif

	gl_FragColor.xyz = color;
	gl_FragColor.w = 1.0;
}
