#include "SoundSystem.h"

ISoundSystem* GetSoundSystem()
{
	return AUDIO::SoundSystem::Inst();
}