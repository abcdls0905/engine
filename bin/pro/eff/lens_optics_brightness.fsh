psin MEDIUMP vec2 oPos;

uniform MEDIUMP sampler2D tex_Depthmap;
uniform MEDIUMP float c_SunSize;
uniform MEDIUMP vec4 c_DepthParam;
uniform MEDIUMP vec4 c_LightProjPos;
uniform MEDIUMP vec2 c_ScreenWidthHeight;

HIGHP float getlinerdepth(in MEDIUMP sampler2D sdepth, in MEDIUMP vec2 UV)
{
   HIGHP float zvalue = tex2D(sdepth , UV).x;
   zvalue =  1.0 / (c_DepthParam.x - zvalue * c_DepthParam.y);
   return zvalue;
}

void main(void)
{
	// y direction is up in gl screen space 
	vec2 sun_pos = (c_LightProjPos.xy + vec2(1.0)) * 0.5;
	
#ifdef IOS_METAL
	// but y direction is down in metal screen space...
	sun_pos.x = 1.0 - sun_pos.x;
	sun_pos.y = 1.0 - sun_pos.y;
#endif
	vec2 tex_scale = 1.0 / c_ScreenWidthHeight;
	
	HIGHP float accum = 0.0;
	
	vec2 coord = sun_pos - tex_scale * 3.0;
	
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			HIGHP float depth = getlinerdepth(tex_Depthmap, coord.xy + vec2(float(i) * tex_scale.x, float(j) * tex_scale.y));

			accum += clamp(depth - (c_DepthParam.w - 1.0), 0.0, 1.0);
		}
	}
	
	accum /= 36.0;
	
	OutputColor0(vec4(accum));
}