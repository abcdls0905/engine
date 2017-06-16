//#ifdef ES3
//	#define vsin in
//	#define vsout out
//	#define psin in
//#else
//	#define vsin attribute
//	#define vsout varying
//	#define psin varying
//#endif//ES3

vsin HIGHP vec3 iPos;
vsin MEDIUMP vec3 iPosAni;

#ifdef VERTEX_NORMAL
	vsin HIGHP vec3 iNormal;
#endif

vsin MEDIUMP vec3 iNormalAni;

#ifdef VERTEX_TEXTURE0
	vsin MEDIUMP vec2 iTex0;
#endif

#ifdef VERTEX_TEXTURE1
	vsin MEDIUMP vec2 iTex1;
#endif

#ifdef FILTERMAP
	vsin MEDIUMP vec2 iTexFilter;
#endif

#ifdef VERTEX_SKELETON
	vsin MEDIUMP vec4 iBone;
	vsin MEDIUMP vec4 iWeight;
#endif

#ifdef VERTEX_INSTANCE
  vsin MEDIUMP float iInstanceIndex;
#endif

#ifdef VERTEX_TANGENT
	vsin MEDIUMP vec3 iTangent;
#endif

#ifdef VERTEX_BINORMAL
	vsin MEDIUMP vec3 iBinormal;
#endif

#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuse;
#else
vsin MEDIUMP vec4 iDiffuse;
#endif


vsout MEDIUMP vec4 oTex0;

#ifdef VERTEX_TEXTURE1
	#if defined(VERTEX_NORMAL) || defined(LIGHTMAP)
		vsout MEDIUMP vec4 oTex1;
		uniform MEDIUMP vec4 c_AmbientMapParam;
		#if defined(LIGHTMAP_AO_COLOR)
			vsout MEDIUMP vec4 oTex2;
		#endif
	#endif
#endif

#ifdef FILTERMAP
	vsout MEDIUMP vec2 oTexFilter;
#endif

#ifdef VERTEX_NORMAL
	vsout HIGHP vec3 oNormal;
#endif

#if defined CAMERALIGHT && defined VERTEX_NORMAL
	vsout MEDIUMP	float oCameraLightInten;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	vsout MEDIUMP vec3 oTangent;
	vsout MEDIUMP vec3 oBinormal;
#endif

vsout MEDIUMP vec4 oDiffuse;
vsout MEDIUMP vec4 oViewToVertex;
vsout MEDIUMP float oClipDistance;

#ifdef SHADOWMAP
	vsout HIGHP vec3 oWorldPos;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
	vsout MEDIUMP vec4 oFog;
#endif

#if defined WARFOG
	vsout MEDIUMP vec2 oTexWarFog;
#endif

//uniform MEDIUMP mat4 mat4PMV;
uniform MEDIUMP mat4 c_mtxWorld;
uniform MEDIUMP vec4 c_BoneList[234];
//uniform MEDIUMP vec4 c_vPositionOffset;
uniform MEDIUMP float c_fBlendWeight;
uniform MEDIUMP mat4 c_mtxDiffuseTM;
uniform MEDIUMP vec4 c_LightMapParam;
uniform MEDIUMP vec4 c_LightMapParam1;
uniform MEDIUMP vec4 c_LightMapParam2;
uniform MEDIUMP mat4  c_mtxShadowViewProj;

#if defined FOGLINEAR
	uniform MEDIUMP vec4 c_FogColor;
	uniform MEDIUMP vec4 c_FogParam;
#endif

#if defined HEIGHT_FOG
	uniform MEDIUMP vec4 c_HeightFogParam;
	uniform MEDIUMP vec4 c_HeightFogColor;
#endif

#if defined FOGEXP
	uniform MEDIUMP vec4 c_FogExpParam;
#endif

//cbPerScene
uniform HIGHP mat4 c_mtxWVP;
uniform MEDIUMP vec4 c_vViewPos;
uniform MEDIUMP vec4 c_vReferPos;
uniform MEDIUMP vec4 c_ClipPlane;
uniform MEDIUMP mat4 c_FilterMatrix;

