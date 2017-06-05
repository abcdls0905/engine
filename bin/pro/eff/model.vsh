attribute mediump vec3 iPos;
attribute mediump vec3 iPosAni;
attribute mediump vec3 iNormal;
attribute mediump vec3 iNormalAni;
#ifdef VERTEX_TEXTURE0
attribute mediump vec2 iTex0;
#endif
#ifdef VERTEX_TEXTURE1
attribute mediump vec2 iTex1;
#endif
#ifdef FILTERMAP
attribute mediump vec2 iTexFilter;
#endif
#ifdef	VERTEX_SKELETON
attribute mediump vec4 iBone;
attribute mediump vec4 iWeight;
#endif
#ifdef VERTEX_TANGENT
attribute mediump vec3 iTangent;
#endif
#ifdef VERTEX_BINORMAL
attribute mediump vec3 iBinormal;
#endif
attribute mediump vec4 iDiffuse;

varying mediump vec4 oTex0;
#ifdef FILTERMAP
varying mediump vec2 oTexFilter;
#endif

varying mediump vec3 oNormal;
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
varying mediump vec3 oTangent;
varying mediump vec3 oBinormal;
#endif
varying mediump vec4 oDiffuse;
varying mediump vec4 oViewToVertex;
varying mediump float oClipDistance;

//uniform mediump mat4 mat4PMV;
uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_BoneList[234];
//uniform mediump vec4 c_vPositionOffset;
uniform mediump float c_fBlendWeight;
uniform mediump mat4 c_mtxDiffuseTM;
uniform mediump vec4 c_LightMapParam;
uniform mediump mat4  c_mtxShadowViewProj;

#ifdef SHADOWMAP
varying mediump vec4 oTexCoordShadow;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
varying mediump vec4 oFog;
#endif

#if defined FOGLINEAR
uniform mediump vec4 c_FogColor;
uniform mediump vec4 c_FogParam;
#endif

#if defined HEIGHT_FOG
uniform mediump vec4 c_HeightFogParam;
uniform mediump vec4 c_HeightFogColor;
#endif

#if defined FOGEXP
uniform mediump vec4 c_FogExpParam;
#endif

//cbPerScene
uniform highp mat4 c_mtxWVP;
uniform mediump vec4 c_vViewPos;
uniform mediump vec4 c_vReferPos;
uniform mediump vec4 c_ClipPlane;
uniform mediump mat4 c_mtxFilterTM;

void main(void)
{
    mediump vec4 _pos = vec4(iPos, 1);
    oTex0.xy = iTex0.xy;
    gl_Position =  c_mtxWVP * _pos;
    mediump vec3 _nor = iNormal;
    oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
    mediump vec4 pos_in_world = c_mtxWorld * _pos;
    vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
    oViewToVertex = vec4(view_vec, length(view_vec));
    #ifdef SHADOWMAP
	    oTexCoordShadow = pos_in_world * c_mtxShadowViewProj;
    #endif	
	
}