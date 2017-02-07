precision mediump float;
precision mediump int;

attribute mediump vec3 iPos;
attribute mediump vec3 iPosAni;
#ifdef VERTEX_NORMAL
attribute mediump vec3 iNormal;
#endif
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
attribute mediump vec4 iDiffuse; // if rendering water£¨ r = foam; g = wave; b = wind; a = depth

varying mediump vec4 oTex0;

#ifdef FILTERMAP
varying mediump vec2 oTexFilter;
#endif

#ifdef VERTEX_NORMAL
varying mediump vec3 oNormal;
#endif

#if defined CAMERALIGHT && defined VERTEX_NORMAL
varying mediump	float oCameraLightInten;
#endif

#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
varying mediump vec3 oTangent;
varying mediump vec3 oBinormal;
#endif
varying mediump vec4 oDiffuse;
varying mediump vec4 oViewToVertex;
varying mediump vec2 oClipDistance;

uniform mediump mat4 c_mtxWorld;
uniform mediump vec4 c_BoneList[234];
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

#if defined WATER
varying mediump vec4 oTex1;  //if rendering water£¨ xy = bumpUv1; zw= v_foamUv
varying mediump vec4 oDarkColor;// xyz = dark color, w = reflection power
varying mediump vec4 oLightColor;
varying mediump vec4 oLightMap;
uniform mediump vec4 c_WaveParam; // x:time;y:wave_movement;z:wave_height
uniform mediump vec4 c_WaveParam1;//x:1 / surface width;y:;y: 1 / water surface height;wave movement; w:wave height
uniform mediump vec4 c_LightPos;
uniform mediump vec4 c_ShoreDark;
uniform mediump vec4 c_ShoreLight;
uniform mediump vec4 c_SeaDark;
uniform mediump vec4 c_SeaLight;

#endif

//cbPerScene
uniform highp mat4 c_mtxWVP;
uniform highp mat4 c_mtxWV;
uniform mediump vec4 c_vViewPos;
uniform mediump vec4 c_vReferPos;
uniform mediump vec4 c_ClipPlane;
uniform mediump mat4 c_mtxFilterTM;

#if defined WARFOG
varying mediump vec2 oTexWarFog;
uniform mediump vec4 c_TexCoordOffset;
uniform mediump vec4 c_ZoneOffsetAndSize;
#endif

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

#if defined WATER

	#ifdef VERTEX_TEXTURE0
		mediump float animTime = iTex0.y + c_WaveParam.x;
		mediump float wave = cos(animTime);
		mediump float waveHeightFactor = (wave + 1.0) * 0.5;
		highp vec4 wavePos = _pos;
		wavePos.y += c_WaveParam1.z * waveHeightFactor * c_WaveParam.y * c_WaveParam.z;
		wavePos.z += wave * c_WaveParam1.w * c_WaveParam.z;

		// water alpha
		mediump float maxWaterAlpha = 0.55;
		mediump vec4 vWave;
		vWave.x = 1.0 - (c_WaveParam.w - maxWaterAlpha) * (1.0 / maxWaterAlpha);
		vWave.x = vWave.x * vWave.x;
		vWave.x = vWave.x * 0.8 + 0.2;
		vWave.x = vWave.x - wave * c_WaveParam.y * 0.1;
		vWave.x = min(1.0, vWave.x);

		vWave.y = (cos((_pos.x + c_WaveParam.x) * _pos.z * 0.003 + c_WaveParam.x) +	1.0) * 0.5;

		mediump vec3 lightDir = normalize(vec3(1.0, 0.0, 1.0));
		mediump vec3 lightVec = c_LightPos.xyz - wavePos.xyz;
		vWave.z = 1.0 - clamp(dot(normalize(lightVec), lightDir), 0.0, 1.0);
		vWave.z = vWave.z * 0.2 + (vWave.z * vWave.z) * 0.8;
		vWave.z = vWave.z + 1.1 - length(lightDir) * 0.008;
		vWave.w = 1.0 + (1.0 - vWave.z * 0.5) * 7.0;

		oTex0 = vWave;

		mediump vec2 texcoordMap = vec2(_pos.x * c_WaveParam1.x, _pos.z * c_WaveParam1.y) * 4.0;
		oTex1.xy = texcoordMap + vec2(0.0, c_WaveParam.x * 0.005) * 1.5;
		
		#if defined USE_FOAM
			oTex1.zw = (texcoordMap + vec2(c_WaveParam.x * 0.005)) * 5.5;
		#endif

		mediump float blendFactor = 1.0 - min(1.0, c_WaveParam.w * 1.6);
		mediump float tx = _pos.x * c_WaveParam1.x - 0.5;
		mediump float ty = _pos.z * c_WaveParam1.y - 0.5;
		mediump float tmp = (tx * tx + ty * ty) / (0.75 * 0.75);
		mediump float blendFactorMul = step(1.0, tmp);
		tmp = pow(tmp, 3.0);

		mediump float blendFactor2 = max(blendFactor - (1.0 - tmp) * 0.5, 0.0);
		blendFactor = mix(blendFactor2, blendFactor, blendFactorMul);

		oDarkColor.xyz = mix(c_ShoreDark.xyz, c_SeaDark.xyz, blendFactor);
		oLightColor.xyz = mix(c_ShoreLight.xyz, c_SeaLight.xyz, blendFactor);
	
		#if defined REFLECTION
			oDarkColor.w = log2(((1.0 - c_WaveParam.w) + blendFactor) * 0.5);
		#else
			oDarkColor.w = 0.0;
		#endif	

		#if defined LIGHTMAP
			oLightMap.xy = vec2(wavePos.x * c_WaveParam1.x, wavePos.y * c_WaveParam1.y);
		#endif

		_pos = wavePos;
	#endif

