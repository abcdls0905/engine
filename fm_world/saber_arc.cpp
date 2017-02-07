//--------------------------------------------------------------------
// 文件名:		saber_arc.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年5月10日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "saber_arc.h"
#include "model.h"
#include "actor.h"
#include "saber_arc_manager.h"
#include "../visual/i_model_player.h"
#include "../visual/i_texture.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"
//#include "../visual/dx_scene_batch.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../visual/i_scene_view.h"

/// \file saber_arc.cpp
/// \brief 运动拖尾

/// entity: SaberArc
/// \brief 运动拖尾实体
/// \see 继承自IVisBase
DECLARE_ENTITY(SaberArc, 0, IVisBase);

/// readonly: object Model
/// \brief 绑定的模型对象
DECLARE_PROPERTY_GET(PERSISTID, SaberArc, Model, GetModelID);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, SaberArc, Visible, GetVisible, SetVisible);
/// property: string Name
/// \brief 拖尾名
DECLARE_PROPERTY_GET(const char*, SaberArc, Name, GetName);
/// property: string Texture
/// \brief 贴图文件名
DECLARE_PROPERTY(const char*, SaberArc, Texture, GetTextureName, 
	SetTextureName);
/// property: string RefractionText
/// \brief 折射贴图文件名
DECLARE_PROPERTY(const char*, SaberArc, RefractionTex, GetRefractionTextureName, 
	SetRefractionTextureName);
/// property: string ColorOne
/// \brief 顶点颜色1
DECLARE_PROPERTY(result_string, SaberArc, ColorOne, GetColorOneString, 
	SetColorOneString);
/// property: string ColorTwo
/// \brief 顶点颜色2
DECLARE_PROPERTY(result_string, SaberArc, ColorTwo, GetColorTwoString, 
	SetColorTwoString);
/// property: float RefractionIntensity
/// \brief 折射强度
DECLARE_PROPERTY(float, SaberArc, RefractionIntensity, GetRefractionIntensity, 
	SetRefractionIntensity);
/// property: float LifeTime
/// \brief 拖尾生存时间
DECLARE_PROPERTY(float, SaberArc, LifeTime, GetLifeTime, SetLifeTime);
///property: float AcrSpeed
/// \brief 刀光速度
DECLARE_PROPERTY(float, SaberArc, AcrSpeed, GetSpeed, SetSpeed);
/// property: bool HighLight
/// \brief 是否高亮
DECLARE_PROPERTY(bool, SaberArc, HighLight, GetHighLight, SetHighLight);
/// property: bool StaticUV
/// \brief 是否保持UV静止
DECLARE_PROPERTY(bool, SaberArc, StaticUV, GetStaticUV, SetStaticUV);

/// method: [StartName, EndName] GetHelperPointName()
/// \brief 获得拖尾所绑定的两个辅助点
//DECLARE_METHOD_N(SaberArc, GetHelperPointName);

#define CB_WVP				0
#define CB_WORLD			4
#define CB_VIEW_POS			8
#define CB_BLADE_LENGTH		9
#define CB_COLOR_START		10
#define CB_COLOR_END		11
#define CB_REFRACTION_INTEN	12
#define CB_FOG_COLOR		13
#define CB_FOG_PARAM		14
#define CB_FOG_EXP_PARAM	15
#define CB_MAX				16

// SaberArc

IRender* SaberArc::s_pRender = NULL;
IDynamicVB* SaberArc::s_pVBuffer = NULL;

