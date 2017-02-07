
#include "../public/module.h"

 

#define REG_ENTITY(class_name) 	extern IEntCreator& class_name##_EntCreator();\
	                             LinkEntCreator(class_name##_EntCreator())
	                      

void lib_main_terrain()
{
	REG_ENTITY(GrassType);
	REG_ENTITY(Terrain);
    REG_ENTITY(TerrainMaterial);
    REG_ENTITY(TerrainBrush);
    REG_ENTITY(TerrainEditor);
	REG_ENTITY(SeaWater);
	REG_ENTITY(Trigger);
}