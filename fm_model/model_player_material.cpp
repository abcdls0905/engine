//--------------------------------------------------------------------
// 文件名:		model_player_material.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "res_model.h"
#include "../utils/array_pod.h"
#include "../public/i_ini_file.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

// 材质键值
enum MATERIAL_KEY_ENUM
{
	MAT_KEY_AMBIENT,
	MAT_KEY_AMBIENTEX,
	MAT_KEY_DIFFUSE,
	MAT_KEY_SPECULAR,
	MAT_KEY_EMISSIVE,

	MAT_KEY_GLOSSINESS,
	MAT_KEY_OPACITY,
	MAT_KEY_ALPHA_REF,
	MAT_KEY_SUBSURFACE_SCALE,
	MAT_KEY_SUBSURFACE_FACTOR,
	MAT_KEY_REFLECT_FACTOR,
	MAT_KEY_BUMP_SCALE,
	MAT_KEY_SATURATION_SCALE,
	MAT_KEY_HEIGHT_SCALE,

	MAT_KEY_SPECULAR_ENABLE,
	MAT_KEY_EMISSIVE_ENABLE,
	MAT_KEY_OPACITY_ENABLE,
	MAT_KEY_REFLECT_ENABLE,
	MAT_KEY_BUMPMAP_ENABLE,
	MAT_KEY_FILTER_ENABLE,
	MAT_KEY_TESSELLATION_ENABLE,
	
	MAT_KEY_ALPHA_TEST,
	MAT_KEY_BLEND,
	MAT_KEY_BLEND_ENHANCE,
	MAT_KEY_BLEND_QUALITY,
	MAT_KEY_NO_Z_WRITE,
	MAT_KEY_NO_Z_TEST,
	MAT_KEY_DOUBLE_SIDE,
	MAT_KEY_BLEND_DOWN_LEVEL,

	MAT_KEY_SAMPLER_CLAMP,
	
	MAT_KEY_UI_FLOAT,
	MAT_KEY_NO_LIGHT,
	MAT_KEY_SPHERE_AMBIENT,
	MAT_KEY_DISAPPEAR,
	MAT_KEY_SKIN_EFFECT,
	MAT_KEY_HAIR_EFFECT,
	
	MAT_KEY_REFRACTION,
	MAT_KEY_APPLIQUE,
	MAT_KEY_SCENE_FOG,
	MAT_KEY_PHASE_MONITOR,

	MAT_KEY_GLOW_ENABLE,
	MAT_KEY_GLOW_ENTIRE,
	MAT_KEY_GLOW_EMISSIVE,
	MAT_KEY_GLOW_EMISSIVEMAP,
	MAT_KEY_GLOW_DIFFUSE,
	MAT_KEY_GLOW_VERTEXCOLOR,
	MAT_KEY_GLOW_APPENDCOLOR,
	MAT_KEY_GLOW_ENHANCE,
	MAT_KEY_GLOW_BLUR,
	MAT_KEY_GLOW_SIZE,
	MAT_KEY_GLOW_ALPHA,

	MAT_KEY_RIM_LIGHT,
	MAT_KEY_RIM_COLOR,
	MAT_KEY_RIM_INTENSITY,
	MAT_KEY_RIM_POWER,

	MAT_KEY_DIFFUSE_MAP,
	MAT_KEY_BUMP_MAP,
	MAT_KEY_SPECULAR_MAP,
	MAT_KEY_SPECULAR_LEVEL_MAP,
	MAT_KEY_GLOSSINESS_MAP,
	MAT_KEY_EMISSIVE_MAP,
	MAT_KEY_REFLECTION_MAP,
	MAT_KEY_DISPLACEMENT_MAP,
	MAT_KEY_LIGHT_MAP,
	MAT_KEY_FILTER_MAP,
};

struct mat_key_index_t
{
	const char* strName;
	unsigned int nHash;
	int nKeyIndex;
};

// 材质键索引表
static mat_key_index_t s_MatKeyIndex[] = {
	"Ambient", 0, MAT_KEY_AMBIENT,
	"AmbientEX", 0, MAT_KEY_AMBIENTEX,
	"Diffuse", 0, MAT_KEY_DIFFUSE,
	"Specular", 0, MAT_KEY_SPECULAR,
	"Emissive", 0, MAT_KEY_EMISSIVE,

	"Glossiness", 0, MAT_KEY_GLOSSINESS,
	"Opacity", 0, MAT_KEY_OPACITY,
	"AlphaRef", 0, MAT_KEY_ALPHA_REF,
	"SubSurfaceScale", 0, MAT_KEY_SUBSURFACE_SCALE,
	"SubSurfaceFactor", 0, MAT_KEY_SUBSURFACE_FACTOR,
	"ReflectFactor", 0, MAT_KEY_REFLECT_FACTOR,
	"BumpScale", 0, MAT_KEY_BUMP_SCALE,
	"SaturationScale", 0, MAT_KEY_SATURATION_SCALE,
	"HeightScale", 0, MAT_KEY_HEIGHT_SCALE,

	"SpecularEnable", 0, MAT_KEY_SPECULAR_ENABLE,
	"EmissiveEnable", 0, MAT_KEY_EMISSIVE_ENABLE,
	"OpacityEnable", 0, MAT_KEY_OPACITY_ENABLE,
	"ReflectEnable", 0, MAT_KEY_REFLECT_ENABLE,
	"BumpMapEnable", 0, MAT_KEY_BUMPMAP_ENABLE,
	"TessellationEnable", 0, MAT_KEY_TESSELLATION_ENABLE,
	
	"AlphaTest", 0, MAT_KEY_ALPHA_TEST,
	"Blend", 0, MAT_KEY_BLEND,
	"BlendEnhance", 0, MAT_KEY_BLEND_ENHANCE,
	"BlendQuality", 0, MAT_KEY_BLEND_QUALITY,
	"NoZWrite", 0, MAT_KEY_NO_Z_WRITE,
	"NoZTest",0, MAT_KEY_NO_Z_TEST,
	"DoubleSide", 0, MAT_KEY_DOUBLE_SIDE,
	"BlendDownLevel", 0, MAT_KEY_BLEND_DOWN_LEVEL,

	"SamplerClamp", 0, MAT_KEY_SAMPLER_CLAMP,

	"UIFloat", 0, MAT_KEY_UI_FLOAT,
	"NoLight", 0, MAT_KEY_NO_LIGHT,
	"SphereAmbient", 0, MAT_KEY_SPHERE_AMBIENT,
	"Disappear", 0, MAT_KEY_DISAPPEAR,
	"SkinEffect", 0, MAT_KEY_SKIN_EFFECT,
	"HairEffect", 0, MAT_KEY_HAIR_EFFECT,
	
	"Refraction", 0, MAT_KEY_REFRACTION,
	"Applique", 0, MAT_KEY_APPLIQUE,
	"SceneFog", 0, MAT_KEY_SCENE_FOG,
	"PhaseMonitor", 0, MAT_KEY_PHASE_MONITOR,

	"GlowEnable", 0, MAT_KEY_GLOW_ENABLE,
	"GlowEntire", 0, MAT_KEY_GLOW_ENTIRE,
	"GlowEmissive", 0, MAT_KEY_GLOW_EMISSIVE,
	"GlowEmissiveMap", 0, MAT_KEY_GLOW_EMISSIVEMAP,
	"GlowDiffuse", 0, MAT_KEY_GLOW_DIFFUSE,
	"GlowVertexColor", 0, MAT_KEY_GLOW_VERTEXCOLOR,
	"GlowAppendColor", 0, MAT_KEY_GLOW_APPENDCOLOR,
	"GlowEnhance", 0, MAT_KEY_GLOW_ENHANCE,
	"GlowBlur", 0, MAT_KEY_GLOW_BLUR,
	"GlowSize", 0, MAT_KEY_GLOW_SIZE,
	"GlowAlpha", 0, MAT_KEY_GLOW_ALPHA,

	"RimLight", 0, MAT_KEY_RIM_LIGHT,
	"RimColor", 0, MAT_KEY_RIM_COLOR,
	"RimIntensity", 0, MAT_KEY_RIM_INTENSITY,
	"RimPower", 0, MAT_KEY_RIM_POWER,

	"DiffuseMap", 0, MAT_KEY_DIFFUSE_MAP,
	"BumpMap", 0, MAT_KEY_BUMP_MAP,
	"SpecularMap", 0, MAT_KEY_SPECULAR_MAP,
	"SpecularLevelMap", 0, MAT_KEY_SPECULAR_LEVEL_MAP,
	"GlossinessMap", 0, MAT_KEY_GLOSSINESS_MAP,
	"EmissiveMap", 0, MAT_KEY_EMISSIVE_MAP,
	"ReflectionMap", 0, MAT_KEY_REFLECTION_MAP,
	"DisplacementMap", 0, MAT_KEY_REFLECTION_MAP,
	"LightMap", 0, MAT_KEY_LIGHT_MAP,
	"FilterMap", 0, MAT_KEY_FILTER_MAP,
};

