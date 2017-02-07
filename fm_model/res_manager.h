//--------------------------------------------------------------------
// 文件名:		res_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RES_MANAGER_H
#define _RES_MANAGER_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"
#include "../public/i_entity.h"

class TResManagerAlloc
{
public:
	TResManagerAlloc() {}
	~TResManagerAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TResManagerAlloc& src) {}
};

// 资源管理

class CResModel;
class CSkeleton;
class CActionSet;
class ModelSystem;
struct skeleton_t;

class CResManager
{
private:
	typedef TStringPod<char, CResModel*, TStringTraitsCI<char>, 
		TResManagerAlloc> model_container_t;
	typedef TStringPod<char, CSkeleton*, TStringTraitsCI<char>, 
		TResManagerAlloc> skeleton_container_t;
	typedef TStringPod<char, CActionSet*, TStringTraitsCI<char>, 
		TResManagerAlloc> action_set_container_t;

public:
	CResManager();
	~CResManager();

	// 初始化
	bool Initialize(ModelSystem* pModelSystem);
	// 释放所有贴图
	bool ReleaseAllTextures();

	// 创建静态模型
	CResModel* CreateResModel(const char* name, const char* tex_paths, 
		bool async);
	// 释放静态模型
	bool ReleaseResModel(CResModel* pResModel);
	
	// 根据骨骼动画数据获得资源对象
	CSkeleton* GetSkeleton(const char* name, skeleton_t* pSkeleton);
	// 创建骨骼动画
	CSkeleton* CreateSkeleton(const char* name, bool async);
	// 释放骨骼动画
	bool ReleaseSkeleton(CSkeleton* pSkeleton);

	// 创建动作集
	CActionSet* CreateActionSet(const char* name, const char* append_path, 
		bool async);
	// 释放动作集
	bool ReleaseActionSet(CActionSet* pActionSet);
	// 回收长时间不使用的动作数据
	int CollectUnuseAction(int seconds);

	// 导出所有的资源信息
	bool DumpAll(const char* file_name);

	// 初始化（可创建其他附属实体）
	virtual bool Init(const IVarList& args);
	// 关闭（可删除其他引用的附属实体，而资源释放应在析构函数中进行）
	virtual bool Shut();

	// 每帧执行逻辑（如果已添加到运行队列）
	virtual void Execute(float seconds);
private:
	CResManager(const CResManager&);
	CResManager& operator=(const CResManager&);	

private:
	ModelSystem* m_pModelSystem;
	model_container_t m_ResModels;
	skeleton_container_t m_Skeletons;
	action_set_container_t m_ActionSets;
};

class CResManagerUpdate : public IEntity
{
public:
	CResManager* res_manager;
public:
	virtual bool Init(const IVarList& args);
	virtual bool Shut(){ return true;}
	virtual void Execute(float seconds);
};

#endif // _RES_MANAGER_H
