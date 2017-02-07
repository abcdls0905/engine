#include "../public/module.h"
#include "soundcore.h"

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());

void lib_main_sound()
{
#ifdef _WIN32
	#pragma comment(lib, "../extern/fmod/lib/fmodex_vc.lib")
#endif
	REG_ENTITY(SoundCore);
}