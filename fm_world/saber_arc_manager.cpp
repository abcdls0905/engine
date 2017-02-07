//--------------------------------------------------------------------
// 文件名:		saber_arc_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "saber_arc_manager.h"
#include "saber_arc.h"
#include "model.h"
#include "actor.h"
//#include "../visual/i_model.h"
#include "../visual/i_model_player.h"
#include "../visual/i_context.h"
#include "../public/i_ini_file.h"
#include "../public/core_log.h"
#include "../public/var_list.h"
#include "../public/inlines.h"

/// \file saber_arc_manager.cpp
/// \brief 运动拖尾管理器

/// entity: SaberArcManager
/// \brief 运动拖尾管理器实体
/// \see 继承自IVisBase
DECLARE_ENTITY(SaberArcManager, 0, IVisBase);

/// property: string TexturePath
/// \brief 运动拖尾贴图文件路径
DECLARE_PROPERTY(const char*, SaberArcManager, TexturePath, GetTexturePath, 
	SetTexturePath);

/// method: object CreateFromModel(object model, string start_label, string end_label)
/// \brief 根据模型信息创建拖尾
/// \param model 模型对象
/// \param start_label 起始标记点
/// \param end_label 结束标记点
/*
DECLARE_METHOD_3(PERSISTID, SaberArcManager, CreateFromModel, 
	const PERSISTID&, const char*, const char*);

/// method: object CreateFromIni(object model, string start_label, string end_label)
/// \brief 根据配置文件创建拖尾
/// \param model 模型对象
/// \param ini_name 配置文件
/// \param arc_name 刀光名称
/// \param start_label 起始标记点
/// \param end_label 结束标记点
DECLARE_METHOD_5(PERSISTID, SaberArcManager, CreateFromIni, 
	const PERSISTID&, const char*, const char*, const char*, const char*);

/// method: bool Delete(object id)
/// \brief 删除拖尾
/// \param model 拖尾对象
DECLARE_METHOD_1(bool, SaberArcManager, Delete, const PERSISTID&);
*/
// SaberArcManager

SaberArcManager::SaberArcManager()
{
	m_bAsyncLoad = true;
	m_strTexturePath = "";
    m_deltaTime = 0;
}

SaberArcManager::~SaberArcManager()
{
}

bool SaberArcManager::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if(!m_pRender)
		return false;

    LoadShader();

	// 设置成单件实体
	//GetCore()->SetSingle(this);

	return true;
}

bool SaberArcManager::Shut()
{
	for (size_t i = 0; i < m_SaberArcs.size(); ++i)
	{
		SaberArc* pSaberArc = (SaberArc*)GetCore()->GetEntity(
			m_SaberArcs[i].SaberArcID);

		if (pSaberArc)
		{
			pSaberArc->Release();
		}
	}

	m_SaberArcs.clear();

    SaberArc::ReleaseVB();

#define shadermap 	TPodHashMap<unsigned int, saber_arc_filter_shaderhandle_t*, TPodTraits<unsigned int>,TSaberArcShaderAlloc>
    for(shadermap::iterator iter = m_ShaderMap.Begin(); iter != m_ShaderMap.End(); iter++ )
    {
        iter.GetData()->pShader->Release();
        CORE_FREE(iter.GetData(),sizeof(saber_arc_filter_shaderhandle_t));
    }   
#undef shadermap
    m_ShaderMap.Clear();

	return true;
}

void SaberArcManager::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// 设置自身为场景唯一对象
	m_pContext->SetUnique(this);
}

void SaberArcManager::Update(float seconds)
{
    m_deltaTime = seconds;
    if (m_deltaTime > 0.03f)
    {
        m_deltaTime = 0.03f;
    }
}

