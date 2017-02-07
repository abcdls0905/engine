#pragma once

#include "../public/i_entity.h"

class FightNet : public IEntity
{
	void StageStartAttack(const PERSISTID& vis_attacker, const IVarList& args);
public:
	bool Init(const IVarList& args);
	bool Shut();

	void on_custom(const char* event, const IVarList& arg);
};