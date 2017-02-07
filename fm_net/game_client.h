//--------------------------------------------------------------------
// 文件名:		game_client.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_CLIENT_H
#define _GAME_CLIENT_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../visual/i_game_client.h"
#include "game_object_set.h"

// 游戏客户端

class GameScene;
class GameSceneObj;
class GameView;

class GameClient: public IGameClient
{
public:
	GameClient();
	virtual ~GameClient();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 主角玩家对象号
	void SetPlayerIdent(const char* value);
	virtual const char* GetPlayerIdent() const;

	// 创建场景
	GameScene* CreateScene(const char* player_ident);
	// 删除场景
	bool DeleteScene();
	// 获得当前场景
	GameScene* GetCurrentScene() const;
	// 获得当前主角对象
	GameSceneObj* GetCurrentPlayer() const;
	// 创建视窗
	GameView* CreateView(const char* view_ident, int capacity);
	// 删除视窗
	bool DeleteView(const char* view_ident);
	// 获得指定标识视窗
	GameView* GetViewByIdent(const char* view_ident) const;

	// 是否玩家对象
	virtual bool IsPlayer(const char* ident) const;
	// 获得玩家对象
	virtual PERSISTID GetPlayer() const;
	// 获得场景对象
	virtual PERSISTID GetScene() const;
	// 获得场景内对象
	virtual PERSISTID GetSceneObj(const char* ident) const;
	// 获得视窗对象
	virtual PERSISTID GetView(const char* view_ident) const;
	// 获得视窗内对象
	virtual PERSISTID GetViewObj(const char* view_ident, 
		const char* item_ident) const;
	// 获得视窗数量
	virtual size_t GetViewCount() const;
	// 获得视窗列表
	virtual void GetViewList(const IVarList& args, IVarList& result);

	// 清除所有数据
	bool ClearAll();

private:
	core_string m_strPlayerIdent;
	GameScene* m_pScene;
	CGameObjectSet m_Views;
};

#endif // _GAME_CLIENT_H

