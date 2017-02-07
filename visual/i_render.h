#ifndef _I_RENDER_H
#define _I_RENDER_H

#include "i_resource.h"
#include "i_res_loader.h"
#include "i_texture.h"
#include "i_scene_view.h"
#include "i_image_access.h"
#include "../public/i_interface.h"

// �豸����ص�����ԭ��
typedef bool (*device_error_callback_t)(void* context, int thread_id,
										int error, const char* info);


class IRender: public IInterface
{
public:
#ifdef PROCESS_LOST_DEVICE
	// �豸��ʧ�ص�����ԭ��
	typedef bool (*DEVICE_LOST_CALLBACK)(void* context);

	struct device_lost_call_t
	{
		void* pContext;
		DEVICE_LOST_CALLBACK pInvalidateFunc;
		DEVICE_LOST_CALLBACK pRestoreFunc;
	};
#endif
	// �豸����
	virtual const char* GetDeviceName() = 0;

	// ������Ⱦ�豸
	virtual bool CreateDevice(bool multithreading) = 0;
	// ���������豸�ߴ�
	virtual bool ResetDevice() = 0;
	// ���µ�ǰ���ڵĳߴ�
	virtual bool UpdateWindowSize() = 0;

	// �������ͳ������(1���ӵ�����)
	virtual performance_info_t* GetPerformance() = 0;
	
	// �������ͳ������(�ڴ�)
	virtual performance_mem_t* GetMemPerformance() = 0;

	// ��Դ�����߳�����
	virtual void SetResourceThreadNum(int value) = 0;
	virtual int GetResourceThreadNum() = 0;

	// �����첽������Դ
	virtual bool RequestLoadResource(int category, IResLoader* loader) = 0;
	// ��õ�ǰ��Դ������������
	virtual int GetResourceLoadTaskNum() = 0;

	// ��ʾ�豸�ߴ�
	virtual void SetDeviceSize(int width, int height) = 0;
	// ��ȡ��ǰ�ֱ���
	virtual int GetDeviceWidth() = 0;
	virtual int GetDeviceHeight() = 0;

	// ������Դ���ַ����ļ�·����ǰ׺
	virtual const char* GetPathPrefix(const char* name) = 0;

	// ��������ƽ̨���ص�ǰ������Դ����չ��
	virtual const char* GetTexExtfix() = 0;

	// ��ȡ�豸��Ϣ
	virtual PERSISTID GetDeviceCapsID() const = 0;
	// ����������ͼ
	virtual ISceneView* CreateSceneView(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent = NULL) = 0;
	// ���õ�ǰ������ͼ
	virtual void SetSceneView(ISceneView* pSceneView) = 0;
	// ��õ�ǰ������ͼ
	virtual ISceneView* GetSceneView() = 0;
	// ���ӵ�ǰ֡�ĳ�����ͼ
	virtual void AddSceneView(ISceneView* pSceneView) = 0;

	// ������Ⱦ������
	virtual IRenderContext* CreateContext() = 0;
	// ���õ�ǰ��Ⱦ������
	virtual void SetContext(IRenderContext* pContext) = 0;
	// ��õ�ǰ��Ⱦ������
	virtual IRenderContext* GetContext() = 0;

