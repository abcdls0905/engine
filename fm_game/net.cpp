#include "net.h"
#include "utility.h"
#include "../visual/i_event_manager.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_receiver.h"
#include "../visual/i_render.h"
#include "../visual/i_scene.h"
#include "../visual/i_actor.h"
#include "../visual/i_camera.h"
#include "../visual/i_terrain.h"
#include "utility.h"

//#define _AUTO_LOGIN

// 对象的移动模式
enum MOTION_MODE_ENUM
{
	MMODE_STOP,		// 停止
	MMODE_MOTION,	// 地表移动
	MMODE_JUMP,		// 跳跃
	MMODE_JUMPTO,	// 改变跳跃的目标方向
	MMODE_FLY,		// 空中移动
	MMODE_SWIM,		// 水中移动
	MMODE_DRIFT,	// 水面移动
	MMODE_CLIMB,	// 爬墙
	MMODE_SLIDE,	// 在不可行走范围内滑行
	MMODE_SINK,		// 下沉
	MMODE_LOCATE,	// 强制定位
};

DECLARE_ENTITY(GameNet, 0, IEntity);

bool GameNet::Init(const IVarList& args)
{
	m_client = (IGameClient*)g_core->CreateEntity("GameClient");
	m_socket = (IGameSock*)g_core->CreateEntity("GameSock");
	g_gamesock = m_socket;
	g_client = m_client;

	event_connect(m_socket->GetID(), "on_connected", EventDelegate(this, &GameNet::on_connected));
	event_connect(m_socket->GetID(), "on_connect_fail", EventDelegate(this, &GameNet::on_connect_fail));
	event_connect(m_socket->GetReceiver(), "on_entry_scene", EventDelegate(this, &GameNet::on_entry_scene));
	event_connect(m_socket->GetReceiver(), "on_exit_scene", EventDelegate(this, &GameNet::on_exit_scene));
	
	event_connect(m_socket->GetReceiver(), "on_scene_property", EventDelegate(this, &GameNet::on_scene_property));
	
	event_connect(m_socket->GetReceiver(), "on_login_succeed", EventDelegate(this, &GameNet::on_login_succeed));
	event_connect(m_socket->GetReceiver(), "on_add_object", EventDelegate(this, &GameNet::on_add_object));	
	event_connect(m_socket->GetReceiver(), "on_before_remove_object", EventDelegate(this, &GameNet::on_remove_object));	
	event_connect(m_socket->GetReceiver(), "on_location", EventDelegate(this, &GameNet::on_location));	
	event_connect(m_socket->GetReceiver(), "on_moving", EventDelegate(this, &GameNet::on_moving));	
	event_connect(m_socket->GetReceiver(), "on_all_dest", EventDelegate(this, &GameNet::on_all_dest));	
	event_connect(m_socket->GetReceiver(), "on_error_code", EventDelegate(this, &GameNet::on_error_code));	
	
	
//自动登陆
#ifdef _AUTO_LOGIN
	m_socket->Connect("172.19.34.199", 2001, false);
#endif
	g_gamesender = (IGameSender*)GetCore()->GetEntity(m_socket->GetSender());
	return true;
}

bool GameNet::Shut()
{
	return true;
}

void GameNet::on_connected(const char* event, const IVarList& arg)
{
	//自动登陆
#ifdef _AUTO_LOGIN
	int n = 3;
	IGameSender* psend = GetSender();
	bool b = psend->Login("hp103", "hp103");
#endif
}

void GameNet::on_connect_fail(const char* event, const IVarList& arg)
{
}

void GameNet::on_login_succeed(const char* event, const IVarList& arg)
{
	IGameSender* psend = GetSender();
	IGameReceiver* preceiver = (IGameReceiver*)g_core->GetEntity(m_socket->GetReceiver());
	int count = preceiver->GetRoleCount();
	if(count > 0)
	{
		const wchar_t* role_name = preceiver->GetRoleName(0);
		psend->ChooseRole(role_name);
	}
	else
	{
		//暂时没支持创建角色
		CORE_TRACE("no role");
		gui_showmbox(L"no role");
	}
}

