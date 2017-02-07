//--------------------------------------------------------------------
// �ļ���:		render.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��1��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "render.h"
#include "context.h"
#include "scene_view.h"
#include "paint_batch.h"
#include "helper.h"
#include "image_access.h"
#include "tex_factory.h"
#include "res_thread.h"
#include "render_tool.h"
#include "color_rt.h"
#include "depth_rt.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

#include "render_state_op.h"
#include "render_draw_op.h"
#include "tex_tools.h"

#include "tex_sampler.h"

#define FREEIMAGE_LIB
#include "freeimage.h"

#include "../public/portable.h"
#include "common_device.h"

#include "render_cmd_op.h"

#ifdef GPU_MULTITHREADING
#include "texture.h"
#endif

#define performance_time Port_GetPerformanceTime

DECLARE_FUNCTION_INTERFACE(Render);

/// entity: IVisBase
/// \brief �ռ���Ӷ������
DECLARE_ABSTRACT_ENTITY(IVisBase, 0, IEntity);

// ���ض�����ɫ��
IVertexShader* render_load_vs(IRender* pRender, const char* file_name,
	const char* func_name, const char* defines)
{
	return pRender->LoadVertexShader(file_name, func_name, defines);
}

// ����������ɫ��
IPixelShader* render_load_ps(IRender* pRender, const char* file_name,
	const char* func_name, const char* defines)
{
	return pRender->LoadPixelShader(file_name, func_name, defines);
}

// Render

Render::Render()
{
	m_DesginLineShader = NULL;
	m_DesignLineVB = NULL;
	m_bAsyncLoadResource = true;
	m_bBlackWhiteDiffuse = false;
	m_bWireFrame = false;
	m_bShowHideModel = false;
	m_bTraceTreeLeaf = true;
	m_nRenderAlphaRef = 128;
	m_nTraceAlphaRef = 0;
	m_bTraceTextureName = false;
	m_bTraceOnlyFront = false;
	m_bUseLightMap = true;
	m_nTraceAloneTexture = 1;
	
	m_pCurrentContext = NULL;
	m_pCurrentSceneView = NULL;
	m_pDeviceErrorFunc = NULL;
	m_pDeviceErrorContext = NULL;

	memset(&m_Performance, 0, sizeof(m_Performance));
	memset(&m_PerformanceMem, 0, sizeof(performance_mem_t));
	memset(m_pResThreads, 0, sizeof(m_pResThreads));
	m_nResThreadNum = 1;
	m_nResThreadIndex = 0;
	m_pResThreads[0] = CORE_NEW(CResThread);
	
	m_nTextureLod = 0;
	m_pTexFactory = CTexFactory::NewInstance(this);

	m_nWinHeight = 800;
	m_nWinWidth = 600;
	m_nDeviceHeight = 800;
	m_nDeviceWidth = 600;
	m_bAnisotropic = false;
	m_nMaxAnisotropy = 1;
	m_nMultiSampleType = 1;

	m_nFrameId = 0;
	m_vBackColor = FmVec4(0.0F, 0.0F, 0.0F, 0.0F);
	m_pRenderTool = CRenderTool::NewInstance(this);
	m_pPaintBatchs = CPaintBatchs::NewInstance(this);

	m_strTexExtfix = "";

	m_nDefautFBO = 0;
	m_pUsedShader = NULL;
	m_pUsedFrameRT = NULL;
	m_pRenderStateOp = NULL;
	m_pRenderStateRecordOp = NULL;

	m_pfmxStreamFileOp = NULL;

	m_pCopyColorRTShader = NULL;
 	m_pCopyColorFrameRT = NULL;
	m_pCopyDepthFrameRT = NULL;

	m_device_state = DEVICE_STATE_READY;

	m_bIsCaptureFrame = false;

#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif

	m_pfmxStreamFileOp = fmxStreamFile::NewInstance();
}

Render::~Render()
{
	if(m_pfmxStreamFileOp)
		m_pfmxStreamFileOp->Release();
	m_pfmxStreamFileOp = NULL;
	//�ͷ�������ص��ڴ�
#define shadermap 	TPodHashMap<unsigned int, Render::copy_depth_shaderhandle_t*, TPodTraits<unsigned int>,TCopyDepthShaderAlloc>
	for(shadermap::iterator iter = m_CopyDepthShaderMapHandle.Begin(); iter != m_CopyDepthShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(Render::copy_depth_shaderhandle_t));
	}   
#undef shadermap

	SAFE_RELEASE(m_pCopyColorRTShader)
    SAFE_RELEASE(m_pCopyColorFrameRT);
	SAFE_RELEASE(m_pCopyDepthFrameRT);

	for (int i = 0; i < m_nResThreadNum; ++i)
	{
		m_pResThreads[i]->Shutdown();
		CORE_DELETE(m_pResThreads[i]);
	}

#ifdef _DEBUG
	// δ�ͷŵ���ͼ
	if (m_pTexFactory->GetCount() > 0)
	{
		m_pTexFactory->Dump("texture_leak.log");
	}
#endif // _DEBUG

	SAFE_RELEASE(m_pTexFactory);

	for (size_t k1 = 0; k1 < m_ShaderList.size(); ++k1)
	{
		if (m_ShaderList[k1].pCode)
		{
			CORE_FREE(m_ShaderList[k1].pCode, m_ShaderList[k1].nCodeSize);
		}
	}

	for (size_t k2 = 0; k2 < m_ShaderFiles.size(); ++k2)
	{
		CORE_DELETE(m_ShaderFiles[k2]);
	}
	
	SAFE_RELEASE(m_pPaintBatchs);
	SAFE_RELEASE(m_pRenderTool);

	for (size_t j = 0; j < m_ResourceList.size(); ++j)
	{
		if (m_ResourceList[j])
		{
			m_ResourceList[j]->Destroy();
			m_ResourceList[j] = NULL;
		}
	}
    
	ReleaseDeviceBuffer();

	UnloadAllCache();
	UnloadAllCacheIni();

#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif
}

