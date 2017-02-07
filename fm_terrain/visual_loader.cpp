//--------------------------------------------------------------------
// 文件名:		visual_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "visual_loader.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_visuals.h"
#include "terrain_grass.h"
#include "terrain_chunk.h"
#include "zone_manager.h"
#include "visual_manager.h"
#include "grass_type.h"
#include "quad_visual.h"
#include "block_file.h"
#include "block_grass.h"
#include "../utils/stream_writer.h"
#include "../visual/vis_utils.h"
#include "../visual/i_vis_base.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#define RAPIDXML_NO_STREAMS
#include "../extern/rapidxml/rapidxml.hpp"
#include "../extern/rapidxml/rapidxml_print.hpp"

#define BLOCK_ID_GRASS_0 '0SRG'

using namespace rapidxml;

// CVisualLoader

CVisualLoader* CVisualLoader::NewInstance()
{
	return CORE_NEW(CVisualLoader);
}

IBlockReader* CVisualLoader::CreateBlockReader(int block_id, void* context)
{
	if (block_id == BLOCK_ID_GRASS_0)
	{
		CVisualLoader* pthis = (CVisualLoader*)context;

		return CBlockGrassReader::NewInstance(&pthis->m_Grass);
	}

	return NULL;
}

CVisualLoader::CVisualLoader()
{
	m_pOwner = NULL;
	m_bDesignMode = false;
	m_Grass.nGrassTypeNum = 0;
	m_Grass.nGrassDataNum = 0;
	m_Grass.pGrassData = NULL;
	m_pHashIndex = NULL;
	m_nHashSize = 0;
}

CVisualLoader::~CVisualLoader()
{
	if (m_Grass.pGrassData)
	{
		CORE_FREE(m_Grass.pGrassData, 
			sizeof(grass_info_t) * m_Grass.nGrassDataNum);
	}

	if (m_pHashIndex)
	{
		CORE_FREE(m_pHashIndex, sizeof(int) * m_nHashSize);
	}

	size_t visual_num = m_Visuals.size();
	
	if (visual_num > 0)
	{
		for (size_t i = 0; i < visual_num; ++i)
		{
			visual_info_t* pInfo = m_Visuals[i];
			
			CORE_FREE(pInfo, pInfo->nSize);
		}
	}
}

void CVisualLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CVisualLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	m_pOwner = pOwner;
	m_bDesignMode = pOwner->GetTerrain()->GetDesignMode();
}

void CVisualLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

void CVisualLoader::SetWidgetFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strWidgetFile = file_name;
}

void CVisualLoader::SetHelperFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strHelperFile = file_name;
}

void CVisualLoader::SetCollideFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strCollideFile = file_name;
}

void CVisualLoader::SetDescFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strDescFile = file_name;
}

void CVisualLoader::SetAtlasFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strAtlasFile = file_name;
}

void CVisualLoader::SetSoundFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strSoundFile = file_name;
}

void CVisualLoader::SetGrassFile(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strGrassFile = file_name;
}

static int get_visual_type(const char* type)
{
	Assert(type != NULL);

	if (stricmp(type, "model") == 0)
	{
		return VISUALTYPE_MODEL;
	}
	else if (stricmp(type, "particle") == 0)
	{
		return VISUALTYPE_PARTICLE;
	}
	else if (stricmp(type, "light") == 0)
	{
		return VISUALTYPE_LIGHT;
	}
	else if (stricmp(type, "sound") == 0)
	{
		return VISUALTYPE_SOUND;
	}
	else if (stricmp(type, "actor") == 0)
	{
		return VISUALTYPE_ACTOR;
	}
	else if (stricmp(type, "decal") == 0)
	{
		return VISUALTYPE_DECAL;
	}
	else if (stricmp(type, "trigger") == 0)
	{
		return VISUALTYPE_TRIGGER;
	}
	else if (stricmp(type, "effectmodel") == 0)
	{
		return VISUALTYPE_EFFECTMODEL;
	}
	else
	{
		return VISUALTYPE_UNKNOWN;
	}
}

bool CVisualLoader::LoadVisuals(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "rb");
	if (NULL == fp)
	{
		// 无需加载
		return true;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		return false;
  }

	core_file::fclose(fp);
	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<0>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CVisualLoader::LoadVisuals)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* root = doc.first_node("visuals");

	if (NULL == root)
	{
		CORE_TRACE("(CVisualLoader::LoadVisuals)no root element");
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* element = root->first_node();

	while (element)
	{
		if (!m_bDesignMode)
		{
			xml_attribute<>* only_design_attr = 
				element->first_attribute("only_design");

			if (only_design_attr)
			{
				const char* only_design = only_design_attr->value();

				if (stricmp(only_design, "true") == 0)
				{
					// 不加载辅助物体
					element = element->next_sibling();
					continue;
				}
			}
		}

		// 物体类型
		const char* type_value = element->name();

		if (NULL == type_value)
		{
			CORE_TRACE("(CVisualLoader::LoadVisuals)element no type");
			CORE_TRACE(file_name);
			return false;
		}

		int type = get_visual_type(type_value);

		if (type == VISUALTYPE_UNKNOWN)
		{
			CORE_TRACE("(CVisualLoader::LoadVisuals)type error");
			CORE_TRACE(type_value);
			CORE_TRACE(file_name);
			return false;
		}

		// 物体名字
		xml_attribute<>* name_attr = element->first_attribute("name");

		if (NULL == name_attr)
		{
			CORE_TRACE("(CVisualLoader::LoadVisuals)element no name");
			CORE_TRACE(file_name);
			return false;
		}

		if (StringEmpty(name_attr->name()))
		{
			CORE_TRACE("(CVisualLoader::LoadVisuals)name empty");
			CORE_TRACE(file_name);
			return false;
		}

		xml_attribute<>* attr = element->first_attribute();
		size_t attr_num = 0;
		size_t string_size = 0;

		while (attr)
		{
			const char* attr_name = attr->name();

			if (stricmp(attr_name, "pos") == 0)
			{
			}
			else if (stricmp(attr_name, "clip_radius") == 0)
			{
			}
			else
			{
				attr_num++;
				string_size += strlen(attr_name) + 1;
				string_size += strlen(attr->value()) + 1;
			}
			
			attr = attr->next_attribute();
		}
		
		size_t info_size = sizeof(visual_info_t) 
			+ attr_num * 2 * sizeof(const char*) + string_size; 
		visual_info_t* pInfo = (visual_info_t*)CORE_ALLOC(info_size);

		memset(pInfo, 0, info_size);
		pInfo->nSize = info_size;
		pInfo->nType = type;
		pInfo->nNext = -1;
		pInfo->pName = "";
		pInfo->pConfig = "";
		pInfo->nAttrNum = attr_num;
		pInfo->pAttrKeys = (const char**)((char*)pInfo + sizeof(visual_info_t));
		pInfo->pAttrValues = pInfo->pAttrKeys + attr_num;

		char* pStrings = (char*)(pInfo->pAttrValues + attr_num);

		attr_num = 0;
		string_size = 0;
		attr = element->first_attribute();

		while (attr)
		{
			const char* attr_name = attr->name();
			const char* attr_value = attr->value();

			if (stricmp(attr_name, "pos") == 0)
			{
				// 位置
				pInfo->vPosition = VisUtil_StringToVector(attr_value);
			}
			else if (stricmp(attr_name, "clip_radius") == 0)
			{
				// 剪裁距离
				pInfo->fClipRadius = (float)atof(attr_value);
			}
			else
			{
				size_t key_size = strlen(attr_name) + 1;
				size_t value_size = strlen(attr_value) + 1;

				memcpy(pStrings, attr_name, key_size);
				pInfo->pAttrKeys[attr_num] = pStrings;
				pStrings += key_size;
				memcpy(pStrings, attr_value, value_size);
				pInfo->pAttrValues[attr_num] = pStrings;

				if (stricmp(attr_name, "name") == 0)
				{
					pInfo->pName = pStrings;
					pInfo->nHash = GetHashValueCase(pStrings);
				}
				else if (stricmp(attr_name, "config") == 0 
					|| stricmp(attr_name, "effectmodel_config") == 0)
				{
					pInfo->pConfig = pStrings;
				}

				pStrings += value_size;
				attr_num++;
			}

			attr = attr->next_attribute();
		}

		m_Visuals.push_back(pInfo);

		element = element->next_sibling();
	}

	return true;
}