// 初始化材质键名索引表
void init_material_key_index()
{
	for (int i = 0; i < sizeof(s_MatKeyIndex) / sizeof(mat_key_index_t); ++i)
	{
		s_MatKeyIndex[i].nHash = GetHashValue(s_MatKeyIndex[i].strName);
	}
}

// 初始化材质数据
void init_material_value(material_info_t* pMatInfo)
{
	memset(pMatInfo, 0, sizeof(material_info_t));
	pMatInfo->vAmbient = FmVec3(0.588F, 0.588F, 0.588F);
	pMatInfo->vAmbientEx = FmVec3(0, 0, 0);
	pMatInfo->vDiffuse = FmVec3(0.588F, 0.588F, 0.588F);
	pMatInfo->vSpecular = FmVec3(1.0F, 1.0F, 1.0F);
	pMatInfo->vEmissive = FmVec3(1.0F, 1.0F, 1.0F);
	pMatInfo->fGlossiness = 10.0F;
	pMatInfo->fOpacity = 1.0F;
	pMatInfo->fSubSurfaceScale = 1.0F;
	pMatInfo->fSubSurfaceFactor = 1.0F;
	pMatInfo->fReflectFactor = 1.0F;
	pMatInfo->fBumpScale = 1.0F;
	pMatInfo->fGlowAlpha = 1.0F;
	pMatInfo->fSaturationScale = 1.0F;
	pMatInfo->fHeightScale = 0.1F;
}

/*
// 更新材质的渲染着色标志
void update_material_shader_info(node_material_t* pMat, 
	material_info_t* pMatInfo)
{
	Assert(pMat != NULL);
	Assert(pMatInfo != NULL);

	unsigned int nRet = 0;
	unsigned int nNodeInfo = pMat->pNode->nModelInfo;

	if (nNodeInfo & MODEL_COLOR_INFO)
	{
		nRet |= VERTEXCOLOR_FLAG;
	}

	if (nNodeInfo & MODEL_SKIN_FLAG)
	{
		nRet |= SKELETONANI_FLAG;
	}

	//if (nNodeInfo & MODEL_DISAPPEAR_FLAG)
	//{
	//	nRet |= DISAPPEAR_FLAG;
	//}

	//if (nNodeInfo & MODEL_SCENEFOG_FLAG)
	//{
	//	nRet |= SCENEFOG_FLAG;
	//}

	if ((nNodeInfo & MODEL_POSITION_INFO) && (nNodeInfo & MODEL_NORMAL_INFO))
	{
		nRet |= NORMAL_FLAG;
	}

	if ((nNodeInfo & MODEL_POSITION_ANI) && (nNodeInfo & MODEL_NORMAL_ANI))
	{
		nRet |= NORMAL_FLAG;
		nRet |= VERTEXANI_FLAG;	
	}

	if (pMatInfo->bSpecularEnable)
	{
		nRet |= SPECULAR_FLAG;	
	}

	if (pMatInfo->bEmissiveEnable)
	{
		nRet |= EMISSIVE_FLAG;
	}

	if (pMatInfo->bOpacityEnable)
	{
		nRet |= OPACITY_FLAG;	
	}

	if (pMatInfo->DiffuseMap.pFileName)
	{
		nRet |= DIFFUSEMAP_FLAG;
	}

	if (pMatInfo->BumpMap.pFileName)
	{
		// 顶点里必须有切线信息才可以使用法线贴图
		if ((nNodeInfo & MODEL_TANGENT_INFO) 
			|| (nNodeInfo & MODEL_TANGENT_ANI))
		{
			nRet |= NORMALMAP_FLAG;
		}
	}

	if (pMatInfo->SpecularMap.pFileName 
		|| pMatInfo->SpecularLevelMap.pFileName)
	{
		nRet |= SPECULARMAP_FLAG;
	}

	//if (pMatInfo->SpecularMap.pFileName)
	//{
	//	nRet |= SPECULARMAP_FLAG;
	//}

	//if (pMatInfo->SpecularLevelMap.pFileName)
	//{
	//	nRet |= SPECULARLEVELMAP_FLAG;
	//}

	if (pMatInfo->EmissiveMap.pFileName)
	{
		if (pMatInfo->bEmissiveEnable || pMatInfo->bSkinEffect)
		{
			nRet |= EMISSIVEMAP_FLAG;
		}
	}
	//if (pMat->MatTexs.ReflectionMap.pFileName)
	//{
	//	nRet |= REFLECTION_FLAG;
	//}

	if (pMatInfo->LightMap.pFileName)
	{	
		nRet |= LIGHTMAP_FLAG;
	}

	if (pMat->pFrameDiffuseTM)
	{
		nRet |= TEXTUREANI_FLAG;
	}

	if (pMatInfo->bVolumeTexture)
	{
		nRet |= VOLUMETEXTURE_FLAG;
	}

	if (pMatInfo->bSphereAmbient)
	{
		nRet |= SPHERE_AMBIENT_FLAG;
	}

	if (pMatInfo->bDisappear)
	{
		nRet |= DISAPPEAR_FLAG;
	}

	if (pMatInfo->bSkinEffect)
	{
		nRet |= SKIN_EFFECT_FLAG;
	}

	if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
	{
		nRet |= REFLECTION_FLAG;
	}

	if (pMatInfo->fSaturationScale < 0.99F)
	{
		nRet |= SATURATION_FLAG;
	}

	pMatInfo->nShaderInfo = nRet;
}
*/

