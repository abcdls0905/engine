#include "eventManager.h"

void EventManager::connect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback)
{
	MAP_EVENT& e = m_senders[sender.nData64];
	e[eventname].push_back(recv_callback);
}

int EventManager::invoke(const PERSISTID& sender, const char* eventname, const IVarList& arg)
{
	invoke(m_globals, eventname, arg); 

	if(m_senders.empty())
		return 0;

	MAP_SENDER::iterator it = m_senders.find(sender.nData64);
	if(it == m_senders.end())
		return 0;

	MAP_EVENT& events = it->second;
	MAP_EVENT::iterator itMap =  events.find(std::string(eventname));
	if(itMap == events.end())
		return 0;

	SLOT_DELEGATES& delegates = itMap->second;
	invoke(itMap->second, itMap->first.c_str(), arg);

	return 1;
}

void EventManager::disconnect(const PERSISTID& sender)
{
	MAP_SENDER::iterator it = m_senders.find(sender.nData64);
	if(it != m_senders.end())
	{
		m_senders.erase(it);
	}
}

void EventManager::disconnect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback)
{
	MAP_SENDER::iterator it = m_senders.find(sender.nData64);
	if(it == m_senders.end())
		return;
		
	MAP_EVENT& events = it->second;
	MAP_EVENT::iterator itMap =  events.find(std::string(eventname));
	if(itMap == events.end())
		return ;

	SLOT_DELEGATES& delegates = itMap->second;
	for(size_t i = 0; i < delegates.size(); i++)
	{
		if(delegates[i] == recv_callback)
		{
			delegates.erase(delegates.begin() + i);
			i--;
		}
	}
}

void EventManager::connect_globals(EventDelegate recv_callback)
{
	m_globals.push_back(recv_callback);
}

void EventManager::disconnect_globals(EventDelegate recv_callback)
{
	SLOT_DELEGATES& delegates = m_globals;
	for(size_t i = 0; i < delegates.size(); i++)
	{
		if(delegates[i] == recv_callback)
		{
			delegates.erase(delegates.begin() + i);
			i--;
		}
	}
}


void EventManager::enableconnect(const PERSISTID& sender, bool enable)
{
	MAP_SENDER::iterator it = m_senders.find(sender.nData64);
	if(it == m_senders.end())
		return;

	MAP_EVENT& events = it->second;
	MAP_EVENT::iterator itBeg =  events.begin();
	MAP_EVENT::iterator itEnd =  events.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		SLOT_DELEGATES& delegates = itBeg->second;
		delegates.SetEnabled(enable);
	}
}
void EventManager::enableconnect(const PERSISTID& sender, const char* eventname, bool enable)
{
	MAP_SENDER::iterator it = m_senders.find(sender.nData64);
	if(it == m_senders.end())
		return;

	MAP_EVENT& events = it->second;
	MAP_EVENT::iterator itBeg =  events.begin();
	MAP_EVENT::iterator itEnd =  events.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		std::string msg = itBeg->first;
		SLOT_DELEGATES& delegates = itBeg->second;
		if (msg==eventname)
		{
			delegates.SetEnabled(enable);
		}
	}
}

//连接一事件和一个代理
void event_connect(const PERSISTID& sender, const char* eventname, EventDelegate recv_delegate)
{
	EventManager::Inst().connect(sender, eventname, recv_delegate);
}

//播放事件
int event_send(const PERSISTID& sender, const char* eventname, const IVarList& arg)
{
	return EventManager::Inst().invoke(sender, eventname, arg);
}

//解除sender的所有链接
void event_disconnect(const PERSISTID& sender)
{
	EventManager::Inst().disconnect(sender);
}

//解除sender的所有链接
void event_disconnect(const PERSISTID& sender, const char* eventname, EventDelegate recv_callback)
{
	EventManager::Inst().disconnect(sender, eventname, recv_callback);
}

//禁用/开启 sender的所有链接
void event_enableconnect(const PERSISTID& sender, bool enable)
{
	EventManager::Inst().enableconnect(sender, enable);
}

//禁用/开启 sender的某个消息链接
void event_enableconnect(const PERSISTID& sender, const char* eventname, bool enable)
{
	EventManager::Inst().enableconnect(sender, eventname, enable);
}

void event_connect_globals(EventDelegate recv_callback)
{
	EventManager::Inst().connect_globals(recv_callback);
}

void disconnect_globals(EventDelegate recv_callback)
{
	EventManager::Inst().disconnect_globals(recv_callback);
}