bool CVisualLoader::LoadCollideInfo(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		// 无需加载
		return true;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);
	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<0>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CVisualLoader::LoadCollideInfo)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* root = doc.first_node("visuals");

	if (NULL == root)
	{
		CORE_TRACE("(CVisualLoader::LoadCollideInfo)no root element");
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* element = root->first_node();

	while (element)
	{
		const char* type = element->name();

		if (NULL == type)
		{
			CORE_TRACE("(CVisualLoader::LoadCollideInfo)element no type");
			CORE_TRACE(file_name);
			return false;
		}

		xml_attribute<>* name_attr = element->first_attribute("name");

		if (NULL == name_attr)
		{
			CORE_TRACE("(CVisualLoader::LoadCollideInfo)element no name");
			CORE_TRACE(file_name);
			return false;
		}

		const char* name = name_attr->value();
		size_t index;

		if (FindVisualIndex(name, index))
		{
			unsigned int collide_info = 0;
			xml_attribute<>* attr = element->first_attribute();

			while (attr)
			{
				if (strcmp(attr->name(), "co_walkable") == 0)
				{
					collide_info |= COLLIDE_WALKABLE;
				}
				else if (strcmp(attr->name(), "co_gen_wall") == 0)
				{
					collide_info |= COLLIDE_GEN_WALL;
				}
				else if (strcmp(attr->name(), "co_gen_roof") == 0)
				{
					collide_info |= COLLIDE_GEN_ROOF;
				}
				else if (strcmp(attr->name(), "co_through") == 0)
				{
					collide_info |= COLLIDE_THROUGH;
				}

				attr = attr->next_attribute();
			}

			m_Visuals[index]->nCollideInfo = collide_info;
		}
		else
		{
			CORE_TRACE("(CVisualLoader::LoadCollideInfo)visual not found");
			CORE_TRACE(name);
			CORE_TRACE(file_name);
		}

		element = element->next_sibling();
	}

	return true;
}

bool CVisualLoader::LoadDesc(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = core_file::fopen(file_name, "rb");

	if (NULL == fp)
	{
		// 无需加载
		return true;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256, TCoreAlloc> auto_buf(file_size + 1);
	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);
	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<0>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(CVisualLoader::LoadDesc)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* root = doc.first_node("visuals");

	if (NULL == root)
	{
		CORE_TRACE("(CVisualLoader::LoadDesc)no root element");
		CORE_TRACE(file_name);
		return false;
	}

	xml_node<>* element = root->first_node();

	while (element)
	{
		const char* type = element->name();

		if (NULL == type)
		{
			CORE_TRACE("(CVisualLoader::LoadDesc)element no type");
			CORE_TRACE(file_name);
			return false;
		}

		xml_attribute<>* name_attr = element->first_attribute("name");

		if (NULL == name_attr)
		{
			CORE_TRACE("(CVisualLoader::LoadDesc)element no name");
			CORE_TRACE(file_name);
			return false;
		}

		const char* name = name_attr->value();
		size_t index;

		if (FindVisualIndex(name, index))
		{
			xml_attribute<>* attr = element->first_attribute();

			while (attr)
			{
				if (strcmp(attr->name(), "desc") == 0)
				{
					const char* attr_value = attr->value();
					size_t value_size = strlen(attr_value) + 1;

					CopyString(m_Visuals[index]->pszDesc, value_size, attr_value);
				}

				attr = attr->next_attribute();
			}
		}
		else
		{
			CORE_TRACE("(CVisualLoader::LoadDesc)visual not found");
			CORE_TRACE(name);
			CORE_TRACE(file_name);
		}

		element = element->next_sibling();
	}

	return true;
}

