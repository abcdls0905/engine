//--------------------------------------------------------------------
// �ļ���:		game_view_obj.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_view_obj.h"
#include "../public/inlines.h"

/// \file game_view_obj.cpp
/// \brief �����Ӵ��еĶ���

/// entity: GameViewObj
/// \brief �����Ӵ��еĶ���ʵ��
/// \see �̳���GameObject
DECLARE_ENTITY(GameViewObj, NULL, GameObject);

// GameViewObj

GameViewObj::GameViewObj()
{
}

GameViewObj::~GameViewObj()
{
}

bool GameViewObj::Init(const IVarList& args)
{
	if (!GameObject::Init(args))
	{
		return false;
	}
	
	return true;
}

bool GameViewObj::Shut()
{
	return GameObject::Shut();
}

