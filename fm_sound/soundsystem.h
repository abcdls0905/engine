#pragma once

#include <assert.h>
#include "../extern/fmod/inc/fmod.hpp"

#include "../visual/i_soundsystem.h"
#include "SoundRes.h"
#include "SoundImp.h"
#include "../public/core_file.h"
#include "../public/core_log.h"

namespace AUDIO{

class SoundSystem : public ISoundSystem
{
	/*
	File callbacks
	*/
	static FMOD_RESULT F_CALLBACK myopen(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
	{
		if (name)
		{
			FILE *fp;

			fp = core_file::fopen(name, "rb");
			if (!fp)
			{
				return FMOD_ERR_FILE_NOTFOUND;
			}

			core_file::fseek(fp, 0, SEEK_END);
			*filesize = core_file::ftell(fp);
			core_file::fseek(fp, 0, SEEK_SET);

			*userdata = (void *)0x12345678;
			*handle = fp;
		}

		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK myclose(void *handle, void *userdata)
	{
		if (!handle)
		{
			return FMOD_ERR_INVALID_PARAM;
		}

		core_file::fclose((FILE *)handle);

		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK myread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	{
		if (!handle)
		{
			return FMOD_ERR_INVALID_PARAM;
		}

		if (bytesread)
		{
			*bytesread = (int)core_file::fread(buffer, 1, sizebytes, (FILE *)handle);   
			if (*bytesread < sizebytes)
			{
				return FMOD_ERR_FILE_EOF;
			}
		}


		return FMOD_OK;
	}


	static FMOD_RESULT F_CALLBACK myseek(void *handle, unsigned int pos, void *userdata)
	{
		printf("************ seek to %d\n", pos);
		if (!handle)
		{
			return FMOD_ERR_INVALID_PARAM;
		}
	   
		core_file::fseek((FILE *)handle, pos, SEEK_SET);

		return FMOD_OK;
	}

	SoundRes m_res;
	SoundImp m_imp;
	FMOD::System *m_system;
	
	inline const FMOD_VECTOR* FMOD_VECTOR_CASE(const SOUND_VECTOR& v)const
	{
		return (const FMOD_VECTOR*)&v;
	}

	inline int Getfmod(int mode)
	{
		switch(mode)
		{
		case SOUND_MODE_DEFAULT:
			return FMOD_HARDWARE;
		case SOUND_MODE_3D:
			return FMOD_HARDWARE|FMOD_3D;
		default:
			assert(0);
			break;
		}
		return 0;
	}

	static FMOD_RESULT F_CALLBACK my_channel_callback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *param1, void *param2)
	{
		switch(type)
		{
		case FMOD_CHANNEL_CALLBACKTYPE_END:                 /* Called when a sound ends. */
			{
				SoundSystem::Inst()->RemoveSound(channel);
			}
			break;
		default:
			break;
		}
		return FMOD_OK;
	}

	inline void RemoveSound(FMOD_CHANNEL* channel)
	{
		long sound = m_imp.Find((FMOD::Channel*)channel);
		if(sound > -1)
		{
			m_imp.Remove(sound);
			long resid = m_imp.GetResID(sound);
			m_res.unloadSound(resid);
		}
	}
public:
	SoundSystem()
	{}

	~SoundSystem()
	{
		m_res.clearAll();
	}

	void Init(int MaxChannel = 32)
	{
		FMOD_RESULT result = FMOD::System_Create(&m_system);
		result = m_system->init(MaxChannel, FMOD_INIT_NORMAL, 0);	

		result = m_system->setFileSystem(myopen, myclose, myread, myseek, NULL, NULL, 2048);
	}

	void Destroy()
	{
		m_res.clearAll();
	}

	SOUNDRES_HANDLE LoadSound(const char* file, int mode = SOUND_MODE_DEFAULT)
	{
		return m_res.createSound(m_system, file, Getfmod(mode));
	}

	SOUND_HANDLE Play(SOUNDRES_HANDLE id, int mode = SOUND_MODE_DEFAULT, bool paused = false)
	{
		FMOD::Sound* pSound = m_res.GetSound(id);
		FMOD::Channel* channel;
		FMOD_RESULT result = m_system->playSound(FMOD_CHANNEL_FREE, pSound, paused, &channel);
		if(FMOD_OK != result)
		{
			CORE_TRACE_EX("sound play fail");
			return -1;
		}
		channel->setCallback(my_channel_callback);
		//assert(FMOD_OK == result);
		return m_imp.Create(channel, id);
	}

	SOUND_HANDLE Play(const char* file, int mode, bool paused = false)
	{
		long sound = LoadSound(file, mode);
		return Play(sound, mode, paused);
	}

	SOUNDRES_HANDLE GetSoundRes(SOUND_HANDLE id)
	{
		return m_imp.GetResID(id);
	}

	bool IsValid(SOUND_HANDLE id)
	{
		return m_imp.GetChannel(id) != NULL;
	}

	unsigned int GetLength(SOUND_HANDLE id)
	{
		FMOD::Sound * pSound = m_res.GetSound(m_imp.GetResID(id));
		unsigned int length;
		pSound->getLength(&length, FMOD_TIMEUNIT_MS);
		return length;
	}

	void SetPosition(SOUND_HANDLE id, unsigned int pos)
	{
		m_imp.GetChannel(id)->setPosition(pos, FMOD_TIMEUNIT_MS);
	}

	unsigned int GetPosition(SOUND_HANDLE id)
	{
		unsigned int pos;
		if(FMOD_OK == m_imp.GetChannel(id)->getPosition(&pos, FMOD_TIMEUNIT_MS))
			return pos;
		return 0;
	}

	void SetVolume(SOUND_HANDLE id, float volume)
	{
		m_imp.GetChannel(id)->setVolume(volume);
	}

	float GetVolume(SOUND_HANDLE id)
	{
		float volume;
		m_imp.GetChannel(id)->getVolume(&volume);
		return volume;
	}
	
	void Set3DParam(SOUND_HANDLE id, const SOUND_VECTOR& pos, const SOUND_VECTOR& vel)
	{
		m_imp.GetChannel(id)->set3DAttributes(FMOD_VECTOR_CASE(pos), FMOD_VECTOR_CASE(vel));
	}

	void Set3DSetListener(const SOUND_VECTOR& pos, const SOUND_VECTOR& vel, const SOUND_VECTOR& forward, const SOUND_VECTOR& up)
	{
		m_system->set3DListenerAttributes(0, FMOD_VECTOR_CASE(pos), FMOD_VECTOR_CASE(vel), FMOD_VECTOR_CASE(forward), FMOD_VECTOR_CASE(up));
	}
	
	void Set3DMinMaxDistance(long resid, float mindistance, float maxdistance)
	{
		m_imp.GetChannel(resid)->set3DMinMaxDistance(mindistance, maxdistance);	
	}

	void Pause(SOUND_HANDLE id)
	{
		m_imp.GetChannel(id)->setPaused(true);
	}

	bool IsPaused(SOUND_HANDLE id)
	{
		bool paused;
		m_imp.GetChannel(id)->getPaused(&paused);
		return paused;
	}

	void Resume(SOUND_HANDLE id)
	{
		m_imp.GetChannel(id)->setPaused(false);
	}

	void Stop(SOUND_HANDLE id)
	{
		m_imp.GetChannel(id)->stop();
	}

	bool IsPlaying(SOUND_HANDLE id)
	{
		bool isplaying;
		if(FMOD_OK == m_imp.GetChannel(id)->isPlaying(&isplaying))
			return isplaying;
		return false;
	}

	bool IsLoop(SOUND_HANDLE id)
	{
		return true;	
	}

	void SetLoop(SOUND_HANDLE id, bool loop)
	{
		if(loop)
			m_imp.GetChannel(id)->setMode(FMOD_LOOP_NORMAL);
		else
			m_imp.GetChannel(id)->setMode(FMOD_LOOP_OFF);
	}
	
	void OnFrame(float delta)
	{
		m_imp.Update(delta);
		m_res.Update(delta);
		m_system->update();
	}

	void AddSoundEffect(SOUND_HANDLE id, ISoundEffect* effect)
	{
		m_imp.AddEffect(effect, id);
	}

	static SoundSystem* Inst()
	{
		static SoundSystem s_inst;
		return &s_inst;
	}
};

};