// 获得材质键索引
static int get_material_key_index(const char* key)
{
	unsigned int hash = GetHashValue(key);

	for (int i = 0; i < sizeof(s_MatKeyIndex) / sizeof(mat_key_index_t); ++i)
	{
		if ((s_MatKeyIndex[i].nHash == hash)
			&& (stricmp(s_MatKeyIndex[i].strName, key) == 0))
		{
			return s_MatKeyIndex[i].nKeyIndex;
		}
	}

	return -1;
}

// 查找节点的材质
static node_material_t* find_node_material(model_node_t* pNode, 
	const char* mat_name)
{
	if (pNode->nMaterialCount > 0)
	{
		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
		{
			if (stricmp(pNode->Materials[k].pszName, mat_name) == 0)
			{
				return &pNode->Materials[k];
			}
		}
	}
	
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		node_material_t* ret = find_node_material(&pNode->ChildNodes[i], 
			mat_name);

		if (ret)
		{
			return ret;
		}
	}

	return NULL;
}

// 查找模型的材质
node_material_t* CModelPlayer::FindModelMaterial(model_t* pInstance,
	const char* mat_name)
{
	for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
	{
		node_material_t* ret = find_node_material(&pInstance->RootNodes[i], 
			mat_name);

		if (ret)
		{
			return ret;
		}
	}

	return NULL;
}

// 查找节点的所有材质
static void find_node_all_material(model_node_t* pNode, const char* mat_name, 
	TArrayPod<node_material_t*, 8, TCoreAlloc>& all_mat)
{
	if (pNode->nMaterialCount > 0)
	{
		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
		{
			if (stricmp(pNode->Materials[k].pszName, mat_name) == 0)
			{
				all_mat.push_back(&pNode->Materials[k]);
			}
		}
	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		find_node_all_material(&pNode->ChildNodes[i], mat_name, all_mat);
	}
}

// 查找模型的所有材质
static void find_model_all_material(model_t* pInstance, 
	const char* mat_name, TArrayPod<node_material_t*, 8, TCoreAlloc>& all_mat)
{
	for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
	{
		find_node_all_material(&pInstance->RootNodes[i], mat_name, all_mat);
	}
}

// 设置节点的所有材质
static void assign_node_material(model_node_t* pNode, 
	const material_info_t& mat_info)
{
	if (pNode->nMaterialCount > 0)
	{
		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
		{
			node_material_t* pMat = &pNode->Materials[k];

			pMat->MatInfo = mat_info;
		}
	}
	
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		assign_node_material(&pNode->ChildNodes[i], mat_info);
	}
}

// 更新节点的所有材质着色属性
//static void update_node_material_shader(model_node_t* pNode)
//{
//	if (pNode->nMaterialCount > 0)
//	{
//		for (unsigned int k = 0; k < pNode->nMaterialCount; ++k)
//		{
//			node_material_t* pMat = &pNode->Materials[k];
//
//			update_material_shader_info(pMat, &pMat->MatInfo);
//		}
//	}
//
//	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
//	{
//		update_node_material_shader(&pNode->ChildNodes[i]);
//	}
//}

// 设置模型的所有材质
static void assign_model_material(model_t* pInstance, 
	const material_info_t& mat_info)
{
	for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
	{
		assign_node_material(&pInstance->RootNodes[i], mat_info); 
	}
}

// 重新设置贴图文件名
static void assign_texture_name(texture_info_t* pTexInfo, const char* tex_name)
{
	if (pTexInfo->pFileName)
	{
		CORE_FREE(pTexInfo->pFileName, pTexInfo->nFileNameLen + 1);
		pTexInfo->pFileName = NULL;
	}

	pTexInfo->nFileNameLen = 0;

	if (tex_name[0] != 0)
	{
		pTexInfo->nFileNameLen = (unsigned int)strlen(tex_name);
		pTexInfo->pFileName = (char*)CORE_ALLOC(pTexInfo->nFileNameLen + 1);
		memcpy(pTexInfo->pFileName, tex_name, pTexInfo->nFileNameLen + 1);
	}
}

static void assign_diffuse_texture_name(texture_info_t* pTexInfo, 
	const char* tex_name, material_info_t* pMatInfo)
{
	assign_texture_name(pTexInfo, tex_name);

	pMatInfo->bForbidStandModel = false;

	if (pTexInfo->nFileNameLen >= strlen("forbid_stand"))
	{
		if (NULL != pTexInfo->pFileName)
		{
			if (NULL != strstr(pTexInfo->pFileName, "forbid_stand"))
			{
				pMatInfo->bForbidStandModel = true;
			}
		}
	}

//#ifdef FXVERSION_MERGE004
	pMatInfo->bVolumeTexture = false;

	if (pTexInfo->nFileNameLen >= 4)
	{
		if ('v' == pTexInfo->pFileName[0] && 'o' == pTexInfo->pFileName[1] 
			&& 'l' == pTexInfo->pFileName[2] && '_' == pTexInfo->pFileName[3])
		{
			pMatInfo->bVolumeTexture = true;
		}
	}
//#endif
}