bool Render::Init()
{
	TextureCache* pTextureCache = (TextureCache*)g_pCore->CreateEntity("TextureCache");
	pTextureCache->Create(this);

	// ������ɫ���ļ�Ŀ¼
	char shader_path[256];

	SafeSprintf(shader_path, sizeof(shader_path), "%seff/", 
		GetCore()->GetProgramPath());
	m_strShaderPath = shader_path;

	// ������Դ�����߳�
	for (int i = 0; i < m_nResThreadNum; ++i)
	{
		m_pResThreads[i]->Initialize();
	}


    
	return true;
}

bool Render::Shut()
{
	if (m_nResThreadNum > 0)
	{
		for (int i = 0; i < m_nResThreadNum; ++i)
		{
			m_pResThreads[i]->Shutdown();
			m_pResThreads[i]->Clear();
			CORE_DELETE(m_pResThreads[i]);
			m_pResThreads[i] = NULL;
		}

		m_nResThreadNum = 0;
	}

	return true;
}

bool Render::NeedExecPerFrame()
{ 
	return true; 
}

void Render::ExecFrameBegin()
{
}

void Render::ExecFrameEnd()
{
	if (!m_bAsyncLoadResource)
	{
		// ǿ��ʹ��ͬ����ʽ������Դ����
		for (int k = 0; k < m_nResThreadNum; ++k)
		{
			m_pResThreads[k]->WorkerFunc();
		}
	}
	
	for (int i = 0; i < m_nResThreadNum; ++i)
	{
		m_pResThreads[i]->Execute();
	}
}

const char* Render::GetDeviceName()
{
	return m_strDeviceName.c_str();
}

int Render::GetDeviceWidth()
{
	return m_nDeviceWidth;
}

int Render::GetDeviceHeight()
{
	return m_nDeviceHeight;
}

int Render::GetWinWidth()
{
	return m_nWinWidth;
}

int Render::GetWinHeight()
{
	return m_nWinHeight;
}

bool Render::GetNeedScaleX()
{
	return (m_nWinWidth != m_nDeviceWidth);
}

bool Render::GetNeedScaleY()
{
	return (m_nWinHeight != m_nDeviceHeight);
}

float Render::GetScaleFactorX()
{
	if (m_nWinWidth == 0)
	{
		return 1.0F;
	}

	return float(m_nDeviceWidth) / float(m_nWinWidth);
}

float Render::GetScaleFactorY()
{
	if (m_nWinHeight == 0)
	{
		return 1.0F;
	}
	
	return float(m_nDeviceHeight) / float(m_nWinHeight);
}

void Render::SetDeviceSize(int width, int height)
{
	m_nDeviceWidth = width;
	m_nDeviceHeight = height;
}

void Render::SetAnisotropic(bool value)
{
	m_bAnisotropic = value;
}

bool Render::GetAnisotropic()
{
	return m_bAnisotropic;
}

void Render::SetMaxAnisotropy(int value)
{
	m_nMaxAnisotropy = value;
}

int Render::GetMaxAnisotroy()
{
	return m_nMaxAnisotropy;
}

void Render::SetMultiSampleType(int type)
{
	if ((type != 1) && (type != 2) && (type != 4) && (type != 8) 
		&& (type != 16))
	{
		return;
	}
	
	m_nMultiSampleType = type;
}

int Render::GetMultiSampleType()
{
	return m_nMultiSampleType;
}

//��ȡ��ǰ��Ⱦ��״̬
IRenderStateOp* Render::GetRenderStateOp() const
{
	return m_pRenderStateOp;
}

//��ȡ��ǰ��Ⱦ��ز���
IRenderDrawOp*  Render::GetRenderDrawOp() const
{
	return m_pRenderDrawOp;
}

//��ȡ��ǰ״̬��¼������
IRenderStateRecordOp* Render::GetRenderStateRecordOp() const
{
    return m_pRenderStateRecordOp;
}

//��ȡ��ǰ������������󶨲���
CTextureSamplerBindOp* Render::GetTextureSamplerBindOp() const
{
	return m_pTextureSamplerBindOp;
}

fmxStreamFile* Render::GetfmxStreamFileOp() const
{
	return m_pfmxStreamFileOp;
}

// ���õ�ǰ
void Render::UsedShader(IShaderProgram* pShader)
{
	m_pUsedShader = pShader;
}

// 
IShaderProgram* Render::GetUsedShader() const
{
	return m_pUsedShader;
}

// ���õ�ǰfbo
void Render::UsedFrameRT(IFrameRT* pFrameRT)
{
	//���Ĭ�����õ�0��Ϊ�ָ�����FBO
	if(pFrameRT == NULL)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_nDefautFBO);
	}//�����Ϊ��
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER,pFrameRT->GetFrame());
	}

	m_pUsedFrameRT = pFrameRT;
}

IFrameRT* Render::GetUsedFrameRT() const
{
	return m_pUsedFrameRT;
}

void* Render::Alloc(size_t size)
{
	return m_mempool.Alloc(size);
}

void Render::Free(void* ptr, size_t size)
{
	m_mempool.Free(ptr,size);
}

bool Render::CreateDeviceBuffer()
{
    //�����豸�����
	DeviceCaps* pDeviceCap = (DeviceCaps*) GetCore()->CreateEntity("DeviceCaps");
//	m_DeviceCapsID = pDeviceCap->GetID();

	m_DeviceCapsID = pDeviceCap->GetID();

	//����豸����
	pDeviceCap->PrintDeviceInfoString();
	//�����ǰ�豸�ֱ���
	char mbzTemp[64];
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"[DeviceSize] %dX%d",m_nDeviceWidth,m_nDeviceHeight);
	CORE_TRACE(mbzTemp);

	//CORE_TRACE(pDeviceCap->GetDeviceInfoString().c_str());
    CORE_TRACE(pDeviceCap->GetCurrentColor_Depth_Stencil_BitInfoString().c_str());


    //���ݻ������е���Ĭ����ͼ��׺��
    //�Ƿ�֧��DDS
	if(pDeviceCap->IsDDSSupported())
	{
		m_strTexExtfix = ".dds";
	}
	else if(pDeviceCap->IsPVRTCSupported())
	{
		m_strTexExtfix = ".pvr";
	}
	else if(pDeviceCap->IsATCSupported())
	{
		m_strTexExtfix = ".ktx";
	}
	else if(pDeviceCap->IsETCSupported())
	{
        m_strTexExtfix = ".pvr";//ʵ����.pvr��ʽΪ���ϸ�ʽҲ֧��etcѹ����ʽ
	}
	else
	{
        m_strTexExtfix = ".tga";
	} 

