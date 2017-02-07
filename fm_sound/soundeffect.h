#pragma once

#include "../visual/i_soundsystem.h"

//谈入
class SoundEffectFadein : public ISoundEffect
{
	int m_startpos;
	int m_endpos;
	bool m_first;
public:
	void Create(int pos, int time)
	{
		m_first = true;

		//开始谈入的位置
		m_startpos = pos; 
		
		//结束谈入的位置
		m_endpos = pos + time;
	}

	void OnPlaying(SOUND_HANDLE id)
	{
		if(!m_isdel)
			return;

		if(m_first)
		{
			m_first = false;
		}

		int length = GetSoundSystem()->GetLength(id);
		unsigned int pos = GetSoundSystem()->GetPosition(id);
		if(pos >= m_startpos & pos <= m_endpos)
		{
			float per = (pos - m_startpos) / float(m_endpos - m_startpos);
			GetSoundSystem()->SetVolume(id, 1.0f * (per));
		}
		else
		{
			GetSoundSystem()->SetVolume(id, 1.0f);
			m_isdel = true;
		}
	}	
};


class SoundEffectOut : public ISoundEffect
{
	int m_startpos;
	int m_endpos;
	bool m_first;
public:
	void Create(int pos, int time)
	{
		m_first = true;

		//开始谈入的位置
		m_startpos = pos; 
		
		//结束谈入的位置
		m_endpos = pos + time;
	}

	void OnPlaying(SOUND_HANDLE id)
	{
		if(!m_isdel)
			return;

		if(m_first)
		{
			m_first = false;
		}

		int length = GetSoundSystem()->GetLength(id);
		unsigned int pos = GetSoundSystem()->GetPosition(id);
		if(pos >= m_startpos & pos <= m_endpos)
		{
			float per = 1.0f - (pos - m_startpos) / float(m_endpos - m_startpos);
			GetSoundSystem()->SetVolume(id, 1.0f * (per));
		}
		else
		{
			GetSoundSystem()->SetVolume(id, 0.0f);
			m_isdel = true;
		}
	}	
};