//--------------------------------------------------------------------
// 文件名:		skeleton_data.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "skeleton_data.h"
#include "model_data.h"
#include "array.h"
#include "math_3d.h"
#include "../visual/i_model_system.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"
#include "../public/core_file.h"

#define FX_XSKT_VERSION 0x1005
#define TRANS_SCALE_FLAG (SKELETON_TRANSLATEINFO_FLAG | SKELETON_SCALEINFO_FLAG)

bool unload_skeleton(skeleton_t* pInst);
skt_node_t* get_skeleton_node_from_name(skeleton_t* pInst, const char* pszName);
/*
static const unsigned int _sincos_masks[] = { (unsigned int)0x0, (unsigned int)~0x0 };
static const unsigned int _sincos_inv_masks[] = { (unsigned int)~0x0, (unsigned int)0x0 };


#define _PS_EXTERN_CONST(Name, Val) \
	const __declspec(align(16)) float _ps_##Name[4] = { Val, Val, Val, Val }
#define _PS_EXTERN_CONST_TYPE(Name, Type, Val) \
	const __declspec(align(16)) Type _ps_##Name[4] = { Val, Val, Val, Val };
#define _EPI32_CONST(Name, Val) \
	static const __declspec(align(16)) __int32 _epi32_##Name[4] = { Val, Val, Val, Val }
#define _PS_CONST(Name, Val) \
	static const __declspec(align(16)) float _ps_##Name[4] = { Val, Val, Val, Val }

_PS_EXTERN_CONST(am_0, 0.0F);
_PS_EXTERN_CONST(am_1, 1.0F);
_PS_EXTERN_CONST(am_m1, -1.0F);
_PS_EXTERN_CONST(am_0p5, 0.5F);
_PS_EXTERN_CONST(am_1p5, 1.5F);
_PS_EXTERN_CONST(am_pi, FLOAT_PI);
_PS_EXTERN_CONST(am_pi_o_2, FLOAT_PI / 2.0F);
_PS_EXTERN_CONST(am_2_o_pi, 2.0F / FLOAT_PI);
_PS_EXTERN_CONST(am_pi_o_4, FLOAT_PI / 4.0F);
_PS_EXTERN_CONST(am_4_o_pi, 4.0F / FLOAT_PI);
_PS_EXTERN_CONST_TYPE(am_sign_mask, unsigned int, 0x80000000);
_PS_EXTERN_CONST_TYPE(am_inv_sign_mask, unsigned int, ~0x80000000);
_EPI32_CONST(1, 1);
_EPI32_CONST(2, 2);
_PS_CONST(sincos_p0, 0.15707963267948963959e1F);
_PS_CONST(sincos_p1, -0.64596409750621907082e0F);
_PS_CONST(sincos_p2, 0.7969262624561800806e-1F);
_PS_CONST(sincos_p3, -0.468175413106023168e-2F);*/

static inline unsigned int get_location_size(int info, int frame)
{
	unsigned int size = sizeof(location_t);

	if ((info & SKELETON_TRANSLATEINFO_FLAG) 
		&& (info & SKELETON_SCALEINFO_FLAG))
	{
		size += (frame - 1) * sizeof(location_t);
	}
	else if (info & SKELETON_SCALEINFO_FLAG)
	{
		size += (frame - 1) * sizeof(location_csr_t);
	}
	else if (info & SKELETON_TRANSLATEINFO_FLAG)
	{
		size += (frame - 1) * sizeof(location_ctr_t);
	}
	else
	{
		size += (frame - 1) * sizeof(location_cr_t);
	}

	return size;
}

static void push_back_bone_to_bone_list(skeleton_t* pInst, skt_node_t* pBoneNode)
{
	unsigned int count = pInst->nTotalBoneNodeCount;

	if (count == pInst->nBoneNodeListSize)
	{
		unsigned int new_size;

		if (0 == count)
		{
			new_size = 8;
		}
		else
		{
			new_size = count * 2;
		}

		skt_node_t** p = (skt_node_t**)CORE_ALLOC(
			sizeof(skt_node_t*) * new_size);

		if (pInst->pBoneNodeList)
		{
			memcpy(p, pInst->pBoneNodeList, sizeof(skt_node_t*) * count);
			CORE_FREE(pInst->pBoneNodeList, 
				sizeof(skt_node_t*) * pInst->nBoneNodeListSize);
		}

		pInst->pBoneNodeList = p;
		pInst->nBoneNodeListSize = new_size;
	}

	pInst->pBoneNodeList[count] = pBoneNode;
	pInst->nTotalBoneNodeCount++;
}

static void push_back_helper_to_helper_list(skeleton_t* pInst, 
	skt_node_t* pBoneNode)
{
	unsigned int count = pInst->nTotalHelperNodeCount;

	if (count == pInst->nHelperNodeListSize)
	{
		unsigned int new_size;

		if (0 == count)
		{
			new_size = 8;
		}
		else
		{
			new_size = count * 2;
		}

		skt_node_t** p = (skt_node_t**)CORE_ALLOC(
			sizeof(skt_node_t*) * new_size);

		if (pInst->pHelperNodeList)
		{
			memcpy(p, pInst->pHelperNodeList, sizeof(skt_node_t*) * count);
			CORE_FREE(pInst->pHelperNodeList, 
				sizeof(skt_node_t*) * pInst->nHelperNodeListSize);
		}

		pInst->pHelperNodeList = p;
		pInst->nHelperNodeListSize = new_size;
	}

	pInst->pHelperNodeList[count] = pBoneNode;
	pInst->nTotalHelperNodeCount++;
}

static void push_back_node_to_node_list(skeleton_t* pInst, 
	skt_node_t* pNode)
{
	unsigned int count = pInst->nTotalNodeCount;

	if (count == pInst->nNodeListSize)
	{
		unsigned int new_size;

		if (0 == count)
		{
			new_size = 8;
		}
		else
		{
			new_size = count * 2;
		}

		skt_node_t** p = (skt_node_t**)CORE_ALLOC(
			sizeof(skt_node_t*) * new_size);

		if (pInst->pNodeList)
		{
			memcpy(p, pInst->pNodeList, sizeof(skt_node_t*) * count);
			CORE_FREE(pInst->pNodeList, 
				sizeof(skt_node_t*) * pInst->nNodeListSize);
		}

		pInst->pNodeList = p;
		pInst->nNodeListSize = new_size;
	}

	pInst->pNodeList[count] = pNode;
	pInst->nTotalNodeCount++;
}

// 创建骨骼节点的哈希表以提高搜索速度
static void create_skeleton_hash_table(skeleton_t* pInst)
{
	unsigned int node_num = pInst->nTotalNodeCount;
	skt_node_t** node_list = pInst->pNodeList;
	skt_node_t** pHashBuckets = (skt_node_t**)CORE_ALLOC(
		sizeof(skt_node_t*) * node_num);

	memset(pHashBuckets, 0, sizeof(skt_node_t*) * node_num);

	for (unsigned int k = 0; k < node_num; ++k)
	{
		skt_node_t* node = node_list[k];
		unsigned int hash = GetHashValueCase(node->pszName);
		unsigned int bucket = hash % node_num;

		node->nHash = hash;
		node->pBucketNext = pHashBuckets[bucket];
		pHashBuckets[bucket] = node;
	}

	pInst->pHashBuckets = pHashBuckets;
}

static inline bool read_file(FILE* fp, void* buffer, size_t size)
{
	return core_file::fread(buffer, sizeof(char), size, fp) == size;
}

static bool	load_skeleton_node(const char* pszModelName, 
	skeleton_t* pMe, skt_node_t* pNode, FILE* pFile)
{
	Assert(pMe != NULL);
	Assert(pNode != NULL);
	Assert(pFile != NULL);

	unsigned int frame_num = pMe->nEndFrame - pMe->nStartFrame + 1;

	// Name
	if (!read_file(pFile, &pNode->nNameLen, 
		sizeof(pNode->nNameLen)))
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node name length "
			"read error! file %s", pszModelName);
		return false;
	}

	if (pNode->nNameLen > 0x1000)
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node name length "
			"error! file %s", pszModelName);
		return false;
	}

	if (pNode->nNameLen < sizeof(pNode->szShortName))
	{
		// 短名字避免申请动态内存
		pNode->pszName = pNode->szShortName;
	}
	else
	{
		pNode->pszName = (char*)CORE_ALLOC(pNode->nNameLen + 1);
	}

	if (!read_file(pFile, pNode->pszName, pNode->nNameLen))
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node name read "
			"error! file %s", pszModelName);
		return false;
	}

	pNode->pszName[pNode->nNameLen] = 0;

	// Type
	if (!read_file(pFile, &pNode->nType, sizeof(pNode->nType)))
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node type read "
			"error! file %s", pszModelName);
		return false;
	}

//#ifdef FXVERSION_MERGE001
	if ((unsigned int)pNode->nType == 1)
	{
		pNode->nType = FXNODE_TYPE_BONE;
	}
	else if ((unsigned int)pNode->nType == 2)
	{
		pNode->nType = FXNODE_TYPE_HELPER;
	}
//#endif // FXVERSION_MERGE001

//#ifdef FXVERSION_MERGE005
	if (pMe->nVersion >= 0x1005)
	{
		if (!read_file(pFile, &pNode->nNodeInfo, sizeof(pNode->nNodeInfo)))
		{
			LOG_ERROR("[error 030004](load_skeleton_node)node info read "
				"error! file %s", pszModelName);
			return false;
		}
	}
//#ifdef FXVERSION_MERGE003
	else if (pMe->nVersion == 0x1004)
	{
		// 0x1004 版本的动作文件增加是否有缩放动画的标记位信息
		unsigned int bScale;

		if (!read_file(pFile, &bScale, sizeof(bScale)))
		{
			LOG_ERROR("[error 030004](load_skeleton_node)node scale read "
				"error! file %s", pszModelName);
			return false;
		}

		if (bScale)
		{
			pNode->nNodeInfo |= SKELETON_SCALEINFO_FLAG;
		}
	}
//#else // FXVERSION_MERGE003
//	unsigned int bScale;
//
//	if (!read_file(pFile, &bScale, sizeof(bScale)))
//	{
//		//FXLOG_ERROR("[error 030004] FXLoadSkeletonAniModel: node scale read "
//		//	"error! File = %s\n", pszModelName);
//		return false;
//	}
//
//	if (bScale)
//	{
//		pNode->nNodeInfo |= SKELETON_SCALEINFO_FLAG;
//	}
//#endif // FXVERSION_MERGE003

	if (pMe->nVersion < 0x1005)
	{
		if (strstr(pNode->pszName, "!T") != NULL)
		{
			pNode->nNodeInfo |= SKELETON_TRANSLATEINFO_FLAG;
		}

		if (pNode->pParent == NULL)
		{
			pNode->nNodeInfo |= SKELETON_TRANSLATEINFO_FLAG;
		}
	}

