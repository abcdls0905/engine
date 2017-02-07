//--------------------------------------------------------------------
// 文件名:		render.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RENDER_H
#define _RENDER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/i_file_sys.h"
#include "../visual/i_resource.h"
#include "../visual/i_res_loader.h"
#include "../visual/i_texture.h"
#include "../visual/shader_list.h"


#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
 
#include "../utils/string_pod.h"
#include "../visual/i_render.h"
#include "mempool2.h"

#include "device_caps.h"

#include "render_utils.h"

#include "render_header.h"

#include "cache_file.h"
#include "cache_ini.h"



class TRenderAlloc
{
public:
	TRenderAlloc() {}
	~TRenderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TRenderAlloc& src) {}
};

class TCopyDepthShaderAlloc
{
public:
	TCopyDepthShaderAlloc() {}
	~TCopyDepthShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TCopyDepthShaderAlloc& src) {}
};





// 渲染器

class CResThread;
class CRenderTool;
class CTexFactory;
class CTexture;
class CImageAccess;
class IRenderContext;
class CSceneView;
class CVertexShader;
class CPixelShader;
class CPaintBatchs;
class CTextureSamplerBindOp;
class fmxStreamFile;
enum
{
	DEVICE_STATE_INVALIDE_START,
	DEVICE_STATE_INVALIDE_END,
	DEVICE_STATE_RESTORE,
	DEVICE_STATE_READY,
};

#include <vector>
class Render;
class TextureCache : public IEntity
{
	struct Item
	{
		CTexture* pTexture;
		float time;
	};

	std::vector<Item> m_items;

	Render* m_pRender;
	static TextureCache * s_inst;
public:
	~TextureCache()
	{
		s_inst = 0;
	}

	void AddTexture(CTexture* pTexture);

	bool Init(const IVarList& args);

	void Create(Render* pRender) { m_pRender = pRender; }
	
	bool Shut();
	
	void Execute(float seconds) ;

	static TextureCache* Inst() { return s_inst; }

	void Clear();

	float m_cache_texture_time;
};

class Render: public IRender
{
public:
    // 深度拷贝的方式
	enum FLAG_DEPTH_SHIFT_ENUM
	{
		ENCODE_RGB888,//直接编码
		ENCODE_RGBA8888,//
		ENCODE_R16,
		ENCODE_R32,
		ENCODE_MAX,
	};

	// 渲染啊拷贝深度图的函数
	struct copy_depth_shaderhandle_t
	{
		IShaderProgram* pShader;
		fm_int vec4DepthParamHandle;//深度解码相关参数
		fm_int tex_DepthHandle;
	};


	int m_device_state;
#ifdef PROCESS_LOST_DEVICE
	TArrayPod<device_lost_call_t, 1, TRenderAlloc> m_DeviceLostCalls;
#endif
	static void DrawDesignLineBatch(void* pdata);
private:
	enum { MAX_RES_THREAD_NUM = 8 };

	typedef TStringPod<char, CTexture*, TStringTraitsCI<char>, 
		TRenderAlloc> texture_container_t;
	typedef TStringPod<char, size_t, TStringTraits<char>,
		TRenderAlloc> shader_container_t;

	struct shader_data_t
	{
		unsigned char* pCode;
		unsigned int nCodeSize;
		IResource* pShader;
	};

	struct shader_file_t
	{
		core_string strFile;
		unsigned int nHash;
		unsigned int nTimeHigh;
		unsigned int nTimeLow;
		shader_container_t Shaders;
	};

	typedef TStringPod<char, CCacheFile*, TStringTraitsCI<char>, 
		TRenderAlloc> CACHE_CONTAINER;
	typedef TStringPod<char, CCacheIni*, TStringTraitsCI<char>,
		TRenderAlloc> CACHE_INI_CONTAINER;
public:
	Render();
	~Render();

	virtual bool Init();
	virtual bool Shut();

	// 是否需要每帧运行（调用ExecFrameBegin和ExecFrameEnd）
	virtual bool NeedExecPerFrame();
	// 每帧开始时调用
	virtual void ExecFrameBegin();
	// 每帧结束时调用
	virtual void ExecFrameEnd();
	
