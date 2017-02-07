attribute highp vec3 iPosition;
attribute mediump vec2 iTexcoord;

uniform highp mat4 c_mtxWVP;
uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_vEyePos;
uniform mediump vec4 c_vColorStart;
uniform mediump vec4 c_vColorEnd;
uniform mediump vec4 c_vParams;

varying mediump vec4 oColor;
varying mediump vec2 oTexcoord;

#if defined(FOGLINEAR)||defined(FOGEXP)
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

	oColor = lerp(c_vColorStart, c_vColorEnd, iTexcoord.y);
	oTexcoord = iTexcoord;
	oTexcoord.y *= c_vParams.x;     // Scale the V

#if defined(FOGLINEAR)||defined(FOGEXP)
    vec4 posWorld = pos * c_mtxWorld;
    oFogDepth = length(posWorld.xyz - c_vEyePos.xyz);
#endif
}
