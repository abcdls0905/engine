vsin HIGHP vec3 iPos;
#if !defined SHADOW
	vsin HIGHP vec2 iUV;
#endif

#if defined SHADOW
	vsout MEDIUMP vec4 oPos;
	vsout MEDIUMP vec4 oViewScale;
#else
	vsout HIGHP vec2 oUV;
	vsout HIGHP vec4 oViewToVertex;
#endif

#if defined SHADOW
	uniform MEDIUMP mat4 c_matProjInverse;
#else
	uniform MEDIUMP mat4 c_matWVP;
	uniform MEDIUMP vec2 c_biasUV;	
	uniform HIGHP vec3 c_cameraPos;
#endif

void main(void)
{
#if defined SHADOW
	   HIGHP vec4 pos = vec4(iPos.xy, 0.9999, 1.0);
	   oPos = pos;
	   gl_Position = pos;

	   oViewScale = c_matProjInverse * pos;
#else
	   HIGHP vec4 pos = vec4(iPos, 1.0);
	   HIGHP vec3 view_vec = pos.xyz - c_cameraPos.xyz;

	   pos = c_matWVP * pos;
	   pos.z = pos.w;

	   gl_Position = pos;

	   oUV = iUV.xy + c_biasUV.xy;

	   oViewToVertex = vec4(view_vec, length(view_vec));
#endif
}