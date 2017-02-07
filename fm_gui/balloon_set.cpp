//--------------------------------------------------------------------
// 文件名:		balloon_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "balloon_set.h"
#include "balloon.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_context.h"
#include "../visual/i_scene.h"
#include "../visual/i_actor.h"

/// \file balloon_set.cpp
/// \brief 依附于场景内模型的气球控件集合

/// entity: BalloonSet
/// \brief 依附于场景内模型的气球控件集合实体
/// \see 继承自IControl
DECLARE_ENTITY(BalloonSet, 0, IControl);

/// property: object Scene
/// \brief 场景对象号
DECLARE_PROPERTY(PERSISTID, BalloonSet, Scene, GetSceneID, SetSceneID);
/// property: bool Sort
/// \brief 是否需要排序
DECLARE_PROPERTY(bool, BalloonSet, Sort, GetSort, SetSort);
/// property: bool UseDepth
/// \brief 是否使用深度
DECLARE_PROPERTY(bool, BalloonSet, UseDepth, GetUseDepth, SetUseDepth);

/// property: bool UseDepthScale
/// \brief 是否使用深度放缩
DECLARE_PROPERTY(bool, BalloonSet, UseDepthScale, GetUseDepthScale, SetUseDepthScale);

/// property: bool TempBalloons
/// \brief 是否缓存并复用汽球
DECLARE_PROPERTY(bool, BalloonSet, TempBalloons, GetTempBalloons, SetTempBalloons);

/// property: int MaxBalloonType
/// \brief 最大汽球类型数量
DECLARE_PROPERTY(int, BalloonSet, MaxBalloonType, GetMaxBalloonType, SetMaxBalloonType);

/// property: float MaxDepthScale
/// \brief 最大深度放缩
DECLARE_PROPERTY(float, BalloonSet, MaxDepthScale, GetMaxDepthScale, SetMaxDepthScale);

/// property: float MinDepthScale
/// \brief 最小深度放缩
DECLARE_PROPERTY(float, BalloonSet, MinDepthScale, GetMinDepthScale, SetMinDepthScale);

/// property: float FadeOutDepth
/// \brief 淡出距离，目标深度大于此值不显示头顶控件
DECLARE_PROPERTY(float, BalloonSet, FadeOutDepth, GetFadeOutDepth, SetFadeOutDepth);

// BalloonSet

BalloonSet::BalloonSet()
{
	m_pFirst = NULL;
	m_pLast = NULL;
	m_bSort = false;
	m_bUseDepth = true;
    m_bUseDepthScale = true;
    m_fMinDepthScale = 0.0001F;
    m_fMaxDepthScale = 1.0F;
    m_fFadeOutDepth = -1.0F;
    m_bTempBalloons = false;
    m_nMaxBalloonType = 0;
}

BalloonSet::~BalloonSet()
{
}

bool BalloonSet::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

    GetCore()->AddExecute(this);
	return true;
}

bool BalloonSet::Shut()
{
	ReleaseAll();
    ReleaseAllTemp();

    GetCore()->RemoveExecute(this);

	return IControl::Shut();
}

// 每帧执行逻辑（如果已添加到运行队列）
void BalloonSet::Execute(float seconds)
{
    CheckAll();
}

bool BalloonSet::GetLoadFinish()
{
	Balloon* p = m_pFirst;

	while (p)
	{
		if (!p->GetLoadFinish())
		{
			return false;
		}
		
		p = p->GetNextNode();
	}
	
	return IControl::GetLoadFinish();
}

void BalloonSet::ReleaseAll()
{
	Balloon* p = m_pFirst;
	
	while (p)
	{
		Balloon* t = p;
		
		p = p->GetNextNode();
		
        if (!TryAddTempBalloon(t))
        {
            t->Release();
        }
	}
	
	m_pFirst = NULL;
	m_pLast = NULL;
}

void BalloonSet::ReleaseAllTemp()
{
    for (size_t i = 0; i < m_vTempTypeArray.size(); ++i)
    {
	    Balloon* p = m_vTempTypeArray[i];
    	
	    while (p)
	    {
		    Balloon* t = p;
    		
		    p = p->GetNextNode();
    		
		    t->Release();
	    }
    }
}

void BalloonSet::CheckAll()
{
	Balloon* p = m_pFirst;
	
	while (p)
	{
		if (p->GetModelID().IsNull() || GetCore()->GetEntity(p->GetModelID()) == NULL)
		{
			Balloon* t = p;
			
			p = p->GetNextNode();
			
			// 随绑定对象一起删除
			t->Detach(&m_pFirst, &m_pLast);

            if (!TryAddTempBalloon(t))
            {
                t->Release();
            }
			continue;
		}

		p = p->GetNextNode();
	}
}