bool CVisualLoader::LoadAtlas(const char* file_name)
{
	Assert(file_name != NULL);

	IIniFile* pIni = g_pCore->CreateIniFile(file_name);

	if (!pIni->LoadFromFile())
	{
		pIni->Release();
		return false;
	}

	size_t sect_num = pIni->GetSectionCount();

	for (size_t i = 0; i < sect_num; ++i)
	{
		const char* sect = pIni->GetSectionByIndex(i);
		size_t index;

		if (!FindVisualIndex(sect, index))
		{
			continue;
		}

		size_t item_num = pIni->GetSectionItemCount(i);
		TAutoMem<atlas_map_t, 16, TCoreAlloc> auto_buf(item_num);
		atlas_map_t* pAtlasMaps = auto_buf.GetBuffer();
		size_t atlas_map_num = 0;

		for (size_t k = 0; k < item_num; ++k)
		{
			const char* key = pIni->GetSectionItemKey(i, k);
			const char* val = pIni->GetSectionItemValue(i, k);
			size_t material_index = atoi(key);
			size_t atlas_map_index = atoi(val);
			const char* split = strchr(val, ',');

			if (NULL == split)
			{
				continue;
			}

			float scale_u = (float)atof(split + 1);

			split = strchr(split + 1, ',');

			if (NULL == split)
			{
				continue;
			}

			float scale_v = (float)atof(split + 1);
			
			split = strchr(split + 1, ',');

			if (NULL == split)
			{
				continue;
			}

			float offset_u = (float)atof(split + 1);

			split = strchr(split + 1, ',');

			if (NULL == split)
			{
				continue;
			}

			float offset_v = (float)atof(split + 1);

			pAtlasMaps[atlas_map_num].nMaterialIndex = material_index;
			pAtlasMaps[atlas_map_num].nAtlasMapIndex = atlas_map_index;
			pAtlasMaps[atlas_map_num].fScaleU = scale_u;
			pAtlasMaps[atlas_map_num].fScaleV = scale_v;
			pAtlasMaps[atlas_map_num].fOffsetU = offset_u;
			pAtlasMaps[atlas_map_num].fOffsetV = offset_v;
			atlas_map_num++;
		}

		visual_info_t* pOldInfo = m_Visuals[index];
		size_t new_info_size = pOldInfo->nSize 
			+ sizeof(atlas_map_t) * atlas_map_num;
		visual_info_t* pNewInfo = (visual_info_t*)CORE_ALLOC(new_info_size);

		memcpy(pNewInfo, pOldInfo, pOldInfo->nSize);
		pNewInfo->nSize = new_info_size;
		pNewInfo->nAtlasMapNum = atlas_map_num;
		pNewInfo->pAtlasMaps = (atlas_map_t*)
			((char*)pNewInfo + pOldInfo->nSize);
		memcpy(pNewInfo->pAtlasMaps, pAtlasMaps, 
			sizeof(atlas_map_t) * atlas_map_num);
		
		// 调整指针的位置
		size_t name_offset = pOldInfo->pName - (char*)pOldInfo;
		size_t config_offset = pOldInfo->pConfig - (char*)pOldInfo;

		if (name_offset < pOldInfo->nSize)
		{
			pNewInfo->pName = (char*)pNewInfo + name_offset;
		}

		if (config_offset < pOldInfo->nSize)
		{
			pNewInfo->pConfig = (char*)pNewInfo + config_offset;
		}

		pNewInfo->pAttrKeys = (const char**)((char*)pNewInfo 
			+ ((char*)pOldInfo->pAttrKeys - (char*)pOldInfo));
		pNewInfo->pAttrValues = (const char**)((char*)pNewInfo 
			+ ((char*)pOldInfo->pAttrValues - (char*)pOldInfo));

		for (size_t j = 0; j < pNewInfo->nAttrNum; ++j)
		{
			size_t key_offset = pOldInfo->pAttrKeys[j] - (char*)pOldInfo;
			size_t val_offset = pOldInfo->pAttrValues[j] - (char*)pOldInfo;

			pNewInfo->pAttrKeys[j] = (char*)pNewInfo + key_offset;
			pNewInfo->pAttrValues[j] = (char*)pNewInfo + val_offset;
		}

		CORE_FREE(pOldInfo, pOldInfo->nSize);
		m_Visuals[index] = pNewInfo;
	}

	pIni->Release();

	return true;
}

bool CVisualLoader::LoadBlockGrass(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);

	return block_file.Load(file_name);
}

void CVisualLoader::CreateHashIndex()
{
	size_t visual_num = m_Visuals.size();

	m_nHashSize = visual_num + 1;
	m_pHashIndex = (int*)CORE_ALLOC(sizeof(int) * m_nHashSize);
	memset(m_pHashIndex, 0xFF, sizeof(int) * m_nHashSize);

	for (size_t k = 0; k < visual_num; ++k)
	{
		visual_info_t* pInfo = m_Visuals[k];
		size_t bucket = pInfo->nHash % m_nHashSize;

		pInfo->nNext = m_pHashIndex[bucket];
		m_pHashIndex[bucket] = (int)k;
	}
}

bool CVisualLoader::FindVisualIndex(const char* name, size_t& index)
{
	Assert(name != NULL);

	if (NULL == m_pHashIndex)
	{
		return false;
	}
	
	unsigned int hash = GetHashValueCase(name);
	size_t bucket = hash % m_nHashSize;
	int k = m_pHashIndex[bucket];

	while (k >= 0)
	{
		visual_info_t* pInfo = m_Visuals[k];

		if ((pInfo->nHash == hash) && (strcmp(pInfo->pName, name) == 0))
		{
			index = k;
			return true;
		}
		
		k = pInfo->nNext;
	}

	return false;
//	unsigned int hash = GetHashValueCase(name);
//	size_t visual_num = m_Visuals.size();
//
//	for (size_t i = 0; i < visual_num; ++i)
//	{
//		visual_info_t* p = m_Visuals[i];
//
//		if ((p->nHash == hash) && (strcmp(p->pName, name) == 0))
//		{
//			index = i;
//			return true;
//		}
//	}
//
//	return false;
}

const char* CVisualLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CVisualLoader::Load(bool async)
{
	if (!m_strFileName.empty())
	{
		LoadVisuals(m_strFileName.c_str());
	}

	if (!m_strWidgetFile.empty())
	{
		LoadVisuals(m_strWidgetFile.c_str());
	}

	if (!m_strHelperFile.empty())
	{
		LoadVisuals(m_strHelperFile.c_str());
	}

	if (!m_strSoundFile.empty())
	{
		LoadVisuals(m_strSoundFile.c_str());
	}

	if (!m_Visuals.empty())
	{
		CreateHashIndex();
	}

	if (!m_strCollideFile.empty())
	{
		LoadCollideInfo(m_strCollideFile.c_str());
	}

	if (!m_strDescFile.empty())
	{
		LoadDesc(m_strDescFile.c_str());
	}

	if (!m_strAtlasFile.empty())
	{
		LoadAtlas(m_strAtlasFile.c_str());
	}

	if (!m_strGrassFile.empty())
	{
		LoadBlockGrass(m_strGrassFile.c_str());
	}

	return true;
}

