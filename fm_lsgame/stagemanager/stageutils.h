

#pragma once

#include "../public/Module.h"

#include "../source_path_define.h"

#include "../global.h"

#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <sstream>

//
const std::string STAGE_LOGIN	= "login";
const std::string STAGE_CREATE	= "create";
const std::string STAGE_HOME	= "home";
const std::string STAGE_MAIN	= "main";
const std::string STAGE_GUIDE	= "guide";
const std::string STAGE_RANK	= "rank";
const std::string STAGE_ROLES	= "roles";
const std::string STAGE_SHOP	= "shop";
const std::string STAGE_ROOMS	= "rooms";
const std::string STAGE_TASK	= "task";


const std::string CAMERA_INDEX_LOGIN = "1";
const std::string CAMERA_INDEX_ROLES = "2";
const std::string CAMERA_INDEX_CREATE = "2";


typedef unsigned int UINT;

class StageUtils : public IEntity
{
public:
	StageUtils(void);
	virtual ~StageUtils(void);

	static void LoadLoginScene();
};