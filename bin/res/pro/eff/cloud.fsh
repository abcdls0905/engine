precision mediump float;
precision mediump int;

#if defined FOG
varying mediump vec4 oFog;
#endif
#if defined SHADOW
	varying mediump vec4 oPos;
	varying mediump vec4 oViewScale;

	uniform mediump vec4 c_pixelSize;
	uniform mediump vec4 c_fadeParam;
	uniform mediump vec4 c_biasUV;
	uniform mediump mat4 c_matProjInverse;

	uniform sampler2D tex_depthMap;
#else
	varying mediump vec2 oUV;
	varying mediump vec4 oViewToVertex;

	uniform mediump vec4 c_fadeParam;
#endif

uniform mediump vec4 c_diffuseColor;
uniform sampler2D tex_diffuseMap;

void main()
{
#if defined SHADOW
	mediump vec2 uv = (oPos + 0.5) * c_pixelSize.xy;
	mediump float depth_value = tex2D(tex_depthMap, uv).x * 1024.0;
	mediump vec4 pos = vec4(oViewScale.xyz / oViewScale.z * depth_value, 1.0);
	
	mediump vec4 worldpos = c_matProjInverse * pos;

	mediump float cloud_height = c_fadeParam.y - worldpos.y;
	uv = worldpos.xz + c_biasUV.zw * cloud_height;
	uv = uv * c_fadeParam.x + c_biasUV.xy;

	mediump vec3 ViewDir = normalize(vec3(c_biasUV.z, 1.0, c_biasUV.w));

	mediump float Alpha = tex2D(tex_diffuseMap, uv).w;
	Alpha *= c_diffuseColor.w;
	Alpha *= clamp((ViewDir.y - c_fadeParam.w) * c_fadeParam.z, 0.0, 1.0);

	gl_FragColor = vec4((1.0 - Alpha), 1.0, 1.0, 1.0);
#else
	mediump vec4 oColor = texture2D(tex_diffuseMap , oUV.xy);
	oColor *= c_diffuseColor;

	#if defined FOG
		oColor.xyz = oColor.xyz * oFog.w + oFog.xyz;
	#endif

	mediump vec3 ViewDir = normalize(oViewToVertex.xyz);
	oColor.w *= clamp((ViewDir.y - c_fadeParam.w) * c_fadeParam.z, 0.0, 1.0);

	gl_FragColor = oColor;
#endif
}