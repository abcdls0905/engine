//--------------------------------------------------------------------
// �ļ���:		visual_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��9��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_INFO_H
#define _VISUAL_INFO_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/persistid.h"
#include "../utils/string_pod.h"


#include "../visual/fm_header.h"

// ��ײѡ��
#define COLLIDE_WALKABLE 0x1
#define COLLIDE_GEN_WALL 0x2
#define COLLIDE_GEN_ROOF 0x4
#define COLLIDE_THROUGH 0x8

// ��������
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

// �ϲ�����ͼӳ��
struct atlas_map_t
{
	size_t nMaterialIndex; // ģ�Ͳ�������
	size_t nAtlasMapIndex; // �ϲ������ͼ������
	float fScaleU; // UV����
	float fScaleV;
	float fOffsetU; // UVƫ��
	float fOffsetV;
};

// ����������Ϣ
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
