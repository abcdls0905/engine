vsin HIGHP vec3 iPos;

vsin HIGHP vec3 iPosAni;

#ifdef VERTEX_NORMAL
	vsin MEDIUMP vec3 iNormal;
#endif

vsin MEDIUMP vec3 iNormalAni;

#if defined VERTEX_TEXTURE0 || defined SABER_ARC
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

#if defined VERTEX_TANGENT || defined SABER_ARC
	vsin MEDIUMP vec3 iTangent;
#endif

#if defined VERTEX_BINORMAL || defined SABER_ARC
	vsin MEDIUMP vec3 iBinormal;
#endif

#ifdef IOS_METAL
vsin MEDIUMP uint iDiffuse;
#else
vsin MEDIUMP vec4 iDiffuse; // if rendering water£¬ r = foam; g = wave; b = wind; a = depth
#endif

vsout MEDIUMP vec4 oTex0;

#ifdef FILTERMAP
	vsout MEDIUMP vec2 oTexFilter;
#endif

#ifdef VERTEX_NORMAL
	vsout MEDIUMP vec3 oNormal;
#endif

#if defined CAMERALIGHT && defined VERTEX_NORMAL
	vsout MEDIUMP	float oCameraLightInten;
#endif

#if (defined SABER_ARC && defined REFRACTION) || (defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP)
	vsout MEDIUMP vec3 oTangent;
	vsout MEDIUMP vec3 oBinormal;
#endif

vsout MEDIUMP vec4 oDiffuse;
vsout HIGHP vec4 oViewToVertex;
vsout HIGHP vec2 oClipDistance;

#ifdef DEPTHMAP
	vsout HIGHP vec4 oFovCoord;
#endif

#ifdef SHADOWMAP
	vsout MEDIUMP vec4 oTexCoordShadow;
#endif

#ifdef SABER_ARC
	#if defined FOGLINEAR || defined FOGEXP || defined REFRACTION
		vsout MEDIUMP float oFogDepth;
	#endif
#else
	#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
		vsout MEDIUMP vec4 oFog;
	#endif
#endif

#if defined WATER
	vsout MEDIUMP vec4 oTex1;  //if rendering water£¬ xy = bumpUv1; zw= v_foamUv
	vsout MEDIUMP vec4 oDarkColor;// xyz = dark color, w = reflection power
	vsout MEDIUMP vec4 oLightColor;
	vsout MEDIUMP vec4 oLightMap;
#endif

#if defined FOAM
	vsout HIGHP vec2 oFoamTex;
#endif

//#if defined WARFOG
//	vsout MEDIUMP vec2 oTexWarFog;
//#endif

#if defined EMISSIVE_SPECIAL
	vsout MEDIUMP vec2 oTexEmissiveSpecial;
#endif

#if defined DYNAMIC_RIPPLE
	vsout MEDIUMP vec2 oRipplesCoord;
#endif

uniform HIGHP mat4 c_mtxWorld;
uniform MEDIUMP vec4 c_BoneList[234];
uniform MEDIUMP float c_fBlendWeight;
uniform MEDIUMP mat4 c_mtxDiffuseTM;
uniform MEDIUMP vec4 c_LightMapParam;
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

#if defined WATER || defined FOAM
	uniform MEDIUMP vec4 c_WaveParam; // x:time;y:wave_movement;z:wave_height // [Foam] x:speed;y:tilling;z:intersect_factor;w:amount
	uniform MEDIUMP vec4 c_WaveParam1;//x:1 / surface width;y:;y: 1 / water surface height;wave movement; w:wave height
	uniform MEDIUMP vec4 c_LightPos;
	uniform MEDIUMP vec4 c_ShoreDark;
	uniform MEDIUMP vec4 c_ShoreLight;
	uniform MEDIUMP vec4 c_SeaDark;
	uniform MEDIUMP vec4 c_SeaLight;
#endif

//cbPerScene
uniform HIGHP mat4 c_mtxWVP;
uniform HIGHP mat4 c_mtxWV;
uniform HIGHP vec4 c_vViewPos;
uniform MEDIUMP vec4 c_vReferPos;
uniform MEDIUMP vec4 c_ClipPlane;
uniform MEDIUMP mat4 c_FilterMatrix;
uniform HIGHP float c_fTime;

#if defined WARFOG
	uniform MEDIUMP vec4 c_TexCoordOffset;
	uniform MEDIUMP vec4 c_ZoneOffsetAndSize;
#endif