//#ifdef FX_SYSTEM_WINDOWS
    m_strTexExtfix = ".dds";//ע��������һ����window������ģ���ͳһPC ddsΪ��׼��ͼ�ļ���ʽ��
//#endif

	glViewport(0, 0, m_nDeviceWidth, m_nDeviceHeight);
	//m_pRenderStateOp->SetViewPort(0, 0, m_nDeviceWidth, m_nDeviceHeight);

	TestErr("Render::CreateDeviceBuffer");  
   
	/*
	m_pColorRT = CreateColorRT(m_nDeviceWidth, m_nDeviceHeight,TEX_FORMAT_RGBA8888_UNORM, RT_TYPE_DEVICE);
	m_pDepthRT = CreateDepthRT(m_nDeviceWidth, m_nDeviceHeight,TEX_FORMAT_D24_UINT, RT_TYPE_DEVICE);
    
	m_pFrameRT = CreateFrameRT( m_pColorRT , m_pDepthRT );
	glBindFramebuffer(GL_FRAMEBUFFER, m_pFrameRT->GetFrame());
	*/

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nDefautFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nDefautFBO);
	return true;
}

// ��ʼ����Ⱦ״̬���
void Render::InitRenderState()
{
	m_pRenderStateRecordOp = CRenderStateRecordOp::NewInstance(this);//�Ƚ���״̬������
	m_pRenderStateOp = CRenderStateOp::NewInstance(this);
	m_pRenderDrawOp = CRenderDrawOp::NewInstance(this);


	m_pTextureSamplerBindOp = CTextureSamplerBindOp::NewInstance(this);
	m_pRenderStateOp->SetViewPort(0, 0, m_nDeviceWidth, m_nDeviceHeight);
    //��������Ѿ���ֲ��CRenderStateOp ���캯��������
	/*
	// ������ɫ���
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// �������д��
	glDepthMask(GL_TRUE);
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// �ر�ALPHA���
	glDisable(GL_BLEND);
	// Ĭ�ϻ�Ϸ�ʽ
	// �ر�ģ�����
	glDisable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xFFFFFFFF);
	glDepthFunc(GL_LEQUAL);
	*/
}



const char* copy_depth_rt_vertex_t_name[] = 
{
	"iPos",
	"iUV",
};



void Render::InitCopyRT()
{
	// ��ʼ��CopyDepth
	m_VSCopyDepthList.SetRender(this);
	m_VSCopyDepthList.SetShaderName("copy_depth_rt.vsh", "vs_main_copy_depth");
	m_VSCopyDepthList.SetDefine(ENCODE_RGB888, "#define ENCODE_RGB888 1\n");
	m_VSCopyDepthList.SetDefine(ENCODE_RGBA8888, "#define ENCODE_RGBA8888 1\n");
	m_VSCopyDepthList.SetDefine(ENCODE_R16, "#define ENCODE_R16 1\n");
	m_VSCopyDepthList.SetDefine(ENCODE_R32, "#define ENCODE_R32 1\n");

	m_PSCopyDepthList.SetRender(this);
	m_PSCopyDepthList.SetShaderName("copy_depth_rt.fsh", "ps_main_copy_depth");
	m_PSCopyDepthList.SetDefine(ENCODE_RGB888, "#define ENCODE_RGB888 1\n");
	m_PSCopyDepthList.SetDefine(ENCODE_RGBA8888, "#define ENCODE_RGBA8888 1\n");
	m_PSCopyDepthList.SetDefine(ENCODE_R16, "#define ENCODE_R16 1\n");
	m_PSCopyDepthList.SetDefine(ENCODE_R32, "#define ENCODE_R32 1\n");
}

// ��ȡ��ǰ��copydepthrtshader
Render::copy_depth_shaderhandle_t* Render::GetShaderHandle(bool bENCODE_RGB888,bool bENCODE_RGBA8888,bool bENCODE_R16,bool bENCODE_R32)
{
	size_t shader_flag = (size_t(bENCODE_RGB888)<<ENCODE_RGB888) + (size_t(bENCODE_RGBA8888)<<ENCODE_RGBA8888) + (size_t(bENCODE_R16)<<ENCODE_R16) + (size_t(bENCODE_R32)<<ENCODE_R32);

	if(!m_CopyDepthShaderMapHandle.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSCopyDepthList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSCopyDepthList.GetShader(shader_flag);
		IShaderProgram* pShader = this->CreateShaderProgram(pVS,pPS,copy_depth_rt_vertex_t_name,2);
		IShaderParamOp* pShaderOp = pShader->GetParamOp();
		Render::copy_depth_shaderhandle_t* pShaderHandle = (Render::copy_depth_shaderhandle_t*) CORE_ALLOC(sizeof(Render::copy_depth_shaderhandle_t));
		pShaderHandle->pShader = pShader;

		pShaderHandle->tex_DepthHandle = pShaderOp->FindParamIdByName("tex_depth");

		pShaderHandle->vec4DepthParamHandle =  pShaderOp->FindParamIdByName("c_DepthParam"); 
		m_CopyDepthShaderMapHandle.Add(shader_flag,pShaderHandle);
	}
	return m_CopyDepthShaderMapHandle.Find(shader_flag).GetData();
}


// ��ȡ��ǰshader����Ϣ����
size_t Render::GetShaderInfoNum()
{
	return m_ShaderList.size();
}

IResource* Render::GetShaderInfo(size_t index)
{
	if(index>= m_ShaderList.size())
		return NULL;
   
	return m_ShaderList[index].pShader;
}

// �Ƿ���Ҫץ֡��������
bool Render::IsCaptureFrame()
{
	return m_bIsCaptureFrame ;
}

void Render::IsCaptureFrame(bool bIsCaputure)
{
	m_bIsCaptureFrame = bIsCaputure ;
}

// ץȡһ֡����
void Render::CaptureFrame()
{
	IsCaptureFrame(true);
}

