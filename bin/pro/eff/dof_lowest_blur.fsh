uniform MEDIUMP sampler2D tex_Colormap;
uniform HIGHP vec2 StepSize;

psin HIGHP vec2 oTex;

void main(void)
{
	MEDIUMP vec3 oColor;
	
	oColor = tex2D(tex_Colormap, vec2(oTex.x + StepSize.x, oTex.y + StepSize.y)).rgb;
	oColor += tex2D(tex_Colormap, vec2(oTex.x + StepSize.x * 2.0, oTex.y + StepSize.y * 2.0)).rgb;
	oColor += tex2D(tex_Colormap, oTex).rgb;
	oColor += tex2D(tex_Colormap, vec2(oTex.x - StepSize.x * 2.0, oTex.y - StepSize.y * 2.0)).rgb;
	oColor += tex2D(tex_Colormap, vec2(oTex.x - StepSize.x, oTex.y - StepSize.y)).rgb;
	
	OutputColor0(vec4(oColor * 0.2, 1.0));
}