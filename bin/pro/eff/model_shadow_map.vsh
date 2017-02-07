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

#if defined VERTEX_TEXTURE0 && defined ALPHATEST
varying mediump vec2 oTex0;
#endif

//varying mediump float oDepth;

//cbPerScene
uniform mediump mat4 c_mtxWVP;

#ifdef VERTEX_SKELETON
uniform mediump vec4 c_BoneList[234];
#endif

void main(void)
{
	mediump vec4 _pos;
	
#ifdef VERTEX_SKELETON	
	mediump vec4 dpos = vec4(0.0, 0.0, 0.0, 0.0);

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
	}
	
	_pos = dpos / dpos.w;
#else
	_pos = vec4(iPos, 1.0);
#endif

	mediump vec4 oPos = c_mtxWVP * _pos; 
	gl_Position = oPos;

#if defined VERTEX_TEXTURE0 && defined ALPHATEST
	oTex0 = iTex0;
#endif

	//oDepth = max(oPos.z, 0.0);
}


