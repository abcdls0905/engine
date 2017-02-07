#include "object_motion.h"
#include "../visual/i_actor.h"
#include "global.h"
#include "../visual/i_terrain.h"

/*************************************************
Function:       GetVectorAngle
Description:    得到向量在x-z平面上的转角
*************************************************/
inline float GetVectorAngle(const FmVec3& vec)
{
    if (vec.x == 0.0f && vec.z == 0.0f)
    {
        return 0.0f;
    }

    float angle = acosf(vec.z / sqrtf(vec.x * vec.x + vec.z * vec.z));
    // 在第2和4象限上的角为负角
    if (vec.x < 0.0f)
        angle = -angle;

    return angle;
}

DECLARE_ENTITY(GameObjMotion, 0, IEntity);

GameObjMotion* g_game_obj_motion = NULL;

void GameObjMotion::SetMotion(IActor* actor, float dx, float dy, float dz, float speed)
{
	MotionObj* _obj = Find(actor->GetID());
	if(_obj)
	{
		MotionObj& obj = *_obj;
		obj.id = actor->GetID();
		obj.srcAngle = actor->GetAngle().y;
		obj.startPos = FmVec2(actor->GetPosition().x, actor->GetPosition().z);
		obj.dstPos = FmVec2(dx, dz);
		obj.vPos = obj.dstPos - obj.startPos;
		obj.dist = FmVec2Length(&obj.vPos);
		obj.dy = dy;
		if(obj.dist < 0.00001)
			return;
		obj.run_speed = speed;
		FmVec2Normalize(&obj.vPos, &obj.vPos);
		obj.startTime = GetCore()->GetTotalSeconds();
		obj.dstAngle = GetVectorAngle(FmVec3(obj.vPos.x, 0.0, obj.vPos.y));
		actor->SetAngle(0.0f, obj.dstAngle, 0.0f);
	}
	else
	{
		MotionObj obj;
		obj.id = actor->GetID();
		obj.srcAngle = actor->GetAngle().y;
		obj.startPos = FmVec2(actor->GetPosition().x, actor->GetPosition().z);
		obj.dstPos = FmVec2(dx, dz);
		obj.vPos = obj.dstPos - obj.startPos;
		obj.dist = FmVec2Length(&obj.vPos);
		obj.dy = dy;
		if(obj.dist < 0.00001)
			return;
		obj.run_speed = speed;
		FmVec2Normalize(&obj.vPos, &obj.vPos);
		obj.startTime = GetCore()->GetTotalSeconds();
		obj.dstAngle = GetVectorAngle(FmVec3(obj.vPos.x, 0, obj.vPos.y));
		actor->SetAngle(0.0f, obj.dstAngle, 0.0f);
		m_motions.push_back(obj);
	}
}

void GameObjMotion::Execute(float seconds)
{
	size_t count = m_motions.size();
	ITerrain* pTerrain = (ITerrain*)g_core->LookupEntity("Terrain");

	for(std::vector<MotionObj>::iterator it = m_motions.begin(); it != m_motions.end();)
	{
		MotionObj& obj = *it;
		IActor* pActor = (IActor*)GetCore()->GetEntity(obj.id);
		if(!pActor)
			return;
		
			float t= (float)(GetCore()->GetTotalSeconds() - obj.startTime);
			float s = (float)((GetCore()->GetTotalSeconds() - obj.startTime) * obj.run_speed);
			if(s >= obj.dist)
			{
				pActor->SetPosition(obj.dstPos.x, obj.dy, obj.dstPos.y);
                CVarList result;
				pActor->BlendAction(CVarList() << "pe_0h_stand" << true << true, result);
				it = m_motions.erase(it);
			}
			else
			{
				FmVec2 curPos = obj.startPos + obj.vPos * s;	
				IActor* pActor = (IActor*)GetCore()->GetEntity(obj.id);
				if(pActor)
				{
                    CVarList result;
					pActor->BlendAction(CVarList() << "pe_0h_walk" << true << true, result);
					pActor->SetPosition(curPos.x, pTerrain->GetPosiY(curPos.x, curPos.y), curPos.y);
				}
				it++;
			}
		
	}
}

bool GameObjMotion::Init(const IVarList& args)
{
	g_game_obj_motion = this;
	GetCore()->AddExecute(this);
	return true;
}
	
bool GameObjMotion::Shut()
{
	return true;
}

void actor_motion(IActor* actor, float x, float y, float z, float speed)
{
	g_game_obj_motion->SetMotion(actor, x, y, z, speed);	
}