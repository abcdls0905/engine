//--------------------------------------------------------------------
// �ļ���:		game_view.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_view.h"
#include "game_view_obj.h"
#include "../public/var_list.h"

/// \file game_view.cpp
/// \brief ��Ϸ�����Ӵ�

/// entity: GameView
/// \brief ��Ϸ�����Ӵ�ʵ��
/// \see �̳���GameObject
DECLARE_ENTITY(GameView, NULL, GameObject);

/*
/// readonly: int Capacity
/// \brief ��������
DECLARE_PROPERTY_GET(int, GameView, Capacity, GetCapacity);

/// method: object GetViewObj(string item_ident)
/// \brief ���ָ����ʶ���Ӵ����
/// \param item_ident �����ʶ
DECLARE_METHOD_1(PERSISTID, GameView, GetViewObj, const char*);
/// method: table GetViewObjList()
/// \brief ����Ӵ�����б�
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