	// ������̬���㻺�� ���һ�������û��������ʹ��
	virtual IStaticVB* CreateStaticVB(const void* pdata, unsigned int size)= 0;
	// ������̬��������
	virtual IStaticIB* CreateStaticIB(const void* pdata, unsigned int size)= 0;
	// ������̬���㻺��
	virtual IDynamicVB* CreateDynamicVB(unsigned int size,const char* pstrIdent = NULL )= 0;
	// ������̬��������
	virtual IDynamicIB* CreateDynamicIB(unsigned int size,const char* pstrIdent = NULL)= 0;
	// ������̬��ͼ
	virtual IStaticTex* CreateStaticTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format,
		const void* tex_data, unsigned int data_size)= 0;
	// ������̬�仯����ͼ
	virtual IDynamicTex* CreateDynamicTex(unsigned int width, 
		unsigned int height, unsigned int levels, TEX_FORMAT format)= 0;
	// ������ɫ��ȾĿ��
	virtual IColorRT* CreateColorRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;
	// �����Զ�����MIPMAP����ɫ��ȾĿ��
	virtual IColorRT* CreateMipmapColorRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;
	// ���������ȾĿ��
	virtual IDepthRT* CreateDepthRT(unsigned int width, 
		unsigned int height, TEX_FORMAT format, int type)= 0;

	// �������ɲ�����ColorTex
	virtual IColorTex* CreateColorTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// �������ɲ�����DepthTex
	virtual IDepthTex* CreateDepthTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// ���������Բ�����ģ��ͼ
	virtual IStencilTex* CreateStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// ���������Բ�����ģ��ͼ
    virtual IDepthStencilTex* CreateDepthStencilTex(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// �������Բ��������ģ����ͼ
	virtual IDepthStencilRT* CreateDepthStencilRT(unsigned int width,unsigned int height, TEX_FORMAT format, int type) = 0;

	// ����һ��FrameRT color��depth����Ҫ��
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL) = 0;

	// ֻҪ��Ȳ���
	virtual IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthRT* pDepthRT,IStencilTex* pStencilTex = NULL) = 0 ;

	// ֻҪ��ɫ
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex = NULL ) = 0;

	// ����һ��FrameRT color��depthstencil
	virtual IFrameRT* CreateFrameRT(IColorTex* pColorTex,IDepthStencilTex* pDepthStencilTex) = 0;

	// ����һ��FrameRT colortx��depthstencil
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT) = 0;

	//ֻ��Ҫ��ɫ��colorrt
	virtual IFrameRT* CreateFrameRT(IColorRT* pColorRT) = 0;

	// д�붯̬���㻺��
	virtual bool UpdateDynamicVB(IDynamicVB* pVBuffer, unsigned int offset,
		const void* pdata, unsigned int size)= 0;
	// д�붯̬��������
	virtual bool UpdateDynamicIB(IDynamicIB* pIBuffer, unsigned int offset,
		const void* pdata, unsigned int size)= 0;
	// д�붯̬��ͼ
	virtual bool UpdateDynamicTex(IDynamicTex* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata) = 0 ;

	// ���ض�����ɫ��
	virtual IVertexShader* LoadVertexShader(const char* file_name, 
		const char* func_name, const char* define) = 0;
	// ����������ɫ��
	virtual IPixelShader* LoadPixelShader(const char* file_name, 
		const char* func_name, const char* define)= 0;
	// ������ɫ������
	virtual IShaderProgram* CreateShaderProgram(IVertexShader* pVS, IPixelShader* pPS,
		const char* element_names[], unsigned int element_num) = 0;

	// ������ͼ���ļ�
	virtual bool SaveTextureToFile(IShaderTex* pShaderTex, 
		const char* file_name) = 0;

	// ���ָ�����ֵ���ͼ
	virtual ITexture* FindTexture(const char* name) = 0;
	// ������ͼ
	virtual ITexture* CreateTexture(const char* name, bool async) = 0;
	// ������������ͼ
	virtual ITexture* CreateCubeTexture(const char* name, bool async) = 0;
	// ���������ͼ
	virtual ITexture* CreateVolumeTexture(const char* name, bool async) = 0;
	// ��������ͼ
	virtual ITexture* CreateNewTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format, const void* tex_data, unsigned int data_size) = 0;
	// ������̬��ͼ
	virtual ITexture* CreateDynamicTexture(const char* name, 
		unsigned int width, unsigned int height, unsigned int levels, 
		TEX_FORMAT format) = 0;
	// ������Ϊ��ȾĿ�����ͼ
	virtual ITexture* CreateRenderTexture(const char* name, 
		unsigned int width, unsigned int height, TEX_FORMAT format) = 0;
	// д�붯̬��ͼ
	virtual bool UpdateDynamicTexture(ITexture* pTexture, int level, 
		unsigned int offset_x, unsigned int offset_y, unsigned int width, 
		unsigned int height, const void* pdata) = 0;

	// �����ͼָ��λ�õ�͸����
	virtual unsigned int GetTextureAlpha(ITexture* pTexture, float u, float v) = 0;

	//��ȡ��ǰ��Ⱦ��״̬
	virtual IRenderStateOp* GetRenderStateOp() const = 0; 

	//��ȡ��ǰ��Ⱦ��ز���
	virtual IRenderDrawOp*  GetRenderDrawOp() const =  0;

	//��ȡ��ǰ״̬��¼������
