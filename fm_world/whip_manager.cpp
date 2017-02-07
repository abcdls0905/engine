//--------------------------------------------------------------------
// 文件名:		whip_manager.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "whip_manager.h"
#include "whip.h"
#include "model.h"
#include "actor.h"
#include "../visual/i_model_player.h"
#include "../visual/i_context.h"
#include "../public/i_ini_file.h"
#include "../public/core_log.h"
#include "../public/var_list.h"
#include "../public/inlines.h"

/// \file whip_manager.cpp
/// \brief 鞭子管理器

/// entity: WhipManager
/// \brief 鞭子管理器实体
/// \see 继承自IVisBase
DECLARE_ENTITY(WhipManager, 0, IVisBase);

/// property: string TexturePath
/// \brief 鞭子贴图文件路径
DECLARE_PROPERTY(const char*, WhipManager, TexturePath, GetTexturePath, 
	SetTexturePath);

/// method: object CreateFromModel(object model, string start_label, string end_label)
/// \brief 根据模型信息创建拖尾
/// \param model 模型对象
/// \param start_label 起始标记点
/// \param end_label 结束标记点
/*
DECLARE_METHOD_3(PERSISTID, WhipManager, CreateFromModel, 
	const PERSISTID&, const char*, const char*);

/// method: object CreateFromIni(object model, string start_label, string end_label)
/// \brief 根据配置文件创建拖尾
/// \param model 模型对象
/// \param ini_name 配置文件
/// \param arc_name 刀光名称
/// \param start_label 起始标记点
/// \param end_label 结束标记点
DECLARE_METHOD_5(PERSISTID, WhipManager, CreateFromIni, 
	const PERSISTID&, const char*, const char*, const char*, const char*);

/// method: bool Delete(object id)
/// \brief 删除拖尾
/// \param model 拖尾对象
DECLARE_METHOD_1(bool, WhipManager, Delete, const PERSISTID&);
*/
// WhipManager

WhipManager::WhipManager()
{
	m_bAsyncLoad = true;
	m_strTexturePath = "";
}

WhipManager::~WhipManager()
{
}

bool WhipManager::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if(!m_pRender)
		return false;

    LoadShader();

	return true;
}

bool WhipManager::Shut()
{
	for (int i = 0; i < MAX_WHIP_COUNT; ++i)
	{
        if (m_Whips[i].IsNull())
        {
            continue;
        }

		Whip* pWhip = (Whip*)GetCore()->GetEntity(m_Whips[i]);
		if (pWhip)
		{
			pWhip->Release();
		}

        m_Whips[i].nData64 = 0;
	}

    Whip::ReleaseVB();

#define shadermap 	TPodHashMap<unsigned int, whip_filter_shaderhandle_t*, TPodTraits<unsigned int>,TWhipShaderAlloc>
    for(shadermap::iterator iter = m_ShaderMap.Begin(); iter != m_ShaderMap.End(); iter++ )
    {
        iter.GetData()->pShader->Release();
        CORE_FREE(iter.GetData(),sizeof(whip_filter_shaderhandle_t));
    }   
#undef shadermap
    m_ShaderMap.Clear();

	return true;
}

void WhipManager::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
	// 设置自身为场景唯一对象
	m_pContext->SetUnique(this);
}

void WhipManager::Realize()
{
    for (int i = 0; i < MAX_WHIP_COUNT; ++i)
    {
        if (m_Whips[i].IsNull())
        {
            continue;
        }

        Whip* pWhip = (Whip*)GetCore()->GetEntity(m_Whips[i]);
        if (NULL == pWhip)
        {
            m_Whips[i].nData64 = 0;
            continue;
        }

        if (pWhip->GetComplete())
        {
            continue;
        }

        if (pWhip->GetVisible())
        {
            pWhip->Update((float)GetCore()->GetFrameSeconds());
            pWhip->Realize();
        }
    }
}

void WhipManager::SetTexturePath(const char* value)
{
	Assert(value != NULL);

	m_strTexturePath = value;
}

const char* WhipManager::GetTexturePath() const
{
	return m_strTexturePath.c_str();
}

int WhipManager::GetEmptyWhip()
{
    for (int i = 0; i < MAX_WHIP_COUNT; ++i)
    {
        if (m_Whips[i].IsNull())
        {
            return i;
        }

        Whip* pWhip = (Whip*)GetCore()->GetEntity(
            m_Whips[i]);
        if (NULL == pWhip)
        {
            m_Whips[i].nData64 = 0;
            return i;
        }

        if (pWhip->GetComplete())
        {
            return i;
        }
    }

    return -1;
}

