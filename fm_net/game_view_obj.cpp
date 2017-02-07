//--------------------------------------------------------------------
// 文件名:		game_view_obj.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "game_view_obj.h"
#include "../public/inlines.h"

/// \file game_view_obj.cpp
/// \brief 容器视窗中的对象

/// entity: GameViewObj
/// \brief 容器视窗中的对象实体
/// \see 继承自GameObject
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

