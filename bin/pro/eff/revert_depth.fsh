
uniform HIGHP sampler2D tex_Source;
uniform HIGHP vec4 c_DepthParam;

psin vec2 oUV;

void main(void)
{
	HIGHP float zvalue = tex2D(tex_Source, oUV).x;
	zvalue = 1.0 / (c_DepthParam.x - zvalue * c_DepthParam.y);
	
	OutputColor0(vec4(zvalue, 0.0, 0.0, 1.0));
}
