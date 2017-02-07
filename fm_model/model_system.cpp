//--------------------------------------------------------------------
// 文件名:		model_system.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年1月1日
// 创建人:		邢岳
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_system.h"
#include "res_manager.h"
#include "res_model.h"
#include "action_set.h"
#include "model_player.h"
#include "action_player.h"
#include "aux_tool.h"
#include "math_3d.h"
#include "../visual/i_render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

//DECLARE_MODULE("");

/*
// 导出内存分配情况
#ifdef _DEBUG 

#include "../utils/leak_checker.h"

extern CLeakChecker g_LeakChecker;

// 导出DLL模块的内存使用情况
static bool ext_dump_memory_fx_modeladv()
{
	g_LeakChecker.DumpMemory();

	return true;
}

// 在脚本中可调用此扩展函数获得内存导出文件
//DECLARE_FUNCTION_0(bool, ext_dump_memory_fx_modeladv);

#endif // _DEBUG
*/

DECLARE_FUNCTION_INTERFACE(ModelSystem);

IRender* g_pRender = NULL;
// 检查模型数据
bool g_bCheckModelData = false;
// 显示模型节点的包围盒
bool g_bShowNodeBoundingBox = false;

void shader_initialize();
void shader_release();
// 初始化材质键名索引表
void init_material_key_index();
// 初始化节点键名索引表
void init_node_key_index();

ModelSystem::ModelSystem()
{
	m_pRender = NULL;
	m_pResManager = CORE_NEW(CResManager);
	m_pResManager->Initialize(this);
}

ModelSystem::~ModelSystem()
{
	shader_release();
	aux_release();
	CORE_DELETE(m_pResManager);
}

bool ModelSystem::Init()
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	CResManagerUpdate* pResManagerUpdate = (CResManagerUpdate*)GetCore()->CreateEntity("CResManagerUpdate");
	pResManagerUpdate->res_manager = m_pResManager;

	if (NULL == m_pRender)
	{
		CORE_TRACE("(ModelSystem::Init)no DxRender");
		return false;
	}

	g_pRender = m_pRender;

	aux_initialize();
	shader_initialize();
	init_node_key_index();
	init_material_key_index();

	return true;
}

bool ModelSystem::Shut()
{
	if (m_pRender)
	{
		// 释放所有贴图以保证解除对DxRender的引用
		m_pResManager->ReleaseAllTextures();
	}

	SAFE_RELEASE(m_pRender);
	
	return true;
}

void ModelSystem::SetCheckModelData(bool value)
{
	g_bCheckModelData = value;
}

bool ModelSystem::GetCheckModelData()
{
	return g_bCheckModelData;
}

void ModelSystem::SetShowNodeBoundingBox(bool value)
{
	g_bShowNodeBoundingBox = value;
}

bool ModelSystem::GetShowNodeBoundingBox()
{
	return g_bShowNodeBoundingBox;
}

void ModelSystem::DrawBoundingBox(const FmVec3* min, 
	const FmVec3* max, unsigned int cr_start, unsigned int cr_end, 
	float scale)
{
	return aux_draw_bound_box(min, max, cr_start, cr_end, scale);
}

void ModelSystem::DrawPoint(const FmVec3* position, unsigned int cr, 
	float size, bool balance)
{
	return aux_draw_point(position, cr, size, balance);
}

IModelPlayer* ModelSystem::CreateModelPlayer(const char* name, 
	double beg_time, const char* tex_paths, bool async)
{
	CResModel* pResModel = m_pResManager->CreateResModel(name, tex_paths, 
		async);

	if (NULL == pResModel)
	{
		return NULL;
	}

	CModelPlayer* pModelPlayer = CModelPlayer::NewInstance(m_pRender, 
		pResModel);

	pModelPlayer->SetName(name);
	pModelPlayer->SetBeginTime(beg_time);

	if (pResModel->IsLoadComplete())
	{
		pModelPlayer->Create();
	}
	
	return pModelPlayer;
}

IActionPlayer* ModelSystem::CreateActionPlayer(const char* name, 
	const char* append_path, const char* def_action, void* trigger_proc, 
	void* user_pointer, bool async)
{
	CActionSet* pActionSet = m_pResManager->CreateActionSet(name, 
		append_path, async);

	if (NULL == pActionSet)
	{
		return NULL;
	}

	CActionPlayer* pActionPlayer = CActionPlayer::NewInstance( 
		pActionSet);

	pActionPlayer->SetAsyncLoad(async);
	pActionPlayer->SetDefaultAction(def_action);
	pActionPlayer->RegistActionTriggerProc((ACTION_TRIGGER_PROC)trigger_proc);
	pActionPlayer->SetUserPointer(user_pointer);

	if (pActionSet->IsLoadComplete())
	{
		pActionPlayer->Create();
	}

	return pActionPlayer;
}

// 创建静态模型
CResModel* ModelSystem::CreateResModel(const char* name, const char* tex_paths, bool async)
{
    return m_pResManager->CreateResModel(name, tex_paths, async);
}

// 释放静态模型
void ModelSystem::ReleaseResModel(CResModel* pResModel)
{
    m_pResManager->ReleaseResModel(pResModel);
}

// 创建骨骼动画
CSkeleton* ModelSystem::CreateSkeleton(const char* name, bool async)
{
    return m_pResManager->CreateSkeleton(name, async);
}

// 释放骨骼动画
void ModelSystem::ReleaseSkeleton(CSkeleton* pSkeleton)
{
    m_pResManager->ReleaseSkeleton(pSkeleton);
}

// 创建动作集
CActionSet* ModelSystem::CreateActionSet(const char* name, const char* append_path, bool async)
{
    return m_pResManager->CreateActionSet(name, append_path, async);
}

// 释放动作集
void ModelSystem::ReleaseActionSet(CActionSet* pActionSet)
{
    m_pResManager->ReleaseActionSet(pActionSet);
}

bool ModelSystem::DumpResource(const char* file_name)
{
	return m_pResManager->DumpAll(file_name);
}

int ModelSystem::CollectUnuseAction(int seconds)
{
	return m_pResManager->CollectUnuseAction(seconds);
}