void SaberArcManager::Realize()
{
	if (m_SaberArcs.empty())
	{
		return;
	}

    size_t k = 0;
    while (k < m_SaberArcs.size())
    {
        SaberArc* pSaberArc = (SaberArc*)GetCore()->GetEntity(
            m_SaberArcs[k].SaberArcID);

        if (NULL == pSaberArc)
        {
            m_SaberArcs.remove(k);
            continue;
        }

        if (pSaberArc->GetSleep())
        {
            ++k;
            continue;
        }

        if (pSaberArc->GetComplete())
        {
            pSaberArc->Release();
            m_SaberArcs.remove(k);
            continue;
        }

        pSaberArc->Update(m_deltaTime);

        if (pSaberArc->GetVisible())
        {
            pSaberArc->Realize();
        }

        ++k;
    }
}

void SaberArcManager::SetTexturePath(const char* value)
{
	Assert(value != NULL);

	m_strTexturePath = value;
}

const char* SaberArcManager::GetTexturePath() const
{
	return m_strTexturePath.c_str();
}

PERSISTID SaberArcManager::CreateFromModel(const PERSISTID& id, 
	const char* start_label, const char* end_label)
{
	Assert(start_label != NULL);
	Assert(end_label != NULL);

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return PERSISTID();
	}

	if (pVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		Model* pModel = (Model*)pVisBase;

		if ((pModel->GetNodeFromName(start_label) < 0) 
			|| (pModel->GetNodeFromName(end_label) < 0))
		{
			return PERSISTID();
		}
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActor = (Actor*)pVisBase;

		if (!pActor->NodeIsExist(start_label) 
			|| !pActor->NodeIsExist(end_label))
		{
			return PERSISTID();
		}
	}
	else
	{
		return PERSISTID();
	}

	SaberArc* pSaberArc = (SaberArc*)GetCore()->CreateEntity("SaberArc");
    pSaberArc->SetMgr(this);
	pSaberArc->SetContext(m_pContext);
	pSaberArc->SetModelInfo(id, start_label, end_label);
	
	saber_arc_t info;

	info.SaberArcID = pSaberArc->GetID();

	m_SaberArcs.push_back(info);

	return pSaberArc->GetID();
}

bool SaberArcManager::Delete(const PERSISTID& id)
{
	size_t count = 0;
	size_t k = 0;

	while (k < m_SaberArcs.size())
	{
		if (m_SaberArcs[k].SaberArcID == id)
		{
			GetCore()->DeleteEntity(m_SaberArcs[k].SaberArcID);
			m_SaberArcs.remove(k);
			return true;
		}
		else
		{
			++k;
		}
	}

	return false;
}

PERSISTID SaberArcManager::CreateFromIni(const PERSISTID& id,
	const char* ini_name, const char* arc_name,
	const char* start_label, const char* end_label)
{
	Assert(start_label != NULL);
	Assert(end_label != NULL);

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return PERSISTID();
	}

	if (pVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		Model* pModel = (Model*)pVisBase;

		if ((pModel->GetNodeFromName(start_label) < 0) 
			|| (pModel->GetNodeFromName(end_label) < 0))
		{
			return PERSISTID();
		}
	}
	else if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActor = (Actor*)pVisBase;

		if (!pActor->NodeIsExist(start_label) 
			|| !pActor->NodeIsExist(end_label))
		{
			return PERSISTID();
		}
	}
	else
	{
		return PERSISTID();
	}

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(ini_name), ini_name);

	IIniFile* pIni = m_pRender->GetCacheIni(fname);
	if (NULL == pIni)
	{
        m_pRender->LoadCacheIni(fname, false, false);
		pIni = m_pRender->GetCacheIni(fname);

		if (!pIni)
		{
			CORE_TRACE("(SaberArcManager::CreateFromIni)load ini failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(arc_name);
			return PERSISTID();
		}
	}

//	CReadIni ini(fname);
//
//	// 尝试读取预加载的缓冲文件
//	size_t file_size;
//	const void* file_data = m_pRender->GetCacheData(fname, file_size);
//
//	if (file_data)
//	{
//		ini.LoadFromString((const char*)file_data, file_size);
//	}
//	else if (!ini.LoadFromFile())
//	{
//		return PERSISTID();
//	}

	SaberArc* pSaberArc = (SaberArc*)GetCore()->CreateEntity("SaberArc");
    pSaberArc->SetMgr(this);
	pSaberArc->SetContext(m_pContext);
	pSaberArc->SetModelInfo(id, start_label, end_label);
	pSaberArc->SetAsyncLoad(m_bAsyncLoad);
	pSaberArc->LoadIni(pIni, arc_name, GetTexturePath());

	saber_arc_t info;

	info.SaberArcID = pSaberArc->GetID();

	m_SaberArcs.push_back(info);

	return pSaberArc->GetID();
}

