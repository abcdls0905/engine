psin MEDIUMP vec2 oUV;
psin HIGHP vec4 oViewScale;

uniform MEDIUMP vec4 c_FogExpParam;
uniform MEDIUMP vec4 c_FogParam;
uniform MEDIUMP vec3 c_FogColor;
uniform MEDIUMP vec4 c_DepthParam;

#ifdef WARFOG
uniform MEDIUMP vec4 c_WarFogOffsetAndSize;
uniform MEDIUMP sampler2D tex_WarFog;
#endif

#ifdef HEIGHT_FOG
uniform MEDIUMP vec4 c_HeightFogColor;
uniform MEDIUMP vec4 c_HeightFogParam;
#endif

#if defined(WARFOG) || defined(HEIGHT_FOG)
uniform HIGHP mat4 c_mtxViewInverse;
#endif

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

uniform HIGHP sampler2D tex_DepthMap;
uniform MEDIUMP sampler2D tex_Prelight;
uniform MEDIUMP sampler2D tex_DiffuseMap;
uniform MEDIUMP sampler2D tex_Specular;

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

#define PI 3.1415926

void main(void)
{
	vec4 oColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec2 tex_uv = oUV;

	HIGHP float depth = tex2DBias(tex_DepthMap, tex_uv, 0.0).x * 1024.0;
	HIGHP vec3 view_pos = oViewScale.xyz / oViewScale.z * depth;
	vec4 diffuse_color = tex2DGamma(tex_DiffuseMap, tex_uv);
	vec4 diffuse = tex2D(tex_Prelight, tex_uv);
	vec3 specular = tex2D(tex_Specular, tex_uv).rgb;
	
	oColor.rgb = diffuse_color.rgb * diffuse.rgb + specular;
	oColor.rgb = min(oColor.rgb, 2000.0);	// 防止亮度太大时后续的后处理出现超出精度范围的情况

	float fog_depth = length(view_pos);

#ifdef FOGEXP
	oColor.rgb = mix(c_FogExpParam.rgb, oColor.rgb, 1.0 / exp(fog_depth * c_FogExpParam.a));
#endif

#ifdef FOGLINEAR
	oColor.rgb = mix(oColor.rgb, c_FogColor.rgb, clamp((fog_depth - c_FogParam.x) * c_FogParam.y, 0.0, 1.0));
#endif

#if defined(WARFOG) || defined(HEIGHT_FOG)
	vec4 world_pos = c_mtxViewInverse * vec4(view_pos, 1.0);
#endif

#ifdef HEIGHT_FOG
	MEDIUMP float hDelta = world_pos.y - c_HeightFogParam.x;
	hDelta = step(hDelta, 0.0) * clamp(hDelta * c_HeightFogParam.y, 0.0, 1.0);

	oColor.xyz = oColor.xyz * (1.0 - hDelta) + c_HeightFogColor.xyz * hDelta;
#endif

#ifdef WARFOG
	vec2 vWarTexCoord;
	
	vWarTexCoord.x = (c_WarFogOffsetAndSize.x - world_pos.x) / c_WarFogOffsetAndSize.z;
	vWarTexCoord.y = (world_pos.z - c_WarFogOffsetAndSize.y) / c_WarFogOffsetAndSize.w;
	
	MEDIUMP vec4 crWarFog = tex2D(tex_WarFog, vWarTexCoord);

	oColor.rgb = oColor.rgb * mix(0.2, 1.0, crWarFog.w);
#endif

#ifdef LUM_SCALE
	oColor.xyz *= c_fInvLumScale;
#endif

	OutputColor0(oColor);
}