bool Render::ReleaseDeviceBuffer()
{
    CRenderStateRecordOp* pRenderStateRecordOp = (CRenderStateRecordOp*)m_pRenderStateRecordOp;
	pRenderStateRecordOp->Release();
	m_pRenderStateRecordOp = NULL;

	CRenderStateOp* pRenderStateOp = (CRenderStateOp*)m_pRenderStateOp;
	pRenderStateOp->Release();
	m_pRenderStateOp = NULL;

	CRenderDrawOp* pRenderDrawOp = (CRenderDrawOp*)m_pRenderDrawOp;
	pRenderDrawOp->Release();
	m_pRenderDrawOp = NULL;

	CTextureSamplerBindOp* pTextureSamplerBindOp = (CTextureSamplerBindOp*)m_pTextureSamplerBindOp;
	pTextureSamplerBindOp->Release();
	m_pTextureSamplerBindOp = NULL;

	GetCore()->DeleteEntity(m_DeviceCapsID);
	return true;
}

bool Render::CreateDevice(bool multithreading)
{
	// ���ڳߴ�
	m_nWinWidth = m_nDeviceWidth;
	m_nWinHeight = m_nDeviceHeight;

#ifdef GPU_MULTITHREADING
	//if(multithreading)
		g_ResourceContext->Init(m_nDeviceWidth, m_nDeviceHeight);
#endif
    
	if (!CreateDeviceBuffer())
	{
		return false;
	}
	InitRenderState();

	InitCopyRT();

	TexTools_Init(this);

	::TestErr("Render::CreateDevice..Err");
	return true;
}

