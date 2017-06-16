// dizzy fragment shader

//#define FOCUS

uniform MEDIUMP vec4 u_vParam;
uniform MEDIUMP vec2 u_vCenter;
uniform MEDIUMP sampler2D u_sDiffuseSampler;

psin MEDIUMP vec2 v_vTexCoord;

void main(void)
{
	float fRotationalRadius = u_vParam.x;

#if defined(FOCUS)
	fRotationalRadius *= max(0.0, length(v_vTexCoord - u_vCenter) - 0.1);
#endif

	float fRadian = u_vParam.y;
	float fDistortInten = u_vParam.z;

	vec2 bias = vec2(sin(fRadian), cos(fRadian));
	
	// 中心距离
	float r = length(v_vTexCoord - 0.5) + 0.000001;
	// 方向向量
	vec2 dir = (v_vTexCoord - 0.5) / r;
	// 扭曲，需二次方或以上
	vec2 v = v_vTexCoord + (r * r) * dir * fDistortInten;

	vec4 oColor = tex2D(u_sDiffuseSampler,
		v + bias * fRotationalRadius);
	oColor += tex2D(u_sDiffuseSampler,
		v - bias * fRotationalRadius);
	oColor *= 0.5;
	
	OutputColor0(oColor);
}

