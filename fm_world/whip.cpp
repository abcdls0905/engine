//--------------------------------------------------------------------
// 文件名:		whip.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "whip.h"
#include "model.h"
#include "actor.h"
#include "whip_manager.h"
#include "../visual/i_model_player.h"
#include "../visual/i_texture.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
//#include "../visual/dx_scene_batch.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../visual/i_scene_view.h"

/// \file whip.cpp
/// \brief 运动拖尾

/// entity: Whip
/// \brief 运动拖尾实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Whip, 0, IVisBase);

/// readonly: object Model
/// \brief 绑定的模型对象
DECLARE_PROPERTY_GET(PERSISTID, Whip, AttackModel, GetAttackModelID);
DECLARE_PROPERTY_GET(PERSISTID, Whip, SufferModel, GetSufferModelID);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, Whip, Visible, GetVisible, SetVisible);
/// property: string Name
/// \brief 拖尾名
DECLARE_PROPERTY_GET(const char*, Whip, Name, GetName);
/// property: string Texture
/// \brief 贴图文件名
DECLARE_PROPERTY(const char*, Whip, Texture, GetTextureName, 
	SetTextureName);
/// property: string ColorOne
/// \brief 顶点颜色1
DECLARE_PROPERTY(result_string, Whip, ColorOne, GetColorOneString, 
	SetColorOneString);
/// property: string ColorTwo
/// \brief 顶点颜色2
DECLARE_PROPERTY(result_string, Whip, ColorTwo, GetColorTwoString, 
	SetColorTwoString);

/// property: float InTime
/// \brief 进入时间
DECLARE_PROPERTY(float, Whip, InTime, GetInTime, SetInTime);
/// property: float PauseTime
/// \brief 暂停时间
DECLARE_PROPERTY(float, Whip, PauseTime, GetPauseTime, SetPauseTime);
/// property: float OutTime
/// \brief 退出时间
DECLARE_PROPERTY(float, Whip, OutTime, GetOutTime, SetOutTime);
/////property: FmVec3 LeftDir
///// \brief 刀光切面
//DECLARE_PROPERTY(FmVec3, Whip, LeftDir, GetLeftDir, SetLeftDir);
/////property: FmVec4 Params
///// \brief 参数属性
//DECLARE_PROPERTY(FmVec4, Whip, Params, GetParams, SetParams);
/// property: bool HighLight
/// \brief 是否高亮
DECLARE_PROPERTY(bool, Whip, HighLight, GetHighLight, SetHighLight);

// Whip

IRender* Whip::s_pRender = NULL;
IDynamicVB* Whip::s_pVBuffer = NULL;

void Whip::DrawWhip(void* data)
{
    if (!s_pVBuffer)
    {
        s_pVBuffer = s_pRender->CreateDynamicVB(
            WHIP_MAX_VERTICES_COUNT * sizeof(whip_vertex_t));
    }

    whip_render_batch* renderBatch = (whip_render_batch*)data;

    s_pRender->UpdateDynamicVB(s_pVBuffer, 0, 
        renderBatch->renderStates.vertices_buffer, 
        renderBatch->renderStates.vertices_count*sizeof(whip_vertex_t));

    renderBatch->pShaderHandle->pShader->UsedShader();

    IShaderParamOp* pShader = renderBatch->pShaderHandle->pShader->GetParamOp();
    IRenderDrawOp*  pDrawOp = renderBatch->pShaderHandle->pShader->GetRenderDrawOp();
    IRenderStateOp* pStateOp = renderBatch->pShaderHandle->pShader->GetRenderStateOp();

    pShader->SetParamValue(renderBatch->pShaderHandle->hMtxWVP,      renderBatch->renderStates.mtxWVP, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hMtxWorld,    renderBatch->renderStates.mtxWorld, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hEyePos,      *(FmVec4*)renderBatch->renderStates.cam_pos, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hColorStart,  *(FmVec4*)renderBatch->renderStates.color_start, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hColorEnd,    *(FmVec4*)renderBatch->renderStates.color_end, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hParams,      *(FmVec4*)renderBatch->renderStates.params, false);

    pShader->SetParamValue(renderBatch->pShaderHandle->hFogParam,    *(FmVec4*)renderBatch->renderStates.fog_param, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hFogColor,    *(FmVec4*)renderBatch->renderStates.fog_color, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hFogExpParam, *(FmVec4*)renderBatch->renderStates.fog_exp_param, false);

    if (renderBatch->pTexture)
    {
        pShader->SetTexture2D(renderBatch->pShaderHandle->hTex, renderBatch->pTexture->GetCanUseShaderTex()->GetTexture());
        ITextureSampler* pTexSamper = renderBatch->pTexture->GetCanUseShaderTex()->GetTextureSampler();
        pTexSamper->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);
    }

    pDrawOp->SetVB(s_pVBuffer->GetBuffer());

    pDrawOp->EnableVertexAttribArray(0, true);
    pDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(whip_vertex_t), 0);
    pDrawOp->EnableVertexAttribArray(1, true);
    pDrawOp->SetVertexAttribPointer(1, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(whip_vertex_t), (void*) (3 * sizeof(float)));

    fm_sizei count = renderBatch->renderStates.vertices_count;

    pStateOp->EnableCullFace(false);
    pStateOp->SetBlendEquation(IRenderStateOp::BLEND_FUNC_ADD);
    pStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_SRC_ALPHA,
        IRenderStateOp::BLEND_COLOR_FUNC(renderBatch->renderStates.destBlendFunc));

    pStateOp->SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_SRC_ALPHA, IRenderStateOp::BLEND_COLOR_SRC_ALPHA, 
        IRenderStateOp::BLEND_COLOR_ONE, IRenderStateOp::BLEND_COLOR_ONE);

    pDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0, count);

    pStateOp->EnableCullFace(true);

    pDrawOp->EnableVertexAttribArray(0, false);
    pDrawOp->EnableVertexAttribArray(1, false);
}

