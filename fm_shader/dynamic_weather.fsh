//#define OCCLUSION



varying mediump vec2 oUV;
varying mediump vec4 oColor;
varying mediump vec3 oWorldPostion;

uniform mediump mat4 c_mtxViewProj;
uniform mediump mat4 c_mtxViewProjPrev;

uniform mediump vec4 c_vPositionOffset;
uniform mediump vec4 c_vVelocity;
uniform mediump vec4 c_vAlpha;

uniform mediump vec4 c_vViewPosition;
uniform mediump vec4 c_vSizeScale;
uniform mediump vec4 c_vLighting;
uniform mediump vec4 c_vForward;

uniform mediump mat4 c_mOcclusionMatrix;

uniform sampler2D tex_Color;
uniform sampler2D tex_OcclusionHeightMap;

void main()
{
   mediump vec4 color = texture2D(tex_Color, oUV) * oColor;
   
#if defined(OCCLUSION)
	// run the world position through the world to box space matrix
   mediump vec3 uv =(vec4(oWorldPostion, 1.0) * c_mOcclusionMatrix).xyz;
   mediump float height = texture2D(tex_OcclusionHeightMap, uv.xz).w;
	
	// clamp the uvs
   mediump vec3 uvclamp = clamp(uv,0.0,1.0);
	if(uvclamp.x == uv.x && uvclamp.z == uv.z && uv.y < height)
		color = vec4(0.0,0.0,0.0,0.0);
#endif
	   
   gl_FragColor = color;
}