#if defined WARFOG
	uniform MEDIUMP vec4 c_TexCoordOffset;
	uniform MEDIUMP vec4 c_ZoneOffsetAndSize;
#endif

#if defined(ES3) && defined(SHADOWMAP) && !defined(IOS_METAL)
	uniform HIGHP mat4 c_mtxLightViewProj1;
	uniform HIGHP mat4 c_mtxLightViewProj2;
	uniform HIGHP mat4 c_mtxLightViewProj3;
	uniform HIGHP mat4 c_mtxLightViewProj4;
	
	#ifdef PSSM4
		vsout HIGHP vec4 oShadowPos[4];
	#elif defined(PSSM3)
		vsout HIGHP vec4 oShadowPos[3];
	#elif defined(PSSM2)
		vsout HIGHP vec4 oShadowPos[2];
	#else
		vsout HIGHP vec4 oShadowPos;
	#endif
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
	HIGHP vec4 _pos;
	#ifdef VERTEX_NORMAL
		HIGHP vec3 _nor = iNormal;
	#endif
#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	MEDIUMP vec3 _tangent;
	MEDIUMP vec3 _binormal;
#endif

#ifdef VERTEX_ANIMATION
	_pos = vec4(iPos + (iPosAni - iPos) * c_fBlendWeight, 1.0);
	#ifdef VERTEX_NORMAL
		_nor = iNormal + (iNormalAni - iNormal) * c_fBlendWeight;
	#endif
#endif

#ifdef VERTEX_SKELETON
	MEDIUMP vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);
	MEDIUMP vec3 dnor = vec3(0.0, 0.0, 0.0);
	MEDIUMP vec3 dtan = vec3(0.0, 0.0, 0.0);
	MEDIUMP vec3 dbin = vec3(0.0, 0.0, 0.0);

    // bone[0]
    mat4 TM;
    MEDIUMP int ii = int(iBone.x) * 3;

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

	#ifdef DISAPPEAR
		MEDIUMP vec4 pos_in_world_temp = c_mtxWorld * _pos;
		float fDistanceToCamera = distance(pos_in_world_temp.xyz, c_vViewPos.xyz);
		#ifdef VERTEX_NORMAL
			_pos.xyz += iNormal * (0.001 * fDistanceToCamera);
		#endif
	#endif

	#ifdef VERTEX_INSTANCE
		mat4 mtxWorld;
		int bb = int(iInstanceIndex) * 3;
		mtxWorld[0] = vec4(c_BoneList[bb + 0].x, c_BoneList[bb + 1].x, c_BoneList[bb + 2].x, 0.0);
		mtxWorld[1] = vec4(c_BoneList[bb + 0].y, c_BoneList[bb + 1].y, c_BoneList[bb + 2].y, 0.0);
		mtxWorld[2] = vec4(c_BoneList[bb + 0].z, c_BoneList[bb + 1].z, c_BoneList[bb + 2].z, 0.0);
		mtxWorld[3] = vec4(c_BoneList[bb + 0].w, c_BoneList[bb + 1].w, c_BoneList[bb + 2].w, 1.0);
		
		MEDIUMP vec4 pos_in_world = mtxWorld * _pos;
	#else
		MEDIUMP vec4 pos_in_world = c_mtxWorld * _pos;
	#endif

	//pos_in_world.xyz += c_vPositionOffset.xyz;// - c_vReferPos.xyz;
	#if defined VERTEX_INSTANCE && defined USEPHYSICS
		_pos = c_mtxWVP * pos_in_world;
		pos_in_world.xyz = pos_in_world.xyz - c_vReferPos.xyz;
	#else
		_pos =  c_mtxWVP * _pos;
	#endif
	
	#ifdef VERTEX_NORMAL
		#ifdef VERTEX_INSTANCE
			oNormal = (mtxWorld * vec4(_nor, 0.0)).xyz;
		#else
			oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
		#endif
	#endif
	//oTex0 = iTex0;

