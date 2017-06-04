
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

// ��ͼ��Դ

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
	//ֻ����ͨ����Ϣ
	int m_datasize;
	TraceTexture m_TraceTexture;
	bool m_async;
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// ��ʼ״̬
		STATE_LOADING,	// ���ڼ���
		STATE_READY,	// ����
		STATE_FAILED,	// ����ʧ��
	};

public:
	static CTexture* NewInstance(Render* pRender,IShaderTex* pTempShaderTex);

public:
	CTexture(Render* pRender,IShaderTex* pTempShaderTex);
	virtual ~CTexture();

	// �ͷ�
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
    
	// �Ƿ����
	virtual bool IsReady()
	{
#ifdef GPU_MULTITHREADING
		return (m_nState == STATE_READY) && (m_eglSync == 0);
#else
		return (m_nState == STATE_READY);
#endif
	}

	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete()
	{
		return IsReady() || (m_nState == STATE_FAILED);
	}
	
	// �����
	virtual PERSISTID GetID() { return m_PersistID; }
	// �������
	virtual const char* GetName() { return m_strName.c_str(); }
	// ��ͼ���
	virtual unsigned int GetWidth() { return m_nWidth; }
	// ��ͼ�߶�
	virtual unsigned int GetHeight() { return m_nHeight; }
	// ��ͼʵ�ʿ��
	virtual unsigned int GetOriginWidth() { return m_nOriginWidth; }
	// ��ͼʵ�ʸ߶�
	virtual unsigned int GetOriginHeight() { return m_nOriginHeight; }
	// ��ͼ��ʽ
	virtual unsigned int GetFormat() { return m_nFormat; }
	// �����ͼ
	virtual IShaderTex* GetShaderTex() { return m_pShaderTex; }

	// ����
	void Destroy();

	// ͬ������
	bool Create();
	// �Ƿ�����첽����
	bool CanAsync();
	// ��ʼ�첽���� 
	bool BeginCreate();
	// �����첽���� 
	bool EndCreate(int category, bool succeed);
	// ����첽������
	IResLoader* GetLoader();

	// ͬ������������ͼ
	bool CreateCube();
	// ��ʼ�첽����������ͼ
	bool BeginCreateCube();

	// ��������ͼ
	bool CreateNew(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format, const void* tex_data, 
		unsigned int data_size);
	// ������̬�仯����ͼ
	bool CreateDynamic(unsigned int width, unsigned int height, 
		unsigned int levels, TEX_FORMAT format);
	// ������Ϊ��ȾĿ�����ͼ
	bool CreateRenderTarget(unsigned int width, unsigned int height,
		TEX_FORMAT format);
	
	// ��������
	bool Build(fm_uint& texture);
	bool Build(unsigned int width, unsigned int height, unsigned int origin_width, unsigned int origin_height, TEX_FORMAT format, fm_uint tex);
	bool Build(const char* file_name, const void* data, unsigned int data_size, const void* data_mask, unsigned int data_mask_size);
	bool BuildCube(unsigned int width, unsigned int height, TEX_FORMAT format, fm_uint tex);
	bool BuildCube(const char* file_name, const void* data, unsigned int data_size);


	// ��ò���͸���ȵ���ͼ
	unsigned int GetTextureAlpha(float u, float v); 

	// �����Ⱦ��
	Render* GetRender() const { return m_pRender; }

	// ��������
	void SetName(const char* value)
	{ 
		Assert(value != NULL);
		
		m_strName = value; 
	}

	// �Ƿ񱣳�ԭ�гߴ�
	void SetOriginSize(bool value) { m_bOriginSize = value; }
	bool GetOriginSize() const { return m_bOriginSize; }

	// ���ö����
	void SetID(const PERSISTID& value) { m_PersistID = value; }

	// ״̬
	void SetState(int value) 
	{ 
		m_nState = value; 
	}

	int GetState() const { return m_nState; }

	unsigned int GetSize() const { return 0; }

	// ��ȡ������ͼ(�˽ӿ�Ϊ��ģ��D3D�����ÿ�����ʽ,����Ĭ��1X1��һ����ʱ������Ϊ��������)
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