bool Render::ResetDevice()
{
	ReleaseDeviceBuffer();
	CreateDeviceBuffer();
	
	// ���´������豸�ߴ���ص�����
	for (size_t i = 0; i < m_ResourceList.size(); ++i)
	{
		if (NULL == m_ResourceList[i])
		{
			continue;
		}

		switch (m_ResourceList[i]->GetResourceType())
		{
		case IResource::TYPE_COLOR_RT:
			{
				CColorRT* pColorRT = (CColorRT*)m_ResourceList[i];

				pColorRT->Create();
			}
			break;
		case IResource::TYPE_DEPTH_RT:
			{
				CDepthRT* pDepthRT = (CDepthRT*)m_ResourceList[i];

				pDepthRT->Create();
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool Render::UpdateWindowSize()
{
	return true;
}

const char* Render::GetPathPrefix(const char* name)
{
	Assert(name != NULL);

	// �ж���Դ�����Ƿ����������·��
	bool is_full_path = false;
#ifdef _WIN32    
	if (strstr(name, ":\\") != NULL || strstr(name, ":/") != NULL)
	{
		// ������������
		is_full_path = true;
	}
	else if (strstr(name, "\\\\") == name)
	{
		// ����·��
		is_full_path = true;
	}
#else
    if(name[0] == '/')
    {
        is_full_path = true;
    }
	else if(strncmp(name, "res/", 4) == 0 || strncmp(name, "res\\", 4) == 0)
	{
		is_full_path = true;
	}
#endif
	if (is_full_path)
	{
		// ������Ҫǰ׺
		return "";
	}
    
	else
	{
		// ���ص�ǰȱʡ����Դ·��
		return GetCore()->GetResourcePath();
	}
}

// ��������ƽ̨���ص�ǰ������Դ����չ��
const char* Render::GetTexExtfix()
{
	return m_strTexExtfix.c_str();
}

performance_info_t* Render::GetPerformance()
{
	return &m_Performance;
}

performance_mem_t* Render::GetMemPerformance()
{
	return &m_PerformanceMem;
}


void Render::SetResourceThreadNum(int value)
{
	if ((value < 1) || (value > MAX_RES_THREAD_NUM) 
		|| (value == m_nResThreadNum))
	{
		return;
	}

	if (value > m_nResThreadNum)
	{
		for (int i = m_nResThreadNum; i < value; ++i)
		{
			m_pResThreads[i] = CORE_NEW(CResThread);
			m_pResThreads[i]->Initialize();
		}
	}
	else
	{
		for (int i = value; i < m_nResThreadNum; ++i)
		{
			m_pResThreads[i]->Shutdown();
			m_pResThreads[i]->Clear();
			CORE_DELETE(m_pResThreads[i]);
			m_pResThreads[i] = NULL;
		}
	}

	m_nResThreadNum = value;
	m_nResThreadIndex = 0;
}

int Render::GetResourceThreadNum()
{
	return m_nResThreadNum;
}

void Render::SetAsyncLoadResource(bool value)
{
	m_bAsyncLoadResource = value;

	if (m_bAsyncLoadResource)
	{
		for (int k = 0; k < m_nResThreadNum; ++k)
		{
			m_pResThreads[k]->Initialize();
		}
	}
	else
	{
		for (int k = 0; k < m_nResThreadNum; ++k)
		{
			m_pResThreads[k]->Shutdown();
		}
	}
}

bool Render::GetAsyncLoadResource()
{
	return m_bAsyncLoadResource;
}

bool Render::RequestLoadResource(int category, IResLoader* loader)
{
	Assert(m_nResThreadNum > 0);

	int index = m_nResThreadIndex++;

	if (m_nResThreadIndex >= m_nResThreadNum)
	{
		m_nResThreadIndex = 0;
	}

	return m_pResThreads[index]->Request(category, loader);
}

int Render::GetResourceLoadTaskNum()
{
	int count = 0;

	for (int i = 0; i < m_nResThreadNum; ++i)
	{
		count += m_pResThreads[i]->GetTaskCount();
	}

	return count;
}

void Render::SetTextureLod(int value)
{
	m_nTextureLod = value;
}

int Render::GetTextureLod()
{
	return m_nTextureLod;
}

IImageAccess* Render::CreateImageAccess()
{
	return CImageAccess::NewInstance(this);
}

IRenderContext* Render::CreateContext()
{
	return CContext::NewInstance(this);
}

void Render::SetContext(IRenderContext* pContext)
{
	m_pCurrentContext = pContext;
}

IRenderContext* Render::GetContext()
{
	return m_pCurrentContext;
}

ISceneView* Render::CreateSceneView(ISceneView::SCENE_VIEW_TYPE scene_view_type,const char* pstrIdent)
{
	return CSceneView::NewInstance(scene_view_type,pstrIdent,this);
}

void Render::SetSceneView(ISceneView* pSceneView)
{
	m_pCurrentSceneView = pSceneView;
}

ISceneView* Render::GetSceneView()
{
	return m_pCurrentSceneView;
}

void Render::AddSceneView(ISceneView* pSceneView)
{
	if(DEVICE_STATE_READY != m_device_state)
		return;
	Assert(pSceneView != NULL);

	m_SceneViews.push_back(pSceneView);
}

void Render::SetRenderAlphaRef(int value)
{
	m_nRenderAlphaRef = value;
}

int Render::GetRenderAlphaRef()
{
	return m_nRenderAlphaRef;
}

void Render::SetTraceAlphaRef(int value) 
{ 
	m_nTraceAlphaRef = value; 
}

int Render::GetTraceAlphaRef() 
{ 
	return m_nTraceAlphaRef; 
}

void Render::SetTraceTextureName(bool value)
{
	m_bTraceTextureName = value;
}

bool Render::GetTraceTextureName()
{
	return m_bTraceTextureName;
}

// ��ȡ�豸��Ϣ
PERSISTID Render::GetDeviceCapsID() const
{
	return m_DeviceCapsID;
}

void Render::SetTraceOnlyFront(bool value)
{
	m_bTraceOnlyFront = value;
}

bool Render::GetTraceOnlyFront()
{
	return m_bTraceOnlyFront;
}

void Render::SetTraceAloneTexture(int value)
{
	m_nTraceAloneTexture = value;
}

int Render::GetTraceAloneTexture()
{
	return m_nTraceAloneTexture;
}

bool Render::ScreenShot(const char* name)
{
	SaveScreenToFile(name, 0, 0, GetDeviceWidth(), GetDeviceHeight());
	return true;
}

// ��Ļ��ͼ��ָ������
bool Render::ScreenShot(const char* name, int x, int y, int w, int h)
{
	//opengl������ϵ���ĳ�d3d��
	SaveScreenToFile(name, x, GetDeviceHeight() - h - y, w, h);
	return true;
}

void Render::BeginFrame(unsigned int back_color)
{
	if(m_device_state != DEVICE_STATE_READY)
	{
		m_vBackColor = VisUtil_ColorToVector4(back_color);

		//
		m_pRenderDrawOp->SetClearColor( m_vBackColor );
		m_pRenderDrawOp->SetClearDepth( 1.0F );
		m_pRenderDrawOp->SetClearStencil( 0 );
		m_pRenderDrawOp->SetClear( IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_STENCIL_BUFFER );
		return;
	}

	// ���￼����ץ�����ݴ���
	if(IsCaptureFrame())
	{
		//������е�����
		GetfmxStreamFileOp()->ClearData();

		size_t frame_id = GetFrameId();
		CRenderCmdBegin cmd_begin(frame_id);
        //fm_tracelog((const char*)&cmd_begin,LOG_FRAME);
		(*GetfmxStreamFileOp())<<(&cmd_begin);
	}

	// ÿһ֡�ʶ���Ҫ��ȡһ�µ�ǰ��FBO ����Ҫ��IOS���棬���ǻ���FBO�ǿ��ܱ��޸�IOSϵͳ�Լ��޸ĵ�������)
	// �ں���ĸ���SceneView�Լ�ȥ����ָ�����
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nDefautFBO);

#ifdef PERFORMANCE_DEBUG
	double dRenderTime = performance_time();
#endif
	m_vBackColor = VisUtil_ColorToVector4(back_color);

 
	m_pRenderDrawOp->SetClearColor( m_vBackColor );
	m_pRenderDrawOp->SetClearDepth( 1.0F );
	m_pRenderDrawOp->SetClearStencil( 0 );



	// �ж��Ƿ���Ҫ����������ɫ

//	m_pRenderDrawOp->SetClear( IRenderDrawOp::CLEAR_DEPTH_BUFFER );//ֻ��Ҫ������
//	m_pRenderDrawOp->SetClear( IRenderDrawOp::CLEAR_COLOR_BUFFER | IRenderDrawOp::CLEAR_DEPTH_BUFFER | IRenderDrawOp::CLEAR_STENCIL_BUFFER );
 

	size_t scene_num = m_SceneViews.size();
	
	for (size_t i = 0; i <scene_num; i++)
	{
		//���ﰴm_SceneViews�����˳��������
		size_t index = scene_num-1-i;
		ISceneView* pView = m_SceneViews[index];
		
		this->SetContext(pView->GetContext());
		this->SetSceneView(pView);

		pView->DrawScene();
		pView->ClearBatchs();
	}

	m_SceneViews.clear();

	// �ָ�ԭ�Ȼָ���FBO ������ν��UI��������ȷ������ȥ ����UIֱ�ӻ��Ƶ�backbuffer����
	glBindFramebuffer(GL_FRAMEBUFFER, m_nDefautFBO);

#ifdef PERFORMANCE_DEBUG
	double dGuiTime = performance_time();
#endif
	// ������
	m_pPaintBatchs->RenderFlat();
#ifdef PERFORMANCE_DEBUG
	m_Performance.dDrawGuiTime += performance_time() - dGuiTime;
#endif

#ifdef PERFORMANCE_DEBUG
	m_Performance.dRenderTime += performance_time() - dRenderTime;
#endif

	//shader const ����ͳ��
#ifdef PERFORMANCE_DEBUG
	m_Performance.dShaderConstData = m_mempool.GetPoolSize() / 1024.0f;
#endif

	this->SetContext(NULL);
	this->SetSceneView(NULL);
}



void Render::EndFrame()
{
//#ifdef PERFORMANCE_DEBUG
//	double dPresentTime = performance_time();
//#endif

//	eglSwapBuffers(0, 0);

	// Ϊ����֡����ԭ�����ݺ��ύ��������׼��
	//glBindFramebuffer(GL_FRAMEBUFFER, m_nFrameBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, m_nRenderBuffer);

	// ���￼����ץ�����ݴ���
	if(IsCaptureFrame())
	{
		size_t frame_id = GetFrameId();
		CRenderCmdEnd cmd_end(frame_id);
        //fm_tracelog((const char*)&cmd_end,LOG_FRAME);
		(*GetfmxStreamFileOp())<<(&cmd_end);
		IsCaptureFrame(false);

        //��ʼ����֡����
		GetfmxStreamFileOp()->SendData();
	}


	// ֡����
	++m_nFrameId;

//#ifdef PERFORMANCE_DEBUG
//	m_Performance.dPresentTime += performance_time() - dPresentTime;
//#endif


#ifdef GPU_MULTITHREADING
	texture_container_t::iterator it = m_Textures.Begin();
	for(; it != m_Textures.End(); it++)
	{
		CTexture* pTexture = it.GetData();
		if(pTexture)
			pTexture->WaitMainThread();
	}
#endif

}

struct DesignLineItem
{
	void* data;
	int data_size;
	int count;
	FmMat4 mtxWorld;
	Render * render;
};

void Render::DrawDesignLineBatch(void* pdata)
{
	DesignLineItem* line = (DesignLineItem*)pdata;
		
	struct vertex_line_t
	{
		FmVec3 pos;
		unsigned int diffuse;
	};
	
	vertex_line_t* vertex = (vertex_line_t*)line->data;
	IRenderContext* pContext = line->render->GetContext();

	// ����WVP
	const FmVec3& refer_pos = pContext->GetReferPosition();
	FmMat4 mtxTemp = line->mtxWorld;
	FmMat4 mtxWVP;

	mtxTemp._41 -= refer_pos.x;
	mtxTemp._42 -= refer_pos.y;
	mtxTemp._43 -= refer_pos.z;
	FmMat4Multiply(&mtxWVP, &mtxTemp, &pContext->GetCamera().mtxViewProjRelative);
	FmMat4Transpose(&mtxWVP, &mtxWVP);

	if(line->data_size > line->render->m_DesignLineVBSize || line->render->m_DesignLineVB == NULL)
	{
		if(line->render->m_DesignLineVB)
			line->render->Release();
		line->render->m_DesignLineVB = line->render->CreateDynamicVB(line->data_size,"Render::DrawDesignLineBatch");
		line->render->m_DesignLineVBSize = line->data_size;
	}

	line->render->UpdateDynamicVB(line->render->m_DesignLineVB, 0, line->data, line->data_size);

	//glUseProgram(line->render->m_DesginLineShader->GetProgram());
	line->render->m_DesginLineShader->UsedShader();
	//glBindBuffer(GL_ARRAY_BUFFER, line->render->m_DesignLineVB->GetBuffer());
	IRenderDrawOp* pRenderDrawOp = line->render->GetRenderDrawOp();
	pRenderDrawOp->SetVB(line->render->m_DesignLineVB->GetBuffer());

	IShaderParamOp* pShaderParamOp = line->render->m_DesginLineShader->GetParamOp();
	
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_line_t), (void*) (0));

	pRenderDrawOp->EnableVertexAttribArray(0,true);
	pRenderDrawOp->SetVertexAttribPointer(0,3,IRenderDrawOp::VERTEX_DATA_FLOAT,sizeof(vertex_line_t), (void*) (0));
	
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(vertex_line_t), (void*) (3 * sizeof(float)));

	pRenderDrawOp->EnableVertexAttribArray(1,true);
	pRenderDrawOp->SetVertexAttribPointer(1,4,IRenderDrawOp::VERTEX_DATA_UNSIGNED_BYTE,sizeof(vertex_line_t), (void*) (3 * sizeof(float)));

	pShaderParamOp->SetParamValue( pShaderParamOp->FindParamIdByName("c_mtxWVP"),mtxWVP);

    //glDrawArrays(GL_LINES, 0, line->count*2);
	line->render->GetRenderDrawOp()->DrawArrays(IRenderDrawOp::DRAW_LINES, 0, line->count*2);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	pRenderDrawOp->SetVB( 0 );
	//glUseProgram(0);
	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	pRenderDrawOp->EnableVertexAttribArray(0,false);
	pRenderDrawOp->EnableVertexAttribArray(1,false);
	delete(line->data);
	delete(pdata);
}

