precision mediump float;
precision mediump int;

varying mediump vec4 oDiffuse;

void main(void)
{
	gl_FragColor = oDiffuse;
}