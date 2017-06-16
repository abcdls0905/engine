
psin HIGHP vec4 oViewScale;
psin HIGHP vec4 oScreenPos;

uniform MEDIUMP vec4 c_vAr;
uniform MEDIUMP vec4 c_vAg;
uniform MEDIUMP vec4 c_vAb;
uniform MEDIUMP vec4 c_vBr;
uniform MEDIUMP vec4 c_vBg;
uniform MEDIUMP vec4 c_vBb;
uniform MEDIUMP vec4 c_vC;

uniform HIGHP float c_fAttenu;
uniform HIGHP float c_fEnvMapLevel;
uniform HIGHP float c_fRefectInten;
uniform HIGHP vec2 c_PixelSize;
uniform HIGHP vec3 c_vViewPos;
uniform HIGHP vec3 c_vColor;
uniform HIGHP mat4 c_mtxLightVP;
uniform MEDIUMP sampler2D tex_DepthMap;
uniform MEDIUMP sampler2D tex_NormalMap;
uniform MEDIUMP sampler2D tex_Specular;
uniform MEDIUMP sampler2D tex_EnvBrdf;
uniform MEDIUMP samplerCube tex_CubeMap;
uniform MEDIUMP samplerCube tex_IrrCubeMap;

#ifdef ES3
	layout(location = 1) out HIGHP vec4 SpecularColor;
#endif //ES3

vec3 MapCubeToSphere(vec3 pos)
{
	vec3 pos2 = pos.xyz * pos.xyz;
	return pos * sqrt(vec3(1.0, 1.0, 1.0) - 0.5 * pos2.yzx - 0.5 * pos2.zxy + 0.333 * pos2.yzx * pos2.zxy);
}

// assumes 0 is min
float smoothstep_opt(float maxi, float x)
{
	x = clamp(x / maxi, 0.0, 1.0);
	return x * x * (3.0 - 2.0 * x);
}

float GetAttenuation(vec3 L, float fInvRadius, bool bUserFallOff, float fFallOffMax)
{
	vec3 vDist = L * fInvRadius;
	float fFallOff = clamp(1.0 + dot(vDist, -vDist), 0.0, 1.0);
	
	if (bUserFallOff)
	{
		fFallOff = smoothstep_opt(fFallOffMax, fFallOff);
	}
	
	return fFallOff;
}

float GetGlossiness(float specular_power)
{
	return clamp(specular_power, 0.0, 1.0);
}

float GetRoughness(float specular_power)
{
	return max(1.0 - GetGlossiness(specular_power), 0.02);
}

float ComputeReflectionCaptureMipFromRoughness(float roughness, float mip_level_count)
{
	return mip_level_count * roughness;
}

vec3 GetEnvBRDFFresnel(sampler2D tex_env_brdf, vec3 specular_color, float gloss, float dotNE)
{
	vec2 envBRDF = tex2DBias(tex_env_brdf, vec2(dotNE, gloss), 0.0).xy;
	
	return mix(envBRDF.xxx, envBRDF.yyy, specular_color);
}

vec3 DecodeSpecColor( vec3 encodedSpecCol, const bool useChrominance )
{
	vec3 specCol;
	
	// Y'CbCr
	encodedSpecCol.yz -= 127.0/255.0;
	if (!useChrominance) 
	{
		encodedSpecCol.yz = vec2(0.0);
	}
	specCol.r = encodedSpecCol.x + 1.402 * encodedSpecCol.z;
	specCol.g = dot( vec3( 1.0, -0.3441, -0.7141 ), encodedSpecCol.xyz );
	specCol.b = encodedSpecCol.x + 1.772 * encodedSpecCol.y;

	return specCol * specCol;
}

vec3 DecodeTransmittanceColor(vec3 encodeCol)
{
	return encodeCol.x > 0.0 ? DecodeSpecColor(encodeCol, true) : vec3(0.0, 0.0, 0.0);
}

vec3 DecodeNormal(vec3 e)
{
   e *= vec3(2.0, 2.0, 1.5);
   e.xy += vec2(floor(e.z), fract(e.z)) / vec2(1023.0, 511.5) - 1.0;
   vec3 v = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
   vec2 signNotZero = vec2(e.x > 0.0, e.y > 0.0) * 2.0 - 1.0;
   if (v.z < 0.0) v.xy = (1.0 - abs(e.yx)) * signNotZero;
   return normalize(-v);
}

