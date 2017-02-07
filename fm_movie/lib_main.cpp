
#include "../public/module.h"

#define REG_ENTITY(class_name) 	    extern IEntCreator& class_name##_EntCreator();\
    LinkEntCreator(class_name##_EntCreator())

#define REG_INTERFACE(class_name) 	extern IIntCreator& class_name##_IntCreator();\
    LinkIntCreator(class_name##_IntCreator())

void lib_main_movie()
{
    REG_ENTITY(CameraWrapper); 
    REG_ENTITY(TimeAxis); 
}