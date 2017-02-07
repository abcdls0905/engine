//--------------------------------------------------------------------
// 文件名:		game_object_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_OBJECT_SET_H
#define _GAME_OBJECT_SET_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "game_object.h"

// 游戏对象容器

class GameObject;

class CGameObjectSet
{
public:
	CGameObjectSet();
	~CGameObjectSet();

	// 查找
	bool Find(const char* ident) const;
	// 添加
	bool Add(const char* ident, GameObject* object);
	// 删除
	bool Remove(const char* ident);
	// 改变标识符
	bool Change(const char* old_ident, const char* new_ident);
	// 清空
	void Clear();
	// 获取对象
	GameObject* GetObjectByIdent(const char* ident) const;
	// 获取对象号
	PERSISTID GetObjectID(const char* ident) const;
	// 获得数量
	size_t GetObjectCount() const;
    // 获取对像通过序号
    PERSISTID GetObjectIDByIndex(const size_t index) const;
	// 获得对象列表
	void GetObjectList(IVarList& result);

private:
	CGameObjectSet(const CGameObjectSet&);
	CGameObjectSet& operator=(const CGameObjectSet&);

	bool FindIndex(const char* ident, size_t& index) const;
	
private:
	TArrayPod<GameObject*, 1, TCoreAlloc> m_Objects;
};

#endif // _GAME_OBJECT_SET_H