void GameNet::on_entry_scene(const char* event, const IVarList& arg)
{
	IGameSender* psend = GetSender();
	
	IGameObject * pGameObj = (IGameObject*)g_core->GetEntity(m_client->GetScene());
	const char* p = pGameObj->QueryPropString("Resource");
	g_render->SetContext(g_scene->GetContext());

	char mbzTemp[512];
	sprintf(mbzTemp,"%s%s%s\\", g_core->GetResourcePath(), "map\\ter\\", p);
	terrain_load(mbzTemp);

	stage_switch(STAGE_SWITCHSCENE);
	psend->ClientReady();
}

void GameNet::on_exit_scene(const char* event, const IVarList& arg)
{
	actor_clear();
}

void GameNet::on_scene_property(const char* event, const IVarList& arg)
{
	IGameSender* psend = GetSender();
	
	IGameObject * pGameObj = (IGameObject*)g_core->GetEntity(m_client->GetScene());
	const char* p = pGameObj->QueryPropString("Resource");
}

void GameNet::on_add_object(const char* event, const IVarList& arg)
{
	PERSISTID id = m_client->GetSceneObj(arg.StringVal(0));
	
	IGameObject* pGameObj = (IGameObject*)g_core->GetEntity(id);
	if(!pGameObj)
		return;  

	IActor* pActor = role_composite_create(pGameObj);
	actor_set(id, pActor->GetID());
	if(m_client->GetPlayer() == id)
	{
		//主角
		ICamera* pCamera = (ICamera*) g_core->GetEntity(g_scene->GetCameraID());
		pCamera->SetBindID(pActor->GetID());
		pCamera->SetPosition(pGameObj->GetPosiX(), pGameObj->GetPosiY(), pGameObj->GetPosiZ());
		pCamera->SetBindPosition(0,3, -10);
		actor_setrole(id, pActor->GetID());
		g_terrain->SetPlayerID(pActor->GetID());
	}
}

void GameNet::on_remove_object(const char* event, const IVarList& arg)
{
	PERSISTID id = m_client->GetSceneObj(arg.StringVal(0));
	if(g_terrain)
	{
		IActor* pActor = actor_find(id);
		if(pActor)
		{
			actor_remove(id);
			g_terrain->RemoveVisual(pActor->GetID());
			g_core->DeleteEntity(pActor->GetID());
		}
	}
}

void GameNet::on_location(const char* event, const IVarList& arg)
{
	PERSISTID id = m_client->GetSceneObj(arg.StringVal(0));
	IGameObject * pGameObj = (IGameObject*)g_core->GetEntity(id);
	IActor* pActor = actor_find(id);
	if(pActor)
		pActor->SetPosition(pGameObj->GetPosiX(), pGameObj->GetPosiY(), pGameObj->GetPosiZ());
}

void GameNet::on_moving(const char* event, const IVarList& arg)
{
	PERSISTID id = m_client->GetSceneObj(arg.StringVal(0));
	IGameObject* pGameObj = (IGameObject*)g_core->GetEntity(id);

	IActor* pActor = actor_find(id);
	if(pActor)
	{
		FmVec3 pos = pActor->GetPosition();
		
		float x = pGameObj->GetDestX();
		float y = pGameObj->GetDestY();
		float z = pGameObj->GetDestZ();
		float speed = pGameObj->GetMoveSpeed();
		pActor->SetAngle(0.0f, pGameObj->GetDestOrient(), 0.0f);
		float mspeed = pGameObj->QueryPropFloat("MoveSpeed");
		actor_motion(pActor, pGameObj->GetDestX(), y, pGameObj->GetDestZ(), mspeed);
	}
}

void GameNet::on_all_dest(const char* event, const IVarList& arg)
{
	
}

void GameNet::on_error_code(const char* event, const IVarList& arg)
{
	wchar_t buf[256];
	SafeSwprintf(buf, sizeof(buf), L"on_error_code %d", arg.IntVal(0));
	gui_showmbox(buf);
}