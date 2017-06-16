uniform MEDIUMP float c_Strength;

psin MEDIUMP vec2 uv;

void main(void)
{
	vec2 vPos = vec2(uv * 2.0 - 1.0);
	float fHit = sqrt(clamp(1.0 - dot(vPos, vPos), 0.0, 1.0));
	float hits = fHit * c_Strength;
	
	OutputColor0(vec4(hits, hits, hits, hits));	
}