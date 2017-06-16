psin vec2 oUV;

uniform MEDIUMP vec4 c_LightAmbient;
uniform HIGHP mat4 c_mtxView;
//uniform MEDIUMP float c_fAmbientIntentity;
uniform MEDIUMP sampler2D tex_Normal;


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

void decode_normal_translucency(in sampler2D normal_map, in vec2 uv, 
	out vec3 nor, out float translucency)
{
	vec4 normal_tex = tex2D(normal_map, uv);
	DecodeNormalTranslucency(normal_tex, nor, translucency);
}

void main(void)
{
	vec2 tex_uv = oUV;

	vec3 ambient_light = c_LightAmbient.rgb;
	
// #ifdef SPHEREAMBIENT
	// vec3 nor;
	// float translucency;
	// decode_normal_translucency(tex_Normal, tex_uv, nor, translucency);
	// // hemispherical lighting: normals facing upside are slightly brighter shaded in ambient
	// ambient_light *= 0.75 + dot(nor, vec3(c_mtxView[1].xyz)) * 0.25;
// #endif

// #ifdef SHADOW 
	// vec4 shadow_inten = tex_Shadow.Sample(sam_PointClamp, tex_uv);

	// #ifdef SSDO
		// vec4 ssdo_tex = tex_SSDO.Sample(sam_PointClamp, tex_uv);
		// ambient_light *= max(1.0 - saturate(ssdo_tex.w * c_vSSDOFactorParams.x), 0.3);
	// #else
		// ambient_light *= shadow_inten.y;
	// #endif
// #endif

// #ifdef INTENSITY
	// ambient_light *= c_fAmbientIntentity;
// #endif

	OutputColor0(vec4(ambient_light, 1.0));
}
