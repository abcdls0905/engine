#pragma once


#include "../tools/xmlfile.h"
#include "../../fm_gui/string_util.h"

#include "../fm_game/helper.h"
#include "../util/logic_util.h"

class ExactModelConfig
{
public:
	struct ExactModelItem
	{
		int id;
		std::string exact_model;
		std::string stage_model;

		std::string show_action;
	};
	typedef std::map<int, ExactModelItem> ExactModelItems;

public:

	ExactModelConfig(){}
	~ExactModelConfig(){}

	void LoadResource(const std::string& file );

	std::string GetExactModel(int index);
	std::string GetStageModel(int index);

	std::string GetShowAction(int index);
	
private:
	ExactModelItems m_ExactModelConfig;
};