PERSISTID WhipManager::CreateFromModel(
    WhipType type, 
    const FmVec3& vLeft,
    const char* szTexName,      // 贴图
    float inTime,               // 进入时间
    float pauseTime,            // 暂停时间
    float outTime,              // 退出时间
    unsigned int colorStart,    // 起始颜色
    unsigned int colorEnd,      // 结束颜色
    bool highlight,             // 是否高亮
    float bladeWidth,           // 宽度
    FmVec4 params,              // 其他参数
    const PERSISTID& attack_id, const char* attack_label, 
    const PERSISTID& suffer_id, const char* suffer_label)
{
    int index = GetEmptyWhip();
    if (index < 0)
    {
        return PERSISTID();
    }
/*
    Assert(attack_label != NULL);
    Assert(suffer_label != NULL);

    IVisBase* pAttackVisBase = (IVisBase*)GetCore()->GetEntity(attack_id);
	IVisBase* pSufferVisBase = (IVisBase*)GetCore()->GetEntity(suffer_id);

	if (NULL == pAttackVisBase || NULL == pSufferVisBase)
	{
		return PERSISTID();
	}

    // 小攻
	if (pAttackVisBase->GetEntInfo()->IsKindOf("Model"))
	{
		Model* pModel = (Model*)pAttackVisBase;

		if ((pModel->GetNodeFromName(attack_label) < 0))
		{
			return PERSISTID();
		}
	}
	else if (pAttackVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActor = (Actor*)pAttackVisBase;

		if (!pActor->NodeIsExist(attack_label))
		{
			return PERSISTID();
		}
	}
	else
	{
		return PERSISTID();
	}

    // 小受
    if (pSufferVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pSufferVisBase;

        if ((pModel->GetNodeFromName(suffer_label) < 0))
        {
            return PERSISTID();
        }
    }
    else if (pSufferVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActor = (Actor*)pSufferVisBase;

        if (!pActor->NodeIsExist(suffer_label))
        {
            return PERSISTID();
        }
    }
    else
    {
        return PERSISTID();
    }
*/
    Whip* pWhip;
    if (m_Whips[index].IsNull())
    {
        pWhip = (Whip*)GetCore()->CreateEntity("Whip");
        pWhip->SetMgr(this);
        m_Whips[index] = pWhip->GetID();
    }
    else
    {
        pWhip = (Whip*)GetCore()->GetEntity(m_Whips[index]);
    }
    pWhip->BuildData(type, 32);
	pWhip->SetContext(m_pContext);
    pWhip->SetModelInfo(attack_id, attack_label, 
                        suffer_id, suffer_label);
    pWhip->SetLeftDir(vLeft);
    pWhip->SetInTime(inTime);
    pWhip->SetPauseTime(pauseTime);
    pWhip->SetOutTime(outTime);
    pWhip->SetColorOne(colorStart);
    pWhip->SetColorTwo(colorEnd);
    pWhip->SetBladeWidth(bladeWidth);
    pWhip->SetHighLight(highlight);
    pWhip->SetTextureName(szTexName);
    pWhip->SetParams(params);
    pWhip->Load();

	return m_Whips[index];
}

bool WhipManager::Delete(const PERSISTID& id)
{
    for (int i = 0; i < MAX_WHIP_COUNT; ++i)
    {
        if (m_Whips[i] == id)
        {
            Whip* pWhip = (Whip*)GetCore()->GetEntity(id);
            pWhip->SetComplete();
            return true;
        }
    }

	return false;
}

