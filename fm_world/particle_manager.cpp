//--------------------------------------------------------------------
// 文件名:		particle_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "particle_manager.h"
#include "particle_node.h"
#include "particle_info.h"
#include "particle_data.h"
#include "particle.h"
#include "effect_data.h"
#include "../visual/vis_utils.h"
#include "../public/i_ini_file.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/core_log.h"

#include "../visual/i_context.h"


/// \file particle_manager.cpp
/// \brief 粒子管理器

/// entity: ParticleManager
/// \brief 粒子管理器实体
/// \see 继承自IVisBase
DECLARE_ENTITY(ParticleManager, 0, IVisBase);

/// readonly: ParticleNodeCount
/// \brief 粒子节点总数
DECLARE_PROPERTY_GET(int, ParticleManager, ParticleNodeCount, 
	GetParticleNodeCount);

/// readonly: ParticleDataCount
/// \brief 粒子数据总数
DECLARE_PROPERTY_GET(int, ParticleManager, ParticleDataCount, 
	GetParticleDataCount);

/// readonly: EffectDataCount
/// \brief 特效数据总数
DECLARE_PROPERTY_GET(int, ParticleManager, EffectDataCount, 
	GetEffectDataCount);

/// property: string TexturePath
/// \brief 粒子贴图文件路径
DECLARE_PROPERTY(const char*, ParticleManager, TexturePath, GetTexturePath, 
	SetTexturePath);
