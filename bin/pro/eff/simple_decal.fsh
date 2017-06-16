// simple decal fragment shader
uniform MEDIUMP vec4 u_vBorderColor;
uniform MEDIUMP vec4 u_vDiffuseColor;
uniform MEDIUMP sampler2D u_sDiffuseSampler;

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

psin MEDIUMP vec2 v_vTexCoord;

void main(void)
{
	if (v_vTexCoord.x > 1.0
		|| v_vTexCoord.y > 1.0
		|| v_vTexCoord.x < 0.0
		|| v_vTexCoord.y < 0.0 
		)
	{
		vec4 color = u_vBorderColor;
		
		#ifdef LUM_SCALE
			color.xyz *= c_fInvLumScale;
		#endif
	
		OutputColor0(color);
	}
	else
	{
		vec4 color = tex2DGamma(u_sDiffuseSampler, v_vTexCoord) * u_vDiffuseColor;
		
		#ifdef LUM_SCALE
			color.xyz *= c_fInvLumScale;
		#endif
		
		OutputColor0(color);
	}
}