const char* saber_arc_vertex_t_name[] = 
{
    "iPos",
    "iTangent",
    "iBinormal",
    "iTexcoord",
};

saber_arc_filter_shaderhandle_t* SaberArcManager::GetShaderHander(unsigned int shader_flag)
{
    if(!m_ShaderMap.Exists(shader_flag))
    {
        IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
        IVertexShader* pVS = m_VSList.GetShader(shader_flag);

        IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS,pPS,saber_arc_vertex_t_name,4);

        saber_arc_filter_shaderhandle_t* pShaderHandle = (saber_arc_filter_shaderhandle_t*) CORE_ALLOC(sizeof(saber_arc_filter_shaderhandle_t));
        pShaderHandle->pShader       = pShader;
        pShaderHandle->hMtxWVP       = pShader->GetParamOp()->FindParamIdByName("c_mtxWVP");
        pShaderHandle->hMtxWorld     = pShader->GetParamOp()->FindParamIdByName("c_mtxWorld");
        pShaderHandle->hCamPos       = pShader->GetParamOp()->FindParamIdByName("c_vEyePos");     
        pShaderHandle->hBladeLength  = pShader->GetParamOp()->FindParamIdByName("c_vBladeLength"); 
        pShaderHandle->hColorStart   = pShader->GetParamOp()->FindParamIdByName("c_vColorStart");  
        pShaderHandle->hColorEnd     = pShader->GetParamOp()->FindParamIdByName("c_vColorEnd");    
        //pShaderHandle->hRefIntensity = pShader->GetParamOp()->FindParamIdByName("c_RefIntensity");
        pShaderHandle->hFogParam     = pShader->GetParamOp()->FindParamIdByName("c_FogParam");
        pShaderHandle->hFogColor     = pShader->GetParamOp()->FindParamIdByName("c_FogColor");
        pShaderHandle->hFogExpParam  = pShader->GetParamOp()->FindParamIdByName("c_FogExpParam");
        pShaderHandle->hTex          = pShader->GetParamOp()->FindParamIdByName("tex_Diffuse");

        m_ShaderMap.Add(shader_flag,pShaderHandle);
    } 

    return m_ShaderMap.Find(shader_flag).GetData();
}

bool SaberArcManager::LoadShader()
{
    m_VSList.SetRender(m_pRender);
    m_VSList.SetShaderName("saber_arc.vsh", "main");
    m_VSList.SetDefine(0, "#define DIFFUSEMAP=1\n");
    m_VSList.SetDefine(1, "#define SINGLE=1\n");
    m_VSList.SetDefine(2, "#define FOGLINEAR=1\n");
    m_VSList.SetDefine(3, "#define FOGEXP=1\n");
    m_VSList.SetDefine(4, "#define REFRACTION=1\n");
    m_VSList.LoadShader(0);

    m_PSList.SetRender(m_pRender);
    m_PSList.SetShaderName("saber_arc.fsh", "main");
    m_PSList.SetDefine(0, "#define DIFFUSEMAP=1\n");
    m_PSList.SetDefine(1, "#define SINGLE=1\n");
    m_PSList.SetDefine(2, "#define FOGLINEAR=1\n");
    m_PSList.SetDefine(3, "#define FOGEXP=1\n");
    m_PSList.SetDefine(4, "#define REFRACTION=1\n");
    m_PSList.LoadShader(0);

    return true;
}