bool CVisualLoader::Create(bool async)
{
	Terrain* pTerrain = m_pOwner->GetTerrain();

	if (pTerrain->GetRender() == NULL)
	{
		// 地形对象已经被释放
		return false;
	}

	if (!m_Visuals.empty())
	{
		CVisualManager* pVisualManager = pTerrain->GetVisualManager();
		int zone_index = (int)m_pOwner->GetIndex();
		const size_t visual_num = m_Visuals.size();

		for (size_t i = 0; i < visual_num; ++i)
		{
			pVisualManager->AddLoadVisual(zone_index, m_Visuals[i]);
		}

		// 数据所有权已经转移
		m_Visuals.clear();
	}

	if (m_Grass.nGrassDataNum > 0)
	{
		CTerrainGrass* pGrass = m_pOwner->GetTerrain()->GetGrass();
		// 获得花草的索引值
		int grass_index[256];

		for (size_t i = 0; i < m_Grass.nGrassTypeNum; ++i)
		{
			const char* grass_name = m_Grass.GrassNames.GetByIndex(i);
			size_t index;

			if (pGrass->FindIndex(grass_name, index))
			{
				grass_index[i] = (int)index;
			}
			else
			{
				grass_index[i] = -1;
				CORE_TRACE("(CVisualLoader::Create)grass not found");
				CORE_TRACE(grass_name);
				CORE_TRACE(m_strGrassFile.c_str());
			}
		}

		for (size_t k = 0; k < m_Grass.nGrassDataNum; ++k)
		{
			int gi = grass_index[m_Grass.pGrassData[k].nGrassIndex];

			if (gi < 0)
			{
				continue;
			}

			float pos_x = m_Grass.pGrassData[k].nPosiX * 0.1F;
			float pos_z = m_Grass.pGrassData[k].nPosiZ * 0.1F;

			pos_x *= m_pOwner->GetTerrain()->GetUnitSize();
			pos_z *= m_pOwner->GetTerrain()->GetUnitSize();
			pos_x += m_pOwner->GetLeft();
			pos_z += m_pOwner->GetTop();

			CTerrainChunk* pChunk = m_pOwner->GetInChunk(pos_x, pos_z);

			if (NULL == pChunk)
			{
				CORE_TRACE("(CVisualLoader::Create)grass position error");
				CORE_TRACE(m_strGrassFile.c_str());
				continue;
			}

			grass_data_t data;

			data.fPosiX = pos_x;
			data.fPosiZ = pos_z;
			data.nGrassIndex = gi;
			data.nScale = (m_Grass.pGrassData[k].nScaleAmount >> 4) & 0xF;
			data.nAmount = m_Grass.pGrassData[k].nScaleAmount & 0xF;
			data.nSeed = m_Grass.pGrassData[k].nSeed;

			pChunk->AddGrass(data);

			// 激活这类草
			pGrass->GetByIndex(gi)->Activate();
		}
	}

	m_pOwner->SetVisualLoaded(true);

	return true;
}

bool CVisualLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

static const char* clear_append_path(const char* path, const char* append_path)
{
	if (path[0] == 0 || append_path[0] == 0)
	{
		return path;
	}

	size_t append_path_len = strlen(append_path);
	result_string sub = path;

	sub = sub.substr(0, append_path_len);

	if (sub == append_path)
	{
		return path + append_path_len;
	}

	return path;
}

static xml_node<>* xml_add_node(xml_document<>* doc, xml_node<>* node, 
	const char* name)
{
	char* node_name = doc->allocate_string(name);
	xml_node<>* new_node = doc->allocate_node(node_element, node_name);

	node->append_node(new_node);

	return new_node;
}

static void xml_add_attr(xml_document<>* doc, xml_node<>* node, 
	const char* name, const char* value)
{
	char* attr_name = doc->allocate_string(name);
	char* attr_value = doc->allocate_string(value);
	xml_attribute<>* new_attr = doc->allocate_attribute(attr_name, attr_value);

	node->append_attribute(new_attr);
}

static bool save_collide_info_and_desc(IVisBase* pVisBase, const char* name, 
	xml_document<>* doc, xml_node<>* collide_root, xml_document<>* desc_doc, 
	xml_node<>* desc_root)
{
	// 碰撞属性
	bool walkable = VisUtil_GetCustomBool(pVisBase, "co_walkable");
	bool gen_roof = VisUtil_GetCustomBool(pVisBase, "co_gen_roof");
	bool gen_wall = VisUtil_GetCustomBool(pVisBase, "co_gen_wall");
	bool through = VisUtil_GetCustomBool(pVisBase, "co_through");

	if (walkable || gen_roof || gen_wall || through)
	{
		xml_node<>* collide_element = xml_add_node(doc, collide_root, "model");

		xml_add_attr(doc, collide_element, "name", name);

		if (walkable)
		{
			xml_add_attr(doc, collide_element, "co_walkable", "true");
		}

		if (gen_roof)
		{
			xml_add_attr(doc, collide_element, "co_gen_roof", "true");
		}

		if (gen_wall)
		{
			xml_add_attr(doc, collide_element, "co_gen_wall", "true");
		}

		if (through)
		{
			xml_add_attr(doc, collide_element, "co_through", "true");
		}
	}

	result_string desc = VisUtil_GetCustomString(pVisBase, "desc");

	Assert(desc.length() < 32);

	if (!desc.empty() && (desc.length() < 32))
	{
		xml_node<>* desc_element = xml_add_node(desc_doc, desc_root, "model");

		xml_add_attr(desc_doc, desc_element, "name", name);
		xml_add_attr(desc_doc, desc_element, "desc", desc.c_str());
	}

	return true;
}

