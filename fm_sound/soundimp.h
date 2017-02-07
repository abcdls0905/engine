#pragma once

namespace AUDIO{

#include <string>
#include <map>
#include <vector>

#include "../visual/i_soundsystem.h"

class SoundImp
{
	struct Item
	{
		FMOD::Channel* channel;
		long resid;
	};

	std::vector<Item> m_items;
	std::map<FMOD::Channel*, long> m_index;
	struct SoundEffectItem
	{
		long id;
		ISoundEffect* effect;
	};
	std::vector<SoundEffectItem> m_effects;
public:
	long Create(FMOD::Channel* channel, long resid)
	{
		if(m_index.size() > 256)
		{
			for(size_t i = 0; i < m_items.size(); i++)
			{
				if(m_items[i].channel == NULL)
				{
					m_items[i].channel = channel;
					m_items[i].resid = resid;
					m_index[channel] = (long)i;
					return (long)i;
				}
			}
		}

		Item item;
		item.channel = channel;
		item.resid = resid;
		m_items.push_back(item);
		m_index[channel] = (long)m_items.size() - 1;
		
		return (long)m_items.size() - 1;
	}
	
	long Find(FMOD::Channel* channel)
	{
		std::map<FMOD::Channel*, long>::iterator it = m_index.find(channel);
		if(it != m_index.end())
			return it->second;
		return -1;
	}

	void Remove(long id)
	{
		if(m_items[id].channel)
		{
			m_items[id].channel->stop();
			m_items[id].channel = NULL;
		}
	}

	FMOD::Channel* GetChannel(long id)
	{
		return m_items[id].channel;
	}

	long GetResID(long id)
	{
		return m_items[id].resid;
	}

	void AddEffect(ISoundEffect* effect, long id)
	{
		SoundEffectItem item;
		item.effect = effect;
		item.id = id;
		m_effects.push_back(item);
	}

	void RemoveEffect(ISoundEffect* effect, long id)
	{
		for(size_t i = 0; i < m_effects.size(); i++)
		{
			if(effect == m_effects[i].effect && id == m_effects[i].id)
			{
				m_effects.erase(m_effects.begin() + i);
				break;
			}
		}
	}

	void Update(float delta)
	{
		for(size_t i = 0; i < m_effects.size(); i++)
		{
			m_effects[i].effect->OnPlaying(m_effects[i].id);
			if(m_effects[i].effect->IsDel())
			{
				delete(m_effects[i].effect);
				m_effects.erase(m_effects.begin() + i);
			}
		}
	}
};

};