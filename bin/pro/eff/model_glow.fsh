uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_DiffuseAlpha;
uniform MEDIUMP sampler2D tex_EmissiveMap;

// PS Constants
uniform HIGHP vec4 c_MaterialAmbient;
uniform HIGHP vec4 c_MaterialAmbientEx;

//scene
uniform HIGHP vec3 c_vLightDir;
uniform HIGHP vec4 c_LightAmbient;
uniform HIGHP vec4 c_LightDiffuse;

//???
uniform HIGHP vec4 c_MaterialDiffuse;
uniform HIGHP vec4 c_MaterialEmissive;
uniform HIGHP vec3 c_MaterialSpecular;
uniform HIGHP vec4 c_AppendColor;

uniform HIGHP float c_fSpecularPower;
uniform HIGHP float c_fAlphaRefValue;

uniform HIGHP vec4 c_GlowColor;
uniform HIGHP float c_fGlowAlpha;

#ifdef LUM_SCALE
	uniform HIGHP float c_fInvLumScale;
#endif

psin HIGHP vec4 oTex0;

#ifdef VERTEX_NORMAL
	psin HIGHP vec3 oNormal;
#endif

psin HIGHP vec4 oDiffuse;

#if (defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG)
	psin HIGHP vec4 oFog;
#endif


HIGHP vec4 lerp(HIGHP vec4 u, HIGHP vec4 v, HIGHP float t)
{
	return u + (v-u)*t;
}

HIGHP vec3 lerp(HIGHP vec3 u, HIGHP vec3 v, HIGHP float t)
{
	return u + (v-u)*t;
}

void main (void)
{
	HIGHP float bias = 0.0;
	
	HIGHP vec4 output_color = vec4(c_GlowColor.rgb, c_fGlowAlpha);
	HIGHP vec3 nor = normalize(oNormal);
	HIGHP vec2 tc = oTex0.xy;
	
	#ifdef DYNAMICLIGHTING
		output_color.rgb = c_MaterialAmbient.rgb * c_LightAmbient.rgb + 
			max(dot(c_vLightDir, nor), 0.0) * c_MaterialDiffuse.rgb * c_LightDiffuse.rgb;
	#endif
	
	#ifdef DIFFUSE
		output_color.rgb *= oDiffuse.rgb;
	#endif
	
	#ifdef DIFFUSEMAP
		HIGHP vec4 crTexDiffuse = tex2DBiasGamma(tex_Diffuse, oTex0.xy, bias);

		#ifdef DIFFUSEMAP_ALPHA
			crTexDiffuse.a = tex2DBias(tex_DiffuseAlpha, oTex0.xy, bias).r;
		#endif
		
		#ifdef ALPHATEST
			if(crTexDiffuse.a - c_fAlphaRefValue < 0.0)
			{
				discard;
			}
		#endif
		
		output_color *= crTexDiffuse;
	#endif
	
	#if defined EMISSIVE
		#if !DIFFUSE && !DYNAMICLIGHTING && !DIFFUSEMAP && !APPENDCOLOR
			output_color.rgb = 0.0;
		#endif
	
		#ifdef EMISSIVEMAP
			MEDIUMP vec3 crTexEmissive = c_MaterialEmissive.xyz * tex2DBiasGamma(tex_EmissiveMap, oTex0.xy, bias).xyz;
			output_color.xyz += crTexEmissive.xyz;
		#else
			output_color.xyz += crTexEmissive.xyz;
		#endif
	#endif
	
	#ifdef BLEND_ENHANCE
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			output_color.xyz = output_color.xyz * oFog.w;
		#endif
	#else
		#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
			output_color.xyz = output_color.xyz * oFog.w + oFog.xyz;
		#endif
	#endif
	
	#ifdef LUM_SCALE
		output_color.xyz *= c_fInvLumScale;
	#endif
	
	OutputColor0(output_color);
}