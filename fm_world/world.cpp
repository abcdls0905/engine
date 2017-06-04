//--------------------------------------------------------------------
// 文件名:		world.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "world.h"
#include "scene.h"
#include "render_tex.h"
#include "../visual/vis_utils.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_texture.h"
#include "../public/var_list.h"
#include "../public/core_log.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../fm_gui/gui.h"

/// \file world.cpp
/// \brief 世界主窗口

/// entity: World
/// \brief 世界主窗口实体
DECLARE_ENTITY(World, 0, IEntity);

/// readonly: DeviceName
/// \brief 显示设备名称
DECLARE_PROPERTY_GET(const char*, World, DeviceName, GetDeviceName);


/// property: object MainGui
/// \brief 设备能力
DECLARE_PROPERTY_GET(PERSISTID, World, DeviceCapsID, GetDeviceCapsID);


/// property: object MainGui
/// \brief 主界面对象
DECLARE_PROPERTY(PERSISTID, World, MainGui, GetMainGuiID, SetMainGuiID);
/// property: object MainScene
/// \brief 主场景对象
DECLARE_PROPERTY(PERSISTID, World, MainScene, GetMainSceneID, SetMainSceneID);
/// property: int Width
/// \brief 窗口宽度
DECLARE_PROPERTY(int, World, Width, GetWidth, SetWidth);
/// property: int Height
/// \brief 窗口高度
DECLARE_PROPERTY(int, World, Height, GetHeight, SetHeight);
/// property: bool Anisotropic
/// \brief 是否开启各向异性过滤
DECLARE_PROPERTY(bool, World, Anisotropic, GetAnisotropic, SetAnisotropic);
/// property: int MaxAnisotropy
/// \brief 最大各向异性过滤
DECLARE_PROPERTY(int, World, MaxAnisotropy, GetMaxAnisotropy, 
	SetMaxAnisotropy);
/// property: int MultiSamleType
/// \brief 抗锯齿
DECLARE_PROPERTY(int, World, MultiSampleType, GetMultiSampleType, 
	SetMultiSampleType);
/// property: bool WireFrame
/// \brief 是否显示多边形线框
DECLARE_PROPERTY(bool, World, WireFrame, GetWireFrame, SetWireFrame);
/// property: string BackColor
/// \brief 背景颜色
DECLARE_PROPERTY(result_string, World, BackColor, GetBackColor, SetBackColor);
/// property: int TraceAlphaRef
/// \brief 碰撞检测时的ALPHA测试参考值，为0表示不测试
DECLARE_PROPERTY(int, World, TraceAlphaRef, GetTraceAlphaRef, 
	SetTraceAlphaRef);
/// property: int RenderAlphaRef
/// \brief 渲染时的ALPHA测试参考值
DECLARE_PROPERTY(int, World, RenderAlphaRef, GetRenderAlphaRef, 
	SetRenderAlphaRef);

/// property: bool UseLightMap
/// \brief 是否使用光贴图
DECLARE_PROPERTY(bool, World, UseLightMap, GetUseLightMap, 
	SetUseLightMap);

/// property: int TextureLod
/// \brief 贴图精度(0-8)
DECLARE_PROPERTY(int, World, TextureLod, GetTextureLod, SetTextureLod);
/// property: int TraceAloneTexture
/// \brief 使用独立的附加贴图数据进行碰撞检测（从而避免锁定正在渲染的贴图）
DECLARE_PROPERTY(int, World, TraceAloneTexture, GetTraceAloneTexture, 
	SetTraceAloneTexture);
/// property: int Sleep
/// \brief 程序主循环休眠时间(毫秒)
DECLARE_PROPERTY(int, World, Sleep, GetSleep, SetSleep);
/// property: int ExecuteFPS
/// \brief 执行逻辑最大帧速
DECLARE_PROPERTY(int, World, ExecuteFPS, GetExecuteFPS, SetExecuteFPS);
/// property: int ResourceThreadNum
/// \brief 资源加载线程数量（1-8）
DECLARE_PROPERTY(int, World, ResourceThreadNum, GetResourceThreadNum, 
	SetResourceThreadNum);
