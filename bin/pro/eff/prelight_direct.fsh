uniform MEDIUMP vec3 c_LightDiffuse;
uniform HIGHP vec3 c_vLightDir;
uniform HIGHP mat4 c_mtxView;
uniform MEDIUMP float c_ShadowParam;
uniform MEDIUMP float c_fShadowDistance;
uniform MEDIUMP vec4 c_farDistance;
uniform HIGHP mat4 c_mtxLightViewProj1;
uniform HIGHP mat4 c_mtxLightViewProj2;
uniform HIGHP mat4 c_mtxLightViewProj3;
uniform HIGHP mat4 c_mtxLightViewProj4;
uniform MEDIUMP vec2 c_PixelSize;
uniform MEDIUMP vec4 c_LightAmbient;

uniform HIGHP sampler2D tex_Depth;
uniform HIGHP sampler2D tex_Normal;
uniform MEDIUMP sampler2D tex_Specular;

#ifdef USE_PCF
	uniform HIGHP sampler2DArrayShadow tex_Shadow1;
#else
	uniform HIGHP sampler2DArray tex_Shadow1;
#endif


psin vec2 oUV;
psin vec4 oViewScale;

#ifdef ES3
	layout(location = 1) out HIGHP vec4 FragColor1;
	void OutputColor1(HIGHP vec4 color)
	{
		FragColor1 = color;
	}
#endif//ES3

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
	
#ifdef LIGHTMAP
	diffuse = vec3(0.0, 0.0, 0.0);
#else
	float diffuseTerm = DiffuseBRDF(nor, view_dir, to_light, specular_power, dotNL);
	diffuse = (diffuseTerm ) * light_color.rgb * diffuse_occlusion;
#endif
	
	// specular_color为0的材质不开启高光
	specular = any(bvec3(specular_color.x > 0.0, specular_color.y > 0.0, specular_color.z > 0.0)) 
		? PhysicalBRDF(nor, view_dir, to_light, specular_power, specular_color) * dinten * light_color.rgb * specular_occlusion : vec3(0.0, 0.0, 0.0);
	
	// Simple energy conservation between specular and diffuse
#ifndef LIGHTMAP
	diffuse *= clamp(1.0 - GetLuminance(specular_color), 0.0, 1.0);  // Sat required to handle precision issues
#endif
}

// direct light

