vsin MEDIUMP vec3 iPos;
	
#ifdef VERTEX_NORMAL
	vsin MEDIUMP vec3 iNormal;
#endif

#ifdef VERTEX_TEXTURE0
	vsin MEDIUMP vec2 iTex0;
#endif

#ifdef	VERTEX_SKELETON
	vsin MEDIUMP vec4 iBone;
	vsin MEDIUMP vec4 iWeight;
#endif

#ifdef NORMALMAP
	vsin MEDIUMP vec3 iTangent;
	vsin MEDIUMP vec3 iBinormal;
#endif

#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuse;
#else
vsin MEDIUMP vec4 iDiffuse;
#endif

vsout MEDIUMP vec2 oTex0;

#ifdef VERTEX_NORMAL
	vsout MEDIUMP vec3 oNormal;
#endif

#ifdef NORMALMAP
	vsout MEDIUMP vec3 oTangent;
	vsout MEDIUMP vec3 oBinormal;
#endif

vsout MEDIUMP vec4 oDiffuse;
vsout MEDIUMP vec2 oClipDistance;


uniform MEDIUMP mat4 c_mtxWorld;
uniform MEDIUMP vec4 c_BoneList[234];
uniform MEDIUMP mat4 c_mtxDiffuseTM;

#if defined WATER
	uniform MEDIUMP vec4 c_WaveParam; // x:time;y:wave_movement;z:wave_height
	uniform MEDIUMP vec4 c_WaveParam1;//x:1 / surface width;y:;y: 1 / water surface height;wave movement; w:wave height
#endif

//cbPerScene
uniform HIGHP mat4 c_mtxWVP;

void main(void)
{
HIGHP vec4 _pos;

#ifdef VERTEX_NORMAL
	MEDIUMP vec3 _nor = iNormal;
#endif

#ifdef NORMALMAP
	MEDIUMP vec3 _tangent;
	MEDIUMP vec3 _binormal;
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

#ifdef NORMALMAP
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
#ifdef NORMALMAP
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
#ifdef NORMALMAP
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
#ifdef NORMALMAP
    dtan += (vec4(iTangent.xyz, 0.0) * TM).xyz * weight;
    dbin += (vec4(iBinormal.xyz, 0.0) * TM).xyz * weight;
#endif

	_pos = dpos / dpos.w;
	
	#ifdef VERTEX_NORMAL
		_nor = dnor;
	#endif
	
	#ifdef NORMALMAP
		_tangent = dtan;
		_binormal = dbin;
	#endif	
#else
	_pos = vec4(iPos, 1.0);
	#ifdef NORMALMAP
		_tangent = iTangent;
		_binormal = iBinormal;
	#endif
#endif

#if defined WATER
	#ifdef VERTEX_TEXTURE0
		MEDIUMP float animTime = iTex0.y + c_WaveParam.x;
		MEDIUMP float wave = cos(animTime);
		MEDIUMP float waveHeightFactor = (wave + 1.0) * 0.5;
		HIGHP vec4 wavePos = _pos;
		wavePos.y += c_WaveParam1.z * waveHeightFactor * c_WaveParam.y * c_WaveParam.z;
		wavePos.z += wave * c_WaveParam1.w * c_WaveParam.z;
		
		_pos = wavePos;
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
		oTex0 = iTex0;
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

#ifdef NORMALMAP
	oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
	oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
#endif

	oClipDistance.x = 1.0;
	oClipDistance.y = _pos.w;

	gl_Position = _pos;
}