/// property: bool AsyncLoadResource
/// \brief 是否异步加载资源
DECLARE_PROPERTY(bool, World, AsyncLoadResource, GetAsyncLoadResource, 
	SetAsyncLoadResource);
/// property: bool BlackWhiteDiffuse
/// \brief 是否使用黑白格子漫反射贴图
DECLARE_PROPERTY(bool, World, BlackWhiteDiffuse, GetBlackWhiteDiffuse, 
	SetBlackWhiteDiffuse);
/// property: bool TraceTreeLeaf
/// \brief 是否开启树叶碰撞
DECLARE_PROPERTY(bool, World, TraceTreeLeaf, GetTraceTreeLeaf, 
	SetTraceTreeLeaf);

#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime
// World

bool World::DeviceErrorHook(void* context, int thread_id, int error, 
	const char* info)
{
	World* pthis = (World*)context;
	
	/*
	if (::TestErr("World::DeviceErrorHook)No enough video memory!"))
	{
		CORE_TRACE("(World::DeviceErrorHook)No enough video memory!");
		// 显示错误提示框
		//MessageBoxA(NULL, "No enough video memory! Program exit!", info, 
		//	MB_OK | MB_ICONSTOP);
		// 终止程序
		abort();
		return true;
	}
	*/
	
	return false;
}

World::World()
{
	m_pRender = NULL;
	m_pModelSystem = NULL;
	m_pMainGui = NULL;
	m_pMainScene = NULL;
	m_pRenderTex = NULL;
	m_nBackColor = 0x00000000;
	m_nWidth = 800;
	m_nHeight = 600;
	m_bAnisotropic = false;
	m_nMultiSampleType = 1;
	m_nMaxAnisotropy = 1;
}

World::~World()
{
}

bool World::Init(const IVarList& args)
{
	GetCore()->AddExecute(this);
	
	return true;
}

bool World::Shut()
{
	if (m_pRender)
	{
		m_pRender->SetDeviceErrorCallback(NULL, NULL);
	}

	render_tex_t* p = m_pRenderTex;

	while (p)
	{
		render_tex_t* t = p;

		p = p->pNext;
		t->pRenderTex->Release();
		CORE_FREE(t, sizeof(render_tex_t));
	}

	m_pRenderTex = NULL;
	SAFE_RELEASE(m_pMainScene);
	SAFE_RELEASE(m_pMainGui);
	//SAFE_RELEASE(m_pModelSystem);
	SAFE_RELEASE(m_pRender);

	return true;
}

void World::Execute(float seconds)
{
	if (NULL == m_pRender)
	{
		return;
	}

#ifdef PERFORMANCE_DEBUG
	double dUpdateTime = performance_time();
#endif

	// 更新渲染到贴图
	if (m_pRenderTex)
	{
		render_tex_t* p = m_pRenderTex;

		while (p)
		{
			p->pRenderTex->Update(seconds);
			p = p->pNext;
		}
	}

	// 更新场景
	if (m_pMainScene)
	{
		m_pMainScene->Update(seconds);
	}

	// 更新界面
	if (m_pMainGui)
	{
		m_pMainGui->Update(seconds);
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dUpdateTime += 
		performance_time() - dUpdateTime;
#endif
}

void World::Display(float offset_seconds)
{
#ifdef PERFORMANCE_DEBUG
	double dRealizeTime = performance_time();
#endif

	// 渲染到贴图
	if (m_pRenderTex)
	{
		render_tex_t* p = m_pRenderTex;

		while (p)
		{
			p->pRenderTex->Realize();
			p = p->pNext;
		}
	}

	// 先渲染场景
	if (m_pMainScene)
	{
		m_pMainScene->Realize();
	}

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->dRealizeTime += 
		performance_time() - dRealizeTime;
#endif

	

	  // 最后渲染界面
	  if (m_pMainGui)
	  {
  #ifdef PERFORMANCE_DEBUG
		  double dGuiRealizeTime = performance_time();
  #endif

		  m_pMainGui->Realize();

  #ifdef PERFORMANCE_DEBUG
		  m_pRender->GetPerformance()->dGuiRealizeTime += 
			  performance_time() - dGuiRealizeTime;
  #endif
	  }

	m_pRender->BeginFrame(m_nBackColor);
	m_pRender->EndFrame();
}

const char* World::GetDeviceName() const
{
	if (NULL == m_pRender)
	{
		return "";
	}

	return m_pRender->GetDeviceName();
}

void World::SetMainGuiID(const PERSISTID& id)
{
	if (id.IsNull())
	{
		m_pMainGui = NULL;
		return;
	}
	
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return;
	}

	m_pMainGui = (Gui*)pEntity;
}