void SaberArc::DrawSaberArc(void* data)
{
    if (!s_pVBuffer)
    {
        s_pVBuffer = s_pRender->CreateDynamicVB(
            WAY_LENGTH * 2 * sizeof(arc_vertex_t));
    }

    saber_arc_render_batch* renderBatch = (saber_arc_render_batch*)data;

    if (!(*renderBatch->renderStates.vb_ready_ptr))
    {
        s_pRender->UpdateDynamicVB(s_pVBuffer, 0, 
            renderBatch->renderStates.vertices_buffer, 
            renderBatch->renderStates.vertices_count*sizeof(arc_vertex_t));
        *renderBatch->renderStates.vb_ready_ptr = true;
    }

    renderBatch->pShaderHandle->pShader->UsedShader();

    IShaderParamOp* pShader = renderBatch->pShaderHandle->pShader->GetParamOp();
    IRenderDrawOp*  pDrawOp = renderBatch->pShaderHandle->pShader->GetRenderDrawOp();
    IRenderStateOp* pStateOp = renderBatch->pShaderHandle->pShader->GetRenderStateOp();

    pShader->SetParamValue(renderBatch->pShaderHandle->hMtxWVP,      renderBatch->renderStates.mtxWVP, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hMtxWorld,    renderBatch->renderStates.mtxWorld, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hCamPos,      *(FmVec4*)renderBatch->renderStates.cam_pos, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hBladeLength, renderBatch->renderStates.blade_length[0], false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hColorStart,  *(FmVec4*)renderBatch->renderStates.color_start, false);
    pShader->SetParamValue(renderBatch->pShaderHandle->hColorEnd,    *(FmVec4*)renderBatch->renderStates.color_end, false);
    //pShader->SetParamValue(renderBatch->pShaderHandle->hRefIntensity,*(FmVec4*)renderBatch->renderStates.ref_intensity, false);
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
    pDrawOp->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(arc_vertex_t), 0);
    pDrawOp->EnableVertexAttribArray(1, true);
    pDrawOp->SetVertexAttribPointer(1, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(arc_vertex_t), (void*) (3 * sizeof(float)));
    pDrawOp->EnableVertexAttribArray(2, true);
    pDrawOp->SetVertexAttribPointer(2, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(arc_vertex_t), (void*) (6 * sizeof(float)));
    pDrawOp->EnableVertexAttribArray(3, true);
    pDrawOp->SetVertexAttribPointer(3, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(arc_vertex_t), (void*) (9 * sizeof(float)));

    fm_sizei count = renderBatch->renderStates.vertices_count;

    pStateOp->EnableCullFace(false);
    pStateOp->EnableDepthTest(false);
    pStateOp->EnableDepthWrite(false);
    pStateOp->SetBlendEquation(IRenderStateOp::BLEND_FUNC_ADD);
    pStateOp->SetBlendFunc(IRenderStateOp::BLEND_COLOR_SRC_ALPHA,IRenderStateOp::BLEND_COLOR_FUNC(renderBatch->renderStates.destBlendFunc));

    pStateOp->SetBlendFuncSeparate(IRenderStateOp::BLEND_COLOR_SRC_ALPHA, IRenderStateOp::BLEND_COLOR_SRC_ALPHA, 
        IRenderStateOp::BLEND_COLOR_ONE, IRenderStateOp::BLEND_COLOR_ONE);

    pDrawOp->DrawArrays(IRenderDrawOp::DRAW_TRIANGLE_STRIP, 0, count);

    pStateOp->EnableCullFace(true);
    pStateOp->EnableDepthTest(true);
    pStateOp->EnableDepthWrite(true);

    pDrawOp->EnableVertexAttribArray(0, false);
    pDrawOp->EnableVertexAttribArray(1, false);
    pDrawOp->EnableVertexAttribArray(2, false);
    pDrawOp->EnableVertexAttribArray(3, false);
}

void SaberArc::ReleaseVB()
{
    SAFE_RELEASE(s_pVBuffer);
    SAFE_RELEASE(s_pRender);
}

