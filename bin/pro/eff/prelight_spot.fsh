psin HIGHP vec4 oScreenPos;
psin HIGHP vec4 oViewScale;
psin HIGHP vec4 oPos;

#ifdef ES3
	layout(location = 1) out HIGHP vec4 FragColor1;
	void OutputColor1(HIGHP vec4 color)
	{
		FragColor1 = color;
	}
#endif//ES3

uniform HIGHP vec3 c_vLightPos;
uniform MEDIUMP float c_fLightRange;
uniform MEDIUMP float c_fSpecularInten;
uniform MEDIUMP vec4 c_LightColor;
uniform MEDIUMP vec3 c_Attenu;
uniform MEDIUMP vec3 c_SpotParam;
uniform MEDIUMP vec3 c_vLightDir1;
uniform MEDIUMP mat4 c_mtxLightVP;

uniform HIGHP sampler2D tex_Depth;
uniform HIGHP sampler2D tex_Normal;
uniform MEDIUMP sampler2D tex_Specular;
uniform MEDIUMP sampler2D tex_Overlay;

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

void decode_normal_translucency(in MEDIUMP sampler2D normal_map, in vec2 uv, 
	out vec3 nor, out float translucency)
{
	vec4 normal_tex = tex2D(normal_map, uv);
	DecodeNormalTranslucency(normal_tex, nor, translucency);
}

MEDIUMP vec3 PhysicalBRDF(vec3 N, vec3 V, vec3 L, float Gloss, vec3 SpecCol)
{
	MEDIUMP vec3 H = normalize( V + L );
	
	// Compute perceptually linear roughness parameter (squared)
	MEDIUMP float m = pow( (1.0 - Gloss * 0.7), 6.0 );
	MEDIUMP float m2 = m * m;
	
	// Prevent highlights from getting too tiny since we don't have real area lights yet
	m2 = max( m2, 0.00001 );
	
	// GGX NDF
	MEDIUMP float NdotH = clamp( dot( N, H ), 0.0, 1.0 );
	MEDIUMP float spec = (NdotH * NdotH) * (m2 - 1.0) + 1.0;
	spec = m2 / (spec * spec);
	
	// Schlick-Smith Visibility Term
	MEDIUMP float k = (0.8 + m * 0.5) * (0.8 + m * 0.5) / 2.0;
	MEDIUMP float Gv = clamp( dot( N, V ), 0.0, 1.0 ) * (1.0 - k) + k;
	MEDIUMP float Gl = clamp( dot( N, L ), 0.0, 1.0 ) * (1.0 - k) + k;
	spec /= max( Gv * Gl, 1e-6 );
		
	// Schlick approximation for Fresnel
	MEDIUMP vec3 fresnel = mix( SpecCol, vec3(1.0, 1.0, 1.0), pow( (1.0 - dot(L, H)) * max(Gloss, 0.7), 5.0 ) ); // 乘以光泽度max(Gloss,0.7)是避免迎光平视时高光过亮的问题，模拟粗糙材质的效果

	return (fresnel * spec) / 4.0;
}

MEDIUMP float DiffuseBRDF(vec3 N, vec3 V, vec3 L, float Gloss, float NdotL)
{
	#define PI 3.1415926
	// TODO: Merge with Specular BRDF to save a few instructions

	Gloss = max(Gloss, 0.7); // 避免光泽度低的材质出现迎光和背光面亮度相差过大的情况，尤其是地表
	
	MEDIUMP float m = pow( (1.0 - Gloss * 0.7), 6.0 );
	
	// Approximation of the full quality Oren-Nayar model
	MEDIUMP float s = dot(L, V) - dot(N, L) * dot(N, V);
	MEDIUMP float t = s <= 0.0 ? 1.0 : max(max(dot(N, L), dot(N, V)), 1e-6);
	MEDIUMP float A = 1.0 / (1.0 + (0.5 - 2.0 / (3.0 * PI)) * m);
	MEDIUMP float B = m * A;
	
	return NdotL * max(A + B * (s / t), 0.0);
}

