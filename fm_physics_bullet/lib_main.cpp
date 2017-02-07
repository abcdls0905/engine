
#include "../public/module.h"

#define REG_INTERFACE(class_name) 	extern IIntCreator& class_name##_IntCreator();\
	                             LinkIntCreator(class_name##_IntCreator())

void lib_main_physics_bullet()
{
	REG_INTERFACE(PhysicsBullet); 
}