//--------------------------------------------------------------------
// 文件名:		model_system.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月1日
// 创建人:		邢岳
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MODEL_SYSTEM_H
#define _MODEL_SYSTEM_H

#include "../visual/i_model_system.h"

// 模型系统

class IRender;
class IActionPlayer;
class CResManager;

class ModelSystem: public IModelSystem
{
public:
	ModelSystem();
	virtual ~ModelSystem();

	virtual bool Init();
	virtual bool Shut();

	// 是否检查模型存在无效数据
	virtual void SetCheckModelData(bool value);
	virtual bool GetCheckModelData();

	// 是否显示模型节点的包围盒
	virtual void SetShowNodeBoundingBox(bool value);
	virtual bool GetShowNodeBoundingBox();
	
	// 渲染包围盒
	virtual void DrawBoundingBox(const FmVec3* min, 
		const FmVec3* max, unsigned int cr_start, unsigned int cr_end, 
		float scale);
	// 渲染一个辅助点
	virtual void DrawPoint(const FmVec3* position, unsigned int cr, 
		float size, bool balance);

	// 创建模型播放器
	virtual IModelPlayer* CreateModelPlayer(const char* name, double beg_time,
		const char* tex_paths, bool async);
	// 创建动作播放器
	virtual IActionPlayer* CreateActionPlayer(const char* name, 
		const char* append_path, const char* def_action, void* trigger_proc, 
		void* user_pointer, bool async);

    // 创建静态模型
    virtual CResModel* CreateResModel(const char* name, const char* tex_paths, bool async);
    // 释放静态模型
    virtual void ReleaseResModel(CResModel* pResModel);

    // 创建骨骼动画
    virtual CSkeleton* CreateSkeleton(const char* name, bool async);
    // 释放骨骼动画
    virtual void ReleaseSkeleton(CSkeleton* pSkeleton);

    // 创建动作集
    virtual CActionSet* CreateActionSet(const char* name, const char* append_path, bool async);
    // 释放动作集
    virtual void ReleaseActionSet(CActionSet* pActionSet);

	// 导出所有的资源信息
	virtual bool DumpResource(const char* file_name);
	// 回收长时间不使用的动作数据
	virtual int CollectUnuseAction(int seconds);

	// 获得渲染器
	IRender* GetRender() const { return m_pRender; }
	// 获得资源管理器
	CResManager* GetResManager() const { return m_pResManager; }

private:
	IRender* m_pRender;
	CResManager* m_pResManager;
};

#endif // _MODEL_SYSTEM_H
