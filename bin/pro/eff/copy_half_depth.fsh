psin MEDIUMP vec2 oUV;
uniform MEDIUMP sampler2D tex_Depthmap;
uniform MEDIUMP vec4 c_DepthParam;

HIGHP float getlinerdepth(in MEDIUMP sampler2D sdepth,in MEDIUMP vec2 UV)
{
   HIGHP float zvalue = tex2D( sdepth , UV ).x;
   zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
   return zvalue;
}

void main(void)
{
	HIGHP float depth = getlinerdepth(tex_Depthmap, oUV) / c_DepthParam.w;
	
	OutputColor0(vec4(depth, 0.0, 0.0, 1.0));
}