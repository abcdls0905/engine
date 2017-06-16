uniform MEDIUMP sampler2D tex_Colormap;
uniform MEDIUMP sampler2D tex_Depthmap;
uniform MEDIUMP vec4 c_LinearParam;
uniform MEDIUMP vec4 c_ViewportSize;
uniform MEDIUMP vec4 c_DOFValue;

psin HIGHP vec2 oTex;

void main(void)
{	
	vec2 poisson[8];
	
	poisson[0] = vec2(0.0,    0.0);
	poisson[1] = vec2(0.527, -0.085);
	poisson[2] = vec2(-0.040,  0.536);
	poisson[3] = vec2(-0.670, -0.179);
	poisson[4] = vec2(-0.419, -0.616);
	poisson[5] = vec2(0.440, -0.639);
	poisson[6] = vec2(-0.757,  0.349);
	poisson[7] = vec2(0.574,  0.685);
	
	vec4 oColor;

	float viewdepth = tex2D(tex_Colormap, oTex).a;
	
	vec2 samper_bias = viewdepth * c_DOFValue.z / c_ViewportSize.xy;

	float num = 0.0;
	vec3 color = vec3(0.0, 0.0, 0.0);

	vec4 cr = tex2D(tex_Colormap, oTex + poisson[0] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[1] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[2] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[3] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[4] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[5] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;
	
	cr = tex2D(tex_Colormap, oTex + poisson[6] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;				
	
	cr = tex2D(tex_Colormap, oTex + poisson[7] * samper_bias);
	color += cr.rgb * cr.a;
	num += cr.a;

	color /= num;
	
	oColor = vec4(color, 1.0);
	
	OutputColor0(oColor);
}
