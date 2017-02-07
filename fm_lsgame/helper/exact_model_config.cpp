#include "exact_model_config.h"
#include <algorithm>



void ExactModelConfig::LoadResource(const std::string& file )
{
	CXmlFile xml(file.c_str());
	if (!xml.LoadFromFile())
	{
		return;
	}

	CVarList section_list;
	xml.GetSectionList(section_list);

	const int section_count = static_cast<int>(section_list.GetCount());
	for (int section_index =0; section_index<section_count; section_index++)
	{
		std::string section = section_list.StringVal(section_index);
		int role_id = StringUtil::StringAsInt(section);

		std::string exact_model = xml.ReadString(section.c_str(), "exact_model", "");
		std::string stage_model = xml.ReadString(section.c_str(), "stage_model", "");
		std::string show_action = xml.ReadString(section.c_str(), "action_show", "");

		ExactModelItem item;
		item.id = role_id;
		item.exact_model = exact_model;
		item.stage_model = stage_model;
		item.show_action = show_action;

		m_ExactModelConfig[role_id] = item;
	}
}

std::string ExactModelConfig::GetExactModel(int index)
{
	if (m_ExactModelConfig.find(index)==m_ExactModelConfig.end())
	{
		return "";
	}

	return m_ExactModelConfig[index].exact_model;
}

std::string ExactModelConfig::GetStageModel(int index)
{
	if (m_ExactModelConfig.find(index)==m_ExactModelConfig.end())
	{
		return "";
	}

	return m_ExactModelConfig[index].stage_model;
}

std::string ExactModelConfig::GetShowAction(int index)
{
	if (m_ExactModelConfig.find(index)==m_ExactModelConfig.end())
	{
		return "";
	}

	return m_ExactModelConfig[index].show_action;
}