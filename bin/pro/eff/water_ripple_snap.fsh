uniform MEDIUMP sampler2D tex_Ripple;
uniform MEDIUMP float c_SimSize;
uniform MEDIUMP vec2 c_WaterRipplesParams;

psin MEDIUMP vec2 uv;

void main(void)
{
	float fSimSize = c_SimSize;
	float fSnapRange = 5.0;
	
	vec2 vRipplesPos = uv * 2.0 - 1.0;
	
	vRipplesPos = vRipplesPos * fSimSize;
	vRipplesPos.x -= fSnapRange * c_WaterRipplesParams.x;
	vRipplesPos.y -= fSnapRange * c_WaterRipplesParams.y;
	
	vec2 tcRipples = ((vRipplesPos.xy) / fSimSize) * 0.5 + 0.5;
	
	OutputColor0(tex2D(tex_Ripple, tcRipples));	
}