void BalloonSet::CheckOrder()
{
	// 排序
	Balloon* p = m_pFirst;
	
	while (true)
	{
		bool order = true;
		
		p = m_pFirst;
		
		while (p)
		{
			if (p->ProcessOrder(&m_pFirst, &m_pLast))
			{
				order = false;
			}
			
			p = p->GetNextNode();
		}
		
		if (order)
		{
			// 排序结束
			break;
		}
	}
}

bool BalloonSet::PaintBalloon(Balloon* pBalloon, float seconds, 
	float win_width, float win_height, const FmMat4& mtxView, 
	const FmMat4& mtxViewProj, const FmVec3& cam_pos)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
		pBalloon->GetModelID());
	
	if (NULL == pVisBase)
	{
		return false;
	}
	
	FmVec3 pos;
	if( !((IActor*)pVisBase)->GetNodePosition(pBalloon->GetNodeName(), pos) )
	{
		pos = pVisBase->GetCenter();
		pos.y = pVisBase->GetPosition().y + pVisBase->GetBoxSize().y;
	}
	pos.x += pBalloon->GetBindX();
	pos.y += pBalloon->GetBindY();
	pos.z += pBalloon->GetBindZ();
	
	// 是否非强制显示
	//if (!pBalloon->GetMustVisible())
	{
        FmVec3 len = cam_pos - pos;
		float dist_square = FmVec3LengthSq(&len);
		float ball_near = pBalloon->GetNearDistance();
		float ball_far = pBalloon->GetFarDistance();
		
		if ((dist_square < (ball_near * ball_near))
			|| (dist_square > (ball_far * ball_far)))
		{
			// 不在有效的距离范围内
			return false;
		}
	}
	
	FmVec4 proj_pos;

	FmVec3Transform(&proj_pos, &pos, &mtxViewProj);

	float depth_z = proj_pos.z / proj_pos.w;
	float depth_w = proj_pos.w;
	
	if ((depth_z > 1.0F) || (depth_z < 0.0F))
	{
		// 不在显示深度内
		return false;
	}

	pBalloon->SetDepth(depth_z);

	if ((m_fFadeOutDepth > 0.0F) && (depth_z > m_fFadeOutDepth))
    {
        // 大于显示深度则不显示
        return false;
    }

    bool old_scale_enable = GetPainter()->GetScaleEnable();
    float old_scale_value = GetPainter()->GetScaleRatio();

    if (m_bUseDepthScale)
    {
        GetPainter()->SetScaleEnable(true);

		FmVec3 vZ;

		FmVec3TransformCoord(&vZ, &pos, &mtxView);

        float scale = (5.0F / vZ.z);

        if (scale < m_fMinDepthScale)
        {
            scale = m_fMinDepthScale;
        }
        else if (scale > m_fMaxDepthScale)
        {
            scale = m_fMaxDepthScale;
        }

        GetPainter()->SetScaleRatio(scale);

	    win_width = float(GetPainter()->GetRenderWidth());
	    win_height = float(GetPainter()->GetRenderHeight());
    }

	float w_2 = win_width * 0.5F;
	float h_2 = win_height * 0.5F;
	// 控件中心点的位置
	//float cx = w_2 + w_2 * v.x;
	//float cy = h_2 - h_2 * v.y;
	float cx = w_2 + w_2 * (proj_pos.x / proj_pos.w);
	float cy = h_2 - h_2 * (proj_pos.y / proj_pos.w);
	
	IControl* pControl = pBalloon->GetControl();
	
	cx += pBalloon->GetOffsetLeft();
	cy += pBalloon->GetOffsetTop();
	
	float width = float(pControl->GetWidth());
	float height = float(pControl->GetHeight());
	float left = cx - width * 0.5F;
	float top = cy - height * 0.5F;
	
	if (((left + width) < 0) || (left > win_width)
		|| ((top + height) < 0) || (top > win_height))
	{
        if (m_bUseDepthScale)
        {
            GetPainter()->SetScaleEnable(old_scale_enable);
            GetPainter()->SetScaleRatio(old_scale_value);
        }

		// 超出屏幕
		return false;
	}
	
	pControl->SetAbsLeft(int(left + 0.5F));
	pControl->SetAbsTop(int(top + 0.5F));

	if (m_bUseDepth)
	{
		GetPainter()->SetDepthValue(depth_z, depth_w);
	}
	
	pControl->Paint(seconds);

	if (m_bUseDepth)
	{
		GetPainter()->SetDepthValue(0.0F, 1.0F);
	}

    if (m_bUseDepthScale)
    {
        GetPainter()->SetScaleEnable(old_scale_enable);
        GetPainter()->SetScaleRatio(old_scale_value);
    }

	return true;
}

