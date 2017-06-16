#if defined SHADOW
	uniform MEDIUMP vec4 c_pixelSize;
	uniform MEDIUMP vec4 c_fadeParam;
	uniform MEDIUMP vec4 c_biasUV;
	uniform MEDIUMP mat4 c_matProjInverse;

	uniform MEDIUMP sampler2D tex_depthMap;
#else
	uniform MEDIUMP vec4 c_fadeParam;
#endif

uniform MEDIUMP vec4 c_diffuseColor;
uniform MEDIUMP sampler2D tex_diffuseMap;
uniform MEDIUMP sampler2D tex_diffuseMapAlpha;

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

#if defined FOG
uniform MEDIUMP vec4 c_fogColor;
uniform MEDIUMP vec4 c_fogParam;
#endif

#if defined SHADOW
	psin MEDIUMP vec4 oPos;
	psin MEDIUMP vec4 oViewScale;
#else
	psin HIGHP vec2 oUV;
	psin HIGHP vec4 oViewToVertex;
#endif

void main()
{
#if defined SHADOW
	MEDIUMP vec2 uv = (oPos + 0.5) * c_pixelSize.xy;
	MEDIUMP float depth_value = tex2D(tex_depthMap, uv).x * 1024.0;
	MEDIUMP vec4 pos = vec4(oViewScale.xyz / oViewScale.z * depth_value, 1.0);
	
	MEDIUMP vec4 worldpos = c_matProjInverse * pos;

	MEDIUMP float cloud_height = c_fadeParam.y - worldpos.y;
	uv = worldpos.xz + c_biasUV.zw * cloud_height;
	uv = uv * c_fadeParam.x + c_biasUV.xy;

	MEDIUMP vec3 ViewDir = normalize(vec3(c_biasUV.z, 1.0, c_biasUV.w));

	#ifdef DIFFUSEMAP_ALPHA
		MEDIUMP float Alpha = tex2D(tex_diffuseMapAlpha, uv).r;
	#else
		MEDIUMP float Alpha = tex2D(tex_diffuseMap, uv).w;
	#endif
	
	Alpha *= c_diffuseColor.w;
	Alpha *= clamp((ViewDir.y - c_fadeParam.w) * c_fadeParam.z, 0.0, 1.0);

	OutputColor0(vec4((1.0 - Alpha), 1.0, 1.0, 1.0));
#else
	MEDIUMP vec4 oColor = tex2DGamma(tex_diffuseMap , oUV.xy);
	
	#ifdef DIFFUSEMAP_ALPHA
		oColor.a = tex2D(tex_diffuseMapAlpha, oUV.xy).r;
	#endif
	
	oColor *= c_diffuseColor;

	
	HIGHP vec3 ViewDir = normalize(oViewToVertex.xyz);
	
	#if defined FOG
		float fogValue = clamp(ViewDir.y / c_fogParam.x, 0.0, 1.0);
		fogValue = pow(fogValue, c_fogParam.y);
	
		oColor.xyz = mix(c_fogColor.xyz, oColor.xyz, fogValue);
	#endif

	oColor.w *= clamp((ViewDir.y - c_fadeParam.w) * c_fadeParam.z, 0.0, 1.0);
	
	#ifdef LUM_SCALE
		oColor.xyz *= c_fInvLumScale;
	#endif

	OutputColor0(oColor);
#endif
}