static bool save_model(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, 
	//xml_document<>* collide_doc, xml_node<>* collide_root, 
	//xml_document<>* desc_doc, xml_node<>* desc_root, 
	ICore* pCore, 
	CTerrainZone* pZone) 
{
	xml_node<>* element = xml_add_node(doc, root, "model");
	
	xml_add_attr(doc, element, "name", name);
	
	CVar prop;
	
	if (pCore->GetProperty(pVisBase, "ModelFile", prop))
	{
		xml_add_attr(doc, element, "model", clear_append_path(
			prop.StringVal(), pZone->GetTerrain()->GetAppendPath()));
	}
	
	if (pCore->GetProperty(pVisBase, "MaterialFile", prop))
	{
		if (!StringEmpty(prop.StringVal()))
		{
			xml_add_attr(doc, element, "material", clear_append_path(
				prop.StringVal(), pZone->GetTerrain()->GetAppendPath()));
		}
	}

	if (pCore->GetProperty(pVisBase, "RigidBodyFile", prop))
	{
		if (!StringEmpty(prop.StringVal()))
		{
			xml_add_attr(doc, element, "rigid_body_file", clear_append_path(
				prop.StringVal(), pZone->GetTerrain()->GetAppendPath()));
		}
	}

	FmVec3 pos = pVisBase->GetPosition();
	
	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();
	
	char buf[128];
	
	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);
	
	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}
	
	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}
	
	if (pCore->GetProperty(pVisBase, "UseVertexColor", prop))
	{
		if (!prop.BoolVal())
		{
			xml_add_attr(doc, element, "vertex_color", "false");
		}
	}

	if (pCore->GetProperty(pVisBase, "UseLightMap", prop))
	{
		if (prop.BoolVal())
		{
			xml_add_attr(doc, element, "light_map", "true");
		}
	}

	// 光贴图尺寸
	int light_map_size = VisUtil_GetCustomInt(pVisBase, "light_map_size");

	if (light_map_size > 0)
	{
		SafeSprintf(buf, sizeof(buf), "%d", light_map_size);
		xml_add_attr(doc, element, "light_map_size", buf);
	}

	// 剪裁距离
	float clip_radius = VisUtil_GetCustomFloat(pVisBase, "clip_radius");

	if (clip_radius > 0.0F)
	{
		SafeSprintf(buf, sizeof(buf), "%d", (int)clip_radius);
		xml_add_attr(doc, element, "clip_radius", buf);
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 辅助物体
	bool only_design = VisUtil_GetCustomBool(pVisBase, "only_design");

	if (only_design)
	{
		xml_add_attr(doc, element, "only_design", "true");
	}

	// 水面倒影
	bool water_reflect = pVisBase->GetWaterReflect();

	if (water_reflect)
	{
		xml_add_attr(doc, element, "water_reflect", "true");
	}

	// 无投影
	bool no_shadow = !(pVisBase->GetCastShadow());

	if (no_shadow)
	{
		xml_add_attr(doc, element, "no_shadow", "true");
	}

	// 接受投影
	bool receive_shadow = pVisBase->GetReceiveShadow();

	if (receive_shadow)
	{
		xml_add_attr(doc, element, "receive_shadow", "true");
	}

	// 附加信息
	if (pCore->GetProperty(pVisBase, "ExtraInfo", prop))
	{
		if (prop.IntVal())
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "extra_info", buf);
		}
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");

	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}

	return true;
}

static bool save_particle(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "particle");
	
	xml_add_attr(doc, element, "name", name);
	
	// 粒子名
	result_string par_name = VisUtil_GetCustomString(pVisBase, "par_name");
	
	xml_add_attr(doc, element, "par_name", par_name.c_str());
	
	FmVec3 pos = pVisBase->GetPosition();
	
	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();
	
	char buf[128];
	
	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);
	
	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}
	
	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}

	// 配置文件
	result_string config = VisUtil_GetCustomString(pVisBase, "config");
	
	xml_add_attr(doc, element, "config", clear_append_path(
		config.c_str(), pZone->GetTerrain()->GetAppendPath()));
	
	// 剪裁距离
	float clip_radius = VisUtil_GetCustomFloat(pVisBase, "clip_radius");

	if (clip_radius > 0.0F)
	{
		SafeSprintf(buf, sizeof(buf), "%d", (int)clip_radius);
		xml_add_attr(doc, element, "clip_radius", buf);
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");
	
	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}
	
	return true;
}

static bool save_light(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "light");
	
	xml_add_attr(doc, element, "name", name);
	
	CVar prop;
	
	if (pCore->GetProperty(pVisBase, "Color", prop))
	{
		xml_add_attr(doc, element, "color", prop.StringVal());
	}
	
	FmVec3 pos = pVisBase->GetPosition();
	
	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();
	
	char buf[128];
	
	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);
	
	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	if (pCore->GetProperty(pVisBase, "LightType", prop))
	{
		xml_add_attr(doc, element, "type", prop.StringVal());

		if (stricmp(prop.StringVal(), "box") == 0)
		{
			CVar size_x;
			CVar size_y;
			CVar size_z;

			pCore->GetProperty(pVisBase, "BoxScaleX", size_x);
			pCore->GetProperty(pVisBase, "BoxScaleY", size_y);
			pCore->GetProperty(pVisBase, "BoxScaleZ", size_z);
			
			SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", size_x.FloatVal(), 
				size_y.FloatVal(), size_z.FloatVal());
			xml_add_attr(doc, element, "size", buf);
		}
		else if (stricmp(prop.StringVal(), "spot") == 0)
		{
			if (pCore->GetProperty(pVisBase, "InnerDegree", prop))
			{
				if (!FloatEqual(prop.FloatVal(), 0.0F))
				{
					SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
					xml_add_attr(doc, element, "inner_degree", buf);
				}
			}

			if (pCore->GetProperty(pVisBase, "OuterDegree", prop))
			{
				if (!FloatEqual(prop.FloatVal(), 0.0F))
				{
					SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
					xml_add_attr(doc, element, "outer_degree", buf);
				}
			}

			if (pCore->GetProperty(pVisBase, "Falloff", prop))
			{
				if (!FloatEqual(prop.FloatVal(), 1.0F))
				{
					SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
					xml_add_attr(doc, element, "Falloff", buf);
				}
			}
		}
	}

	if (pCore->GetProperty(pVisBase, "Intensity", prop))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
		xml_add_attr(doc, element, "intensity", buf);
	}

	if (pCore->GetProperty(pVisBase, "Range", prop))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
		xml_add_attr(doc, element, "range", buf);
	}
	
	if (pCore->GetProperty(pVisBase, "Attenu0", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "att0", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "Attenu1", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "att1", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "Attenu2", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "att2", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "ShadowMapSize", prop))
	{
		if (prop.IntVal() != 0)
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "shadow_map_size", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "Blink", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "blink", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "BlinkPeriod", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "period", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "BlinkTick", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 0.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "tick", buf);
		}
	}
	
	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 是否只作为静态光源
	bool only_design = VisUtil_GetCustomBool(pVisBase, "only_design");
	
	if (only_design)
	{
		xml_add_attr(doc, element, "only_design", "true");
	}
	
	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");
	
	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}
	
	return true;
}

