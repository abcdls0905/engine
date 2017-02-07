#ifndef _I_RENDER_H
#define _I_RENDER_H

#include "i_resource.h"
#include "i_res_loader.h"
#include "i_texture.h"
#include "i_scene_view.h"
#include "i_image_access.h"
#include "../public/i_interface.h"

// 设备错误回调函数原型
typedef bool (*device_error_callback_t)(void* context, int thread_id,
										int error, const char* info);


class IRender: public IInterface
{
public:
#ifdef PROCESS_LOST_DEVICE
	// 设备丢失回调函数原型
	typedef bool (*DEVICE_LOST_CALLBACK)(void* context);

	struct device_lost_call_t
	{
		void* pContext;
		DEVICE_LOST_CALLBACK pInvalidateFunc;
		DEVICE_LOST_CALLBACK pRestoreFunc;
	};
#endif
	// 设备名称
	virtual const char* GetDeviceName() = 0;

	// 创建渲染设备
	virtual bool CreateDevice(bool multithreading) = 0;
	// 重新设置设备尺寸
	virtual bool ResetDevice() = 0;
	// 更新当前窗口的尺寸
	virtual bool UpdateWindowSize() = 0;

	// 获得性能统计数据(1秒钟的性能)
	virtual performance_info_t* GetPerformance() = 0;
	
	// 获得性能统计数据(内存)
	virtual performance_mem_t* GetMemPerformance() = 0;

	// 资源加载线程数量
	virtual void SetResourceThreadNum(int value) = 0;
	virtual int GetResourceThreadNum() = 0;

	// 请求异步加载资源
	virtual bool RequestLoadResource(int category, IResLoader* loader) = 0;
	// 获得当前资源加载任务数量
	virtual int GetResourceLoadTaskNum() = 0;

	// 显示设备尺寸
	virtual void SetDeviceSize(int width, int height) = 0;
	// 获取当前分辨率
	virtual int GetDeviceWidth() = 0;
	virtual int GetDeviceHeight() = 0;

	// 根据资源名字返回文件路径的前缀
	virtual const char* GetPathPrefix(const char* name) = 0;

	// 根据运行平台返回当前加载资源的扩展名
	virtual const char* GetTexExtfix() = 0;

	// 获取设备信息
	virtual PERSISTID GetDeviceCapsID() const = 0;
	// 创建场景视图
	virtual ISceneView* CreateSceneView(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent = NULL) = 0;
	// 设置当前场景视图
	virtual void SetSceneView(ISceneView* pSceneView) = 0;
	// 获得当前场景视图
	virtual ISceneView* GetSceneView() = 0;
	// 增加当前帧的场景视图
	virtual void AddSceneView(ISceneView* pSceneView) = 0;

	// 创建渲染器参数
	virtual IRenderContext* CreateContext() = 0;
	// 设置当前渲染器参数
	virtual void SetContext(IRenderContext* pContext) = 0;
	// 获得当前渲染器参数
	virtual IRenderContext* GetContext() = 0;