bool Render::DrawDesignLine(const FmMat4& mtxWorld, int primitive_type,	
	unsigned int primitive_count, const void* vertex_data, 
	unsigned int vertex_stride)
{
	if(NULL == m_DesginLineShader)
	{
		IVertexShader* pVS = LoadVertexShader("design_line.vsh", "main", "");
		IPixelShader* pPS = LoadPixelShader("design_line.fsh", "main", "");
		const char* vb[] = {"iPos", "iDiffuse"};
		m_DesginLineShader = CreateShaderProgram(pVS, pPS, vb, 2);
	}

	
	DesignLineItem* line = NEW DesignLineItem;
	line->mtxWorld = mtxWorld;
	line->render = this;
	line->data_size = vertex_stride*primitive_count*2;
	line->data = NEW char[line->data_size];
	memcpy(line->data, vertex_data, line->data_size);
	line->count = primitive_count;
	GetSceneView()->AddSolidBatch(DrawDesignLineBatch, line, "Render::DrawDesignLine" );

	//FmMatrixMultiplyTranspose(&mtxWVP, &mtxTemp, 
	//	&pContext->GetCamera().mtxViewProjRelative);

//	return m_pCurSceneBatchs->AddHelper(mtxWVP, primitive_type, 
//		primitive_count, vertex_data, vertex_stride);
	return false;
}

bool Render::DrawTexture(IShaderTex* pTex, int left, int top, int right, 
	int bottom)
{
	//return m_pCurSceneBatchs->AddPicture(pTex, left, top, right, bottom,
	//	DRAW_PICTURE_COLOR);
	return false;
}