SaberArc::SaberArc()
{
    m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
    m_vTarget = FmVec3(0.0F, 0.0F, 0.0F);
	m_bVisible = true;
	m_bInitialized = false;
	m_bAsyncLoad = false;
	m_bComplete = false;
	m_bStart = false;
	m_bHighLight = true;
	m_bOnePoint = false;
	m_bStaticUV = false;
	m_nColorOne = 0x80162EBE;
	m_nColorTwo = 0x00FF0000;
	m_fBladeLength = 0.0F;
	m_fLifeTime = 1.0F;
    m_fOffsetParam = 0.0f;
    m_fInterpolateDis = 0.0f;
	m_fSpeed = 0.0F;
	m_fRefractionIntensity = 1.0F;

	m_pTex = NULL;
	m_pRefractionTex = NULL;
	m_nPointsNumber = 0;
	
    memset(m_fVertexTime, 0, sizeof(m_fVertexTime));
    memset(m_PointsOffset, 0, sizeof(m_PointsOffset));
    m_verticesCount = 0;
    m_bVBReady = false;
    m_bSleep = false;
}

SaberArc::~SaberArc()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pRefractionTex);
}

bool SaberArc::Init(const IVarList& args)
{
    if (!s_pRender)
    {
        s_pRender = (IRender*)GetCore()->GetInterface("Render");
    }

	return true;
}

bool SaberArc::Shut()
{
	return true;
}

bool SaberArc::Load()
{
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pRefractionTex);

	if (!m_strTexture.empty())
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			s_pRender->GetPathPrefix(m_strTexture.c_str()), 
			m_strTexture.c_str());
		m_pTex = s_pRender->CreateTexture(fname, m_bAsyncLoad);
	}

	if (!m_strRefractionTex.empty())
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			s_pRender->GetPathPrefix(m_strRefractionTex.c_str()), 
			m_strTexture.c_str());
		m_pRefractionTex = s_pRender->CreateTexture(fname, m_bAsyncLoad);
	}

	return true;
}

bool SaberArc::LoadIni(IIniFile* ini, const char* psname, 
	const char* tex_path)
{
	Assert(ini != NULL);
	Assert(psname != NULL);

    if (!ini->FindSection(psname))
    {
        char szText[128];
        sprintf(szText, "SaberArc Not find config: %s", psname);
        CORE_TRACE(szText);
        return false;
    }

	m_strName = psname;
	// 贴图
	m_strTexture = ini->ReadString(psname, "texture", "");

	m_strTexture = tex_path + m_strTexture;

	// 折射贴图
	m_strRefractionTex = ini->ReadString(psname, "refraction_tex", "");

	if (!m_strRefractionTex.empty())
	{
		m_strRefractionTex = tex_path + m_strRefractionTex;
	}

	// 折射强度
	m_fRefractionIntensity = ini->ReadFloat(psname, "refraction", 0.0F);

	// 刀光生存时间
	m_fLifeTime = ini->ReadFloat(psname, "lifetime", 1.0F);

	// 偏移参数
	m_fOffsetParam = ini->ReadFloat(psname, "offsetparam", 0.0F);

	// 插值距离
	m_fInterpolateDis = ini->ReadFloat(psname, "interpolate", 0.0F);

	// 刀光起始颜色
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

	// 刀光结束颜色
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
	// 抛出速度
	m_fSpeed = ini->ReadFloat(psname, "speed", 0.0F);
	// 是否保持位置
	m_bStaticUV = ini->FindItem(psname, "staticuv");

	return Load();
}

bool SaberArc::SetModelInfo(const PERSISTID& model_id, const char* start_name, 
	const char* end_name)
{
	m_ModelID = model_id;
	m_strBladeStart = start_name;
	m_strBladeEnd = end_name;

	return true;
}

void SaberArc::GetHelperPointName(const IVarList& args, IVarList& result)
{
	result.AddString(m_strBladeStart.c_str());
	result.AddString(m_strBladeEnd.c_str());
}