void Whip::ReleaseVB()
{
    SAFE_RELEASE(s_pVBuffer);
    SAFE_RELEASE(s_pRender);
}

Whip::Whip()
{
    m_pMgr = NULL;
    m_pData = NULL;
    m_type = WHIPTYPE_INVALID;

	m_bVisible = true;
    m_bReady = false;
	m_bAsyncLoad = false;
	m_bComplete = false;
	m_bStart = false;
	m_bHighLight = true;

	m_nColorOne = 0x80162EBE;
	m_nColorTwo = 0x00FF0000;

	m_pTex = NULL;
}

Whip::~Whip()
{
	SAFE_RELEASE(m_pTex);

    if (m_pData)
    {
        delete m_pData;
        m_pData = NULL;
    }
}

bool Whip::Init(const IVarList& args)
{
    if (!s_pRender)
    {
        s_pRender = (IRender*)GetCore()->GetInterface("Render");
    }

	return true;
}

bool Whip::Shut()
{
	return true;
}

void Whip::Update(float seconds)
{
    if (!m_bVisible)
    {
        return;
    }

    m_bReady = (CalculateAttackPosition() && CalculateSufferPosition());
}

void Whip::Realize()
{
    if (!m_bVisible || !m_bReady)
    {
        return;
    }

    float deltaTime = (float)GetCore()->GetFrameSeconds();

    IRenderContext* pContext = s_pRender->GetContext();

    // 当前摄像机的位置
    const FmVec3& cam_pos = pContext->GetCamera().vPosition;
    m_pData->Update(deltaTime, cam_pos);
    if (m_pData->m_fTime > m_pData->m_fInTime + m_pData->m_fPauseTime + m_pData->m_fOutTime)
    {
        m_bComplete = true;
    }

    FmVec3 vCenter = (m_pData->m_vAttackPosition + m_pData->m_vSufferPosition)*0.5f;
    // 视空间的距离，这是半透明物体进行排序的依据
    const FmMat4& mtxView = pContext->GetCamera().mtxView;
    float camera_z = mtxView._13 * vCenter.x + mtxView._23 * vCenter.y 
        + mtxView._33 * vCenter.z + mtxView._43;

    // 设置WVP
    FmMatrixTranspose(&m_shaderHandle.renderStates.mtxWVP, &pContext->GetCamera().mtxViewProj);
    FmMatrixIdentity(&m_shaderHandle.renderStates.mtxWorld);

    // 当前摄像机的位置
    m_shaderHandle.renderStates.cam_pos[0] = cam_pos.x;
    m_shaderHandle.renderStates.cam_pos[1] = cam_pos.y;
    m_shaderHandle.renderStates.cam_pos[2] = cam_pos.z;
    m_shaderHandle.renderStates.cam_pos[3] = 0.0F;

    // 刀光颜色
    FmVec4 color_start = VisUtil_ColorToVector4(m_nColorOne);
    memcpy(&m_shaderHandle.renderStates.color_start, &color_start, sizeof(float)*4);
    FmVec4 color_end = VisUtil_ColorToVector4(m_nColorTwo);
    memcpy(&m_shaderHandle.renderStates.color_end, &color_end, sizeof(float)*4);

    // 参数
    m_shaderHandle.renderStates.params[0] = m_pData->m_uScale;
    m_shaderHandle.renderStates.params[1] = m_pData->m_fBladeWidth;
    m_shaderHandle.renderStates.params[2] = 0.0f;
    m_shaderHandle.renderStates.params[3] = 0.0f;

    // 设置Fog
    float fog_start = pContext->GetFloat(IRenderContext::F_FOG_START);
    float fog_end = pContext->GetFloat(IRenderContext::F_FOG_END);
    m_shaderHandle.renderStates.fog_param[0] = fog_start;
    m_shaderHandle.renderStates.fog_param[1] = 1.0F / (fog_end - fog_start);
    m_shaderHandle.renderStates.fog_param[2] = 0.0F;
    m_shaderHandle.renderStates.fog_param[3] = 0.0F;

    // 颜色高亮
    m_shaderHandle.renderStates.bHighLight = m_bHighLight;
    if (m_bHighLight)
    {
        memset(m_shaderHandle.renderStates.fog_color, 0, sizeof(float)*4);
    }
    else
    {
        const FmVec4& v = pContext->GetVector4(IRenderContext::V_FOG_CURRENT);
        memcpy(m_shaderHandle.renderStates.fog_color, &v, sizeof(float)*4);
    }

    float fog_density = pContext->GetFloat(IRenderContext::F_FOG_DENSITY);
    if (m_bHighLight)
    {
        memset(m_shaderHandle.renderStates.fog_exp_param, 0, sizeof(float)*4);
    }
    else
    {
        const FmVec4& v = pContext->GetVector4(IRenderContext::V_FOG_EXP_CURRENT);
        memcpy(m_shaderHandle.renderStates.fog_exp_param, &v, sizeof(float)*4);
    }
    m_shaderHandle.renderStates.fog_exp_param[3] = fog_density;

    m_shaderHandle.renderStates.vertices_count = m_pData->m_verticesCount;
    m_shaderHandle.renderStates.vertices_buffer = m_pData->m_verticesBuffer;
    m_shaderHandle.renderStates.destBlendFunc = 
        m_bHighLight ? IRenderStateOp::BLEND_COLOR_ONE : IRenderStateOp::BLEND_COLOR_ONE_MINUS_DST_ALPHA;

    bool diffuse_map = (NULL != m_pTex);
    bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
    bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;
    size_t shader_flag = size_t(fog_linear) + (size_t(fog_exp) << 1)  + (size_t(diffuse_map) << 2);
    m_shaderHandle.pShaderHandle = m_pMgr->GetShaderHander((unsigned int)shader_flag);
    m_shaderHandle.pTexture = m_pTex;

    s_pRender->GetSceneView()->AddBlendBatch(DrawWhip, &m_shaderHandle, camera_z); 
}

