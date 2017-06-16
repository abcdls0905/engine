uniform MEDIUMP sampler2D tex_Colormap;
uniform MEDIUMP sampler2D tex_Depthmap;
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

	oColor = vec4(tex2D(tex_Colormap, oTex).rgb, clamp(viewdepth, 0.0, 1.0));
	
	OutputColor0(oColor);
}