bool SaberArc::InitMatrix()
{
	if (!m_bInitialized)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_ModelID);

		if (NULL == pVisBase)
		{
			return false;
		}

		if (!pVisBase->GetLoaded())
		{
			return false;
		}

		FmMat4 start, end;

		if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			Actor* pActor = (Actor*)pVisBase;

			if ( !pActor->GetCurrentNodeMatrix(NULL, m_strBladeStart.c_str(), 
                &start) )
            {
                return false;
            }
			if ( !pActor->GetCurrentNodeMatrix(NULL, m_strBladeEnd.c_str(), 
                &end) )
            {
                return false;
            }
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			Model* pModel = (Model*)pVisBase;
			IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

			if (NULL == pModelPlayer)
			{
				return false;
			}

			int start_label_index = pModel->GetNodeFromName(
				m_strBladeStart.c_str());
			int end_label_index = pModel->GetNodeFromName(
				m_strBladeEnd.c_str());
			
			if ( !pModelPlayer->GetLabelMatrix(start_label_index, start) )
            {
                return false;
            }
            if ( !pModelPlayer->GetLabelMatrix(end_label_index, end) )
            {
                return false;
            }
		}
		else
		{
			Assert(0);
			return false;
		}

		FmVec3 v1(start.m[3][0], start.m[3][1], start.m[3][2]);
		FmVec3 v2(end.m[3][0], end.m[3][1], end.m[3][2]);
		FmMat4 mtxWorld;

		pVisBase->GetWorldMatrix(mtxWorld);

		FmVec3TransformCoord(&v1, &v1, &mtxWorld);
		FmVec3TransformCoord(&v2, &v2, &mtxWorld);

		m_bOnePoint = (v1 == v2) == true;

		if (m_bOnePoint)
		{
			FmVec3 vScale;
			FmQuat vRotation;
			FmVec3 vTranslation;

			FmMat4Decompose(&vScale, &vRotation, &vTranslation, &start);
			//VisUtil_MatrixDecompose(&vScale, &vRotation, &vTranslation, &start);

			m_fBladeLength = FM_MAX(vScale.x, FM_MAX(vScale.y, vScale.z));
		}
		else
		{
			FmVec3 len(v1-v2);
			m_fBladeLength = VisUtil_Vec3Length(&len);
		}

		m_bInitialized = true;
	}

	return true;
}