bool Whip::Load()
{
	SAFE_RELEASE(m_pTex);

	if (!m_strTexture.empty())
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			s_pRender->GetPathPrefix(m_strTexture.c_str()), 
			m_strTexture.c_str());
		m_pTex = s_pRender->CreateTexture(fname, m_bAsyncLoad);
	}

	return true;
}

bool Whip::LoadIni(IIniFile* ini, const char* psname, 
	const char* tex_path)
{
	Assert(ini != NULL);
	Assert(psname != NULL);

    if (!ini->FindSection(psname))
    {
        char szText[128];
        sprintf(szText, "Whip Not find config: %s", psname);
        CORE_TRACE(szText);
        return false;
    }

    // 名
	m_strName = psname;

    // 类型
    int type = ini->ReadInteger(psname, "type", 1);

    // 鞭子段数
    int slices= ini->ReadInteger(psname, "slices", 32);

    BuildData((WhipType)type, (unsigned int)slices);

	// 贴图
	m_strTexture = ini->ReadString(psname, "texture", "");

	m_strTexture = tex_path + m_strTexture;

	// 进入时间
	m_pData->m_fInTime = ini->ReadFloat(psname, "intime", m_pData->m_fInTime);

    // 暂停时间
    m_pData->m_fPauseTime = ini->ReadFloat(psname, "pausetime", 0.0f);

    // 退出时间
    m_pData->m_fOutTime = ini->ReadFloat(psname, "outtime", m_pData->m_fOutTime);

	// 起始颜色
	const char* par_color = ini->ReadString(psname, "startcolor", 
		"255,255,255,255");

	if (strchr(par_color, ',') != NULL)
	{
		m_nColorOne = VisUtil_ColorToInt(par_color);
	}
	else
	{
		// 也可用整数形式的颜色数据
		m_nColorOne = atoi(par_color);
	}

	// 结束颜色
	par_color = ini->ReadString(psname, "endcolor", 
		"255,255,255,255");

	if (strchr(par_color, ',') != NULL)
	{
		m_nColorTwo = VisUtil_ColorToInt(par_color);
	}
	else
	{
		// 也可用整数形式的颜色数据
		m_nColorTwo = atoi(par_color);
	}

	// 是否高亮
	m_bHighLight = ini->FindItem(psname, "highlight");

    // 宽度
    m_pData->m_fBladeWidth = ini->ReadFloat(psname, "bladewidth", 0.1f);

    // 其他参数
	const char* par_params = ini->ReadString(psname, "params", 
		"0.0,0.0,0.0,1.0");
	m_pData->m_params = VisUtil_StringToVector4(par_params);

	return Load();
}

