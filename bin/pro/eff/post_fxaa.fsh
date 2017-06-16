psin vec2 v_vec2UV0;

uniform MEDIUMP sampler2D tex_color;

uniform vec4 u_vec4TexelDeltaAndScale;

/*
#define LOWEST 1

void main() {

#ifdef LOWEST
	float FXAA_SPAN_MAX = 8.0;
	float FXAA_REDUCE_MUL = 1.0/8.0;
	float FXAA_REDUCE_MIN = (1.0/128.0);

	vec2 uv0 = v_vec2UV0.xy * u_vec4TexelDeltaAndScale.zw;

	vec3 rgbNW = tex2D(tex_color, uv0 + (vec2(-1.0, -1.0) * u_vec4TexelDeltaAndScale.xy)).xyz;
	vec3 rgbNE = tex2D(tex_color, uv0 + (vec2(+1.0, -1.0) * u_vec4TexelDeltaAndScale.xy)).xyz;
	vec3 rgbSW = tex2D(tex_color, uv0 + (vec2(-1.0, +1.0) * u_vec4TexelDeltaAndScale.xy)).xyz;
	vec3 rgbSE = tex2D(tex_color, uv0 + (vec2(+1.0, +1.0) * u_vec4TexelDeltaAndScale.xy)).xyz;
	vec3 rgbM  = tex2D(tex_color, uv0).xyz;

	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot( rgbM, luma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * u_vec4TexelDeltaAndScale.xy;

	vec3 rgbA = (1.0/2.0) * (
		tex2D(tex_color, uv0 + dir * (1.0/3.0 - 0.5)).xyz +
		tex2D(tex_color, uv0 + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		tex2D(tex_color, uv0 + dir * (0.0/3.0 - 0.5)).xyz +
		tex2D(tex_color, uv0 + dir * (3.0/3.0 - 0.5)).xyz);
	float lumaB = dot(rgbB, luma);

	vec4 oColor;
	if((lumaB < lumaMin) || (lumaB > lumaMax)){
		oColor.xyz=rgbA;
	} else {
		oColor.xyz=rgbB;
	}
	oColor.w = 1.0;
	OutputColor0(oColor);
#else
	OutputColor0(vec4(1.0, 1.0, 1.0, 1.0));
#endif
}
*/

float fxaaConsoleEdgeThresholdMin = 0.00;
float fxaaConsoleEdgeSharpness = 8.0;
float fxaaConsoleEdgeThreshold = 0.25;

void main()
{
    vec4 fxaaConsoleRcpFrameOpt = vec4(2.0 , 0.22, u_vec4TexelDeltaAndScale.x, u_vec4TexelDeltaAndScale.y);
    vec4 fxaaConsoleRcpFrameOpt2 = vec4(1.0 , 0.11 ,u_vec4TexelDeltaAndScale.x, u_vec4TexelDeltaAndScale.y);
   
	vec2 uv0 = v_vec2UV0.xy * u_vec4TexelDeltaAndScale.zw;
	
	vec2 pos = uv0; 
	vec4 oColor;

	float lumaNw = tex2D(tex_color, uv0 + (vec2(-1.0, -1.0) * u_vec4TexelDeltaAndScale.xy)).y;
	float lumaNe = tex2D(tex_color, uv0 + (vec2(+1.0, -1.0) * u_vec4TexelDeltaAndScale.xy)).y;
	float lumaSw = tex2D(tex_color, uv0 + (vec2(-1.0, +1.0) * u_vec4TexelDeltaAndScale.xy)).y;
	float lumaSe = tex2D(tex_color, uv0 + (vec2(+1.0, +1.0) * u_vec4TexelDeltaAndScale.xy)).y;
	vec4 rgbyM  = tex2D(tex_color, uv0);  
	
	float lumaM =  rgbyM.y;
	float lumaMaxNwSw = max(lumaNw, lumaSw);
	lumaNe += 1.0/384.0;
	float lumaMinNwSw = min(lumaNw, lumaSw);
	float lumaMaxNeSe = max(lumaNe, lumaSe);
	float lumaMinNeSe = min(lumaNe, lumaSe);
	float lumaMax = max(lumaMaxNeSe, lumaMaxNwSw);
	float lumaMin = min(lumaMinNeSe, lumaMinNwSw);
	float lumaMaxScaled = lumaMax * fxaaConsoleEdgeThreshold;
	float lumaMinM = min(lumaMin, lumaM);
	float lumaMaxScaledClamped = max(fxaaConsoleEdgeThresholdMin, lumaMaxScaled);
	float lumaMaxM = max(lumaMax, lumaM);
	float dirSwMinusNe = lumaSw - lumaNe;
	float lumaMaxSubMinM = lumaMaxM - lumaMinM;
	float dirSeMinusNw = lumaSe - lumaNw;
	if(lumaMaxSubMinM < lumaMaxScaledClamped) 
	{
		oColor = rgbyM;
	}
	else
	{
		vec2 dir;
		dir.x = dirSwMinusNe + dirSeMinusNw;
		dir.y = dirSwMinusNe - dirSeMinusNw;
		vec2 dir1 = normalize(dir.xy);
		vec4 rgbyN1 = tex2D(tex_color, pos.xy - dir1 * fxaaConsoleRcpFrameOpt.zw);
		vec4 rgbyP1 = tex2D(tex_color, pos.xy + dir1 * fxaaConsoleRcpFrameOpt.zw);
		float dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * fxaaConsoleEdgeSharpness;
		vec2 dir2 = clamp(dir1.xy / dirAbsMinTimesC, -2.0, 2.0);
		vec4 rgbyN2 = tex2D(tex_color, pos.xy - dir2 * fxaaConsoleRcpFrameOpt2.zw);
		vec4 rgbyP2 = tex2D(tex_color, pos.xy + dir2 * fxaaConsoleRcpFrameOpt2.zw);
		vec4 rgbyA = rgbyN1 + rgbyP1;
		vec4 rgbyB = ((rgbyN2 + rgbyP2) * 0.25) + (rgbyA * 0.25);
		bool twoTap = ( rgbyB.y < lumaMin) || ( rgbyB.y > lumaMax);
		if(twoTap) rgbyB.xyz = rgbyA.xyz * 0.5;
		oColor = rgbyB;
	}

	oColor.w = 1.0;
	OutputColor0(oColor);	
}
