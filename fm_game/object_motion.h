#pragma once
#include "../public/i_entity.h"
#include "../visual/i_actor.h"
#include <vector>
#include <map>

class GameObjMotion : public IEntity
{
	struct MotionObj
	{
		PERSISTID id;
		double startTime;
		double run_speed;
		double rotate_speed;
		double dist;
		float dy;
		float dstAngle;
		float srcAngle;
		FmVec2 startPos;
		FmVec2 vPos;
		FmVec2 dstPos;
	};

	std::vector<MotionObj> m_motions;
	inline MotionObj* Find(const PERSISTID& id)
	{
		for(size_t i = 0; i < m_motions.size(); i++)
		{
			if(id.nData64 == m_motions[i].id.nData64)
				return &m_motions[i];
		}
		return NULL;
	}

	void Execute(float seconds);
public:
	void SetMotion(IActor* actor, float dx, float dy, float dz, float speed);

	bool Init(const IVarList& args);
	
	bool Shut();
};