#ifdef SHADOW
	#ifdef USE_PCF
		HIGHP float tex2DProj(HIGHP sampler2DShadow tex, HIGHP vec4 pos)
		{
			#ifdef ES2
				return shadow2DProjEXT(tex, pos);
			#else
				return textureProj(tex, pos);
			#endif//ES2
		}
		
		float shadow_lookup(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float x, float y, float shadowmap_index)
		{
			HIGHP vec4 offset = vec4 ( x * c_PixelSize.x * shadow_pos.w,
		                        y * c_PixelSize.y * shadow_pos.w,
		                        0.0, 0.0);
			shadow_pos = shadow_pos + offset;
			return texture(tex, vec4(shadow_pos.xy, shadowmap_index, shadow_pos.z));
		}
	#endif

	MEDIUMP vec4 final_shadow_color(in MEDIUMP float shadow, in HIGHP float distance)
	{
		float v = clamp((1.0 - shadow) * c_ShadowParam, 0.0, 1.0);
		float r = clamp(distance / c_fShadowDistance, 0.0, 1.0);
		return vec4(1.0 - v * (1.0 - r * r), 0.5, 1.0, 1.0);
	}
	

	#ifdef USE_PCF
	float calculate_shadow(HIGHP sampler2DArrayShadow tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#else
	float calculate_shadow(HIGHP sampler2DArray tex, HIGHP vec4 shadow_pos, float shadowmap_index)
	#endif
	{
		#ifdef USE_PCF
			HIGHP float sum = shadow_lookup(tex, shadow_pos, -0.5, -0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, -0.5, 0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, 0.5, 0.5, shadowmap_index);
			sum += shadow_lookup(tex, shadow_pos, 0.5, -0.5, shadowmap_index);
			sum = sum * 0.25;
			return sum;
		#else
			HIGHP float shadow_depth = texture(tex, vec3(shadow_pos.xy, shadowmap_index)).x;
			return (shadow_depth < shadow_pos.z) && (shadow_depth < 1.0) ? 0.0 : 1.0;
		#endif
	}
#endif

void main(void)
{
	vec2 tex_uv = oUV;
	HIGHP float depth_tex = tex2D(tex_Depth, tex_uv).x;
	
// #ifdef STEREO
	// float convergence = c_fStereoConvergence; //c_PixelSize.z;
	// vec2 screen_xy = tex_uv * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	// vec4 pos_proj = vec4(screen_xy * input.iScreenPos.w, input.iScreenPos.zw);
	// vec3 convergence_pos = mul(pos_proj, c_mtxProjInverse).xyz;
	
	// convergence_pos *= convergence / convergence_pos.z;
	
	// vec3 view_dir = oViewScale.xyz - convergence_pos;
	// vec3 view_pos = convergence_pos + view_dir / view_dir.z * (depth_tex * 1024.0 - convergence);
// #else
	HIGHP vec3 view_pos = oViewScale.xyz / oViewScale.z * depth_tex * 1024.0;
// #endif

	float shadow = 1.0;
#if defined(SHADOW) && !defined(LIGHTMAP)
	HIGHP float shadow_depth = 1.0;
	
	#if !defined(PSSM2) && !defined(PSSM3) && !defined(PSSM4)
		vec4 shadow_pos = c_mtxLightViewProj1 * vec4(view_pos, 1.0);
		shadow_depth = calculate_shadow(tex_Shadow1, shadow_pos, 0.0);
	#else
		vec4 shadow_distance_comparison = step(c_farDistance, vec4(view_pos.z));
		float shadowmap_index = dot(vec4(1.0,
			#ifdef PSSM2
				1.0,
			#else
				0.0,
			#endif
			
			#ifdef PSSM3
				1.0,
			#else
				0.0,
			#endif
			
			#ifdef PSSM4
				1.0
			#else
				0.0
			#endif
			),shadow_distance_comparison);
		
		vec4 vec4_view_pos = vec4(view_pos, 1.0);
		#ifdef PSSM4
			vec4 shadow_pos_array[4] = vec4[](c_mtxLightViewProj1 * vec4_view_pos,
				c_mtxLightViewProj2 * vec4_view_pos,
				c_mtxLightViewProj3 * vec4_view_pos,
				c_mtxLightViewProj4 * vec4_view_pos);
		#elif defined(PSSM3)
			vec4 shadow_pos_array[3] = vec4[](c_mtxLightViewProj1 * vec4_view_pos,
				c_mtxLightViewProj2 * vec4_view_pos,
				c_mtxLightViewProj3 * vec4_view_pos);
		#elif defined(PSSM2)
			vec4 shadow_pos_array[2] = vec4[](c_mtxLightViewProj1 * vec4_view_pos,
				c_mtxLightViewProj2 * vec4_view_pos);
		#else
			vec4 shadow_pos_array[1] = vec4[](c_mtxLightViewProj1 * vec4_view_pos);
		#endif

		HIGHP vec4 shadow_pos = shadow_pos_array[int(shadowmap_index)];
		shadow_depth = calculate_shadow(tex_Shadow1, shadow_pos, shadowmap_index);
	#endif
			
	vec4 shadow_inten = final_shadow_color(shadow_depth, view_pos.z);

	shadow = shadow_inten.x;
#endif

	vec3 light_dir = normalize((c_mtxView * vec4(c_vLightDir, 0.0)).xyz);
	vec3 view_dir = normalize(view_pos*(-1.0));

	vec3 diffuse;
	vec3 specular;
	calc_light(diffuse, specular, tex_uv, -normalize(oViewScale.xyz), light_dir, c_LightDiffuse.rgb, shadow);

#if !defined(LIGHTMAP)
	diffuse += c_LightAmbient.rgb;
#endif	

	OutputColor0(vec4(diffuse, 1.0));
	
	#ifdef ES3
		OutputColor1(vec4(specular, 1.0));
	#endif
}

