precision mediump float;
precision mediump int;

in mediump vec3 iPos;
in mediump vec3 iPosAni;
#ifdef VERTEX_NORMAL
in mediump vec3 iNormal;
#endif
in mediump vec3 iNormalAni;
#ifdef VERTEX_TEXTURE0
in mediump vec2 iTex0;
#endif
#ifdef VERTEX_TEXTURE1
in mediump vec2 iTex1;
#endif
#ifdef FILTERMAP
in mediump vec2 iTexFilter;
#endif
#ifdef	VERTEX_SKELETON
in mediump vec4 iBone;
in mediump vec4 iWeight;
#endif
#ifdef VERTEX_TANGENT
in mediump vec3 iTangent;
#endif
#ifdef VERTEX_BINORMAL
in mediump vec3 iBinormal;
#endif
in mediump vec4 iDiffuse;

out mediump vec4 oTex0;


#ifdef FILTERMAP
out mediump vec2 oTexFilter;
#endif

#ifdef VERTEX_NORMAL
out mediump vec3 oNormal;
#endif

#ifdef CAMERALIGHT
out mediump	float oCameraLightInten;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
out mediump vec3 oTangent;
out mediump vec3 oBinormal;
#endif
out mediump vec4 oDiffuse;
out mediump vec4 oViewToVertex;
out mediump float oClipDistance;

//uniform mediump mat4 mat4PMV;
uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_BoneList[234];
//uniform mediump vec4 c_vPositionOffset;
uniform mediump float c_fBlendWeight;
uniform mediump mat4 c_mtxDiffuseTM;
uniform mediump vec4 c_LightMapParam;
uniform mediump mat4  c_mtxShadowViewProj;

#ifdef SHADOWMAP
uniform mediump mat4 c_mtxView;
out mediump vec4 oViewPos;
uniform mediump mat4 c_mtxLightViewProj1;
uniform mediump mat4 c_mtxLightViewProj2;
uniform mediump mat4 c_mtxLightViewProj3;
uniform mediump mat4 c_mtxLightViewProj4;
out mediump vec4 v_shadowPos1;
out mediump vec4 v_shadowPos2;
out mediump vec4 v_shadowPos3;
out mediump vec4 v_shadowPos4;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
out mediump vec4 oFog;
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

#if defined WARFOG
	out mediump vec2 oTexWarFog;
	uniform mediump vec4 c_TexCoordOffset;
	uniform mediump vec4 c_ZoneOffsetAndSize;
#endif

//#define VERTEX_TEXTURE0
//#define TEXTUREANI
//#define DISAPPEAR
//#define VERTEX_SKELETON
//#define VERTEX_TEXTURE0
//#define VIEWVECTOR
//#define CLIPPLANE
//#define WARFOG

void main(void)
{
	highp vec4 _pos;
	#ifdef VERTEX_NORMAL
		mediump vec3 _nor = iNormal;
	#endif
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	mediump vec3 _tangent;
	mediump vec3 _binormal;
#endif

#ifdef VERTEX_ANIMATION
	_pos = vec4(iPos + (iPosAni - iPos) * c_fBlendWeight, 1.0);
	#ifdef VERTEX_NORMAL
		_nor = iNormal + (iNormalAni - iNormal) * c_fBlendWeight;
	#endif
#endif

#ifdef VERTEX_SKELETON
	mediump vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);
	mediump vec3 dnor = vec3(0.0, 0.0, 0.0);
	mediump vec3 dtan = vec3(0.0, 0.0, 0.0);
	mediump vec3 dbin = vec3(0.0, 0.0, 0.0);

    // bone[0]
    mat4 TM;
    mediump int ii = int(iBone.x) * 3;

    TM[0] = c_BoneList[ii + 0];
    TM[1] = c_BoneList[ii + 1];
    TM[2] = c_BoneList[ii + 2];
    TM[3] = vec4(0.0, 0.0, 0.0, 1.0);

    float weight = iWeight.x;
    dpos += vec4(iPos.xyz, 1.0) * TM * weight;

#ifdef VERTEX_NORMAL
    dnor += (vec4(iNormal.xyz, 0.0) * TM).xyz * weight;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
    dtan += (vec4(iTangent.xyz, 0.0) * TM).xyz * weight;
    dbin += (vec4(iBinormal.xyz, 0.0) * TM).xyz * weight;
#endif

    // bone[1]
    ii = int(iBone.y) * 3;

    TM[0] = c_BoneList[ii + 0];
    TM[1] = c_BoneList[ii + 1];
    TM[2] = c_BoneList[ii + 2];
    TM[3] = vec4(0.0, 0.0, 0.0, 1.0);

    weight = iWeight.y;
    dpos += vec4(iPos.xyz, 1.0) * TM * weight;
#ifdef VERTEX_NORMAL
    dnor += (vec4(iNormal.xyz, 0.0) * TM).xyz * weight;
#endif
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
    dtan += (vec4(iTangent.xyz, 0.0) * TM).xyz * weight;
    dbin += (vec4(iBinormal.xyz, 0.0) * TM).xyz * weight;
