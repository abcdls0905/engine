psin MEDIUMP vec4 oDiffuse;
psin MEDIUMP vec2 oTex0;
#if defined(LIGHTMAP)
	psin MEDIUMP vec2 oLightTexcoord;
#endif
psin MEDIUMP float oHeight;
psin MEDIUMP vec3 oNormal;

psin MEDIUMP vec4 oPos;


uniform MEDIUMP float c_fAlphaRef;
uniform MEDIUMP sampler2D tex_Diffuse;
uniform MEDIUMP sampler2D tex_Prelight;

#ifdef ES3
	layout(location = 1) out HIGHP vec4 DiffuseColor;
	layout(location = 2) out HIGHP vec4 SpecularColor;
	#ifdef LIGHTMAP
		layout(location = 3) out HIGHP vec4 LightMapColor;
	#endif
#endif//ES3

vec3 EncodeSpecColor(vec3 specCol)
{
	vec3 encodedSpecCol = vec3( 0, 127.0/255.0, 127.0/255.0 );
	
	// Y'CbCr
	specCol = sqrt( specCol );
	encodedSpecCol.x = dot( vec3(0.299, 0.587, 0.114), specCol.rgb );
	encodedSpecCol.y += dot( vec3(-0.1687, -0.3312, 0.5), specCol.rgb );
	encodedSpecCol.z += dot( vec3(0.5, -0.4186, -0.0813), specCol.rgb );
	
	return encodedSpecCol;
}

vec3 EncodeNormal(vec3 n)
{
	n = -n;
	vec3 abs_n = abs(n);
	vec2 p = abs_n.xy * (1.0 / (abs_n.x + abs_n.y + abs_n.z));
	vec2 v = (n.z <= 0.0) ? (1.0 - p.yx) : p;
	v = vec2((n.x >= 0.0) ? v.x : -v.x, (n.y >= 0.0) ? v.y: -v.y);
	vec2 enc1023 = v * 511.5 + 511.5;
	
	vec2 residual;
	residual.x = float(fract(enc1023.x) > 0.5) / 3.0;
	residual.y = float(fract(enc1023.y) > 0.5) / 3.0;
	return vec3(floor(enc1023) / 1023.0, residual.x * 2.0 + residual.y);
}

void main()
{
	if(oDiffuse.w - c_fAlphaRef < 0.0)
	{
		discard;
	}
	
	// normal
	MEDIUMP vec3 vNormal = normalize(oNormal);
	vec3 encode_normal = EncodeNormal(vNormal);
	
	// diffuse
	MEDIUMP vec4 crDTex = tex2D(tex_Diffuse, oTex0);


	#ifndef EARLYZ
		#ifndef ALPHA_TO_COVERAGE
			if( (crDTex.w - c_fAlphaRef)<0.0)
			{
			  discard;
			}
		#endif
	#endif
	
	MEDIUMP vec4 oColor = crDTex;
	MEDIUMP float light = oHeight ;
	oColor.xyz *= light;
	oColor.w *= oDiffuse.w;
	
	// specular
	MEDIUMP float specular_power = 0.0;
	MEDIUMP vec3 specular_color = vec3(0.0, 0.0, 0.0);
	
	OutputColor0(
		vec4(encode_normal.x, encode_normal.y, 0.0, encode_normal.z));
	DiffuseColor = oColor;
	specular_color = max(specular_color, 0.001);
	SpecularColor = vec4(specular_power, EncodeSpecColor(specular_color));
	
	// lightmap
	#ifdef LIGHTMAP
		LightMapColor = tex2DGamma(tex_Prelight, oLightTexcoord) * DECODE_TO_LINEAR(2.0);
		LightMapColor.a = 1.0;
	#endif
}