/// property: bool AsyncLoad
/// \brief 是否异步加载粒子贴图
DECLARE_PROPERTY(bool, ParticleManager, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: float LodBegin
/// \brief LOD起始距离
DECLARE_PROPERTY(float, ParticleManager, LodBegin, GetLodBegin, SetLodBegin);
/// property: float LodEnd
/// \brief LOD结束距离
DECLARE_PROPERTY(float, ParticleManager, LodEnd, GetLodEnd, SetLodEnd);

/*
/// method: object CreateInfo()
/// \brief 创建粒子参数对象
DECLARE_METHOD_0(PERSISTID, ParticleManager, CreateInfo);
/// method: object CreateInfoFromIni(string ini_name, string ps_name)
/// \brief 根据配置文件创建粒子参数对象
/// \param ini_name 配置文件名
/// \param ps_name 粒子名
DECLARE_METHOD_2(PERSISTID, ParticleManager, CreateInfoFromIni, const char*, 
	const char*);

/// method: object Create(object info_id)
/// \brief 根据粒子参数创建粒子对象
/// \param info_id 粒子参数对象
DECLARE_METHOD_1(PERSISTID, ParticleManager, Create, const PERSISTID&);
/// method: object CreateFromIni(string ini_name, string psname)
/// \brief 根据配置文件创建粒子对象
/// \param ini_name 配置文件名
/// \param psname 粒子名
DECLARE_METHOD_2(PERSISTID, ParticleManager, CreateFromIni, const char*, 
	const char*);
/// method: bool Delete(object obj)
/// \brief 删除粒子对象
/// \param obj 粒子对象
DECLARE_METHOD_1(bool, ParticleManager, Delete, const PERSISTID&);

/// method: bool ReloadIni()
/// \brief 重新加载所有配置文件信息
DECLARE_METHOD_0(bool, ParticleManager, ReloadIni);
*/

// ParticleManager

ParticleManager::ParticleManager()
{
	m_pFirst = NULL;
	m_pLast = NULL;
	m_strTexturePath = "tex\\particles\\";
	m_bAsyncLoad = true;
	m_fLodBegin = 20.0F;
	m_fLodEnd = 200.0F;
	m_fWindAngle = 0.0F;
	m_fWindSpeed = 0.0F;
	m_fWindPower = 0.0F;
	m_vWindDirection = FmVec3(0.0F, 0.0F, 0.0F);
	m_fCacheTime = 60.0F;

	m_pRender = NULL;
	m_bVisible = true;

	/*
	m_pParticleLayout = NULL;
	m_pGpuParticleLayout = NULL;
	m_pRasterState = NULL;
	m_pNormalBlendState = NULL;
	m_pDefaultBlendState = NULL;
	m_pAddBlendState = NULL;
	m_pMultiplyBlendState = NULL;
	*/
}

ParticleManager::~ParticleManager()
{
	CParticleNode* p = m_pFirst;

	while (p)
	{
		CParticleNode* t = p->GetNextNode();
		p->Release();
		p = t;
	}

	for (size_t i = 0; i < m_Datas.size(); ++i)
	{
		m_Datas[i]->Release();
	}

	for (size_t i = 0; i < m_EffectDatas.size(); ++i)
	{
		m_EffectDatas[i]->Release();
	}

#define shadermap 	TPodHashMap<unsigned int, ParticleManager::particle_shader_handle*, TPodTraits<unsigned int>,TParticleShaderAlloc>
	for(shadermap::iterator iter = m_ShaderMapHandle.Begin(); iter != m_ShaderMapHandle.End(); iter++ )
	{
		iter.GetData()->pShader->Release();
		CORE_FREE(iter.GetData(),sizeof(ParticleManager::particle_shader_handle));
	}   
#undef shadermap
 
}

const char* particle_vertex_cpu_t_name[] = 
{
	"iCpuPos",
	"iCpuColor",
	"iCpuUV",
};

const char* particle_vertex_gpu_t_name[]=
{
	"iGpuPos",
	"iGpuEmitterDir",
	"iGpuUV",
	"iGpuPosDelta",
	"iGpuRandom0",
	"iGpuRandom1",
	"iGpuRandom2",
};


bool ParticleManager::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");

	if (NULL == m_pRender)
	{
		return false;
	}

	// 设置着色器
	m_VSList.SetRender(m_pRender);
	m_VSList.SetShaderName("particle.vsh", "vs_main_particle");
	m_VSList.SetDefine(PAR_DEPTHMAP,       "#define PAR_DEPTHMAP  1\n"); // 深度图；
	m_VSList.SetDefine(PAR_BILLBOARD_Y,    "#define PAR_BILLBOARD_Y 1\n"); // 仅Y轴广告牌模式；
	m_VSList.SetDefine(PAR_BILLBOARD_NONE, "#define PAR_BILLBOARD_NONE 1\n"); // 广告牌模式；
	m_VSList.SetDefine(PAR_HIGHLIGHT,      "#define PAR_HIGHLIGHT 1\n"); // 高亮；
	m_VSList.SetDefine(PAR_FOGUSE,         "#define PAR_FOGUSE 1\n"); // 雾开关；
	m_VSList.SetDefine(PAR_FOGLINEAR,      "#define PAR_FOGLINEAR 1\n"); // 线性雾；
	m_VSList.SetDefine(PAR_FOGEXP,         "#define PAR_FOGEXP 1\n"); // 体积雾；
	m_VSList.SetDefine(PAR_GPU,            "#define PAR_GPU 1\n"); // 渲染模式；
	//m_VSList.SetDefine(7, "#define RANDCOLOR 1\n");
	//m_VSList.SetDefine(8, "#define GLOBAL 1\n");
	//m_VSList.SetDefine(9, "#define ZBIAS 1\n");
	//m_VSList.SetDefine(10,"#define CLIPPLANE 1\n");
	//m_VSList.SetDefine(11,"#define COLORANI 1\n");
	//m_VSList.LoadShader(0);

	m_PSList.SetRender(m_pRender);
	m_PSList.SetShaderName("particle.fsh", "ps_main_particle");
	m_PSList.SetDefine(PAR_DEPTHMAP,       "#define PAR_DEPTHMAP  1\n"); // 深度图；
	m_PSList.SetDefine(PAR_BILLBOARD_Y,    "#define PAR_BILLBOARD_Y 1\n"); // 仅Y轴广告牌模式；
	m_PSList.SetDefine(PAR_BILLBOARD_NONE, "#define PAR_BILLBOARD_NONE 1\n"); // 广告牌模式；
	m_PSList.SetDefine(PAR_HIGHLIGHT,      "#define PAR_HIGHLIGHT 1\n"); // 高亮；
	m_PSList.SetDefine(PAR_FOGUSE,         "#define PAR_FOGUSE 1\n"); // 雾开关；
	m_PSList.SetDefine(PAR_FOGLINEAR,      "#define PAR_FOGLINEAR 1\n"); // 线性雾；
	m_PSList.SetDefine(PAR_FOGEXP,         "#define PAR_FOGEXP 1\n"); // 体积雾；
	m_PSList.SetDefine(PAR_GPU,            "#define PAR_GPU 1\n"); // 渲染模式；
	//m_PSList.SetDefine(7, "#define RANDCOLOR 1\n");
	//m_PSList.SetDefine(8, "#define GLOBAL 1\n");
	//m_PSList.SetDefine(9, "#define ZBIAS 1\n");
	//m_PSList.SetDefine(10,"#define CLIPPLANE 1\n");
	//m_PSList.SetDefine(11,"#define COLORANI 1\n");
	//m_PSList.LoadShader(0);


	/*
	m_DistortionVSList.SetRender(m_pRender);
	m_DistortionVSList.SetShaderName("particle.vsh", "ps_main_particle");
	m_DistortionVSList.SetDefine(0, "#define FOGLINEAR 1\n");
	m_DistortionVSList.SetDefine(1, "#define DEPTHMAP 1\n");
	m_DistortionVSList.SetDefine(0, "BILLBOARD_Y=1;");
	m_DistortionVSList.SetDefine(1, "BILLBOARD_NONE=1;");
	m_DistortionVSList.SetDefine(2, "GPU=1;");
	m_DistortionVSList.SetDefine(3, "HIGHLIGHT=1;");
	m_DistortionVSList.SetDefine(4, "#define RANDCOLOR 1\n");
	m_DistortionVSList.SetDefine(5, "GLOBAL=1;");
	m_DistortionVSList.LoadShader(0);

	m_DistortionPSList.SetRender(m_pRender);
	m_DistortionPSList.SetShaderName("particle.hlsl", "ps_main_particle_ref");
	m_DistortionPSList.SetDefine(0, "BILLBOARD_Y=1;");
	m_DistortionPSList.SetDefine(1, "BILLBOARD_NONE=1;");
	m_DistortionPSList.SetDefine(2, "GPU=1;");
	m_DistortionPSList.SetDefine(3, "HIGHLIGHT=1;");
	m_DistortionPSList.SetDefine(4, "GLOBAL=1;");
	m_DistortionPSList.LoadShader(0);
	/**/

	return true;
}

