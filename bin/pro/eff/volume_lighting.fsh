uniform MEDIUMP sampler2D tex_Depthmap;
uniform MEDIUMP vec4 c_BlurPixelSize;
uniform MEDIUMP vec4 c_DepthParam;

psin MEDIUMP vec2 oPos;

HIGHP float getlinerdepth(in MEDIUMP sampler2D sdepth, in MEDIUMP vec2 UV)
{
   HIGHP float zvalue = tex2D(sdepth , UV).x;
   zvalue =  1.0 / (c_DepthParam.x - zvalue * c_DepthParam.y);
   return zvalue;
}

void main(void)
{
	vec2 tex_uv = oPos;
	
#ifdef USE_HALF_DEPTH
	HIGHP float depth = tex2D(tex_Depthmap , tex_uv).x;
#else
	HIGHP float depth = getlinerdepth(tex_Depthmap, tex_uv) / c_DepthParam.w;
#endif
	
	OutputColor0(vec4(depth, depth, depth, 1.0 - depth));
}