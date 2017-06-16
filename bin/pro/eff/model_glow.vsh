vsin HIGHP vec3 iPos;
vsin HIGHP vec3 iPosAni;

#ifdef VERTEX_NORMAL
	vsin MEDIUMP vec3 iNormal;
#endif

vsin MEDIUMP vec3 iNormalAni;

#if defined VERTEX_TEXTURE0
	vsin MEDIUMP vec2 iTex0;
#endif

#ifdef VERTEX_SKELETON
	vsin MEDIUMP vec4 iBone;
	vsin MEDIUMP vec4 iWeight;
#endif

#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuse;
#else
vsin MEDIUMP vec4 iDiffuse;
#endif

vsout MEDIUMP vec4 oTex0;

#ifdef VERTEX_NORMAL
	vsout MEDIUMP vec3 oNormal;
#endif

vsout MEDIUMP vec4 oDiffuse;
vsout HIGHP vec4 oViewToVertex;

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
	vsout MEDIUMP vec4 oFog;
#endif

uniform HIGHP mat4 c_mtxWorld;
uniform MEDIUMP vec4 c_BoneList[234];
uniform MEDIUMP float c_fBlendWeight;
uniform MEDIUMP mat4 c_mtxDiffuseTM;

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
uniform HIGHP mat4 c_mtxWV;
uniform HIGHP vec4 c_vViewPos;
uniform MEDIUMP vec4 c_vReferPos;

uniform HIGHP mat4 c_mtxVP;
uniform MEDIUMP float c_fGlowSize;
uniform HIGHP vec2 c_PixelSize;

void main(void)
{
	HIGHP vec4 _pos;

#ifdef VERTEX_NORMAL
	MEDIUMP vec3 _nor = iNormal;
#endif
	
#ifdef VERTEX_ANIMATION
	_pos = vec4(iPos + (iPosAni - iPos) * c_fBlendWeight, 1.0);
	#ifdef VERTEX_NORMAL
		_nor = iNormal + (iNormalAni - iNormal) * c_fBlendWeight;
	#endif
#endif

#if LINE
	MEDIUMP vec3 nor = normalize(_nor);
	float thickness = dot(nor, _nor) * 4.0 - 2.0;
	_nor = nor;
#endif

#ifdef VERTEX_SKELETON
	MEDIUMP vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);
	MEDIUMP vec3 dnor = vec3(0.0, 0.0, 0.0);

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

	_pos = dpos / dpos.w;
	
	#ifdef VERTEX_NORMAL
		_nor = dnor;
	#endif
#else
	_pos = vec4(iPos, 1.0);
#endif

#if !LINE
	// 沿法线方向扩大（本地空间）
	_pos += _nor * c_fGlowSize;
#endif

	HIGHP vec4 pos_in_world = c_mtxWorld * _pos;
	
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
#else
	oTex0.x = 0.0;
	oTex0.y = 0.0;
#endif

#ifdef VERTEX_COLOR
	#ifdef IOS_METAL
		oDiffuse.x = float((iDiffuse & uint(0x00FF0000)) >> 16) / 255.0;
		oDiffuse.y = float((iDiffuse & uint(0x0000FF00)) >> 8) / 255.0;
		oDiffuse.z = float(iDiffuse & uint(0x000000FF)) / 255.0;
	#else
		oDiffuse.xyz = iDiffuse.zyx;
	#endif

	//#ifdef OPACITY
		#ifdef IOS_METAL
			oDiffuse.w =  float(iDiffuse >> 24) / 255.0;
		#else
			oDiffuse.w = iDiffuse.w;
		#endif
	//#else
	//	oDiffuse.w = 1.0;
	//#endif
#else
	oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
#endif

#if defined VIEWVECTOR
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
#endif

#ifdef VIEWVECTOR
	oViewToVertex = vec4(view_vec, length(view_vec));
#endif
	
#if (defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG)
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

#if (defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG)
	oFog = fogColor;
#endif

#if LINE
	// 沿法线方向扩大（屏幕空间）
	HIGHP vec4 nor_proj = c_mtxVP * vec4(oNormal, 0.0);
	vec2 offset = normalize(nor_proj.xy / c_PixelSize);
	offset *= c_PixelSize * thickness;
	
	_pos = c_mtxWVP * _pos;
	_pos.xy += offset * c_fGlowSize * _pos.w;
	
	gl_Position = _pos;
#else
	gl_Position = c_mtxWVP * _pos;
#endif
}