	// 创建渲染设备
	bool CreateDevice(bool multithreading = false);
	// 重新设置设备尺寸
	bool ResetDevice();
	// 更新当前窗口的尺寸
	bool UpdateWindowSize();
	// 渲染画面
	void BeginFrame(unsigned int back_color);
	void EndFrame();
	
	// 设备名称
	const char* GetDeviceName();

	// 显示设备尺寸
	void SetDeviceSize(int width, int height); 
	int GetDeviceWidth();
	int GetDeviceHeight();

	// 当前显示窗口尺寸
	int GetWinWidth();
	int GetWinHeight();

	// 坐标是否需要缩放
	bool GetNeedScaleX();
	bool GetNeedScaleY();

	// 坐标缩放因子（显示设备尺寸与当前窗口尺寸的比例）
	float GetScaleFactorX();
	float GetScaleFactorY();

	// 是否开启各向异性过滤
	void SetAnisotropic(bool value);
	bool GetAnisotropic();
	
	// 最大各向异性过滤
	void SetMaxAnisotropy(int value);
	int GetMaxAnisotroy();

	// 多重采样类型
	void SetMultiSampleType(int type);
	int GetMultiSampleType();

	// 是否异步加载资源
	void SetAsyncLoadResource(bool value);
	bool GetAsyncLoadResource();

	// 贴图精度
	void SetTextureLod(int value);
	int GetTextureLod();

	// 渲染时的标准ALPHA测试参考值
	void SetRenderAlphaRef(int value);
	int GetRenderAlphaRef();

	// 碰撞检测时的ALPHA测试参考值
	void SetTraceAlphaRef(int value);
	int GetTraceAlphaRef();

	// 光线追踪时是否返回贴图名
	void SetTraceTextureName(bool value);
	bool GetTraceTextureName();

	// 获取设备信息
	PERSISTID GetDeviceCapsID() const;

	// 是否只对正面三角形进行光线追踪
	void SetTraceOnlyFront(bool value);
	bool GetTraceOnlyFront();

	// 使用独立的附加贴图数据进行碰撞检测（从而避免锁定正在渲染的贴图）
	void SetTraceAloneTexture(int value);
	int GetTraceAloneTexture();

	// 是否使用黑白格子漫反射贴图
	void SetBlackWhiteDiffuse(bool value) { m_bBlackWhiteDiffuse = value; }
	bool GetBlackWhiteDiffuse() { return m_bBlackWhiteDiffuse; }

	// 是否显示网格
	void SetWireFrame(bool value) { m_bWireFrame = value; }
	bool GetWireFrame() { return m_bWireFrame; }

	// 是否显示具有隐藏属性标签的模型
	virtual void SetShowHideModel(bool value) { m_bShowHideModel = value; }
	virtual bool GetShowHideModel() { return m_bShowHideModel; }

	// 是否对具有树叶标记属性的模型做碰撞检测
	void SetTraceTreeLeaf(bool value) { m_bTraceTreeLeaf = value; }
	bool GetTraceTreeLeaf() { return m_bTraceTreeLeaf; }

	// 是否使用光贴图
	virtual void SetUseLightMap(bool value){ m_bUseLightMap = value; }
	virtual bool GetUseLightMap() { return m_bUseLightMap; }

	// 根据资源名字返回文件路径的前缀
	const char* GetPathPrefix(const char* name);

	// 根据运行平台返回当前加载资源的扩展名
	const char* GetTexExtfix();

	// 创建图片读写类
	IImageAccess* CreateImageAccess();

	// 创建渲染器参数
	IRenderContext* CreateContext();
	// 设置当前渲染器参数
	void SetContext(IRenderContext* pContext);
	// 获得当前渲染器参数
	IRenderContext* GetContext();

	// 创建场景视图
	ISceneView* CreateSceneView(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent = NULL);
	// 设置当前场景视图
	void SetSceneView(ISceneView* pSceneView);
	// 获得当前场景视图
	ISceneView* GetSceneView();
	// 增加当前帧的场景视图
	void AddSceneView(ISceneView* pSceneView);