bool Render::DrawBoundBox(const FmMat4& mat_global, const FmVec3& center, 
	const FmVec3& box_size, const FmVec3& vColor)
{
	FmVec3 c = center;
	FmVec3 b = box_size * 0.5F;
	FmVec3 v1(c.x - b.x, c.y + b.y, c.z - b.z);
	FmVec3 v2(c.x + b.x, c.y + b.y, c.z - b.z);
	FmVec3 v3(c.x + b.x, c.y + b.y, c.z + b.z);
	FmVec3 v4(c.x - b.x, c.y + b.y, c.z + b.z);
	FmVec3 v5(c.x - b.x, c.y - b.y, c.z - b.z);
	FmVec3 v6(c.x + b.x, c.y - b.y, c.z - b.z);
	FmVec3 v7(c.x + b.x, c.y - b.y, c.z + b.z);
	FmVec3 v8(c.x - b.x, c.y - b.y, c.z + b.z);

	struct vertex_line_t
	{
		FmVec3 pos;
		unsigned int diffuse;
	};

	vertex_line_t v[24];

	v[0].pos = v1;
	v[1].pos = v2;
	v[2].pos = v2;
	v[3].pos = v3;
	v[4].pos = v3;
	v[5].pos = v4;
	v[6].pos = v4;
	v[7].pos = v1;
	v[8].pos = v1;
	v[9].pos = v5;
	v[10].pos = v2;
	v[11].pos = v6;
	v[12].pos = v3;
	v[13].pos = v7;
	v[14].pos = v4;
	v[15].pos = v8;
	v[16].pos = v5;
	v[17].pos = v6;
	v[18].pos = v6;
	v[19].pos = v7;
	v[20].pos = v7;
	v[21].pos = v8;
	v[22].pos = v8;
	v[23].pos = v5;

	unsigned int color = VisUtil_VectorToColor(vColor);

	for (size_t i = 0; i < 24; ++i)
	{
		v[i].diffuse = color;
	}

	DrawDesignLine(mat_global, 0, 12, v, sizeof(vertex_line_t));

	return true;
}

bool Render::SetDeviceErrorCallback(device_error_callback_t func, 
	void* context)
{
	m_pDeviceErrorFunc = func;
	m_pDeviceErrorContext = context;

	return true;
}

bool Render::RaiseDeviceError(int error, const char* info)
{
	Assert(info != NULL);

	if (NULL == m_pDeviceErrorFunc)
	{
		return false;
	}

#ifdef FX_SYSTEM_WINDOWS
	m_pDeviceErrorFunc(m_pDeviceErrorContext, GetCurrentThreadId(), error, 
		info);
#else
	m_pDeviceErrorFunc(m_pDeviceErrorContext, 0 , error, 
		info);
#endif

 


	return true;
}

#ifdef PROCESS_LOST_DEVICE
// �豸��ʧʱ�Ĵ���
bool Render::Invalidate()
{
	TestErr("Render Invalidate Start");
	if(m_device_state == DEVICE_STATE_READY)
	{
		m_device_state = DEVICE_STATE_INVALIDE_START;
		m_pPaintBatchs->ClearRenderResource();
		for(size_t i = 0; i < m_ResourceList.size(); i++)
		{
			if(m_ResourceList[i])
			{
				m_ResourceList[i]->Invalidate();
				char buf[256];
				sprintf(buf, "RenderInvalidate %d", i);
				TestErr(buf);
			}
		}

		// �����豸��ʧ�ص�����
		for (size_t i = 0; i < m_DeviceLostCalls.size(); ++i)
		{
			device_lost_call_t* p = &m_DeviceLostCalls[i];

			if (p->pInvalidateFunc)
			{
				p->pInvalidateFunc(p->pContext);
			}
		}

		m_device_state = DEVICE_STATE_INVALIDE_END;
	}

	TestErr("Render Invalidate End");
	return true;
}

// �豸�ָ�ʱ�Ĵ���
bool Render::Restore()
{
	
	if(DEVICE_STATE_INVALIDE_END == m_device_state)
	{
		ResetDevice();
	//	CreateDeviceBuffer();
		InitRenderState();
		
		
	//	m_pRenderStateOp->SetViewPort(0, 0, m_nDeviceWidth, m_nDeviceHeight);

	/*
	m_pRenderStateOp = CRenderStateOp::NewInstance(this);
	m_pRenderDrawOp = CRenderDrawOp::NewInstance(this);
	m_pTextureSamplerBindOp = CTextureSamplerBindOp::NewInstance(this);
	m_pRenderStateOp->SetViewPort(0, 0, m_nDeviceWidth, m_nDeviceHeight);*/
	//	CreateDevice();
	//	((CRenderStateOp*)m_pRenderStateOp)->Restore();
	//	((CRenderDrawOp*)m_pRenderDrawOp)->Restore();
	//	m_pRenderStateOp->SetViewPort(0, 0, m_nDeviceWidth, m_nDeviceHeight);
	

		#ifdef FX_SYSTEM_WINDOWS
		//	InitCopyRT();
		#endif

	//	TexTools_Init(this);
	//	glViewport(0, 0, m_nDeviceWidth, m_nDeviceHeight);
	//	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nDefautFBO);
	//	glBindFramebuffer(GL_FRAMEBUFFER, m_nDefautFBO);

		TestErr("Render Restore Start");
		m_device_state = DEVICE_STATE_RESTORE;
		//�Ȼػָ������������������Դ
		for(size_t i = 0; i < m_ResourceList.size(); i++)
		{
			if(m_ResourceList[i] )
			{
				if(m_ResourceList[i]->GetResourceType() != IResource::TYPE_TEXTURE_SAMPLER)
				{
					m_ResourceList[i]->Restore();
					char buf[256];
					sprintf(buf, "RenderRestore %d", i);
					TestErr(buf);
				}
			}
		}

		//���ڻָ�������
		for(size_t i = 0; i < m_ResourceList.size(); i++)
		{
			if(m_ResourceList[i] )
			{
				if(m_ResourceList[i]->GetResourceType() == IResource::TYPE_TEXTURE_SAMPLER)
				{
					m_ResourceList[i]->Restore();
					char buf[256];
					sprintf(buf, "RenderRestore %d", i);
					TestErr(buf);
				}
			}
		}


		// �����豸�ָ�����
		for (size_t i = 0; i < m_DeviceLostCalls.size(); ++i)
		{
			device_lost_call_t* p = &m_DeviceLostCalls[i];

			if (p->pInvalidateFunc)
			{
				p->pRestoreFunc(p->pContext);
			}
		}
		m_device_state = DEVICE_STATE_READY;
		TestErr("Render Restore End");
	}
		
	return true;
}

