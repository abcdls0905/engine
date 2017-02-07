#extension GL_OES_standard_derivatives : enable

#ifdef MASK
#else
varying mediump vec4 oViewScale;
varying mediump vec4 oScreenPos;

uniform mediump sampler2D tex_DepthMap;
uniform mediump sampler2D tex_ShadowMap;

uniform mediump mat4 c_mtxLightVP;
uniform mediump vec4 c_DepthBiasAndSlope;
uniform mediump vec4 c_PixelSize;
uniform mediump vec4 c_ShadowParam;
#endif

#ifdef MASK
#else
mediump float calculate_shadow(in mediump sampler2D shadowmap, in mediump vec4 coord, in mediump float curdepth)
{
	mediump float mindepth = texture2D(shadowmap, coord.xy).x * 1024.0;

	mediump float val = 0.0;
	if (mindepth >= curdepth)
		val = 1.0;

	return val;
}

mediump float getlinerdepth(in mediump sampler2D sdepth,in mediump vec2 UV)
{
   mediump float zvalue = texture2D(sdepth , UV).x;
   zvalue =  1.0 / (c_ShadowParam.z - zvalue * c_ShadowParam.w);
   return zvalue;
}

mediump float DecodeFloatRGB(mediump vec3 rgb) 
{
	return dot(rgb, vec3(1.0, 1.0/255.0, 1.0/65025.0));
}

mediump vec4 EncodeFloatRGB(mediump float v) 
{
	mediump vec4 enc = vec4(1.0, 255.0, 65025.0,16581375.0) * v;
	enc = fract(enc);
	enc.xyz -= enc.yzw * vec3(1.0 / 255.0, 1.0/255.0, 1.0/255.0);
    return enc;
}

mediump vec4 final_shadow_color(in mediump float shadow, in mediump float distance)
{
	mediump float v = clamp((1.0 - shadow) * c_ShadowParam.x, 0.0, 1.0);
	mediump float r = clamp(distance / c_ShadowParam.y, 0.0, 1.0);
		
	mediump float fShadow = 1.0 - v * (1.0 - r * r);
	return EncodeFloatRGB(fShadow);
}
#endif

void main(void)
{
#ifdef MASK
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
#else
	mediump vec2 tex_uv = oScreenPos.xy * c_PixelSize.xy;

	mediump float depth_value = getlinerdepth(tex_DepthMap, tex_uv);
	mediump vec4 pos = vec4(oViewScale.xyz / oViewScale.z * depth_value, 1.0);

	mediump vec4 projcoord = c_mtxLightVP * pos;
	mediump vec2 smcoord = (projcoord.xy / projcoord.w + 1.0) * 0.5;
	
	smcoord.y = 1.0 - smcoord.y;
	
	mediump float depth = projcoord.z;

	depth -= c_DepthBiasAndSlope.x + c_DepthBiasAndSlope.y * (abs(dFdx(oScreenPos)) + abs(dFdy(oScreenPos))).x;

	mediump vec4 coord = vec4(smcoord, 0.0, 0.0);
	mediump float shadow = calculate_shadow(tex_ShadowMap, coord, depth);
	
	gl_FragColor = final_shadow_color(shadow, pos.z);
#endif
}