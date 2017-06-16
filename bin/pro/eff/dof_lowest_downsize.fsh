uniform MEDIUMP sampler2D tex_Colormap;
uniform HIGHP vec2 c_vTexelOffset;

psin HIGHP vec2 oTex;

void main(void)
{
	vec4 oColor = vec4(0.0, 0.0, 0.0, 1.0);

#if defined(QUARTER_FILTER)	
	// quarter pixel color
	oColor = tex2D(tex_Colormap, oTex);
#else
	// sixteenth pixel color
	oColor.rgb = tex2D(tex_Colormap, oTex.xy + c_vTexelOffset.xy).rgb;
	oColor.rgb += tex2D(tex_Colormap, oTex.xy - c_vTexelOffset.xy).rgb;
	oColor.rgb += tex2D(tex_Colormap, oTex.xy + vec2(c_vTexelOffset.x, -c_vTexelOffset.y)).rgb;
	oColor.rgb += tex2D(tex_Colormap, oTex.xy + vec2(-c_vTexelOffset.x, c_vTexelOffset.y)).rgb;
	
	oColor.rgb *= 0.25;
#endif
	
	OutputColor0(oColor);
}