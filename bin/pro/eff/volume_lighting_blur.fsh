uniform MEDIUMP sampler2D tex_ColorMapLighting;
uniform MEDIUMP vec4 c_vSunScreenPos;
uniform MEDIUMP float c_fAttenuation;
uniform MEDIUMP float c_fViewportRatio;
uniform MEDIUMP vec4 c_BlurPixelSize;
uniform MEDIUMP float c_fBlurRadius;

psin MEDIUMP vec2 oPos;

void main(void)
{
	vec4 oColor;
	
	vec2 tex_uv = oPos;
	
	vec2 sunPosProj = c_vSunScreenPos.xy;
	
	vec2 sunVec = sunPosProj.xy - tex_uv;
  
	float fAspectRatio = 1.333 / c_fViewportRatio;
  
	float sunDist = 1.0 - clamp(length(sunVec * vec2(1.0, fAspectRatio)) * c_fAttenuation, 0.0, 1.0);

	vec2 sunDir = sunPosProj.xy - tex_uv;
  
	float accum = 0.0;
	sunDir.xy *= c_fBlurRadius;
  
	float depth = 0.0;
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 0.0).x;
	accum += depth * (1.0 - 0.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 1.0).x;
	accum += depth * (1.0 - 1.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 2.0).x;
	accum += depth * (1.0 - 2.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 3.0).x;
	accum += depth * (1.0 - 3.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 4.0).x;
	accum += depth * (1.0 - 4.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 5.0).x;
	accum += depth * (1.0 - 5.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 6.0).x;
	accum += depth * (1.0 - 6.0 / 8.0);
	
	depth = tex2D(tex_ColorMapLighting, tex_uv + sunDir.xy * 7.0).x;
	accum += depth * (1.0 - 7.0 / 8.0);
	
	accum /= float(8.0);

	oColor = accum * 2.0 * vec4(sunDist, sunDist, sunDist, 1.0);
	
	OutputColor0(oColor);
}