void DecodeNormalTranslucency(in vec4 normal_tex, out vec3 normal, out float translucency)
{
	normal.xyz = DecodeNormal(normal_tex.xyw);
	translucency = normal_tex.z;
}

vec3 ComputeSH(vec3 norWS, vec4 vAr, vec4 vAg, vec4 vAb,
	vec4 vBr, vec4 vBg, vec4 vBb, vec3 vC)
{
	vec3 x1, x2, x3;

	vec4 nor = vec4(norWS, 1.0);

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
	vec4 oColor0 = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 oColor1 = vec4(0.0, 0.0, 0.0, 0.0);
	
	vec2 tex_uv = gl_FragCoord.xy * c_PixelSize.xy;
	float depth_tex = tex2D(tex_DepthMap, tex_uv).x;

	vec3 view_pos = oViewScale.xyz / oViewScale.w * depth_tex * 1024.0;
		
	vec3 local_coord = abs(c_mtxLightVP * vec4(view_pos + c_vViewPos, 1.0)).xyz;
	float d = max(local_coord.x, max(local_coord.y, local_coord.z));
	
	if (d > 1.0)
	{
		return;
	}
		
	vec4 normal_tex = tex2D(tex_NormalMap, tex_uv);
	vec3 normal;
	float translucency;
	DecodeNormalTranslucency(normal_tex, normal, translucency);
	
	vec4 specular_tex = tex2D(tex_Specular, tex_uv);

	vec3 specular_color = DecodeSpecColor( specular_tex.gba, translucency == 0.0 );
	float specular_power = specular_tex.r;
	
	vec3 view_dir = normalize(view_pos);
	float dotNE = dot(normal, -view_dir);
		
	// 距离衰减因子
#ifdef NO_ATTENU
	float attenu_factor = 1.0;
#else
	vec3 local_coord_sphere = MapCubeToSphere(clamp(local_coord.xyz, -1.0, 1.0));
	float attenu_factor = GetAttenuation(local_coord_sphere, 1.0, true, c_fAttenu);
#endif

	float diffuse_occlusion = 1.0;
	float specular_occlusion = 1.0;
	
#ifdef NO_CUBEMAP
	oColor0 = vec4(1.0, 1.0, 1.0, 1.0);
#else
	vec3 transmittance = DecodeTransmittanceColor(vec3(translucency, specular_tex.zw));//ThinTranslucency(normal, -normal, DecodeTransmittanceColor(float3(translucency, specular_tex.zw)));
	transmittance *= 0.4;
	
	#ifdef SH
		oColor0.rgb = ComputeSH(normal, c_vAr, c_vAg, c_vAb, c_vBr, c_vBg, c_vBb, c_vC.xyz);
		oColor0.rgb += transmittance * ComputeSH(-normal, c_vAr, c_vAg, c_vAb, c_vBr, c_vBg, c_vBb, c_vC.xyz);
	#else
		oColor0 = texCubeBias(tex_IrrCubeMap, normal, 0.0);
		oColor0.rgb += transmittance * texCubeBias(tex_IrrCubeMap, -normal, 0.0).rgb;
	#endif
#endif

	oColor0.rgb = oColor0.rgb * diffuse_occlusion * c_vColor;
	oColor0.a = attenu_factor;

#ifdef NO_CUBEMAP
	vec3 reflect_color = vec3(0.0, 0.0, 0.0);
#else
	// 生成高光信息处理
	vec3 reflect_dir =	reflect(view_dir, normal);
	
	float roughness = GetRoughness(specular_power);
	float mip_level = ComputeReflectionCaptureMipFromRoughness(roughness, c_fEnvMapLevel);
	vec4 reflect_tex = texCubeBias(tex_CubeMap, reflect_dir, mip_level);
	
	float glossiness = GetGlossiness(specular_power);
	vec3 reflect_brdf = GetEnvBRDFFresnel(tex_EnvBrdf, specular_color, glossiness, dotNE);
	
	vec3 reflect_color = reflect_tex.rgb * reflect_brdf * c_vColor * c_fRefectInten;

	reflect_color *= specular_occlusion;
#endif

	oColor1.rgb = reflect_color.rgb;
	oColor1.a = attenu_factor;
}
