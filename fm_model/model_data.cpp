//--------------------------------------------------------------------
// 文件名:		model_data.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_data.h"
#include "oct_tree.h"
#include "oct_creator.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_file.h"
#include "../public/core_log.h"
//#include "../utils/system_utils.h"

// 初始化材质数据
void init_material_value(material_info_t* pMatInfo);
// 更新材质的渲染着色标志
//void update_material_shader_info(node_material_t* pMat, 
//	material_info_t* pMatInfo);
// 加载模型材质文件
bool load_model_material_file(model_t* pModel, const char* pszModelName);

//合并骨骼数据到顶点里
inline void merge_bone_data(vertex_data_t& pData, const node_weight_table_t* weight_table, int weight_vertex_num);

static const unsigned int FX_XMOD_VERSION = 0x1002;

struct node_key_index_t
{
	const char* strName;
	unsigned int nHash;
	int nDefine;
	bool bAllowModify;
};

// 节点键索引表
static node_key_index_t s_NodeKeyIndex[] = {
	"PositionInfo", 0, MODEL_POSITION_INFO, false,
	"PositionAni", 0, MODEL_POSITION_ANI, false,
	"NormalInfo", 0, MODEL_NORMAL_INFO, false,
	"NormalAni", 0, MODEL_NORMAL_ANI, false,
	"ColorInfo", 0, MODEL_COLOR_INFO, false,
	"ColorAni", 0, MODEL_COLOR_ANI, false,
	"IllumInfo", 0, MODEL_ILLUM_INFO, false,
	"IllumAni", 0, MODEL_ILLUM_ANI, false,
	"TangentInfo", 0, MODEL_TANGENT_INFO, false,
	"TangentAni", 0, MODEL_TANGENT_ANI, false,
	"BinormalInfo", 0, MODEL_BINORMAL_INFO, false,
	"BinormalAni", 0, MODEL_BINORMAL_ANI, false,
	"MatrixInfo", 0, MODEL_MATRIX_INFO, false,
	"MatrixAni", 0, MODEL_MATRIX_ANI, false,
	"Skin", 0, MODEL_SKIN_FLAG, false,
	"Mesh", 0, MODEL_MESH_FLAG, false,
	"Disappear", 0, MODEL_DISAPPEAR_FLAG, true,
	"SceneFog", 0, MODEL_SCENEFOG_FLAG, true,
	"Applique", 0, MODEL_APPLIQUE_FLAG, true,
	"BB", 0, MODEL_BB_FLAG, true,
	"BBX", 0, MODEL_BBX_FLAG, true,
	"BBY", 0, MODEL_BBY_FLAG, true,
	"TreeLeaf", 0, MODEL_TREELEAF_FLAG, true,
	"Hide", 0, MODEL_HIDE_FLAG, true,
	"Blend", 0, MODEL_BLEND_FLAG, true,
	"BBEX", 0, MODEL_BBEX_FLAG, true,
	"MainModel", 0, MODEL_MAINMODEL_FLAG, true,
	"WaterClip", 0, MODEL_WATERCLIP_FLAG, true,
	"HelperPositionInfo", 0, HELPER_POSITION_INFO, false,
	"HelperPositionAni", 0, HELPER_POSITION_ANI, false,
	"HelperNormalInfo", 0, HELPER_NORMAL_INFO, false,
	"HelperNormalAni", 0, HELPER_NORMAL_ANI, false,
	"HelperMatrixInfo", 0, HELPER_MATRIX_INFO, false,
	"HelperMatrixAni", 0, HELPER_MATRIX_ANI, false,
	"HelperCamera", 0, HELPER_CAMERA_FLAG, false,
};

// 初始化节点键名索引表
void init_node_key_index()
{
	for (int i = 0; i < sizeof(s_NodeKeyIndex) / sizeof(node_key_index_t); ++i)
	{
		s_NodeKeyIndex[i].nHash = GetHashValue(s_NodeKeyIndex[i].strName);
	}
}

// 获得节点宏定义
static int get_node_key_define(const char* key)
{
	unsigned int hash = GetHashValue(key);

	for (int i = 0; i < sizeof(s_NodeKeyIndex) / sizeof(node_key_index_t); ++i)
	{
		if ((s_NodeKeyIndex[i].nHash == hash)
			&& (stricmp(s_NodeKeyIndex[i].strName, key) == 0))
		{
			return s_NodeKeyIndex[i].nDefine;
		}
	}

	return -1;
}

// 获得节点宏定义
static bool get_node_key_allow_modify(const char* key)
{
	unsigned int hash = GetHashValue(key);

	for (int i = 0; i < sizeof(s_NodeKeyIndex) / sizeof(node_key_index_t); ++i)
	{
		if ((s_NodeKeyIndex[i].nHash == hash)
			&& (stricmp(s_NodeKeyIndex[i].strName, key) == 0))
		{
			return s_NodeKeyIndex[i].bAllowModify;
		}
	}

	return false;
}

// 查找节点
static model_node_t* find_node(model_node_t* pNode, const char* node_name)
{
	if (stricmp(pNode->pszName, node_name) == 0)
	{
		return pNode;
	}

	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		model_node_t* ret = find_node(&pNode->ChildNodes[i], node_name);

		if (ret)
		{
			return ret;
		}
	}

	return NULL;
}

// 查找模型的节点
model_node_t* find_model_node(model_t* pModel, const char* node_name)
{
	for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
	{
		model_node_t* ret = find_node(&pModel->RootNodes[i], node_name);

		if (ret)
		{
			return ret;
		}
	}

	return NULL;
}

bool get_model_node_allow_modify(const char* key)
{
	Assert(key != NULL);

	return get_node_key_allow_modify(key);
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

// 设置节点数据
static bool set_node_value(model_node_t* pNode, const char* key, const char* val)
{
	int key_define = get_node_key_define(key);

	if (key_define < 0)
	{
		return false;
	}

	if (stricmp(val, "true") == 0)
	{
		pNode->nModelInfo |= key_define;

		if (stricmp(key, "Hide") == 0)
		{
			pNode->bHide = true;
		}
		else if (stricmp(key, "WaterClip") == 0)
		{
			pNode->bWaterClip = true;
		}
	}
	else
	{
		pNode->nModelInfo &= ~key_define;

		if (stricmp(key, "Hide") == 0)
		{
			pNode->bHide = false;
		}
		else if (stricmp(key, "WaterClip") == 0)
		{
			pNode->bWaterClip = false;
		}
	}

	return true;
}

// 获得模型节点数据
static bool get_node_value(model_node_t* pNode, const char* key, char* val, 
	unsigned int val_size)
{
	int key_define = get_node_key_define(key);

	if (key_define < 0)
	{
		return false;
	}

	bool_to_string((pNode->nModelInfo & key_define) != 0, val, val_size);

	return true;
}

// 设置模型节点数据
bool set_model_node_value(model_node_t* pNode, const char* key, const char* val)
{
	Assert(pNode != NULL);
	Assert(key != NULL);
	Assert(val != NULL);

	if (!set_node_value(pNode, key, val))
	{
		return false;
	}

	return true;
}

// 获得模型节点数据
bool get_model_node_value(model_node_t* pNode, const char* key, char* val, 
	unsigned int val_size)
{
	Assert(pNode != NULL);
	Assert(key != NULL);
	Assert(val != NULL);
	Assert(val_size > 0);

	return get_node_value(pNode, key, val, val_size);
}

// 创建节点八叉树
static void load_node_split_tree(model_node_t* pNode, const char* pszModelName)
{
	Assert(pNode != NULL);
	Assert(pszModelName != NULL);

	// 对于顶点动画，我们无法采用空间分割的办法进行碰撞加速
	if (pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		goto DO_CHILD;
	}

	if ((pNode->nModelInfo & MODEL_BB_FLAG) || 
		(pNode->nModelInfo & MODEL_BBX_FLAG)  || 
		(pNode->nModelInfo & MODEL_BBY_FLAG))
	{
		goto DO_CHILD;
	}

	// 创建切割树
	if ((pNode->nType == FXNODE_TYPE_MESH) && (NULL == pNode->pOctTree))
	{
		COctCreator creator;

		if (creator.Create(pNode))
		{
			pNode->pOctTree = creator.GetOctTree();

			// 转移八叉树的所有权
			creator.Clear();
		}
		else
		{
			LOG_ERROR("[error 030001]create model %s node %s split tree failed",
				pszModelName, pNode->pszName);
		}
	}

DO_CHILD:
	// 子结点
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		load_node_split_tree(&pNode->ChildNodes[i], pszModelName);
	}
}

// 卸载节点八叉树
static void unload_node_split_tree(model_node_t* pNode)
{
	Assert(pNode != NULL);

	// 子结点
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		unload_node_split_tree(&pNode->ChildNodes[i]);
	}

	if (pNode->pOctTree)
	{
		CORE_DELETE(pNode->pOctTree);
		pNode->pOctTree = NULL;
	}
}

// 创建模型八叉树
static void load_model_split_tree(model_t* pInstance, const char* pszModelName)
{
	Assert(pInstance != NULL);
	Assert(pszModelName != NULL);

	// 导入各个结点碰撞用数据
	for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
	{
		load_node_split_tree(&pInstance->RootNodes[i], pszModelName);
	}
}

// 卸载模型八叉树
static void unload_model_split_tree(model_t* pInstance)
{
	Assert(pInstance != NULL);

	for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
	{
		unload_node_split_tree(&pInstance->RootNodes[i]);
	}
}

static const char* strstr_ex_ansi(const char* pszName, const char* pszToken)
{
	Assert(pszName != NULL);
	Assert(pszToken != NULL);

	const char* pDst = strstr(pszName, pszToken);

	if (pDst == NULL)
	{
		return NULL;
	}

	if (pDst != pszName && (isalpha(*(pDst - 1)) || isdigit(*(pDst - 1))))
	{
		return NULL;
	}

	unsigned int len = (unsigned int)strlen(pszToken);

	if (isalpha(*(pDst + len)) || isdigit(*(pDst + len)))
	{
		return NULL;
	}

	return pDst;
}

