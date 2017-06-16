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

#ifdef VERTEX_TANGENT
	vsin MEDIUMP vec3 iTangent;
#endif

#ifdef VERTEX_BINORMAL
	vsin MEDIUMP vec3 iBinormal;
#endif

// if rendering water£¬ r = foam; g = wave; b = wind; a = depth
#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuse;
#else
vsin MEDIUMP vec4 iDiffuse;
#endif

vsout MEDIUMP vec4 oTex0;

#ifdef VERTEX_TEXTURE1
	#if defined(VERTEX_NORMAL) || defined(LIGHTMAP)
		vsout MEDIUMP vec4 oTex1;
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

uniform MEDIUMP mat4 c_mtxWorld;
uniform MEDIUMP vec4 c_BoneList[234];
uniform MEDIUMP float c_fBlendWeight;
uniform MEDIUMP mat4 c_mtxDiffuseTM;
uniform MEDIUMP vec4 c_LightMapParam;
uniform MEDIUMP mat4  c_mtxShadowViewProj;

//cbPerScene
uniform HIGHP mat4 c_mtxWVP;
uniform HIGHP mat4 c_mtxWV;
uniform MEDIUMP vec4 c_vViewPos;
uniform MEDIUMP vec4 c_vReferPos;
uniform MEDIUMP vec4 c_ClipPlane;
uniform MEDIUMP mat4 c_FilterMatrix;

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

	MEDIUMP vec4 pos_in_world = c_mtxWorld * _pos;

	_pos =  c_mtxWVP * _pos;
	
	#ifdef VERTEX_NORMAL
		oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
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

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
	oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
	oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
#endif

	gl_Position = _pos;
}