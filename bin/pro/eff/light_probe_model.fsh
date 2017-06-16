
psin HIGHP vec3 oNormal;

uniform MEDIUMP vec4 c_vAr;
uniform MEDIUMP vec4 c_vAg;
uniform MEDIUMP vec4 c_vAb;
uniform MEDIUMP vec4 c_vBr;
uniform MEDIUMP vec4 c_vBg;
uniform MEDIUMP vec4 c_vBb;
uniform MEDIUMP vec4 c_vC;

uniform MEDIUMP samplerCube tex_CubeMap;
uniform MEDIUMP samplerCube tex_IrrCubeMap;

vec3 ComputeSH(vec3 norWS, vec4 vAr, vec4 vAg, vec4 vAb,
	vec4 vBr, vec4 vBg, vec4 vBb, vec3 vC)
{
	vec3 x1, x2, x3;

	vec4 nor = vec4(norWS, 1.0f);

	// Linear + constant polynomial terms
	x1.r = dot(vAr, nor);
	x1.g = dot(vAg, nor);
	x1.b = dot(vAb, nor);

	// 4 of the quadratic polynomials
	vec4 vB = nor.xyzz * nor.yzzx;
	x2.r = dot(vBr, vB);
	x2.g = dot(vBg, vB);
	x2.b = dot(vBb, vB);

	// Final quadratic polynomial
	float C = nor.x * nor.x - nor.y * nor.y;
	x3 = C * vC;

	return (x1 + x2 + x3);
}

void main(void)
{
	vec4 oColor = vec4(1.0, 1.0, 1.0, 1.0);

	vec3 normal = normalize(oNormal);

#ifdef NO_CUBEMAP
#else
	#ifdef INSPACE
		oColor = texCube(tex_CubeMap, normal);
	#else
		#ifdef SH
			oColor.rgb = ComputeSH(normal, c_vAr, c_vAg, c_vAb, c_vBr, c_vBg, c_vBb, c_vC.xyz);
		#else
			oColor = texCube(tex_IrrCubeMap, normal);
		#endif
	#endif
#endif
	
	OutputColor0(oColor);
}

