//--------------------------------------------------------------------
// 文件名:		game_scene.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_scene.h"
#include "game_scene_obj.h"
#include "../public/var_list.h"

/// \file game_scene.cpp
/// \brief 游戏场景

/// entity: GameScene
/// \brief 游戏场景实体
/// \see 继承自GameObject
DECLARE_ENTITY(GameScene, NULL, GameObject);

/*
/// method: object GetSceneObj(string object_ident)
/// \brief 获得指定标识的场景对象
/// \param object_ident 对象标识
DECLARE_METHOD_1(PERSISTID, GameScene, GetSceneObj, const char*);
/// method: table GetSceneObjList()
/// \brief 获得视窗物件列表
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

// 获得场景中对像通过序号
PERSISTID GameScene::GetSceneObjByIndex(size_t index) const
{

    return m_Objects.GetObjectIDByIndex(index);
}

void GameScene::GetSceneObjList(const IVarList& args, IVarList& result)
{
	return m_Objects.GetObjectList(result);
}