//#else // FXVERSION_MERGE005
//	if (!read_file(pFile, &pNode->nNodeInfo, 
//		sizeof(pNode->nNodeInfo)))
//	{
//		//FXLOG_ERROR("[error 030004] FXLoadSkeletonAniModel: node info read "
//		//	"error! File = %s\n", pszModelName);
//		return false;
//	}
//#endif // FXVERSION_MERGE005

	// 
	if (pNode->nType == FXNODE_TYPE_BONE)
	{
		if (!read_file(pFile, &pNode->bActive, sizeof(pNode->bActive)))
		{
			LOG_ERROR("[error 030004](load_skeleton_node)node active "
				"read error! file %s", pszModelName);
			return false;
		}

		if (pNode->bActive)
		{
			unsigned int nSize = get_location_size(pNode->nNodeInfo, frame_num);

			pNode->pLocation = (location_t*)CORE_ALLOC(nSize);
			
			if (!read_file(pFile, pNode->pLocation, nSize))
			{
				LOG_ERROR("[error 030004](load_skeleton_node)node "
					"location read error! file %s", pszModelName);
				return false;
			}

			pNode->nLocationSize = nSize;
			pNode->nLocationCount = frame_num;
		}
		else
		{
			pNode->pLocation = &pNode->LocationZero;
			
			if (!read_file(pFile, pNode->pLocation, sizeof(location_t)))
			{
				LOG_ERROR("[error 030004](load_skeleton_node)node "
					"location read error! file %s", pszModelName);
				return false;
			}

			pNode->nLocationCount = 1;
		}

		// Regist to node list
		pNode->nBoneId = pMe->nTotalBoneNodeCount;
		push_back_bone_to_bone_list(pMe, pNode);

		// Get additional information
		//if (strstr(pNode->pszName, "!K_ClothPiece") != NULL)
		//{
		//	pNode->bKineticsClothPiece = true;
		//}
	}
	else if (pNode->nType == FXNODE_TYPE_HELPER)
	{
//#ifdef FXVERSION_MERGE005
		if (pMe->nVersion >= 0x1005)
		{
//#endif
			if (pNode->nNodeInfo & SKELETON_HELPERANI_FLAG)
			{
				unsigned int nSize = get_location_size(pNode->nNodeInfo, 
					frame_num);

				pNode->pLocation = (location_t*)CORE_ALLOC(nSize);
				
				if (!read_file(pFile, pNode->pLocation, nSize))
				{
					LOG_ERROR("[error 030004](load_skeleton_node)node "
						"location read error! file %s", pszModelName);
					return false;
				}
				
				pNode->nLocationSize = nSize;
				pNode->nLocationCount = frame_num;
			}
			else
			{
				pNode->pLocation = &pNode->LocationZero;
				
				if (!read_file(pFile, pNode->pLocation, sizeof(location_t)))
				{
					LOG_ERROR("[error 030004](load_skeleton_node)node "
						"location read error! file %s", pszModelName);
					return false;
				}
				
				pNode->nLocationCount = 1;
			}
//#ifdef FXVERSION_MERGE005
		}
		else
		{
			if (pNode->nNameLen > MODEL_MATRIX_POS 
				&& pNode->pszName[MODEL_MATRIX_POS] == 'M')
			{
				pNode->nNodeInfo |= SKELETON_HELPERANI_FLAG;

				unsigned int nSize = get_location_size(pNode->nNodeInfo, 
					frame_num);
				pNode->pLocation = (location_t*)CORE_ALLOC(nSize);
				
				if (!read_file(pFile, pNode->pLocation, nSize))
				{
					LOG_ERROR("[error 030004](load_skeleton_node)node "
						"location read error! file %s", pszModelName);
					return false;
				}
				
				pNode->nLocationSize = nSize;
				pNode->nLocationCount = frame_num;
			}
			else
			{
				pNode->pLocation = &pNode->LocationZero;
				
				if (!read_file(pFile, pNode->pLocation, sizeof(location_t)))
				{
					LOG_ERROR("[error 030004](load_skeleton_node)node "
						"location read error! file %s", pszModelName);
					return false;
				}
				
				pNode->nLocationCount = 1;
			}
		}
//#endif
		
		// Regist to node list
		push_back_helper_to_helper_list(pMe, pNode);
	}
	else
	{
		// 错误的节点类型
		return false;
	}

	pNode->nIndex = pMe->nTotalNodeCount;
	push_back_node_to_node_list(pMe, pNode);

	// Child
	if (!read_file(pFile, &pNode->nChildNodeCount, 
		sizeof(pNode->nChildNodeCount)))
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node "
			"child count read error! file %s", pszModelName);
		return false;
	}

	if (pNode->nChildNodeCount > 0x100)
	{
		LOG_ERROR("[error 030004](load_skeleton_node)node child "
			"number(%d) error! file %s", pNode->nChildNodeCount, pszModelName);
		pNode->nChildNodeCount = 0;
		return false;
	}
	else if (pNode->nChildNodeCount > 0)
	{
		pNode->ChildNodes = (skt_node_t*)CORE_ALLOC(
			sizeof(skt_node_t) * pNode->nChildNodeCount);
		memset(pNode->ChildNodes, 0, 
			sizeof(skt_node_t) * pNode->nChildNodeCount);

		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			pNode->ChildNodes[i].pParent = pNode;

			if (!load_skeleton_node(pszModelName, pMe, &pNode->ChildNodes[i], 
				pFile))
			{
				return false;
			}
		}
	}

	return true;
}

