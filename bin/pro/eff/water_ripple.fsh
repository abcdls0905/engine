uniform MEDIUMP sampler2D tex_Ripple;
uniform MEDIUMP float c_TexSize;

psin MEDIUMP vec2 uv;

void main(void)
{
	vec4 color = vec4(0.0);

	vec4 h0 = tex2D(tex_Ripple, uv);
	
	HIGHP float fTexelOffset = 1.0 / c_TexSize;

	vec4 h00 = tex2D(tex_Ripple, uv + vec2(-fTexelOffset, 0.0));
	vec4 h10 = tex2D(tex_Ripple, uv + vec2(fTexelOffset, 0.0));
	vec4 h01 = tex2D(tex_Ripple, uv + vec2(0.0, -fTexelOffset));
	vec4 h11 = tex2D(tex_Ripple, uv + vec2(0.0, fTexelOffset));
	
	float fAcce = (h00.z + h10.z + h01.z + h11.z) * 0.25 - h0.z;

	vec3 vVerletWeights = vec3(1.995, 0.995, 1.0);
	float cCurrHeight = (vVerletWeights.x * h0.z - vVerletWeights.y * h0.w + vVerletWeights.z * fAcce);

	color.z = cCurrHeight;

	color.w = h0.z;
	
	HIGHP float fHalfTexelOffset = 0.5 / c_TexSize;

	h0 = tex2D(tex_Ripple, uv + vec2(fHalfTexelOffset));
	h10 = tex2D(tex_Ripple, uv + vec2(fTexelOffset, 0.0) + vec2(fHalfTexelOffset));
	h11 = tex2D(tex_Ripple, uv + vec2(0.0, fTexelOffset) + vec2(fHalfTexelOffset));

	vec3 vWeights = vec3(h0.w, h10.w, h11.w);
	vec3 vNormal = vec3(-(vWeights.x - vWeights.y), -(vWeights.x - vWeights.z), 0.05);
	vNormal = normalize(vNormal.xyz);
 
	color.xy = vNormal.xy * 0.5 + 0.5;
	
	vec4 clampEdge = vec4(0.5, 0.5, 0.0, 0.0);
	
	color.xyzw = (uv.x < fHalfTexelOffset * 4.0) ? clampEdge.xyzw : color.xyzw;
	color.xyzw = (uv.y < fHalfTexelOffset * 4.0) ? clampEdge.xyzw : color.xyzw;
	color.xyzw = (uv.x > 1.0 - fHalfTexelOffset * 4.0) ? clampEdge.xyzw : color.xyzw;
	color.xyzw = (uv.y > 1.0 - fHalfTexelOffset * 4.0) ? clampEdge.xyzw : color.xyzw;
	
	OutputColor0(color);	
}