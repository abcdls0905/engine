vsin MEDIUMP vec3 iPos;
vsin MEDIUMP vec3 iPosAni;
vsin MEDIUMP vec3 iNormal;
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

#ifdef VERTEX_TANGENT
	vsin MEDIUMP vec3 iTangent;
#endif

#ifdef VERTEX_BINORMAL
	vsin MEDIUMP vec3 iBinormal;
#endif

vsin MEDIUMP vec4 iDiffuse;

#if defined VERTEX_TEXTURE0
	vsout MEDIUMP vec2 oTex0;
#endif

//cbPerScene
uniform MEDIUMP mat4 c_mtxWVP;

uniform MEDIUMP float c_fGlowSize;

#ifdef VERTEX_SKELETON
	uniform MEDIUMP vec4 c_BoneList[234];
#endif

void main(void)
{
	MEDIUMP vec4 _pos;
	
#ifdef VERTEX_SKELETON	
	MEDIUMP vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);

	MEDIUMP ivec4 boneIndex = ivec4(iBone);

	float _weight[4];
	_weight[0] = iWeight.x;
	_weight[1] = iWeight.y;
	_weight[2] = iWeight.z;
	_weight[3] = iWeight.w;
	for (int x = 0; x < 4; x++)
	{
		mat4 TM;
		MEDIUMP int ii = boneIndex[x] * 3;
		TM[0] = c_BoneList[ii + 0];
		TM[1] = c_BoneList[ii + 1];
		TM[2] = c_BoneList[ii + 2];
		TM[3] = vec4(0.0, 0.0, 0.0, 1.0);
		
		float weight = _weight[x];
		dpos += vec4(iPos.xyz, 1.0) * TM * weight ;
	}
	
	_pos = dpos / dpos.w;
#else
	_pos = vec4(iPos, 1.0);
#endif

	MEDIUMP vec4 pos = c_mtxWVP * _pos;
    pos /=pos.w;
	// 沿法线方向扩大 
	_pos.xyz += iNormal * c_fGlowSize*clamp(pos.z*5.0,1.0, 5.0 );
	MEDIUMP vec4 oPos = c_mtxWVP * _pos;

	gl_Position = oPos;

#if defined VERTEX_TEXTURE0
	oTex0 = iTex0;
#endif
}