PERSISTID World::GetMainGuiID() const
{
	if (NULL == m_pMainGui)
	{
		return PERSISTID();
	}
	
	return m_pMainGui->GetID();
}

void World::SetMainSceneID(const PERSISTID& id)
{
	if (id.IsNull())
	{
		m_pMainScene = NULL;
		return;
	}
	
	IEntity* pEntity = GetCore()->GetEntity(id);
	
	if (NULL == pEntity)
	{
		return;
	}

	m_pMainScene = (Scene*)pEntity;
}

PERSISTID World::GetMainSceneID() const
{
	if (NULL == m_pMainScene)
	{
		return PERSISTID();
	}

	return m_pMainScene->GetID();
}

bool World::AddRenderTex(const PERSISTID& id)
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return false;
	}

	render_tex_t* p = (render_tex_t*)CORE_ALLOC(sizeof(render_tex_t));

	p->pRenderTex = (RenderTex*)pEntity;
	p->pNext = m_pRenderTex;

	m_pRenderTex = p;
	
	return true;
}

bool World::RemoveRenderTex(const PERSISTID& id)
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return false;
	}

	render_tex_t* p = m_pRenderTex;
	render_tex_t* prev = NULL;

	while (p)
	{
		render_tex_t* t = p;

		prev = p;
		p = p->pNext;

		if (t->pRenderTex == (RenderTex*)pEntity)
		{
			if (NULL == prev)
			{
				m_pRenderTex = t->pNext;
			}
			else
			{
				prev->pNext = t->pNext;
			}

			CORE_FREE(t, sizeof(render_tex_t));
			return true;
		}
	}
	
	return false;
}

void World::SetWidth(int value)
{
	m_nWidth = value;
}

int World::GetWidth() const
{
	return m_nWidth;
}

void World::SetHeight(int value)
{
	m_nHeight = value;
}

int World::GetHeight() const
{
	return m_nHeight;
}

void World::SetAnisotropic(bool value)
{
	m_bAnisotropic = value;

	if (m_pRender)
	{
		m_pRender->SetAnisotropic(value);
	}
}

bool World::GetAnisotropic() const
{
	return m_bAnisotropic;
}

void World::SetMaxAnisotropy(int value)
{
	if ((value < 0) || (value > 16))
	{
		return;
	}

	m_nMaxAnisotropy = value;

	if (m_pRender)
	{
		m_pRender->SetMaxAnisotropy(value);
	}
}

int World::GetMaxAnisotropy() const
{
	return m_nMaxAnisotropy;
}

void World::SetMultiSampleType(int value)
{
	m_nMultiSampleType = value;
}

int World::GetMultiSampleType() const
{
	return m_nMultiSampleType;
}

void World::SetWireFrame(bool value)
{
	m_pRender->SetWireFrame(value);
}

bool World::GetWireFrame() const
{
	return m_pRender->GetWireFrame();
}

void World::SetBackColor(const char* value)
{
	m_nBackColor = VisUtil_ColorToInt(value);
}

result_string World::GetBackColor() const
{
	return VisUtil_IntToColor(m_nBackColor);
}

void World::SetTraceAlphaRef(int value)
{
	m_pRender->SetTraceAlphaRef(value);
}

int World::GetTraceAlphaRef() const
{
	if (NULL == m_pRender)
	{
		return 0;
	}
	
	return m_pRender->GetTraceAlphaRef();
}

void World::SetRenderAlphaRef(int value)
{
	m_pRender->SetRenderAlphaRef(value);
}

int World::GetRenderAlphaRef() const
{
	return m_pRender->GetRenderAlphaRef();
}

void World::SetTextureLod(int value)
{
	if (NULL == m_pRender)
	{
		return;
	}

	m_pRender->SetTextureLod(value);
}

