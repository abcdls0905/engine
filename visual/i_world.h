#ifndef _I_WORLD_H
#define _I_WORLD_H

#include "../public/module.h"

class Gui;
class IScene;
class IWorld : public IEntity
{
public:
		// 每帧渲染
	virtual void Display(float offset_seconds) = 0;

	// 主界面对象
	virtual void SetMainGuiID(const PERSISTID& id) = 0;
	
	virtual PERSISTID GetMainGuiID() const = 0;
	
	virtual Gui* GetMainGui() const  = 0;

	// 主场景对象
	virtual void SetMainSceneID(const PERSISTID& id) = 0;
	virtual PERSISTID GetMainSceneID() const = 0;
	virtual IScene* GetMainScene() const = 0;

	// 添加渲染到贴图
	virtual bool AddRenderTex(const PERSISTID& id) = 0;
	// 移除渲染到贴图
	virtual bool RemoveRenderTex(const PERSISTID& id) = 0;

	// 渲染窗口宽度
	virtual void SetWidth(int value) = 0;
	virtual int GetWidth() const = 0;

	// 渲染窗口高度
	virtual void SetHeight(int value) = 0;
	virtual int GetHeight() const = 0;

	// 是否开启各向异性过滤
	virtual void SetAnisotropic(bool value) = 0;
	virtual bool GetAnisotropic() const = 0;

	// 最大各向异性过滤
	virtual void SetMaxAnisotropy(int value) = 0;
	virtual int GetMaxAnisotropy() const = 0;

	// 全屏抗锯齿
	virtual void SetMultiSampleType(int value) = 0;
	virtual int GetMultiSampleType() const = 0;

	// 只显示三角形边线
	virtual void SetWireFrame(bool value) = 0;
	virtual bool GetWireFrame() const = 0;

	// 背景颜色
	virtual void SetBackColor(const char* value) = 0;
	virtual result_string GetBackColor() const = 0;

	// 碰撞检测时的ALPHA测试参考值
	virtual void SetTraceAlphaRef(int value) = 0;
	virtual int GetTraceAlphaRef() const = 0;

	// 渲染时的ALPHA测试参考值
	virtual void SetRenderAlphaRef(int value) = 0;
	virtual int GetRenderAlphaRef() const = 0;

	// 贴图精度
	virtual void SetTextureLod(int value) = 0;
	virtual int GetTextureLod() const = 0;

	// 使用独立的附加贴图数据进行碰撞检测（从而避免锁定正在渲染的贴图）
	virtual void SetTraceAloneTexture(int value) = 0;
	virtual int GetTraceAloneTexture() const = 0;

	// 程序主循环休眠时间(毫秒)
	virtual void SetSleep(int value) = 0;
	virtual int GetSleep() const = 0;

	// 执行逻辑帧速
	virtual void SetExecuteFPS(int value) = 0;
	virtual int GetExecuteFPS() const = 0;

	// 资源加载线程数量
	virtual void SetResourceThreadNum(int value) = 0;
	virtual int GetResourceThreadNum() const = 0;

	// 是否异步加载资源
	virtual void SetAsyncLoadResource(bool value) = 0;
	virtual bool GetAsyncLoadResource() const = 0;

	// 是否使用黑白格子漫反射贴图
	virtual void SetBlackWhiteDiffuse(bool value) = 0;
	virtual bool GetBlackWhiteDiffuse() const = 0;

	// 创建渲染设备
	virtual bool CreateDevice(bool multithreading =false) = 0;
	// 重新设置设备尺寸
	virtual bool ResetDevice() = 0;

	// 导出渲染器的资源信息
	virtual bool DumpResource(const char* file_name) = 0;
	// 导出模型系统的资源信息
	virtual bool DumpModelResource(const char* file_name) = 0;
	// 获得当前资源加载任务数量
	virtual int GetResourceLoadTaskNum() = 0;
	// 清除一些渲染过程中使用的临时资源
	virtual bool ClearRenderResource() = 0;
	// 删除所有无引用的资源
	virtual bool CollectResource() = 0;
	// 回收长时间不使用的动作数据
	virtual int CollectUnuseAction(int seconds) = 0;

	// 重新加载所有效果文件
	virtual bool ReloadAllShader() = 0;
	
	// 屏幕截图
	virtual bool ScreenShot(const char* name) = 0;

	// 是否开启树叶碰撞
	virtual void SetTraceTreeLeaf(bool value) = 0;
	virtual bool GetTraceTreeLeaf() const = 0;

	// 设置是否记录文件访问日志
	virtual bool SetLogFileIO(bool value) = 0;

	// 获取设备硬件能力
	virtual PERSISTID GetDeviceCapsID() const = 0;

	// 是否使用光贴图
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() const = 0;
};
#endif