void BalloonSet::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
	RefreshInnerAbsPos();

	if (m_bSort)
	{
		CheckOrder();
	}
	
	IScene* pScene = (IScene*)GetCore()->GetEntity(m_SceneID);

	if (NULL == pScene)
	{
		return;
	}

	// 当前场景的渲染器参数
	IRenderContext* pContext = pScene->GetContext();
	float width = (float)GetPainter()->GetRenderWidth();
	float height = (float)GetPainter()->GetRenderHeight();
    const FmMat4& mtxView =  pContext->GetCamera().mtxView;
	const FmMat4& mtxViewProj = pContext->GetCamera().mtxViewProj;
	const FmVec3& cam_pos = pContext->GetCamera().vPosition;
    TArrayPod<PERSISTID, 1, TCoreAlloc> vNeedDelete;
	Balloon* p = m_pFirst;
	
	while (p)
	{
	    IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
		    p->GetModelID());
    	
	    if (NULL == pVisBase)
	    {
            vNeedDelete.push_back(p->GetID());
	    }
        else
        {
		    PaintBalloon(p, seconds, width, height, mtxView, mtxViewProj, cam_pos); 
        }

		p = p->GetNextNode();
	}

    // 清理需要删除的头顶控件
    for (size_t i = 0; i < vNeedDelete.size(); ++i)
    {
        DeleteBalloon(vNeedDelete[i]);
    }

    vNeedDelete.clear();
}

void BalloonSet::SetSceneID(const PERSISTID& id)
{
	IEntity* pEntity = GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Scene"))
	{
		return;
	}

	m_SceneID = id;
}

PERSISTID BalloonSet::GetSceneID() const
{
	return m_SceneID;
}

void BalloonSet::SetSort(bool value)
{
	m_bSort = value;
}

bool BalloonSet::GetSort() const
{
	return m_bSort;
}

void BalloonSet::SetUseDepth(bool value)
{
	m_bUseDepth = value;
}

bool BalloonSet::GetUseDepth() const
{
	return m_bUseDepth;
}

Balloon* BalloonSet::NewBalloon(IControl* pControl, const PERSISTID& model, const int type)
{
    Balloon* pBalloon = (Balloon*)GetCore()->CreateEntity("Balloon");
	
	pBalloon->SetControl(pControl);
    pBalloon->SetType(type);
	pBalloon->SetModelID(model);
	pBalloon->Attach(&m_pFirst, &m_pLast);
	
	return pBalloon;
}

// 是否使用远近放缩
void BalloonSet::SetUseDepthScale(bool value)
{
    m_bUseDepthScale = value;
}

bool BalloonSet::GetUseDepthScale() const
{
    return m_bUseDepthScale;
}

// 是否缓存并复用汽球
void BalloonSet::SetTempBalloons(bool value)
{
    m_bTempBalloons = value;
}

bool BalloonSet::GetTempBalloons() const
{
    return m_bTempBalloons;
}

// 最大的汽球类型数量
void BalloonSet::SetMaxBalloonType(int value)
{
    if (value < 0)
    {
        value = 0;
    }

    if (m_nMaxBalloonType != value)
    {
        // 清理所有
        ReleaseAllTemp();

        // 设置当前
        m_nMaxBalloonType = value;

        if (value >= 0)
        {
            m_vTempTypeArray.resize(value);

            for (int i = 0; i < value; ++i)
            {
                m_vTempTypeArray[i] = NULL;
            }
        }
    }
}

int BalloonSet::GetMaxBalloonType() const
{
    return m_nMaxBalloonType;
}

void BalloonSet::SetMaxDepthScale(float value)
{
    if (value < 1.0F)
    {
        value = 1.0F;
    }

    if (value < m_fMinDepthScale)
    {
        m_fMinDepthScale = value;
    }

    m_fMaxDepthScale = value;
}

float BalloonSet::GetMaxDepthScale() const
{
    return m_fMaxDepthScale;
}

void BalloonSet::SetMinDepthScale(float value)
{
    if (value < 0.0001F)
    {
        value = 0.0001F;
    }
    else if (value > m_fMaxDepthScale)
    {
        m_fMaxDepthScale = value;
    }
    
    m_fMinDepthScale = value;
}

float BalloonSet::GetMinDepthScale() const
{
    return m_fMinDepthScale;
}

void BalloonSet::SetFadeOutDepth(float value)
{
    m_fFadeOutDepth = value;
}

float BalloonSet::GetFadeOutDepth() const
{
    return m_fFadeOutDepth;
}

PERSISTID BalloonSet::AddBalloon(const PERSISTID& id, const PERSISTID& model, 
	int type)
{
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(model);
	
	if (NULL == pVisBase)
	{
		return PERSISTID();
	}
	
	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return PERSISTID();
	}
	
	IControl* pControl = (IControl*)GetCore()->GetEntity(id);
	
	if (NULL == pControl)
	{
		return PERSISTID();
	}
	
	if (!pControl->GetEntInfo()->IsKindOf("IControl"))
	{
		return PERSISTID();
	}

	return NewBalloon(pControl, model, type)->GetID();
}