void SaberArc::Update(float seconds)
{
	if (!m_bVisible)
	{
		return;
	}

    if (!m_bInitialized && !InitMatrix())
    {
        return;
    }

    float delta_seconds = seconds < m_fLifeTime ? seconds : m_fLifeTime;

	IVisBase* pVisBase = NULL;
	FmMat4 start, end;
	int num_segments = 0;
	m_bComplete = false;

	if (!m_ModelID.IsNull())
	{
		pVisBase = (IVisBase*)GetCore()->GetEntity(m_ModelID);

		if (pVisBase)
		{
			// 同步位置
			m_vPosition = pVisBase->GetPosition();
		}
		else
		{
			m_ModelID = PERSISTID();
		}
	}

	if (m_ModelID.IsNull())
	{
		if (!m_bStart)
		{
			m_bComplete = true;
			return;
		}

		for (int v = WAY_LENGTH - 2; v > -1; v--)
		{
			m_fVertexTime[v] -= delta_seconds;

			if (m_fVertexTime[v] > FLT_EPSILON)
            {
                num_segments++;
			}
		}

		if (num_segments < 1)
		{
			m_bComplete = true;
			return;
		}
		else
		{
			m_fVertexTime[--num_segments] = 0.0F;
		}
	}
	else
    {
		if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
		{
			Actor* pActor = (Actor*)pVisBase;

			pActor->GetCurrentNodeMatrix(NULL, m_strBladeStart.c_str(), 
				&start);
			pActor->GetCurrentNodeMatrix(NULL, m_strBladeEnd.c_str(), 
                &end);
		}
		else if (pVisBase->GetEntInfo()->IsKindOf("Model"))
		{
			Model* pModel = (Model*)pVisBase;
			IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

			if (NULL == pModelPlayer)
			{
				return;
			}

			int start_label_index = pModel->GetNodeFromName(
				m_strBladeStart.c_str());
			int end_label_index = pModel->GetNodeFromName(
				m_strBladeEnd.c_str());

			pModelPlayer->GetLabelMatrix(start_label_index, start);
			pModelPlayer->GetLabelMatrix(end_label_index, end);
		}
		else
		{
			Assert(0);
		}

        FmMat4 model_mat;
        pVisBase->GetWorldMatrix(model_mat);

        //m_vOffsetDir.x = model_mat._31;
        //m_vOffsetDir.y = model_mat._32;
        //m_vOffsetDir.z = model_mat._33;
        m_vOffsetDir.x = m_vTarget.x - model_mat._41;
        m_vOffsetDir.y = m_vTarget.y - model_mat._42;
        m_vOffsetDir.z = m_vTarget.z - model_mat._43;
        m_vOffsetDir.normalize();

        // 计算刀光弧面上沿新的起始点
        FmVec3 v1(start.m[3][0], start.m[3][1], start.m[3][2]);
        // 计算刀光弧面下沿新的结束点
        FmVec3 v2(end.m[3][0], end.m[3][1], end.m[3][2]);

        // 刀光世界坐标系上的两个点
        FmVec3 vStart, vEnd;
        FmVec3TransformCoord(&vStart, &v1, &model_mat);
        FmVec3TransformCoord(&vEnd, &v2, &model_mat);

        //FmVec3 vDisTo0 = vStart - m_ControlPoints[0];
        FmVec3 vDisTo1 = vStart - m_ControlPoints[1];
        //float disTo0 = vDisTo0.length();
        float disTo1 = vDisTo1.length();
        FmVec3 vEndTo0 = vEnd - m_ControlPoints[0];
        float time1 = m_fVertexTime[1] - delta_seconds;

        if (disTo1 > 5.0f)
        {
            // 此时通常为发生瞬移,刀光将重新开始
            memset(m_fVertexTime, 0, sizeof(m_fVertexTime));
            m_ControlPoints[1] = vStart;
        }
        else
        {
            for (int v = 0; v < WAY_LENGTH; v++)
            {
                m_fVertexTime[v] -= delta_seconds;

                if (m_fVertexTime[v] <= FLT_EPSILON)
                {
                    memset(m_fVertexTime + v, 0, (WAY_LENGTH - v)*sizeof(float));
                    break;
                }

                num_segments++;
            }
        }

        int interpolateCount = 0;

		if (disTo1 > 0.02f)
		{
            if (m_fInterpolateDis > FLT_EPSILON)
            {
                interpolateCount = (int)(disTo1 / m_fInterpolateDis);
            }

            if (interpolateCount == 0)
            {
                if (num_segments == WAY_LENGTH)
                {
                    num_segments--;
                }

			    // 将整个队列往后退一格
			    for (int v = num_segments - 1; v > -1; v--)
			    {
				    m_ControlPoints[v + 1] = m_ControlPoints[v];
                    m_ControlPointsDir[v + 1] = m_ControlPointsDir[v];
				    m_fVertexTime[v + 1] = m_fVertexTime[v];
                }

                // 添加一个节点
                num_segments++;
            }
            else
            {
                if (num_segments + interpolateCount + 1 > WAY_LENGTH)
                {
                    num_segments = WAY_LENGTH - interpolateCount - 1;
                }

                // 将整个队列往后退interpolateCount + 1格
                for (int v = num_segments - 1; v > -1; v--)
                {
                    m_ControlPoints[v + interpolateCount + 1] = m_ControlPoints[v];
                    m_ControlPointsDir[v + interpolateCount + 1] = m_ControlPointsDir[v];
                    m_fVertexTime[v + interpolateCount + 1] = m_fVertexTime[v];
                }

                num_segments += interpolateCount + 1;
            }

		}

		if (!m_bStart)
		{
			m_fVertexTime[0] = 0;
		}
		else
        {
			m_fVertexTime[0] = m_fLifeTime + delta_seconds;
		}

        m_ControlPoints[0] = vStart;
        m_ControlPointsDir[0] = vEnd - vStart;
        float disStartToEnd = m_ControlPointsDir[0].length();
        m_ControlPointsDir[0] /= disStartToEnd;

        if (interpolateCount > 0)
        {
            float disEndTo0 = vEndTo0.length();
            for (int i = 0; i < interpolateCount; i++)
            {
                float r = (i + 1.0f) / (interpolateCount + 1.0f);
                float len = disStartToEnd + (disEndTo0 - disStartToEnd)*r;

                m_fVertexTime[i+1] = m_fVertexTime[0] + (m_fVertexTime[interpolateCount + 1] - m_fVertexTime[0])*r;
                m_ControlPoints[i+1] = m_ControlPoints[0] + (m_ControlPoints[interpolateCount + 1] - m_ControlPoints[0])*r;

                m_ControlPointsDir[i+1] = vEnd - m_ControlPoints[i+1];
                m_ControlPointsDir[i+1].normalize();
                m_ControlPoints[i+1] = vEnd - m_ControlPointsDir[i+1]*len;

                m_ControlPointsDir[i+1] = m_ControlPointsDir[0] + (m_ControlPointsDir[interpolateCount + 1] - m_ControlPointsDir[0])*r;
                m_ControlPointsDir[i+1].normalize();
            }
        }

        if (m_fOffsetParam > FLT_EPSILON)
        {
            for (int i = 0; i < num_segments; i++)
            {
                float dot = FmVec3Dot(&m_vOffsetDir, &m_ControlPointsDir[i]);

                /*
                if (dot > 0.0f)
                {
                    m_PointsOffset[i] = dot*m_fOffsetParam;
                }
                else
                {
                    m_PointsOffset[i] = 0.0f;
                }
                */
                if (dot > 1.0f)
                {
                    m_PointsOffset[i] = m_fOffsetParam;
                }
                else if (dot > -1.0f)
                {
                    m_PointsOffset[i] = (dot + 1.0f)*0.5f*m_fOffsetParam;
                }
                else
                {
                    m_PointsOffset[i] = 0.0f;
                }
            }
        }
	}

	m_nPointsNumber = num_segments;
}

