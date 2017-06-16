uniform MEDIUMP sampler2D tex_ColorMap;
uniform HIGHP sampler2D tex_DepthMap;
uniform MEDIUMP sampler2D tex_DiffuseMap;

uniform HIGHP float c_fWidth;
uniform HIGHP float c_fSSSLevel;
uniform HIGHP float c_fCorrection;
uniform HIGHP float c_fMaxDD;
uniform HIGHP vec2 c_PixelSize;
uniform HIGHP vec2 c_Orientation;

psin HIGHP vec2 oTex;

void main(void)
{
	vec4 oColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	// Offsets are chosen so that CDF has uniform distribution for 7 taps and variance 3.3 (same area for each sample)
	//const float offsets[6] = { 0.352, 0.719, 1.117, 1.579, 2.177, 3.213 };  // 3.3
	float offsets[6];
	offsets[0] = 0.352;
	offsets[1] = 0.719;
	offsets[2] = 1.117;
	offsets[3] = 1.579;
	offsets[4] = 2.177;
	offsets[5] = 3.213;
	
	HIGHP float depth = tex2D(tex_DepthMap, oTex).x * 1024.0;
	
	float subsurface_factor = tex2D(tex_DiffuseMap, oTex).a;

	HIGHP vec2 projRadius = c_Orientation * 0.001 / depth * subsurface_factor;

	vec3 totalWeight = vec3(1.0);
	vec3 colorSum = tex2D(tex_ColorMap, oTex).rgb;
	vec3 blurFalloff = -1.0 / (2.0 * vec3( 3.3, 1.4, 1.1 ));
	
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			vec2 uv = oTex + offsets[j] * projRadius;
		
			vec3 tap = tex2D(tex_ColorMap, uv).rgb;
			HIGHP float tap_depth = tex2D(tex_DepthMap, uv).x * 1024.0;

			HIGHP float depthDiff = (tap_depth - depth) * 1000.0; // Difference in mm

			vec3 weight = exp((offsets[j] * offsets[j] + depthDiff * depthDiff) * blurFalloff);
			
			totalWeight += weight;
			colorSum += weight * tap;
		}
		
		projRadius *= -1.0;
	}
	
	oColor.rgb = colorSum / totalWeight;
	
	OutputColor0(oColor);
}
