#pragma once

#include <assert.h>
#include "../extern/fmod/inc/fmod.hpp"

#include <string>
#include <map>
#include <vector>
#include "../public/portable.h"

namespace AUDIO{

struct ResKey
{
	ResKey()
	{}
	ResKey(const char* _file, int _mode)
		:file(_file), mode(_mode)
	{}

	std::string file;
	int mode;
};

static bool operator < (const ResKey& v1, const ResKey& v2)
{
	if(v1.mode == v2.mode)
		return v1.file < v2.file;
	else
		return (v1.file < v2.file) && (v1.mode < v2.mode);
}

class SoundRes
{
	struct Item
	{
		Item()
			:isdel(false)
		{}
		int mode;
		FMOD::Sound* sound;
		long ref;
		bool isdel;
		long timer;
		std::string name;
	};
	
	std::vector<Item> m_items;
	std::map<ResKey, long> m_index;
public:
	long createSound(FMOD::System* sys, const char* file, int mode)
	{
		ResKey _key(file, mode);
		std::map<ResKey, long>::iterator it = m_index.find(_key);
		if(it != m_index.end() /*&& m_items[it->second].ref > 0*/)
		{
			m_items[it->second].isdel = false;
			m_items[it->second].ref++;
			return it->second;
		}

		FMOD_RESULT result;
		FMOD::Sound *sound;
		result = sys->createStream(file, mode, 0, &sound);
		assert(FMOD_OK == result);
		if(FMOD_OK != result)
			return -1;

		if(m_items.size() > 1024)
		{
			for(size_t i = 0; i < m_items.size(); i++)
			{
				Item& item = m_items[i];
				if(item.sound == 0)
				{
					item.ref = 1;
					item.mode = mode;
					item.sound = sound;
					return (long)i;
				}
			}
		}

		Item item;
		item.ref = 1;
		item.mode = mode;
		item.sound = sound;
		item.name = file;
		m_items.push_back(item);
		m_index[_key] = (long)m_items.size() - 1;
		return (long)m_items.size() - 1;
	}

	void unloadSound(long id)
	{
		Item& item = m_items[id];
		assert(item.ref >0);
		item.ref --;
		if(item.ref < 1)
		{
			item.isdel = true;
			item.timer = 0;
		}
	}

	void clearAll()
	{
		for(size_t i = 0; i < m_items.size(); i++)
		{
			if(m_items[i].sound)
			{
				m_items[i].sound->release();
			}
		}
		m_items.clear();
	}


	FMOD::Sound* GetSound(long id)
	{
		return m_items[id].sound;
	}

	bool IsValid(long id)
	{
		return m_items[id].ref > 0;
	}

	void Update(float delta)
	{
		for(size_t i = 0; i < m_items.size(); i++)
		{
			if(m_items[i].isdel && m_items[i].sound)
			{
				//改成最近延迟30秒删除，减少io的次数
				if((m_items[i].timer += delta) > 300)
				{
					m_items[i].sound->release();
					m_items[i].sound = NULL;
					ResKey key(m_items[i].name.c_str(), m_items[i].mode);
					m_index.erase(key);
				}
			}
		}
	}
};

};