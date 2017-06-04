
#ifndef	_VISUAL_I_MODEL_SYSTEM_H
#define	_VISUAL_I_MODEL_SYSTEM_H

#include "../public/module.h"
#include "../math/fm_math.h"

#define IMODEL_ADV_NODERADIAN180

// 节点类型
enum FXNODE_TYPE
{
	FXNODE_TYPE_INVALID = 0x00000000,
	FXNODE_TYPE_MESH = 0x00000001,
	FXNODE_TYPE_SKIN = 0x00000002,
	FXNODE_TYPE_HELPER = 0x00000003,
	FXNODE_TYPE_BONE = 0x00000004,
	FXNODE_TYPE_CAMERA = 0x00000005,
};

// 动作触发回调函数
typedef bool (*ACTION_TRIGGER_PROC)(void* user_pointer,
	unsigned int nActionIndex, const char* pszTriggerName, 
	unsigned int nFrame);

// 模型系统接口

class IModelPlayer;
class IActionPlayer;
class CResModel;
class CSkeleton;
class CActionSet;

class IModelSystem: public IInterface
{
public:
	// 指定是否检查模型存在无效数据
	virtual void SetCheckModelData(bool value) = 0;
	// 判断当前是否检查模型存在无效数据
	virtual bool GetCheckModelData() = 0;
	
	// 是否显示模型节点的包围盒
	virtual void SetShowNodeBoundingBox(bool value) = 0;
	virtual bool GetShowNodeBoundingBox() = 0;

	// 渲染包围盒
	virtual void DrawBoundingBox(const FmVec3* min, 
		const FmVec3* max, unsigned int cr_start, unsigned int cr_end, 
		float scale) = 0;
	// 渲染一个辅助点
	virtual void DrawPoint(const FmVec3* position, unsigned int cr, 
		float size, bool balance) = 0;
	
	// 创建模型播放器
	virtual IModelPlayer* CreateModelPlayer(const char* name, double beg_time,
		const char* tex_paths,	bool async) = 0;
	// 创建动作播放器
	virtual IActionPlayer* CreateActionPlayer(const char* name, 
		const char* append_path, const char* def_action, void* trigger_proc, 
		void* user_pointer, bool async) = 0;
	
	// 导出所有的资源信息
	virtual bool DumpResource(const char* file_name) = 0;
	// 回收长时间不使用的动作数据
	virtual int CollectUnuseAction(int seconds) = 0;

    /* 
        以下接口函数用于对文件资源的预加载
    */

    // 创建静态模型
    virtual CResModel* CreateResModel(const char* name, const char* tex_paths, bool async) = 0;
    // 释放静态模型
    virtual void ReleaseResModel(CResModel* pResModel) = 0;

    // 创建骨骼动画
    virtual CSkeleton* CreateSkeleton(const char* name, bool async) = 0;
    // 释放骨骼动画
    virtual void ReleaseSkeleton(CSkeleton* pSkeleton) = 0;

    // 创建动作集
    virtual CActionSet* CreateActionSet(const char* name, const char* append_path, bool async) = 0;
    // 释放动作集
    virtual void ReleaseActionSet(CActionSet* pActionSet) = 0;

    /* 
        以上接口函数用于对文件资源的预加载
    */
};

#endif // _VISUAL_I_MODEL_SYSTEM_H