bool ParticleManager::Shut()
{
	return true;
}

void ParticleManager::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// 设置自身为场景唯一对象
	m_pContext->SetUnique(this);
}

void ParticleManager::Update(float seconds)
{
	// 遍历所有粒子
	CParticleNode* p = m_pFirst;

	while (p)
	{
		CParticleNode* t = p->GetNextNode();

		if (p->GetComplete())
		{
			// 删除已结束的粒子
			DeleteParticle(p);
		}
		else
		{
			p->Update(seconds);

			if (!p->GetParticleID().IsNull())
			{
				// 关闭显示
				p->SetShowState(false);
			}
		}

		p = t;
	}

	//CheckUnload();
	m_offset_seconds = seconds;
}

void ParticleManager::Realize()
{
	if(!m_bVisible)
		return;

	float offset_seconds = m_offset_seconds;
	IRenderContext* pContext = m_pRender->GetContext();
	FmVec3 camera_pos = pContext->GetCamera().vPosition;
	float lod_begin = m_fLodBegin;
	float def_lod_end = m_fLodEnd;

	// 计算细节等级
	CParticleNode* p = m_pFirst;

	if (def_lod_end > lod_begin)
	{
		float lod_begin_sq = lod_begin * lod_begin;

		while (p)
		{
			if (p->GetShowState())
			{
				// 粒子的LOD结束距离
				float lod_end = p->GetLodEnd();

				if (lod_end <= 0.0F)
				{
					// 未设置则使用缺省值
					lod_end = def_lod_end + p->GetRadius();
				}

				const FmVec3& emitter_pos = p->GetEmitterPos();
				float sx = emitter_pos.x - camera_pos.x;
				float sy = emitter_pos.y - camera_pos.y;
				float sz = emitter_pos.z - camera_pos.z;
				float square = sx * sx + sy * sy + sz * sz;
				float lod_value;

				if (square < lod_begin_sq)
				{
					lod_value = 1.0F;
				}
				else if (square > (lod_end * lod_end))
				{
					lod_value = 0.0F;
				}
				else
				{
					float distance =  sqrt(square);

					// 根据到摄像机的距离设置LOD因子
					lod_value = (lod_end - distance) / (lod_end - lod_begin);
				}

				p->SetLodValue(lod_value);
			}

			p = p->GetNextNode();
		}
	}

	// 风向和风速
	float wind_angle = pContext->GetFloat(IRenderContext::F_WIND_ANGLE);
	float wind_speed = pContext->GetFloat(IRenderContext::F_WIND_SPEED);

	// 发生改变时
	if ((wind_angle != m_fWindAngle) || (wind_speed != m_fWindSpeed))
	{
		m_fWindAngle = wind_angle;
		m_fWindSpeed = wind_speed;
		m_fWindPower = wind_speed / 12.0F;
		m_vWindDirection.x = sinf(wind_angle);
		m_vWindDirection.y = 0.0F; 
		m_vWindDirection.z = cosf(wind_angle);

		p = m_pFirst;

		while (p)
		{
			p->SetWindPower(m_fWindPower);
			p->SetWindDirection(m_vWindDirection);
			p = p->GetNextNode();
		}
	}

	p = m_pFirst;

	while (p)
	{
		p->Realize(offset_seconds);
		p = p->GetNextNode();
	}
}

