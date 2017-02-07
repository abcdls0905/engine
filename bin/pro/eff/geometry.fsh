varying mediump vec4 oDiffuse;
varying mediump vec4 oNormal;

void main(void)
{
    gl_FragColor.x = oDiffuse.x * (0.6 + oNormal.w);
	gl_FragColor.y = oDiffuse.y * (0.6 + oNormal.w);
	gl_FragColor.z = oDiffuse.z * (0.6 + oNormal.w);
    gl_FragColor.w = oDiffuse.w;
}