	// 创建静态顶点缓冲 最后一个参与用户调试输出使用
	virtual IStaticVB* CreateStaticVB(const void* pdata, unsigned int size)= 0;
	// 创建静态索引缓冲
	virtual IStaticIB* CreateStaticIB(const void* pdata, unsigned int size)= 0;
	// 创建动态顶点缓冲
	virtual IDynamicVB* CreateDynamicVB(unsigned int size,const char* pstrIdent = NULL )= 0;
	// 创建动态索引缓冲
	virtual IDynamicIB* CreateDynamicIB(unsigned int size,const char* pstrIdent = NULL)= 0;
	// 创建静态贴图
	virtual IStaticTex* CreateStaticTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format,
		const void* tex_data, unsigned int data_size)= 0;
	// 创建动态变化的贴图
	virtual IDynamicTex* CreateDynamicTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format)= 0;
	// 创建颜色渲染目标
	virtual IColorRT* CreateColorRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;
	// 创建自动生成MIPMAP的颜色渲染目标
	virtual IColorRT* CreateMipmapColorRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;
	// 创建深度渲染目标
	virtual IDepthRT* CreateDepthRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;

	// 创建不可采样的ColorTex
	virtual IColorTex* CreateColorTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// 创建不可采样的DepthTex
	virtual IDepthTex* CreateDepthTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// 创建不可以采样的模板图
	virtual IStencilTex* CreateStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// 创建不可以采样的模板图
    virtual IDepthStencilTex* CreateDepthStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// 创建可以采样的深度模板贴图
	virtual IDepthStencilRT* CreateDepthStencilRT(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// 创建一个FrameRT color和depth都需要的
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL) = 0;

	// 只要深度采样
	virtual IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL) = 0 ;

	// 只要颜色
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex = NULL ) = 0;

	// 创建一个FrameRT color和depthstencil
	virtual IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex) = 0;

	// 创建一个FrameRT colortx和depthstencil
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT) = 0;

	//只需要颜色的colorrt
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT) = 0;

	// 写入动态顶点缓冲
	virtual bool UpdateDynamicVB(IDynamicVB* pVBuffer, unsigned int offset,
		const void* pdata, unsigned int size)= 0;
	// 写入动态索引缓冲
	virtual bool UpdateDynamicIB(IDynamicIB* pIBuffer, unsigned int offset,
		const void* pdata, unsigned int size)= 0;
	// 写入动态贴图
	virtual bool UpdateDynamicTex(IDynamicTex* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata) = 0 ;

	// 加载顶点着色器
	virtual IVertexShader* LoadVertexShader(const char* file_name, 
		const char* func_name, const char* define) = 0;
	// 加载像素着色器
	virtual IPixelShader* LoadPixelShader(const char* file_name, 
		const char* func_name, const char* define)= 0;
	// 创建着色器程序
	virtual IShaderProgram* CreateShaderProgram(IVertexShader* pVS, IPixelShader* pPS,
		const char* element_names[], unsigned int element_num) = 0;

	// 保存贴图到文件
	virtual bool SaveTextureToFile(IShaderTex* pShaderTex, 
		const char* file_name) = 0;

	// 获得指定名字的贴图
	virtual ITexture* FindTexture(const char* name) = 0;
	// 创建贴图
	virtual ITexture* CreateTexture(const char* name, bool async) = 0;
	// 创建立方体贴图
	virtual ITexture* CreateCubeTexture(const char* name, bool async) = 0;
	// 创建体积贴图
	virtual ITexture* CreateVolumeTexture(const char* name, bool async) = 0;
	// 创建新贴图
	virtual ITexture* CreateNewTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format, const void* tex_data, unsigned int data_size) = 0;
	// 创建动态贴图
	virtual ITexture* CreateDynamicTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format) = 0;
	// 创建作为渲染目标的贴图
	virtual ITexture* CreateRenderTexture(const char* name, 
		unsigned int width, unsigned int height, TEX_FORMAT format) = 0;
	// 写入动态贴图
	virtual bool UpdateDynamicTexture(ITexture* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata) = 0;

	// 获得贴图指定位置的透明度
	virtual unsigned int GetTextureAlpha(ITexture* pTexture, float u, float v) = 0;

	//获取当前渲染的状态
	virtual IRenderStateOp* GetRenderStateOp() const = 0; 

	//获取当前渲染相关操作
	virtual IRenderDrawOp*  GetRenderDrawOp() const =  0;

	//获取当前状态记录器操作
