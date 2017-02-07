//--------------------------------------------------------------------
// �ļ���:		balloon_set.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
/// \brief �����ڳ�����ģ�͵�����ؼ�����

/// entity: BalloonSet
/// \brief �����ڳ�����ģ�͵�����ؼ�����ʵ��
/// \see �̳���IControl
DECLARE_ENTITY(BalloonSet, 0, IControl);

/// property: object Scene
/// \brief ���������
DECLARE_PROPERTY(PERSISTID, BalloonSet, Scene, GetSceneID, SetSceneID);
/// property: bool Sort
/// \brief �Ƿ���Ҫ����
DECLARE_PROPERTY(bool, BalloonSet, Sort, GetSort, SetSort);
/// property: bool UseDepth
/// \brief �Ƿ�ʹ�����
DECLARE_PROPERTY(bool, BalloonSet, UseDepth, GetUseDepth, SetUseDepth);

/// property: bool UseDepthScale
/// \brief �Ƿ�ʹ����ȷ���
DECLARE_PROPERTY(bool, BalloonSet, UseDepthScale, GetUseDepthScale, SetUseDepthScale);

/// property: bool TempBalloons
/// \brief �Ƿ񻺴沢��������
DECLARE_PROPERTY(bool, BalloonSet, TempBalloons, GetTempBalloons, SetTempBalloons);

/// property: int MaxBalloonType
/// \brief ���������������
DECLARE_PROPERTY(int, BalloonSet, MaxBalloonType, GetMaxBalloonType, SetMaxBalloonType);

/// property: float MaxDepthScale
/// \brief �����ȷ���
DECLARE_PROPERTY(float, BalloonSet, MaxDepthScale, GetMaxDepthScale, SetMaxDepthScale);

/// property: float MinDepthScale
/// \brief ��С��ȷ���
DECLARE_PROPERTY(float, BalloonSet, MinDepthScale, GetMinDepthScale, SetMinDepthScale);

/// property: float FadeOutDepth
/// \brief �������룬Ŀ����ȴ��ڴ�ֵ����ʾͷ���ؼ�
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

// ÿִ֡���߼����������ӵ����ж��У�
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
			
			// ��󶨶���һ��ɾ��
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
	// ����
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
			// �������
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
	
	// �Ƿ��ǿ����ʾ
	//if (!pBalloon->GetMustVisible())
	{
        FmVec3 len = cam_pos - pos;
		float dist_square = FmVec3LengthSq(&len);
		float ball_near = pBalloon->GetNearDistance();
		float ball_far = pBalloon->GetFarDistance();
		
		if ((dist_square < (ball_near * ball_near))
			|| (dist_square > (ball_far * ball_far)))
		{
			// ������Ч�ľ��뷶Χ��
			return false;
		}
	}
	
	FmVec4 proj_pos;

	FmVec3Transform(&proj_pos, &pos, &mtxViewProj);

	float depth_z = proj_pos.z / proj_pos.w;
	float depth_w = proj_pos.w;
	
	if ((depth_z > 1.0F) || (depth_z < 0.0F))
	{
		// ������ʾ�����
		return false;
	}

	pBalloon->SetDepth(depth_z);

	if ((m_fFadeOutDepth > 0.0F) && (depth_z > m_fFadeOutDepth))
    {
        // ������ʾ�������ʾ
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
	// �ؼ����ĵ��λ��
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

		// ������Ļ
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

	// ��ǰ��������Ⱦ������
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

    // ������Ҫɾ����ͷ���ؼ�
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

// �Ƿ�ʹ��Զ������
void BalloonSet::SetUseDepthScale(bool value)
{
    m_bUseDepthScale = value;
}

bool BalloonSet::GetUseDepthScale() const
{
    return m_bUseDepthScale;
}

// �Ƿ񻺴沢��������
void BalloonSet::SetTempBalloons(bool value)
{
    m_bTempBalloons = value;
}

bool BalloonSet::GetTempBalloons() const
{
    return m_bTempBalloons;
}

// ����������������
void BalloonSet::SetMaxBalloonType(int value)
{
    if (value < 0)
    {
        value = 0;
    }

    if (m_nMaxBalloonType != value)
    {
        // ��������
        ReleaseAllTemp();

        // ���õ�ǰ
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

// �Ƿ��п��е��Ѵ�������
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

// ����п��е��Ѵ���������ֱ���ã�û�з��ؿ�
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