bool Render::AddDeviceLostCallback(void* context, 
	DEVICE_LOST_CALLBACK invalidate_func, DEVICE_LOST_CALLBACK restore_func)
{
	device_lost_call_t data;

	data.pContext = context;
	data.pInvalidateFunc = invalidate_func;
	data.pRestoreFunc = restore_func;

	m_DeviceLostCalls.push_back(data);

	return true;
}

bool Render::RemoveDeviceLostCallback(void* context)
{
	size_t k = 0;
	
	while (k < m_DeviceLostCalls.size())
	{
		if (m_DeviceLostCalls[k].pContext == context)
		{
			m_DeviceLostCalls.remove(k);
		}
		else
		{
			++k;
		}
	}

	return true;
}

#endif

DeviceCaps* Render::GetDeviceCaps()
{
	return (DeviceCaps*)GetCore()->GetEntity(m_DeviceCapsID);
}


bool Render::FindCache(const char* file_name)
{
	Assert(file_name != NULL);

	return m_CacheFiles.Exists(file_name);
}

bool Render::LoadCache(const char* file_name, bool async_load)
{
	Assert(file_name != NULL);

	CCacheFile* pCache;

	if (m_CacheFiles.GetData(file_name, pCache))
	{
		return true;
	}

	pCache = CCacheFile::NewInstace();
	pCache->SetName(file_name);

	if (async_load)
	{
		// �����첽���ص��������
		if (!pCache->BeginCreate())
		{
			pCache->Release();
			return false;
		}

		// �����̼߳���
		if (!this->RequestLoadResource(0, pCache->GetLoader()))
		{
			pCache->Release();
			return false;
		}
	}
	else
	{
		// ͬ������
		if (!pCache->Create())
		{
			pCache->Release();
			return false;
		}
	}

	m_CacheFiles.Add(file_name, pCache);

	return true;
}

bool Render::UnloadCache(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheFile* pCache;

	if (!m_CacheFiles.GetData(file_name, pCache))
	{
		return false;
	}

	m_CacheFiles.RemoveData(file_name, pCache);

	pCache->Release();

	return true;
}

bool Render::UnloadAllCache()
{
	CACHE_CONTAINER::iterator it = m_CacheFiles.Begin();
	CACHE_CONTAINER::iterator end = m_CacheFiles.End();

	for (; it != end; ++it)
	{
		it.GetData()->Release();
	}

	m_CacheFiles.Clear();

	return true;
}

bool Render::GetCacheLoaded(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheFile* pCache;

	if (!m_CacheFiles.GetData(file_name, pCache))
	{
		return false;
	}

	return pCache->IsReady();
}

bool Render::GetCacheLoadFinish(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheFile* pCache;

	if (!m_CacheFiles.GetData(file_name, pCache))
	{
		return false;
	}

	return pCache->IsLoadComplete();
}

const void* Render::GetCacheData(const char* file_name, size_t& size)
{
	Assert(file_name != NULL);

	CCacheFile* pCache;

	if (!m_CacheFiles.GetData(file_name, pCache))
	{
		return NULL;
	}

	size = pCache->GetSize();

	return pCache->GetData();
}

size_t Render::GetCacheList(IVarList& result)
{
	result.Clear();

	CACHE_CONTAINER::iterator it = m_CacheFiles.Begin();
	CACHE_CONTAINER::iterator end = m_CacheFiles.End();

	for (; it != end; ++it)
	{
		result.AddString(it.GetKey());
	}

	return result.GetCount();
}

bool Render::FindCacheIni(const char* file_name)
{
	Assert(file_name != NULL);

	return m_CacheInis.Exists(file_name);
}

bool Render::LoadCacheIni(const char* file_name, bool create_hash, 
	bool async_load)
{
	Assert(file_name != NULL);

	CCacheIni* pCacheIni;

	if (m_CacheInis.GetData(file_name, pCacheIni))
	{
		return true;
	}

	pCacheIni = CCacheIni::NewInstace();
	pCacheIni->SetName(file_name);

	if (async_load)
	{
		// �����첽���ص��������
		if (!pCacheIni->BeginCreate(create_hash))
		{
			pCacheIni->Release();
			return false;
		}

		// �����̼߳���
		if (!this->RequestLoadResource(0, pCacheIni->GetLoader()))
		{
			pCacheIni->Release();
			return false;
		}
	}
	else
	{
		// ͬ������
		if (!pCacheIni->Create(create_hash))
		{
			pCacheIni->Release();
			return false;
		}
	}

	m_CacheInis.Add(file_name, pCacheIni);

	return true;
}

bool Render::UnloadCacheIni(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheIni* pCacheIni;

	if (!m_CacheInis.GetData(file_name, pCacheIni))
	{
		return false;
	}

	m_CacheInis.RemoveData(file_name, pCacheIni);
	pCacheIni->Release();

	return true;
}

bool Render::UnloadAllCacheIni()
{
	CACHE_INI_CONTAINER::iterator it = m_CacheInis.Begin();
	CACHE_INI_CONTAINER::iterator end = m_CacheInis.End();

	for (; it != end; ++it)
	{
		it.GetData()->Release();
	}

	m_CacheInis.Clear();

	return true;
}

bool Render::GetCacheIniLoaded(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheIni* pCacheIni;

	if (!m_CacheInis.GetData(file_name, pCacheIni))
	{
		return false;
	}

	return pCacheIni->IsReady();
}

bool Render::GetCacheIniLoadFinish(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheIni* pCacheIni;

	if (!m_CacheInis.GetData(file_name, pCacheIni))
	{
		return false;
	}

	return pCacheIni->IsLoadComplete();
}

IIniFile* Render::GetCacheIni(const char* file_name)
{
	Assert(file_name != NULL);

	CCacheIni* pCacheIni;

	if (!m_CacheInis.GetData(file_name, pCacheIni))
	{
		return NULL;
	}

	return pCacheIni->GetIni();
}

TextureCache * TextureCache::s_inst = 0;