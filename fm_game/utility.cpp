#include "../public/i_core.h"
#include "global.h"
#include "utility.h"
#include <map>

class ActorSet
{
	std::map<int64_t, int64_t> m_index;
	PERSISTID m_rolekey;
	PERSISTID m_rolevalue;
public:
	inline void Set(const PERSISTID& key, const PERSISTID& value)
	{
		m_index[key.nData64] = value.nData64;
	}

	inline void Remove(const PERSISTID& key)
	{
		std::map<int64_t, int64_t>::iterator it = m_index.find(key.nData64);
		if(it == m_index.end())
			return;
		m_index.erase(it);
	}

	inline PERSISTID Find(const PERSISTID& key)
	{
		std::map<int64_t, int64_t>::iterator it = m_index.find(key.nData64);
		if(it == m_index.end())
			return PERSISTID(0, 0);
		PERSISTID pid;
		pid.nData64 = it->second;
		return pid;
	}

	void SetRole(const PERSISTID& key, const PERSISTID& value)
	{
		m_rolekey = key;
		m_rolevalue = value;
	}

	void Clear()
	{
		for(std::map<int64_t, int64_t>::iterator it = m_index.begin(); it!= m_index.end(); it++)
		{
			PERSISTID pid;
			pid.nData64  = it->second;
			g_core->DeleteEntity(pid);
		}
		m_index.clear();
		SetRole(PERSISTID(), PERSISTID());
	}

	const PERSISTID& GetRole()const
	{
		return m_rolevalue;
	}
};

ActorSet g_actorset;

IActor* actor_find(const PERSISTID& id)
{
	return (IActor*)g_core->GetEntity(g_actorset.Find(id));
}

void actor_set(const PERSISTID& key, const PERSISTID& actor_id)
{
	g_actorset.Set(key, actor_id);
}

void actor_setrole(const PERSISTID& key, const PERSISTID& actor_id)
{
	g_actorset.SetRole(key, actor_id);
}

void actor_remove(const PERSISTID& id)
{
	g_actorset.Remove(id);
}

void actor_clear()
{
	g_actorset.Clear();
}

IActor* actor_getrole()
{
	return (IActor*)g_core->GetEntity(g_actorset.GetRole());
}