PERSISTID BalloonSet::CreateBalloon(const char* control_name, 
	const PERSISTID& model, int type)
{
	Assert(control_name != NULL);

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(model);
	
	if (NULL == pVisBase)
	{
		return PERSISTID();
	}
	
	if (!pVisBase->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return PERSISTID();
	}
	
	IControl* pControl = GetGui()->CreateControl(control_name);

	if (NULL == pControl)
	{
		return PERSISTID();
	}
	
	return NewBalloon(pControl, model, type)->GetID();
}

// 是否有空闲的已创建汽球
bool BalloonSet::FindTempBalloon(int type)
{
    if (m_bTempBalloons)
    {
        if (type >= 0 && type < (int)m_vTempTypeArray.size())
        {
            if (m_vTempTypeArray[type] != NULL)
            {
                return true;
            }
        }
    }

    return false;
}

// 如果有空闲的已创建汽球，则直接用，没有返回空
PERSISTID BalloonSet::AddTempBalloon(const PERSISTID& model, int type)
{
    if (m_bTempBalloons)
    {
        if (type >= 0 && type < (int)m_vTempTypeArray.size())
        {
            if (m_vTempTypeArray[type] != NULL)
            {
                Balloon* pBalloon = m_vTempTypeArray[type];

				Balloon* pNext = pBalloon->GetNextNode();

                m_vTempTypeArray[type] = pNext;

                if (pNext != NULL)
                {
                   pNext->SetPrevNode(NULL);
                }

                pBalloon->SetType(type);
	            pBalloon->SetModelID(model);
                pBalloon->SetPrevNode(NULL);
                pBalloon->SetNextNode(NULL);
	            pBalloon->Attach(&m_pFirst, &m_pLast);
				pBalloon->SetTemped(false);

                return pBalloon->GetID();
            }
        }
    }

    return PERSISTID();
}

bool BalloonSet::TryAddTempBalloon(Balloon* pBalloon)
{
	if (pBalloon->IsTemped())
	{
		return true;
	}

    if (m_bTempBalloons)
    {
        int type = pBalloon->GetType();

        if (type >= 0 && type < (int)m_vTempTypeArray.size())
        {
            if (NULL == m_vTempTypeArray[type])
            {
                m_vTempTypeArray[type] = pBalloon;
                pBalloon->SetPrevNode(NULL);
                pBalloon->SetNextNode(NULL);
				pBalloon->SetTemped(true);
            }
            else
            {
                m_vTempTypeArray[type]->SetPrevNode(pBalloon);
                pBalloon->SetNextNode(m_vTempTypeArray[type]);
                pBalloon->SetPrevNode(NULL);
                m_vTempTypeArray[type] = pBalloon;
				pBalloon->SetTemped(true);
            }

            return true;
        }
        else
        {
            return false;
        }
    }
	else
    {
        return false;
    }
}

bool BalloonSet::DeleteBalloon(const PERSISTID& id)
{
	Balloon* pBalloon = (Balloon*)GetCore()->GetEntity(id);
	
	if (NULL == pBalloon)
	{
		return false;
	}

	if (pBalloon->IsTemped())
	{
		return true;
	}
	
	if (!pBalloon->GetEntInfo()->IsKindOf("Balloon"))
	{
		return false;
	}
	
	pBalloon->Detach(&m_pFirst, &m_pLast);

    if (!TryAddTempBalloon(pBalloon))
    {
        pBalloon->Release();
    }


	return true;
}

bool BalloonSet::DeleteFromModel(const PERSISTID& id)
{
	Balloon* p = m_pFirst;
	
	while (p)
	{
		Balloon* t = p->GetNextNode();
		
		if (p->GetModelID() == id)
		{
			p->Detach(&m_pFirst, &m_pLast); 

            if (!TryAddTempBalloon(p))
            {
                p->Release();
            }
		}
		
		p = t;
	}
	
	return true;
}

bool BalloonSet::ClearBalloon()
{
	ReleaseAll();

	return true;
}

void BalloonSet::GetBalloonList(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	Balloon* p = m_pFirst;
	
	while (p)
	{
		result.AddObject(p->GetID());
		
		p = p->GetNextNode();
	}
}

void BalloonSet::GetModelBalloonList(const IVarList& args, 
	IVarList& result)
{
	PERSISTID model = args.ObjectVal(0);
	
	result.Clear();
	
	Balloon* p = m_pFirst;
	
	while (p)
	{
		if (p->GetModelID() == model)
		{
			result.AddObject(p->GetID());
		}
		
		p = p->GetNextNode();
	}
}