MEDIUMP float GetLuminance(MEDIUMP vec3 color)
{
	// This seems to work better (less same luma ghost trails).
	// CCIR 601 function for luma.
	return dot(color, vec3(0.299, 0.587, 0.114));

	// Rec 709 function for luma.
	//return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void calc_light(out vec3 diffuse, out vec3 specular, in vec2 tex_uv, 
	in vec3 view_dir, in vec3 to_light, in vec3 light_color, in float shadow)
{
	light_color *= shadow;
	
	vec3 nor;
	float translucency;
	decode_normal_translucency(tex_Normal, tex_uv, nor, translucency);

	vec4 specular_tex = tex2D(tex_Specular, tex_uv);

	vec3 specular_color = DecodeSpecColor( specular_tex.gba, translucency == 0.0 );
	specular_color = DECODE_TO_LINEAR(specular_color);
	float specular_power = specular_tex.r;

	vec3 half_dir = normalize(to_light + view_dir);
	
	float dotNL = clamp(dot(nor, to_light), 0.0, 1.0);
	float dotNE = clamp(dot(nor, view_dir), 0.0, 1.0);
	float dotVH = clamp(dot(view_dir, half_dir), 0.0, 1.0);
	float dotNH = clamp(dot(nor, half_dir), 0.0, 1.0);
	float dinten = dotNL;

	float diffuseTerm = DiffuseBRDF(nor, view_dir, to_light, specular_power, dotNL);
	
	float diffuse_occlusion = 1.0;
	float specular_occlusion = 1.0;
// #ifdef SSDO
	// vec4 ssdo_tex = tex_SSDO.Sample(sam_PointClamp, tex_uv);
	// ssdo_tex.xyz = ssdo_tex.xyz * 2.0 - 1.0;
	// ssdo_tex.xyz *= SH2_WEIGHT_L1;
	// diffuse_occlusion = max(1.0 - clamp(dot(to_light, ssdo_tex.xyz), 0.0, 1.0) * c_vSSDOFactorParams.y), 0.3);
	// specular_occlusion = max(1.0 - clamp(dot(half_dir, ssdo_tex.xyz) * c_vSSDOFactorParams.y), 0.0, 1.0), 0.3);
// #endif
	
	//vec3 transmittance = ThinTranslucency(nor, to_light, DecodeTransmittanceColor(vec3(translucency, specular_tex.zw)));
	
	diffuse = (diffuseTerm ) * light_color.rgb * diffuse_occlusion;
	
	// specular_color为0的材质不开启高光
	specular = any(bvec3(specular_color.x > 0.0, specular_color.y > 0.0, specular_color.z > 0.0)) 
		? PhysicalBRDF(nor, view_dir, to_light, specular_power, specular_color) * dinten * light_color.rgb * specular_occlusion : vec3(0.0, 0.0, 0.0);
	
	// Simple energy conservation between specular and diffuse
	diffuse *= clamp(1.0 - GetLuminance(specular_color), 0.0, 1.0);  // Sat required to handle precision issues
}

void main(void)
{
	vec2 tex_uv = (oPos.xy / oPos.w + vec2(1.0, 1.0)) * vec2(0.5, 0.5);
#ifdef IOS_METAL
	tex_uv.y = 1.0 - tex_uv.y;
#endif
	HIGHP float depth_tex = tex2D(tex_Depth, tex_uv).x;
	
// #ifdef STEREO
	// float convergence = c_fStereoConvergence; 
	// vec2 screen_xy = tex_uv * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	// vec4 pos_proj = vec4(screen_xy * input.iScreenPos.w, input.iScreenPos.zw);
	// vec3 convergence_pos = mul(pos_proj, c_mtxProjInverse).xyz;
	
	// convergence_pos *= convergence / convergence_pos.z;
	
	// vec3 view_dir = oViewScale.xyz - convergence_pos;
	// vec3 view_pos = convergence_pos + view_dir / view_dir.z * (depth_tex * 1024.0 - convergence);
// #else
	vec3 view_pos = oViewScale.xyz / oViewScale.z * depth_tex * 1024.0;
// #endif

	HIGHP vec3 to_light = c_vLightPos - view_pos;
	HIGHP float dist = length(to_light);
	
	// normalized
	to_light /= dist;
	
	// L(light direction vector) * D(direction to vertex)
	float rho = dot(to_light, c_vLightDir1);
	
	// clip by distance
	if ((dist > c_fLightRange) || (rho < c_SpotParam.x))
	{
		OutputColor0(vec4(0.0, 0.0, 0.0, 0.0));
		#ifdef ES3
			OutputColor1(vec4(0.0, 0.0, 0.0, 0.0));
		#endif
		
		return;
	}
	
	// attenuation
	HIGHP float d = dist / c_fLightRange;
	
#ifdef ATTENU2
	HIGHP float attenu = max(0.0, 1.0 - (c_Attenu.y * d + c_Attenu.z * d * d));
#else
	HIGHP float attenu = max(0.0, 1.0 - c_Attenu.y * d);
#endif

	// intensity: pow((L * D - cos(phi / 2)) / (cos(theta / 2) - cos(phi / 2)), falloff)
	float inten = pow(max((rho - c_SpotParam.x) / c_SpotParam.y, 0.000001), c_SpotParam.z) * attenu;

	float shadow = 1.0;
// #ifdef SHADOW	
	// shadow = calc_shadow(view_pos);
// #endif

	vec4 light_color = c_LightColor;

#ifdef OVERLAY
	//#if !SHADOW
		vec4 tc = vec4(view_pos, 1.0) * c_mtxLightVP;
		tc.xy = (tc.xy / tc.w + 1.0) * 0.5;
		//tc.y = 1.0 - tc.y;
	//#endif
	light_color *= tex2D(tex_Overlay, tc.xy);
#endif

	vec3 diffuse;
	vec3 specular;
	calc_light(diffuse, specular, tex_uv, -normalize(oViewScale.xyz), to_light, inten * light_color.rgb, shadow);
	
	OutputColor0(vec4(diffuse, 1.0));
	#ifdef ES3
		OutputColor1(vec4(specular * c_fSpecularInten, 1.0));
	#endif
}
