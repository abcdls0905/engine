precision mediump float;
precision mediump int;

attribute mediump vec3 iPos;

#if defined SHADOW
	uniform mediump mat4 c_matProjInverse;
	varying mediump vec4 oPos;
	varying mediump vec4 oViewScale;
#else
	attribute mediump vec2 iUV;
	uniform mediump mat4 c_matWVP;
	uniform mediump vec2 c_biasUV;	
	uniform mediump vec3 c_cameraPos;

	#if defined FOG
	uniform mediump vec4 c_fogColor;
	uniform mediump vec4 c_fogParam;

	varying mediump vec4 oFog;
	#endif

	varying mediump vec2 oUV;
	varying mediump vec4 oViewToVertex;
#endif

void main(void)
{
#if defined SHADOW
	   mediump vec4 pos = vec4(iPos.xy, 0.9999, 1.0);
	   oPos = pos;
	   gl_Position = pos;

	   oViewScale = c_matProjInverse * pos;
#else
	   mediump vec4 pos = vec4(iPos, 1.0);
	   mediump vec3 view_vec = pos.xyz - c_cameraPos.xyz;

	   pos = c_matWVP * pos;
	   pos.z = pos.w;

	   gl_Position = pos;

	   oUV = iUV.xy + c_biasUV.xy;

	   oViewToVertex = vec4(view_vec, length(view_vec));

#if defined FOG
	   vec3 ViewDir = normalize(view_vec.xyz);

	   float fogValue = clamp(ViewDir.y / c_fogParam.x, 0.0, 1.0);
	   oFog.w = pow(fogValue, c_fogParam.y);

	   oFog.xyz = c_fogColor.xyz * (1.0 - oFog.w);
#endif

#endif
}