attribute highp vec3 iPos;
attribute highp vec2 iTexCoord;

varying highp vec2 oTexCoord;

uniform highp mat4 c_mtxWVP;

void main()
{
	oTexCoord = iTexCoord;
	gl_Position = vec4(iPos, 1.0) * c_mtxWVP;
}