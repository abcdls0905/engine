
#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/fast_str.h"
#include "../visual/fm_define.h"
#include "../visual/i_texture.h"
#include "tex_tools.h"
#include <assert.h>
#include "../utils/lock_util.h"
#include "common_device.h"

// 贴图资源

class Render;
class IResLoader;

class TraceTexture
{
	unsigned char* m_data;
	unsigned long m_size;
	void Destroy()
	{
		if(m_data)
		{
			free(m_data);
			m_data = NULL;
		}
	}
public:
	TraceTexture()
		:m_data(NULL)
	{}

	~TraceTexture()
	{
		Destroy();
	}

	int Create(const char* file_name, const void* data, unsigned int data_size)
	{
		_ALPHA_INFO alpha_info;
		if(!CreateTextureAlphaFromData(file_name, data, data_size, &alpha_info))
			return 0;
		m_data  = alpha_info.Data;
		m_size = alpha_info.DataSize;
		return alpha_info.DataSize;
	}

	int GetAlpha(float u, float v, int src_width, int src_height)
	{
		if(m_data == NULL )
			return 0xff;
		int x = (int)(src_width * u);
		int y = int(src_height * v);
		int index = y*src_width + x;
		if(index < 0 || index >= (int)m_size)
			return 0xff;
		return m_data[index];
	}
};

class CTexture: public ITexture
{
	//只保存通道信息
	int m_datasize;
	TraceTexture m_TraceTexture;
	bool m_async;
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// 初始状态
		STATE_LOADING,	// 正在加载
		STATE_READY,	// 就绪
		STATE_FAILED,	// 加载失败
	};

public:
	static CTexture* NewInstance(Render* pRender,IShaderTex* pTempShaderTex);

public:
	CTexture(Render* pRender,IShaderTex* pTempShaderTex);
	virtual ~CTexture();

	// 释放
	virtual void Release();
	
	void SetAsync(bool async)
	{
		m_async = async;
	}

#ifdef GPU_MULTITHREADING
	void WaitThread()
	{
		g_ResourceContext->ResThreadWaitSync(this->m_lock, this->m_eglSync);
	}

	void WaitMainThread()
	{
		
		if(g_ResourceContext && m_eglSync)
		{
			if(!g_ResourceContext->MainThreadWaitSync(this->m_lock, this->m_eglSync))
				return ;
			m_eglSync = 0;
		}
	}
#endif
    
	// 是否就绪
	virtual bool IsReady()
	{
#ifdef GPU_MULTITHREADING
		return (m_nState == STATE_READY) && (m_eglSync == 0);
#else
		return (m_nState == STATE_READY);
#endif
	}

	// 是否加载结束（成功或失败）
	virtual bool IsLoadComplete()
	{
		return IsReady() || (m_nState == STATE_FAILED);
	}
	
	// 对象号
	virtual PERSISTID GetID() { return m_PersistID; }
	// 获得名称
	virtual const char* GetName() { return m_strName.c_str(); }
	// 贴图宽度
	virtual unsigned int GetWidth() { return m_nWidth; }
	// 贴图高度
	virtual unsigned int GetHeight() { return m_nHeight; }
	// 贴图实际宽度
	virtual unsigned int GetOriginWidth() { return m_nOriginWidth; }
	// 贴图实际高度
	virtual unsigned int GetOriginHeight() { return m_nOriginHeight; }
	// 贴图格式
	virtual unsigned int GetFormat() { return m_nFormat; }
	// 获得贴图
	virtual IShaderTex* GetShaderTex() { return m_pShaderTex; }

	// 销毁
	void Destroy();

	// 同步创建
	bool Create();
	// 是否可以异步加载
	bool CanAsync();
	// 开始异步创建 
	bool BeginCreate();
	// 结束异步创建 
	bool EndCreate(int category, bool succeed);
	// 获得异步加载器
	IResLoader* GetLoader();

	// 同步创建环境贴图
	bool CreateCube();
	// 开始异步创建环境贴图
	bool BeginCreateCube();

	// 创建新贴图
	bool CreateNew(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format, const void* tex_data, 
		unsigned int data_size);
	// 创建动态变化的贴图
	bool CreateDynamic(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format);
	// 创建作为渲染目标的贴图
	bool CreateRenderTarget(unsigned int width, unsigned int height,
		TEX_FORMAT format);
	
	// 设置数据
	bool Build(fm_uint& texture);
	bool Build(unsigned int width, unsigned int height, unsigned int origin_width, unsigned int origin_height, TEX_FORMAT format, fm_uint tex);
	bool Build(const char* file_name, const void* data, unsigned int data_size, const void* data_mask, unsigned int data_mask_size);
	bool BuildCube(unsigned int width, unsigned int height, TEX_FORMAT format, fm_uint tex);
	bool BuildCube(const char* file_name, const void* data, unsigned int data_size);


	// 获得测试透明度的贴图
	unsigned int GetTextureAlpha(float u, float v); 

	// 获得渲染器
	Render* GetRender() const { return m_pRender; }

	// 设置名称
	void SetName(const char* value)
	{ 
		Assert(value != NULL);
		
		m_strName = value; 
	}

	// 是否保持原有尺寸
	void SetOriginSize(bool value) { m_bOriginSize = value; }
	bool GetOriginSize() const { return m_bOriginSize; }

	// 设置对象号
	void SetID(const PERSISTID& value) { m_PersistID = value; }

	// 状态
	void SetState(int value) 
	{ 
		m_nState = value; 
	}

	int GetState() const { return m_nState; }

	unsigned int GetSize() const { return 0; }

	// 获取可用贴图(此接口为了模拟D3D的设置空纹理方式,采用默认1X1的一个临时纹理作为代替纹理)
	virtual IShaderTex* GetCanUseShaderTex() ;



private:
	CTexture(const CTexture&);
	CTexture& operator=(const CTexture&);
	
private:
	Render* m_pRender;
	IResLoader* m_pLoader;
	core_string m_strName;
	bool m_bOriginSize;
	PERSISTID m_PersistID;
	int m_nState;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nOriginWidth;
	unsigned int m_nOriginHeight;
	TEX_FORMAT m_nFormat;
	IShaderTex* m_pShaderTex;
	IShaderTex* m_pTempShaderTex;
	CLockUtil m_lock;
	void* m_eglSync;
};

#endif // _TEXTURE_H
