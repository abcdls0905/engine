//--------------------------------------------------------------------
// �ļ���:		game_client.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_client.h"
#include "game_scene.h"
#include "game_scene_obj.h"
#include "game_view.h"
#include "game_view_obj.h"
#include "../public/var_list.h"
#include "../public/var.h"
#include <math.h>

/// \file game_client.cpp
/// \brief ��Ϸ�ͻ���

/// entity: GameClient
/// \brief ��Ϸ�ͻ���ʵ��
DECLARE_ENTITY(GameClient, NULL, IEntity);

/// readonly: string PlayerIdent
/// \brief ������Ҷ����ʶ
DECLARE_PROPERTY_GET(const char*, GameClient, PlayerIdent, GetPlayerIdent);

/*
/// method: bool IsPlayer()
/// \brief �Ƿ���Ҷ���
DECLARE_METHOD_1(bool, GameClient, IsPlayer, const char*);
/// method: object GetPlayer()
/// \brief �����Ҷ���
DECLARE_METHOD_0(PERSISTID, GameClient, GetPlayer);
/// method: object GetScene()
/// \brief ��ó�������
DECLARE_METHOD_0(PERSISTID, GameClient, GetScene);
/// method: object GetSceneObj(string ident)
/// \brief ��ó����ڶ���
/// \param ident �����ʶ
DECLARE_METHOD_1(PERSISTID, GameClient, GetSceneObj, const char*);
/// method: object GetView(string view_ident)
/// \brief ����Ӵ�����
/// \param view_ident �Ӵ���ʶ
DECLARE_METHOD_1(PERSISTID, GameClient, GetView, const char*);
/// method: object GetViewObj(string view_ident, string item_ident)
/// \brief ����Ӵ��ڶ���
/// \param view_ident �Ӵ���ʶ
/// \param item_ident ��Ʒ��ʶ
DECLARE_METHOD_2(PERSISTID, GameClient, GetViewObj, const char*, const char*);
/// method: table GetViewList()
/// \brief ����Ӵ��б�
DECLARE_METHOD_T(GameClient, GetViewList);

/// method: bool ClearAll()
/// \brief �����������
DECLARE_METHOD_0(bool, GameClient, ClearAll);*/

// GameClient

GameClient::GameClient()
{
	m_pScene = NULL;
}

GameClient::~GameClient()
{
}

bool GameClient::Init(const IVarList& args)
{
	return true;
}

bool GameClient::Shut()
{
	ClearAll();
	
	return true;
}

void GameClient::SetPlayerIdent(const char* value)
{
	m_strPlayerIdent = value;
}

const char* GameClient::GetPlayerIdent() const
{
	return m_strPlayerIdent.c_str();
}

GameScene* GameClient::CreateScene(const char* player_ident)
{
	SAFE_RELEASE(m_pScene);
	
	m_pScene = (GameScene*)GetCore()->CreateEntity("GameScene");
	
	m_strPlayerIdent = player_ident;

	return m_pScene;
}

bool GameClient::DeleteScene()
{
	SAFE_RELEASE(m_pScene);

	return true;
}

GameScene* GameClient::GetCurrentScene() const
{
	return m_pScene;
}

GameSceneObj* GameClient::GetCurrentPlayer() const
{
	if (NULL == m_pScene)
	{
		return NULL;
	}

	return m_pScene->GetSceneObjByIdent(m_strPlayerIdent.c_str());
}

GameView* GameClient::CreateView(const char* view_ident, int capacity)
{
	GameView* pView = (GameView*)GetCore()->CreateEntity("GameView");
	
	pView->SetCapacity(capacity);

	m_Views.Add(view_ident, pView);

	return pView;
}

bool GameClient::DeleteView(const char* view_ident)
{
	return m_Views.Remove(view_ident);
}

GameView* GameClient::GetViewByIdent(const char* view_ident) const
{
	return (GameView*)m_Views.GetObjectByIdent(view_ident);
}

bool GameClient::IsPlayer(const char* ident) const
{
	if (NULL == m_pScene)
	{
		return false;
	}

	return strcmp(m_strPlayerIdent.c_str(), ident) == 0;
}

PERSISTID GameClient::GetPlayer() const
{
	if (NULL == m_pScene)
	{
		return PERSISTID();
	}

	GameSceneObj* pObj = m_pScene->GetSceneObjByIdent(
		m_strPlayerIdent.c_str());

	if (NULL == pObj)
	{
		return PERSISTID();
	}

	return pObj->GetID();
}

PERSISTID GameClient::GetScene() const
{
	if (NULL == m_pScene)
	{
		return PERSISTID();
	}

	return m_pScene->GetID();
}

PERSISTID GameClient::GetSceneObj(const char* ident) const
{
	if (NULL == m_pScene)
	{
		return PERSISTID();
	}

	GameSceneObj* pObj = m_pScene->GetSceneObjByIdent(ident);

	if (NULL == pObj)
	{
		return PERSISTID();
	}

	return pObj->GetID();
}

PERSISTID GameClient::GetView(const char* view_ident) const
{
	GameView* pView = (GameView*)m_Views.GetObjectByIdent(view_ident);

	if (NULL == pView)
	{
		return PERSISTID();
	}

	return pView->GetID();
}

PERSISTID GameClient::GetViewObj(const char* view_ident, 
	const char* item_ident) const
{
	GameView* pView = (GameView*)m_Views.GetObjectByIdent(view_ident);

	if (NULL == pView)
	{
		return PERSISTID();
	}

	GameViewObj* pObj = pView->GetViewObjByIdent(item_ident);

	if (NULL == pObj)
	{
		return PERSISTID();
	}

	return pObj->GetID();
}

size_t GameClient::GetViewCount() const
{
	return m_Views.GetObjectCount();
}

void GameClient::GetViewList(const IVarList& args, IVarList& result)
{
	return m_Views.GetObjectList(result);
}

bool GameClient::ClearAll()
{
	SAFE_RELEASE(m_pScene);
	
	m_Views.Clear();
	
	return true;
}