	// 屏幕截图
	bool ScreenShot(const char* name);

	// 屏幕截图并指定区域
	bool ScreenShot(const char* name, int x, int y, int w, int h);

	// 获得性能统计数据(1秒钟的性能)
	performance_info_t* GetPerformance();

	// 获得性能统计数据(内存)
	performance_mem_t* GetMemPerformance();

	// 资源加载线程数量
	void SetResourceThreadNum(int value);
	int GetResourceThreadNum();

	// 请求异步加载资源
	bool RequestLoadResource(int category, IResLoader* loader);
	// 获得当前资源加载任务数量
	int GetResourceLoadTaskNum();
	
	// 创建静态顶点缓冲
	IStaticVB* CreateStaticVB(const void* pdata, unsigned int size);
	// 创建静态索引缓冲
	IStaticIB* CreateStaticIB(const void* pdata, unsigned int size);
	// 创建动态顶点缓冲
	IDynamicVB* CreateDynamicVB(unsigned int size,const char* pstrIdent = NULL );
	// 创建动态索引缓冲
	IDynamicIB* CreateDynamicIB(unsigned int size,const char* pstrIdent = NULL );
	// 创建静态贴图
	IStaticTex* CreateStaticTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format,
		const void* tex_data, unsigned int data_size);

	// 创建动态变化的贴图
	IDynamicTex* CreateDynamicTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format);
	// 创建颜色渲染目标
	IColorRT* CreateColorRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// 创建自动生成MIPMAP的颜色渲染目标
	IColorRT* CreateMipmapColorRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// 创建深度渲染目标
	IDepthRT* CreateDepthRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// 创建不可采样的ColorTex
    IColorTex* CreateColorTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// 创建不可采样的DepthTex
	IDepthTex* CreateDepthTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// 创建不可以采样的模板图
    IStencilTex* CreateStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// 创建不可以采样的模板图
	IDepthStencilTex* CreateDepthStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// 创建可以采样的深度模板贴图
	IDepthStencilRT* CreateDepthStencilRT(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// 创建一个FrameRT color和depth都需要的
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL );

	// 只要深度采样
	IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL );
	// 只要颜色
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex = NULL );

    //一种比特殊带模板缓冲的纹理创建
	IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex);

	// 创建一个FrameRT colortx和depthstencil
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex);

	// 创建一个FrameRT colortx和depthstencil
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

	// 创建一个
    IFrameRT* CreateFrameRT(IColorRT* pColorRT);

	// 写入动态顶点缓冲
	bool UpdateDynamicVB(IDynamicVB* pVBuffer, unsigned int offset,
		const void* pdata, unsigned int size);
	// 写入动态索引缓冲
	bool UpdateDynamicIB(IDynamicIB* pIBuffer, unsigned int offset,
		const void* pdata, unsigned int size);
	// 写入动态贴图
	bool UpdateDynamicTex(IDynamicTex* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);

	// 加载顶点着色器
	IVertexShader* LoadVertexShader(const char* file_name, 
		const char* func_name, const char* define);
	// 加载像素着色器
	IPixelShader* LoadPixelShader(const char* file_name, 
		const char* func_name, const char* define);
	// 创建着色器程序
	IShaderProgram* CreateShaderProgram(IVertexShader* pVS, IPixelShader* pPS,
		const char* element_names[], unsigned int element_num);

	// 重新加载所有效果文件
	bool ReloadAllShader();
	// 导出所有的资源信息
	bool DumpResource(const char* file_name);
	// 清除一些渲染过程中使用的临时资源
	bool ClearRenderResource();

	// 保存贴图到文件
	bool SaveTextureToFile(IShaderTex* pShaderTex, 
		const char* file_name);

	// 获得指定名字的贴图
	ITexture* FindTexture(const char* name);
	// 创建贴图
	ITexture* CreateTexture(const char* name, bool async);
	// 创建立方体贴图
	ITexture* CreateCubeTexture(const char* name, bool async);
	// 创建体积贴图
	ITexture* CreateVolumeTexture(const char* name, bool async){ return 0; };
	// 创建新贴图
	ITexture* CreateNewTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format, const void* tex_data, unsigned int data_size);
	// 创建动态贴图
	ITexture* CreateDynamicTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format);
	// 创建作为渲染目标的贴图
	ITexture* CreateRenderTexture(const char* name, 
		unsigned int width, unsigned int height, TEX_FORMAT format);
	// 写入动态贴图
	bool UpdateDynamicTexture(ITexture* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);
	// 获得贴图指定位置的透明度
	unsigned int GetTextureAlpha(ITexture* pTexture, float u, float v);
	// 从渲染目标复制贴图
	bool CopyTextureFromColorRT(ITexture* pTexture, IColorRT* pRT);

	// 画辅助线条
	bool DrawDesignLine(const FmMat4& mtxWorld, 
		int primitive_type,	unsigned int primitive_count, 
		const void* vertex_data, unsigned int vertex_stride);
	// 显示纹理
	bool DrawTexture(IShaderTex* pTex, int left, int top, 
		int right, int bottom);
	// 画包围盒
	bool DrawBoundBox(const FmMat4& mat_global, const FmVec3& center, 
		const FmVec3& box_size, const FmVec3& vColor);

	// 设置设备出错回调
	bool SetDeviceErrorCallback(device_error_callback_t func, 
		void* context);

	// 显示设备产生错误
	bool RaiseDeviceError(int error, const char* info);

	// 从现有资源创建贴图对象
	IStaticTex* CreateStaticTexFrom(fm_uint nTex);
	IStaticCube* CreateStaticCubeFrom(fm_uint nTex);
	// 释放资源
	bool ReleaseResource(int index);

	// 获得贴图
	CTexture* GetTextureById(const PERSISTID& id);
	// 创建界面贴图
	CTexture* CreateGlyphTexture(const char* name, bool async);
	// 创建界面贴图
	// 把一个小图创建到大图中
	CTexture* CreateGlyphTexture(const char* name, int x, int y, const char* src_name, bool async);
	// 释放贴图
	bool ReleaseTexture(CTexture* pTex);

	bool DestroyTexture(CTexture* pTex);

	// 创建一个纹理采样器
 	ITextureSampler* CreateTextureSamper(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D,
		ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag = ITextureSampler::TS_MAG_LINEAR,
		ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min = ITextureSampler::TS_MIN_LINEAR,
		ITextureSampler::TEX_WRAP_MODE u_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE,
		ITextureSampler::TEX_WRAP_MODE v_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE);

	// 从已存在存在的采样器中查询状态并创建
	ITextureSampler* CreateTextureSamperFromShaderTex(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D);

	// Color RT拷贝（特殊解决后处理的问题)
	bool CopyColorRT(IColorRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	// 深度贴图拷贝
	bool CopyDepthRT(IDepthRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	// 拷贝深度贴图
	bool CopyDepthStencilRT(IDepthStencilRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	bool DrawAxisAlignedQuad(const char* pstrIdent = NULL);

	bool DrawHalfCube(const char* pstrIdent = NULL);

	CPaintBatchs* GetPaintBatchs() const { return m_pPaintBatchs; }

	// 获得渲染辅助工具
	CRenderTool* GetRenderTool() const { return m_pRenderTool; }
	// 获得当前背景色
	FmVec4 GetBackColor() const { return m_vBackColor; }
	// 获得当前帧计数
	size_t GetFrameId() const { return m_nFrameId; }

    //获取当前渲染的状态
	IRenderStateOp* GetRenderStateOp() const;

	//获取当前渲染相关操作
    IRenderDrawOp*  GetRenderDrawOp() const;

	IRenderStateRecordOp* GetRenderStateRecordOp() const;

	//获取当前采样器和纹理绑定操作
    CTextureSamplerBindOp* GetTextureSamplerBindOp() const;

	fmxStreamFile* GetfmxStreamFileOp() const;

	// 设置当前
	void UsedShader(IShaderProgram* pShader);
    // 
	IShaderProgram* GetUsedShader() const;

	// 设置当前fbo
	void UsedFrameRT(IFrameRT* pFrameRT);

	IFrameRT* GetUsedFrameRT() const;

    // 用于托管一些非常小的内存长度小于128个字节的数据
	// 使用内存池
    void* Alloc(size_t size);

    void Free(void* ptr, size_t size);

#ifdef PROCESS_LOST_DEVICE
	// 设备恢复时的处理
	bool Restore();
	// 设备丢失时的处理
	bool Invalidate();

	bool AddDeviceLostCallback(void* context, 
		DEVICE_LOST_CALLBACK invalidate_func, DEVICE_LOST_CALLBACK restore_func);

	// 移除设备丢失回调函数
	bool RemoveDeviceLostCallback(void* context);
#endif

	DeviceCaps* GetDeviceCaps();

	// 查找缓冲文件
	virtual bool FindCache(const char* file_name);
	// 加载缓冲文件
	virtual bool LoadCache(const char* file_name, bool async_load);
	// 卸载缓冲文件
	virtual bool UnloadCache(const char* file_name);
	// 卸载所有的缓冲文件
	virtual bool UnloadAllCache();
	// 缓冲文件是否成功加载
	virtual bool GetCacheLoaded(const char* file_name);
	// 缓冲文件是否结束加载（成功或失败）
	virtual bool GetCacheLoadFinish(const char* file_name);
	// 获得缓冲文件数据
	virtual const void* GetCacheData(const char* file_name, size_t& size);
	// 获得缓冲文件列表
	virtual size_t GetCacheList(IVarList& result);

	// 查找缓冲配置文件
	virtual bool FindCacheIni(const char* file_name);
	// 加载缓冲配置文件
	virtual bool LoadCacheIni(const char* file_name, bool create_hash, 
		bool async_load);
	// 卸载缓冲配置文件
	virtual bool UnloadCacheIni(const char* file_name);
	// 卸载所有的缓冲配置文件
	virtual bool UnloadAllCacheIni();
	// 缓冲配置文件是否成功加载
	virtual bool GetCacheIniLoaded(const char* file_name);
	// 缓冲配置文件是否结束加载（成功或失败）
	virtual bool GetCacheIniLoadFinish(const char* file_name);
	// 获得缓冲配置文件数据
	virtual IIniFile* GetCacheIni(const char* file_name);
	// 抓取一帧数据
    virtual void CaptureFrame();
	// 是否需要抓帧数出数据
	bool IsCaptureFrame();

	//以下2个接口提供内部渲染相关资源的查询
	size_t GetResourceNum();
	IResource* GetResource(size_t index);
private:
 
    // 直接全屏输出到屏幕上
    bool Print2D(IColorRT* src,const char* pstrIdent);

	// 直接绘制到全屏上面
	bool Print2D(IDepthRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent);

	// 直接绘制到全屏上面
	bool Print2D(IDepthStencilRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent);

	// 创建设备缓冲
	bool CreateDeviceBuffer();
	// 释放设备缓冲
	bool ReleaseDeviceBuffer();

	// 初始化渲染状态相关
	void InitRenderState();

    void InitCopyRT();

	// 获得资源的可用位置索引
	int GetResourceIndex();
	
	// 创建顶点着色器
	CVertexShader* CreateVertexShader(const void* shader_data,
		unsigned int shader_size);
	// 创建像素着色器
	CPixelShader* CreatePixelShader(const void* shader_data, 
		unsigned int shader_size);

	// 加载着色器代码
	int LoadShader(const char* file_name, const char* func_name,
		const char* define);
	// 查找着色器文件
	bool FindShaderFileIndex(const char* file_name, size_t& index) const;

	// 获取当前的copydepthrtshader
	Render::copy_depth_shaderhandle_t* GetShaderHandle(bool bENCODE_RGB888,bool bENCODE_RGBA8888,bool bENCODE_R16,bool bENCODE_R32);

    // 获取当前shader的信息数据
	size_t GetShaderInfoNum();
	IResource* GetShaderInfo(size_t index);

	// 设置是否进行抓帧数据
    void IsCaptureFrame(bool bIsCaputure);
 
	void SetTextureCacheTime(float time);
	void ClearTextureCache();
 
private:

	CResThread* m_pResThreads[MAX_RES_THREAD_NUM];
	int m_nResThreadNum;
	int m_nResThreadIndex;
	IRenderContext* m_pCurrentContext;
	ISceneView* m_pCurrentSceneView;
	core_string m_strDeviceName;
	core_string m_strShaderPath;
	core_string m_strTexExtfix;
	bool m_bAsyncLoadResource;
	bool m_bBlackWhiteDiffuse;
	bool m_bWireFrame;
	bool m_bShowHideModel;
	bool m_bTraceTreeLeaf;
	int m_nRenderAlphaRef;	
	int m_nTraceAlphaRef;
	int m_nTraceAloneTexture;
	bool m_bTraceTextureName;
	bool m_bTraceOnlyFront;
	int m_nTextureLod;
	int m_nDeviceHeight;
	int m_nDeviceWidth;
	int m_nWinHeight;
	int m_nWinWidth;
	bool m_bAnisotropic;
	int m_nMaxAnisotropy;
	int m_nMultiSampleType;
	bool m_bUseLightMap;
	bool m_bIsCaptureFrame;

	fm_int m_nDefautFBO;

	CRenderTool* m_pRenderTool;
	size_t m_nFrameId;
	FmVec4 m_vBackColor;
	TArrayPod<ISceneView*, 1, TRenderAlloc> m_SceneViews;
	CPaintBatchs* m_pPaintBatchs;
	performance_info_t m_Performance;
	performance_mem_t m_PerformanceMem;
	device_error_callback_t m_pDeviceErrorFunc;
	void* m_pDeviceErrorContext;

	CTexFactory* m_pTexFactory;
	texture_container_t m_Textures;
	TArrayPod<IResource*, 1, TRenderAlloc> m_ResourceList;
	TArrayPod<int, 1, TRenderAlloc> m_ResourceFree;
	TArrayPod<shader_data_t, 1, TRenderAlloc> m_ShaderList;
	TArrayPod<shader_file_t*, 1, TRenderAlloc> m_ShaderFiles;
//    PERSISTID m_DeviceCapsID;

	IShaderProgram* m_DesginLineShader;
	IDynamicVB* m_DesignLineVB;
	int m_DesignLineVBSize;
	IShaderProgram* m_pUsedShader;

	IRenderStateOp* m_pRenderStateOp;
	IRenderDrawOp*  m_pRenderDrawOp;
	IRenderStateRecordOp* m_pRenderStateRecordOp;

	//用于帧分析
	fmxStreamFile*        m_pfmxStreamFileOp;

	IFrameRT*       m_pUsedFrameRT;
    //用于记录纹理和采样器绑定关系
	CTextureSamplerBindOp* m_pTextureSamplerBindOp;
	/*
	IColorRT* m_pColorRT;
	IDepthRT* m_pDepthRT;
	IFrameRT* m_pFrameRT;
	*/
 	IFrameRT*  m_pCopyColorFrameRT;
	IFrameRT*  m_pCopyDepthFrameRT;

	IShaderProgram* m_pCopyColorRTShader;
	fm_int      m_nCopyColorRTSrcTex;

	TShaderList<IVertexShader, ENCODE_MAX> m_VSCopyDepthList;
	TShaderList<IPixelShader, ENCODE_MAX>  m_PSCopyDepthList;


	CACHE_CONTAINER m_CacheFiles;
	CACHE_INI_CONTAINER m_CacheInis;

	TPodHashMap<unsigned int, Render::copy_depth_shaderhandle_t*, TPodTraits<unsigned int>, TCopyDepthShaderAlloc> m_CopyDepthShaderMapHandle;


    IShaderProgram* m_pCopyDepthRTShader;
	fm_int      m_nCopyDepthRTSrcTex;

	PERSISTID   m_DeviceCapsID;

	CMemPool2   m_mempool;
};



#endif // _RENDER_H
