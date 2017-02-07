//--------------------------------------------------------------------
// 文件名:		ent_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ENT_MANAGER_H
#define _ENT_MANAGER_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// 实体管理

class IEntity;
class IEntCreator;
class CEntInfo;
class CEntInfoList;
class CCore;

class CEntManager
{
public:
	explicit CEntManager(CCore* pCore);
	~CEntManager();

	// 添加创建器
	bool AddCreator(IEntCreator* pCreator);

	// 设置所有的父类信息
	void SetAllParentInfo();

	// 获得数量
	size_t GetCount() const;
	// 获得实体信息
	CEntInfo* GetEntInfoByIndex(size_t index) const;
	// 查找实体信息
	CEntInfo* GetEntInfo(const char* name) const;

	// 添加到每帧运行队列
	bool AddFrameExecute(IEntity* pEntity);
	// 从每帧运行队列删除
	bool RemoveFrameExecute(IEntity* pEntity);
	// 每帧执行逻辑
	void ExecuteFrame(float seconds);
	
	// 导出所有实体信息到文件
	bool Dump(const char* file_name);

private:
	CEntManager();
	CEntManager(const CEntManager&);
	CEntManager& operator=(const CEntManager&);

private:
	CCore* m_pCore;
	bool m_bModified;
	TArrayPod<IEntity*, 1> m_FrameExecute;
	TArrayPod<IEntity*, 1> m_CurrentExecute;
	CEntInfoList* m_pEntInfoList;
};

#endif // _ENT_MANAGER_H
