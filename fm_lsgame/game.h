
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
	//����ʵ��
	void CreateEnt();
	//����ʵ��
	IEntity* CreateEnt(std::string strEntName, std::string strGlobalName="");
	//ɾ��ʵ��
	void DeleteEnt();

private:
	std::vector<PERSISTID> m_vecEnt;
	std::map<std::string, size_t> m_mapNameToIndex;

};