// 字符串转换为颜色向量
static FmVec3 string_to_vector(const char* s)
{
	char buf[32];

	CopyString(buf, sizeof(buf), s);

	char* s1 = buf;
	char* split1 = strchr(s1, ',');

	if (NULL == split1)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}

	*split1 = 0;

	char* s2 = split1 + 1;
	char* split2 = strchr(s2, ',');

	if (NULL == split2)
	{
		return FmVec3(0.0F, 0.0F, 0.0F);
	}

	*split2 = 0;
	
	char* s3 = split2 + 1;

	float x = (float)atof(s1);
	float y = (float)atof(s2);
	float z = (float)atof(s3);

	return FmVec3(x, y, z);
}

// 颜色向量转换为字符串
static void vector_to_string(const FmVec3& v, char* buffer, int size)
{
	SafeSprintf(buffer, size, "%.3f,%.3f,%.3f", v.x, v.y, v.z);
}

// 浮点数转换为字符串
static void float_to_string(float v, char* buffer, int size)
{
	SafeSprintf(buffer, size, "%f", v);
}

// 布尔值转换为字符串
static void bool_to_string(bool v, char* buffer, int size)
{
	if (v)
	{
		SafeSprintf(buffer, size, "true");
	}
	else
	{
		SafeSprintf(buffer, size, "false");
	}
}

// 整数值转换为字符串
static void int_to_string(int v, char* buffer, int size)
{
	SafeSprintf(buffer, size, "%d", v);
}

// 贴图信息转换字符串
static void texture_to_string(texture_info_t* pTexInfo, char* buffer, int size)
{
	if (pTexInfo->pFileName)
	{
		CopyString(buffer, size, pTexInfo->pFileName);
	}
	else
	{
		buffer[0] = 0;
	}
}

// 设置材质数据
static bool set_material_value(material_info_t* pMatInfo, const char* key, 
	const char* val)
{
	int key_index = get_material_key_index(key);

	if (key_index < 0)
	{
		return false;
	}

	switch (key_index)
	{
	case MAT_KEY_AMBIENT:
		pMatInfo->vAmbient = string_to_vector(val);
		break;
	case MAT_KEY_AMBIENTEX:
		pMatInfo->vAmbientEx = string_to_vector(val);
		break;
	case MAT_KEY_DIFFUSE:
		pMatInfo->vDiffuse = string_to_vector(val);
		break;
	case MAT_KEY_SPECULAR:
		pMatInfo->vSpecular = string_to_vector(val);
		break;
	case MAT_KEY_EMISSIVE:
		pMatInfo->vEmissive = string_to_vector(val);
		break;
	case MAT_KEY_GLOSSINESS:
		pMatInfo->fGlossiness = (float)atof(val);
		break;
	case MAT_KEY_OPACITY:
		pMatInfo->fOpacity = (float)atof(val);
		break;
	case MAT_KEY_ALPHA_REF:
		pMatInfo->fAlphaRef = (float)atof(val);
		break;
	case MAT_KEY_SUBSURFACE_SCALE:
		pMatInfo->fSubSurfaceScale = (float)atof(val);
		break;
	case MAT_KEY_SUBSURFACE_FACTOR:
		pMatInfo->fSubSurfaceFactor = (float)atof(val);
		break;
	case MAT_KEY_REFLECT_FACTOR:
		pMatInfo->fReflectFactor = (float)atof(val);
		break;
	case MAT_KEY_BUMP_SCALE:
		pMatInfo->fBumpScale = (float)atof(val);
		break;
	case MAT_KEY_SATURATION_SCALE:
		pMatInfo->fSaturationScale = (float)atof(val);
		break;
	case MAT_KEY_HEIGHT_SCALE:
		pMatInfo->fHeightScale = (float)atof(val);
		break;
	case MAT_KEY_SPECULAR_ENABLE:
		pMatInfo->bSpecularEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_EMISSIVE_ENABLE:
		pMatInfo->bEmissiveEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_OPACITY_ENABLE:
		pMatInfo->bOpacityEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_REFLECT_ENABLE:
		pMatInfo->bReflectEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_BUMPMAP_ENABLE:
		pMatInfo->bBumpMapEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_FILTER_ENABLE:
		pMatInfo->bFilterEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_TESSELLATION_ENABLE:
		pMatInfo->bTessellationEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_ALPHA_TEST:
		pMatInfo->bAlphaTest = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_BLEND:
		pMatInfo->bBlend = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_BLEND_ENHANCE:
		pMatInfo->bBlendEnhance = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_BLEND_QUALITY:
		pMatInfo->bBlendQuality = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_NO_Z_WRITE:
		pMatInfo->bNoZWrite = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_NO_Z_TEST:
		pMatInfo->bNoZTest = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_DOUBLE_SIDE:
		pMatInfo->bDoubleSide = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_BLEND_DOWN_LEVEL:
		pMatInfo->bBlendDownLevel = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_SAMPLER_CLAMP:
		pMatInfo->bSamplerClamp = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_UI_FLOAT:
		pMatInfo->bUIFloat = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_NO_LIGHT:
		pMatInfo->bNoLight = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_SPHERE_AMBIENT:
		pMatInfo->bSphereAmbient = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_DISAPPEAR:
		pMatInfo->bDisappear = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_SKIN_EFFECT:
		pMatInfo->bSkinEffect = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_HAIR_EFFECT:
		pMatInfo->bHairEffect = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_REFRACTION:
		pMatInfo->bRefraction = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_APPLIQUE:
		pMatInfo->bApplique = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_SCENE_FOG:
		pMatInfo->bSceneFog = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_PHASE_MONITOR:
		pMatInfo->bPhaseMonitor = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_ENABLE:
		pMatInfo->bGlowEnable = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_ENTIRE:
		pMatInfo->bGlowEntire = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_EMISSIVE:
		pMatInfo->bGlowEmissive = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_EMISSIVEMAP:
		pMatInfo->bGlowEmissiveMap = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_DIFFUSE:
		pMatInfo->bGlowDiffuse = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_VERTEXCOLOR:
		pMatInfo->bGlowVertexColor = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_APPENDCOLOR:
		pMatInfo->bGlowAppendColor = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_ENHANCE:
		pMatInfo->bGlowEnhance = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_BLUR:
		pMatInfo->bGlowBlur = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_GLOW_SIZE:
		pMatInfo->fGlowSize = (float)atof(val);
		break;
	case MAT_KEY_GLOW_ALPHA:
		pMatInfo->fGlowAlpha = (float)atof(val);
		break;
	case MAT_KEY_RIM_LIGHT:
		pMatInfo->bRimLight = stricmp(val, "true") == 0;
		break;
	case MAT_KEY_RIM_COLOR:
		pMatInfo->vRimColor = string_to_vector(val);
		break;
	case MAT_KEY_RIM_INTENSITY:
		pMatInfo->fRimIntensity = (float)atof(val);
		break;
	case MAT_KEY_RIM_POWER:
		pMatInfo->fRimPower = (float)atof(val);
		break;
	case MAT_KEY_DIFFUSE_MAP:
		//assign_texture_name(&pMatInfo->DiffuseMap, val);
		assign_diffuse_texture_name(&pMatInfo->DiffuseMap, val, pMatInfo);
		break;
	case MAT_KEY_BUMP_MAP:
		assign_texture_name(&pMatInfo->BumpMap, val);
		break;
	case MAT_KEY_SPECULAR_MAP:
		assign_texture_name(&pMatInfo->SpecularMap, val);
		break;
	case MAT_KEY_SPECULAR_LEVEL_MAP:
		assign_texture_name(&pMatInfo->SpecularLevelMap, val);
		break;
	case MAT_KEY_GLOSSINESS_MAP:
		assign_texture_name(&pMatInfo->GlossinessMap, val);
		break;
	case MAT_KEY_EMISSIVE_MAP:
		assign_texture_name(&pMatInfo->EmissiveMap, val);
		break;
	case MAT_KEY_REFLECTION_MAP:
		assign_texture_name(&pMatInfo->ReflectionMap, val);
		break;
	case MAT_KEY_DISPLACEMENT_MAP:
		assign_texture_name(&pMatInfo->DisplacementMap, val);
		break;
	case MAT_KEY_LIGHT_MAP:
		assign_texture_name(&pMatInfo->LightMap, val);
		break;
	case MAT_KEY_FILTER_MAP:
		assign_texture_name(&pMatInfo->FilterMap, val);
		break;
	default:
		Assert(0);
		break;
	}

	return true;
}

