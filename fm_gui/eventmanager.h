#pragma once
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../visual/i_event_manager.h"

#include <map>
#include <string>
#include <vector>

typedef std::vector<EventDelegate> SLOT_DELEGATE_LIST;
struct SLOT_DELEGATES
{
public:
	SLOT_DELEGATES()
	{
		enabled = true;
		delegates.clear();
	}
	~SLOT_DELEGATES()
	{
		delegates.clear();
	}

	bool GetEnabled(){return enabled;}
	void SetEnabled(bool _val){enabled = _val;}

	size_t size() const 
	{
		return delegates.size();
	}
	SLOT_DELEGATE_LIST::const_reference operator[](size_t index) const
	{
		return delegates[index];
	}
	void push_back(const EventDelegate& val)
	{
		delegates.push_back(val);
	}
	SLOT_DELEGATE_LIST::iterator erase(SLOT_DELEGATE_LIST::iterator _Where)
	{
		return delegates.erase(_Where);
	}
	SLOT_DELEGATE_LIST::iterator begin()
	{
		return delegates.begin();
	}
	SLOT_DELEGATE_LIST::iterator end()
	{
		return delegates.end();
	}

private:
	bool enabled;
	SLOT_DELEGATE_LIST delegates;
};



class EventManager
{
	//typedef std::vector<EventDelegate> SLOT_DELEGATES;
	typedef std::map<std::string, SLOT_DELEGATES> MAP_EVENT;
	typedef std::map<uint64_t, MAP_EVENT> MAP_SENDER;
	SLOT_DELEGATES m_globals;
	MAP_SENDER m_senders;

	inline void invoke(SLOT_DELEGATES& delegates,  const char* name, const IVarList& arg)
	{
		for(size_t i = 0; i < delegates.size(); i++)
		{
			if (delegates.GetEnabled())
			{
				delegates[i](name, arg);
			}
		}
	}

public:
	void connect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback);
	int invoke(const PERSISTID& sender, const char* eventname, const IVarList& arg);
	void disconnect(const PERSISTID& sender);
	void disconnect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback);

	void enableconnect(const PERSISTID& sender, bool enable);
	void enableconnect(const PERSISTID& sender, const char* eventname, bool enable);

	void connect_globals(EventDelegate recv_callback);
	void disconnect_globals(EventDelegate recv_callback);

	static EventManager& Inst()
	{
		static EventManager s_inst;
		return s_inst;
	}
};