void Whip::BuildData(WhipType type, unsigned int slices)
{
    m_bComplete = false;

    m_type = type;
    if (m_pData)
    {
        delete m_pData;
    }

    switch (m_type)
    {
    case WHIPTYPE_ROPE:
        m_pData = new RopeData();
        break;
    case WHIPTYPE_SIN_WHIP:
        m_pData = new SinWhipData();
        break;
    case WHIPTYPE_CIRCLE_WHIP:
        m_pData = new CircleWhipData();
        break;
    case WHIPTYPE_SIN_WHIP_2:
        m_pData = new SinWhipData2();
        break;
    case WHIPTYPE_CIRCLE_WHIP_2:
        m_pData = new CircleWhipData2();
        break;
    case WHIPTYPE_CIRCLE_SABER:
        m_pData = new CircleSaberData();
        break;
    case WHIPTYPE_SIMI_CIRCLE_SABER:
        m_pData = new SimiCircleSaberData();
        break;
    }

    m_pData->Init(slices);
}

bool Whip::SetModelInfo(
    const PERSISTID& attack_id, const char* attack_label, 
    const PERSISTID& suffer_id, const char* suffer_label)
{
	m_attackID = attack_id;
    m_sufferID = suffer_id;
	m_strAttackLabel = attack_label;
	m_strSufferLabel = suffer_label;

	return true;
}

void Whip::GetModelInfo(IVarList& result)
{
    result.AddObject(m_attackID);
    result.AddString(m_strAttackLabel.c_str());

    result.AddObject(m_sufferID);
	result.AddString(m_strSufferLabel.c_str());
}

bool Whip::CalculateAttackPosition()
{
    FmMat4 tmp;

    IVisBase* pAttackVisBase = (IVisBase*)GetCore()->GetEntity(m_attackID);

    if (NULL == pAttackVisBase)
    {
        return false;
    }
    if (!pAttackVisBase->GetLoaded())
    {
        return false;
    }

    if (pAttackVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActor = (Actor*)pAttackVisBase;

        if ( !pActor->GetCurrentNodeMatrix(NULL, m_strAttackLabel.c_str(), &tmp) )
        {
            return false;
        }
    }
    else if (pAttackVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pAttackVisBase;
        IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

        if (NULL == pModelPlayer)
        {
            return false;
        }

        int start_label_index = pModel->GetNodeFromName(
            m_strAttackLabel.c_str());
        
        if ( start_label_index  == -1 || !pModelPlayer->GetLabelMatrix(start_label_index, tmp) )
        {
            m_pData->m_vAttackPosition = pAttackVisBase->GetPosition();
            return true;
            return false;
        }
    }
    else
    {
        m_pData->m_vAttackPosition = pAttackVisBase->GetPosition();
        return true;

        //Assert(0);
        //return false;
    }

    FmMat4 model_mat;

    pAttackVisBase->GetWorldMatrix(model_mat);

    FmVec3 v(tmp.m[3][0], tmp.m[3][1], tmp.m[3][2]);

    FmVec3TransformCoord(&m_pData->m_vAttackPosition, &v, &model_mat);

    return true;
}