void SaberArc::Realize()
{
	if (!m_bVisible || !m_bInitialized)
	{
		return;
	}

    IVisBase* pModel = (IVisBase*)s_pRender->GetCore()->GetEntity(m_ModelID);
    if (!pModel || !pModel->GetVisible())
    {
        return;
    }

    int sp_num = m_nPointsNumber;
    if (sp_num < 2)
    {
        return;
    }

	m_verticesCount = (unsigned int)(sp_num * 2);
	arc_vertex_t* vrt = m_verticesBuffer;

    float speed = abs(m_fSpeed);

    float length = 0.0f;

	for (int i = 0; i < sp_num; i++)
	{
		const FmVec3& dir = m_ControlPointsDir[i];

        if (m_fOffsetParam > FLT_EPSILON)
        {
            float r = i/(sp_num - 1.0f);
            r*=2.0f;
            vrt[i * 2 + 0].pos = m_ControlPoints[i] - dir * (m_PointsOffset[i] * r);
        }
        else
        {
		    vrt[i * 2 + 0].pos = m_ControlPoints[i];
        }
		vrt[i * 2 + 1].pos = vrt[i * 2 + 0].pos 
			+ dir * (m_fBladeLength + speed * float(i) / float(sp_num));
		vrt[i * 2 + 0].tangent = vrt[i * 2 + 1].tangent = dir;

		if (i == 0)
		{
			vrt[i * 2 + 0].binormal = vrt[i * 2 + 1].binormal = 
				m_ControlPoints[i] - m_ControlPoints[i + 1];
		}
		else if (i == sp_num - 1)
		{
			vrt[i * 2 + 0].binormal = vrt[i * 2 + 1].binormal = 
				m_ControlPoints[i - 1] - m_ControlPoints[i];
		}
		else
		{
			vrt[i * 2 + 0].binormal = vrt[i * 2 + 1].binormal = 
				m_ControlPoints[i - 1] - m_ControlPoints[i + 1];
		}

        if (i > 0)
        {
            FmVec3 v = m_ControlPoints[i] - m_ControlPoints[i - 1];
            length += v.length();
        }

        vrt[i * 2 + 0].tu = 1.0F;
        vrt[i * 2 + 1].tu = 0.0F;
		vrt[i * 2 + 0].tv = length;
	}

    if (length < 0.05f)
    {
        // 太短了,就别画它了
        return;
    }

    // 按顶点距离分布V坐标
    for (int i = 0; i < sp_num; i++)
    {
        vrt[i * 2 + 0].tv /= (length + 0.1f);
        //vrt[i * 2 + 0].tv = 0.0f;
        vrt[i * 2 + 1].tv = vrt[i * 2 + 0].tv;
    }

    m_bVBReady = false;

    IRenderContext* pContext = s_pRender->GetContext();

    // 视空间的距离，这是半透明物体进行排序的依据
    const FmMat4& mtxView = pContext->GetCamera().mtxView;
    float camera_z = mtxView._13 * m_vPosition.x + mtxView._23 * m_vPosition.y 
        + mtxView._33 * m_vPosition.z + mtxView._43;

	// 设置WVP
	FmMatrixTranspose(&m_shaderHandle.renderStates.mtxWVP, &pContext->GetCamera().mtxViewProj);
	FmMatrixIdentity(&m_shaderHandle.renderStates.mtxWorld);

	// 当前摄像机的位置
	const FmVec3& cam_pos = pContext->GetCamera().vPosition;
	m_shaderHandle.renderStates.cam_pos[0] = cam_pos.x;
    m_shaderHandle.renderStates.cam_pos[1] = cam_pos.y;
    m_shaderHandle.renderStates.cam_pos[2] = cam_pos.z;
    m_shaderHandle.renderStates.cam_pos[3] = 0.0F;

	// 刀光长度
    m_shaderHandle.renderStates.blade_length[0] = m_fBladeLength;
    m_shaderHandle.renderStates.blade_length[1] = 0.0F;
    m_shaderHandle.renderStates.blade_length[2] = 0.0F;
    m_shaderHandle.renderStates.blade_length[3] = 0.0F;

	// 刀光颜色
	FmVec4 color_start = VisUtil_ColorToVector4(m_nColorOne);
    memcpy(&m_shaderHandle.renderStates.color_start, &color_start, sizeof(float)*4);
    FmVec4 color_end = VisUtil_ColorToVector4(m_nColorTwo);
    memcpy(&m_shaderHandle.renderStates.color_end, &color_end, sizeof(float)*4);

    m_shaderHandle.renderStates.ref_intensity[0] = m_fRefractionIntensity;
    m_shaderHandle.renderStates.ref_intensity[1] = 0.0F;
    m_shaderHandle.renderStates.ref_intensity[2] = 0.0F;
    m_shaderHandle.renderStates.ref_intensity[3] = 0.0F;

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

	m_shaderHandle.renderStates.vertices_count = m_verticesCount;
    m_shaderHandle.renderStates.vertices_buffer = m_verticesBuffer;
    m_shaderHandle.renderStates.vb_ready_ptr = &m_bVBReady;
    m_shaderHandle.renderStates.destBlendFunc = 
        m_bHighLight ? IRenderStateOp::BLEND_COLOR_ONE : IRenderStateOp::BLEND_COLOR_ONE_MINUS_DST_ALPHA;

    bool diffuse_map = (NULL != m_pTex);
    bool single = m_bOnePoint;
    bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;
    bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;
    size_t shader_flag = size_t(diffuse_map) + (size_t(single) << 1)
        + (size_t(fog_linear) << 2) + (size_t(fog_exp) << 3);
    m_shaderHandle.pShaderHandle = m_pMgr->GetShaderHander((unsigned int)shader_flag);
    m_shaderHandle.pTexture = m_pTex;

    s_pRender->GetSceneView()->AddBlendBatch(DrawSaberArc, &m_shaderHandle, camera_z); 

    // 折射处理
    if (m_fRefractionIntensity > 0.0F && m_pRefractionTex) 
    {
        size_t shader_flag = (size_t(single) << 1) + (1 << 4);
        m_refShaderHandle.pShaderHandle = m_pMgr->GetShaderHander((unsigned int)shader_flag);
        m_refShaderHandle.pTexture = m_pRefractionTex;
        memcpy(&m_refShaderHandle.renderStates, &m_shaderHandle.renderStates, sizeof(render_states));
        s_pRender->GetSceneView()->AddBlendBatch(DrawSaberArc, &m_refShaderHandle, camera_z); 
    }
}

