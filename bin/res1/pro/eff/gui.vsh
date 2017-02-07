precision mediump float;
precision mediump int;

attribute mediump vec4 iPos;
attribute mediump vec4 iDiffuse;

#ifdef USE_POINT
	attribute mediump float iPSize;
#endif

#ifdef USE_POINT
	attribute mediump float oPSize : PSIZE;
#endif

#ifdef USE_PICTURE
attribute mediump vec2 iTexCoord;
#endif

varying mediump vec4 oDiffuse;
varying mediump vec2 oTexCoord;

uniform mediump mat4 c_mtxOrient;
uniform mediump vec4 c_TwoPixelSize;
uniform mediump mat4 c_mtxRotate;

void main()
{
	mediump vec4 pos;
#ifdef USE_ROTATE
	pos = c_mtxRotate * iPos;
#else
	pos = iPos;
#endif
	pos = c_mtxOrient * pos;
	oDiffuse = iDiffuse.zyxw;

#ifdef USE_POINT
	oPSize = iPSize;
#endif

#ifdef USE_DEPTH
	pos.xyz = pos.xyz * pos.w;
#endif

#ifdef USE_PICTURE
	oTexCoord = iTexCoord;
#endif
	gl_Position = pos;
}