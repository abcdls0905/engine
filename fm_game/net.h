#pragma once
#include "global.h"
#include "../public/i_var_list.h"
#include "../visual/i_game_client.h"
#include "../visual/i_game_sock.h"

#include <map>

class Actor;
class IGameSender;
class GameNet : public IEntity
{
	IGameClient* m_client;
	IGameSock* m_socket;

	inline IGameSender* GetSender()
	{
		return (IGameSender*)g_core->GetEntity(m_socket->GetSender());
	}
	
	void on_connect_fail(const char* event, const IVarList& arg);
	void on_connected(const char* event, const IVarList& arg);
	void on_login_succeed(const char* event, const IVarList& arg);
	void on_entry_scene(const char* event, const IVarList& arg);
	void on_exit_scene(const char* event, const IVarList& arg);
	void on_scene_property(const char* event, const IVarList& arg);
	void on_add_object(const char* event, const IVarList& arg);
	void on_remove_object(const char* event, const IVarList& arg);
	void on_location(const char* event, const IVarList& arg);
	void on_moving(const char* event, const IVarList& arg);
	void on_all_dest(const char* event, const IVarList& arg);
	void on_error_code(const char* event, const IVarList& arg);
public:
	bool Init(const IVarList& args);
	bool Shut();
};