// 获得模型材质数据
static bool get_material_value(material_info_t* pMatInfo, const char* key, 
	char* val, unsigned int val_size)
{
	int key_index = get_material_key_index(key);

	if (key_index < 0)
	{
		return false;
	}

	switch (key_index)
	{
	case MAT_KEY_AMBIENT:
		vector_to_string(pMatInfo->vAmbient, val, val_size);
		break;
	case MAT_KEY_DIFFUSE:
		vector_to_string(pMatInfo->vDiffuse, val, val_size);
		break;
	case MAT_KEY_SPECULAR:
		vector_to_string(pMatInfo->vSpecular, val, val_size);
		break;
	case MAT_KEY_EMISSIVE:
		vector_to_string(pMatInfo->vEmissive, val, val_size);
		break;
	case MAT_KEY_GLOSSINESS:
		float_to_string(pMatInfo->fGlossiness, val, val_size);
		break;
	case MAT_KEY_OPACITY:
		float_to_string(pMatInfo->fOpacity, val, val_size);
		break;
	case MAT_KEY_ALPHA_REF:
		float_to_string(pMatInfo->fAlphaRef, val, val_size);
		break;
	case MAT_KEY_SUBSURFACE_SCALE:
		float_to_string(pMatInfo->fSubSurfaceScale, val, val_size);
		break;
	case MAT_KEY_SUBSURFACE_FACTOR:
		float_to_string(pMatInfo->fSubSurfaceFactor, val, val_size);
		break;
	case MAT_KEY_REFLECT_FACTOR:
		float_to_string(pMatInfo->fReflectFactor, val, val_size);
		break;
	case MAT_KEY_BUMP_SCALE:
		float_to_string(pMatInfo->fBumpScale, val, val_size);
		break;
	case MAT_KEY_SATURATION_SCALE:
		float_to_string(pMatInfo->fSaturationScale, val, val_size);
		break;
	case MAT_KEY_HEIGHT_SCALE:
		float_to_string(pMatInfo->fHeightScale, val, val_size);
		break;
	case MAT_KEY_SPECULAR_ENABLE:
		bool_to_string(pMatInfo->bSpecularEnable, val, val_size);
		break;
	case MAT_KEY_EMISSIVE_ENABLE:
		bool_to_string(pMatInfo->bEmissiveEnable, val, val_size);
		break;
	case MAT_KEY_OPACITY_ENABLE:
		bool_to_string(pMatInfo->bOpacityEnable, val, val_size);
		break;
	case MAT_KEY_REFLECT_ENABLE:
		bool_to_string(pMatInfo->bReflectEnable, val, val_size);
		break;
	case MAT_KEY_BUMPMAP_ENABLE:
		bool_to_string(pMatInfo->bBumpMapEnable, val, val_size);
		break;
	case MAT_KEY_FILTER_ENABLE:
		bool_to_string(pMatInfo->bFilterEnable, val, val_size);
		break;
	case MAT_KEY_TESSELLATION_ENABLE:
		bool_to_string(pMatInfo->bTessellationEnable, val, val_size);
		break;
	case MAT_KEY_ALPHA_TEST:
		bool_to_string(pMatInfo->bAlphaTest, val, val_size);
		break;
	case MAT_KEY_BLEND:
		bool_to_string(pMatInfo->bBlend, val, val_size);
		break;
	case MAT_KEY_BLEND_ENHANCE:
		bool_to_string(pMatInfo->bBlendEnhance, val, val_size);
		break;
	case MAT_KEY_BLEND_QUALITY:
		bool_to_string(pMatInfo->bBlendQuality, val, val_size);
		break;
	case MAT_KEY_NO_Z_WRITE:
		bool_to_string(pMatInfo->bNoZWrite, val, val_size);
		break;
	case MAT_KEY_NO_Z_TEST:
		bool_to_string(pMatInfo->bNoZTest, val, val_size);
		break;
	case MAT_KEY_DOUBLE_SIDE:
		bool_to_string(pMatInfo->bDoubleSide, val, val_size);
		break;
	case MAT_KEY_BLEND_DOWN_LEVEL:
		bool_to_string(pMatInfo->bBlendDownLevel, val, val_size);
		break;
	case MAT_KEY_SAMPLER_CLAMP:
		bool_to_string(pMatInfo->bSamplerClamp, val, val_size);
		break;
	case MAT_KEY_UI_FLOAT:
		bool_to_string(pMatInfo->bUIFloat, val, val_size);
		break;
	case MAT_KEY_NO_LIGHT:
		bool_to_string(pMatInfo->bNoLight, val, val_size);
		break;
	case MAT_KEY_SPHERE_AMBIENT:
		bool_to_string(pMatInfo->bSphereAmbient, val, val_size);
		break;
	case MAT_KEY_DISAPPEAR:
		bool_to_string(pMatInfo->bDisappear, val, val_size);
		break;
	case MAT_KEY_SKIN_EFFECT:
		bool_to_string(pMatInfo->bSkinEffect, val, val_size);
		break;
	case MAT_KEY_HAIR_EFFECT:
		bool_to_string(pMatInfo->bHairEffect, val, val_size);
		break;
	case MAT_KEY_REFRACTION:
		bool_to_string(pMatInfo->bRefraction, val, val_size);
		break;
	case MAT_KEY_APPLIQUE:
		bool_to_string(pMatInfo->bApplique, val, val_size);
		break;
	case MAT_KEY_SCENE_FOG:
		bool_to_string(pMatInfo->bSceneFog, val, val_size);
		break;
	case MAT_KEY_PHASE_MONITOR:
		bool_to_string(pMatInfo->bPhaseMonitor, val, val_size);
		break;
	case MAT_KEY_GLOW_ENABLE:
		bool_to_string(pMatInfo->bGlowEnable, val, val_size);
		break;
	case MAT_KEY_GLOW_ENTIRE:
		bool_to_string(pMatInfo->bGlowEntire, val, val_size);
		break;
	case MAT_KEY_GLOW_EMISSIVE:
		bool_to_string(pMatInfo->bGlowEmissive, val, val_size);
		break;
	case MAT_KEY_GLOW_EMISSIVEMAP:
		bool_to_string(pMatInfo->bGlowEmissiveMap, val, val_size);
		break;
	case MAT_KEY_GLOW_DIFFUSE:
		bool_to_string(pMatInfo->bGlowDiffuse, val, val_size);
		break;
	case MAT_KEY_GLOW_VERTEXCOLOR:
		bool_to_string(pMatInfo->bGlowVertexColor, val, val_size);
		break;
	case MAT_KEY_GLOW_APPENDCOLOR:
		bool_to_string(pMatInfo->bGlowAppendColor, val, val_size);
		break;
	case MAT_KEY_GLOW_ENHANCE:
		bool_to_string(pMatInfo->bGlowEnhance, val, val_size);
		break;
	case MAT_KEY_GLOW_BLUR:
		bool_to_string(pMatInfo->bGlowBlur, val, val_size);
		break;
	case MAT_KEY_GLOW_SIZE:
		float_to_string(pMatInfo->fGlowSize, val, val_size);
		break;
	case MAT_KEY_GLOW_ALPHA:
		float_to_string(pMatInfo->fGlowAlpha, val, val_size);
		break;
	case MAT_KEY_RIM_LIGHT:
		bool_to_string(pMatInfo->bRimLight, val, val_size);
		break;
	case MAT_KEY_RIM_COLOR:
		vector_to_string(pMatInfo->vRimColor, val, val_size);
		break;
	case MAT_KEY_RIM_INTENSITY:
		float_to_string(pMatInfo->fRimIntensity, val, val_size);
		break;
	case MAT_KEY_RIM_POWER:
		float_to_string(pMatInfo->fRimPower, val, val_size);
		break;
	case MAT_KEY_DIFFUSE_MAP:
		texture_to_string(&pMatInfo->DiffuseMap, val, val_size);
		break;
	case MAT_KEY_BUMP_MAP:
		texture_to_string(&pMatInfo->BumpMap, val, val_size);
		break;
	case MAT_KEY_SPECULAR_MAP:
		texture_to_string(&pMatInfo->SpecularMap, val, val_size);
		break;
	case MAT_KEY_SPECULAR_LEVEL_MAP:
		texture_to_string(&pMatInfo->SpecularLevelMap, val, val_size);
		break;
	case MAT_KEY_GLOSSINESS_MAP:
		texture_to_string(&pMatInfo->GlossinessMap, val, val_size);
		break;
	case MAT_KEY_EMISSIVE_MAP:
		texture_to_string(&pMatInfo->EmissiveMap, val, val_size);
		break;
	case MAT_KEY_REFLECTION_MAP:
		texture_to_string(&pMatInfo->ReflectionMap, val, val_size);
		break;
	case MAT_KEY_DISPLACEMENT_MAP:
		texture_to_string(&pMatInfo->DisplacementMap, val, val_size);
		break;
	case MAT_KEY_LIGHT_MAP:
		texture_to_string(&pMatInfo->LightMap, val, val_size);
		break;
	case MAT_KEY_FILTER_MAP:
		texture_to_string(&pMatInfo->FilterMap, val, val_size);
		break;
	default:
		Assert(0);
		break;
	}

	return true;
}