static bool save_sound(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "sound");
	
	xml_add_attr(doc, element, "name", name);
	
	CVar prop;
	
	if (pCore->GetProperty(pVisBase, "Name", prop))
	{
		xml_add_attr(doc, element, "sound", clear_append_path(
			prop.StringVal(), pZone->GetTerrain()->GetAppendPath()));
	}
	
	FmVec3 pos = pVisBase->GetPosition();
	
	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();
	
	char buf[128];
	
	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);
	
	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	if (pCore->GetProperty(pVisBase, "Volume", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 1.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "volume", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "MinInterval", prop))
	{
		if (prop.FloatVal() > 0.0F)
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "min_interval", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "MaxInterval", prop))
	{
		if (prop.FloatVal() > 0.0F)
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "max_interval", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "InDegree", prop))
	{
		if (prop.IntVal() > 0)
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "in_degree", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "OutDegree", prop))
	{
		if (prop.IntVal() > 0)
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "out_degree", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "OutVolume", prop))
	{
		if (prop.FloatVal() > 0.0F)
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "out_volume", buf);
		}
	}

	if (pCore->GetProperty(pVisBase, "MinDistance", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 1.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "min_dist", buf);
		}
	}
	
	if (pCore->GetProperty(pVisBase, "MaxDistance", prop))
	{
		if (!FloatEqual(prop.FloatVal(), 100.0F))
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", prop.FloatVal());
			xml_add_attr(doc, element, "max_dist", buf);
		}
	}
	
	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");
	
	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}
	
	return true;
}

static bool save_actor(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "actor");

	xml_add_attr(doc, element, "name", name);

	// 配置文件
	result_string config = VisUtil_GetCustomString(pVisBase, "config");

	xml_add_attr(doc, element, "config", clear_append_path(config.c_str(), 
		pZone->GetTerrain()->GetAppendPath()));

	FmVec3 pos = pVisBase->GetPosition();

	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();

	char buf[128];

	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);

	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}

	// 剪裁距离
	float clip_radius = VisUtil_GetCustomFloat(pVisBase, "clip_radius");

	if (clip_radius > 0.0F)
	{
		SafeSprintf(buf, sizeof(buf), "%d", (int)clip_radius);
		xml_add_attr(doc, element, "clip_radius", buf);
	}

	// 水面倒影
	bool water_reflect = pVisBase->GetWaterReflect();

	if (water_reflect)
	{
		xml_add_attr(doc, element, "water_reflect", "true");
	}

	CVar prop;

	// 附加信息
	if (pCore->GetProperty(pVisBase, "ExtraInfo", prop))
	{
		if (prop.IntVal())
		{
			SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
			xml_add_attr(doc, element, "extra_info", buf);
		}
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");

	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}

	return true;
}

static bool save_decal(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "decal");

	xml_add_attr(doc, element, "name", name);

	FmVec3 pos = pVisBase->GetPosition();

	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();

	char buf[128];

	// 贴花的位置精度要求更高
	SafeSprintf(buf, sizeof(buf), "%f,%f,%f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);

	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}

	CVar prop;

	if (pCore->GetProperty(pVisBase, "EnableAlphaTest", prop))
	{
		if (prop.BoolVal())
		{
			xml_add_attr(doc, element, "alpha_test", "true");
		}
	}

	if (pCore->GetProperty(pVisBase, "DiffuseMap", prop))
	{
		xml_add_attr(doc, element, "diffuse", prop.StringVal());
	}

	if (pCore->GetProperty(pVisBase, "CenterToPosition", prop))
	{
		xml_add_attr(doc, element, "center_to_pos", prop.StringVal());
	}

	if (pCore->GetProperty(pVisBase, "ProjectMode", prop))
	{
		if (stricmp(prop.StringVal(), "ortho") != 0)
		{
			xml_add_attr(doc, element, "proj_mode", prop.StringVal());
		}
	}

	if (pCore->GetProperty(pVisBase, "ProjectAngle", prop))
	{
		if (prop.FloatVal() > 0.0F)
		{
			SafeSprintf(buf, sizeof(buf), "%.1f", prop.FloatVal());
			xml_add_attr(doc, element, "proj_angle", buf);
		}
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");

	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}

	return true;
}

static bool save_trigger(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "trigger");

	xml_add_attr(doc, element, "name", name);

	FmVec3 pos = pVisBase->GetPosition();

	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();

	char buf[128];

	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);

	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}

	// 边框尺寸
	CVar size_x;
	CVar size_y;
	CVar size_z;

	pCore->GetProperty(pVisBase, "SizeX", size_x);
	pCore->GetProperty(pVisBase, "SizeY", size_y);
	pCore->GetProperty(pVisBase, "SizeZ", size_z);

	if ((!FloatEqual(size_x.FloatVal(), 1.0F))
		|| (!FloatEqual(size_y.FloatVal(), 1.0F))
		|| (!FloatEqual(size_z.FloatVal(), 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", size_x.FloatVal(), 
			size_y.FloatVal(), size_z.FloatVal());
		xml_add_attr(doc, element, "size", buf);
	}

	// 脚本名
	CVar script_name;

	if (pCore->GetProperty(pVisBase, "ScriptName", script_name))
	{
		if (!StringEmpty(script_name.StringVal()))
		{
			xml_add_attr(doc, element, "script_name", script_name.StringVal());
		}
	}

	// 事件参数
	CVar parameter;

	if (pCore->GetProperty(pVisBase, "Parameter", parameter))
	{
		if (!StringEmpty(parameter.StringVal()))
		{
			xml_add_attr(doc, element, "parameter", parameter.StringVal());
		}
	}

	// 定时时长
	CVar period;

	if (pCore->GetProperty(pVisBase, "Period", period))
	{
		if (period.FloatVal() > 0.0F)
		{
			SafeSprintf(buf, sizeof(buf), "%.3f", double(period.FloatVal()));
			xml_add_attr(doc, element, "period", buf);
		}
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");

	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}

	return true;
}

