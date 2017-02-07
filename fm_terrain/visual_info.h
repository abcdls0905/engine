//--------------------------------------------------------------------
// 文件名:		visual_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年9月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_INFO_H
#define _VISUAL_INFO_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/persistid.h"
#include "../utils/string_pod.h"


#include "../visual/fm_header.h"

// 碰撞选项
#define COLLIDE_WALKABLE 0x1
#define COLLIDE_GEN_WALL 0x2
#define COLLIDE_GEN_ROOF 0x4
#define COLLIDE_THROUGH 0x8

// 物体类型
enum VISUAL_TYTPE_ENUM
{
	VISUALTYPE_UNKNOWN,
	VISUALTYPE_MODEL,
	VISUALTYPE_PARTICLE,
	VISUALTYPE_LIGHT,
	VISUALTYPE_SOUND,
	VISUALTYPE_ACTOR,
	VISUALTYPE_DECAL,
	VISUALTYPE_TRIGGER,
	VISUALTYPE_EFFECTMODEL,
};

// 合并光贴图映射
struct atlas_map_t
{
	size_t nMaterialIndex; // 模型材质索引
	size_t nAtlasMapIndex; // 合并后光贴图的索引
	float fScaleU; // UV缩放
	float fScaleV;
	float fOffsetU; // UV偏移
	float fOffsetV;
};

// 场景物体信息
struct visual_info_t
{
	size_t nSize;
	int nType;
	int nState;
	int nZoneIndex;
	FmVec3 vPosition;
	PERSISTID VisualID;
	bool bInView;
	const char* pName;
	const char* pConfig;
	unsigned int nHash;
	int nNext;
	float fClipRadius;
	unsigned int nCollideInfo;
	char pszDesc[32];
	size_t nAtlasMapNum;
	atlas_map_t* pAtlasMaps;
	size_t nAttrNum;
	const char** pAttrKeys;
	const char** pAttrValues;
	// key pointers...
	// value pointers...
	// strings...
};

#endif // _VISUAL_INFO_H
