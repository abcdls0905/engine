//--------------------------------------------------------------------
// 文件名:		world.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WORLD_H
#define _WORLD_H

#include "../public/module.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_world.h"
#include "Scene.h"
// 主渲染窗口

class IRender;
class ModelSystem;
class Gui;
class Scene;
class RenderTex;

class World: public IWorld
{
private:
	struct render_tex_t
	{
		render_tex_t* pNext;
		RenderTex* pRenderTex;
	};

	// 设备错误捕获
	static bool DeviceErrorHook(void* context, int thread_id,
		int error, const char* info);

public:
	World();
	virtual ~World();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 每帧更新
	virtual void Execute(float seconds);
	
	// 每帧渲染
	void Display(float offset_seconds);

	// 获得显示设备名称
	const char* GetDeviceName() const;
	
	// 主界面对象
	void SetMainGuiID(const PERSISTID& id);
	PERSISTID GetMainGuiID() const;
	Gui* GetMainGui() const { return m_pMainGui; }

	// 主场景对象
	void SetMainSceneID(const PERSISTID& id);
	PERSISTID GetMainSceneID() const;
	IScene* GetMainScene() const { return m_pMainScene; }

	// 添加渲染到贴图
	bool AddRenderTex(const PERSISTID& id);
	// 移除渲染到贴图
	bool RemoveRenderTex(const PERSISTID& id);

	// 渲染窗口宽度
	void SetWidth(int value);
	int GetWidth() const;

	// 渲染窗口高度
	void SetHeight(int value);
	int GetHeight() const;

	// 是否开启各向异性过滤
	void SetAnisotropic(bool value);
	bool GetAnisotropic() const;

	// 最大各向异性过滤
	void SetMaxAnisotropy(int value);
	int GetMaxAnisotropy() const;

	// 全屏抗锯齿
	void SetMultiSampleType(int value);
	int GetMultiSampleType() const;

	// 只显示三角形边线
	void SetWireFrame(bool value);
	bool GetWireFrame() const;

	// 背景颜色
	void SetBackColor(const char* value);
	result_string GetBackColor() const;

	// 碰撞检测时的ALPHA测试参考值
	void SetTraceAlphaRef(int value);
	int GetTraceAlphaRef() const;

	// 渲染时的ALPHA测试参考值
	void SetRenderAlphaRef(int value);
	int GetRenderAlphaRef() const;

	// 贴图精度
	void SetTextureLod(int value);
	int GetTextureLod() const;

	// 使用独立的附加贴图数据进行碰撞检测（从而避免锁定正在渲染的贴图）
	void SetTraceAloneTexture(int value);
	int GetTraceAloneTexture() const;

	// 程序主循环休眠时间(毫秒)
	void SetSleep(int value);
	int GetSleep() const;

	// 执行逻辑帧速
	void SetExecuteFPS(int value);
	int GetExecuteFPS() const;

	// 资源加载线程数量
	void SetResourceThreadNum(int value);
	int GetResourceThreadNum() const;

	// 是否异步加载资源
	void SetAsyncLoadResource(bool value);
	bool GetAsyncLoadResource() const;

	// 是否使用黑白格子漫反射贴图
	void SetBlackWhiteDiffuse(bool value);
	bool GetBlackWhiteDiffuse() const;

	// 创建渲染设备
	bool CreateDevice(bool multithreading);
	// 重新设置设备尺寸
	bool ResetDevice();

	// 导出渲染器的资源信息
	bool DumpResource(const char* file_name);
	// 导出模型系统的资源信息
	bool DumpModelResource(const char* file_name);
	// 获得当前资源加载任务数量
	int GetResourceLoadTaskNum();
	// 清除一些渲染过程中使用的临时资源
	bool ClearRenderResource();
	// 删除所有无引用的资源
	bool CollectResource();
	// 回收长时间不使用的动作数据
	int CollectUnuseAction(int seconds);

	// 重新加载所有效果文件
	bool ReloadAllShader();
	
	// 屏幕截图
	bool ScreenShot(const char* name);

	// 是否开启树叶碰撞
	void SetTraceTreeLeaf(bool value);
	bool GetTraceTreeLeaf() const;

	// 设置是否记录文件访问日志
	bool SetLogFileIO(bool value);

	// 获取设备硬件能力
	PERSISTID GetDeviceCapsID() const;

	// 是否使用光贴图
	void SetUseLightMap(bool value);
	bool GetUseLightMap() const;

public:
	// 设置渲染设备参数
	bool SetDeviceParam();

private:
	IRender* m_pRender;
	ModelSystem* m_pModelSystem;
	Gui* m_pMainGui;
	Scene* m_pMainScene;
	render_tex_t* m_pRenderTex;
	unsigned int m_nBackColor;
	int m_nWidth;
	int m_nHeight;
	bool m_bAnisotropic;
	int m_nMaxAnisotropy;
	int m_nMultiSampleType;
};

#endif // _WORLD_H