#endif

    // bone[2]
    ii = int(iBone.z) * 3;

    TM[0] = c_BoneList[ii + 0];
    TM[1] = c_BoneList[ii + 1];
    TM[2] = c_BoneList[ii + 2];
    TM[3] = vec4(0.0, 0.0, 0.0, 1.0);

    weight = iWeight.z;
    dpos += vec4(iPos.xyz, 1.0) * TM * weight;
#ifdef VERTEX_NORMAL
    dnor += (vec4(iNormal.xyz, 0.0) * TM).xyz * weight;
#endif
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
    dtan += (vec4(iTangent.xyz, 0.0) * TM).xyz * weight;
    dbin += (vec4(iBinormal.xyz, 0.0) * TM).xyz * weight;
#endif

    // bone[3]
    ii = int(iBone.w) * 3;

    TM[0] = c_BoneList[ii + 0];
    TM[1] = c_BoneList[ii + 1];
    TM[2] = c_BoneList[ii + 2];
    TM[3] = vec4(0.0, 0.0, 0.0, 1.0);

    weight = iWeight.w;
    dpos += vec4(iPos.xyz, 1.0) * TM * weight;
#ifdef VERTEX_NORMAL
    dnor += (vec4(iNormal.xyz, 0.0) * TM).xyz * weight;
#endif
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
    dtan += (vec4(iTangent.xyz, 0.0) * TM).xyz * weight;
    dbin += (vec4(iBinormal.xyz, 0.0) * TM).xyz * weight;
#endif

	_pos = dpos / dpos.w;

	#ifdef VERTEX_NORMAL
		_nor = dnor;
	#endif

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
		#ifdef VERTEX_NORMAL
			pos_in_world.xyz += iNormal * (0.001 * fDistanceToCamera);
		#endif
	#endif

	//pos_in_world.xyz += c_vPositionOffset.xyz;// - c_vReferPos.xyz;
	_pos =  c_mtxWVP * _pos;
	#ifdef VERTEX_NORMAL
		oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
	#endif
	//oTex0 = iTex0;

#ifdef SHADOWMAP
    oViewPos = c_mtxView * pos_in_world;
    v_shadowPos1 = c_mtxLightViewProj1 * pos_in_world;
    v_shadowPos2 = c_mtxLightViewProj2 * pos_in_world;
    v_shadowPos3 = c_mtxLightViewProj3 * pos_in_world;
    v_shadowPos4 = c_mtxLightViewProj4 * pos_in_world;
#endif

//#if defined VERTEX_TEXTURE0 || defined VERTEX_TEXTURE1
	oTex0 = vec4(0.0, 0.0, 0.0, 0.0);
//#endif

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
	oDiffuse.xyz = iDiffuse.zyx;

#ifdef OPACITY
	oDiffuse.w = iDiffuse.w;
#else
	oDiffuse.w = 1.0;
#endif
#else
	oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
#endif

#if defined VIEWVECTOR ||  defined CAMERALIGHT
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
#endif

#ifdef VIEWVECTOR
	oViewToVertex = vec4(view_vec, length(view_vec));
#endif

#ifdef CAMERALIGHT
	oCameraLightInten = max(dot(-normalize(view_vec.xyz), normalize(oNormal.xyz)), 0.0);
	// ¥”œ¬Õ˘œ¬À•ºı
	oCameraLightInten *= clamp(iPos.y / 1.5, 0.0, 1.0) * 0.5 + 0.5;
#endif

#if defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG
  vec4 fogColor = vec4(0.0, 0.0, 0.0, 1.0);
#endif

#if defined FOGEXP
	float fogExp = 1.0 / exp(oViewToVertex.w * c_FogExpParam.w);

	fogColor.w = fogExp;
	fogColor.xyz = c_FogExpParam.xyz * (1.0 - fogExp);
#endif

#if defined FOGLINEAR
	float foglinear = clamp((oViewToVertex.w - c_FogParam.x) * c_FogParam.y, 0.0, 1.0);

	fogColor.w = fogColor.w * (1.0 - foglinear);
	fogColor.xyz = fogColor.xyz * (1.0 - foglinear) + c_FogColor.xyz * foglinear;
#endif

#if defined HEIGHT_FOG
	float hDelta = pos_in_world.y - c_HeightFogParam.x;
	float fogHeight = step(hDelta, 0.0) * clamp(hDelta * c_HeightFogParam.y, 0.0, 1.0);
	fogColor.w = fogColor.w * (1.0-fogHeight);
	fogColor.xyz = fogColor.xyz * (1.0-fogHeight) + c_HeightFogColor.xyz * fogHeight;
#endif

#if defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG
	oFog = fogColor;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
	oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
#endif

#if defined WARFOG
	vec3 tcd = pos_in_world.xyz;
	tcd += c_TexCoordOffset.xyz;
	oTexWarFog.x = (c_ZoneOffsetAndSize.x - tcd.x) / c_ZoneOffsetAndSize.z;
	oTexWarFog.y = (tcd.z - c_ZoneOffsetAndSize.y) / c_ZoneOffsetAndSize.w;
#endif

#if defined CLIPPLANE
	oClipDistance = dot(_pos, c_ClipPlane);
#endif
	gl_Position = _pos;
}