static bool save_effectmodel(CQuadVisual* pObject, IVisBase* pVisBase, 
	const char* name, xml_document<>* doc, xml_node<>* root, ICore* pCore, 
	CTerrainZone* pZone)
{
	xml_node<>* element = xml_add_node(doc, root, "effectmodel");

	xml_add_attr(doc, element, "name", name);

	// 配置文件
	result_string effectmodel_config = 
		VisUtil_GetCustomString(pVisBase, "effectmodel_config");

	xml_add_attr(doc, element, "effectmodel_config", clear_append_path(
		effectmodel_config.c_str(), pZone->GetTerrain()->GetAppendPath()));

	// 特效模型名
	result_string effectmodel_name = 
		VisUtil_GetCustomString(pVisBase, "effectmodel_name");

	xml_add_attr(doc, element, "effectmodel_name", effectmodel_name.c_str());

	FmVec3 pos = pVisBase->GetPosition();

	// 相对位置
	pos.x -= pZone->GetLeft();
	pos.z -= pZone->GetTop();
	pos.x /= pZone->GetTerrain()->GetUnitSize();
	pos.z /= pZone->GetTerrain()->GetUnitSize();

	char buf[128];

	SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", pos.x, pos.y, pos.z);
	xml_add_attr(doc, element, "pos", buf);

	FmVec3 angle = pVisBase->GetAngle();

	if ((!FloatEqual(angle.x, 0.0F))
		|| (!FloatEqual(angle.y, 0.0F))
		|| (!FloatEqual(angle.z, 0.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", angle.x, angle.y, 
			angle.z);
		xml_add_attr(doc, element, "angle", buf);
	}

	FmVec3 scale = pVisBase->GetScale();

	if ((!FloatEqual(scale.x, 1.0F))
		|| (!FloatEqual(scale.y, 1.0F))
		|| (!FloatEqual(scale.z, 1.0F)))
	{
		SafeSprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", scale.x, scale.y, 
			scale.z);
		xml_add_attr(doc, element, "scale", buf);
	}

	// 剪裁距离
	float clip_radius = VisUtil_GetCustomFloat(pVisBase, "clip_radius");

	if (clip_radius > 0.0F)
	{
		SafeSprintf(buf, sizeof(buf), "%d", (int)clip_radius);
		xml_add_attr(doc, element, "clip_radius", buf);
	}

	// 水面倒影
	bool water_reflect = pVisBase->GetWaterReflect();

	if (water_reflect)
	{
		xml_add_attr(doc, element, "water_reflect", "true");
	}

	// 装饰物
	bool widget = VisUtil_GetCustomBool(pVisBase, "widget");

	if (widget)
	{
		xml_add_attr(doc, element, "widget", "true");
	}

	// 标记信息
	result_string tag = VisUtil_GetCustomString(pVisBase, "tag");

	if (!tag.empty())
	{
		xml_add_attr(doc, element, "tag", tag.c_str());
	}

	return true;
}

static bool save_xml_doc(xml_document<>* doc, const char* file_name)
{
	Assert(doc != NULL);
	Assert(file_name != NULL);

	TStreamWriter<char, 256> writer;
	TStreamWriter<char, 256>::iterator it = writer.Begin();

	print(it, *doc, 0);

	char* buf = writer.GetData();
	size_t length = writer.GetCount();
	FILE* fp = Port_FileOpen(file_name, "wb");

	if (NULL == fp)
	{
		CORE_TRACE("(save_xml_doc)open file failed");
		CORE_TRACE(file_name);
		return false;
	}

	if (core_file::fwrite(buf, sizeof(char), length, fp) != length)
	{
		core_file::fclose(fp);
		CORE_TRACE("(save_xml_doc)write file error");
		CORE_TRACE(file_name);
		return false;
	}

	core_file::fclose(fp);

	return true;
}

// 比较物体名字
int __cdecl visual_compare(const void* elem1, const void* elem2)
{
	CQuadVisual* pObject1 = (*(CQuadVisual**)elem1);
	CQuadVisual* pObject2 = (*(CQuadVisual**)elem2);

	return strcmp(pObject1->GetName(), pObject2->GetName());
}

