//--------------------------------------------------------------------
// �ļ���:		render.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TRenderAlloc& src) {}
};

class TCopyDepthShaderAlloc
{
public:
	TCopyDepthShaderAlloc() {}
	~TCopyDepthShaderAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TCopyDepthShaderAlloc& src) {}
};





// ��Ⱦ��

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
    // ��ȿ����ķ�ʽ
	enum FLAG_DEPTH_SHIFT_ENUM
	{
		ENCODE_RGB888,//ֱ�ӱ���
		ENCODE_RGBA8888,//
		ENCODE_R16,
		ENCODE_R32,
		ENCODE_MAX,
	};

	// ��Ⱦ���������ͼ�ĺ���
	struct copy_depth_shaderhandle_t
	{
		IShaderProgram* pShader;
		fm_int vec4DepthParamHandle;//��Ƚ�����ز���
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

	// �Ƿ���Ҫÿ֡���У�����ExecFrameBegin��ExecFrameEnd��
	virtual bool NeedExecPerFrame();
	// ÿ֡��ʼʱ����
	virtual void ExecFrameBegin();
	// ÿ֡����ʱ����
	virtual void ExecFrameEnd();
	
	// ������Ⱦ�豸
	bool CreateDevice(bool multithreading = false);
	// ���������豸�ߴ�
	bool ResetDevice();
	// ���µ�ǰ���ڵĳߴ�
	bool UpdateWindowSize();
	// ��Ⱦ����
	void BeginFrame(unsigned int back_color);
	void EndFrame();
	
	// �豸����
	const char* GetDeviceName();

	// ��ʾ�豸�ߴ�
	void SetDeviceSize(int width, int height); 
	int GetDeviceWidth();
	int GetDeviceHeight();

	// ��ǰ��ʾ���ڳߴ�
	int GetWinWidth();
	int GetWinHeight();

	// �����Ƿ���Ҫ����
	bool GetNeedScaleX();
	bool GetNeedScaleY();

	// �����������ӣ���ʾ�豸�ߴ��뵱ǰ���ڳߴ�ı�����
	float GetScaleFactorX();
	float GetScaleFactorY();

	// �Ƿ����������Թ���
	void SetAnisotropic(bool value);
	bool GetAnisotropic();
	
	// ���������Թ���
	void SetMaxAnisotropy(int value);
	int GetMaxAnisotroy();

	// ���ز�������
	void SetMultiSampleType(int type);
	int GetMultiSampleType();

	// �Ƿ��첽������Դ
	void SetAsyncLoadResource(bool value);
	bool GetAsyncLoadResource();

	// ��ͼ����
	void SetTextureLod(int value);
	int GetTextureLod();

	// ��Ⱦʱ�ı�׼ALPHA���Բο�ֵ
	void SetRenderAlphaRef(int value);
	int GetRenderAlphaRef();

	// ��ײ���ʱ��ALPHA���Բο�ֵ
	void SetTraceAlphaRef(int value);
	int GetTraceAlphaRef();

	// ����׷��ʱ�Ƿ񷵻���ͼ��
	void SetTraceTextureName(bool value);
	bool GetTraceTextureName();

	// ��ȡ�豸��Ϣ
	PERSISTID GetDeviceCapsID() const;

	// �Ƿ�ֻ�����������ν��й���׷��
	void SetTraceOnlyFront(bool value);
	bool GetTraceOnlyFront();

	// ʹ�ö����ĸ�����ͼ���ݽ�����ײ��⣨�Ӷ���������������Ⱦ����ͼ��
	void SetTraceAloneTexture(int value);
	int GetTraceAloneTexture();

	// �Ƿ�ʹ�úڰ׸�����������ͼ
	void SetBlackWhiteDiffuse(bool value) { m_bBlackWhiteDiffuse = value; }
	bool GetBlackWhiteDiffuse() { return m_bBlackWhiteDiffuse; }

	// �Ƿ���ʾ����
	void SetWireFrame(bool value) { m_bWireFrame = value; }
	bool GetWireFrame() { return m_bWireFrame; }

	// �Ƿ���ʾ�����������Ա�ǩ��ģ��
	virtual void SetShowHideModel(bool value) { m_bShowHideModel = value; }
	virtual bool GetShowHideModel() { return m_bShowHideModel; }

	// �Ƿ�Ծ�����Ҷ������Ե�ģ������ײ���
	void SetTraceTreeLeaf(bool value) { m_bTraceTreeLeaf = value; }
	bool GetTraceTreeLeaf() { return m_bTraceTreeLeaf; }

	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value){ m_bUseLightMap = value; }
	virtual bool GetUseLightMap() { return m_bUseLightMap; }

	// ������Դ���ַ����ļ�·����ǰ׺
	const char* GetPathPrefix(const char* name);

	// ��������ƽ̨���ص�ǰ������Դ����չ��
	const char* GetTexExtfix();

	// ����ͼƬ��д��
	IImageAccess* CreateImageAccess();

	// ������Ⱦ������
	IRenderContext* CreateContext();
	// ���õ�ǰ��Ⱦ������
	void SetContext(IRenderContext* pContext);
	// ��õ�ǰ��Ⱦ������
	IRenderContext* GetContext();

	// ����������ͼ
	ISceneView* CreateSceneView(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent = NULL);
	// ���õ�ǰ������ͼ
	void SetSceneView(ISceneView* pSceneView);
	// ��õ�ǰ������ͼ
	ISceneView* GetSceneView();
	// ���ӵ�ǰ֡�ĳ�����ͼ
	void AddSceneView(ISceneView* pSceneView);

	// ��Ļ��ͼ
	bool ScreenShot(const char* name);

	// ��Ļ��ͼ��ָ������
	bool ScreenShot(const char* name, int x, int y, int w, int h);

	// �������ͳ������(1���ӵ�����)
	performance_info_t* GetPerformance();

	// �������ͳ������(�ڴ�)
	performance_mem_t* GetMemPerformance();

	// ��Դ�����߳�����
	void SetResourceThreadNum(int value);
	int GetResourceThreadNum();

	// �����첽������Դ
	bool RequestLoadResource(int category, IResLoader* loader);
	// ��õ�ǰ��Դ������������
	int GetResourceLoadTaskNum();
	
	// ������̬���㻺��
	IStaticVB* CreateStaticVB(const void* pdata, unsigned int size);
	// ������̬��������
	IStaticIB* CreateStaticIB(const void* pdata, unsigned int size);
	// ������̬���㻺��
	IDynamicVB* CreateDynamicVB(unsigned int size,const char* pstrIdent = NULL );
	// ������̬��������
	IDynamicIB* CreateDynamicIB(unsigned int size,const char* pstrIdent = NULL );
	// ������̬��ͼ
	IStaticTex* CreateStaticTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format,
		const void* tex_data, unsigned int data_size);

	// ������̬�仯����ͼ
	IDynamicTex* CreateDynamicTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format);
	// ������ɫ��ȾĿ��
	IColorRT* CreateColorRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// �����Զ�����MIPMAP����ɫ��ȾĿ��
	IColorRT* CreateMipmapColorRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// ���������ȾĿ��
	IDepthRT* CreateDepthRT(unsigned int width, unsigned int height, TEX_FORMAT format, int type);

	// �������ɲ�����ColorTex
    IColorTex* CreateColorTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// �������ɲ�����DepthTex
	IDepthTex* CreateDepthTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// ���������Բ�����ģ��ͼ
    IStencilTex* CreateStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// ���������Բ�����ģ��ͼ
	IDepthStencilTex* CreateDepthStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// �������Բ��������ģ����ͼ
	IDepthStencilRT* CreateDepthStencilRT(unsigned int width,unsigned int height, TEX_FORMAT format, int type);

	// ����һ��FrameRT color��depth����Ҫ��
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL );

	// ֻҪ��Ȳ���
	IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL );
	// ֻҪ��ɫ
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex = NULL );

    //һ�ֱ������ģ�建���������
	IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex);

	// ����һ��FrameRT colortx��depthstencil
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex);

	// ����һ��FrameRT colortx��depthstencil
	IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

	// ����һ��
    IFrameRT* CreateFrameRT(IColorRT* pColorRT);

	// д�붯̬���㻺��
	bool UpdateDynamicVB(IDynamicVB* pVBuffer, unsigned int offset,
		const void* pdata, unsigned int size);
	// д�붯̬��������
	bool UpdateDynamicIB(IDynamicIB* pIBuffer, unsigned int offset,
		const void* pdata, unsigned int size);
	// д�붯̬��ͼ
	bool UpdateDynamicTex(IDynamicTex* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);

	// ���ض�����ɫ��
	IVertexShader* LoadVertexShader(const char* file_name, 
		const char* func_name, const char* define);
	// ����������ɫ��
	IPixelShader* LoadPixelShader(const char* file_name, 
		const char* func_name, const char* define);
	// ������ɫ������
	IShaderProgram* CreateShaderProgram(IVertexShader* pVS, IPixelShader* pPS,
		const char* element_names[], unsigned int element_num);

	// ���¼�������Ч���ļ�
	bool ReloadAllShader();
	// �������е���Դ��Ϣ
	bool DumpResource(const char* file_name);
	// ���һЩ��Ⱦ������ʹ�õ���ʱ��Դ
	bool ClearRenderResource();

	// ������ͼ���ļ�
	bool SaveTextureToFile(IShaderTex* pShaderTex, 
		const char* file_name);

	// ���ָ�����ֵ���ͼ
	ITexture* FindTexture(const char* name);
	// ������ͼ
	ITexture* CreateTexture(const char* name, bool async);
	// ������������ͼ
	ITexture* CreateCubeTexture(const char* name, bool async);
	// ���������ͼ
	ITexture* CreateVolumeTexture(const char* name, bool async){ return 0; };
	// ��������ͼ
	ITexture* CreateNewTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format, const void* tex_data, unsigned int data_size);
	// ������̬��ͼ
	ITexture* CreateDynamicTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format);
	// ������Ϊ��ȾĿ�����ͼ
	ITexture* CreateRenderTexture(const char* name, 
		unsigned int width, unsigned int height, TEX_FORMAT format);
	// д�붯̬��ͼ
	bool UpdateDynamicTexture(ITexture* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata);
	// �����ͼָ��λ�õ�͸����
	unsigned int GetTextureAlpha(ITexture* pTexture, float u, float v);
	// ����ȾĿ�긴����ͼ
	bool CopyTextureFromColorRT(ITexture* pTexture, IColorRT* pRT);

	// ����������
	bool DrawDesignLine(const FmMat4& mtxWorld, 
		int primitive_type,	unsigned int primitive_count, 
		const void* vertex_data, unsigned int vertex_stride);
	// ��ʾ����
	bool DrawTexture(IShaderTex* pTex, int left, int top, 
		int right, int bottom);
	// ����Χ��
	bool DrawBoundBox(const FmMat4& mat_global, const FmVec3& center, 
		const FmVec3& box_size, const FmVec3& vColor);

	// �����豸����ص�
	bool SetDeviceErrorCallback(device_error_callback_t func, 
		void* context);

	// ��ʾ�豸��������
	bool RaiseDeviceError(int error, const char* info);

	// ��������Դ������ͼ����
	IStaticTex* CreateStaticTexFrom(fm_uint nTex);
	IStaticCube* CreateStaticCubeFrom(fm_uint nTex);
	// �ͷ���Դ
	bool ReleaseResource(int index);

	// �����ͼ
	CTexture* GetTextureById(const PERSISTID& id);
	// ����������ͼ
	CTexture* CreateGlyphTexture(const char* name, bool async);
	// ����������ͼ
	// ��һ��Сͼ��������ͼ��
	CTexture* CreateGlyphTexture(const char* name, int x, int y, const char* src_name, bool async);
	// �ͷ���ͼ
	bool ReleaseTexture(CTexture* pTex);

	bool DestroyTexture(CTexture* pTex);

	// ����һ�����������
 	ITextureSampler* CreateTextureSamper(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D,
		ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag = ITextureSampler::TS_MAG_LINEAR,
		ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min = ITextureSampler::TS_MIN_LINEAR,
		ITextureSampler::TEX_WRAP_MODE u_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE,
		ITextureSampler::TEX_WRAP_MODE v_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE);

	// ���Ѵ��ڴ��ڵĲ������в�ѯ״̬������
	ITextureSampler* CreateTextureSamperFromShaderTex(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D);

	// Color RT���������������������)
	bool CopyColorRT(IColorRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	// �����ͼ����
	bool CopyDepthRT(IDepthRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	// ���������ͼ
	bool CopyDepthStencilRT(IDepthStencilRT* src,IColorRT* dest,const char* pstrIdent = NULL);

	bool DrawAxisAlignedQuad(const char* pstrIdent = NULL);

	bool DrawHalfCube(const char* pstrIdent = NULL);

	CPaintBatchs* GetPaintBatchs() const { return m_pPaintBatchs; }

	// �����Ⱦ��������
	CRenderTool* GetRenderTool() const { return m_pRenderTool; }
	// ��õ�ǰ����ɫ
	FmVec4 GetBackColor() const { return m_vBackColor; }
	// ��õ�ǰ֡����
	size_t GetFrameId() const { return m_nFrameId; }

    //��ȡ��ǰ��Ⱦ��״̬
	IRenderStateOp* GetRenderStateOp() const;

	//��ȡ��ǰ��Ⱦ��ز���
    IRenderDrawOp*  GetRenderDrawOp() const;

	IRenderStateRecordOp* GetRenderStateRecordOp() const;

	//��ȡ��ǰ������������󶨲���
    CTextureSamplerBindOp* GetTextureSamplerBindOp() const;

	fmxStreamFile* GetfmxStreamFileOp() const;

	// ���õ�ǰ
	void UsedShader(IShaderProgram* pShader);
    // 
	IShaderProgram* GetUsedShader() const;

	// ���õ�ǰfbo
	void UsedFrameRT(IFrameRT* pFrameRT);

	IFrameRT* GetUsedFrameRT() const;

    // �����й�һЩ�ǳ�С���ڴ泤��С��128���ֽڵ�����
	// ʹ���ڴ��
    void* Alloc(size_t size);

    void Free(void* ptr, size_t size);

#ifdef PROCESS_LOST_DEVICE
	// �豸�ָ�ʱ�Ĵ���
	bool Restore();
	// �豸��ʧʱ�Ĵ���
	bool Invalidate();

	bool AddDeviceLostCallback(void* context, 
		DEVICE_LOST_CALLBACK invalidate_func, DEVICE_LOST_CALLBACK restore_func);

	// �Ƴ��豸��ʧ�ص�����
	bool RemoveDeviceLostCallback(void* context);
#endif

	DeviceCaps* GetDeviceCaps();

	// ���һ����ļ�
	virtual bool FindCache(const char* file_name);
	// ���ػ����ļ�
	virtual bool LoadCache(const char* file_name, bool async_load);
	// ж�ػ����ļ�
	virtual bool UnloadCache(const char* file_name);
	// ж�����еĻ����ļ�
	virtual bool UnloadAllCache();
	// �����ļ��Ƿ�ɹ�����
	virtual bool GetCacheLoaded(const char* file_name);
	// �����ļ��Ƿ�������أ��ɹ���ʧ�ܣ�
	virtual bool GetCacheLoadFinish(const char* file_name);
	// ��û����ļ�����
	virtual const void* GetCacheData(const char* file_name, size_t& size);
	// ��û����ļ��б�
	virtual size_t GetCacheList(IVarList& result);

	// ���һ��������ļ�
	virtual bool FindCacheIni(const char* file_name);
	// ���ػ��������ļ�
	virtual bool LoadCacheIni(const char* file_name, bool create_hash, 
		bool async_load);
	// ж�ػ��������ļ�
	virtual bool UnloadCacheIni(const char* file_name);
	// ж�����еĻ��������ļ�
	virtual bool UnloadAllCacheIni();
	// ���������ļ��Ƿ�ɹ�����
	virtual bool GetCacheIniLoaded(const char* file_name);
	// ���������ļ��Ƿ�������أ��ɹ���ʧ�ܣ�
	virtual bool GetCacheIniLoadFinish(const char* file_name);
	// ��û��������ļ�����
	virtual IIniFile* GetCacheIni(const char* file_name);
	// ץȡһ֡����
    virtual void CaptureFrame();
	// �Ƿ���Ҫץ֡��������
	bool IsCaptureFrame();

	//����2���ӿ��ṩ�ڲ���Ⱦ�����Դ�Ĳ�ѯ
	size_t GetResourceNum();
	IResource* GetResource(size_t index);
private:
 
    // ֱ��ȫ���������Ļ��
    bool Print2D(IColorRT* src,const char* pstrIdent);

	// ֱ�ӻ��Ƶ�ȫ������
	bool Print2D(IDepthRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent);

	// ֱ�ӻ��Ƶ�ȫ������
	bool Print2D(IDepthStencilRT* src,Render::copy_depth_shaderhandle_t* pShaderHandle,const char* pstrIdent);

	// �����豸����
	bool CreateDeviceBuffer();
	// �ͷ��豸����
	bool ReleaseDeviceBuffer();

	// ��ʼ����Ⱦ״̬���
	void InitRenderState();

    void InitCopyRT();

	// �����Դ�Ŀ���λ������
	int GetResourceIndex();
	
	// ����������ɫ��
	CVertexShader* CreateVertexShader(const void* shader_data,
		unsigned int shader_size);
	// ����������ɫ��
	CPixelShader* CreatePixelShader(const void* shader_data, 
		unsigned int shader_size);

	// ������ɫ������
	int LoadShader(const char* file_name, const char* func_name,
		const char* define);
	// ������ɫ���ļ�
	bool FindShaderFileIndex(const char* file_name, size_t& index) const;

	// ��ȡ��ǰ��copydepthrtshader
	Render::copy_depth_shaderhandle_t* GetShaderHandle(bool bENCODE_RGB888,bool bENCODE_RGBA8888,bool bENCODE_R16,bool bENCODE_R32);

    // ��ȡ��ǰshader����Ϣ����
	size_t GetShaderInfoNum();
	IResource* GetShaderInfo(size_t index);

	// �����Ƿ����ץ֡����
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

	//����֡����
	fmxStreamFile*        m_pfmxStreamFileOp;

	IFrameRT*       m_pUsedFrameRT;
    //���ڼ�¼����Ͳ������󶨹�ϵ
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
