
#include "../public/module.h"

#define REG_ENTITY(class_name) 	    extern IEntCreator& class_name##_EntCreator();\
    LinkEntCreator(class_name##_EntCreator())

#define REG_INTERFACE(class_name) 	extern IIntCreator& class_name##_IntCreator();\
    LinkIntCreator(class_name##_IntCreator())

void lib_main_geometry()
{
    REG_ENTITY(Geometry); 
    REG_ENTITY(LinesDrawer); 
    REG_INTERFACE(Primitive3DDrawer); 
}