attribute highp vec3 iPosition;
attribute mediump vec3 iTangent;
attribute mediump vec3 iBinormal;
attribute mediump vec2 iTexcoord;

uniform highp mat4 c_mtxWVP;
uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_vEyePos;
uniform mediump float c_vBladeLength;
uniform mediump vec4 c_vColorStart;
uniform mediump vec4 c_vColorEnd;

varying mediump vec4 oColor;
varying mediump vec2 oTexcoord;

#if defined(REFRACTION)
varying mediump vec2 oTangent;
varying mediump vec2 oBinormal;
#endif

#if defined(FOGLINEAR)||defined(FOGEXP)||defined(REFRACTION)
varying mediump float oFogDepth;
#endif

mediump vec4 lerp(mediump vec4 u, mediump vec4 v, mediump float t)
{
	return u + (v-u)*t;
}

void main(void)
{
	highp vec4 pos = vec4(iPosition.xyz, 1.0);
	gl_Position = pos * c_mtxWVP;

#if defined(SINGLE)
	vec3 ViewDir = c_vEyePos.xyz - pos.xyz;
	if(length(iBinormal) > 0.001)
		pos.xyz += normalize(cross(iBinormal, ViewDir)) * (iTexcoord.x - 0.25) * c_vBladeLength;
#endif

	oColor = lerp(c_vColorStart, c_vColorEnd, iTexcoord.y);
	oTexcoord = iTexcoord;

#if defined(REFRACTION)
    vec4 vTmp;
    vTmp = vec4(normalize(iTangent), 0.0) * c_mtxWVP;
	oTangent = vTmp.xy / gl_Position.w;
    vTmp = vec4(normalize(iBinormal), 0.0) * c_mtxWVP;
	oBinormal = vTmp.xy / gl_Position.w;
	
	vec4 posWorld = pos * c_mtxWorld;
	oFogDepth = length(posWorld.xyz - c_vEyePos.xyz);
#else 
  #if defined(FOGLINEAR)||defined(FOGEXP)
	vec4 posWorld = pos * c_mtxWorld;
	oFogDepth = length(posWorld.xyz - c_vEyePos.xyz);
  #endif
#endif
}
