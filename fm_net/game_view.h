//--------------------------------------------------------------------
// 文件名:		game_view.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "game_object.h"
#include "game_object_set.h"

// 游戏容器视窗

class GameViewObj;

class GameView: public GameObject
{
public:
	GameView();
	virtual ~GameView();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 视窗容器容量
	void SetCapacity(int value);
	int GetCapacity() const;

	// 添加视窗中物件
	GameViewObj* AddViewObj(const char* item_ident);
	// 删除视窗中物件
	bool RemoveViewObj(const char* item_ident);
	// 移动视窗中物件
	bool ChangeViewObj(const char* old_item_ident, const char* new_item_ident);
	// 获得视窗中物件
	GameViewObj* GetViewObjByIdent(const char* item_ident) const;

	// 获得视窗中物件对象号
	virtual PERSISTID GetViewObj(const char* item_ident) const;
	// 获得视窗中物件数量
	virtual size_t GetViewObjCount() const;
	// 获得视窗中物体对象列表
	virtual void GetViewObjList(const IVarList& args, IVarList& result);
	
private:
	int m_nCapacity;
	CGameObjectSet m_Objects;
};

#endif // _GAME_VIEW_H

