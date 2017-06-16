uniform MEDIUMP float c_fAlphaRefValue;
uniform MEDIUMP vec4 c_AppendColor;

#ifdef LUM_SCALE
	uniform MEDIUMP float c_fInvLumScale;
#endif

void main (void)
{
	vec4 color = c_AppendColor;
	
#ifdef LUM_SCALE
	color.xyz *= c_fInvLumScale;
#endif

	OutputColor0(color);
}
