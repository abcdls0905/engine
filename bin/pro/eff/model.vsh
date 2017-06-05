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

//#define VERTEX_TEXTURE0
//#define TEXTUREANI
//#define DISAPPEAR
//#define VERTEX_SKELETON
//#define VERTEX_TEXTURE0
//#define VIEWVECTOR
//#define CLIPPLANE
void main(void)
{
	mediump vec4 _pos;
	mediump vec3 _nor = iNormal;
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	mediump vec3 _tangent;
	mediump vec3 _binormal;
#endif
	
#ifdef VERTEX_ANIMATION
	_pos = vec4(iPos + (iPosAni - iPos) * c_fBlendWeight, 1.0);
	_nor = iNormal + (iNormalAni - iNormal) * c_fBlendWeight;
#endif	

#ifdef VERTEX_SKELETON	
	mediump vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);
	mediump vec3 dnor = vec3(0.0, 0.0, 0.0);
	mediump vec3 dtan = vec3(0.0, 0.0, 0.0);
	mediump vec3 dbin = vec3(0.0, 0.0, 0.0);
	
	mediump ivec4 boneIndex = ivec4(iBone);

	float _weight[4];
	_weight[0] = iWeight.x;
	_weight[1] = iWeight.y;
	_weight[2] = iWeight.z;
	_weight[3] = iWeight.w;
	for (int x = 0; x < 4; x++)
	{
		mat4 TM;
		mediump int ii = boneIndex[x] * 3;
		TM[0] = c_BoneList[ii + 0];
		TM[1] = c_BoneList[ii + 1];
		TM[2] = c_BoneList[ii + 2];
		TM[3] = vec4(0.0, 0.0, 0.0, 1.0);
		
		float weight = _weight[x];
		dpos += vec4(iPos.xyz, 1.0) * TM * weight ;
		dnor += (vec4(iNormal, 0.0) * TM).xyz * weight;
		#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
			dtan += (vec4(iTangent, 0.0) * TM).xyz * weight;
			dbin += (vec4(iBinormal, 0.0) * TM).xyz * weight;
		#endif	
	}
	
	_pos = dpos / dpos.w;
	_nor = dnor;
	#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
		_tangent = dtan;
		_binormal = dbin;
	#endif	
#else
	_pos = vec4(iPos, 1.0);
	#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
		_tangent = iTangent;
		_binormal = iBinormal;
	#endif
#endif

	mediump vec4 pos_in_world = c_mtxWorld * _pos;
	
	#ifdef DISAPPEAR
		float fDistanceToCamera = distance(pos_in_world.xyz, c_vViewPos.xyz);
		pos_in_world.xyz += iNormal * (0.001 * fDistanceToCamera);
	#endif
	
	//pos_in_world.xyz += c_vPositionOffset.xyz;// - c_vReferPos.xyz;
	gl_Position =  c_mtxWVP * _pos;
	oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
	//oTex0 = iTex0;
	
#ifdef SHADOWMAP
	oTexCoordShadow = pos_in_world * c_mtxShadowViewProj;
#endif	
	
#if defined VERTEX_TEXTURE0 || defined VERTEX_TEXTURE1
	oTex0 = vec4(0.0, 0.0, 0.0, 0.0);
#endif
	
#ifdef VERTEX_TEXTURE0
	#ifdef TEXTUREANI
		oTex0.x = (iTex0.x - 0.5) * c_mtxDiffuseTM[0][0] + (iTex0.y - 0.5) * c_mtxDiffuseTM[1][0] + c_mtxDiffuseTM[2][0] + 0.5;
		oTex0.y = (iTex0.x - 0.5) * c_mtxDiffuseTM[0][1] + (iTex0.y - 0.5) * c_mtxDiffuseTM[1][1] + c_mtxDiffuseTM[2][1] + 0.5;
	#else
		oTex0.x = iTex0.x;
		oTex0.y = iTex0.y;
	#endif
#endif

#ifdef VERTEX_TEXTURE1
	oTex0.zw = iTex1.xy * c_LightMapParam.xy + c_LightMapParam.zw;
#endif

#ifdef FILTERMAP
	#if defined DIFFUSEMAP || defined NORMALMAP || defined SPECULARMAP
		vec2 filterTexUV = iTexFilter;
	#else
		vec2 filterTexUV = iTex0.xy;
	#endif

	#ifdef FILTERTEXTUREANI
		oTexFilter.x = (filterTexUV.x - 0.5) * c_mtxFilterTM[0][0] + 
			(filterTexUV.y - 0.5) * c_mtxFilterTM[0][1] + c_mtxFilterTM[0][2] + 0.5;
		oTexFilter.y = (filterTexUV.x - 0.5) * c_mtxFilterTM[1][0] + 
			(filterTexUV.y - 0.5) * c_mtxFilterTM[1][1] + c_mtxFilterTM[1][2] + 0.5;
	#else
		oTexFilter.xy = filterTexUV.xy;
	#endif
#endif
#ifdef VERTEX_COLOR
	oDiffuse = (iDiffuse/255.0).zyxw;
#endif
		
#ifdef VIEWVECTOR
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
	oViewToVertex = vec4(view_vec, length(view_vec));
#endif

#if defined FOGLINEAR
	oFog.w = 1.0 - clamp((oViewToVertex.w - c_FogParam.x) * c_FogParam.y, 0.0, 1.0);
	oFog.xyz = c_FogColor.xyz * (1.0 - oFog.w);
#elif defined HEIGHT_FOG
	oFog.w = 1.0 - clamp((pos_in_world.y - c_HeightFogParam.x) * c_HeightFogParam.y, 0.0, 1.0);
	oFog.xyz = c_HeightFogColor.xyz * (1.0 - oFog.w);
#elif defined FOGEXP
	oFog.w = 1.0 / exp(oViewToVertex.w * c_FogExpParam.w);
	oFog.xyz = c_FogExpParam.xyz * (1.0 - oFog.w);
#endif	

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
	oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
#endif

#if defined CLIPPLANE
	oClipDistance = dot(gl_Position, c_ClipPlane);
#endif
}


