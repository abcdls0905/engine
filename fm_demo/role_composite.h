#pragma once
#include "../visual/i_game_client.h"
#include "../visual/i_vis_base.h"
#include "../public/core_type.h"
#include "../visual/i_actor.h"
#include "../public/core_file.h"
#include "../extern/rapidxml/rapidxml.hpp"

#include "../math/fm_math.h"
#include "../visual/i_world.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"
 
class XmlFile
{
	int m_size;
	char* m_data;
public:
	~XmlFile()
	{
		if(m_data)
			CORE_FREE(m_data, m_size);
	}
	
	XmlFile()
		:m_data(0)
	{
	}

	inline bool Load(ICore* core,const char* filename)
	{
		char buf[256];
		sprintf(buf, "%s%s", core->GetResourcePath(), filename);
		FILE* fp = core_file::fopen(buf, "rb");
		if(fp == NULL)
		{
			return false;
		}
		core_file::fseek(fp, 0, SEEK_END);
		int size = core_file::ftell(fp);
		core_file::fseek(fp, 0, SEEK_SET);
		m_data = (char*)CORE_ALLOC(size + 1);
		core_file::fread((void*)m_data, sizeof(char), size, fp);
		m_data[size] = 0;
		core_file::fclose(fp);
		m_size = size + 1;

		using namespace rapidxml;
		m_doc.parse<0>(m_data);

		return true;
	}

	inline const char* Find(const char* element_name, const char* key_attrib, const char* key_value, const char* target_attrib)
	{
		using namespace rapidxml;
		xml_node<>* root = m_doc.first_node("Object");
		xml_node<>* node = root->first_node(element_name);
		while(node)
		{
			xml_attribute<>* att = node->first_attribute(key_attrib);
			if(att && stricmp(att->value(), key_value) == 0)
			{
				return node->first_attribute(target_attrib)->value();
			}
			node = node->next_sibling(element_name);
		}
		return NULL;
	}

	inline const char* Find(const char* element_name, const char* key_attrib, int key_value, const char* target_attrib)
	{
		char buf[40];
        sprintf(buf, "%d", key_value);
		return Find(element_name, key_attrib, buf, target_attrib);
	}

	rapidxml::xml_document<> m_doc;
};

class RoleComposite : public IEntity
{
	void LoadRes();

	bool LoadXmlResFile(const char* file);

	XmlFile m_PlayerCreateXml;
	XmlFile m_PartDefineXml;
public:
	RoleComposite(void);
	~RoleComposite(void);
	// 创建一个角色
	IActor* CreateSceneObj(IScene* pScene,IWorld* pWorld,ITerrain* pTerrain,const char* pNPC,FmVec3 pos);

	virtual bool Init(const IVarList& args);

	virtual bool Shut();
};
