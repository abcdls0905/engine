uniform MEDIUMP sampler2D tex_BaseMap;

psin vec2 oUV;

vec3 DecodeNormal(in vec3 e)
{
   e *= vec3(2.0, 2.0, 1.5);
   e.xy += vec2(floor(e.z), fract(e.z)) / vec2(1023.0, 511.5) - 1.0;
   vec3 v = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
   vec2 signNotZero = vec2(e.x > 0.0, e.y > 0.0) * 2.0 - 1.0;
   if (v.z < 0.0) v.xy = (1.0 - abs(e.yx)) * signNotZero;
   return normalize(-v);
}

void DecodeNormalTranslucency(in vec4 normal_tex, out vec3 normal, out float translucency)
{
	normal.xyz = DecodeNormal(normal_tex.xyw);
	translucency = normal_tex.z;
}

vec3 DecodeSpecColor( vec3 encodedSpecCol, const bool useChrominance )
{
	vec3 specCol;
	
	// Y'CbCr
	encodedSpecCol.yz -= vec2(127.0/255.0, 127.0/255.0);
	if (!useChrominance) encodedSpecCol.yz = vec2(0.0, 0.0);
	specCol.r = encodedSpecCol.x + 1.402 * encodedSpecCol.z;
	specCol.g = dot( vec3( 1.0, -0.3441, -0.7141 ), encodedSpecCol.xyz );
	specCol.b = encodedSpecCol.x + 1.772 * encodedSpecCol.y;

	return specCol * specCol;
}

void main(void)
{
	vec4 oColor = tex2D(tex_BaseMap, oUV);

#ifdef SHOW_NORMAL
	vec3 normal;
	float translucency;
	DecodeNormalTranslucency(oColor, normal, translucency);
	
	oColor.rgb = normal;
#endif

#ifdef SHOW_SPECULAR_LEVEL
	vec3 specular_color = DecodeSpecColor( oColor.gba, true );
	oColor.rgb = specular_color;
	oColor.a = 1.0;
#endif
	
#ifdef SHOW_GLOSS
	float specular_power = oColor.r;
	oColor.rgb = vec3(specular_power, specular_power, specular_power);
	oColor.a = 1.0;
#endif

	OutputColor0(oColor);
}