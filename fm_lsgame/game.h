
#pragma once

#include "../public/i_entity.h"
#include <string>
#include <vector>
#include <map>

class Game : public IEntity
{
public:
	Game();
	~Game();

	bool Init(const IVarList& args);
	bool Shut();
private:
	//创建实体
	void CreateEnt();
	//创建实体
	IEntity* CreateEnt(std::string strEntName, std::string strGlobalName="");
	//删除实体
	void DeleteEnt();

private:
	std::vector<PERSISTID> m_vecEnt;
	std::map<std::string, size_t> m_mapNameToIndex;

};