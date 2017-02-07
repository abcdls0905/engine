//--------------------------------------------------------------------
// 文件名:		game_view_obj.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_VIEW_OBJ_H
#define _GAME_VIEW_OBJ_H

#include "game_object.h"

// 容器视窗中的对象

class GameViewObj: public GameObject
{
public:
	GameViewObj();
	virtual ~GameViewObj();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
};

#endif // _GAME_VIEW_OBJ_H

