//--------------------------------------------------------------------
// 文件名:		game_view.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_view.h"
#include "game_view_obj.h"
#include "../public/var_list.h"

/// \file game_view.cpp
/// \brief 游戏容器视窗

/// entity: GameView
/// \brief 游戏容器视窗实体
/// \see 继承自GameObject
DECLARE_ENTITY(GameView, NULL, GameObject);

/*
/// readonly: int Capacity
/// \brief 容器容量
DECLARE_PROPERTY_GET(int, GameView, Capacity, GetCapacity);

/// method: object GetViewObj(string item_ident)
/// \brief 获得指定标识的视窗物件
/// \param item_ident 物件标识
DECLARE_METHOD_1(PERSISTID, GameView, GetViewObj, const char*);
/// method: table GetViewObjList()
/// \brief 获得视窗物件列表
DECLARE_METHOD_T(GameView, GetViewObjList);*/

// GameView

GameView::GameView()
{
	m_nCapacity = 0;
}

GameView::~GameView()
{
}

bool GameView::Init(const IVarList& args)
{
	if (!GameObject::Init(args))
	{
		return false;
	}
	
	return true;
}

bool GameView::Shut()
{
	m_Objects.Clear();

	return GameObject::Shut();
}

void GameView::SetCapacity(int value)
{
	m_nCapacity = value;
}

int GameView::GetCapacity() const
{
	return m_nCapacity;
}

GameViewObj* GameView::AddViewObj(const char* item_ident)
{
	GameViewObj* pViewObj = (GameViewObj*)GetCore()->CreateEntity("GameViewObj");
	m_Objects.Add(item_ident, pViewObj);
	return pViewObj;
}

bool GameView::RemoveViewObj(const char* item_ident)
{
	return m_Objects.Remove(item_ident);
}

bool GameView::ChangeViewObj(const char* old_item_ident, 
	const char* new_item_ident)
{
	return m_Objects.Change(old_item_ident, new_item_ident);
}

GameViewObj* GameView::GetViewObjByIdent(const char* item_ident) const
{
	return (GameViewObj*)m_Objects.GetObjectByIdent(item_ident);
}

PERSISTID GameView::GetViewObj(const char* item_ident) const
{
	return m_Objects.GetObjectID(item_ident);
}

size_t GameView::GetViewObjCount() const
{
	return m_Objects.GetObjectCount();
}

void GameView::GetViewObjList(const IVarList& args, IVarList& result)
{
	return m_Objects.GetObjectList(result);
}

