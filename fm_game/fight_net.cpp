#include "utility.h"
#include "global.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_sock.h"
#include "fight_net.h"
#include "../visual/i_event_manager.h"
#include "../visual/i_actor.h"

enum NORMAL_ATTACK_STAGE
{
    NORMAL_ATTACK_STAGE_NONE = 0,
    NORMAL_ATTACK_STAGE_BEGIN,		// 开始普通攻击：使用者，目标类型
    NORMAL_ATTACK_STAGE_EFFECT,		// 造成效果：类型
    NORMAL_ATTACK_STAGE_BREAK,		// 打断：原因
    NORMAL_ATTACK_STAGE_FINISH,		// 结束：原因
};


DECLARE_ENTITY(FightNet, 0, IEntity);

bool FightNet::Init(const IVarList& args)
{
	event_connect(g_gamesock->GetReceiver(), "on_custom", EventDelegate(this, &FightNet::on_custom));
	return true;
}

bool FightNet::Shut()
{
	return true;
}

void FightNet::on_custom(const char* event, const IVarList& arg)
{
	const char* type = arg.StringVal(1);
	if(stricmp(type, "normal_attack") == 0)
	{
		int stageType = arg.IntVal(2);
		PERSISTID objectId = arg.ObjectVal(3);
		int type = arg.GetType(3);
		
		switch (stageType)
		{
		case NORMAL_ATTACK_STAGE_BEGIN:		// 开始普通攻击：攻击者
			{
				CVarList params;
				params.Append(arg, 4, arg.GetCount() - 1);
				StageStartAttack(objectId, params);           
			}
			break;
		}
	}
}

void FightNet::StageStartAttack(const PERSISTID& vis_attacker, const IVarList& args)
{
	char buf[256];
	Port_SafeSprintf(buf, 256, "%u-%u", vis_attacker.nIdent, vis_attacker.nSerial);
	PERSISTID id = g_client->GetSceneObj(buf);
	IGameObject* pGameObj = (IGameObject*)g_core->GetEntity(id);
	const char* fightid = pGameObj->QueryPropString("FightID");
}