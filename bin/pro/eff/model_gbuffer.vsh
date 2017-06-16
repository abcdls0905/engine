//#ifdef ES3
//	#define vsin in
//	#define vsout out
//	#define psin in
//#else
//	#define vsin attribute
//	#define vsout varying
//	#define psin varying
//#endif//ES3

vsin MEDIUMP vec3 iPos;
vsin MEDIUMP vec3 iPosAni;

#ifdef VERTEX_NORMAL
	vsin MEDIUMP vec3 iNormal;
#endif

vsin MEDIUMP vec3 iNormalAni;

#ifdef VERTEX_TEXTURE0
	vsin MEDIUMP vec2 iTex0;
#endif

#ifdef VERTEX_TEXTURE1
	vsin MEDIUMP vec2 iTex1;
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
		vsout MEDIUMP vec4 oTex2;
		uniform MEDIUMP vec4 c_AmbientMapParam;
	#endif
#endif

#ifdef VERTEX_NORMAL
	vsout MEDIUMP vec3 oNormal;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	vsout MEDIUMP vec3 oTangent;
	vsout MEDIUMP vec3 oBinormal;
#endif

vsout MEDIUMP vec4 oDiffuse;
vsout MEDIUMP vec4 oViewToVertex;
vsout MEDIUMP float oClipDistance;

#ifdef SHADOWMAP
	vsout HIGHP vec4 oViewPos;
	
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

//cbPerScene
uniform HIGHP mat4 c_mtxWVP;
uniform HIGHP mat4 c_mtxWV;
uniform MEDIUMP vec4 c_vViewPos;
uniform MEDIUMP vec4 c_vReferPos;
uniform MEDIUMP vec4 c_ClipPlane;
uniform MEDIUMP mat4 c_FilterMatrix;

#ifdef SHADOWMAP
	uniform HIGHP mat4 c_mtxView;
	uniform HIGHP mat4 c_mtxLightViewProj1;
	#ifdef PSSM2
		uniform HIGHP mat4 c_mtxLightViewProj2;
	#endif
	#ifdef PSSM3
		uniform HIGHP mat4 c_mtxLightViewProj3;
	#endif
	#ifdef PSSM4
		uniform HIGHP mat4 c_mtxLightViewProj4;
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
		MEDIUMP vec3 _nor = iNormal;
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

		_nor = (mtxWorld * vec4(_nor, 0.0)).xyz;
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
		//oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
		oNormal = (c_mtxWV * vec4(_nor, 0.0)).xyz;
	#endif
	//oTex0 = iTex0;

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
		oTex1.xy = iTex1.xy * c_LightMapParam.xy + c_LightMapParam1.xy;
		oTex1.zw = iTex1.xy * c_LightMapParam.xy + c_LightMapParam1.zw;
		
		oTex2.xy = iTex1.xy * c_AmbientMapParam.xy + c_AmbientMapParam.zw;
	#endif
#endif

#ifdef VERTEX_COLOR
	#ifdef IOS_METAL
		oDiffuse.x = float((iDiffuse & uint(0x00FF0000)) >> 16) / 255.0;
		oDiffuse.y = float((iDiffuse & uint(0x0000FF00)) >> 8) / 255.0;
		oDiffuse.z = float(iDiffuse & uint(0x000000FF)) / 255.0;
	#else
		oDiffuse.xyz = iDiffuse.zyx;
	#endif
#else
	oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
#endif

#if defined FALLOFF || defined DISAPPEAR
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;

	oViewToVertex = vec4(view_vec, length(view_vec));
#endif

#ifdef SHADOWMAP
    oViewPos = c_mtxView * pos_in_world;

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

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	#ifdef USEPHYSICS
		// apex cloth input tangent need normalized
		oTangent = normalize(_tangent);
		// apex cloth input binormal is null
		oBinormal = cross(iNormal, oTangent);
		
		oTangent = (c_mtxWV * vec4(oTangent, 0.0)).xyz;
		oBinormal = (c_mtxWV * vec4(oBinormal, 0.0)).xyz;
	#else
		//oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
		//oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
		oTangent = (c_mtxWV * vec4(_tangent, 0.0)).xyz;
		oBinormal = (c_mtxWV * vec4(_binormal, 0.0)).xyz;
	#endif
#endif

#if defined CLIPPLANE
	oClipDistance = dot(_pos, c_ClipPlane);
#endif
	gl_Position = _pos;
}
