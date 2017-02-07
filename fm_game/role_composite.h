#pragma once
#include "../visual/i_game_client.h"
#include "../visual/i_vis_base.h"
#include "../public/core_type.h"
#include "../visual/i_actor.h"
#include <map>
#include <string>
#include <vector>
#include "../public/core_file.h"
#include "../extern/rapidxml/rapidxml.hpp"

extern ICore* g_core;;

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

	inline bool Load(const char* filename)
	{
		char buf[256];
		sprintf(buf, "%s%s", g_core->GetResourcePath(), filename);
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
	IVisBase* CreatePlayerComposite1(IEntity* pScene, IActor* pActor2, int roleIndex, bool IsAsyncLoad);
	IVisBase* CreateActor(IEntity* pScene);
	
	//获取裸模的部件资源
	core_string GetExposedConfig(int roleIndex, const char* strModelType, int iIndex = 0);

	// 获取部件列表
	void GetExposedConfigNameList(const IVarList &args, IVarList &result);

	void LoadRes();

	bool LoadXmlResFile(const char* file);

	XmlFile m_PlayerCreateXml;
	XmlFile m_PartDefineXml;
public:
	RoleComposite(void);
	~RoleComposite(void);

	bool CreateSceneObjCompositeWithActor2(IGameObject* pGameObject, IVisBase* pActor2, bool IsAsyncLoad);

	IActor* CreateSceneObj(IGameObject* pGameObject);

	virtual bool Init(const IVarList& args);

	virtual bool Shut();
};
