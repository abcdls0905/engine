uniform MEDIUMP sampler2D tex_Colormap;
uniform MEDIUMP sampler2D tex_Depthmap;
uniform MEDIUMP sampler2D tex_Blurmap;

uniform MEDIUMP vec4 c_DepthParam;
uniform MEDIUMP vec4 c_LinearParam;
uniform MEDIUMP vec4 c_ViewportSize;
uniform MEDIUMP vec4 c_DOFValue;

psin HIGHP vec2 oTex;

HIGHP float getlinerdepth(in MEDIUMP sampler2D sdepth,in MEDIUMP vec2 UV)
{
   HIGHP float zvalue = tex2D( sdepth , UV ).x;
   zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
   return zvalue;
}

void main(void)
{	
	MEDIUMP vec4 oColor;
	
#ifdef USE_HALF_DEPTH
	HIGHP float depth = tex2D( tex_Depthmap , oTex ).x * c_DepthParam.w;
#else
	HIGHP float depth = getlinerdepth(tex_Depthmap, oTex);
#endif
	
#ifdef LINEAR
	HIGHP float viewdepth = clamp((depth - c_LinearParam.x) / c_LinearParam.y, 0.0, 1.0) + 0.01;
#else	
	HIGHP float viewdepth = abs(depth - c_DOFValue.x) * c_DOFValue.y + 0.01;
#endif

	viewdepth = clamp(viewdepth, 0.0, 1.0);
	
	vec2 poisson[8];
	
	poisson[0] = vec2(0.0,    0.0);
	poisson[1] = vec2(0.527, -0.085);
	poisson[2] = vec2(-0.040,  0.536);
	poisson[3] = vec2(-0.670, -0.179);
	poisson[4] = vec2(-0.419, -0.616);
	poisson[5] = vec2(0.440, -0.639);
	poisson[6] = vec2(-0.757,  0.349);
	poisson[7] = vec2(0.574,  0.685);
	
	vec2 samper_bias = viewdepth * c_DOFValue.z / c_ViewportSize.xy;
	
	vec3 blur = vec3(0.0, 0.0, 0.0);
	
	vec4 cr = tex2D(tex_Blurmap, oTex + poisson[0] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[1] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[2] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[3] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[4] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[5] * samper_bias);
	blur += cr.rgb;
	
	cr = tex2D(tex_Blurmap, oTex + poisson[6] * samper_bias);
	blur += cr.rgb;			
	
	cr = tex2D(tex_Blurmap, oTex + poisson[7] * samper_bias);
	blur += cr.rgb;

	blur /= 8.0;
	
	vec4 focus = tex2D(tex_Colormap, oTex);
	
	oColor.rgb = mix(focus.rgb, blur.rgb, viewdepth);
	oColor.a = 1.0;
	
	OutputColor0(oColor);
}