bool CVisualLoader::SaveVisuals(const char* file_name, 
	const char* widget_file, const char* helper_file, 
	const char* collide_file, const char* desc_file, const char* sound_file, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(widget_file != NULL);
	Assert(helper_file != NULL);
	Assert(collide_file != NULL);
	Assert(desc_file != NULL);
	Assert(sound_file != NULL);
	Assert(pZone != NULL);

	Terrain* pTerrain = pZone->GetTerrain();
	ICore* pCore = pTerrain->GetCore();
	CTerrainVisuals* pVisuals = pZone->GetVisuals();
	
	xml_document<> doc;
	
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	xml_attribute<>* version = doc.allocate_attribute("version", "1.0");

	declaration->append_attribute(version);
	doc.append_node(declaration);

	xml_node<>* root = doc.allocate_node(node_element, 
		doc.allocate_string("visuals"));

	doc.append_node(root);

	// 装饰物配置
	xml_document<> widget_doc;

	declaration = widget_doc.allocate_node(node_declaration);
	version = widget_doc.allocate_attribute("version", "1.0");
	declaration->append_attribute(version);
	widget_doc.append_node(declaration);

	xml_node<>* widget_root = widget_doc.allocate_node(node_element, 
		widget_doc.allocate_string("visuals"));

	widget_doc.append_node(widget_root);

	// 辅助物体配置
	xml_document<> helper_doc;

	declaration = helper_doc.allocate_node(node_declaration);
	version = helper_doc.allocate_attribute("version", "1.0");
	declaration->append_attribute(version);
	helper_doc.append_node(declaration);

	xml_node<>* helper_root = helper_doc.allocate_node(node_element, 
		helper_doc.allocate_string("visuals"));

	helper_doc.append_node(helper_root);

	// 附加碰撞属性配置
	xml_document<> collide_doc;

	declaration = collide_doc.allocate_node(node_declaration);
	version = collide_doc.allocate_attribute("version", "1.0");
	declaration->append_attribute(version);
	collide_doc.append_node(declaration);

	xml_node<>* collide_root = collide_doc.allocate_node(node_element, 
		collide_doc.allocate_string("visuals"));

	collide_doc.append_node(collide_root);

	// 物体描述属性配置
	xml_document<> desc_doc;

	declaration = desc_doc.allocate_node(node_declaration);
	version = desc_doc.allocate_attribute("version", "1.0");
	declaration->append_attribute(version);
	desc_doc.append_node(declaration);

	xml_node<>* desc_root = desc_doc.allocate_node(node_element, 
		desc_doc.allocate_string("visuals"));

	desc_doc.append_node(desc_root);

	// 声音配置
	xml_document<> sound_doc;

	declaration = sound_doc.allocate_node(node_declaration);
	version = sound_doc.allocate_attribute("version", "1.0");
	declaration->append_attribute(version);
	sound_doc.append_node(declaration);

	xml_node<>* sound_root = sound_doc.allocate_node(node_element, 
		sound_doc.allocate_string("visuals"));

	sound_doc.append_node(sound_root);

	TArrayPod<CQuadVisual*, 1, TCoreAlloc> vObjects;
	size_t visual_size = pVisuals->GetVisualSize();

	// 根据name进行排序
	for (size_t i = 0; i < visual_size; ++i)
	{
		CQuadVisual* pObject = pVisuals->GetVisualByIndex(i);

		if (NULL == pObject)
		{
			continue;
		}

		vObjects.push_back(pObject);
	}

	// 按照名字排序
	qsort(vObjects.data(), vObjects.size(), sizeof(CQuadVisual*), 
		visual_compare);

	for (size_t k = 0; k < vObjects.size(); ++k)
	{
		CQuadVisual* pObject = vObjects[k];
		IVisBase* pVisBase = (IVisBase*)pCore->GetEntity(
			pObject->GetVisualID());

		if (NULL == pVisBase)
		{
			continue;
		}

		if (!pVisBase->GetCustoms()->Exists("name"))
		{
			continue;
		}
		
		const char* name = pObject->GetName();

		if (StringEmpty(name))
		{
			continue;
		}

		if (VisUtil_GetCustomBool(pVisBase, "no_save"))
		{
			continue;
		}

		xml_document<>* cur_doc = &doc;
		xml_node<>* doc_root = root;

		if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			save_collide_info_and_desc(pVisBase, name, &collide_doc, 
				collide_root, &desc_doc, desc_root);
		}

		if (VisUtil_GetCustomBool(pVisBase, "widget"))
		{
			if (!StringEmpty(widget_file))
			{
				cur_doc = &widget_doc;
				doc_root = widget_root;
			}
			else
			{
				// 既然是widget，又指定不保存，则应该立即返回
				// 否则将被错误的保存到下面所能匹配的文件中，造成物件重复的BUG
				continue;
			}
		}
		
		if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			if (VisUtil_GetCustomBool(pVisBase, "only_design"))
			{
				if ((!StringEmpty(helper_file)))
				{
					if (doc_root == widget_root)
					{
						// 如果已经标记是widget物件，又有only_design标记，应该修正
						pVisBase->GetCustoms()->Assign("widget", 
							CVar(VTYPE_BOOL, false));
					}

					cur_doc = &helper_doc;
					doc_root = helper_root;
				}
				else
				{
					// 既然是only_design，又指定不保存，则应该立即返回
					// 否则将被错误的保存到下面所能匹配的文件中，造成物件重复的BUG
					continue;
				}
			}

			save_model(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
				//&collide_doc, collide_root, &desc_doc, desc_root, pCore, pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Particle"))
		{
			save_particle(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("LightSource"))
		{
			save_light(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Sound"))
		{
			if (!StringEmpty(sound_file))
			{
				cur_doc = &sound_doc;
				doc_root = sound_root;
			}

			save_sound(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			save_actor(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Decal"))
		{
			save_decal(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Trigger"))
		{
			save_trigger(pObject, pVisBase, name, cur_doc, doc_root, pCore, 
				pZone);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("EffectModel"))
		{
			save_effectmodel(pObject, pVisBase, name, cur_doc, doc_root, 
				pCore, pZone);
		}
	}

	if (!StringEmpty(file_name))
	{
		if (root->first_node() == NULL)
		{
			g_pCore->GetFileSys()->FileDelete(file_name);
		}
		else if (!save_xml_doc(&doc, file_name))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save visual failed");
			CORE_TRACE(file_name);
			return false;
		}
	}

	if (!StringEmpty(widget_file))
	{
		if (widget_root->first_node() == NULL)
		{
			g_pCore->GetFileSys()->FileDelete(widget_file);
		}
		else if (!save_xml_doc(&widget_doc, widget_file))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save widget failed");
			CORE_TRACE(widget_file);
			return false;
		}
	}

	if (!StringEmpty(helper_file))
	{
		if (helper_root->first_node() == NULL)
		{
            g_pCore->GetFileSys()->FileDelete(helper_file);
		}
		else if (!save_xml_doc(&helper_doc, helper_file))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save helper failed");
			CORE_TRACE(helper_file);
			return false;
		}
	}
	
	if (!StringEmpty(collide_file))
	{
		if (collide_root->first_node() == NULL)
		{
            g_pCore->GetFileSys()->FileDelete(collide_file);
		}
		else if (!save_xml_doc(&collide_doc, collide_file))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save collide failed");
			CORE_TRACE(collide_file);
			return false;
		}
	}

	if (!StringEmpty(desc_file))
	{
		if (desc_root->first_node() == NULL)
		{
            g_pCore->GetFileSys()->FileDelete(desc_file);
		}
		else if (!save_xml_doc(&desc_doc, desc_file))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save desc failed");
			CORE_TRACE(desc_file);
			return false;
		}
	}

	if (!StringEmpty(sound_file))
	{
		if (sound_root->first_node() == NULL)
		{
            g_pCore->GetFileSys()->FileDelete(sound_file);
		}
		else if (!save_xml_doc(&sound_doc, sound_file))
		{
			CORE_TRACE("(CVisualLoader::SaveVisuals)save sound failed");
			CORE_TRACE(sound_file);
			return false;
		}
	}

	return true;
}

bool CVisualLoader::SaveBlockGrass(const char* file_name, CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	CTerrainGrass* pGrass = pZone->GetTerrain()->GetGrass();
	size_t grass_num = pGrass->GetCount();
	size_t data_num = 0;
	size_t chunk_num = pZone->GetChunkCount();
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = pZone->GetChunkByIndex(i);
		
		for (size_t k = 0; k < grass_num; ++k)
		{
			data_num += pChunk->GetGrassCount((int)k);
		}
	}
	
	if (0 == data_num)
	{
		g_pCore->GetFileSys()->FileDelete(file_name);
		return true;
	}
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	void* handle = block_file.SaveBegin(file_name);

	if (NULL == handle)
	{
		return false;
	}

	CBlockGrassWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_GRASS_0, &writer))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}

