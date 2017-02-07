attribute mediump vec2 iTexcoord;

uniform highp   mat4 c_mtxWVP;
uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_vEyePos;
uniform mediump vec4 c_vColorStart;
uniform mediump vec4 c_vColorEnd;
uniform highp   vec4 c_vPosition;
uniform mediump vec4 c_vDir;
uniform mediump vec4 c_vUp;
uniform mediump vec4 c_vLeft;
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
    float pi = 3.141593;
    
    // two math function params
    float a = c_vParams.z;
    float b = c_vParams.w;
    
    float t = c_vParams.x - c_vParams.y*iTexcoord.x;
    t = t*pi;
    float r = iTexcoord.y;
    float s = sin(t);
    float c = cos(t);
    float d = r*(pi*0.5 - t);
    
    // math position
    highp vec3 posMath;
    posMath.x = -r*s*cos(2.0*t);
    posMath.y = (r*r)*sin(a*d)*s*c*0.5 + sin(pi*r)*s*0.075 + sin(2*pi*r)*s*0.035;
    posMath.z = (r*r)*(s*s)*c*cos(b*d);

    // whip position
	highp vec4 pos = c_vPosition + c_vDir*posMath.x + c_vUp*posMath.y + c_vLeft*posMath.z;
    pos.w = 1.0;
    
    // last position
	gl_Position = pos * c_mtxWVP;

    // Color
	oColor = lerp(c_vColorStart, c_vColorEnd, iTexcoord.y);
	oTexcoord = iTexcoord;

#if defined(FOGLINEAR)||defined(FOGEXP)
    vec4 posWorld = pos * c_mtxWorld;
    oFogDepth = length(posWorld.xyz - c_vEyePos.xyz);
#endif
}