bool SaberArc::GetLoadFinished()
{
	if (m_pTex != NULL)
	{
		if (!m_pTex->IsLoadComplete())
		{
			return false;
		}
	}

	if (m_pRefractionTex != NULL)
	{
		if (!m_pRefractionTex->IsLoadComplete())
		{
			return false;
		}
	}

	return true;
}

void SaberArc::SetVisible(bool value)
{
	m_bVisible = value;
}

bool SaberArc::GetVisible() const
{
	return m_bVisible;
}

void SaberArc::SetSleep(bool value)
{
    m_bSleep = value;

    if (!m_bSleep)
    {
        memset(m_fVertexTime, 0, sizeof(m_fVertexTime));
    }
}

bool SaberArc::GetSleep() const
{
    return m_bSleep;
}

const char* SaberArc::GetName() const
{
	return m_strName.c_str();
}

void SaberArc::SetAsyncLoad(bool value)
{
	m_bAsyncLoad = value;
}

bool SaberArc::GetAsyncLoad() const
{
	return m_bAsyncLoad;
}

void SaberArc::SetTextureName(const char* value)
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

const char* SaberArc::GetRefractionTextureName() const
{
	return m_strRefractionTex.c_str();
}

void SaberArc::SetRefractionTextureName(const char* value)
{
	Assert(value != NULL);

	if (0 == m_strRefractionTex.compare(value))
	{
		return;
	}

	m_strRefractionTex = value;
	SAFE_RELEASE(m_pRefractionTex);

	if (!m_strRefractionTex.empty())
	{
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s", 
			s_pRender->GetPathPrefix(m_strRefractionTex.c_str()), 
			m_strRefractionTex.c_str());
		m_pRefractionTex = s_pRender->CreateTexture(fname, m_bAsyncLoad);
	}
}

