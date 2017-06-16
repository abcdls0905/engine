uniform HIGHP mat4 c_mtxViewInverse;
uniform MEDIUMP vec2 c_fEnvParam;

uniform HIGHP sampler2D tex_Depth;
uniform HIGHP sampler2D tex_Normal;
uniform MEDIUMP sampler2D tex_Specular;
uniform MEDIUMP samplerCube tex_GlobalEnvMap;

psin vec2 oUV;
psin vec4 oViewScale;

#ifdef ES3
	#ifdef FRAMEBUFFER_FETCH
		//layout(location = 0) inout HIGHP vec4 FragColor0;
		layout(location = 1) inout HIGHP vec4 Normal_Depth;
		layout(location = 3) inout HIGHP vec4 SpecularColor;

		uniform MEDIUMP float c_fInvLumScale;
	#else
		layout(location = 1) out HIGHP vec4 FragColor1;
		void OutputColor1(HIGHP vec4 color)
		{
			FragColor1 = color;
		}
	#endif
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

MEDIUMP vec3 Fresnel(vec3 N, vec3 V, vec3 SpecCol, float Gloss)
{
	return mix(SpecCol, vec3(1.0, 1.0, 1.0), pow(abs(1.0 - dot(N, V)) * max(Gloss, 0.7), 5.0));
}

MEDIUMP vec4 EnvTexUseAlpha(vec4 crTexReflect)
{
#ifdef REFLECT_HDR
	MEDIUMP float scale = exp2((crTexReflect.a-0.25)*16.0);
	return vec4(crTexReflect.xyz * scale, 1.0);
#else
	return crTexReflect;
#endif
}

void main(void)
{
	vec2 tex_uv = oUV;
#ifdef FRAMEBUFFER_FETCH
	HIGHP float depth_tex = Normal_Depth.w;
#else
	HIGHP float depth_tex = tex2D(tex_Depth, tex_uv).x * 1024.0;
#endif
	HIGHP vec3 view_pos = oViewScale.xyz / oViewScale.z * depth_tex;

	vec3 view_dir = -normalize(view_pos);
	
	vec3 nor;
#ifdef FRAMEBUFFER_FETCH
	nor = Normal_Depth.xyz;
	float translucency = 0.0;
	vec4 specular_tex = SpecularColor;
#else
	float translucency;
	decode_normal_translucency(tex_Normal, tex_uv, nor, translucency);
	
	vec4 specular_tex = tex2D(tex_Specular, tex_uv);
#endif
	
	vec3 specular_color = DecodeSpecColor( specular_tex.gba, translucency == 0.0 );
	specular_color = DECODE_TO_LINEAR(specular_color);
	float specular_power = specular_tex.r;

	vec3 reflect_dir = reflect(-view_dir, nor);
	reflect_dir = normalize((c_mtxViewInverse * vec4(reflect_dir, 0.0)).xyz);
	
	float roughness = GetRoughness(specular_power);
	float mip_level = ComputeReflectionCaptureMipFromRoughness(roughness, c_fEnvParam.y);
	
	// #if defined IOS
		// reflect_dir.y = -reflect_dir.y;
	// #endif
	
	MEDIUMP vec4 crTexReflect = EnvTexUseAlpha(texCubeLodGamma(tex_GlobalEnvMap, reflect_dir, mip_level));
	
	MEDIUMP vec3 reflect_color = (specular_power > 0.0) ? Fresnel(nor, view_dir, specular_color, specular_power) : vec3(0.0);
	
	reflect_color = crTexReflect.xyz * reflect_color * c_fEnvParam.x;
	//reflect_color = vec3(texCubeLodGamma(tex_GlobalEnvMap, reflect_dir, mip_level).w);

#ifdef FRAMEBUFFER_FETCH
	FragColor0 = FragColor0 + vec4(reflect_color * c_fInvLumScale, 1.0);
	Normal_Depth = Normal_Depth;
	SpecularColor = SpecularColor;
#else
	OutputColor0(vec4(0.0, 0.0, 0.0, 0.0));
	
	#ifdef ES3
		OutputColor1(vec4(reflect_color, 1.0));
	#endif
#endif
}

