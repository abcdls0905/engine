#include "../public/i_entity.h"
#include "soundsystem.h"

class SoundCore : public IEntity
{
public:
	~SoundCore()
	{
	}

	bool Init(const IVarList& args)
	{
		GetSoundSystem()->Init();
		g_pCore->AddExecute(this);
		return true;
	}

	bool Shut()
	{
		GetSoundSystem()->Destroy();
		return true;
	}

	void Execute(float seconds)
	{
		AUDIO::SoundSystem* pSoundSystem = (AUDIO::SoundSystem*)GetSoundSystem();
		pSoundSystem->OnFrame(seconds);
		//((SoundSystem*)GetSoundSystem());//->OnFrame(seconds);
	}
};
