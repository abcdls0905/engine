#include "../public/module.h"

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());
#define REG_INTERFACE(p) IIntCreator& p##_IntCreator();LinkIntCreator(p##_IntCreator());

void lib_main_net()
{
	REG_INTERFACE(WinSocket);
	REG_ENTITY(CustomHandler);
	REG_ENTITY(GameClient);
	REG_ENTITY(GameObject);
	REG_ENTITY(IGameMsgHandler);
	REG_ENTITY(GameReceiver);
	REG_ENTITY(GameScene);
	REG_ENTITY(GameSceneObj);
	REG_ENTITY(GameSender);
	REG_ENTITY(GameSock);
	REG_ENTITY(GameView);
	REG_ENTITY(GameViewObj);
}