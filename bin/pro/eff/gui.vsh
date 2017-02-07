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

uniform mediump vec4 c_TwoPixelSize;
uniform mediump mat4 c_mtxRotate;

void main()
{
	
#ifdef USE_ROTATE
	gl_Position = c_mtxRotate * iPos;
#else
	gl_Position = iPos;
#endif
	gl_Position.xy = (vec4(gl_Position.xy, 1.0, 1.0) * c_TwoPixelSize).xy + vec2(-1.0, 1.0);
	oDiffuse = (iDiffuse/255.0).zyxw;

#ifdef USE_POINT
	oPSize = iPSize;
#endif

#ifdef USE_DEPTH
	gl_Position.xyz *= gl_Position.w;
#endif

#ifdef USE_PICTURE
	oTexCoord = iTexCoord;
#endif
}