//#ifdef FXVERSION_MERGE004
static void init_node_info(model_node_t* pNode)
{
	Assert(pNode != NULL);
	Assert(pNode->pszName != NULL);

	pNode->nModelInfo = 0;

	if ('p' == pNode->pszName[MODEL_POSITION_POS])
	{
		pNode->nModelInfo |= MODEL_POSITION_INFO;
	}
	else if ('P' == pNode->pszName[MODEL_POSITION_POS])
	{
		pNode->nModelInfo |= MODEL_POSITION_ANI;
	}

	if ('n' == pNode->pszName[MODEL_NORMAL_POS])
	{
		pNode->nModelInfo |= MODEL_NORMAL_INFO;
	}
	else if ('N' == pNode->pszName[MODEL_NORMAL_POS])
	{
		pNode->nModelInfo |= MODEL_NORMAL_ANI;
	}

	if ('c' == pNode->pszName[MODEL_COLOR_POS])
	{
		pNode->nModelInfo |= MODEL_COLOR_INFO;
	}
	else if ('C' == pNode->pszName[MODEL_COLOR_POS])
	{
		pNode->nModelInfo |= MODEL_COLOR_ANI;
	}

	if ('i' == pNode->pszName[MODEL_ILLUM_POS])
	{
		pNode->nModelInfo |= MODEL_ILLUM_INFO;
	}
	else if ('I' == pNode->pszName[MODEL_ILLUM_POS])
	{
		pNode->nModelInfo |= MODEL_ILLUM_ANI;
	}

	if ('t' == pNode->pszName[MODEL_TANGENT_POS])
	{
		pNode->nModelInfo |= MODEL_TANGENT_INFO;
	}
	else if ('T' == pNode->pszName[MODEL_TANGENT_POS])
	{
		pNode->nModelInfo |= MODEL_TANGENT_ANI;
	}

	if ('b' == pNode->pszName[MODEL_BINORMAL_POS])
	{
		pNode->nModelInfo |= MODEL_BINORMAL_INFO;
	}
	else if ('B' == pNode->pszName[MODEL_BINORMAL_POS])
	{
		pNode->nModelInfo |= MODEL_BINORMAL_ANI;
	}

	if ('m' == pNode->pszName[MODEL_MATRIX_POS])
	{
		pNode->nModelInfo |= MODEL_MATRIX_INFO;
	}
	else if ('M' == pNode->pszName[MODEL_MATRIX_POS])
	{
		pNode->nModelInfo |= MODEL_MATRIX_ANI;
	}

	if (FXNODE_TYPE_SKIN == pNode->nType)
	{
		pNode->nModelInfo |= MODEL_SKIN_FLAG;
	}
	else if (FXNODE_TYPE_MESH == pNode->nType)
	{
		pNode->nModelInfo |= MODEL_MESH_FLAG;
	}
}