bool Whip::CalculateSufferPosition()
{
    FmMat4 tmp;

    IVisBase* pSufferVisBase = (IVisBase*)GetCore()->GetEntity(m_sufferID);

    if (NULL == pSufferVisBase)
    {
        return false;
    }
    if (!pSufferVisBase->GetLoaded())
    {
        return false;
    }

    if (pSufferVisBase->GetEntInfo()->IsKindOf("Actor"))
    {
        Actor* pActor = (Actor*)pSufferVisBase;

        if ( !pActor->GetCurrentNodeMatrix(NULL, m_strSufferLabel.c_str(), &tmp) )
        {
            return false;
        }
    }
    else if (pSufferVisBase->GetEntInfo()->IsKindOf("Model"))
    {
        Model* pModel = (Model*)pSufferVisBase;
        IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

        if (NULL == pModelPlayer)
        {
            return false;
        }

        int start_label_index = pModel->GetNodeFromName(
            m_strSufferLabel.c_str());

        if ( start_label_index  == -1 || !pModelPlayer->GetLabelMatrix(start_label_index, tmp) )
        {
            m_pData->m_vSufferPosition = pSufferVisBase->GetPosition();
            return true;
            return false;
        }
    }
    else
    {
        m_pData->m_vSufferPosition = pSufferVisBase->GetPosition();
        return true;

        //Assert(0);
        //return false;
    }

    FmMat4 model_mat;

    pSufferVisBase->GetWorldMatrix(model_mat);

    FmVec3 v(tmp.m[3][0], tmp.m[3][1], tmp.m[3][2]);

    FmVec3TransformCoord(&m_pData->m_vSufferPosition, &v, &model_mat);

    return true;
}

bool Whip::GetLoadFinished()
{
	if (m_pTex != NULL)
	{
		if (!m_pTex->IsLoadComplete())
		{
			return false;
		}
	}

	return true;
}

void Whip::SetVisible(bool value)
{
	m_bVisible = value;
}

bool Whip::GetVisible() const
{
	return m_bVisible;
}

const char* Whip::GetName() const
{
	return m_strName.c_str();
}

void Whip::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool Whip::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void Whip::SetTextureName(const char* value)
{
	Assert(value != NULL);

	if (0 == m_strTexture.compare(value))
	{
		return;
	}

	m_strTexture = value;
	SAFE_RELEASE(m_pTex);

	if (!m_strTexture.empty())
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			s_pRender->GetPathPrefix(m_strTexture.c_str()), 
			m_strTexture.c_str());
		m_pTex = s_pRender->CreateTexture(fname, true);
	}
}

const char* Whip::GetTextureName() const
{
	return m_strTexture.c_str();
}

void Whip::SetColorOneString(const char* value)
{
	Assert(value != NULL);
	
	m_nColorOne = VisUtil_ColorToInt(value);
}

result_string Whip::GetColorOneString() const
{
	return VisUtil_IntToColor(m_nColorOne);
}

void Whip::SetColorTwoString(const char* value)
{
	Assert(value != NULL);

	m_nColorTwo = VisUtil_ColorToInt(value);
}

result_string Whip::GetColorTwoString() const
{
	return VisUtil_IntToColor(m_nColorTwo);
}

// 顶点颜色1
void Whip::SetColorOne(unsigned int value)
{
    m_nColorOne = value;
}
unsigned int Whip::GetColorOne() const
{
    return m_nColorOne;
}

// 顶点颜色2
void Whip::SetColorTwo(unsigned int value)
{
    m_nColorTwo = value;
}
unsigned int Whip::GetColorTwo() const
{
    return m_nColorTwo;
}

void Whip::SetInTime(float value)
{
	m_pData->m_fInTime = value;
}

float Whip::GetInTime() const
{
	return m_pData->m_fInTime;
}

// 暂停时间
void Whip::SetPauseTime(float value)
{
    m_pData->m_fPauseTime = value;
}

float Whip::GetPauseTime() const
{
    return m_pData->m_fPauseTime;
}

void Whip::SetOutTime(float value)
{
    m_pData->m_fOutTime = value;
}

float Whip::GetOutTime() const
{
    return m_pData->m_fOutTime;
}

void Whip::SetBladeWidth(float value)
{
    m_pData->m_fBladeWidth = value;
}

float Whip::GetBladeWidth() const
{
    return m_pData->m_fBladeWidth;
}

// 设置切面方向
void Whip::SetLeftDir(const FmVec3& vLeft)
{
    m_pData->m_vLeft = vLeft;
}

const FmVec3& Whip::GetLeftDir() const
{
    return m_pData->m_vLeft;
}

// 当前鞭子尾部位置
const FmVec3& Whip::GetTailPosition() const
{
    return m_pData->m_vTailPosition;
}

// 相关参数
void Whip::SetParams(const FmVec4& params)
{
    m_pData->m_params = params;
}

const FmVec4& Whip::GetParams() const
{
    return m_pData->m_params;
}

void Whip::SetStart(bool value)
{
    m_bStart = value;
}

bool Whip::GetStart() const
{
    return m_bStart;
}

void Whip::SetHighLight(bool value)
{
	m_bHighLight = value;
}

bool Whip::GetHighLight() const
{
	return m_bHighLight;
}
