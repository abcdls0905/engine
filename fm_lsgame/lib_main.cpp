

#include "../public/module.h"

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());

void lib_main_game()
{
	REG_ENTITY(Game);
	REG_ENTITY(StageManager);
	REG_ENTITY(FreeCamera);

	REG_ENTITY(GuiManager);
	REG_ENTITY(FormLogin);
}