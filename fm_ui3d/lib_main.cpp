#include "ui3d_core.h"

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());
#define REG_INTERFACE(p) IIntCreator& p##_IntCreator();LinkIntCreator(p##_IntCreator());

void lib_main_ui3d()
{
	REG_ENTITY(RenderUI);

	REG_INTERFACE(ui3d_core);
}