#else
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

	#ifdef VERTEX_TEXTURE1
		oTex0.zw = iTex1.xy * c_LightMapParam.xy + c_LightMapParam.zw;
	#else
		oTex0.z = 0.0;
		oTex0.w = 0.0;
	#endif

	#ifdef VERTEX_NORMAL
		#if defined REFRACTION || defined WATER_SIMPLE
			oNormal = (c_mtxWV * vec4(_nor, 0.0)).xyz;
		#else
			oNormal = (c_mtxWorld * vec4(_nor, 0.0)).xyz;
		#endif
	#endif

	#ifdef SHADOWMAP
		oTexCoordShadow = pos_in_world * c_mtxShadowViewProj;
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

	//#ifdef OPACITY
		oDiffuse.w = iDiffuse.w;
	//#else
	//	oDiffuse.w = 1.0;
	//#endif
	#else
		oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
	#endif

	#if defined CAMERALIGHT && defined VERTEX_NORMAL
		oCameraLightInten = max(dot(-normalize(view_vec.xyz), normalize(oNormal.xyz)), 0.0);
		// ¥”œ¬Õ˘œ¬À•ºı
		oCameraLightInten *= clamp(iPos.y / 1.5, 0.0, 1.0) * 0.5 + 0.5;
	#endif

	#if defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP
		oTangent = (c_mtxWorld * vec4(_tangent, 0.0)).xyz;
		oBinormal = (c_mtxWorld * vec4(_binormal, 0.0)).xyz;
	#endif
#endif

#if defined VIEWVECTOR ||  defined CAMERALIGHT
	vec3 view_vec = pos_in_world.xyz - c_vViewPos.xyz;
#endif

#ifdef VIEWVECTOR
	oViewToVertex = vec4(view_vec, length(view_vec));
#endif
	
	_pos =  c_mtxWVP * _pos;

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

#if defined WARFOG
	vec3 tcd = pos_in_world.xyz;
	tcd += c_TexCoordOffset.xyz;
	oTexWarFog.x = (c_ZoneOffsetAndSize.x - tcd.x) / c_ZoneOffsetAndSize.z;
	oTexWarFog.y = (tcd.z - c_ZoneOffsetAndSize.y) / c_ZoneOffsetAndSize.w;
#endif

#if defined CLIPPLANE
	oClipDistance.x = dot(_pos, c_ClipPlane);
#else
	oClipDistance.x = 1.0;
#endif

#if defined REFRACTION
	oClipDistance.y = _pos.w;
#else
	oClipDistance.y = 1.0;
#endif

	gl_Position = _pos;
}