skeleton_t* load_skeleton(const char* pszModelName)
{
	Assert(pszModelName != NULL);

	//FXLOG_DEVELOPER("[START] FXLoadSkeletonAniModel\n");

	FILE* pFile = core_file::fopen(pszModelName, "rb");
	
	if (NULL == pFile)
	{
		return NULL;
	}

	skeleton_t* ret = (skeleton_t*)CORE_ALLOC(sizeof(skeleton_t));

	memset(ret, 0, sizeof(skeleton_t));
	//ret->id = -1; // 初始id

	// Flag
	if (!read_file(pFile, &ret->nFlag, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)flag read error, "
			"file %s", pszModelName);
		CORE_FREE(ret, sizeof(skeleton_t));
		core_file::fclose(pFile);
		return NULL;
	}

	// Version
	if (!read_file(pFile, &ret->nVersion, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)version read error, "
			"file = %s", pszModelName);
		CORE_FREE(ret, sizeof(skeleton_t));
		core_file::fclose(pFile);
		return NULL;
	}

	if (ret->nVersion != FX_XSKT_VERSION)
	{
		switch (ret->nVersion)
		{
		case 0x1000:
			{
				LOG_ERROR("[error 030004](load_skeleton)version "
					"number is not match! file %s, EngineVersion %x, "
					"FileVersion %x", pszModelName, FX_XSKT_VERSION, 
					ret->nVersion);
				CORE_FREE(ret, sizeof(skeleton_t));
				core_file::fclose(pFile);
				return NULL;
			}
		case 0x1001:
			{
				LOG_ERROR("[error 030004](load_skeleton)version "
					"number is not match! file %s, EngineVersion %x, "
					"FileVersion %x", pszModelName, FX_XSKT_VERSION, 
					ret->nVersion);
				CORE_FREE(ret, sizeof(skeleton_t));
				core_file::fclose(pFile);
				return NULL;
			}
		case 0x1002:
			{
				LOG_ERROR("[error 030004](load_skeleton)version "
					"number is not match! file %s, EngineVersion %x, "
					"FileVersion %x", pszModelName, FX_XSKT_VERSION, 
					ret->nVersion);
				CORE_FREE(ret, sizeof(skeleton_t));
				core_file::fclose(pFile);
				return NULL;
			}
		case 0x1003:
			{
//#ifdef FXVERSION_MERGE003
				//FXLOG_WARNING("FXLoadSkeletonAniModel:Version number is not "
				//	"match! File = %s, EngineVersion = %x, FileVersion = %x\n", 
				//	pszModelName, FX_XSKT_VERSION, ret->uVersion);
				break;
//#else
//				//FXLOG_ERROR("[error 030004] FXLoadSkeletonAniModel:Version "
//				//	"number is not match!File = %s, EngineVersion = %x, "
//				//	"FileVersion = %x\n", pszModelName, FX_XSKT_VERSION, 
//				//	ret->uVersion);
//				CORE_FREE(ret, sizeof(skeleton_t));
//				core_file::fclose(pFile);
//				return NULL;
//#endif
			}
		case 0x1004:
			{
//#ifdef FXVERSION_MERGE005
				//FXLOG_WARNING("FXLoadSkeletonAniModel:Version number is not "
				//	"match! File = %s, EngineVersion = %x, FileVersion = %x\n", 
				//	pszModelName, FX_XSKT_VERSION, ret->uVersion);
				break;
//#else
//				//FXLOG_ERROR("[error 030004] FXLoadSkeletonAniModel:Version "
//				//	"number is not match! File = %s, EngineVersion = %x, "
//				//	"FileVersion = %x\n", pszModelName, FX_XSKT_VERSION, 
//				//	ret->uVersion);
//				CORE_FREE(ret, sizeof(skeleton_t));
//				core_file::fclose(pFile);
//				return NULL;
//#endif
			}
		case 0x1005:
			{
				break;
			}
		default:
			{
				LOG_ERROR("[error 030004](load_skeleton)version "
					"number is not match! file %s, EngineVersion %x, "
					"FileVersion %x", pszModelName, FX_XSKT_VERSION, 
					ret->nVersion);
				CORE_FREE(ret, sizeof(skeleton_t));
				core_file::fclose(pFile);
				return NULL;
			}
		}
	}

	// File type
	if (!read_file(pFile, &ret->nFileType, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)file type read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// Name Length and name
	if (!read_file(pFile, &ret->nNameLen, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)name length read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (ret->nNameLen > 0x1000)
	{
		LOG_ERROR("[error 030004](load_skeleton)name length "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	ret->pszName = (char*)CORE_ALLOC(ret->nNameLen + 1);
	if (!read_file(pFile, ret->pszName, ret->nNameLen))
	{
		LOG_ERROR("[error 030004](load_skeleton)name read error! "
			"file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}
	ret->pszName[ret->nNameLen] = '\0';

	// Start frame, end frame and FPS
	if (!read_file(pFile, &ret->nStartFrame, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)start frame read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->nEndFrame, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)end frame read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if ((ret->nEndFrame - ret->nStartFrame) > 0x10000)
	{
		LOG_ERROR("[error 030004](load_skeleton)too many frames! "
			"file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->fInternalFPS, sizeof(float)))
	{
		LOG_ERROR("[error 030004](load_skeleton)internal fps read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->vRootMin, sizeof(FmVec3)))
	{
		LOG_ERROR("[error 030004](load_skeleton)min root read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if (!read_file(pFile, &ret->vRootMax, sizeof(FmVec3)))
	{
		LOG_ERROR("[error 030004](load_skeleton)max root read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if ((ret->vRootMax.x - ret->vRootMin.x) > 50.0F 
		|| (ret->vRootMax.y - ret->vRootMin.y) > 50.0F 
		|| (ret->vRootMax.z - ret->vRootMin.z) > 50.0F)
	{
		//FXLOG_WARNING("min = %f, %f, %f, max = %f, %f, %f\n", 
		//	ret->bbRootMin.x, ret->bbRootMin.y, ret->bbRootMin.z,
		//	ret->bbRootMax.x, ret->bbRootMax.y,	ret->bbRootMax.z);
	}

	// Root node count
	if (!read_file(pFile, &ret->nRootNodeCount, sizeof(unsigned int)))
	{
		LOG_ERROR("[error 030004](load_skeleton)root count read "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	if ((ret->nRootNodeCount > 0x100) || (ret->nRootNodeCount <= 0))
	{
		LOG_ERROR("[error 030004](load_skeleton)root node number "
			"error! file %s", pszModelName);
		unload_skeleton(ret);
		core_file::fclose(pFile);
		return NULL;
	}

	// Load every bone node
	ret->RootNodes = (skt_node_t*)CORE_ALLOC(
		sizeof(skt_node_t) * ret->nRootNodeCount);
	memset(ret->RootNodes, 0, sizeof(skt_node_t) * ret->nRootNodeCount);

	for (unsigned int i = 0; i < ret->nRootNodeCount; ++i)
	{
		if (!load_skeleton_node(pszModelName, ret, &ret->RootNodes[i], pFile))
		{
			unload_skeleton(ret);
			core_file::fclose(pFile);
			return NULL;
		}
	}

	core_file::fclose(pFile);

	// 创建节点的哈希索引表
	create_skeleton_hash_table(ret);

	//FXLOG_DEVELOPER("[END] FXLoadSkeletonAniModel\n");

	return ret;
}

static void unload_skeleton_node(skt_node_t* pNode, skt_node_t* allNodeList)
{
	Assert(pNode != NULL);

	// Child
	for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
	{
		unload_skeleton_node(&pNode->ChildNodes[i], allNodeList);
	}

	// Myself
	if (pNode->pszName != pNode->szShortName)
	{
		CORE_FREE(pNode->pszName, pNode->nNameLen + 1);
		pNode->pszName = NULL;
	}

	if (pNode->pLocation != &pNode->LocationZero)
	{
		CORE_FREE(pNode->pLocation, pNode->nLocationSize);
		pNode->pLocation = NULL;
	}

	if (NULL == allNodeList)
	{
		CORE_FREE(pNode->ChildNodes, sizeof(skt_node_t) * pNode->nChildNodeCount);
		pNode->ChildNodes = NULL;
	}
}

bool unload_skeleton(skeleton_t* pInst)
{
	Assert(pInst != NULL);

	if (pInst != NULL)
	{
		pInst->bLoaded = false;

		if (pInst->RootNodes != NULL)
		{
			// 释放各个结点
			for (unsigned int i = 0; i < pInst->nRootNodeCount; ++i)
			{
				unload_skeleton_node(&pInst->RootNodes[i], pInst->AllNodeList);
			}

			if (NULL == pInst->AllNodeList)
			{
				CORE_FREE(pInst->RootNodes, 
					sizeof(skt_node_t) * pInst->nRootNodeCount);
			}
		}

		if (pInst->pszName)
		{
			CORE_FREE(pInst->pszName, pInst->nNameLen + 1);
		}

		if (pInst->pBoneNodeList)
		{
			CORE_FREE(pInst->pBoneNodeList, 
				sizeof(skt_node_t*) * pInst->nBoneNodeListSize);
		}

		if (pInst->pHelperNodeList)
		{
			CORE_FREE(pInst->pHelperNodeList, 
				sizeof(skt_node_t*) * pInst->nHelperNodeListSize);
		}

		if (pInst->pNodeList)
		{
			CORE_FREE(pInst->pNodeList, 
				sizeof(skt_node_t*) * pInst->nNodeListSize);
		}

		if (pInst->AllNodeList)
		{
			CORE_FREE(pInst->AllNodeList, 
				sizeof(skt_node_t) * pInst->nTotalNodeCount);
		}

		if (pInst->pHashBuckets)
		{
			CORE_FREE(pInst->pHashBuckets, 
				sizeof(skt_node_t*) * pInst->nTotalNodeCount);
		}

		CORE_FREE(pInst, sizeof(skeleton_t));
	}

	return true;
}

static void copy_skeleton_structure_node(skeleton_t* pDstInst, 
	skt_node_t* pDst, skt_node_t* pSrc, unsigned int& node_used)
{
	unsigned int name_len = pSrc->nNameLen;

	if (name_len < sizeof(pDst->szShortName))
	{
		// 名字较短的时候避免申请动态内存
		pDst->pszName = pDst->szShortName;
	}
	else
	{
		pDst->pszName = (char*)CORE_ALLOC(name_len + 1);
	}
	
	memcpy(pDst->pszName, pSrc->pszName, name_len + 1);
	pDst->nNameLen = name_len;
	pDst->bActive = pSrc->bActive;
	pDst->nType = pSrc->nType;
	pDst->nIndex = pSrc->nIndex;
	pDst->nBoneId = pSrc->nBoneId;
	pDst->nNodeInfo = pSrc->nNodeInfo;
	//pDst->bKineticsClothPiece = pSrc->bKineticsClothPiece;

	Assert(pSrc->nLocationCount > 0);

	// 只有一个位置数据时不申请动态内存
	pDst->pLocation = &pDst->LocationZero;
	pDst->nLocationCount = 1;
	// 骨骼的初始位置
	pDst->pLocation[0] = pSrc->pLocation[0];

	if (pSrc->nType == FXNODE_TYPE_BONE)
	{
		push_back_bone_to_bone_list(pDstInst, pDst);
	}
	else if (pSrc->nType == FXNODE_TYPE_HELPER)
	{
		push_back_helper_to_helper_list(pDstInst, pDst);
	}

	push_back_node_to_node_list(pDstInst, pDst);
	
	//pDst->initInverseMatrix = pSrc->initInverseMatrix;

	unsigned int child_num = pSrc->nChildNodeCount;

	if (child_num > 0)
	{
		pDst->ChildNodes = pDstInst->AllNodeList + node_used;
		pDst->nChildNodeCount = child_num;
		node_used += child_num;

		for (unsigned int i = 0; i < child_num; ++i)
		{
			skt_node_t* child = &pDst->ChildNodes[i];

			child->pParent = pDst;
			copy_skeleton_structure_node(pDstInst, child, &pSrc->ChildNodes[i], 
				node_used);
		}
	}
}

void copy_skeleton_structure(skeleton_t* pDstInst, skeleton_t* pSrcInst)
{
	Assert(pSrcInst != pDstInst);

	if (0 == pSrcInst->nTotalBoneNodeCount)
	{
		return;
	}

	for (unsigned int i = 0; i < pDstInst->nRootNodeCount; ++i)
	{
		unload_skeleton_node(&pDstInst->RootNodes[i], pDstInst->AllNodeList);
	}
	
	if (pDstInst->RootNodes)
	{
		if (NULL == pDstInst->AllNodeList)
		{
			CORE_FREE(pDstInst->RootNodes, 
				sizeof(skt_node_t) * pDstInst->nRootNodeCount);
		}
	}

	if (pDstInst->pNodeList)
	{
		CORE_FREE(pDstInst->pNodeList, 
			sizeof(skt_node_t*) * pDstInst->nTotalNodeCount);
		pDstInst->pNodeList = NULL;
	}

	if (pDstInst->pBoneNodeList)
	{
		CORE_FREE(pDstInst->pBoneNodeList, 
			sizeof(skt_node_t*) * pDstInst->nBoneNodeListSize);
		pDstInst->pBoneNodeList = NULL;
	}

	if (pDstInst->pHelperNodeList)
	{
		CORE_FREE(pDstInst->pHelperNodeList, 
			sizeof(skt_node_t*) * pDstInst->nHelperNodeListSize);
		pDstInst->pHelperNodeList = NULL;
	}

	if (pDstInst->AllNodeList)
	{
		CORE_FREE(pDstInst->AllNodeList, 
			sizeof(skt_node_t) * pDstInst->nTotalNodeCount);
		pDstInst->AllNodeList = NULL;
	}

	if (pDstInst->pHashBuckets)
	{
		CORE_FREE(pDstInst->pHashBuckets, 
			sizeof(skt_node_t*) * pDstInst->nTotalNodeCount);
		pDstInst->pHashBuckets = NULL;
	}

	pDstInst->nTotalNodeCount = 0;
	pDstInst->nTotalBoneNodeCount = 0;
	pDstInst->nTotalHelperNodeCount = 0;

	unsigned int node_num = pSrcInst->nTotalNodeCount;

	pDstInst->pNodeList = (skt_node_t**)CORE_ALLOC(
		sizeof(skt_node_t*) * node_num);
	pDstInst->nNodeListSize = node_num;
	pDstInst->pBoneNodeList = (skt_node_t**)CORE_ALLOC(
		sizeof(skt_node_t*) * pSrcInst->nTotalBoneNodeCount);
	pDstInst->nBoneNodeListSize = pSrcInst->nTotalBoneNodeCount;
	pDstInst->pHelperNodeList = (skt_node_t**)CORE_ALLOC(
		sizeof(skt_node_t*) * pSrcInst->nTotalHelperNodeCount);
	pDstInst->nHelperNodeListSize = pSrcInst->nTotalHelperNodeCount;
	pDstInst->AllNodeList = (skt_node_t*)CORE_ALLOC(
		sizeof(skt_node_t) * node_num);
	memset(pDstInst->AllNodeList, 0, sizeof(skt_node_t) * node_num);

	unsigned int node_used = 0;
	unsigned int root_num = pSrcInst->nRootNodeCount;

	pDstInst->RootNodes = pDstInst->AllNodeList + node_used;
	pDstInst->nRootNodeCount = root_num;
	node_used += root_num;

	pDstInst->nStartFrame = 0;
	pDstInst->nEndFrame = 0;

	for (unsigned int i = 0; i < root_num; ++i)
	{
		copy_skeleton_structure_node(pDstInst, &pDstInst->RootNodes[i], 
			&pSrcInst->RootNodes[i], node_used);
	}

	Assert(node_used == node_num);

	create_skeleton_hash_table(pDstInst);
}

void init_skeleton_pose_inverse_matrix(skt_node_t* node, 
	FmMat4* mtxCurrentTM)
{
	Assert(node != NULL);

	FmMat4 mtxOldTM = *mtxCurrentTM;
	FmMat4 mtxTM;

	math_location_to_matrix(&node->pLocation[0], &mtxTM);
	FmMat4Inverse(&mtxTM, NULL,&mtxTM);
	FmMat4Multiply(mtxCurrentTM, mtxCurrentTM, &mtxTM);

	node->mtxInitInverse = *mtxCurrentTM;

	for (unsigned int i = 0; i < node->nChildNodeCount; ++i)
	{
		init_skeleton_pose_inverse_matrix(&node->ChildNodes[i], mtxCurrentTM);
	}

	*mtxCurrentTM = mtxOldTM;
}

// 获得动作的可用帧数
int get_skeleton_frame_count(skeleton_t* pInst)
{
	int uRet = pInst->nEndFrame - pInst->nStartFrame + 1;

	if (uRet < 4)
	{
		return 0;
	}
	
	return uRet - 4;
}

/*
static void FXGetNodeLocation(skeleton_t* pInst, 
	skt_node_t* pNode, unsigned int nFrame, skeleton_t* pBaseSkeleton, 
	int nTranslate, float fScale, location_t* location)
{
	Assert(nFrame < (pInst->uEndFrame - pInst->uStartFrame + 1));

	// 取得结点tpose的location
	location_t* ori_location;

	if ((pBaseSkeleton != NULL) && (FXNODE_TYPE_BONE == pNode->type))
	{
		ori_location = &pBaseSkeleton->boneNodeList[pNode->boneID]->location[0];
	}
	else
	{
		ori_location = &pNode->location[0];
	}
		
	if (nFrame == 0)
	{
		*location = *ori_location;

		if (NULL == pNode->parent)
		{
			if (nTranslate == 1)
			{
				// 只有向前的位移
				location->pz = 0.0F;
			}
			else if (nTranslate == 3)
			{
				location->px = 0.0F;
				location->pz = 0.0F;
			}
		}

		return;
	}

	unsigned int flag = pNode->uNodeInfo & TRANS_SCALE_FLAG;

	if (flag == 0)
	{
		location_cr_t* pLocationCR = (location_cr_t*)((unsigned char*)pNode->location 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_cr_t));
		float pitch = (float)pLocationCR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);

		location->position = ori_location->position;
		location->scale = ori_location->scale;

		if (NULL == pNode->parent)
		{
			if (nTranslate == 1)
			{
				// 只有向前的位移
				location->pz = 0.0F;
			}
			else if (nTranslate == 3)
			{
				location->px = 0.0F;
				location->pz = 0.0F;
			}
			
			location->position *= fScale;
		}

		return;
	}

	if (flag == TRANS_SCALE_FLAG)
	{
		*location = pNode->location[nFrame];

		if (NULL == pNode->parent)
		{
			if (nTranslate == 1)
			{
				// 只有向前的位移
				location->pz = 0.0F;
			}
			else if (nTranslate == 3)
			{
				location->px = 0.0F;
				location->pz = 0.0F;
			}

			location->position *= fScale;
		}

		return;
	}

	if (flag == SKELETON_TRANSLATEINFO_FLAG)
	{
		location_ctr_t* pLocationCTR = (location_ctr_t*)((unsigned char*)pNode->location 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_ctr_t));
		float pitch = (float)pLocationCTR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCTR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCTR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);

		location->px = pLocationCTR->px;
		location->py = pLocationCTR->py;
		location->pz = pLocationCTR->pz;
		location->scale = ori_location->scale;

		if (NULL == pNode->parent)
		{
			if (nTranslate == 1)
			{
				// 只有向前的位移
				location->pz = 0.0F;
			}
			else if (nTranslate == 3)
			{
				location->px = 0.0F;
				location->pz = 0.0F;
			}

			location->position *= fScale;
		}

		return;
	}
	
	if (flag == SKELETON_SCALEINFO_FLAG)
	{
		location_csr_t* pLocationCSR = (location_csr_t*)((unsigned char*)pNode->location 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_csr_t));
		float pitch = (float)pLocationCSR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCSR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCSR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);

		location->sx = pLocationCSR->sx;
		location->sy = pLocationCSR->sy;
		location->sz = pLocationCSR->sz;
		location->position = ori_location->position;

		if (NULL == pNode->parent)
		{
			if (nTranslate == 1)
			{
				// 只有向前的位移
				location->pz = 0.0F;
			}
			else if (nTranslate == 3)
			{
				location->px = 0.0F;
				location->pz = 0.0F;
			}

			location->position *= fScale;
		}

		return;
	}
}
*/

static inline void get_node_location_fast(skeleton_t* pInst, 
	skt_node_t* pNode, unsigned int nFrame, location_t* location)
{
	Assert(nFrame < (pInst->nEndFrame - pInst->nStartFrame + 1));

	if (0 == nFrame)
	{
		*location = pNode->pLocation[0];
		return;
	}

	unsigned int flag = pNode->nNodeInfo & TRANS_SCALE_FLAG;

	if (flag == 0)
	{
		location_cr_t* pLocationCR = (location_cr_t*)((unsigned char*)pNode->pLocation 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_cr_t));
		float pitch = (float)pLocationCR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);

		location->position = pNode->pLocation[0].position;
		location->scale = pNode->pLocation[0].scale;
		return;
	}

	if (flag == TRANS_SCALE_FLAG)
	{
		*location = pNode->pLocation[nFrame];
		return;
	}

	if (flag == SKELETON_TRANSLATEINFO_FLAG)
	{
		location_ctr_t* pLocationCTR = (location_ctr_t*)((unsigned char*)pNode->pLocation 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_ctr_t));
		float pitch = (float)pLocationCTR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCTR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCTR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);
		location->position.x = pLocationCTR->px;
		location->position.y = pLocationCTR->py;
		location->position.z = pLocationCTR->pz;
		location->scale = pNode->pLocation[0].scale;
		return;
	}

	if (flag == SKELETON_SCALEINFO_FLAG)
	{
		location_csr_t* pLocationCSR = (location_csr_t*)((unsigned char*)pNode->pLocation 
			+ sizeof(location_t) + (nFrame - 1) * sizeof(location_csr_t));
		float pitch = (float)pLocationCSR->pitch * FLOAT_PI * 2.0F / 65535.0F;
		float yaw = (float)pLocationCSR->yaw * FLOAT_PI * 2.0F / 65535.0F;
		float roll = (float)pLocationCSR->roll * FLOAT_PI * 2.0F / 65535.0F;

		FmQuatRotationYawPitchRoll(&location->rotate, yaw, pitch, 
			roll);

		location->scale.x = pLocationCSR->sx;
		location->scale.y = pLocationCSR->sy;
		location->scale.z = pLocationCSR->sz;
		location->position = pNode->pLocation[0].position;
		return;
	}
}

static inline void apply_trans_scale_2(int translate, float scale, 
	location_t* loc1, location_t* loc2)
{
	if (translate == 1)
	{
		// 只有向前的位移
		loc1->position.z = 0.0F;
		loc2->position.z = 0.0F;
	}
	else if (translate == 3)
	{
		loc1->position.x = 0.0F;
		loc1->position.z = 0.0F;
		loc2->position.x = 0.0F;
		loc2->position.z = 0.0F;
	}

	loc1->position *= scale;
	loc2->position *= scale;
}

static inline void get_node_rot_2(skt_node_t* pNode, location_t* ori_loc, 
	unsigned int nFrame1, unsigned int nFrame2, location_t* loc1, 
	location_t* loc2)
{
	location_cr_t* pLocationCR = (location_cr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame1 - 1) * sizeof(location_cr_t));
	float pitch = (float)pLocationCR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	float yaw = (float)pLocationCR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	float roll = (float)pLocationCR->roll * FLOAT_PI * 2.0F / 65535.0F;

	FmQuatRotationYawPitchRoll(&loc1->rotate, yaw, pitch, roll);
	loc1->position = ori_loc->position;
	loc1->scale = ori_loc->scale;

	pLocationCR = (location_cr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame2 - 1) * sizeof(location_cr_t));
	pitch = (float)pLocationCR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	yaw = (float)pLocationCR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	roll = (float)pLocationCR->roll * FLOAT_PI * 2.0F / 65535.0F;

	FmQuatRotationYawPitchRoll(&loc2->rotate, yaw, pitch, roll);
	loc2->position = ori_loc->position;
	loc2->scale = ori_loc->scale;
}

static inline void get_node_trans_rot_2(skt_node_t* pNode, location_t* ori_loc, 
	unsigned int nFrame1, unsigned int nFrame2, location_t* loc1, 
	location_t* loc2)
{
	location_ctr_t* pLocationCTR = (location_ctr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame1 - 1) * sizeof(location_ctr_t));
	float pitch = (float)pLocationCTR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	float yaw = (float)pLocationCTR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	float roll = (float)pLocationCTR->roll * FLOAT_PI * 2.0F / 65535.0F;

	FmQuatRotationYawPitchRoll(&loc1->rotate, yaw, pitch, roll);
	loc1->position.x = pLocationCTR->px;
	loc1->position.y = pLocationCTR->py;
	loc1->position.z = pLocationCTR->pz;
	loc1->scale = ori_loc->scale;

	pLocationCTR = (location_ctr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame2 - 1) * sizeof(location_ctr_t));
	pitch = (float)pLocationCTR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	yaw = (float)pLocationCTR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	roll = (float)pLocationCTR->roll * FLOAT_PI * 2.0F / 65535.0F;

	FmQuatRotationYawPitchRoll(&loc2->rotate, yaw, pitch, roll);
	loc2->position.x = pLocationCTR->px;
	loc2->position.y = pLocationCTR->py;
	loc2->position.z = pLocationCTR->pz;
	loc2->scale = ori_loc->scale;
}

static inline void get_node_scale_rot_2(skt_node_t* pNode, location_t* ori_loc, 
	unsigned int nFrame1, unsigned int nFrame2, location_t* loc1, 
	location_t* loc2)
{
	location_csr_t* pLocationCSR = (location_csr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame1 - 1) * sizeof(location_csr_t));
	float pitch = (float)pLocationCSR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	float yaw = (float)pLocationCSR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	float roll = (float)pLocationCSR->roll * FLOAT_PI * 2.0F / 65535.0F;
	
	FmQuatRotationYawPitchRoll(&loc1->rotate, yaw, pitch, roll);
	loc1->scale.x = pLocationCSR->sx;
	loc1->scale.y = pLocationCSR->sy;
	loc1->scale.z = pLocationCSR->sz;
	loc1->position = ori_loc->position;
	
	pLocationCSR = (location_csr_t*)((unsigned char*)pNode->pLocation 
		+ sizeof(location_t) + (nFrame2 - 1) * sizeof(location_csr_t));
	pitch = (float)pLocationCSR->pitch * FLOAT_PI * 2.0F / 65535.0F;
	yaw = (float)pLocationCSR->yaw * FLOAT_PI * 2.0F / 65535.0F;
	roll = (float)pLocationCSR->roll * FLOAT_PI * 2.0F / 65535.0F;

	FmQuatRotationYawPitchRoll(&loc2->rotate, yaw, pitch, roll);
	loc2->scale.x = pLocationCSR->sx;
	loc2->scale.y = pLocationCSR->sy;
	loc2->scale.z = pLocationCSR->sz;
	loc2->position = ori_loc->position;
}

// 获得根节点的帧位置
static inline void get_root_node_location2(skeleton_t* pInst, 
	skt_node_t* pNode, unsigned int nFrame1, unsigned int nFrame2, 
	skeleton_t* pBase, int nTranslate, float fScale, location_t* loc1, 
	location_t* loc2)
{
	Assert(nFrame1 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame1 > 0);
	Assert(nFrame2 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame2 > 0);

	// 取得结点tpose的location
	unsigned int flag = pNode->nNodeInfo & TRANS_SCALE_FLAG;

	if (0 == flag)
	{
		if (pBase)
		{
			get_node_rot_2(pNode, 
				&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
				nFrame2, loc1, loc2);
		}
		else
		{
			get_node_rot_2(pNode, &pNode->pLocation[0], nFrame1, nFrame2, 
				loc1, loc2);
		}

		apply_trans_scale_2(nTranslate, fScale, loc1, loc2);
		return;
	}

	if (flag == TRANS_SCALE_FLAG)
	{
		*loc1 = pNode->pLocation[nFrame1];
		*loc2 = pNode->pLocation[nFrame2];
		apply_trans_scale_2(nTranslate, fScale, loc1, loc2);
		return;
	}

	if (flag == SKELETON_TRANSLATEINFO_FLAG)
	{
		if (pBase)
		{
			get_node_trans_rot_2(pNode, 
				&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
				nFrame2, loc1, loc2);
		}
		else
		{
			get_node_trans_rot_2(pNode, &pNode->pLocation[0], nFrame1, 
				nFrame2, loc1, loc2);
		}

		apply_trans_scale_2(nTranslate, fScale, loc1, loc2);
		return;
	}

	if (flag == SKELETON_SCALEINFO_FLAG)
	{
		if (pBase)
		{
			get_node_scale_rot_2(pNode, 
				&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
				nFrame2, loc1, loc2);
		}
		else
		{
			get_node_scale_rot_2(pNode, &pNode->pLocation[0], nFrame1, 
				nFrame2, loc1, loc2);
		}

		apply_trans_scale_2(nTranslate, fScale, loc1, loc2);
		return;
	}
}

// 获得非根节点的帧位置
static inline void get_bone_node_location2(skeleton_t* pInst, 
	skt_node_t* pNode, unsigned int nFrame1, unsigned int nFrame2, 
	int nTranslate, float fScale, location_t* loc1, location_t* loc2)
{
	Assert(nFrame1 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame1 > 0);
	Assert(nFrame2 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame2 > 0);

	// 取得结点tpose的location
	unsigned int flag = pNode->nNodeInfo & TRANS_SCALE_FLAG;

	if (0 == flag)
	{
		get_node_rot_2(pNode, &pNode->pLocation[0], nFrame1, nFrame2, loc1, 
			loc2);
		return;
	}

	if (flag == TRANS_SCALE_FLAG)
	{
		*loc1 = pNode->pLocation[nFrame1];
		*loc2 = pNode->pLocation[nFrame2];
		return;
	}

	if (flag == SKELETON_TRANSLATEINFO_FLAG)
	{
		get_node_trans_rot_2(pNode, &pNode->pLocation[0], nFrame1, nFrame2, 
			loc1, loc2);
		return;
	}

	if (flag == SKELETON_SCALEINFO_FLAG)
	{
		get_node_scale_rot_2(pNode, &pNode->pLocation[0], nFrame1, nFrame2, 
			loc1, loc2);
		return;
	}
}

// 获得非根节点的帧位置
static inline void get_bone_node_location_base2(skeleton_t* pInst, 
	skt_node_t* pNode, unsigned int nFrame1, unsigned int nFrame2, 
	skeleton_t* pBase, int nTranslate, float fScale, location_t* loc1, 
	location_t* loc2)
{
	Assert(nFrame1 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame1 > 0);
	Assert(nFrame2 < (pInst->nEndFrame - pInst->nStartFrame + 1));
	Assert(nFrame2 > 0);

	// 取得结点tpose的location
	unsigned int flag = pNode->nNodeInfo & TRANS_SCALE_FLAG;

	if (0 == flag)
	{
		get_node_rot_2(pNode, 
			&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
			nFrame2, loc1, loc2);
		return;
	}

	if (flag == TRANS_SCALE_FLAG)
	{
		*loc1 = pNode->pLocation[nFrame1];
		*loc2 = pNode->pLocation[nFrame2];
		return;
	}

	if (flag == SKELETON_TRANSLATEINFO_FLAG)
	{
		get_node_trans_rot_2(pNode, 
			&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
			nFrame2, loc1, loc2);
		return;
	}

	if (flag == SKELETON_SCALEINFO_FLAG)
	{
		get_node_scale_rot_2(pNode, 
			&pBase->pBoneNodeList[pNode->nBoneId]->pLocation[0], nFrame1, 
			nFrame2, loc1, loc2);
		return;
	}
}

bool copy_skeleton_action(skeleton_t* pDstInst, skeleton_t* pSrcInst, 
	bool bLoop, float fSpeed, float fCurrentTime, skeleton_t* pBaseSkeleton, 
	int nTranslate, float fScale, int* pBoneDisable)
{
	Assert(pDstInst->nTotalBoneNodeCount == pSrcInst->nTotalBoneNodeCount);

	unsigned int nFrameCount = get_skeleton_frame_count(pSrcInst);
	unsigned int nFrame;
	unsigned int nFrameNext;
	float fFactor;

	if (nFrameCount > 0)
	{
		float fFPS = fSpeed * pSrcInst->fInternalFPS;

		math_get_frame_lerp(nFrameCount, fFPS, fCurrentTime, bLoop, &nFrame, 
			&fFactor);
		nFrameNext = math_get_next_frame(nFrameCount + 1, nFrame, bLoop);
		
		if ((nFrame > nFrameCount) || (nFrameNext > nFrameCount))
		{
			// 增加越界保护
			return false;
		}

		nFrame += 3;
		nFrameNext += 3;
	}
	else
	{
		nFrame = pSrcInst->nEndFrame - pSrcInst->nStartFrame;
		nFrameNext = nFrame;
		fFactor = 0.0F;
	}

	// Copy bone node location.
	unsigned int src_bone_num = pSrcInst->nTotalBoneNodeCount;
	unsigned int dst_bone_num = pDstInst->nTotalBoneNodeCount;
	skt_node_t** src_bone_list = pSrcInst->pBoneNodeList;
	skt_node_t** dst_bone_list = pDstInst->pBoneNodeList;
	location_t BoneS;
	location_t BoneE;
	skt_node_t* src_bone;
	skt_node_t* dst_bone;

	if (0 == src_bone_num)
	{
		return true;
	}

	// 单独处理根骨骼节点
	src_bone = src_bone_list[0];
	dst_bone = dst_bone_list[0];

	if (src_bone->bActive)
	{
		get_root_node_location2(pSrcInst, src_bone, nFrame, 
			nFrameNext, pBaseSkeleton, nTranslate, fScale, &BoneS, &BoneE);
		math_location_lerp(&dst_bone->pLocation[0], &BoneS, 
			&BoneE, fFactor);
		//dst_bone->uNodeInfo = src_bone->uNodeInfo;
		//dst_bone->bActive = true;
	}

	if (NULL == pBoneDisable)
	{
		if ((NULL == pBaseSkeleton) || (pBaseSkeleton == pSrcInst))
		{
			for (unsigned int i = 1; i < src_bone_num; ++i)
			{
				src_bone = src_bone_list[i];

				if (!src_bone->bActive)
				{
					//dst_bone_list[x]->bActive = false;
					continue;
				}

				dst_bone = dst_bone_list[i];

				get_bone_node_location2(pSrcInst, src_bone, nFrame, 
					nFrameNext, nTranslate, fScale, &BoneS, &BoneE);
				math_location_lerp(&dst_bone->pLocation[0], &BoneS, 
					&BoneE, fFactor);
				//dst_bone->uNodeInfo = src_bone->uNodeInfo;
				//dst_bone->bActive = true;
			}
		}
		else
		{
			for (unsigned int i = 1; i < src_bone_num; ++i)
			{
				src_bone = src_bone_list[i];

				if (!src_bone->bActive)
				{
					//dst_bone_list[x]->bActive = false;
					continue;
				}

				dst_bone = dst_bone_list[i];

				get_bone_node_location_base2(pSrcInst, src_bone, nFrame, 
					nFrameNext, pBaseSkeleton, nTranslate, fScale, &BoneS, 
					&BoneE);
				math_location_lerp(&dst_bone->pLocation[0], &BoneS, 
					&BoneE, fFactor);
				//dst_bone->uNodeInfo = src_bone->uNodeInfo;
				//dst_bone->bActive = true;
			}
		}
	}
	else
	{
		if ((NULL == pBaseSkeleton) || (pBaseSkeleton == pSrcInst))
		{
			for (unsigned int i = 1; i < src_bone_num; ++i)
			{
				if (pBoneDisable[i])
				{
					// 被屏蔽的节点采用初始位置
					dst_bone_list[i]->pLocation[0] = 
						src_bone_list[i]->pLocation[0];
					continue;
				}

				src_bone = src_bone_list[i];

				if (!src_bone->bActive)
				{
					//dst_bone_list[x]->bActive = false;
					continue;
				}

				dst_bone = dst_bone_list[i];

				get_bone_node_location2(pSrcInst, src_bone, nFrame, 
					nFrameNext, nTranslate, fScale, &BoneS, &BoneE);
				math_location_lerp(&dst_bone->pLocation[0], &BoneS, 
					&BoneE, fFactor);
				//dst_bone->uNodeInfo = src_bone->uNodeInfo;
				//dst_bone->bActive = true;
			}
		}
		else
		{
			for (unsigned int i = 1; i < src_bone_num; ++i)
			{
				if (pBoneDisable[i])
				{
					// 被屏蔽的节点采用初始位置
					dst_bone_list[i]->pLocation[0] = 
						src_bone_list[i]->pLocation[0];
					continue;
				}

				src_bone = src_bone_list[i];

				if (!src_bone->bActive)
				{
					//dst_bone_list[i]->bActive = false;
					continue;
				}

				dst_bone = dst_bone_list[i];

				get_bone_node_location_base2(pSrcInst, src_bone, nFrame, 
					nFrameNext, pBaseSkeleton, nTranslate, fScale, &BoneS, 
					&BoneE);
				math_location_lerp(&dst_bone->pLocation[0], &BoneS, 
					&BoneE, fFactor);
				//dst_bone->uNodeInfo = src_bone->uNodeInfo;
				//dst_bone->bActive = true;
			}
		}
	}

	// Copy helper node location.
	unsigned int src_helper_num = pSrcInst->nTotalHelperNodeCount;
	unsigned int dst_helper_num = pDstInst->nTotalHelperNodeCount;

	if ((src_helper_num == 0) || (dst_helper_num == 0))
	{
		// 没有可以处理的辅助点
		return true;
	}

	skt_node_t** src_helper_list = pSrcInst->pHelperNodeList;
	skt_node_t** dst_helper_list = pDstInst->pHelperNodeList;

	for (unsigned int i = 0; i < dst_helper_num; ++i)
	{
		skt_node_t* dst_node = dst_helper_list[i];
		unsigned int hash = dst_node->nHash;
		unsigned int bucket = hash % pSrcInst->nTotalNodeCount;
		skt_node_t* src_node = pSrcInst->pHashBuckets[bucket];

		while (src_node)
		{
			if ((src_node->nHash == hash)
				&& (strcmp(src_node->pszName, dst_node->pszName) == 0))
			{
				unsigned int node_info = src_node->nNodeInfo;

				if (node_info & SKELETON_HELPERANI_FLAG)
				{
					get_bone_node_location2(pSrcInst, src_node, nFrame, 
						nFrameNext, nTranslate, fScale, &BoneS, &BoneE);
					math_location_lerp(&dst_node->pLocation[0], 
						&BoneS, &BoneE, fFactor);
				}
				else
				{
					dst_node->pLocation[0] = src_node->pLocation[0];
				}

				//dst_node->uNodeInfo = node_info;
				break;
			}

			src_node = src_node->pBucketNext;
		}
	}
	
	return true;
}

bool action_blend(skeleton_t* pDstInst, skeleton_t* pSrcInst, 
	float fBlendWeight, bool bLoop2, float fTime2, float fSpeed2, 
	skeleton_t* pBaseSkeleton, int nTranslate, float fScale, int* pBoneDisable)
{
	Assert(pDstInst->nTotalBoneNodeCount == pSrcInst->nTotalBoneNodeCount);

	unsigned int nFrameCnt2 = get_skeleton_frame_count(pSrcInst);
	unsigned int nFrame2;
	unsigned int nFrameNext2;
	float fFactor2;

	if (nFrameCnt2 > 0)
	{
		float fFPS2 = fSpeed2 * pSrcInst->fInternalFPS;

		math_get_frame_lerp(nFrameCnt2, fFPS2, fTime2, bLoop2, &nFrame2,
			&fFactor2);
		nFrameNext2 = math_get_next_frame(nFrameCnt2 + 1, nFrame2, bLoop2);
		
		if ((nFrame2 > nFrameCnt2) || (nFrameNext2 > nFrameCnt2))
		{
			// 增加越界保护
			return false;
		}

		nFrame2 += 3;
		nFrameNext2 += 3;
	}
	else
	{
		nFrame2 = pSrcInst->nEndFrame - pSrcInst->nStartFrame;
		nFrameNext2 = nFrame2;
		fFactor2 = 0.0F;
	}

	skt_node_t** src_bone_list = pSrcInst->pBoneNodeList;
	skt_node_t** dst_bone_list = pDstInst->pBoneNodeList;
	int bone_num = pDstInst->nTotalBoneNodeCount;
	location_t Bone2S, Bone2E, Bone2;
	skt_node_t* src_bone;
	skt_node_t* dst_bone;

	if (0 == bone_num)
	{
		return true;
	}

	// 单独处理根骨骼节点
	src_bone = src_bone_list[0];
	dst_bone = dst_bone_list[0];
	//dst_bone->uNodeInfo |= src_bone->uNodeInfo;

	if (src_bone->bActive)
	{
		get_root_node_location2(pSrcInst, src_bone, nFrame2, 
			nFrameNext2, pBaseSkeleton, nTranslate, fScale, &Bone2S, &Bone2E);
		math_location_lerp(&Bone2, &Bone2S, &Bone2E, fFactor2);

		if (dst_bone->bActive)
		{
			math_location_lerp(&dst_bone->pLocation[0], 
				&dst_bone->pLocation[0], &Bone2, fBlendWeight);
		}
		else
		{
			//dst_bone->bActive = true;
			dst_bone->pLocation[0] = Bone2;
		}
	}
	else if (dst_bone->bActive)
	{
		// 保持原来的形态
	}
	else
	{
		location_t& loc = dst_bone->pLocation[0];

		loc.position = FmVec3(0.0F, 0.0F, 0.0F);
		loc.rotate = FmQuat(0.0F, 0.0F, 0.0F, 1.0F);
		loc.scale = FmVec3(1.0F, 1.0F, 1.0F);
	}

	if (NULL == pBoneDisable)
	{
		if ((NULL == pBaseSkeleton) || (pBaseSkeleton == pSrcInst))
		{
			for (int i = 1; i < bone_num; ++i)
			{
				src_bone = src_bone_list[i];
				dst_bone = dst_bone_list[i];
				//dst_bone->uNodeInfo |= src_bone->uNodeInfo;

				if (src_bone->bActive)
				{
					get_bone_node_location2(pSrcInst, src_bone, nFrame2, 
						nFrameNext2, nTranslate, fScale, &Bone2S, &Bone2E);
					math_location_lerp(&Bone2, &Bone2S, &Bone2E, fFactor2);

					if (dst_bone->bActive)
					{
						math_location_lerp(&dst_bone->pLocation[0], 
							&dst_bone->pLocation[0], &Bone2, fBlendWeight);
					}
					else
					{
						//dst_bone->bActive = true;
						dst_bone->pLocation[0] = Bone2;
					}
				}
				else if (dst_bone->bActive)
				{
					// 保持原来的形态
				}
				else
				{
					location_t& loc = dst_bone->pLocation[0];

					loc.position = FmVec3(0.0F, 0.0F, 0.0F);
					loc.rotate = FmQuat(0.0F, 0.0F, 0.0F, 1.0F);
					loc.scale = FmVec3(1.0F, 1.0F, 1.0F);
				}
			}
		}
		else
		{
			for (int i = 1; i < bone_num; ++i)
			{
				src_bone = src_bone_list[i];
				dst_bone = dst_bone_list[i];
				//dst_bone->uNodeInfo |= src_bone->uNodeInfo;

				if (src_bone->bActive)
				{
					get_bone_node_location_base2(pSrcInst, src_bone, nFrame2, 
						nFrameNext2, pBaseSkeleton, nTranslate, fScale, 
						&Bone2S, &Bone2E);
					math_location_lerp(&Bone2, &Bone2S, &Bone2E, fFactor2);

					if (dst_bone->bActive)
					{
						math_location_lerp(&dst_bone->pLocation[0], 
							&dst_bone->pLocation[0], &Bone2, fBlendWeight);
					}
					else
					{
						//dst_bone->bActive = true;
						dst_bone->pLocation[0] = Bone2;
					}
				}
				else if (dst_bone->bActive)
				{
					// 保持原来的形态
				}
				else
				{
					location_t& loc = dst_bone->pLocation[0];

					loc.position = FmVec3(0.0F, 0.0F, 0.0F);
					loc.rotate = FmQuat(0.0F, 0.0F, 0.0F, 1.0F);
					loc.scale = FmVec3(1.0F, 1.0F, 1.0F);
				}
			}
		}
	}
	else
	{
		if ((NULL == pBaseSkeleton) || (pBaseSkeleton == pSrcInst))
		{
			for (int i = 1; i < bone_num; ++i)
			{
				if (pBoneDisable[i])
				{
					// 被屏蔽的节点
					continue;
				}

				src_bone = src_bone_list[i];
				dst_bone = dst_bone_list[i];
				//dst_bone->uNodeInfo |= src_bone->uNodeInfo;

				if (src_bone->bActive)
				{
					get_bone_node_location2(pSrcInst, src_bone, nFrame2, 
						nFrameNext2, nTranslate, fScale, &Bone2S, &Bone2E);
					math_location_lerp(&Bone2, &Bone2S, &Bone2E, fFactor2);

					if (dst_bone->bActive)
					{
						math_location_lerp(&dst_bone->pLocation[0], 
							&dst_bone->pLocation[0], &Bone2, fBlendWeight);
					}
					else
					{
						//dst_bone->bActive = true;
						dst_bone->pLocation[0] = Bone2;
					}
				}
				else if (dst_bone->bActive)
				{
					// 保持原来的形态
				}
				else
				{
					location_t& loc = dst_bone->pLocation[0];

					loc.position = FmVec3(0.0F, 0.0F, 0.0F);
					loc.rotate = FmQuat(0.0F, 0.0F, 0.0F, 1.0F);
					loc.scale = FmVec3(1.0F, 1.0F, 1.0F);
				}
			}
		}
		else
		{
			for (int i = 1; i < bone_num; ++i)
			{
				if (pBoneDisable[i])
				{
					// 被屏蔽的节点
					continue;
				}

				src_bone = src_bone_list[i];
				dst_bone = dst_bone_list[i];
				//dst_bone->uNodeInfo |= src_bone->uNodeInfo;

				if (src_bone->bActive)
				{
					get_bone_node_location_base2(pSrcInst, src_bone, nFrame2, 
						nFrameNext2, pBaseSkeleton, nTranslate, fScale, 
						&Bone2S, &Bone2E);
					math_location_lerp(&Bone2, &Bone2S, &Bone2E, fFactor2);

					if (dst_bone->bActive)
					{
						math_location_lerp(&dst_bone->pLocation[0], 
							&dst_bone->pLocation[0], &Bone2, fBlendWeight);
					}
					else
					{
						//dst_bone->bActive = true;
						dst_bone->pLocation[0] = Bone2;
					}
				}
				else if (dst_bone->bActive)
				{
					// 保持原来的形态
				}
				else
				{
					location_t& loc = dst_bone->pLocation[0];

					loc.position = FmVec3(0.0F, 0.0F, 0.0F);
					loc.rotate = FmQuat(0.0F, 0.0F, 0.0F, 1.0F);
					loc.scale = FmVec3(1.0F, 1.0F, 1.0F);
				}
			}
		}
	}

	return true;
}

skt_node_t* get_skeleton_node_from_name(skeleton_t* pInst, 
	const char* pszName)
{
	Assert(pInst != NULL);
	Assert(pszName != NULL);

	unsigned int node_num = pInst->nTotalNodeCount;

	if (0 == node_num)
	{
		return NULL;
	}

	unsigned int hash = GetHashValueCase(pszName);
	unsigned int bucket = hash % node_num;
	skt_node_t* node = pInst->pHashBuckets[bucket];

	while (node)
	{
		if ((node->nHash == hash)
			&& (strcmp(node->pszName, pszName) == 0))
		{
			return node;
		}

		node = node->pBucketNext;
	}

	return NULL;
}

skt_node_t* get_skeleton_bone_node_from_name(skeleton_t* pInst, 
	const char* pszName)
{
	Assert(pInst != NULL);
	Assert(pszName != NULL);

	unsigned int node_num = pInst->nTotalNodeCount;

	if (0 == node_num)
	{
		return NULL;
	}

	unsigned int hash = GetHashValueCase(pszName);
	unsigned int bucket = hash % node_num;
	skt_node_t* node = pInst->pHashBuckets[bucket];

	while (node)
	{
		if ((node->nType == FXNODE_TYPE_BONE)
			&& (node->nHash == hash)
			&& (strcmp(node->pszName, pszName) == 0))
		{
			return node;
		}

		node = node->pBucketNext;
	}

	return NULL;
}

void get_current_root_node_tm(skt_pose_t* ref, FmMat4* mtxTM)
{
	Assert(ref != NULL);
	Assert(mtxTM != NULL);

	if (NULL == ref->pCurrentNodeTMList)
	{
		FmMatrixIdentity(mtxTM);
		return;
	}

	skeleton_t* pInst = ref->pSkeleton;

	for (unsigned int i = 0; i < pInst->nRootNodeCount; ++i)
	{
		skt_node_t* node = &pInst->RootNodes[i];
		
		if (FXNODE_TYPE_BONE == node->nType)
		{
			*mtxTM = ref->pCurrentNodeTMList[node->nIndex];
			return;
		}
	}

	FmMatrixIdentity(mtxTM);
}

void get_root_node_tm_no_transform(skeleton_t* pInst, unsigned int nFrame, 
	int nTranslate, FmMat4* mtxTM)
{
	Assert(pInst != NULL);
	Assert(mtxTM != NULL);

	skt_node_t* node = pInst->RootNodes;

	for (unsigned int i = 0; i < pInst->nRootNodeCount; ++i)
	{
		if (FXNODE_TYPE_BONE == pInst->RootNodes[i].nType)
		{
			node = &pInst->RootNodes[i];
			break;
		}
	}

	unsigned int nFrameCnt = get_skeleton_frame_count(pInst);

	if (nFrame > (nFrameCnt + 3))
	{
		nFrame = nFrameCnt + 3;
	}

	// Get current node location.
	if ((node->pParent == NULL) && (nTranslate != 0))
	{
		// Get current node location.
		location_t loc;

		get_node_location_fast(pInst, node, nFrame, &loc);

		FmMat4 mtxTemp;

		math_location_to_matrix(&loc, &mtxTemp);
		FmMatrixIdentity(mtxTM);

		if (nTranslate == 1)
		{
			// 只有向前的位移
			mtxTM->_43 = mtxTemp._43;
		}
		else if (nTranslate == 3)
		{
			mtxTM->_41 = mtxTemp._41;
			mtxTM->_43 = mtxTemp._43;
		}
	}
	else
	{
		FmMatrixIdentity(mtxTM);
	}
}

void get_current_root_node_tm_no_transform(skeleton_t* pInst, 
	float fCurrentTime, float fSpeed, bool bLoop, int nTranslate, 
	FmMat4* mtxTM)
{
	Assert(pInst != NULL);
	Assert(mtxTM != NULL);

	skt_node_t* node = pInst->RootNodes;

	for (unsigned int i = 0; i < pInst->nRootNodeCount; ++i)
	{
		if (FXNODE_TYPE_BONE == pInst->RootNodes[i].nType)
		{
			node = &pInst->RootNodes[i];
			break;
		}
	}

	unsigned int nFrameCnt = get_skeleton_frame_count(pInst);
	unsigned int nFrame;
	unsigned int nFrameNext;
	float fFactor;

	if (nFrameCnt > 0)
	{
		float fFPS = fSpeed * pInst->fInternalFPS;

		math_get_frame_lerp(nFrameCnt, fFPS, fCurrentTime, bLoop, &nFrame, 
			&fFactor);
		nFrameNext = math_get_next_frame(nFrameCnt + 1, nFrame, bLoop);
		
		if ((nFrame > nFrameCnt) || (nFrameNext > nFrameCnt))
		{
			// 增加越界保护
			FmMatrixIdentity(mtxTM);
			return;
		}

		nFrame += 3;
		nFrameNext += 3;
	}
	else
	{
		nFrame = pInst->nEndFrame - pInst->nStartFrame;
		nFrameNext = nFrame;
		fFactor = 0.0F;
	}

	// Get current node location.
	location_t src;
	location_t dst;

	if ((node->pParent == NULL) && (nTranslate != 0))
	{
		// Get current node location.
		get_node_location_fast(pInst, node, nFrame, &src);
		get_node_location_fast(pInst, node, nFrameNext, &dst);

		// Lerp
		math_location_lerp(&src, &src, &dst, fFactor);

		FmMat4 mtxTemp;

		math_location_to_matrix(&src, &mtxTemp);
		FmMatrixIdentity(mtxTM);

		if (nTranslate == 1)
		{
			// 只有向前的位移
			mtxTM->_43 = mtxTemp._43;
		}
		else if (nTranslate == 3)
		{
			mtxTM->_41 = mtxTemp._41;
			mtxTM->_43 = mtxTemp._43;
		}
	}
	else
	{
		FmMatrixIdentity(mtxTM);
	}
}

void update_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
	FmMat4* pChangeNodeTMList, skt_pose_t* pParentRef)
{
	Assert(pInst != NULL);

	// 为了提高效率，使用循环替代递归操作，前提条件-父节点必须在子节点的前面
	location_t temp_loc;
	FmMat4 mtxTM;
	FmMat4 mtxTempTM;

	// 将部分逻辑判断拿到循环外面以提高效率
	if (NULL == pParentRef)
	{
		// 骨骼节点
		int bone_node_num = pInst->nTotalBoneNodeCount;
		skt_node_t** bone_node_list = pInst->pBoneNodeList;

		for (int k1 = 0; k1 < bone_node_num; ++k1)
		{
			skt_node_t* node = bone_node_list[k1];
			FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];
			
			// Calculate the transform matrix.
			math_location_to_matrix(&node->pLocation[0], &mtxTM);

			// Update node transform matrix to the reference.
			skt_node_t* parent_node = node->pParent;

			if (parent_node)
			{
				const FmMat4& mtxParentTM = 
					pCurrentNodeTMList[parent_node->nIndex];

				if (parent_node->nNodeInfo & SKELETON_SCALEINFO_FLAG)
				{
					// 父骨骼的缩放信息不向子骨骼传递
					//FmVec3 scaling;
					//FmQuat rotation;
					//FmVec3 translation;

					//D3DXMatrixDecompose(&scaling, &rotation, &translation,
					//	&mtxParentTM);

					//FmMat4 mtxParentNoScaling;

					//D3DXMatrixRotationQuaternion(&mtxParentNoScaling, 
					//	&rotation);

					//mtxParentNoScaling._41 = translation.x;
					//mtxParentNoScaling._42 = translation.y;
					//mtxParentNoScaling._43 = translation.z;

					FmMat4 mtxParentNoScaling = mtxParentTM;

					VisUtil_SetMatrixScale(&mtxParentNoScaling, 1.0F, 1.0F, 
						1.0F);

					FmVec3 pos;

					if (pChangeNodeTMList != NULL)
					{
						FmMatrixMultiply(&mtxTempTM, 
							&pChangeNodeTMList[node->nIndex], &mtxTM);

						pos.x = mtxTempTM._41;
						pos.y = mtxTempTM._42;
						pos.z = mtxTempTM._43;

						FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTempTM,
							&mtxParentNoScaling);
					}
					else
					{
						pos.x = mtxTM._41;
						pos.y = mtxTM._42;
						pos.z = mtxTM._43;

						FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
							&mtxParentNoScaling);
					}

					mtxNodeTM._41 = pos.x;
					mtxNodeTM._42 = pos.y;
					mtxNodeTM._43 = pos.z;
				}
				else
				{
					if (pChangeNodeTMList != NULL)
					{
						FmMatrixMultiply(&mtxTempTM, 
							&pChangeNodeTMList[node->nIndex], &mtxTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTempTM, 
							&mtxParentTM);
					}
					else
					{
						FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
							&mtxParentTM);
					}
				}
			}
			else
			{
				if (pChangeNodeTMList != NULL)
				{
					FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
						&pChangeNodeTMList[node->nIndex]);
				}
				else
				{
					mtxNodeTM = mtxTM;
				}
			}

			// Build cur_matrix variable for skeleton animation.
			FmMatrixMultiply(&node->mtxOriginal, &node->mtxInitInverse, 
				&mtxNodeTM);
			FmMatrixTranspose(&node->mtxCurrent, &node->mtxOriginal);
		}
		
		// 辅助节点
		int helper_node_num = pInst->nTotalHelperNodeCount;
		skt_node_t** helper_node_list = pInst->pHelperNodeList;

		for (int k2 = 0; k2 < helper_node_num; ++k2)
		{
			skt_node_t* node = helper_node_list[k2];
			FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];
			
			// Calculate the transform matrix.
			math_location_to_matrix(&node->pLocation[0], &mtxTM);
			
			// Update node transform matrix to the reference.
			if (node->pParent)
			{
				FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
					&pCurrentNodeTMList[node->pParent->nIndex]);
			}
			else
			{
				mtxNodeTM = mtxTM;
			}
		}

		return;
	}

	int node_num = pInst->nTotalNodeCount;
	skt_node_t** node_list = pInst->pNodeList;

	for (int k = 0; k < node_num; ++k)
	{
		skt_node_t* node = node_list[k];
		FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];
		skeleton_t* pParentInst = pParentRef->pSkeleton;

		if (!node->bGetSuper)
		{
			node->pSuperNode = get_skeleton_node_from_name(pParentInst, 
				node->pszName);
			node->bGetSuper = true; 
		}

		skt_node_t* pParentNode = node->pSuperNode;

		if (pParentNode)
		{
			// Update node transform matrix to the reference.
			mtxNodeTM = pParentRef->pCurrentNodeTMList[pParentNode->nIndex];

			if (node->nType != FXNODE_TYPE_HELPER)
			{
				// Build cur_matrix variable for skeleton animation.
				node->mtxOriginal = pParentNode->mtxOriginal;
				node->mtxCurrent = pParentNode->mtxCurrent;
			}

			continue;
		}

		// Calculate the transform matrix.
		math_location_to_matrix(&node->pLocation[0], &mtxTM);

		if (node->nType == FXNODE_TYPE_BONE)
		{
			// Update node transform matrix to the reference.
			skt_node_t* parent_node = node->pParent;

			if (parent_node)
			{
				const FmMat4& mtxParentTM = 
					pCurrentNodeTMList[parent_node->nIndex];

				if (parent_node->nNodeInfo & SKELETON_SCALEINFO_FLAG)
				{
					// 父骨骼的缩放信息不向子骨骼传递
					//FmVec3 scaling;
					//FmQuat rotation;
					//FmVec3 translation;

					//D3DXMatrixDecompose(&scaling, &rotation, &translation,
					//	&mtxParentTM);

					//FmMat4 mtxParentNoScaling;

					//D3DXMatrixRotationQuaternion(&mtxParentNoScaling, 
					//	&rotation);

					//mtxParentNoScaling._41 = translation.x;
					//mtxParentNoScaling._42 = translation.y;
					//mtxParentNoScaling._43 = translation.z;

					FmMat4 mtxParentNoScaling = mtxParentTM;

					VisUtil_SetMatrixScale(&mtxParentNoScaling, 1.0F, 1.0F, 
						1.0F);

					FmVec3 pos;

					if (pChangeNodeTMList != NULL)
					{
						FmMatrixMultiply(&mtxTempTM, 
							&pChangeNodeTMList[node->nIndex], &mtxTM);

						pos.x = mtxTempTM._41;
						pos.y = mtxTempTM._42;
						pos.z = mtxTempTM._43;

						FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTempTM,
							&mtxParentNoScaling);
					}
					else
					{
						pos.x = mtxTM._41;
						pos.y = mtxTM._42;
						pos.z = mtxTM._43;

						FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
							&mtxParentNoScaling);
					}

					mtxNodeTM._41 = pos.x;
					mtxNodeTM._42 = pos.y;
					mtxNodeTM._43 = pos.z;
				}
				else
				{
					if (pChangeNodeTMList != NULL)
					{
						FmMatrixMultiply(&mtxTempTM, 
							&pChangeNodeTMList[node->nIndex], &mtxTM);
						FmMatrixMultiply(&mtxNodeTM, &mtxTempTM, 
							&mtxParentTM);
					}
					else
					{
						FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
							&mtxParentTM);
					}
				}
			}
			else
			{
				if (pChangeNodeTMList != NULL)
				{
					FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
						&pChangeNodeTMList[node->nIndex]);
				}
				else
				{
					mtxNodeTM = mtxTM;
				}
			}

			// Build cur_matrix variable for skeleton animation.
			FmMatrixMultiply(&node->mtxOriginal, &node->mtxInitInverse, 
				&mtxNodeTM);
			FmMatrixTranspose(&node->mtxCurrent, &node->mtxOriginal);
		}
		else if (node->nType == FXNODE_TYPE_HELPER)
		{
			// Update node transform matrix to the reference.
			if (node->pParent)
			{
				FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
					&pCurrentNodeTMList[node->pParent->nIndex]);
			}
			else
			{
				mtxNodeTM = mtxTM;
			}
		}
	}
}