PERSISTID WhipManager::CreateFromIni(const FmVec3& vLeft, 
    const PERSISTID& attack_id, const char* attack_label, 
    const PERSISTID& suffer_id, const char* suffer_label,
    const char* ini_name, const char* arc_name)
{
    int index = GetEmptyWhip();
    if (index < 0)
    {
        return PERSISTID();
    }
/*
    Assert(attack_label != NULL);
    Assert(suffer_label != NULL);

    IVisBase* pAttackVisBase = (IVisBase*)GetCore()->GetEntity(attack_id);
    IVisBase* pSufferVisBase = (IVisBase*)GetCore()->GetEntity(suffer_id);

    if (NULL == pAttackVisBase || NULL == pSufferVisBase)
    {
        return PERSISTID();
    }

    // 小攻
    if (pAttackVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pAttackVisBase;

        if ((pModel->GetNodeFromName(attack_label) < 0))
        {
            return PERSISTID();
        }
    }
    else if (pAttackVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActor = (Actor*)pAttackVisBase;

        if (!pActor->NodeIsExist(attack_label))
        {
            return PERSISTID();
        }
    }
    else
    {
        return PERSISTID();
    }

    // 小受
    if (pSufferVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pSufferVisBase;

        if ((pModel->GetNodeFromName(suffer_label) < 0))
        {
            return PERSISTID();
        }
    }
    else if (pSufferVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActor = (Actor*)pSufferVisBase;

        if (!pActor->NodeIsExist(suffer_label))
        {
            return PERSISTID();
        }
    }
    else
    {
        return PERSISTID();
    }
*/

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
			CORE_TRACE("(WhipManager::CreateFromIni)load ini failed");
			CORE_TRACE(ini_name);
			CORE_TRACE(arc_name);
			return PERSISTID();
		}
	}

    Whip* pWhip;
    if (m_Whips[index].IsNull())
    {
        pWhip = (Whip*)GetCore()->CreateEntity("Whip");
        pWhip->SetMgr(this);
        m_Whips[index] = pWhip->GetID();
    }
    else
    {
        pWhip = (Whip*)GetCore()->GetEntity(m_Whips[index]);
    }
	pWhip->SetContext(m_pContext);
	pWhip->SetModelInfo(attack_id, attack_label, 
        suffer_id, suffer_label);
	pWhip->SetAsyncLoad(m_bAsyncLoad);
    pWhip->LoadIni(pIni, arc_name, GetTexturePath());

    pWhip->SetLeftDir(vLeft);

	return m_Whips[index];
}

const char* whip_vertex_t_name[] = 
{
    "iPos",
    "iTexcoord",
};

whip_filter_shaderhandle_t* WhipManager::GetShaderHander(unsigned int shader_flag)
{
    if(!m_ShaderMap.Exists(shader_flag))
    {
        IPixelShader*  pPS = m_PSList.GetShader(shader_flag);
        IVertexShader* pVS = m_VSList.GetShader(shader_flag);

        IShaderProgram* pShader = m_pRender->CreateShaderProgram(pVS, pPS, whip_vertex_t_name, 2);

        whip_filter_shaderhandle_t* pShaderHandle = (whip_filter_shaderhandle_t*) CORE_ALLOC(sizeof(whip_filter_shaderhandle_t));
        pShaderHandle->pShader      = pShader;
        pShaderHandle->hMtxWVP      = pShader->GetParamOp()->FindParamIdByName("c_mtxWVP");
        pShaderHandle->hMtxWorld    = pShader->GetParamOp()->FindParamIdByName("c_mtxWorld");
        pShaderHandle->hEyePos      = pShader->GetParamOp()->FindParamIdByName("c_vEyePos");  
        pShaderHandle->hColorStart  = pShader->GetParamOp()->FindParamIdByName("c_vColorStart");  
        pShaderHandle->hColorEnd    = pShader->GetParamOp()->FindParamIdByName("c_vColorEnd");    
        pShaderHandle->hParams      = pShader->GetParamOp()->FindParamIdByName("c_vParams");  

        pShaderHandle->hFogParam    = pShader->GetParamOp()->FindParamIdByName("c_FogParam");
        pShaderHandle->hFogColor    = pShader->GetParamOp()->FindParamIdByName("c_FogColor");
        pShaderHandle->hFogExpParam = pShader->GetParamOp()->FindParamIdByName("c_FogExpParam");
        pShaderHandle->hTex         = pShader->GetParamOp()->FindParamIdByName("tex_Diffuse");

        m_ShaderMap.Add(shader_flag,pShaderHandle);
    } 

    return m_ShaderMap.Find(shader_flag).GetData();
}

bool WhipManager::LoadShader()
{
    m_VSList.SetRender(m_pRender);
    m_VSList.SetShaderName("whip.vsh", "main");
    m_VSList.SetDefine(0, "#define FOGLINEAR=1\n");
    m_VSList.SetDefine(1, "#define FOGEXP=1\n");
    m_VSList.SetDefine(2, "#define DIFFUSEMAP=1\n");
    m_VSList.LoadShader(0);

    m_PSList.SetRender(m_pRender);
    m_PSList.SetShaderName("whip.fsh", "main");
    m_PSList.SetDefine(0, "#define FOGLINEAR=1\n");
    m_PSList.SetDefine(1, "#define FOGEXP=1\n");
    m_PSList.SetDefine(2, "#define DIFFUSEMAP=1\n");
    m_PSList.LoadShader(0);

    return true;
}