// 释放贴图名
static void free_tex_file_name(material_info_t* pMatInfo)
{
	if (pMatInfo->DiffuseMap.pFileName)
	{
		CORE_FREE(pMatInfo->DiffuseMap.pFileName,
			pMatInfo->DiffuseMap.nFileNameLen + 1);
		pMatInfo->DiffuseMap.pFileName = NULL;
	}

	if (pMatInfo->BumpMap.pFileName)
	{
		CORE_FREE(pMatInfo->BumpMap.pFileName,
			pMatInfo->BumpMap.nFileNameLen + 1);
		pMatInfo->BumpMap.pFileName = NULL;
	}

	if (pMatInfo->SpecularMap.pFileName)
	{
		CORE_FREE(pMatInfo->SpecularMap.pFileName,
			pMatInfo->SpecularMap.nFileNameLen + 1);
		pMatInfo->SpecularMap.pFileName = NULL;
	}

	if (pMatInfo->SpecularLevelMap.pFileName)
	{
		CORE_FREE(pMatInfo->SpecularLevelMap.pFileName,
			pMatInfo->SpecularLevelMap.nFileNameLen + 1);
		pMatInfo->SpecularLevelMap.pFileName = NULL;
	}

	if (pMatInfo->GlossinessMap.pFileName)
	{
		CORE_FREE(pMatInfo->GlossinessMap.pFileName,
			pMatInfo->GlossinessMap.nFileNameLen + 1);
		pMatInfo->GlossinessMap.pFileName = NULL;
	}

	if (pMatInfo->EmissiveMap.pFileName)
	{
		CORE_FREE(pMatInfo->EmissiveMap.pFileName,
			pMatInfo->EmissiveMap.nFileNameLen + 1);
		pMatInfo->EmissiveMap.pFileName = NULL;
	}

	if (pMatInfo->ReflectionMap.pFileName)
	{
		CORE_FREE(pMatInfo->ReflectionMap.pFileName,
			pMatInfo->ReflectionMap.nFileNameLen + 1);
		pMatInfo->ReflectionMap.pFileName = NULL;
	}

	if (pMatInfo->DisplacementMap.pFileName)
	{
		CORE_FREE(pMatInfo->DisplacementMap.pFileName,
			pMatInfo->DisplacementMap.nFileNameLen + 1);
		pMatInfo->DisplacementMap.pFileName = NULL;
	}

	if (pMatInfo->LightMap.pFileName)
	{
		CORE_FREE(pMatInfo->LightMap.pFileName,
			pMatInfo->LightMap.nFileNameLen + 1);
		pMatInfo->LightMap.pFileName = NULL;
	}
	if (pMatInfo->FilterMap.pFileName)
	{
		CORE_FREE(pMatInfo->FilterMap.pFileName,
			pMatInfo->FilterMap.nFileNameLen + 1);
		pMatInfo->FilterMap.pFileName = NULL;
	}
}

