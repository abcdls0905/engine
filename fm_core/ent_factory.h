//--------------------------------------------------------------------
// 文件名:		ent_factory.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ENT_FACTORY_H
#define _ENT_FACTORY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/i_var_list.h"
#include "../public/i_entity.h"
#include "../utils/array_pod.h"

// 实体工厂

class CCore;
class CHookManager;
class CEntManager;
class IEntCreator;

class CEntFactory
{
private:
	struct entity_t
	{
		PERSISTID EntityId;
		IEntity* pEntity[2];

		entity_t()
		{
			pEntity[0] = NULL;
			pEntity[1] = NULL;
		}
	};

public:
	explicit CEntFactory(CCore* pCore, CEntManager* pEntManager, 
		CHookManager* pHookManager);
	~CEntFactory();

	// 创建实体
	IEntity* Create(const char* name, const IVarList& args);
	// 销毁实体
	void Destroy(IEntity* pEntity);
	// 将实体标记为删除
	void SetDeleted(IEntity* pEntity);
	
	// 添加窗口消息处理
	bool AddMsgProc(IEntity* pEntity, unsigned int msg, int prior, 
		bool instant);
	// 移除窗口消息处理
	bool RemoveMsgProc(IEntity* pEntity, unsigned int msg);

	// 根据对象ID获得实体
	IEntity* Get(const PERSISTID& id) const
	{
		// 省去边界判断
		size_t index = id.nIdent & m_nBoundSize;
		bool not_equal = m_Entities[index].EntityId != id;
		return m_Entities[index].pEntity[size_t(not_equal)];
	}

	// 获得所有实体
	size_t GetAll(IVarList& result) const;
	// 获得实体数量
	size_t GetCount() const { return m_nCount; }
	// 查找名字符合的实体
	IEntity* Find(const char* name) const;
	size_t FindMore(const char* name, IVarList& result) const;
	// 销毁标记为删除的实体
	void ClearDeleted();
	// 全部删除
	void ReleaseAll();
	// 导出所有实体信息到文件
	bool Dump(const char* file_name);
	
private:
	CEntFactory();
	CEntFactory(const CEntFactory&);
	CEntFactory& operator=(const CEntFactory&);

	// 删除实体
	void Delete(IEntity* pEntity);
	// 生成序列号
	unsigned int GenSerial();

private:
	CCore* m_pCore;
	CEntManager* m_pEntManager;
	CHookManager* m_pHookManager;
	TArrayPod<entity_t, 1> m_Entities;
	TArrayPod<unsigned int, 1> m_FreeList;
	TArrayPod<IEntity*, 1> m_Deletes;
	unsigned int m_nSerial;
	size_t m_nCount;
	size_t m_nUsedSize;
	size_t m_nBoundSize;
};

#endif // _ENT_FACTORY_H