// 是否可见
void ParticleManager::SetVisible(bool value)
{
	m_bVisible = value;
}

bool ParticleManager::GetVisible() const
{
	return m_bVisible;
}


int ParticleManager::GetParticleNodeCount() const
{
	int count = 0;
	CParticleNode* p = m_pFirst;

	while (p)
	{
		++count;
		p = p->GetNextNode();
	}

	return count;
}

void ParticleManager::SetTexturePath(const char* value)
{
	Assert(value != NULL);

	m_strTexturePath = value;
}

const char* ParticleManager::GetTexturePath() const
{
	return m_strTexturePath.c_str();
}

 
IVertexShader* ParticleManager::GetParticleVS(size_t shader_flag)
{
	return m_VSList.GetShader(shader_flag);
}

IPixelShader* ParticleManager::GetParticlePS(size_t shader_flag)
{
	return m_PSList.GetShader(shader_flag);
}


ParticleManager::particle_shader_handle* ParticleManager::GetShaderHandle(size_t shader_flag, bool bnIsGpu)
{
	if(!m_ShaderMapHandle.Exists(shader_flag))
	{
		IVertexShader* pVS = m_VSList.GetShader(shader_flag);
		IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
		IShaderProgram* pShader = NULL;
		if(bnIsGpu)
		{
			pShader = m_pRender->CreateShaderProgram(pVS,pPS,particle_vertex_gpu_t_name,7);
		}
		else
		{
			pShader = m_pRender->CreateShaderProgram(pVS,pPS,particle_vertex_cpu_t_name,3);
		}
		IShaderParamOp* pShaderOp = pShader->GetParamOp();
		ParticleManager::particle_shader_handle* pShaderHandle
			= (ParticleManager::particle_shader_handle*)CORE_ALLOC(sizeof(ParticleManager::particle_shader_handle));
		pShaderHandle->pShader = pShader;
		pShaderHandle->tex_BaseMapHandle = pShaderOp->FindParamIdByName("tex_BaseMap");
		pShaderHandle->vec4ParticleColorHandle = pShaderOp->FindParamIdByName("c_ParticleColor");
		pShaderHandle->mat4ViewProj = pShaderOp->FindParamIdByName("c_mat4ViewProj");
		pShaderHandle->vec4FogParamHandle = pShaderOp->FindParamIdByName("c_FogParam");
		pShaderHandle->vec4FogColorHandle = pShaderOp->FindParamIdByName("c_FogColor");
		pShaderHandle->vec4FogExpParamHandle = pShaderOp->FindParamIdByName("c_FogExpParam");

		if(bnIsGpu)
		{
			pShaderHandle->vec3EmitterPos = pShaderOp->FindParamIdByName("c_vec3EmitterPos");
			pShaderHandle->vec3ViewPostion = pShaderOp->FindParamIdByName("c_vec3ViewPosition");
			pShaderHandle->vec4RotateAxis = pShaderOp->FindParamIdByName("c_vec4RotateAxis");
			pShaderHandle->vec4Time = pShaderOp->FindParamIdByName("c_vec4Time");
			pShaderHandle->vec4Speed = pShaderOp->FindParamIdByName("c_vec4Speed");
			pShaderHandle->vec4Force = pShaderOp->FindParamIdByName("c_vec4Force");
			pShaderHandle->vec4UV_Bias = pShaderOp->FindParamIdByName("c_vec4UV_Bias");
			pShaderHandle->vec4TrackAlpha = pShaderOp->FindParamIdByName("c_fTrack");
		}

		m_ShaderMapHandle.Add(shader_flag,pShaderHandle);
	}

	return m_ShaderMapHandle.Find(shader_flag).GetData();
}