int World::GetTextureLod() const
{
	if (NULL == m_pRender)
	{
		return 0;
	}

	return m_pRender->GetTextureLod();
}

void World::SetTraceAloneTexture(int value)
{
	m_pRender->SetTraceAloneTexture(value);
}

int World::GetTraceAloneTexture() const
{
	return m_pRender->GetTraceAloneTexture();
}

void World::SetSleep(int value)
{
	GetCore()->SetSleep(value);
}

int World::GetSleep() const
{
	return GetCore()->GetSleep();
}

void World::SetExecuteFPS(int value)
{
	GetCore()->SetFramePerSecond(value);
}

int World::GetExecuteFPS() const
{
	return GetCore()->GetFramePerSecond();
}

void World::SetResourceThreadNum(int value)
{
	m_pRender->SetResourceThreadNum(value);
}

int World::GetResourceThreadNum() const
{
	return m_pRender->GetResourceThreadNum();
}

void World::SetAsyncLoadResource(bool value)
{
	m_pRender->SetAsyncLoadResource(value);
}

bool World::GetAsyncLoadResource() const
{
	return m_pRender->GetAsyncLoadResource();
}

void World::SetBlackWhiteDiffuse(bool value)
{
	m_pRender->SetBlackWhiteDiffuse(value);
}

bool World::GetBlackWhiteDiffuse() const
{
	return m_pRender->GetBlackWhiteDiffuse();
}

bool World::SetDeviceParam()
{
	Assert(m_pRender != NULL);
	
	m_pRender->SetDeviceSize(m_nWidth, m_nHeight);
	m_pRender->SetAnisotropic(m_bAnisotropic);
	m_pRender->SetMultiSampleType(m_nMultiSampleType);
	m_pRender->SetMaxAnisotropy(m_nMaxAnisotropy);

	return true;
}

bool World::CreateDevice(bool multithreading)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	
	if (NULL == m_pRender)
	{
		CORE_TRACE("(World::CreateDevice)render not found");
		return false;
	}

	// 捕获设备出错
	m_pRender->SetDeviceErrorCallback(DeviceErrorHook, this);
	
	if (!SetDeviceParam())
	{
		return false;
	}
	
	if (!m_pRender->CreateDevice(multithreading))
	{
		return false;
	}

	m_pModelSystem = (ModelSystem*)GetCore()->GetInterface("ModelSystem");

	return true;
}

bool World::ResetDevice()
{
	if (NULL == m_pRender)
	{
		return false;
	}

	if (!SetDeviceParam())
	{
		return false;
	}

	return m_pRender->ResetDevice();
}

bool World::DumpResource(const char* file_name)
{
	return m_pRender->DumpResource(file_name);
}

bool World::DumpModelResource(const char* file_name)
{
	if (NULL == m_pModelSystem)
	{
		return false;
	}

	//return m_pModelSystem->DumpResource(file_name);
	return false;
}

int World::GetResourceLoadTaskNum()
{
	return m_pRender->GetResourceLoadTaskNum();
}

bool World::ClearRenderResource()
{
	return m_pRender->ClearRenderResource();
}

bool World::CollectResource()
{
	return true;
}

int World::CollectUnuseAction(int seconds)
{
	if (NULL == m_pModelSystem)
	{
		return 0;
	}

	//return m_pModelSystem->CollectUnuseAction(seconds);
	return 0;
}

bool World::ReloadAllShader()
{
	return m_pRender->ReloadAllShader();
}

bool World::ScreenShot(const char* name)
{
	return m_pRender->ScreenShot(name);
}

void World::SetTraceTreeLeaf(bool value)
{
	m_pRender->SetTraceTreeLeaf(value);
}

bool World::GetTraceTreeLeaf() const
{
	return m_pRender->GetTraceTreeLeaf();
}

bool World::SetLogFileIO(bool value)
{
	GetCore()->SetLogFileIO(value);

	return true;
}

// 获取设备硬件能力
PERSISTID World::GetDeviceCapsID() const
{
	return m_pRender->GetDeviceCapsID();
}

void World::SetUseLightMap(bool value)
{
	m_pRender->SetUseLightMap(value);
}

bool World::GetUseLightMap() const
{
	return m_pRender->GetUseLightMap();
}