// 加载材质定义文件
static bool load_material_file(model_t* pInstance, 
	const char* file_name)
{
	IIniFile* pIni = g_pCore->CreateIniFile(file_name);

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		return false;
	}

	size_t sect_num = pIni->GetSectionCount();

	for (size_t i = 0; i < sect_num; ++i)
	{
		const char* sect_name = pIni->GetSectionByIndex(i);
		// 节点的材质数据
		TArrayPod<node_material_t*, 8, TCoreAlloc> all_mat;

		// 指定节点的所有材质数据
		find_model_all_material(pInstance, sect_name, all_mat);

		size_t all_mat_num = all_mat.size();

		if (0 == all_mat_num)
		{
			continue;
		}

		for (size_t m = 0; m < all_mat_num; ++m)
		{
			material_info_t* pMatInfo = &all_mat[m]->MatInfo;
			
			// 必须要先释放内存
			free_tex_file_name(pMatInfo);
			// 先设置缺省值，保证不使用模型的原始材质信息，而完全由材质文件来控制
			init_material_value(pMatInfo);
		}
		
		size_t item_num = pIni->GetSectionItemCount(i);

		for (size_t index = 0; index < all_mat_num; ++index)
		{
			node_material_t* pMat = all_mat[index];
			
			for (size_t k = 0; k < item_num; ++k)
			{
				const char* key = pIni->GetSectionItemKey(i, k);
				const char* val = pIni->GetSectionItemValue(i, k);

				set_material_value(&pMat->MatInfo, key, val);
			}

			// 更新着色器信息
			//update_material_shader_info(pMat, &pMat->MatInfo);
		}
	}

	pIni->Release();

	return true;
}

// 加载模型材质文件
bool load_model_material_file(model_t* pModel, const char* pszModelName)
{
	Assert(pModel != NULL);
	Assert(pszModelName != NULL);

	size_t name_size = strlen(pszModelName);
	TAutoMem<char, 256, TCoreAlloc> auto_buf(name_size + 1);
	char* fname = auto_buf.GetBuffer();

	memcpy(fname, pszModelName, name_size + 1);

	char* dot = strrchr(fname, '.');

	if (NULL == dot)
	{
		return false;
	}

	CopyString(dot, name_size + 1 - (dot - fname), ".mtl");

	return load_material_file(pModel, fname);
}

// 加载模型引用的指定材质文件
bool CModelPlayer::LoadCustomMaterialFile(IIniFile* pIni)
{
	Assert(pIni != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::LoadCustomMaterialFile] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
    }

    UnloadCustomMaterials();

	size_t sect_num = pIni->GetSectionCount();

	if (0 == sect_num)
	{
		// 不使用另外指定的材质
		return true;
	}

	m_pCustomMaterials = (custom_material_t*)CORE_ALLOC(
		sizeof(custom_material_t) * sect_num);
	m_nCustomMaterialNum = (unsigned int)sect_num;
	memset(m_pCustomMaterials, 0, sizeof(custom_material_t) * sect_num);

	for (size_t i = 0; i < sect_num; ++i)
	{
		material_info_t* pMatInfo = &m_pCustomMaterials[i].MatInfo;

		init_material_value(pMatInfo);

		const char* sect_name = pIni->GetSectionByIndex(i);
		node_material_t* pMat = FindModelMaterial(pModel, sect_name);

		if (NULL == pMat)
		{
			// 找不到对应的模型材质
			continue;
		}

		// 直接用材质名指针的值，这样可以提高渲染时查找的速度
		m_pCustomMaterials[i].pMaterialName = pMat->pszName;

		size_t item_num = pIni->GetSectionItemCount(i);

		for (size_t k = 0; k < item_num; ++k)
		{
			const char* key = pIni->GetSectionItemKey(i, k);
			const char* val = pIni->GetSectionItemValue(i, k);

			set_material_value(pMatInfo, key, val);
		}
		
		// 更新着色器信息
		//update_material_shader_info(pMat, pMatInfo);
	}

	return true;
}

// 删除模型引用的指定材质
bool CModelPlayer::UnloadCustomMaterials()
{
	if (m_nCustomMaterialNum > 0)
	{
		UnloadCustomTextures();

		for (unsigned int k = 0; k < m_nCustomMaterialNum; ++k)
		{
			material_info_t* pMatInfo = &m_pCustomMaterials[k].MatInfo;

			free_tex_file_name(pMatInfo);
		}

		CORE_FREE(m_pCustomMaterials, 
			sizeof(custom_material_t) * m_nCustomMaterialNum);
		m_pCustomMaterials = NULL;
		m_nCustomMaterialNum = 0;
	}

	return true;
}

// 查找自定义材质
material_info_t* CModelPlayer::InnerFindCustomMaterial(node_material_t* pMat)
{
	for (size_t i = 0; i < m_nCustomMaterialNum; ++i)
	{
		if (m_pCustomMaterials[i].pMaterialName == pMat->pszName)
		{
			return &m_pCustomMaterials[i].MatInfo;
		}
	}

	return NULL;
}

