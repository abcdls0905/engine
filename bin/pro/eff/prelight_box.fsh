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

uniform MEDIUMP vec4 c_LightColor;
uniform MEDIUMP vec3 c_Attenu;
uniform MEDIUMP mat4 c_mtxLightVP;

uniform HIGHP sampler2D tex_Depth;

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
	HIGHP vec3 view_pos = oViewScale.xyz / oViewScale.z * depth_tex * 1024.0;
// #endif

	HIGHP vec4 local_coord = abs(vec4(view_pos, 1.0) * c_mtxLightVP);
	HIGHP float d = max(local_coord.x, max(local_coord.y, local_coord.z));
	
	if (d > 1.0)
	{
		OutputColor0(vec4(0.0, 0.0, 0.0, 0.0));
		#ifdef ES3
			OutputColor1(vec4(0.0, 0.0, 0.0, 0.0));
		#endif
		
		return;
	}

#ifdef ATTENU2
	d = max(d + c_Attenu.x, 0.0);

	HIGHP float attenu = max(0.0, 1.0 - (c_Attenu.y * d + c_Attenu.z * d * d));
#else
	HIGHP float attenu = max(0.0, 1.0 - c_Attenu.y * d);
#endif	
	
	float shadow = 1.0;
// #ifdef SHADOW	
	// shadow = calc_shadow(view_pos);
// #endif

	vec3 diffuse = c_LightColor.rgb * attenu;
	
	OutputColor0(vec4(diffuse, 1.0));
	#ifdef ES3
		OutputColor1(vec4(0.0, 0.0, 0.0, 1.0));
	#endif
}
