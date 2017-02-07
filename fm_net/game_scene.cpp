//--------------------------------------------------------------------
// �ļ���:		game_scene.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_scene.h"
#include "game_scene_obj.h"
#include "../public/var_list.h"

/// \file game_scene.cpp
/// \brief ��Ϸ����

/// entity: GameScene
/// \brief ��Ϸ����ʵ��
/// \see �̳���GameObject
DECLARE_ENTITY(GameScene, NULL, GameObject);

/*
/// method: object GetSceneObj(string object_ident)
/// \brief ���ָ����ʶ�ĳ�������
/// \param object_ident �����ʶ
DECLARE_METHOD_1(PERSISTID, GameScene, GetSceneObj, const char*);
/// method: table GetSceneObjList()
/// \brief ����Ӵ�����б�
DECLARE_METHOD_T(GameScene, GetSceneObjList);*/

// GameScene

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

bool GameScene::Init(const IVarList& args)
{
	if (!GameObject::Init(args))
	{
		return false;
	}
	
	return true;
}

bool GameScene::Shut()
{
	m_Objects.Clear();
	
	return GameObject::Shut();
}

GameSceneObj* GameScene::AddSceneObj(const char* object_ident)
{
	GameSceneObj* pSceneObj = (GameSceneObj*)
		GetCore()->CreateEntity("GameSceneObj");

	m_Objects.Add(object_ident, pSceneObj);

	return pSceneObj;
}

bool GameScene::RemoveSceneObj(const char* object_ident)
{
	return m_Objects.Remove(object_ident);
}

GameSceneObj* GameScene::GetSceneObjByIdent(const char* object_ident) const
{
	return (GameSceneObj*)m_Objects.GetObjectByIdent(object_ident);
}

PERSISTID GameScene::GetSceneObj(const char* object_ident) const
{
	return m_Objects.GetObjectID(object_ident);
}

size_t GameScene::GetSceneObjCount() const
{
	return m_Objects.GetObjectCount();
}

// ��ó����ж���ͨ�����
PERSISTID GameScene::GetSceneObjByIndex(size_t index) const
{

    return m_Objects.GetObjectIDByIndex(index);
}

void GameScene::GetSceneObjList(const IVarList& args, IVarList& result)
{
	return m_Objects.GetObjectList(result);
}