//	virtual IRenderStateRecordOp* GetRenderStateRecordOp() const = 0;

	// 画包围盒
	virtual bool DrawBoundBox(const FmMat4& mat_global, const FmVec3& center, 
		const FmVec3& box_size, const FmVec3& vColor) = 0;

	virtual void BeginFrame(unsigned int back_color) = 0;
	virtual void EndFrame() = 0;

	// 设置设备出错回调
	virtual bool SetDeviceErrorCallback(device_error_callback_t func, 
		void* context) = 0;

	// 最大各向异性过滤
	virtual void SetMaxAnisotropy(int value) = 0;
	virtual int GetMaxAnisotroy() = 0;

	// 是否开启各向异性过滤
	virtual void SetAnisotropic(bool value) = 0;
	virtual bool GetAnisotropic() = 0;

	// 是否显示网格
	virtual void SetWireFrame(bool value) = 0;
	virtual bool GetWireFrame() = 0;

    // 是否显示具有隐藏属性标签的模型
    virtual void SetShowHideModel(bool value) = 0;
    virtual bool GetShowHideModel() = 0;

	// 碰撞检测时的ALPHA测试参考值
	virtual void SetTraceAlphaRef(int value) = 0;
	virtual int GetTraceAlphaRef() = 0;

	// 渲染时的标准ALPHA测试参考值
	virtual void SetRenderAlphaRef(int value) = 0;
	virtual int GetRenderAlphaRef() = 0;

	// 贴图精度
	virtual void SetTextureLod(int value) = 0;
	virtual int GetTextureLod() = 0;

	// 使用独立的附加贴图数据进行碰撞检测（从而避免锁定正在渲染的贴图）
	virtual void SetTraceAloneTexture(int value) = 0;
	virtual int GetTraceAloneTexture() = 0;

	// 是否异步加载资源
	virtual void SetAsyncLoadResource(bool value) = 0;
	virtual bool GetAsyncLoadResource() = 0;

	// 是否使用黑白格子漫反射贴图
	virtual void SetBlackWhiteDiffuse(bool value) = 0;
	virtual bool GetBlackWhiteDiffuse() = 0;

	// 多重采样类型
	virtual void SetMultiSampleType(int type) = 0;
	virtual int GetMultiSampleType() = 0;

	// 重新加载所有效果文件
	virtual bool ReloadAllShader() = 0;
	// 导出所有的资源信息
	virtual bool DumpResource(const char* file_name) = 0;
	// 清除一些渲染过程中使用的临时资源
	virtual bool ClearRenderResource() = 0;
	// 屏幕截图
	virtual bool ScreenShot(const char* name) = 0;
	// 屏幕截图并指定区域
	virtual bool ScreenShot(const char* name, int x, int y, int w, int h) = 0;
	// 是否对具有树叶标记属性的模型做碰撞检测
	virtual void SetTraceTreeLeaf(bool value) = 0;
	virtual bool GetTraceTreeLeaf() = 0;
	// 是否使用光贴图
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() = 0;

	// 画辅助线条
	virtual bool DrawDesignLine(const FmMat4& mtxWorld, 
		int primitive_type,	unsigned int primitive_count, 
		const void* vertex_data, unsigned int vertex_stride) = 0;

	// 光线追踪时是否返回贴图名
	virtual void SetTraceTextureName(bool value) = 0;
	virtual bool GetTraceTextureName() = 0;

	// 是否只对正面三角形进行光线追踪
	virtual void SetTraceOnlyFront(bool value) = 0;
	virtual bool GetTraceOnlyFront() = 0;

	// 创建图片读写类
	virtual IImageAccess* CreateImageAccess() = 0;

	// Color RT拷贝 如果dest为NULL则代表直接绘制到当前FBO上面
	virtual bool CopyColorRT(IColorRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	// Color RT拷贝 如果dest为NULL则代表直接绘制到当前FBO上面 该格式目前只支持RGB888 或者RGBA8888 
	// 这里需要注意的是关于copy深度图的时机,底层解码的深度图的时候转换为线性深度，所以自动获取当前的使用的GetSceneView()下的管理的渲染对象的摄像机参数进行解码
	// 这里考虑易用性，所以直接忽略这个问题的存在
	virtual bool CopyDepthRT(IDepthRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	virtual bool CopyDepthStencilRT(IDepthStencilRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	// 绘制全屏一个正方形 
	virtual bool DrawAxisAlignedQuad(const char* pstrIdent = NULL) = 0;

	virtual bool DrawHalfCube(const char* pstrIdent = NULL) = 0;

	//根据可采样贴图绑定一个采样器
	virtual ITextureSampler* CreateTextureSamper(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D,\
														ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag = ITextureSampler::TS_MAG_LINEAR,\
														ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min = ITextureSampler::TS_MIN_LINEAR,\
														ITextureSampler::TEX_WRAP_MODE u_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE,\
														ITextureSampler::TEX_WRAP_MODE v_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE) = 0;


#ifdef PROCESS_LOST_DEVICE
	// 设备恢复时的处理
	virtual bool Restore() = 0;

	// 设备丢失时的处理
	virtual bool Invalidate() = 0;

	virtual bool AddDeviceLostCallback(void* context, 
		DEVICE_LOST_CALLBACK invalidate_func, DEVICE_LOST_CALLBACK restore_func) = 0;

	// 移除设备丢失回调函数
	virtual bool RemoveDeviceLostCallback(void* context) = 0;
#endif

	// 设置当前fbo
	virtual void UsedFrameRT(IFrameRT* pFrameRT) = 0;

	virtual IFrameRT* GetUsedFrameRT() const = 0;

	// 查找缓冲文件
	virtual bool FindCache(const char* file_name) = 0;
	// 加载缓冲文件
	virtual bool LoadCache(const char* file_name, bool async_load) = 0;
	// 卸载缓冲文件
	virtual bool UnloadCache(const char* file_name) = 0;
	// 卸载所有的缓冲文件
	virtual bool UnloadAllCache() = 0;
	// 缓冲文件是否成功加载
	virtual bool GetCacheLoaded(const char* file_name) = 0;
	// 缓冲文件是否结束加载（成功或失败）
	virtual bool GetCacheLoadFinish(const char* file_name) = 0;
	// 获得缓冲文件数据
	virtual const void* GetCacheData(const char* file_name, size_t& size) = 0;
	// 获得缓冲文件列表
	virtual size_t GetCacheList(IVarList& result) = 0;
	// 查找缓冲配置文件
	virtual bool FindCacheIni(const char* file_name) = 0;
	// 加载缓冲配置文件
	virtual bool LoadCacheIni(const char* file_name, bool create_hash, 
		bool async_load) = 0;
	// 卸载缓冲配置文件
	virtual bool UnloadCacheIni(const char* file_name) = 0;
	// 卸载所有的缓冲配置文件
	virtual bool UnloadAllCacheIni() = 0;
	// 缓冲配置文件是否成功加载
	virtual bool GetCacheIniLoaded(const char* file_name) = 0;
	// 缓冲配置文件是否结束加载（成功或失败）
	virtual bool GetCacheIniLoadFinish(const char* file_name) = 0;
	// 获得缓冲配置文件数据
	virtual IIniFile* GetCacheIni(const char* file_name) = 0;
    // 获取当前shader的信息数据
	virtual size_t GetShaderInfoNum() = 0;
	// 获取shader资源
	virtual IResource* GetShaderInfo(size_t index) = 0;
	// 抓取一帧数据
	virtual void CaptureFrame() = 0;
	// 设置贴图缓存时间
	virtual void SetTextureCacheTime(float time) = 0;
	// 清除当前贴图缓存
	virtual void ClearTextureCache() = 0;
};
#endif