// 更新骨骼矩阵
void update_ragdoll_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
    bool* pCurrentNodeUserResetList, skt_pose_t* pParentRef)
{
    Assert(pInst != NULL);

    // 为了提高效率，使用循环替代递归操作，前提条件-父节点必须在子节点的前面
    location_t temp_loc;
    FmMat4 mtxTM;
    FmMat4 mtxTempTM;

    // 将部分逻辑判断拿到循环外面以提高效率
    if (NULL == pParentRef)
    {
        // 骨骼节点
        int bone_node_num = pInst->nTotalBoneNodeCount;
        skt_node_t** bone_node_list = pInst->pBoneNodeList;

        for (int k1 = 0; k1 < bone_node_num; ++k1)
        {
            skt_node_t* node = bone_node_list[k1];

            FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];

            if (!pCurrentNodeUserResetList[node->nIndex])
            {
                // Calculate the transform matrix.
                math_location_to_matrix(&node->pLocation[0], &mtxTM);

                // Update node transform matrix to the reference.
                skt_node_t* parent_node = node->pParent;

                if (parent_node)
                {
                    const FmMat4& mtxParentTM = 
                        pCurrentNodeTMList[parent_node->nIndex];

                    if (parent_node->nNodeInfo & SKELETON_SCALEINFO_FLAG)
                    {
                        FmMat4 mtxParentNoScaling = mtxParentTM;

                        VisUtil_SetMatrixScale(&mtxParentNoScaling, 1.0F, 1.0F, 
                            1.0F);

                        FmVec3 pos;
                        pos.x = mtxTM._41;
                        pos.y = mtxTM._42;
                        pos.z = mtxTM._43;

                        FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
                        FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                            &mtxParentNoScaling);

                        mtxNodeTM._41 = pos.x;
                        mtxNodeTM._42 = pos.y;
                        mtxNodeTM._43 = pos.z;
                    }
                    else
                    {
                        FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                            &mtxParentTM);
                    }
                }
                else
                {
                    mtxNodeTM = mtxTM;
                }
            }

            // Build cur_matrix variable for skeleton animation.
            FmMatrixMultiply(&node->mtxOriginal, &node->mtxInitInverse, 
                &mtxNodeTM);
            FmMatrixTranspose(&node->mtxCurrent, &node->mtxOriginal);
        }

        // 辅助节点
        int helper_node_num = pInst->nTotalHelperNodeCount;
        skt_node_t** helper_node_list = pInst->pHelperNodeList;

        for (int k2 = 0; k2 < helper_node_num; ++k2)
        {
            skt_node_t* node = helper_node_list[k2];
            FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];

            // Calculate the transform matrix.
            math_location_to_matrix(&node->pLocation[0], &mtxTM);

            // Update node transform matrix to the reference.
            if (node->pParent)
            {
                FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                    &pCurrentNodeTMList[node->pParent->nIndex]);
            }
            else
            {
                mtxNodeTM = mtxTM;
            }
        }

        return;
    }

    int node_num = pInst->nTotalNodeCount;
    skt_node_t** node_list = pInst->pNodeList;

    for (int k = 0; k < node_num; ++k)
    {
        skt_node_t* node = node_list[k];
        FmMat4& mtxNodeTM = pCurrentNodeTMList[node->nIndex];
        skeleton_t* pParentInst = pParentRef->pSkeleton;

        if (!node->bGetSuper)
        {
            node->pSuperNode = get_skeleton_node_from_name(pParentInst, 
                node->pszName);
            node->bGetSuper = true; 
        }

        skt_node_t* pParentNode = node->pSuperNode;

        if (pParentNode)
        {
            // Update node transform matrix to the reference.
            mtxNodeTM = pParentRef->pCurrentNodeTMList[pParentNode->nIndex];

            if (node->nType != FXNODE_TYPE_HELPER)
            {
                // Build cur_matrix variable for skeleton animation.
                node->mtxOriginal = pParentNode->mtxOriginal;
                node->mtxCurrent = pParentNode->mtxCurrent;
            }

            continue;
        }

        // Calculate the transform matrix.
        math_location_to_matrix(&node->pLocation[0], &mtxTM);

        if (node->nType == FXNODE_TYPE_BONE)
        {
            if (!pCurrentNodeUserResetList[node->nIndex])
            {
                // Update node transform matrix to the reference.
                skt_node_t* parent_node = node->pParent;

                if (parent_node)
                {
                    const FmMat4& mtxParentTM = 
                        pCurrentNodeTMList[parent_node->nIndex];

                    if (parent_node->nNodeInfo & SKELETON_SCALEINFO_FLAG)
                    {
                        FmMat4 mtxParentNoScaling = mtxParentTM;

                        VisUtil_SetMatrixScale(&mtxParentNoScaling, 1.0F, 1.0F, 
                            1.0F);

                        FmVec3 pos;

                        pos.x = mtxTM._41;
                        pos.y = mtxTM._42;
                        pos.z = mtxTM._43;

                        FmVec3TransformCoord(&pos, &pos, &mtxParentTM);
                        FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                            &mtxParentNoScaling);

                        mtxNodeTM._41 = pos.x;
                        mtxNodeTM._42 = pos.y;
                        mtxNodeTM._43 = pos.z;
                    }
                    else
                    {
                        FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                            &mtxParentTM);
                    }
                }
                else
                {
                    mtxNodeTM = mtxTM;
                }
            }

            // Build cur_matrix variable for skeleton animation.
            FmMatrixMultiply(&node->mtxOriginal, &node->mtxInitInverse, 
                &mtxNodeTM);
            FmMatrixTranspose(&node->mtxCurrent, &node->mtxOriginal);
        }
        else if (node->nType == FXNODE_TYPE_HELPER)
        {
            // Update node transform matrix to the reference.
            if (node->pParent)
            {
                FmMatrixMultiply(&mtxNodeTM, &mtxTM, 
                    &pCurrentNodeTMList[node->pParent->nIndex]);
            }
            else
            {
                mtxNodeTM = mtxTM;
            }
        }
    }
}