//	virtual IRenderStateRecordOp* GetRenderStateRecordOp() const = 0;

	// ����Χ��
	virtual bool DrawBoundBox(const FmMat4& mat_global, const FmVec3& center, 
		const FmVec3& box_size, const FmVec3& vColor) = 0;

	virtual void BeginFrame(unsigned int back_color) = 0;
	virtual void EndFrame() = 0;

	// �����豸����ص�
	virtual bool SetDeviceErrorCallback(device_error_callback_t func, 
		void* context) = 0;

	// ���������Թ���
	virtual void SetMaxAnisotropy(int value) = 0;
	virtual int GetMaxAnisotroy() = 0;

	// �Ƿ����������Թ���
	virtual void SetAnisotropic(bool value) = 0;
	virtual bool GetAnisotropic() = 0;

	// �Ƿ���ʾ����
	virtual void SetWireFrame(bool value) = 0;
	virtual bool GetWireFrame() = 0;

    // �Ƿ���ʾ�����������Ա�ǩ��ģ��
    virtual void SetShowHideModel(bool value) = 0;
    virtual bool GetShowHideModel() = 0;

	// ��ײ���ʱ��ALPHA���Բο�ֵ
	virtual void SetTraceAlphaRef(int value) = 0;
	virtual int GetTraceAlphaRef() = 0;

	// ��Ⱦʱ�ı�׼ALPHA���Բο�ֵ
	virtual void SetRenderAlphaRef(int value) = 0;
	virtual int GetRenderAlphaRef() = 0;

	// ��ͼ����
	virtual void SetTextureLod(int value) = 0;
	virtual int GetTextureLod() = 0;

	// ʹ�ö����ĸ�����ͼ���ݽ�����ײ��⣨�Ӷ���������������Ⱦ����ͼ��
	virtual void SetTraceAloneTexture(int value) = 0;
	virtual int GetTraceAloneTexture() = 0;

	// �Ƿ��첽������Դ
	virtual void SetAsyncLoadResource(bool value) = 0;
	virtual bool GetAsyncLoadResource() = 0;

	// �Ƿ�ʹ�úڰ׸�����������ͼ
	virtual void SetBlackWhiteDiffuse(bool value) = 0;
	virtual bool GetBlackWhiteDiffuse() = 0;

	// ���ز�������
	virtual void SetMultiSampleType(int type) = 0;
	virtual int GetMultiSampleType() = 0;

	// ���¼�������Ч���ļ�
	virtual bool ReloadAllShader() = 0;
	// �������е���Դ��Ϣ
	virtual bool DumpResource(const char* file_name) = 0;
	// ���һЩ��Ⱦ������ʹ�õ���ʱ��Դ
	virtual bool ClearRenderResource() = 0;
	// ��Ļ��ͼ
	virtual bool ScreenShot(const char* name) = 0;
	// ��Ļ��ͼ��ָ������
	virtual bool ScreenShot(const char* name, int x, int y, int w, int h) = 0;
	// �Ƿ�Ծ�����Ҷ������Ե�ģ������ײ���
	virtual void SetTraceTreeLeaf(bool value) = 0;
	virtual bool GetTraceTreeLeaf() = 0;
	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value) = 0;
	virtual bool GetUseLightMap() = 0;

	// ����������
	virtual bool DrawDesignLine(const FmMat4& mtxWorld, 
		int primitive_type,	unsigned int primitive_count, 
		const void* vertex_data, unsigned int vertex_stride) = 0;

	// ����׷��ʱ�Ƿ񷵻���ͼ��
	virtual void SetTraceTextureName(bool value) = 0;
	virtual bool GetTraceTextureName() = 0;

	// �Ƿ�ֻ�����������ν��й���׷��
	virtual void SetTraceOnlyFront(bool value) = 0;
	virtual bool GetTraceOnlyFront() = 0;

	// ����ͼƬ��д��
	virtual IImageAccess* CreateImageAccess() = 0;

	// Color RT���� ���destΪNULL�����ֱ�ӻ��Ƶ���ǰFBO����
	virtual bool CopyColorRT(IColorRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	// Color RT���� ���destΪNULL�����ֱ�ӻ��Ƶ���ǰFBO���� �ø�ʽĿǰֻ֧��RGB888 ����RGBA8888 
	// ������Ҫע����ǹ���copy���ͼ��ʱ��,�ײ��������ͼ��ʱ��ת��Ϊ������ȣ������Զ���ȡ��ǰ��ʹ�õ�GetSceneView()�µĹ������Ⱦ�����������������н���
	// ���￼�������ԣ�����ֱ�Ӻ����������Ĵ���
	virtual bool CopyDepthRT(IDepthRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	virtual bool CopyDepthStencilRT(IDepthStencilRT* src,IColorRT* dest,const char* pstrIdent = NULL) = 0;

	// ����ȫ��һ�������� 
	virtual bool DrawAxisAlignedQuad(const char* pstrIdent = NULL) = 0;

	virtual bool DrawHalfCube(const char* pstrIdent = NULL) = 0;

	//���ݿɲ�����ͼ��һ��������
	virtual ITextureSampler* CreateTextureSamper(IShaderTex* pBindShaderTex,ITextureSampler::TEX_TYPE tex_type = ITextureSampler::TEX_2D,\
														ITextureSampler::TEX_SAMPLER_MAG_FILTER tex_mag = ITextureSampler::TS_MAG_LINEAR,\
														ITextureSampler::TEX_SAMPLER_MIN_FILTER tex_min = ITextureSampler::TS_MIN_LINEAR,\
														ITextureSampler::TEX_WRAP_MODE u_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE,\
														ITextureSampler::TEX_WRAP_MODE v_wrap_mode = ITextureSampler::TWM_CLAMP_TO_EDGE) = 0;


#ifdef PROCESS_LOST_DEVICE
	// �豸�ָ�ʱ�Ĵ���
	virtual bool Restore() = 0;

	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate() = 0;

	virtual bool AddDeviceLostCallback(void* context, 
		DEVICE_LOST_CALLBACK invalidate_func, DEVICE_LOST_CALLBACK restore_func) = 0;

	// �Ƴ��豸��ʧ�ص�����
	virtual bool RemoveDeviceLostCallback(void* context) = 0;
#endif

	// ���õ�ǰfbo
	virtual void UsedFrameRT(IFrameRT* pFrameRT) = 0;

	virtual IFrameRT* GetUsedFrameRT() const = 0;

	// ���һ����ļ�
	virtual bool FindCache(const char* file_name) = 0;
	// ���ػ����ļ�
	virtual bool LoadCache(const char* file_name, bool async_load) = 0;
	// ж�ػ����ļ�
	virtual bool UnloadCache(const char* file_name) = 0;
	// ж�����еĻ����ļ�
	virtual bool UnloadAllCache() = 0;
	// �����ļ��Ƿ�ɹ�����
	virtual bool GetCacheLoaded(const char* file_name) = 0;
	// �����ļ��Ƿ�������أ��ɹ���ʧ�ܣ�
	virtual bool GetCacheLoadFinish(const char* file_name) = 0;
	// ��û����ļ�����
	virtual const void* GetCacheData(const char* file_name, size_t& size) = 0;
	// ��û����ļ��б�
	virtual size_t GetCacheList(IVarList& result) = 0;
	// ���һ��������ļ�
	virtual bool FindCacheIni(const char* file_name) = 0;
	// ���ػ��������ļ�
	virtual bool LoadCacheIni(const char* file_name, bool create_hash, 
		bool async_load) = 0;
	// ж�ػ��������ļ�
	virtual bool UnloadCacheIni(const char* file_name) = 0;
	// ж�����еĻ��������ļ�
	virtual bool UnloadAllCacheIni() = 0;
	// ���������ļ��Ƿ�ɹ�����
	virtual bool GetCacheIniLoaded(const char* file_name) = 0;
	// ���������ļ��Ƿ�������أ��ɹ���ʧ�ܣ�
	virtual bool GetCacheIniLoadFinish(const char* file_name) = 0;
	// ��û��������ļ�����
	virtual IIniFile* GetCacheIni(const char* file_name) = 0;
    // ��ȡ��ǰshader����Ϣ����
	virtual size_t GetShaderInfoNum() = 0;
	// ��ȡshader��Դ
	virtual IResource* GetShaderInfo(size_t index) = 0;
	// ץȡһ֡����
	virtual void CaptureFrame() = 0;
	// ������ͼ����ʱ��
	virtual void SetTextureCacheTime(float time) = 0;
	// �����ǰ��ͼ����
	virtual void ClearTextureCache() = 0;
};
#endif