// 增加自定义材质
material_info_t* CModelPlayer::InnerAddCustomMaterial(node_material_t* pMat)
{
	custom_material_t* pOldCustomMaterials = m_pCustomMaterials;
	int new_index = m_nCustomMaterialNum;

	m_nCustomMaterialNum++;	
	m_pCustomMaterials = (custom_material_t*)CORE_ALLOC(
		sizeof(custom_material_t) * (new_index + 1));

	if (NULL != pOldCustomMaterials)
	{
		memcpy(m_pCustomMaterials, pOldCustomMaterials, 
			sizeof(custom_material_t) * new_index);
		CORE_FREE(pOldCustomMaterials, sizeof(custom_material_t) * new_index);
	}

	material_info_t* pMatInfo = &m_pCustomMaterials[new_index].MatInfo;

	m_pCustomMaterials[new_index].pMaterialName = pMat->pszName;
	memcpy(pMatInfo, &pMat->MatInfo, sizeof(material_info_t));

	if (pMatInfo->DiffuseMap.pFileName != NULL)
	{
		pMatInfo->DiffuseMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->DiffuseMap.nFileNameLen + 1);
		memcpy(pMatInfo->DiffuseMap.pFileName, 
			pMat->MatInfo.DiffuseMap.pFileName, 
			sizeof(char) * (pMatInfo->DiffuseMap.nFileNameLen + 1));
		pMatInfo->DiffuseMap.pTex = NULL;
	}

	if (pMatInfo->BumpMap.pFileName != NULL)
	{
		pMatInfo->BumpMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->BumpMap.nFileNameLen + 1);
		memcpy(pMatInfo->BumpMap.pFileName, 
			pMat->MatInfo.BumpMap.pFileName, 
			sizeof(char) * (pMatInfo->BumpMap.nFileNameLen + 1));
		pMatInfo->BumpMap.pTex = NULL;
	}

	if (pMatInfo->LightMap.pFileName != NULL)
	{
		pMatInfo->LightMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->LightMap.nFileNameLen + 1);
		memcpy(pMatInfo->LightMap.pFileName, 
			pMat->MatInfo.LightMap.pFileName, 
			sizeof(char) * (pMatInfo->LightMap.nFileNameLen + 1));
		pMatInfo->LightMap.pTex = NULL;
	}

	if (pMatInfo->FilterMap.pFileName != NULL)
	{
		pMatInfo->FilterMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->FilterMap.nFileNameLen + 1);
		memcpy(pMatInfo->FilterMap.pFileName, 
			pMat->MatInfo.FilterMap.pFileName, 
			sizeof(char) * (pMatInfo->FilterMap.nFileNameLen + 1));
		pMatInfo->FilterMap.pTex = NULL;
	}


	if (pMatInfo->SpecularMap.pFileName != NULL)
	{
		pMatInfo->SpecularMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->SpecularMap.nFileNameLen + 1);
		memcpy(pMatInfo->SpecularMap.pFileName, 
			pMat->MatInfo.SpecularMap.pFileName, 
			sizeof(char) * (pMatInfo->SpecularMap.nFileNameLen + 1));
		pMatInfo->SpecularMap.pTex = NULL;
	}

	if (pMatInfo->SpecularLevelMap.pFileName != NULL)
	{
		pMatInfo->SpecularLevelMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->SpecularLevelMap.nFileNameLen + 1);
		memcpy(pMatInfo->SpecularLevelMap.pFileName, 
			pMat->MatInfo.SpecularLevelMap.pFileName, 
			sizeof(char) * (pMatInfo->SpecularLevelMap.nFileNameLen + 1));
		pMatInfo->SpecularLevelMap.pTex = NULL;
	}

	if (pMatInfo->GlossinessMap.pFileName != NULL)
	{
		pMatInfo->GlossinessMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->GlossinessMap.nFileNameLen + 1);
		memcpy(pMatInfo->GlossinessMap.pFileName, 
			pMat->MatInfo.GlossinessMap.pFileName, 
			sizeof(char) * (pMatInfo->GlossinessMap.nFileNameLen + 1));
		pMatInfo->GlossinessMap.pTex = NULL;
	}

	if (pMatInfo->EmissiveMap.pFileName != NULL)
	{
		pMatInfo->EmissiveMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->EmissiveMap.nFileNameLen + 1);
		memcpy(pMatInfo->EmissiveMap.pFileName, 
			pMat->MatInfo.EmissiveMap.pFileName, 
			sizeof(char) * (pMatInfo->EmissiveMap.nFileNameLen + 1));
		pMatInfo->EmissiveMap.pTex = NULL;
	}

	if (pMatInfo->ReflectionMap.pFileName != NULL)
	{
		pMatInfo->ReflectionMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->ReflectionMap.nFileNameLen + 1);
		memcpy(pMatInfo->ReflectionMap.pFileName, 
			pMat->MatInfo.ReflectionMap.pFileName, 
			sizeof(char) * (pMatInfo->ReflectionMap.nFileNameLen + 1));
		pMatInfo->ReflectionMap.pTex = NULL;
	}

	if (pMatInfo->DisplacementMap.pFileName != NULL)
	{
		pMatInfo->DisplacementMap.pFileName = (char*)CORE_ALLOC(
			pMatInfo->DisplacementMap.nFileNameLen + 1);
		memcpy(pMatInfo->DisplacementMap.pFileName, 
			pMat->MatInfo.DisplacementMap.pFileName, 
			sizeof(char) * (pMatInfo->DisplacementMap.nFileNameLen + 1));
		pMatInfo->DisplacementMap.pTex = NULL;
	}

	return pMatInfo;
}

bool CModelPlayer::FindCustomMaterial(const char* mat_name)
{
	Assert(mat_name != NULL);

	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::FindCustomMaterial] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	material_info_t* pMatInfo = InnerFindCustomMaterial(pMat);

	if (NULL == pMatInfo)
	{
		return false;
	}

	return true;
}

// 设置自定义材质数据
bool CModelPlayer::SetCustomMaterialValue(const char* mat_name,
	const char* key, const char* val)
{
	Assert(mat_name != NULL);
	Assert(key != NULL);
	Assert(val != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetCustomMaterialValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	material_info_t* pMatInfo = InnerFindCustomMaterial(pMat);

	if (NULL == pMatInfo)
	{
		pMatInfo = InnerAddCustomMaterial(pMat);
	}

	if (!set_material_value(pMatInfo, key, val))
	{
		return false;
	}

	//update_material_shader_info(pMat, pMatInfo);

	return true;
}

// 获得自定义材质数据
bool CModelPlayer::GetCustomMaterialValue(const char* mat_name,
	const char* key, char* val, int val_size)
{
	Assert(mat_name != NULL);
	Assert(key != NULL);
	Assert(val != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetCustomMaterialValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	material_info_t* pMatInfo = InnerFindCustomMaterial(pMat);

	if (NULL == pMatInfo)
	{
		return false;
	}

	return get_material_value(pMatInfo, key, val, val_size);
}

bool CModelPlayer::SetModelMaterialValue(const char* mat_name, 
	const char* key, const char* val)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetModelMaterialValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	if (!set_material_value(&pMat->MatInfo, key, val))
	{
		return false;
	}

	//update_material_shader_info(pMat, &pMat->MatInfo);

	return true;
}

bool CModelPlayer::GetModelMaterialValue(const char* mat_name, 
	const char* key, char* val, int val_size)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetModelMaterialValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	return get_material_value(&pMat->MatInfo, key, val, val_size);
}