IVertexShader* ParticleManager::GetDistortionVS(size_t shader_flag)
{
	return m_DistortionVSList.GetShader(0);
}

IPixelShader* ParticleManager::GetDistortionPS(size_t shader_flag)
{
	return m_DistortionPSList.GetShader(0);
}
 

bool ParticleManager::FindDataIndex(const char* ini_name, 
	const char* ps_name, size_t& index)
{
	Assert(ini_name != NULL);
	Assert(ps_name != NULL);

	size_t data_num = m_Datas.size(); 

	if (0 == data_num)
	{
		return false;
	}

	CParticleData** datas = &m_Datas[0];
	unsigned int hash = GetHashValue(ps_name);

	for (size_t i = 0; i < data_num; ++i)
	{
		CParticleData* p = datas[i];
		
		if ((p->GetHash() == hash)
			&& (stricmp(p->GetName(), ps_name) == 0)
			&& (stricmp(p->GetIniName(), ini_name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

CParticleData* ParticleManager::CreateData(const char* ini_name, 
	const char* ps_name)
{
	size_t index;

	if (FindDataIndex(ini_name, ps_name, index))
	{
		return m_Datas[index];
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(ini_name), ini_name);

	IIniFile* pIni = m_pRender->GetCacheIni(fname);
	bool cached = (pIni != NULL);

	if (NULL == pIni)
	{
		pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(ParticleManager::CreateData)load ini failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(ps_name);
			return NULL;
		}
	}

	CParticleData* pData = CParticleData::NewInstance();

	if (!pData->LoadIni(pIni, ps_name, GetTexturePath()))
	{
		if (!cached)
		{
			pIni->Release();
		}

		pData->Release();
		return NULL;
	}

	pData->SetIniName(ini_name);
	pData->SetName(ps_name);
	pData->SetHash(GetHashValue(ps_name));

	m_Datas.push_back(pData);

	if (!cached)
	{
		pIni->Release();
	}

	return pData;
}

bool ParticleManager::ReloadIni()
{
	IIniFile* pIni = NULL;
	const char* ini_file = "";

	for (size_t i = 0; i < m_Datas.size(); ++i)
	{
		CParticleData* pData = m_Datas[i];
		const char* ini_name = pData->GetIniName();

		if (StringEmpty(ini_name))
		{
			continue;
		}

		if (stricmp(ini_name, ini_file) != 0)
		{
			char fname[256];

			SafeSprintf(fname, sizeof(fname), "%s%s", 
				m_pRender->GetPathPrefix(ini_name), ini_name);

			if (NULL == pIni)
			{
				pIni = GetCore()->CreateIniFile(fname);
			}
			
			pIni->SetFileName(fname);

			if (!pIni->LoadFromFile())
			{
				CORE_TRACE("(ParticleManager::ReloadIni)load ini failed");
				CORE_TRACE(ini_name);
				continue;
			}

			ini_file = ini_name;
		}

		if (!pData->LoadIni(pIni, pData->GetName(), GetTexturePath())) 
		{
			CORE_TRACE("(ParticleManager::ReloadIni)load info failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(pData->GetName());
		}
	}

	if (pIni)
	{
		pIni->Release();
	}

	return true;
}

CParticleNode* ParticleManager::CreateParticle(CParticleData* pData)
{
	Assert(pData != NULL);

	CParticleNode* pNode = CParticleNode::NewInstance(this, m_pRender);

	pNode->SetWindPower(m_fWindPower);
	pNode->SetWindDirection(m_vWindDirection);

	if (!pNode->Create(pData, m_bAsyncLoad))
	{
		pNode->Release();
		return NULL;
	}

	// 添加到链表
	if (NULL == m_pFirst) 
	{
		m_pFirst = pNode;
		m_pLast = pNode;
	}
	else
	{
		m_pLast->SetNextNode(pNode);
		pNode->SetPrevNode(m_pLast);
		m_pLast = pNode;
	}
		
	return pNode;
}

void ParticleManager::DeleteParticle(CParticleNode* pNode)
{
	Assert(pNode != NULL);

	PERSISTID id = pNode->GetParticleID();

	if (!id.IsNull())
	{
		// 删除相关的粒子对象
		GetCore()->DeleteEntity(id);
	}

	// 从链表中删除
	CParticleNode* pPrev = pNode->GetPrevNode();
	CParticleNode* pNext = pNode->GetNextNode();
	
	if (pPrev)
	{
		pPrev->SetNextNode(pNext);
	}

	if (pNext)
	{
		pNext->SetPrevNode(pPrev);
	}

	if (m_pFirst == pNode)
	{
		m_pFirst = pNext;
	}

	if (m_pLast == pNode)
	{
		m_pLast = pPrev;
	}

	pNode->Release();
}

PERSISTID ParticleManager::CreateInfo()
{
	ParticleInfo* pInfo = (ParticleInfo*)GetCore()->CreateEntity("ParticleInfo");

	return pInfo->GetID();
}

PERSISTID ParticleManager::CreateInfoFromIni(const char* ini_name, 
	const char* ps_name)
{
	Assert(ini_name != NULL);
	Assert(ps_name != NULL);
	
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(ini_name), ini_name);

	IIniFile* pIni = m_pRender->GetCacheIni(fname);
	bool cached = (pIni != NULL);

	if (NULL == pIni)
	{
	    pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(ParticleManager::CreateInfoFromIni)load ini failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(ps_name);
			return PERSISTID();
		}
	}

	ParticleInfo* pInfo = (ParticleInfo*)GetCore()->CreateEntity("ParticleInfo");

	if (!pInfo->GetData()->LoadIni(pIni, ps_name, GetTexturePath()))
	{
		if (!cached)
		{
			pIni->Release();
		}
		
		pInfo->Release();
		return PERSISTID();
	}

	if (!cached)
	{
		pIni->Release();
	}

	return pInfo->GetID();
}

PERSISTID ParticleManager::Create(const PERSISTID& info_id)
{
	if (NULL == m_pRender)
	{
		return PERSISTID();
	}

	ParticleInfo* pInfo = (ParticleInfo*)GetCore()->GetEntity(info_id);

	if (NULL == pInfo)
	{
		return PERSISTID();
	}

	
	CParticleNode* pNode = CreateParticle(pInfo->GetData());

	if (NULL == pNode)
	{
		return PERSISTID();
	}

	Particle* p = (Particle*)GetCore()->CreateEntity("Particle");

	p->SetContext(m_pContext);
	p->SetParticleNode(pNode);
	p->SetInfoID(info_id);

	// 设置关联对象
	pNode->SetParticleID(p->GetID());
	
	return p->GetID();
}

PERSISTID ParticleManager::CreateFromIni(const char* ini_name, 
	const char* psname)
{
	Assert(ini_name != NULL);
	Assert(psname != NULL);

	if (NULL == m_pRender)
	{
		return PERSISTID();
	}

	CParticleData* pData = CreateData(ini_name, psname);

	if (NULL == pData)
	{
		return PERSISTID();
	}
	
	CParticleNode* pNode = CreateParticle(pData);

	if (NULL == pNode)
	{
		return PERSISTID();
	}

	Particle* p = (Particle*)GetCore()->CreateEntity("Particle" );
	
	p->SetContext(m_pContext);
	p->SetParticleNode(pNode);
	
	// 设置关联对象
	pNode->SetParticleID(p->GetID());
	
	return p->GetID();
}

bool ParticleManager::Delete(const PERSISTID& id)
{
	Particle* pParticle = (Particle*)GetCore()->GetEntity(id);

	if (NULL == pParticle)
	{
		return false;
	}

	pParticle->Release();
	
	return true;
}

bool ParticleManager::FindEffectDataIndex(const char* ini_name, 
	const char* em_name, size_t& index)
{
	Assert(ini_name != NULL);
	Assert(em_name != NULL);

	size_t data_num = m_EffectDatas.size(); 

	if (0 == data_num)
	{
		return false;
	}

	CEffectData** datas = &m_EffectDatas[0];
	unsigned int hash = GetHashValue(em_name);

	for (size_t i = 0; i < data_num; ++i)
	{
		CEffectData* p = datas[i];

		if ((p->GetHash() == hash)
			&& (stricmp(p->GetName(), em_name) == 0)
			&& (stricmp(p->GetIniName(), ini_name) == 0))
		{
			index = i;
			return true;
		}
	}

	return false;
}

CEffectData* ParticleManager::CreateEffectData(const char* ini_name, 
	const char* em_name)
{
	if (NULL == m_pRender)
	{
		return NULL;
	}

	size_t index;

	if (FindEffectDataIndex(ini_name, em_name, index))
	{
		m_EffectDatas[index]->SetUnuseTime(0.0F);
		return m_EffectDatas[index];
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(ini_name), ini_name);

	IIniFile* pIni = m_pRender->GetCacheIni(fname);
	bool cached = (pIni != NULL);

	if (NULL == pIni)
	{
		pIni = GetCore()->CreateIniFile(fname);

		if (!pIni->LoadFromFile())
		{
			pIni->Release();
			CORE_TRACE("(ParticleManager::CreateEffectData)load ini failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(em_name);
			return NULL;
		}
	}

	CEffectData* pData = CEffectData::NewInstance(m_pRender);

	if (!pData->LoadIni(pIni, em_name))
	{
		if (!cached)
		{
			pIni->Release();
		}

		pData->Release();
		return NULL;
	}

	pData->SetIniName(ini_name);
	pData->SetName(em_name);
	pData->SetHash(GetHashValue(em_name));

	m_EffectDatas.push_back(pData);

	if (!cached)
	{
		pIni->Release();
	}

	return pData;
}

bool ParticleManager::ReloadEffectIni()
{
	IIniFile* pIni = NULL;
	const char* ini_file = "";

	for (size_t i = 0; i < m_EffectDatas.size(); ++i)
	{
		CEffectData* pData = m_EffectDatas[i];
		const char* ini_name = pData->GetIniName();

		if (StringEmpty(ini_name))
		{
			continue;
		}

		if (stricmp(ini_name, ini_file) != 0)
		{
			char fname[256];

			SafeSprintf(fname, sizeof(fname), "%s%s", 
				m_pRender->GetPathPrefix(ini_name), ini_name);

			if (NULL == pIni)
			{
				pIni = GetCore()->CreateIniFile(fname);
			}
			
			pIni->SetFileName(fname);

			if (!pIni->LoadFromFile())
			{
				CORE_TRACE("(ParticleManager::ReloadEffectIni)load ini failed");
				CORE_TRACE(ini_name);
				continue;
			}

			ini_file = ini_name;
		}

		if (!pData->LoadIni(pIni, pData->GetName())) 
		{
			CORE_TRACE("(ParticleManager::ReloadEffectIni)load info failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(pData->GetName());
		}
	}

	if (pIni)
	{
		pIni->Release();
	}

	return true;
}

bool ParticleManager::CheckUnload()
{
	int ticks = m_UnloadTimer.GetElapseMillisec(100);

	if (ticks < 100)
	{
		return true;
	}

	size_t data_num = m_EffectDatas.size(); 

	if (0 == data_num)
	{
		return true;
	}

	float seconds = ticks * 0.001F;
	CEffectData** datas = &m_EffectDatas[0];

	for (size_t i = data_num - 1; i > 0; --i)
	{
		CEffectData* p = datas[i];

		if (NULL == p)
		{
			continue;
		}

		if (p->GetRefs() > 1)
		{
			continue;
		}
		else
		{
			float unuse_time = p->GetUnuseTime() + seconds;

			// 不需要短暂缓冲的数据在不被使用后即刻清除
			if (p->GetNoCache() || (unuse_time > m_fCacheTime))
			{
				m_EffectDatas.remove(i);
				p->Release();
			}
			else
			{
				p->SetUnuseTime(unuse_time);
			}
		}
	}

	return true;
}
