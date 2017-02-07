
#include "../public/module.h"


#define REG_ENTITY(class_name) 	extern IEntCreator& class_name##_EntCreator();\
	                             LinkEntCreator(class_name##_EntCreator())
	                            


void lib_main_logic()
{
#ifdef FX_SYSTEM_WINDOWS
	REG_ENTITY(CInput);
#endif
	REG_ENTITY(FreeCamera);
	REG_ENTITY(Game_Scene);
	REG_ENTITY(RoleComposite);
}