// 比较骨骼结构是否一致
bool compare_skeleton_bone_tree(skeleton_t* dst, skeleton_t* src)
{
	if (dst->nTotalBoneNodeCount != src->nTotalBoneNodeCount)
	{
		return false;
	}

	for (unsigned int i = 0; i < dst->nTotalBoneNodeCount; ++i)
	{
		skt_node_t* pNodeSrc = src->pBoneNodeList[i];
		skt_node_t* pNodeDst = dst->pBoneNodeList[i];

		if (strcmp(pNodeSrc->pszName, pNodeDst->pszName) != 0)
		{
			//FXLOG_ERROR("[error 030043]FXBoneTreeCompare[%s] mapping is[%s]!\n", 
			//	pNodeSrc->pszName, pNodeDst->pszName);
			return false;
		}

		skt_node_t* pParentSrc = pNodeSrc->pParent;
		skt_node_t* pParentDst = pNodeDst->pParent;

		if (pParentDst == NULL && pParentSrc != NULL)
		{
			//FXLOG_ERROR("[error 030043]FXBoneTreeCompare[%s] mapping is[%s]!\n", 
			//	pNodeSrc->pszName, pNodeDst->pszName);
			return false;
		}

		if (pParentDst != NULL && pParentSrc == NULL)
		{
			//FXLOG_ERROR("[error 030043]FXBoneTreeCompare[%s] mapping is[%s]!\n", 
			//	pNodeSrc->pszName, pNodeDst->pszName);
			return false;
		}

		if (pParentDst != NULL && pParentSrc != NULL)
		{
			if (strcmp(pParentDst->pszName, pParentSrc->pszName) != 0)
			{
				//FXLOG_ERROR("[error 030043]FXBoneTreeCompare[%s] mapping is[%s]!\n", 
				//	pNodeSrc->pszName, pNodeDst->pszName);
				return false;
			}
		}
	}

	return true;
}

