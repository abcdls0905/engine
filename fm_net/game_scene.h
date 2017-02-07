//--------------------------------------------------------------------
// 文件名:		game_scene.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_SCENE_H
#define _GAME_SCENE_H

#include "game_object.h"
#include "game_object_set.h"

// 游戏场景

class GameSceneObj;

class GameScene: public GameObject
{
public:
	GameScene();
	virtual ~GameScene();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 添加场景中对象
	GameSceneObj* AddSceneObj(const char* object_ident);
	// 删除场景中对象
	bool RemoveSceneObj(const char* object_ident);
	// 获得场景中对象
	GameSceneObj* GetSceneObjByIdent(const char* object_ident) const;

	// 获得场景中对象号
	virtual PERSISTID GetSceneObj(const char* object_ident) const;
	// 获得场景中对象数量
	virtual size_t GetSceneObjCount() const;
    // 获得场景中对像通过序号
    virtual PERSISTID GetSceneObjByIndex(size_t index) const;
	// 获得场景中对象列表
	virtual void GetSceneObjList(const IVarList& args, IVarList& result);

private:
	CGameObjectSet m_Objects;
};

#endif // _GAME_SCENE_H

