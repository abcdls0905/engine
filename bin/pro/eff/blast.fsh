uniform MEDIUMP sampler2D tex_ColorMap;
uniform MEDIUMP vec4 c_fBlast;
uniform MEDIUMP vec4 c_PixelSize;
uniform MEDIUMP vec4 c_BlastColor;

psin MEDIUMP vec2 oLightProjPos;
psin MEDIUMP vec2 oVertexProjPos;

void main(void)
{
	vec4 oColor = vec4(0.0);
	vec2 screen_tex = gl_FragCoord.xy * c_PixelSize.xy;
	vec2 vecVertexToLightProj = oLightProjPos - oVertexProjPos;
	
	// y direction is up in gl screen space
	//vecVertexToLightProj = vec2(vecVertexToLightProj.x, -vecVertexToLightProj.y);
	vecVertexToLightProj = vec2(vecVertexToLightProj.x, vecVertexToLightProj.y);

	float len = length(vecVertexToLightProj);
	
	vecVertexToLightProj *= pow(clamp(len, 0.000001, 1.0), c_fBlast.y) / len;
	
	vec2 step = vecVertexToLightProj * c_fBlast.x;

	for (int i = 0; i < 4; i++)
	{
		vec2 texcoord = screen_tex + step * (float(i) - 1.0);

		oColor += tex2D(tex_ColorMap, texcoord) 
			* (0.4 - float(i) * 0.1);
	}
	
	OutputColor0(oColor);
}