static void open_bone_node(skt_node_t* pNode, skt_node_t* pOpenNode, 
	int disable, int* pBoneDisable)
{
	if (pNode->nType != FXNODE_TYPE_BONE)
	{
		return;
	}
	
	if (pNode == pOpenNode)
	{
		disable = 0;
	}

	pBoneDisable[pNode->nBoneId] = disable;
	
	for (size_t i = 0; i < pNode->nChildNodeCount; ++i)
	{
		open_bone_node(&pNode->ChildNodes[i], pOpenNode, disable, pBoneDisable);
	}
}

bool open_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pOpenNode,
	int* pBoneDisable)
{
	for (size_t i = 0; i < pInst->nRootNodeCount; ++i)
	{
		open_bone_node(&pInst->RootNodes[i], pOpenNode, 1, pBoneDisable);
	}
	
	return true;
}

static void close_bone_node(skt_node_t* pNode, skt_node_t* pCloseNode, 
	int disable, int* pBoneDisable)
{
	if (pNode->nType != FXNODE_TYPE_BONE)
	{
		return;
	}

	if (pNode == pCloseNode)
	{
		disable = 1;
	}

	pBoneDisable[pNode->nBoneId] = disable;

	for (size_t i = 0; i < pNode->nChildNodeCount; ++i)
	{
		close_bone_node(&pNode->ChildNodes[i], pCloseNode, disable, pBoneDisable);
	}
}

bool close_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pCloseNode,
	int* pBoneDisable)
{
	for (size_t i = 0; i < pInst->nRootNodeCount; ++i)
	{
		close_bone_node(&pInst->RootNodes[i], pCloseNode, 0, pBoneDisable);
	}

	return true;
}