#if defined(SHADOWMAP) && defined(ES3) && !defined(IOS_METAL)
    #ifdef PSSM4
		oShadowPos[3] = c_mtxLightViewProj4 * pos_in_world;
	#endif
	
	#ifdef PSSM3
		oShadowPos[2] = c_mtxLightViewProj3 * pos_in_world;
	#endif
	
	#ifdef PSSM2
		oShadowPos[1] = c_mtxLightViewProj2 * pos_in_world;
	#endif
	
	#if defined(PSSM2) || defined(PSSM3) || defined(PSSM4)
		oShadowPos[0] = c_mtxLightViewProj1 * pos_in_world;
	#else
		oShadowPos = c_mtxLightViewProj1 * pos_in_world;
	#endif
#endif

#ifdef SHADOWMAP
	oWorldPos = pos_in_world.xyz;
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

	#if defined(VERTEX_NORMAL) || defined(LIGHTMAP)
		//oTex1.xy = iTex1.xy * c_LightMapParam1.xy + c_LightMapParam1.zw;
		//oTex1.zw = iTex1.xy * c_LightMapParam2.xy + c_LightMapParam2.zw;

		//#ifdef NORMALMAP
			oTex1.xy = iTex1.xy * c_LightMapParam.xy + c_LightMapParam1.xy;
			oTex1.zw = iTex1.xy * c_LightMapParam.xy + c_LightMapParam1.zw;
			
			#if defined(LIGHTMAP_AO_COLOR) 
				oTex2.xy = iTex1.xy * c_AmbientMapParam.xy + c_AmbientMapParam.zw;
			#endif 

		//#endif	
	#endif

#endif

#ifdef FILTERMAP
	#if defined DIFFUSEMAP || defined NORMALMAP || defined SPECULARMAP
		vec2 filterTexUV = iTexFilter;
	#else
		vec2 filterTexUV = iTex0.xy;
	#endif

	#ifdef FILTERTEXTUREANI
		oTexFilter.x = (filterTexUV.x - 0.5) * c_FilterMatrix[0][0] +
			(filterTexUV.y - 0.5) * c_FilterMatrix[0][1] + c_FilterMatrix[0][2] + 0.5;
		oTexFilter.y = (filterTexUV.x - 0.5) * c_FilterMatrix[1][0] +
			(filterTexUV.y - 0.5) * c_FilterMatrix[1][1] + c_FilterMatrix[1][2] + 0.5;
	#else
		oTexFilter.xy = filterTexUV.xy;
	#endif
#endif
	
#ifdef VERTEX_COLOR
	#ifdef IOS_METAL
		oDiffuse.x = float((iDiffuse & uint(0x00FF0000)) >> 16) / 255.0;
		oDiffuse.y = float((iDiffuse & uint(0x0000FF00)) >> 8) / 255.0;
		oDiffuse.z = float(iDiffuse & uint(0x000000FF)) / 255.0;
	#else
		oDiffuse.xyz = iDiffuse.zyx;
	#endif //IOS_METAL
	
	#ifdef OPACITY
		#ifdef IOS_METAL
			oDiffuse.w =  float(iDiffuse >> 24) / 255.0;
		#else
			oDiffuse.w = iDiffuse.w;
		#endif
	#else
		oDiffuse.w = 1.0;
	#endif //OPACITY

#else //VERTEX_COLOR
	oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
#endif //VERTEX_COLOR

#if defined VIEWVECTOR ||  defined CAMERALIGHT
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
#endif

#ifdef VIEWVECTOR
	oViewToVertex = vec4(view_vec, length(view_vec));
#endif

#if defined CAMERALIGHT && defined VERTEX_NORMAL
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
	#ifdef USEPHYSICS
		// apex cloth input tangent need normalized
		oTangent = normalize(_tangent);
		// apex cloth input binormal is null
		oBinormal = cross(iNormal, oTangent);
		
		oTangent = (c_mtxWorld * vec4(oTangent, 0.0)).xyz;
		oBinormal = (c_mtxWorld * vec4(oBinormal, 0.0)).xyz;
	#else
		oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
		oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
	#endif
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