#ifdef SABER_ARC
	uniform MEDIUMP vec4 c_vSaberParam;
	uniform MEDIUMP vec4 c_vColorStart;
	uniform MEDIUMP vec4 c_vColorEnd;
#endif

#if defined DYNAMIC_RIPPLE
	uniform HIGHP vec4 c_vRippleParams;
#endif

MEDIUMP vec4 lerp(MEDIUMP vec4 u, MEDIUMP vec4 v, MEDIUMP float t)
{
	return u + (v-u)*t;
}

void main(void)
{
HIGHP vec4 _pos;

#ifdef VERTEX_NORMAL
	MEDIUMP vec3 _nor = iNormal;
#endif

#if (defined SABER_ARC && defined REFRACTION) || (defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP)
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
	
	#if (defined SABER_ARC && defined REFRACTION) || (defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP)
		_tangent = iTangent;
		_binormal = iBinormal;
	#endif
#endif

	HIGHP vec4 pos_in_world = c_mtxWorld * _pos;
	
#ifdef DISAPPEAR
	HIGHP float fDistanceToCamera = distance(pos_in_world.xyz, c_vViewPos.xyz);
	
	#ifdef VERTEX_NORMAL
		pos_in_world.xyz += iNormal * (0.001 * fDistanceToCamera);
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

		// water alpha
		MEDIUMP float maxWaterAlpha = 0.55;
		MEDIUMP vec4 vWave;
		vWave.x = 1.0 - (c_WaveParam.w - maxWaterAlpha) * (1.0 / maxWaterAlpha);
		vWave.x = vWave.x * vWave.x;
		vWave.x = vWave.x * 0.8 + 0.2;
		vWave.x = vWave.x - wave * c_WaveParam.y * 0.1;
		vWave.x = min(1.0, vWave.x);

		vWave.y = (cos((_pos.x + c_WaveParam.x) * _pos.z * 0.003 + c_WaveParam.x) +	1.0) * 0.5;

		MEDIUMP vec3 lightDir = normalize(vec3(1.0, 0.0, 1.0));
		MEDIUMP vec3 lightVec = c_LightPos.xyz - wavePos.xyz;
		vWave.z = 1.0 - clamp(dot(normalize(lightVec), lightDir), 0.0, 1.0);
		vWave.z = vWave.z * 0.2 + (vWave.z * vWave.z) * 0.8;
		vWave.z = vWave.z + 1.1 - length(lightDir) * 0.008;
		vWave.w = 1.0 + (1.0 - vWave.z * 0.5) * 7.0;

		oTex0 = vWave;

		MEDIUMP vec2 texcoordMap = vec2(_pos.x * c_WaveParam1.x, _pos.z * c_WaveParam1.y) * 4.0;
		oTex1.xy = texcoordMap + vec2(0.0, c_WaveParam.x * 0.005) * 1.5;
		
		#if defined USE_FOAM
			oTex1.zw = (texcoordMap + vec2(c_WaveParam.x * 0.005)) * 5.5;
		#endif

		MEDIUMP float blendFactor = 1.0 - min(1.0, c_WaveParam.w * 1.6);
		MEDIUMP float tx = _pos.x * c_WaveParam1.x - 0.5;
		MEDIUMP float ty = _pos.z * c_WaveParam1.y - 0.5;
		MEDIUMP float tmp = (tx * tx + ty * ty) / (0.75 * 0.75);
		MEDIUMP float blendFactorMul = step(1.0, tmp);
		tmp = pow(tmp, 3.0);

		MEDIUMP float blendFactor2 = max(blendFactor - (1.0 - tmp) * 0.5, 0.0);
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
		#ifdef EMISSIVE_SPECIAL
			oTexEmissiveSpecial.x = iTex0.x;
			oTexEmissiveSpecial.y = iTex0.y;
		#endif
		
		#ifdef TEXTUREANI
			oTex0.x = (iTex0.x - 0.5) * c_mtxDiffuseTM[0][0] + (iTex0.y - 0.5) * c_mtxDiffuseTM[1][0] + c_mtxDiffuseTM[2][0] + 0.5;
			oTex0.y = (iTex0.x - 0.5) * c_mtxDiffuseTM[0][1] + (iTex0.y - 0.5) * c_mtxDiffuseTM[1][1] + c_mtxDiffuseTM[2][1] + 0.5;
		#else
			oTex0.x = iTex0.x;
			oTex0.y = iTex0.y;
		#endif
	#elif defined SABER_ARC
		oTex0.xy = iTex0.xy;
	#else
		#ifdef EMISSIVE_SPECIAL
			oTexEmissiveSpecial.x = 0.0;
			oTexEmissiveSpecial.y = 0.0;
		#endif
		
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
	#elif defined SABER_ARC
		oDiffuse = lerp(c_vColorStart, c_vColorEnd, iTex0.y);
	#else
		oDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
	#endif

	#if defined CAMERALIGHT && defined VERTEX_NORMAL
		oCameraLightInten = max(dot(-normalize(view_vec.xyz), normalize(oNormal.xyz)), 0.0);
		// ´ÓÏÂÍùÏÂË¥¼õ
		oCameraLightInten *= clamp(iPos.y / 1.5, 0.0, 1.0) * 0.5 + 0.5;
	#endif

	#if (defined SABER_ARC && defined REFRACTION) || (defined VERTEX_TANGENT && defined VERTEX_BINORMAL && defined NORMALMAP)
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

#ifdef SABER_ARC
	gl_Position = _pos * c_mtxWVP;
#endif

#if defined SABER_ARC && defined SINGLE
	vec3 ViewDir = c_vViewPos.xyz - _pos.xyz;
	
	if (length(iBinormal) > 0.001)
	{
		MEDIUMP float blade_len = c_vSaberParam.x;
		_pos.xyz += normalize(cross(iBinormal, ViewDir)) * (iTex0.x - 0.25) * blade_len;
	}
#endif

#if !defined SABER_ARC && (defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG)
	vec4 fogColor = vec4(0.0, 0.0, 0.0, 1.0);
#endif

#if !defined SABER_ARC && defined FOGEXP
	float fogExp = 1.0 / exp(oViewToVertex.w * c_FogExpParam.w);
	
	fogColor.w = fogExp;
	fogColor.xyz = c_FogExpParam.xyz * (1.0 - fogExp);
#endif
 
#if !defined SABER_ARC && defined FOGLINEAR
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

#if !defined SABER_ARC && (defined FOGLINEAR || defined FOGEXP || defined HEIGHT_FOG)
	oFog = fogColor;
#endif

//#ifdef WARFOG
//	vec3 tcd = pos_in_world.xyz;
//	tcd += c_TexCoordOffset.xyz;
//	oTexWarFog.x = (c_ZoneOffsetAndSize.x - tcd.x) / c_ZoneOffsetAndSize.z;
//	oTexWarFog.y = (tcd.z - c_ZoneOffsetAndSize.y) / c_ZoneOffsetAndSize.w;
//#endif

#ifdef SABER_ARC
	#ifdef REFRACTION
		vec4 vTmp;
		
		vTmp = vec4(normalize(iTangent), 0.0) * c_mtxWVP;
		oTangent = vTmp.xyz / gl_Position.w;
		
		vTmp = vec4(normalize(iBinormal), 0.0) * c_mtxWVP;
		oBinormal = vTmp.xyz / gl_Position.w;
		
		vec4 posWorld = _pos * c_mtxWorld;
		oFogDepth = length(posWorld.xyz - c_vViewPos.xyz);
		
	#elif defined FOGLINEAR || defined FOGEXP
		vec4 posWorld = _pos * c_mtxWorld;
		oFogDepth = length(posWorld.xyz - c_vViewPos.xyz);
	#endif
#else
	#ifdef CLIPPLANE
		oClipDistance.x = dot(_pos, c_ClipPlane);
	#else
		oClipDistance.x = 1.0;
	#endif

	#ifdef REFRACTION
		oClipDistance.y = _pos.w;
	#else
		oClipDistance.y = 1.0;
	#endif
#endif

#if defined DYNAMIC_RIPPLE
	float fSimSize = c_vRippleParams.w;
	float fSnapRange = 5.0;
	vec2 vSimOrig = c_vRippleParams.xz;
	
	vec2 vRipplesPos = vSimOrig - (pos_in_world.xz + c_vReferPos.xz);
	oRipplesCoord = ((vRipplesPos.xy) / fSimSize) * 0.5 + 0.5;
#endif

#if defined FOAM
	oFoamTex = vec2(pos_in_world.xz + c_vReferPos.xz) + c_fTime * c_WaveParam.x;
#endif

#ifndef SABER_ARC
	#ifdef DEPTHMAP
		oFovCoord = c_mtxWVP * _pos;
		gl_Position = oFovCoord;
	#else
		gl_Position = c_mtxWVP * _pos;
	#endif
#endif
}