static void init_material_info(node_material_t* pMat)
{
	Assert(pMat != NULL);
	Assert(pMat->pszName != NULL);

	pMat->nMaterialInfo = 0;

	if ('a' == pMat->pszName[MATERIAL_AMBIENT_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_AMBIENT_INFO;
	}
	else if ('A' == pMat->pszName[MATERIAL_AMBIENT_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_AMBIENT_ANI;
	}

	if ('d' == pMat->pszName[MATERIAL_DIFFUSE_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_DIFFUSE_INFO;
	}
	else if ('D' == pMat->pszName[MATERIAL_DIFFUSE_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_DIFFUSE_ANI;
	}

	if ('s' == pMat->pszName[MATERIAL_SPECULAR_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_SPECULAR_INFO;
	}
	else if ('S' == pMat->pszName[MATERIAL_SPECULAR_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_SPECULAR_ANI;
	}

	if ('e' == pMat->pszName[MATERIAL_EMISSIVE_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_EMISSIVE_INFO;
	}
	else if ('E' == pMat->pszName[MATERIAL_EMISSIVE_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_EMISSIVE_ANI;
	}

	if ('o' == pMat->pszName[MATERIAL_OPACITY_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_OPACITY_INFO;
	}
	else if ('O' == pMat->pszName[MATERIAL_OPACITY_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_OPACITY_ANI;
	}

	if ('g' == pMat->pszName[MATERIAL_GLOSS_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_GLOSS_INFO;
	}
	else if ('G' == pMat->pszName[MATERIAL_GLOSS_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_GLOSS_ANI;
	}

	if ('d' == pMat->pszName[MATERIAL_DIFFUSE_MAP_POS] ||
		'D' == pMat->pszName[MATERIAL_DIFFUSE_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_DIFFUSE_MAP_INFO;
	}

	if ('b' == pMat->pszName[MATERIAL_BUMP_MAP_POS] ||
		'B' == pMat->pszName[MATERIAL_BUMP_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_BUMP_MAP_INFO;
	}

	if ('s' == pMat->pszName[MATERIAL_SPECULAR_MAP_POS] ||
		'S' == pMat->pszName[MATERIAL_SPECULAR_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_SPECULAR_MAP_INFO;
	}

	if ('l' == pMat->pszName[MATERIAL_SPECULAR_LEVEL_MAP_POS] ||
		'L' == pMat->pszName[MATERIAL_SPECULAR_LEVEL_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_SPECULAR_LEVEL_MAP_INFO;
	}

	if ('g' == pMat->pszName[MATERIAL_GLOSSINESS_MAP_POS] ||
		'G' == pMat->pszName[MATERIAL_GLOSSINESS_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_GLOSSINESS_MAP_INFO;
	}

	if ('r' == pMat->pszName[MATERIAL_REFLECTION_ENV_MAP_POS] ||
		'R' == pMat->pszName[MATERIAL_REFLECTION_ENV_MAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_REFLECTION_ENV_MAP_INFO;
	}

	if ('i' == pMat->pszName[MATERIAL_LIGHTMAP_POS] ||
		'I' == pMat->pszName[MATERIAL_LIGHTMAP_POS])
	{
		pMat->nMaterialInfo |= MATERIAL_LIGHTMAP_INFO;
	}
}
//#endif // FXVERSION_MERGE004

static inline bool read_file(FILE* fp, void* buffer, size_t size)
{
	return core_file::fread(buffer, sizeof(char), size, fp) == size;
}

// 读取名字信息
static inline bool read_name(FILE* pFile, char* buffer, unsigned int size)
{
	unsigned int name_len;

	if (!read_file(pFile, &name_len, sizeof(name_len)))
	{
		return false;
	}

	if (0 == name_len || name_len > 0x100)
	{
		return false;
	}

	if (name_len >= size)
	{
		if (!read_file(pFile, buffer, size - 1))
		{
			return false;
		}

		buffer[size - 1] = 0;
		core_file::fseek(pFile, SEEK_CUR, name_len - (size - 1));
	}
	else
	{
		if (!read_file(pFile, buffer, name_len))
		{
			return false;
		}

		buffer[name_len] = 0;
	}

	return true;
}

// 加载名字信息
static inline bool load_name(FILE* pFile, char*& buffer, unsigned int& size)
{
	if (!read_file(pFile, &size, sizeof(size)))
	{
		return false;
	}

	if (0 == size || size > 0x100)
	{
		return false;
	}

	buffer = (char*)CORE_ALLOC(size + 1);

	if (!read_file(pFile, buffer, size))
	{
		return false;
	}

	buffer[size] = 0;

	return true;
}

static int weight_table_compare(const void* elem1, const void* elem2)
{
	node_weight_table_t* p1 = (node_weight_table_t*)elem1;
	node_weight_table_t* p2 = (node_weight_table_t*)elem2;

	if (p1->fWeight > p2->fWeight)
	{
		return -1;
	}

	if (p1->fWeight < p2->fWeight)
	{
		return 1;
	}

	return 0;
}

// 读取节点的材质信息
static bool load_material(model_t* pMe, model_node_t* pNode,
	node_material_t* pMat, unsigned int frame_num, FILE* pFile)
{
	// 防止缺省光泽度为0
	//pMat->MatInfo.fGlossiness = 10.0F;
	// 初始化材质信息
	init_material_value(&pMat->MatInfo);

	// 记录所属节点
	pMat->pNode = pNode;

	// Material name
	if (!load_name(pFile, pMat->pszName, pMat->nNameLen))
	{
		return false;
	}

	// Material ID
	if (!read_file(pFile, &pMat->nIndexInNode, sizeof(int)))
	{
		LOG_ERROR("(load_material)material id read error! file %s, "
			"material name %s", pMe->pszName, pMat->pszName);
		return false;
	}

//#ifdef FXVERSION_MERGE004
	if (pMe->nVersion > 0x1001)
	{
//#endif
		if (!read_file(pFile, &pMat->nMaterialInfo, sizeof(pMat->nMaterialInfo)))
		{
			LOG_ERROR("(load_material)material info read error! file %s, "
				"material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		if (pMat->nMaterialInfo & MATERIAL_FLAG_EXT)
		{
			if (!read_file(pFile, &pMat->nMaterialInfoEx, 
				sizeof(pMat->nMaterialInfoEx)))
			{
				LOG_ERROR("(load_material)material info read error! file:%s, "
					"material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}
		}
//#ifdef FXVERSION_MERGE004
	}
	else
	{
		// 转换材质信息
		init_material_info(pMat);

		// Get Double side property
		unsigned int bDoubleSide;

		if (!read_file(pFile, &bDoubleSide, sizeof(bDoubleSide)))
		{
			LOG_ERROR("(load_material)double side read error! file %s, "
				"material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		if (bDoubleSide)
		{
			pMat->nMaterialInfo |= MATERIAL_DOUBLESIDE_FLAG;
		}

		// Analyze extension information from material name
		char* pMatExtSection = strchr(pMat->pszName, '!');

		if (pMatExtSection != NULL)
		{
			if (strstr_ex_ansi(pMatExtSection, "BLEND") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_BLEND_FLAG;
			}

			if (strstr_ex_ansi(pMatExtSection, "ATEST") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_ATEST_FLAG;
			}

			if (strstr_ex_ansi(pMatExtSection, "NOLIGHT") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_NOLIGHT_FLAG;
			}

			if (strstr_ex_ansi(pMatExtSection, "BLENDENHANCE") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_BLENDENHANCE_FLAG;
			}

			if (strstr_ex_ansi(pMatExtSection, "NOLERPT") != NULL)
			{
				pMat->bNoLerpT = true;
			}

			if (strstr_ex_ansi(pMatExtSection, "NOLERPM") != NULL)
			{
				pMat->bNoLerpM = true;
			}

			if (strstr_ex_ansi(pMatExtSection, "REFRACTION") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_REFRACTION_FLAG;
			}

			if (strstr_ex_ansi(pMatExtSection, "NOZWRITE") != NULL)
			{
				pMat->nMaterialInfo |= MATERIAL_NOZWRITE_FLAG;
			}
		}
	}
//#endif // FXVERSION_MERGE004

	// Vertex count
	if (!read_file(pFile, &pMat->nVertexCount, sizeof(pMat->nVertexCount)))
	{
		LOG_ERROR("(load_material)material id read error! file %s, "
			"material name %s", pMe->pszName, pMat->pszName);
		return false;
	}

	if (pMat->nVertexCount > 0x10000)
	{
		LOG_ERROR("(load_material)too many vertices! file %s, "
			"material name %s", pMe->pszName, pMat->pszName);
		return false;
	}

	// Calculate single vertex size and multi vertex size and vertex element
	pMat->nSingleVertexSize = 0;
	pMat->nMultiVertexSize = 0;
	
	// Position 
	if (pNode->nModelInfo & MODEL_POSITION_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_POSITION_ANI)
	{
		pMat->nMultiVertexSize += sizeof(float) * 3;
	}

	// Normal
	if (pNode->nModelInfo & MODEL_NORMAL_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_NORMAL_ANI)
	{
		pMat->nMultiVertexSize += sizeof(float) * 3;
	}

	// Color
	if (pNode->nModelInfo & MODEL_COLOR_INFO)
	{
		pMat->nSingleVertexSize += sizeof(unsigned int);
	}
	else if (pNode->nModelInfo & MODEL_COLOR_ANI)
	{
		pMat->nMultiVertexSize += sizeof(unsigned int);
	}

	// Illum
	if (pNode->nModelInfo & MODEL_ILLUM_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float);
	}
	else if (pNode->nModelInfo & MODEL_ILLUM_ANI)
	{
		pMat->nMultiVertexSize += sizeof(float);
	}

	// Tangent
	if (pNode->nModelInfo & MODEL_TANGENT_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_TANGENT_ANI)
	{
		pMat->nMultiVertexSize += sizeof(float) * 3;
	}

	// Binormal
	if (pNode->nModelInfo & MODEL_BINORMAL_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 3;
	}
	else if (pNode->nModelInfo & MODEL_BINORMAL_ANI)
	{
		pMat->nMultiVertexSize += sizeof(float) * 3;
	}

	// Diffuse texture
	if (pMat->nMaterialInfo & (MATERIAL_DIFFUSE_MAP_INFO | MATERIAL_BUMP_MAP_INFO
		| MATERIAL_SPECULAR_MAP_INFO | MATERIAL_SELF_ILLUMINATION_INFO))
	{
		pMat->nSingleVertexSize += sizeof(float) * 2;
	}

	// Filter texture
	if (pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 2;
	}

	// Lightmap texture
	if (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
	{
		pMat->nSingleVertexSize += sizeof(float) * 2;
	}

	// Read single vertex buffer
	if (pMat->nSingleVertexSize != 0)
	{
		//if (pMat->uVertexCnt > (DX_VERTEX_BLOCK_NUM * DX_VERTEX_BLOCK_SIZE))
		//{
		//	return false;
		//}

		create_vertex_data(&(pMat->SingleVB), pMat->nSingleVertexSize,
			pMat->nVertexCount);

		if (!read_file(pFile, pMat->SingleVB.pVertices, 
			pMat->nSingleVertexSize * pMat->nVertexCount))
		{
			LOG_ERROR("(load_material)material single vb read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Read multi vertex buffer
	if (pMat->nMultiVertexSize != 0)
	{
		pMat->pMultiVBs = (float**)CORE_ALLOC(sizeof(float*) * frame_num);
		memset(pMat->pMultiVBs, 0, sizeof(float*) * frame_num);

		for (unsigned int k = 0; k < frame_num; ++k)
		{
			pMat->pMultiVBs[k] = (float*)CORE_ALLOC(
				pMat->nMultiVertexSize * pMat->nVertexCount);

			if (!read_file(pFile, pMat->pMultiVBs[k], 
				pMat->nMultiVertexSize * pMat->nVertexCount))
			{
				LOG_ERROR("(load_material)material multi vb read error! "
					"file %s, material name %s", pMe->pszName, pMat->pszName);
				return false;
			}
		}
	}

	// IB
	if (pMe->nLODCount > 0)
	{
		pMat->pLODIndexCount = (unsigned int*)CORE_ALLOC(
			sizeof(unsigned int) * pMe->nLODCount);
		pMat->pLODIB = (unsigned short**)CORE_ALLOC(
			sizeof(unsigned short*) * pMe->nLODCount);
	}

	for (unsigned int i = 0; i < pMe->nLODCount; ++i)
	{
		if (!read_file(pFile, (void*)&pMat->pLODIndexCount[i], 
			sizeof(unsigned int)))
		{
			LOG_ERROR("(load_material)material index count read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	for (unsigned int j = 0; j < pMe->nLODCount; ++j)
	{
		if (pMat->pLODIndexCount[j] != 0)
		{
			pMat->pLODIB[j] = (unsigned short*)CORE_ALLOC(
				sizeof(unsigned short) * pMat->pLODIndexCount[j]);
			if (!read_file(pFile, (void*)pMat->pLODIB[j], 
				sizeof(unsigned short) * pMat->pLODIndexCount[j]))
			{
				LOG_ERROR("(load_material)material index read error! "
					"file %s, material name %s", pMe->pszName, pMat->pszName);
				return false;
			}
		}
	}

//#ifdef FXVAMODEL_DEBUG
//	for (unsigned int i = 0; i < pMe->uLODCount; ++i)
//	{
//		for (unsigned int j = 0; j < pMat->puLODIndexCnt[i]; ++j)
//		{
//			if (pMat->pLODIB[i][j] >= pMat->uVertexCnt)
//			{
//				LOG_ERROR("[error 030002] load_material:The index of "
//					"vertex is overflow! model is:%s, mat is:%s", 
//					pMe->pszName, pMat->pszName);
//				return false;
//			}
//		}
//	}
//#endif

	if (pNode->nType == FXNODE_TYPE_SKIN)
	{
		// Skinned vertex bone blend size
		pMat->nSkinnedVertexBoneBlendVertexSize = 
			(sizeof(short) + sizeof(float)) * MAX_VERTEX_BONE_BLEND;

		// Vertex bone weight table
		unsigned int weight_vertex_num = pMat->nVertexCount * MAX_VERTEX_BONE_BLEND;

		pMat->pWeightTable = (node_weight_table_t*)CORE_ALLOC(
			sizeof(node_weight_table_t) * weight_vertex_num);

		for (unsigned int v = 0; v < weight_vertex_num; v++)
		{
			if (!read_file(pFile, 
				&pMat->pWeightTable[v].nBoneNameOffset, sizeof(int)))
			{
				LOG_ERROR("(load_material)material bone name offset read "
					"error! file %s, material name %s", pMe->pszName, 
					pMat->pszName);
				return false;
			}

			if (!read_file(pFile, 
				&pMat->pWeightTable[v].fWeight, sizeof(float)))
			{
				LOG_ERROR("(load_material)material bone weight read error! "
					"file %s, material name %s", pMe->pszName, pMat->pszName);
				return false;
			}

			pMat->pWeightTable[v].nBoneID = -1;
		}

//#ifdef FXVAMODEL_DEBUG
//		for (unsigned int v = 0; v < weight_vertex_num; v++)
//		{
//			if (Port_IsNaN((pMat->pWeightTable[v].fWeight))
//			{
//				LOG_ERROR("[error 030003] load_material:The blend vertex's "
//					"weight is invalid! Weight = %f, model is %s, mat is %s", 
//					pMat->pWeightTable[v].fWeight, pMe->pszName, pMat->pszName);
//				return false;
//			}
//		}
//#endif

		// 对每个顶点的权值进行排序
		unsigned int vertex_num = pMat->nVertexCount;
		node_weight_table_t* pWeightTable = pMat->pWeightTable;

		for (unsigned int k = 0; k < vertex_num; ++k)
		{
			qsort(pWeightTable, MAX_VERTEX_BONE_BLEND, 
				sizeof(node_weight_table_t), weight_table_compare);
			pWeightTable += MAX_VERTEX_BONE_BLEND;
		}

		// Bone name table
		if (!read_file(pFile, &pMat->nBoneNameListLen, 
			sizeof(unsigned int)))
		{
			LOG_ERROR("(load_material)material bone name list len read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->pszBoneNameList = (char*)CORE_ALLOC(pMat->nBoneNameListLen + 1);

		if (!read_file(pFile, pMat->pszBoneNameList, 
			pMat->nBoneNameListLen))
		{
			LOG_ERROR("(load_material)material bone name list read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->pszBoneNameList[pMat->nBoneNameListLen] = 0;

		merge_bone_data(pMat->SingleVB, pMat->pWeightTable, weight_vertex_num);
		pMat->nSingleVertexSize = pMat->SingleVB.nStride;
	}

	// Material parameter ambient
	if (pMat->nMaterialInfo & MATERIAL_AMBIENT_ANI)
	{
		pMat->pFrameAmbient = (FmVec3*)CORE_ALLOC(
			sizeof(FmVec3) * frame_num);

		if (!read_file(pFile, pMat->pFrameAmbient, 
			sizeof(FmVec3) * frame_num))
		{
			LOG_ERROR("(load_material)material ambient read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.vAmbient = pMat->pFrameAmbient[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_AMBIENT_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.vAmbient, 
			sizeof(FmVec3)))
		{
			LOG_ERROR("(load_material)material ambient read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Material parameter diffuse
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_ANI)
	{
		pMat->pFrameDiffuse = (FmVec3*)CORE_ALLOC(
			sizeof(FmVec3) * frame_num);

		if (!read_file(pFile, pMat->pFrameDiffuse, 
			sizeof(FmVec3) * frame_num))
		{
			LOG_ERROR("(load_material)material diffuse read error! "
				"file:%s, material name:%s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.vDiffuse = pMat->pFrameDiffuse[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.vDiffuse, 
			sizeof(FmVec3)))
		{
			LOG_ERROR("(load_material)material diffuse read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Material parameter specular
	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_ANI)
	{
		pMat->pFrameSpecular = (FmVec3*)CORE_ALLOC(
			sizeof(FmVec3) * frame_num);

		if (!read_file(pFile, pMat->pFrameSpecular, 
			sizeof(FmVec3) * frame_num))
		{
			LOG_ERROR("(load_material)material specular read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.vSpecular = pMat->pFrameSpecular[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_SPECULAR_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.vSpecular, 
			sizeof(FmVec3)))
		{
			LOG_ERROR("(load_material)material specular read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Material parameter emissive
	if (pMat->nMaterialInfo & MATERIAL_EMISSIVE_ANI)
	{
		pMat->pFrameEmissive = (FmVec3*)CORE_ALLOC(
			sizeof(FmVec3) * frame_num);

		if (!read_file(pFile, pMat->pFrameEmissive, 
			sizeof(FmVec3) * frame_num))
		{
			LOG_ERROR("(load_material)material emissive read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.vEmissive = pMat->pFrameEmissive[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_EMISSIVE_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.vEmissive, 
			sizeof(FmVec3)))
		{
			LOG_ERROR("(load_material)material emissive read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Material parameter opacity
	if (pMat->nMaterialInfo & MATERIAL_OPACITY_ANI)
	{
		pMat->pFrameOpacity = (float*)CORE_ALLOC(sizeof(float) * frame_num);

		if (!read_file(pFile, pMat->pFrameOpacity, 
			sizeof(float) * frame_num))
		{
			LOG_ERROR("(load_material)material opacity read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.fOpacity = pMat->pFrameOpacity[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_OPACITY_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.fOpacity, 
			sizeof(float)))
		{
			LOG_ERROR("(load_material)material opacity read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Material parameter glossiness
	if (pMat->nMaterialInfo & MATERIAL_GLOSS_ANI)
	{
		pMat->pFrameGlossiness = (float*)CORE_ALLOC(sizeof(float) * frame_num);

		if (!read_file(pFile, pMat->pFrameGlossiness, 
			sizeof(float) * frame_num))
		{
			LOG_ERROR("(load_material)material glossiness read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}

		pMat->MatInfo.fGlossiness = pMat->pFrameGlossiness[0];
	}
	else if (pMat->nMaterialInfo & MATERIAL_GLOSS_INFO)
	{
		if (!read_file(pFile, &pMat->MatInfo.fGlossiness, 
			sizeof(float)))
		{
			LOG_ERROR("(load_material)material glossiness read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	if (pMat->nMaterialInfo & MATERIAL_FLAG_EXT && pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO)
	{
		// Material parameter filter param
		if (pMat->nMaterialInfoEx & MATERIAL_FILTER_PARAM_ANI)
		{
			int nReverse = 0;
			if (!read_file(pFile, &nReverse, sizeof(int)))
			{
				LOG_ERROR("(load_material)material filter reverse read error! "
					"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}

			pMat->pFrameFilterParam = (filter_param*)CORE_ALLOC(
				sizeof(filter_param) * frame_num);

			if (!read_file(pFile, pMat->pFrameFilterParam, 
				sizeof(filter_param) * frame_num))
			{
				LOG_ERROR("(load_material)material filter param ani read error! "
					"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}

			pMat->MatInfo.filterParam = pMat->pFrameFilterParam[0];
		}
		else
		{
			int nReverse = 0;
			if (!read_file(pFile, &nReverse, sizeof(int)))
			{
				LOG_ERROR("(load_material)material filter reverse read error! "
					"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}

			if (!read_file(pFile, &pMat->MatInfo.filterParam, 
				sizeof(filter_param)))
			{
				LOG_ERROR("(load_material)material filter param read error! "
					"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}
		}
	}

	// Diffuse map parameter
	char tex_name[128];

	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.DiffuseMap.pFileName, 
			pMat->MatInfo.DiffuseMap.nFileNameLen))
		{
			return false;
		}

		if (pMat->MatInfo.DiffuseMap.nFileNameLen >= strlen("forbid_stand"))
		{
			if (NULL != pMat->MatInfo.DiffuseMap.pFileName)
			{
				if (NULL != strstr(pMat->MatInfo.DiffuseMap.pFileName, 
					"forbid_stand"))
				{
					pMat->MatInfo.bForbidStandModel = true;
				}
			}
		}

//#ifdef FXVERSION_MERGE004
		if (pMat->MatInfo.DiffuseMap.nFileNameLen >= 4)
		{
			if ('v' == pMat->MatInfo.DiffuseMap.pFileName[0] 
			&& 'o' == pMat->MatInfo.DiffuseMap.pFileName[1] 
			&& 'l' == pMat->MatInfo.DiffuseMap.pFileName[2] 
			&& '_' == pMat->MatInfo.DiffuseMap.pFileName[3])
			{
				pMat->MatInfo.bVolumeTexture = true;
			}
		}

		if (pMe->nVersion <= 0x1001)
		{
			//if ('M' == tex_name[TEXTURE_MATRIX_POS] 
			//	|| 'm' == tex_name[TEXTURE_MATRIX_POS])
			if ('M' == tex_name[0] || 'm' == tex_name[0])
			{
				pMat->nMaterialInfo |= MATERIAL_DIFFUSE_UV_ANI;
			}
		}
//#endif
	}

	// Texture parameter
	if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_UV_ANI)
	{
		pMat->pFrameDiffuseTM = (uv_param_t*)CORE_ALLOC(
			sizeof(uv_param_t) * frame_num);

		if (!read_file(pFile, pMat->pFrameDiffuseTM, 
			sizeof(uv_param_t) * frame_num))
		{
			LOG_ERROR("(load_material)material diffuse tm read error! "
				"file %s, material name %s", pMe->pszName, pMat->pszName);
			return false;
		}
	}

	// Bump map parameter
	if (pMat->nMaterialInfo & MATERIAL_BUMP_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.BumpMap.pFileName, 
			pMat->MatInfo.BumpMap.nFileNameLen))
		{
			return false;
		}
	}

	// Specular map parameter
	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.SpecularMap.pFileName, 
			pMat->MatInfo.SpecularMap.nFileNameLen))
		{
			return false;
		}
	}

	// Specular level map parameter
	if (pMat->nMaterialInfo & MATERIAL_SPECULAR_LEVEL_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.SpecularLevelMap.pFileName, 
			pMat->MatInfo.SpecularLevelMap.nFileNameLen))
		{
			return false;
		}
	}

	// Glossiness map
	if (pMat->nMaterialInfo & MATERIAL_GLOSSINESS_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.GlossinessMap.pFileName, 
			pMat->MatInfo.GlossinessMap.nFileNameLen))
		{
			return false;
		}
	}

	// Reflection map
	if (pMat->nMaterialInfo & MATERIAL_REFLECTION_ENV_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.ReflectionMap.pFileName, 
			pMat->MatInfo.ReflectionMap.nFileNameLen))
		{
			return false;
		}
	}

	// light map parameter
	if (pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.LightMap.pFileName, 
			pMat->MatInfo.LightMap.nFileNameLen))
		{
			return false;
		}
	}

	// self illumimation map parameter
	if (pMat->nMaterialInfo & MATERIAL_SELF_ILLUMINATION_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.EmissiveMap.pFileName, 
			pMat->MatInfo.EmissiveMap.nFileNameLen))
		{
			return false;
		}
	}

	// Material parameter diffusemapframe
	if (pMat->nMaterialInfoEx & MATERIAL_FRAME_INFO)
	{
		pMat->pFrameTexture = (float*)CORE_ALLOC(sizeof(float) * frame_num);
		
		if (!read_file(pFile, pMat->pFrameTexture, 
			sizeof(float) * frame_num))
		{
			LOG_ERROR("(load_material)material glossiness read error! "
				"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
			return false;
		}
	}

		// filter map parameter
	if (pMat->nMaterialInfoEx & MATERIAL_FILTER_MAP_INFO)
	{
		if (!read_name(pFile, tex_name, sizeof(tex_name)))
		{
			LOG_ERROR("(load_material)material filter map read error! "
				"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
			return false;
		}

		if (!load_name(pFile, pMat->MatInfo.FilterMap.pFileName, 
			pMat->MatInfo.FilterMap.nFileNameLen))
		{
			LOG_ERROR("(load_material)material filter map load error! "
				"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
			return false;
		}

		// Filter Texture parameter
		if (pMat->nMaterialInfoEx & MATERIAL_FILTER_UV_ANI)
		{
			pMat->pFrameFilterTM = (uv_param_t*)CORE_ALLOC(
				sizeof(uv_param_t) * frame_num);

			if (!read_file(pFile, pMat->pFrameFilterTM, 
				sizeof(uv_param_t) * frame_num))
			{
				LOG_ERROR("(load_material)material filter tm read error! " 
					"file:%s, material name:%s\n", pMe->pszName, pMat->pszName);
				return false;
			}
		}
	}
	
	material_info_t* pMatInfo = &pMat->MatInfo;

	pMatInfo->bSpecularEnable = (pMat->nMaterialInfo & MATERIAL_SPECULAR_INFO) != 0
		|| (pMat->nMaterialInfo & MATERIAL_SPECULAR_ANI) != 0
		|| (pMat->MatInfo.SpecularLevelMap.pFileName != NULL);
	pMatInfo->bEmissiveEnable = (pMat->nMaterialInfo & MATERIAL_EMISSIVE_INFO) != 0
		|| (pMat->nMaterialInfo & MATERIAL_EMISSIVE_ANI) != 0;
	pMatInfo->bOpacityEnable = (pMat->nMaterialInfo & MATERIAL_OPACITY_INFO) != 0
		|| (pMat->nMaterialInfo & MATERIAL_OPACITY_ANI) != 0;
	pMatInfo->bReflectEnable = (pMat->MatInfo.ReflectionMap.pFileName != NULL)
		|| (pMat->MatInfo.SpecularLevelMap.pFileName != NULL);
	pMatInfo->bBumpMapEnable = pMat->MatInfo.BumpMap.pFileName != NULL;
	pMatInfo->bFilterEnable = pMat->MatInfo.FilterMap.pFileName != NULL;
	pMatInfo->bAlphaTest = (pMat->nMaterialInfo & MATERIAL_ATEST_FLAG) != 0;
	pMatInfo->bBlend = (pMat->nMaterialInfo & MATERIAL_BLEND_FLAG) != 0;
	pMatInfo->bBlendEnhance = (pMat->nMaterialInfo & MATERIAL_BLENDENHANCE_FLAG) != 0;
	pMatInfo->bBlendQuality = (pMat->nMaterialInfo & MATERIAL_BLENDQUALITY) != 0;
	pMatInfo->bNoZWrite = (pMat->nMaterialInfo & MATERIAL_NOZWRITE_FLAG) != 0;
	pMatInfo->bDoubleSide = (pMat->nMaterialInfo & MATERIAL_DOUBLESIDE_FLAG) != 0;
	pMatInfo->bSamplerClamp = (pMat->nMaterialInfo & MATERIAL_SAMPLER_CLAMP) != 0;
	pMatInfo->bNoLight = (pMat->nMaterialInfo & MATERIAL_NOLIGHT_FLAG) != 0;
	pMatInfo->bDisappear = (pNode->nModelInfo & MODEL_DISAPPEAR_FLAG) != 0;
	pMatInfo->bRefraction = (pMat->nMaterialInfo & MATERIAL_REFRACTION_FLAG) != 0;
	pMatInfo->bApplique = (pNode->nModelInfo & MODEL_APPLIQUE_FLAG) != 0;
	pMatInfo->bSceneFog = (pMat->nMaterialInfo & MATERIAL_VOLUMEFOG_FLAG) != 0
		|| (pNode->nModelInfo & MODEL_SCENEFOG_FLAG) != 0;

	// 根据材质设置着色器信息
	//update_material_shader_info(pMat, &pMat->MatInfo);

	return true;
}

static bool load_model_node(model_t* pMe, model_node_t* pNode, 
	FILE* pFile)
{
	Assert(pNode != NULL);
	Assert(pFile != NULL);

	// Regist the node to global node list
	if (NULL == pMe->pNodeList)
	{
		Assert(0 == pMe->nNodeCount);

		pMe->pNodeList = (model_node_t**)CORE_ALLOC(sizeof(model_node_t*) * 1);
		pMe->pNodeList[pMe->nNodeCount] = pNode;
	}
	else
	{
		model_node_t** p = (model_node_t**)CORE_ALLOC(
			sizeof(model_node_t*) * (pMe->nNodeCount + 1));

		memcpy(p, pMe->pNodeList, sizeof(model_node_t*) * pMe->nNodeCount);

		if (pMe->pNodeList)
		{
			CORE_FREE(pMe->pNodeList, sizeof(model_node_t*) * pMe->nNodeCount);
		}

		pMe->pNodeList = p;
		pMe->pNodeList[pMe->nNodeCount] = pNode;
	}

	pNode->nIndex = pMe->nNodeCount;
	pMe->nNodeCount++;

	// Calculate frame count
	unsigned int frame_num = pMe->nEndFrame - pMe->nStartFrame + 1;

	// Node name and it's length.
	if (!load_name(pFile, pNode->pszName, pNode->nNameLen))
	{
		return false;
	}

	// Node Type
	if (!read_file(pFile, &pNode->nType, sizeof(pNode->nType)))
	{
		LOG_ERROR("(load_model_node)node type read error! "
			"file %s, node name %s", pMe->pszName, pNode->pszName);
		return false;
	}

//#ifdef FXVERSION_MERGE004
	if (pMe->nVersion > 0x1001)
	{
		// 读取节点信息
		if (!read_file(pFile, &pNode->nModelInfo, sizeof(pNode->nModelInfo)))
		{
			LOG_ERROR("(load_model_node)node info read error! "
				"file %s, node name %s", pMe->pszName, pNode->pszName);
			return false;
		}

		if (pNode->nModelInfo & MODEL_WATERCLIP_FLAG)
		{
			pNode->bWaterClip = true;
		}
	}
	else
	{
		// 转换节点信息
		init_node_info(pNode);
	}
//#else
//	// 读取节点信息
//	if (!read_file(pFile, &pNode->nModelInfo, 
//		sizeof(pNode->nModelInfo)))
//	{
//		LOG_ERROR("(load_model_node)node type read error! "
//			"file %s, node name %s", pMe->pszName, pNode->pszName);
//		return false;
//	}
//#endif

	if (pNode->nType == FXNODE_TYPE_MESH)
	{
//#ifdef FXVERSION_MERGE004
		if (pMe->nVersion <= 0x1001)
		{
			// Analyze extension information from node name
			char* pNodeExtSection = strchr(pNode->pszName, '!');

			if (pNodeExtSection != NULL)
			{
				if (strstr_ex_ansi(pNodeExtSection, "BLEND") != NULL)
				{
					pNode->nModelInfo |= MODEL_BLEND_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "DECAL") != NULL)
				{
					pNode->nModelInfo |= MODEL_APPLIQUE_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "BB") != NULL)
				{
					pNode->nModelInfo |= MODEL_BB_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "BBEX") != NULL)
				{
					pNode->nModelInfo |= MODEL_BBEX_FLAG;
				}

				if(strstr_ex_ansi(pNodeExtSection, "BBY") != NULL)
					pNode->nModelInfo |= MODEL_BBY_FLAG;

				if (strstr_ex_ansi(pNodeExtSection, "BBX") != NULL)
				{
					pNode->nModelInfo |= MODEL_BBX_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "NOLERPT") != NULL)
				{
					pNode->bNoLerp = true;
				}

				if (strstr_ex_ansi(pNodeExtSection, "DISAPPEAR") != NULL)
				{
					pNode->nModelInfo |= MODEL_DISAPPEAR_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "SCENEFOG") != NULL)
				{
					pNode->nModelInfo |= MODEL_SCENEFOG_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "TREELEAF") != NULL)
				{
					pNode->nModelInfo |= MODEL_TREELEAF_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "HIDE") != NULL)
				{
					pNode->nModelInfo |= MODEL_HIDE_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "APPLIQUE") != NULL)
				{
					pNode->nModelInfo |= MODEL_APPLIQUE_FLAG;
				}
			}
		}
//#endif // FXVERSION_MERGE004

		// Local model matrix
		if (pNode->nModelInfo & MODEL_MATRIX_ANI)
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(
				sizeof(location_t) * frame_num);

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t) * frame_num))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}
		else if (pNode->nModelInfo & MODEL_MATRIX_INFO)
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(sizeof(location_t));

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t)))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}

		// Split tree file offset
		int	nSplitTreeOffset;

		if (!read_file(pFile, &nSplitTreeOffset, 
			sizeof(nSplitTreeOffset)))
		{
			LOG_ERROR("(load_model_node)node split tree read error! "
				"file %s, node name %s", pMe->pszName, pNode->pszName);
			return false;
		}

		// Bounding box
		if (pNode->nModelInfo & MODEL_POSITION_INFO 
			|| pNode->nModelInfo & MODEL_POSITION_ANI 
			|| pNode->nModelInfo & MODEL_NORMAL_INFO 
			|| pNode->nModelInfo & MODEL_NORMAL_ANI)
		{
			if (!read_file(pFile, &pNode->vBoundMin, 
				sizeof(pNode->vBoundMin)))
			{
				LOG_ERROR("(load_model_node)node min bb read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}

			if (!read_file(pFile, &pNode->vBoundMax, 
				sizeof(pNode->vBoundMax)))
			{
				LOG_ERROR("(load_model_node)node max bb read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}

		// Material count
		if (!read_file(pFile, &pNode->nMaterialCount, 
			sizeof(pNode->nMaterialCount)))
		{
			LOG_ERROR("(load_model_node)node mat count read error! "
				"file %s, node name %s", pMe->pszName, pNode->pszName);
			return false;
		}

		// Every material's 
		if (pNode->nMaterialCount != 0)
		{
			pNode->Materials = (node_material_t*)CORE_ALLOC(
				sizeof(node_material_t) * pNode->nMaterialCount);
			memset(pNode->Materials, 0, 
				sizeof(node_material_t) * pNode->nMaterialCount);

			for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
			{
				node_material_t* pMat = &pNode->Materials[m];

				if (!load_material(pMe, pNode, pMat, frame_num, pFile))
				{
					if (pMat->pszName)
					{
						LOG_ERROR("[error](load_model_node)model name is %s, "
							"mat name is %s", pMe->pszName, pMat->pszName);
					}
					else
					{
						LOG_ERROR("[error](load_model_node)model name is %s", 
							pMe->pszName);
					}

					return false;
				}

				// 记录在整个模型中的材质索引
				pMat->nIndexInModel = pMe->nMaterialCount + m;
			}
		}
		else
		{
			LOG_ERROR("[error](load_model_node)material count must > 0, "
				"model name is %s, node name is %s", pMe->pszName, 
				pNode->pszName);
			return false;
		}

		pMe->nMaterialCount += pNode->nMaterialCount;
	}
	else if (pNode->nType == FXNODE_TYPE_HELPER)
	{
		// Local model matrix
		if (pNode->nModelInfo & MODEL_MATRIX_ANI)
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(
				sizeof(location_t) * frame_num);

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t) * frame_num))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}

			if (pNode->nModelInfo & HELPER_CAMERA_FLAG)
			{
				pNode->pCamera = (node_camera_t*)CORE_ALLOC(
					sizeof(node_camera_t) * frame_num);

				if (!read_file(pFile, pNode->pCamera, 
					sizeof(node_camera_t) * frame_num))
				{
					LOG_ERROR("(load_model_node)node camera read error! "
						"file %s, node name %s", pMe->pszName, pNode->pszName);
					return false;
				}
			}
		}
		else
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(sizeof(location_t));

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t)))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}

			if (pNode->nModelInfo & HELPER_CAMERA_FLAG)
			{
				pNode->pCamera = (node_camera_t*)CORE_ALLOC(
					sizeof(node_camera_t));

				if (!read_file(pFile, pNode->pCamera, 
					sizeof(node_camera_t)))
				{
					LOG_ERROR("(load_model_node)node camera read error! "
						"file %s, node name %s", pMe->pszName, pNode->pszName);
					return false;
				}
			}
		}
	}
	else if (pNode->nType == FXNODE_TYPE_CAMERA)
	{
		LOG_ERROR("[error](load_model_node)engine is no more support camera "
			"type node, model name is %s, node name is %s", pMe->pszName, 
			pNode->pszName);
		return false;
	}
	else if (pNode->nType == FXNODE_TYPE_SKIN)
	{
//#ifdef FXVERSION_MERGE004
		if (pMe->nVersion <= 0x1001)
		{
			// Analyze extension information from node name
			char* pNodeExtSection = strchr(pNode->pszName, '!');

			if (pNodeExtSection != NULL)
			{
				if (strstr_ex_ansi(pNodeExtSection, "BLEND") != NULL)
				{
					pNode->nModelInfo |= MODEL_BLEND_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "DECAL") != NULL)
				{
					pNode->nModelInfo |= MODEL_APPLIQUE_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "BB") != NULL)
				{
					pNode->nModelInfo |= MODEL_BB_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "BBY") != NULL)
				{
					pNode->nModelInfo |= MODEL_BBY_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "BBEX") != NULL)
				{
					pNode->nModelInfo |= MODEL_BBEX_FLAG;
				}

				if (strstr_ex_ansi(pNodeExtSection, "MAINMODEL") != NULL)
				{
					pNode->nModelInfo |= MODEL_MAINMODEL_FLAG;
				}
			}
		}
//#endif
		// Local model matrix
		if (pNode->nModelInfo & MODEL_MATRIX_ANI)
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(
				sizeof(location_t) * frame_num);

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t) * frame_num))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}
		else if (pNode->nModelInfo & MODEL_MATRIX_INFO)
		{
			pNode->pLocation = (location_t*)CORE_ALLOC(sizeof(location_t));

			if (!read_file(pFile, pNode->pLocation, 
				sizeof(location_t)))
			{
				LOG_ERROR("(load_model_node)node location read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}

		// Split tree file offset
		int	nSplitTreeOffset;

		if (!read_file(pFile, &nSplitTreeOffset, 
			sizeof(nSplitTreeOffset)))
		{
			LOG_ERROR("(load_model_node)node split tree read error! "
				"file %s, node name %s", pMe->pszName, pNode->pszName);
			return false;
		}

		// Bounding box
		if (pNode->nModelInfo & MODEL_POSITION_INFO 
			|| pNode->nModelInfo & MODEL_POSITION_ANI 
			|| pNode->nModelInfo & MODEL_NORMAL_INFO 
			|| pNode->nModelInfo & MODEL_NORMAL_ANI)
		{
			if (!read_file(pFile, &pNode->vBoundMin, 
				sizeof(pNode->vBoundMin)))
			{
				LOG_ERROR("(load_model_node)node min bb read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}

			if (!read_file(pFile, &pNode->vBoundMax, 
				sizeof(pNode->vBoundMax)))
			{
				LOG_ERROR("(load_model_node)node max bb read error! "
					"file %s, node name %s", pMe->pszName, pNode->pszName);
				return false;
			}
		}

		// Material count
		if (!read_file(pFile, &pNode->nMaterialCount, sizeof(pNode->nMaterialCount)))
		{
			LOG_ERROR("(load_model_node)node mat count read error! "
				"file %s, node name %s", pMe->pszName, pNode->pszName);
			return false;
		}

		// Every material's 
		if (pNode->nMaterialCount != 0)
		{
			pNode->Materials = (node_material_t*)CORE_ALLOC(
				sizeof(node_material_t) * pNode->nMaterialCount);
			memset(pNode->Materials, 0, sizeof(node_material_t) * pNode->nMaterialCount);

			for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
			{
				node_material_t* pMat = &pNode->Materials[m];

				if (!load_material(pMe, pNode, pMat, frame_num, pFile))
				{
					if (pMat->pszName)
					{
						LOG_ERROR("[error](load_model_node)model name is %s, "
							"mat name is %s", pMe->pszName, pMat->pszName);
					}
					else
					{
						LOG_ERROR("[error](load_model_node)model name is %s", 
							pMe->pszName);
					}

					return false;
				}

				pMat->nIndexInModel = pMe->nMaterialCount + m;
			}
		}
		else
		{
			LOG_ERROR("[error](load_model_node)material count must > 0, "
				"model name is %s, node name is %s", pMe->pszName, 
				pNode->pszName);
			return false;
		}

		pMe->nMaterialCount += pNode->nMaterialCount;
	}

	if (pNode->nModelInfo & MODEL_HIDE_FLAG)
	{
		pNode->bHide = true;
	}

	if (pNode->nType == FXNODE_TYPE_MESH || pNode->nType == FXNODE_TYPE_SKIN)
	{
//#ifdef FXVERSION_MERGE002
		if (pMe->nVersion >= 0x1001)
		{
//#endif
			if (!read_file(pFile, &pNode->nAttachedHelperCount, 
				sizeof(pNode->nAttachedHelperCount)))
			{
				LOG_ERROR("(load_model_node)attach helper count "
					"read error! file %s, node name %s", pMe->pszName, 
					pNode->pszName);
				return false;
			}

			if (pNode->nAttachedHelperCount > 0)
			{
				pNode->pAttachedHelpers = (model_node_t*)CORE_ALLOC(
					sizeof(model_node_t) * pNode->nAttachedHelperCount);
				memset(pNode->pAttachedHelpers, 0, 
					sizeof(model_node_t) * pNode->nAttachedHelperCount);

				for (unsigned int i = 0; i < pNode->nAttachedHelperCount; ++i)
				{
					model_node_t* pAttachNode = &pNode->pAttachedHelpers[i];

					if (!load_name(pFile, 
						pAttachNode->pszAttachedHelperParentName, 
						pAttachNode->nAttachedHelperParentNameLen))
					{
						return false;
					}

					if (!load_name(pFile, pAttachNode->pszName, 
						pAttachNode->nNameLen))
					{
						return false;
					}

					if (NULL == pAttachNode->pszAttachedHelperParentName)
					{
						LOG_ERROR("[error 030056]Helper parent name is "
							"NULL, model name is %s", pMe->pszName);
						return false;
					}

					if (NULL == pAttachNode->pszName)
					{
						LOG_ERROR("[error 030056]Helper name is NULL, "
							"model name is %s", pMe->pszName);
						return false;
					}

					if (!read_file(pFile, &pAttachNode->nType, 
						sizeof(pAttachNode->nType)))
					{
						LOG_ERROR("(load_model_node)attach node "
							"type read error! file %s, node name %s", 
							pMe->pszName, pNode->pszName);
						return false;
					}

//#ifdef FXVERSION_MERGE004
					if (pMe->nVersion >= 0x1002)
					{
//#endif
						if (!read_file(pFile, 
							&pAttachNode->nModelInfo, 
							sizeof(pAttachNode->nModelInfo)))
						{
							LOG_ERROR("(load_model_node)attach "
								"node info read error! file %s, node name %s", 
								pMe->pszName, pNode->pszName);
							return false;
						}
//#ifdef FXVERSION_MERGE004
					}
					else
					{
						init_node_info(pAttachNode);
					}
//#endif

					if (pAttachNode->nModelInfo & MODEL_MATRIX_ANI)
					{
						pAttachNode->pLocation = (location_t*)CORE_ALLOC(
							sizeof(location_t) * frame_num);

						if (!read_file(pFile, pAttachNode->pLocation, 
							sizeof(location_t) * frame_num))
						{
							LOG_ERROR("(load_model_node)attach "
								"node location read error! file %s, "
								"node name %s", pMe->pszName, pNode->pszName);
							return false;
						}

						if (pAttachNode->nModelInfo & HELPER_CAMERA_FLAG)
						{
							pAttachNode->pCamera = (node_camera_t*)
								CORE_ALLOC(sizeof(node_camera_t) * frame_num);

							if (!read_file(pFile, pAttachNode->pCamera, 
								sizeof(node_camera_t) * frame_num))
							{
								LOG_ERROR("(load_model_node)attach "
									"node camera read error! file %s, "
									"node name %s", pMe->pszName, 
									pNode->pszName);
								return false;
							}
						}
					}
					else
					{
						pAttachNode->pLocation = (location_t*)CORE_ALLOC(
							sizeof(location_t));

						if (!read_file(pFile, 
							pAttachNode->pLocation, sizeof(location_t)))
						{
							LOG_ERROR("(load_model_node)attach "
								"node location read error! file %s, node "
								"name %s", pMe->pszName, pNode->pszName);
							return false;
						}

						if (pAttachNode->nModelInfo & HELPER_CAMERA_FLAG)
						{
							pAttachNode->pCamera = (node_camera_t*)
								CORE_ALLOC(sizeof(node_camera_t));

							if (!read_file(pFile, 
								pAttachNode->pCamera, sizeof(node_camera_t)))
							{
								LOG_ERROR("(load_model_node)attach "
									"node camera read error! file %s, node "
									"name %s", pMe->pszName, pNode->pszName);
								return false;
							}
						}
					}

					if (pMe->pNodeList == NULL)
					{
						Assert(pMe->nNodeCount == 0);

						pMe->pNodeList = (model_node_t**)CORE_ALLOC(
							sizeof(model_node_t*) * 1);
						pMe->pNodeList[pMe->nNodeCount] = pAttachNode;
					}
					else
					{
						model_node_t** p = (model_node_t**)CORE_ALLOC(
							sizeof(model_node_t*) * (pMe->nNodeCount + 1));

						memcpy(p, pMe->pNodeList, 
							sizeof(model_node_t*) * pMe->nNodeCount);

						if (pMe->pNodeList)
						{
							CORE_FREE(pMe->pNodeList, 
								sizeof(model_node_t*) * pMe->nNodeCount);
						}

						pMe->pNodeList = p;
						pMe->pNodeList[pMe->nNodeCount] = pAttachNode;
					}

					pAttachNode->nIndex = pMe->nNodeCount;
					pMe->nNodeCount++;
				}
			}
//#ifdef FXVERSION_MERGE002
		}
//#endif
	}

	// Load child node
	if (!read_file(pFile, &pNode->nChildNodeCount, 
		sizeof(pNode->nChildNodeCount)))
	{
		LOG_ERROR("(load_model_node)node child count read error! "
			"file %s, node name %s", pMe->pszName, pNode->pszName);
		return false;
	}

	if (pNode->nChildNodeCount > 0x200)
	{
		LOG_ERROR("[error 030004](load_model_node)node child "
			"number(%d) error! file %s", pNode->nChildNodeCount, pMe->pszName);
		pNode->nChildNodeCount = 0;
		return false;
	}
	else if (pNode->nChildNodeCount > 0)
	{
		pNode->ChildNodes = (model_node_t*)CORE_ALLOC(
			sizeof(model_node_t) * pNode->nChildNodeCount);
		memset(pNode->ChildNodes, 0, sizeof(model_node_t) * pNode->nChildNodeCount);

		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			if (!load_model_node(pMe, &pNode->ChildNodes[i], pFile))
			{
				return false;
			}
		}
	}

	return true;
}

// 卸载模型节点
static void unload_model_node(model_t* pMe, model_node_t* pNode)
{
	Assert(pNode != NULL);

	unsigned int frame_num = pMe->nEndFrame - pMe->nStartFrame + 1;

	if (pNode->pszName)
	{
		CORE_FREE(pNode->pszName, pNode->nNameLen + 1);
		pNode->pszName = NULL;
	}

	if (pNode->pCamera)
	{
		if (pNode->nModelInfo & MODEL_MATRIX_ANI)
		{
			CORE_FREE(pNode->pCamera, sizeof(node_camera_t) * frame_num);
		}
		else
		{
			CORE_FREE(pNode->pCamera, sizeof(node_camera_t));
		}

		pNode->pCamera = NULL;
	}

	if (pNode->pLocation)
	{
		if (pNode->nModelInfo & MODEL_MATRIX_ANI)
		{
			CORE_FREE(pNode->pLocation, sizeof(location_t) * frame_num);
		}
		else
		{
			CORE_FREE(pNode->pLocation, sizeof(location_t));
		}

		pNode->pLocation = NULL;
	}

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		if (NULL == pNode->Materials)
		{
			continue;
		}

		node_material_t* pMat = &pNode->Materials[m];

		SAFE_RELEASE(pMat->MatInfo.DiffuseMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.BumpMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.SpecularMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.SpecularLevelMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.GlossinessMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.EmissiveMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.ReflectionMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.DisplacementMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.LightMap.pTex);
		SAFE_RELEASE(pMat->MatInfo.FilterMap.pTex);

//		SAFE_RELEASE(pMat->pInputLayout);
		SAFE_RELEASE(pMat->pGPUIB);
		SAFE_RELEASE(pMat->pSingleGPUVB);
		SAFE_RELEASE(pMat->pMultiGPUVB);
		SAFE_RELEASE(pMat->pVBlendGPUVB);

		if (pMat->pszName)
		{
			CORE_FREE(pMat->pszName, pMat->nNameLen + 1);
		}

		if (pMat->pszVertexDefine)
		{
			CORE_FREE(pMat->pszVertexDefine, 
				strlen(pMat->pszVertexDefine) + 1);
		}

		// LPD3DVERTEXELEMENT9
		//if (pMat->pVElement)
		//{
		//	CORE_FREE(pMat->pVElement, 
		//		sizeof(D3D11_INPUT_ELEMENT_DESC) * pMat->nVElementSize);
		//}

		// IB
		if ((pMat->pLODIB != NULL) && (pMat->pLODIndexCount != NULL))
		{
			for (unsigned int i = 0; i < pMe->nLODCount; ++i)
			{
				if (pMat->pLODIB[i])
				{
					CORE_FREE(pMat->pLODIB[i], 
						sizeof(unsigned short) * pMat->pLODIndexCount[i]);
				}
			}
		}

		// LOD IB
		if (pMat->pLODIB)
		{
			CORE_FREE(pMat->pLODIB, sizeof(unsigned short*) * pMe->nLODCount);
		}

		// LOD IB count
		if (pMat->pLODIndexCount)
		{
			CORE_FREE(pMat->pLODIndexCount, 
				sizeof(unsigned int) * pMe->nLODCount);
		}

		// Single VB
		if (pMat->SingleVB.nCount > 0)
		{
			release_vertex_data(&(pMat->SingleVB));
			pMat->SingleVB.nCount = 0;
		}

		// Multi VB
		if (pMat->pMultiVBs)
		{
			for (unsigned int k = 0; k < frame_num; ++k)
			{
				if (pMat->pMultiVBs[k])
				{
					CORE_FREE(pMat->pMultiVBs[k], 
						pMat->nMultiVertexSize * pMat->nVertexCount);
				}
			}

			CORE_FREE(pMat->pMultiVBs, sizeof(float*) * frame_num);
		}

		// Vertex bone blend table
		if (pMat->pWeightTable)
		{
			CORE_FREE(pMat->pWeightTable, sizeof(node_weight_table_t) 
				* pMat->nVertexCount * MAX_VERTEX_BONE_BLEND);
		}

		// Bone name list
		if (pMat->pszBoneNameList)
		{
			CORE_FREE(pMat->pszBoneNameList, pMat->nBoneNameListLen + 1);
		}

		// Material parameter
		if (pMat->pFrameAmbient)
		{
			CORE_FREE(pMat->pFrameAmbient, sizeof(FmVec3) * frame_num);
		}

		if (pMat->pFrameDiffuse)
		{
			CORE_FREE(pMat->pFrameDiffuse, sizeof(FmVec3) * frame_num);
		}

		if (pMat->pFrameSpecular)
		{
			CORE_FREE(pMat->pFrameSpecular, sizeof(FmVec3) * frame_num);
		}

		if (pMat->pFrameEmissive)
		{
			CORE_FREE(pMat->pFrameEmissive, sizeof(FmVec3) * frame_num);
		}

		if (pMat->pFrameGlossiness)
		{
			CORE_FREE(pMat->pFrameGlossiness, sizeof(float) * frame_num);
		}

		if (pMat->pFrameOpacity)
		{
			CORE_FREE(pMat->pFrameOpacity, sizeof(float) * frame_num);
		}

		if (pMat->pFrameDiffuseTM)
		{
			CORE_FREE(pMat->pFrameDiffuseTM, sizeof(uv_param_t) * frame_num);
		}

		if(pMat->pFrameFilterTM)
		{
			CORE_FREE(pMat->pFrameFilterTM, sizeof(uv_param_t) * frame_num);
		}

		if(pMat->pFrameFilterParam)
		{
			CORE_FREE(pMat->pFrameFilterParam, sizeof(filter_param) * frame_num);
		}

		// Texture names
		if (pMat->MatInfo.DiffuseMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.DiffuseMap.pFileName,
				pMat->MatInfo.DiffuseMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.BumpMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.BumpMap.pFileName,
				pMat->MatInfo.BumpMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.SpecularMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.SpecularMap.pFileName,
				pMat->MatInfo.SpecularMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.SpecularLevelMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.SpecularLevelMap.pFileName,
				pMat->MatInfo.SpecularLevelMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.GlossinessMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.GlossinessMap.pFileName,
				pMat->MatInfo.GlossinessMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.EmissiveMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.EmissiveMap.pFileName,
				pMat->MatInfo.EmissiveMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.ReflectionMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.ReflectionMap.pFileName,
				pMat->MatInfo.ReflectionMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.DisplacementMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.DisplacementMap.pFileName,
				pMat->MatInfo.DisplacementMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.LightMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.LightMap.pFileName,
				pMat->MatInfo.LightMap.nFileNameLen + 1);
		}

		if (pMat->MatInfo.FilterMap.pFileName)
		{
			CORE_FREE(pMat->MatInfo.FilterMap.pFileName,
				pMat->MatInfo.FilterMap.nFileNameLen + 1);
		}

		// Bone list
		if (pMat->pBoneList)
		{
			CORE_FREE(pMat->pBoneList, sizeof(unsigned int) * MAX_BONE);
		}

		pMat->nMappedBoneCount = 0;
	}

	if (pNode->Materials)
	{
		CORE_FREE(pNode->Materials, sizeof(node_material_t) * pNode->nMaterialCount);
		pNode->Materials = NULL;
	}

	if (pNode->pAttachedHelpers != NULL)
	{
		for (unsigned int i = 0; i < pNode->nAttachedHelperCount; ++i)
		{
			unload_model_node(pMe, &pNode->pAttachedHelpers[i]);
		}

		CORE_FREE(pNode->pAttachedHelpers, 
			sizeof(model_node_t) * pNode->nAttachedHelperCount);
		pNode->pAttachedHelpers = NULL;
	}

	if (pNode->pszAttachedHelperParentName)
	{
		CORE_FREE(pNode->pszAttachedHelperParentName, 
			pNode->nAttachedHelperParentNameLen + 1);
		pNode->pszAttachedHelperParentName = NULL;
	}

	if (pNode->ChildNodes != NULL)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			model_node_t* child = &pNode->ChildNodes[i];

			unload_model_node(pMe, child);
		}

		CORE_FREE(pNode->ChildNodes, 
			sizeof(model_node_t) * pNode->nChildNodeCount);
	}
}

// 加载模型实例
model_t* load_model(const char* pszModelName)
{
	Assert(pszModelName != NULL);

	//CORE_LOG_DEVELOPER("[START] LoadVertexAniModel(%s)\n", pszModelName);

	FILE* pFile = core_file::fopen(pszModelName, "rb");

	if (NULL == pFile)
	{
		LOG_ERROR("[error 030008](load_model)can't open model file! "
			"file name %s", pszModelName);
		return NULL;
	}

	model_t* ret = (model_t*)CORE_ALLOC(sizeof(model_t));

	memset(ret, 0, sizeof(model_t));
	//ret->id = -1; // 初始ID

	// Flag
	if (!read_file(pFile, &ret->nFlag, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model flag read failed, file %s", pszModelName);
		CORE_FREE(ret, sizeof(model_t));
		core_file::fclose(pFile);
		return NULL;
	}

	// Version
	if (!read_file(pFile, &ret->nVersion, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model version read failed, file %s",
			pszModelName);
		CORE_FREE(ret, sizeof(model_t));
		core_file::fclose(pFile);
		return NULL;
	}

	if (ret->nVersion != FX_XMOD_VERSION)
	{
		switch (ret->nVersion)
		{
		case 0x1000:
			{
//#ifdef FXVERSION_MERGE002
				//FXLOG_WARNING("LoadVertexAniModel:Version number is not "
				//	"match! File = %s, EngineVersion = %x, FileVersion = %x\n", 
				//	pszModelName, FX_XMOD_VERSION, ret->uVersion);
				break;
//#else
//				//FXLOG_ERROR("[error 030004]  LoadVertexAniModel:Version "
//				//	"number is not match!File = %s, EngineVersion = %x, "
//				//	"FileVersion = %x\n", pszModelName, FX_XMOD_VERSION, 
//				//	ret->uVersion);
//				CORE_FREE(ret, sizeof(model_t));
//				core_file::fclose(pFile);
//				return NULL;
//#endif
			}

		case 0x1001:
			{
//#ifdef FXVERSION_MERGE004
				//FXLOG_WARNING("LoadVertexAniModel:Version number is not "
				//	"match! File = %s, EngineVersion = %x, FileVersion = %x\n", 
				//	pszModelName, FX_XMOD_VERSION, ret->uVersion);
				break;
//#else
//				//FXLOG_ERROR("[error 030004]  LoadVertexAniModel:Version "
//				//	"number is not match!File = %s, EngineVersion = %x, "
//				//	"FileVersion = %x\n", pszModelName, FX_XMOD_VERSION, 
//				//	ret->uVersion);
//				CORE_FREE(ret, sizeof(model_t));
//				core_file::fclose(pFile);
//				return NULL;
//#endif
			}
		case 0x1002:
			break;
		default:
			{
				LOG_ERROR("[error 030004](load_model)version "
					"number is not match! file %s, EngineVersion %x, "
					"FileVersion %x", pszModelName, FX_XMOD_VERSION, 
					ret->nVersion);
				CORE_FREE(ret, sizeof(model_t));
				core_file::fclose(pFile);
				return NULL;
			}
		}
	}

	if (ret->pszTexBase == NULL)
	{
		// 获得模型所在的文件路径
		const char* path_end = pszModelName;
		const char* s = strrchr(pszModelName, '\\');

		if (s)
		{
			path_end = s + 1;
		}
		else
		{
			s = strrchr(pszModelName, '/');

			if (s)
			{
				path_end = s + 1;
			}
		}

		size_t size = path_end - pszModelName;

		ret->pszTexBase = (char*)CORE_ALLOC(size + 1);
		memcpy(ret->pszTexBase, pszModelName, size);
		ret->pszTexBase[size] = 0;
	}

	// File type
	if (!read_file(pFile, &ret->nFileType, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model file type read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// Origin flag
	if (!read_file(pFile, &ret->bOrigin, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model origin read failed, file %s",
			pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// LOD level count
	if (!ret->bOrigin)
	{
		if (!read_file(pFile, &ret->nLODCount, sizeof(unsigned int)))
		{
			LOG_ERROR("(load_model)model lod count read failed, "
				"file %s", pszModelName);
			unload_model(ret);
			core_file::fclose(pFile);
			return NULL;
		}
	}
	else
	{
		ret->nLODCount = 1;
	}

	// Name Length and name
	if (!load_name(pFile, ret->pszName, ret->nNameLen))
	{
		LOG_ERROR("[error](load_model)model name %s\n", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// Start frame, end frame and FPS
	if (!read_file(pFile, &ret->nStartFrame, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model start frame read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->nEndFrame, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model end frame read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->fInternalFPS, sizeof(float)))
	{
		LOG_ERROR("(load_model)model internal fps read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// Root bounding box
	if (!read_file(pFile, &ret->vRootMin, sizeof(FmVec3)))
	{
		LOG_ERROR("(load_model)model root min read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->vRootMax, sizeof(FmVec3)))
	{
		LOG_ERROR("(load_model)model root max read failed, "
			"file %s", pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	ret->vCenter = (ret->vRootMin + ret->vRootMax) * 0.5F;

	// Root node count
	if (!read_file(pFile, &ret->nRootNodeCount, sizeof(unsigned int)))
	{
		LOG_ERROR("(load_model)model root count read failed, file %s", 
			pszModelName);
		unload_model(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (ret->nRootNodeCount > 0)
	{
		if (ret->nRootNodeCount > 0x10000)
		{
			LOG_ERROR("[error 030009](load_model)too much root node! "
				"file %s", pszModelName);
			unload_model(ret);
			core_file::fclose(pFile);
			return NULL;
		}

		// 导入各个结点渲染用数据
		ret->RootNodes = (model_node_t*)CORE_ALLOC(
			sizeof(model_node_t) * ret->nRootNodeCount);
		memset(ret->RootNodes, 0, sizeof(model_node_t) * ret->nRootNodeCount);

		for (unsigned int i = 0; i < ret->nRootNodeCount; ++i)
		{
			if (!load_model_node(ret, &ret->RootNodes[i], pFile))
			{
				LOG_ERROR("[error](load_model)load failed! "
					"model name %s", pszModelName);
				unload_model(ret);
				core_file::fclose(pFile);
				return NULL;
			}
		}

		// 加载模型碰撞树
		load_model_split_tree(ret, pszModelName);
	}

	ret->bLoaded = true;
	core_file::fclose(pFile);
	// 加载模型的材质文件
	load_model_material_file(ret, pszModelName);

	//CORE_LOG_DEVELOPER("[END] LoadVertexAniModel(%s)\n", pszModelName);

	return ret;
}

// 卸载模型实例
void unload_model(model_t* pModel)
{
	Assert(pModel != NULL);

	//CORE_LOG_DEVELOPER("[START] unload_model\n");

	pModel->bLoaded = false;

	if (pModel->pszName)
	{
		CORE_FREE(pModel->pszName, pModel->nNameLen + 1);
		pModel->pszName = NULL;
	}

	if (pModel->pNodeList)
	{
		CORE_FREE(pModel->pNodeList, sizeof(model_node_t*) * pModel->nNodeCount);
		pModel->pNodeList = NULL;
	}

	if (pModel->pszTexBase)
	{
		CORE_FREE(pModel->pszTexBase, strlen(pModel->pszTexBase) + 1);
		pModel->pszTexBase = NULL;
	}

	if (pModel->RootNodes != NULL)
	{
		// 释放模型碰撞树
		unload_model_split_tree(pModel);

		// 释放各个结点的渲染数据
		for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
		{
			unload_model_node(pModel, &pModel->RootNodes[i]);
		}

		CORE_FREE(pModel->RootNodes, sizeof(model_node_t) * pModel->nRootNodeCount);
		pModel->RootNodes = NULL;
	}

	CORE_FREE(pModel, sizeof(model_t));

	//CORE_LOG_DEVELOPER("[END] unload_model\n");

	//return 1;
}


//在原有的顶点并入骨骼数据
inline void merge_bone_data(vertex_data_t& dstData, const vertex_data_t& srcData, const node_weight_table_t* weight_table, int weight_vertex_num)
{
	dstData.nCount = srcData.nCount;
	dstData.nStride = srcData.nStride+sizeof(float)*4+sizeof(int)*4;
	dstData.pVertices = (unsigned char*)CORE_ALLOC(dstData.nStride * srcData.nCount);
	int offset = 0;
	unsigned char* dst_ptr = dstData.pVertices;
	unsigned char* src_ptr = srcData.pVertices;
	for(int i = 0; i < (int)srcData.nCount; i++)
	{
		memcpy(dst_ptr, src_ptr, srcData.nStride);
		dst_ptr += srcData.nStride;
		src_ptr += srcData.nStride;

		
		//BONEID
		memcpy(dst_ptr, &weight_table[offset].nBoneID, sizeof(int));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &weight_table[offset+1].nBoneID, sizeof(int));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &weight_table[offset+2].nBoneID, sizeof(int));
		dst_ptr += sizeof(int);
		memcpy(dst_ptr, &weight_table[offset+3].nBoneID, sizeof(int));
		dst_ptr += sizeof(int);
		
		//WEIGHT
		memcpy(dst_ptr, &weight_table[offset].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+1].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+2].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		memcpy(dst_ptr, &weight_table[offset+3].fWeight, sizeof(float));
		dst_ptr += sizeof(float);
		
		offset += 4;
	}
}

inline void merge_bone_data(vertex_data_t& pData, const node_weight_table_t* weight_table, int weight_vertex_num)
{
	vertex_data_t dstData;
	merge_bone_data(dstData, pData, weight_table, weight_vertex_num);
	release_vertex_data(&pData);
	pData = dstData;
}