const char* SaberArc::GetTextureName() const
{
	return m_strTexture.c_str();
}

void SaberArc::SetColorOneString(const char* value)
{
	Assert(value != NULL);
	
	m_nColorOne = VisUtil_ColorToInt(value);
}

result_string SaberArc::GetColorOneString() const
{
	return VisUtil_IntToColor(m_nColorOne);
}

void SaberArc::SetColorTwoString(const char* value)
{
	Assert(value != NULL);

	m_nColorTwo = VisUtil_ColorToInt(value);
}

result_string SaberArc::GetColorTwoString() const
{
	return VisUtil_IntToColor(m_nColorTwo);
}

void SaberArc::SetRefractionIntensity(float value)
{
	m_fRefractionIntensity = value;
}

float SaberArc::GetRefractionIntensity() const
{
	return m_fRefractionIntensity;
}

void SaberArc::SetLifeTime(float value)
{
	m_fLifeTime = value;
	
	if (m_fLifeTime < 0.0001F)
	{
		for (size_t v = 0; v < WAY_LENGTH; v++)
		{
			m_fVertexTime[v] = -1e20F;
		}
	}
}

float SaberArc::GetLifeTime() const
{
	return m_fLifeTime;
}

void SaberArc::SetSpeed(float value)
{
	m_fSpeed = value;
}

float SaberArc::GetSpeed() const
{
	return m_fSpeed;
}

void SaberArc::SetStart(bool value)
{
    m_bStart = value;
    if (m_bStart)
    {
        Update(0.0f);
    }
}

bool SaberArc::GetStart() const
{
    return m_bStart;
}

void SaberArc::SetHighLight(bool value)
{
	m_bHighLight = value;
}

bool SaberArc::GetHighLight() const
{
	return m_bHighLight;
}

void SaberArc::SetStaticUV(bool value)
{
	m_bStaticUV = value;
}

bool SaberArc::GetStaticUV() const
{
	return m_bStaticUV;
}
