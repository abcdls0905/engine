//--------------------------------------------------------------------
// 文件名:		broken_shadow.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年9月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "broken_shadow.h"
#include "actor.h"
#include "../visual/vis_utils.h"
#include "../public/core_log.h"

/// \file broken_shadow.cpp
/// \brief 残影效果

/// entity: BrokenShadow
/// \brief 残影效果实体
/// \see 继承自IVisBase
DECLARE_ENTITY(BrokenShadow, 0, IVisBase);

/// property: float Interval
/// \brief 产生一个残影的间隔时间
DECLARE_PROPERTY(float, BrokenShadow, Interval, GetInterval, SetInterval);
/// property: float LifeTime
/// \brief 生命周期
DECLARE_PROPERTY(float, BrokenShadow, LifeTime, GetLifeTime, SetLifeTime);
/// property: int ShadowCount
/// \brief 残影数量
DECLARE_PROPERTY(int, BrokenShadow, ShadowCount, GetShadowCount, 
	SetShadowCount);
/// property: string Color
/// \brief 颜色
DECLARE_PROPERTY(result_string, BrokenShadow, Color, GetColorString, 
	SetColorString);

#define BS_MIN_COUNT 0
#define BS_MAX_COUNT 10

// BrokenShadow

BrokenShadow::BrokenShadow()
{
	m_fInterval = 0.2F;
	m_fLastCreateTime = 0.0F;
	m_fLifeTime = 0.5F;
	m_nShadowCount = -1;
	m_nMaxShadowCount = BS_MAX_COUNT;
	m_nColor = 0xFFFFFFFF;
	m_bComplete = false;
}

BrokenShadow::~BrokenShadow()
{
	ClearBSInfo();
}

bool BrokenShadow::Init(const IVarList& args)
{
	return true;
}

bool BrokenShadow::Shut()
{
	ClearData();

	return true;
}

void BrokenShadow::Update(float seconds)
{
	m_bComplete = true;

	if (OBJECT_TYPE_INVALID == m_ObjectType)
	{
		return;
	}

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_ObjectID);

	if (!pVisBase)
	{
		return;
	}
	
	size_t uCount = m_vBSInfo.size();

	if (0 == m_nShadowCount && 0 == uCount)
	{
		ClearData();
		return;
	}

	// 当前数据备份
	FmMat4 mat;

	pVisBase->GetWorldMatrix(mat);

	unsigned int color = pVisBase->GetColor();

	if (uCount > 0)
	{
		for (size_t i = 0; i < uCount; ++i)
		{
			m_vBSInfo[i]->alpha -= seconds / m_fLifeTime;
		}
	}

	if (m_nShadowCount == 0)
	{
		// 指定残影个数的情况下，当已经添加指定数量的残影后，不再添加
		for (int i = (int)uCount - 1; i >= 0; --i)
		{
			if (m_vBSInfo[i]->alpha > 0.0F)
			{
				continue;
			}

			ClearData(i);
		}

		if (m_vBSInfo.size() > 0)
		{
			m_bComplete = false;
		}

		return;
	}
	else
	{
		// 插入新信息
		if (m_fLastCreateTime > m_fInterval)
		{
			if ((int)uCount >= m_nMaxShadowCount)
			{
				for (int i = (int)uCount - m_nMaxShadowCount - 1; i >= 0; i--)
				{
					ClearData(i);
				}
			}

			bs_info_t* info = CORE_NEW(bs_info_t);

			info->mat = mat;
			info->alpha = 1.0F;

			if (OBJECT_TYPE_ACTOR == m_ObjectType)
			{
				AddNode(info->child, pVisBase);
			}

			m_vBSInfo.push_back(info);
			m_fLastCreateTime = 0.0F;

			if (m_nShadowCount > 0)
			{
				m_nShadowCount--;
			}
		}
		else
		{
			m_fLastCreateTime += seconds;
		}
	}
	m_bComplete = false;
}

struct node_visible_t
{
	const char* name;
	bool visible;
};

void BrokenShadow::Realize()
{
	if (OBJECT_TYPE_INVALID == m_ObjectType)
	{
		return;
	}

	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_ObjectID);

	if (NULL == pVisBase)
	{
		return;
	}

	if (0 == m_vBSInfo.size())
	{
		return;
	}

	// 当前数据备份
	FmMat4 mat;

	pVisBase->GetWorldMatrix(mat);

	unsigned int color = pVisBase->GetColor();
	TArrayPod<child_t, 1, TCoreAlloc> child_list;

	if (OBJECT_TYPE_ACTOR == m_ObjectType)
	{
		AddNode(child_list, pVisBase);
	}

	// 描画
	size_t uCount = m_vBSInfo.size();

	for (size_t i = 0; i < uCount; i++)
	{
		bs_info_t* info = m_vBSInfo[i];

		if (info->alpha <= 0.0F)
		{
			continue;
		}

		if (OBJECT_TYPE_ACTOR == m_ObjectType)
		{
			Actor* pActor = (Actor*)pVisBase;
			CVarList args, result;

			pActor->GetNoBrokenShadowList(args, result);

			size_t count = result.GetCount();

			for (size_t j = 0; j < info->child.size(); j++)
			{
				Actor* actor = (Actor*)GetCore()->GetEntity(
					info->child[j].id);

				if (NULL != actor && actor->GetVisible())
				{
					pActor->SetWorldMatrix(info->mat);
					actor->SetColor((m_nColor & 0x00FFFFFF) 
						| (int(info->alpha * m_nColor) & 0xFF000000));

					TArrayPod<node_visible_t, 1, TCoreAlloc> old_visible;

					for (size_t k = 0; k < count; ++k)
					{
						node_visible_t data;

						data.name = result.StringVal(k);
						data.visible = actor->GetNodeVisible(data.name);

						old_visible.push_back(data);

						actor->SetNodeVisible(data.name, false);
					}

					actor->SetCurrentPose(info->child[j].pose);
					actor->RealizeBrokenShadow();

					for (size_t k1 = 0; k1 < count; ++k1)
					{
						node_visible_t& data = old_visible[k1];

						actor->SetNodeVisible(data.name, data.visible);
					}
				}
			}

			//pActor->SetColor((m_nColor & 0x00FFFFFF) | (int(info.alpha * m_nColor) & 0xFF000000));
			//pActor->SetWorldMatrix(info.mat);
			//pActor->RealizeBrokenShadow();
		} 
		else
		{
			if (pVisBase->GetVisible())
			{
				pVisBase->SetColor((m_nColor & 0x00FFFFFF) 
					| (int(info->alpha * m_nColor) & 0xFF000000));
				pVisBase->SetWorldMatrix(info->mat);
				pVisBase->Realize();
			}
		}
	}

	// 恢复当前数据
	pVisBase->SetColor(color);
	pVisBase->SetWorldMatrix(mat);

	if (OBJECT_TYPE_ACTOR == m_ObjectType)
	{
		for (size_t i = 0; i < child_list.size(); i++)
		{
			child_t child = child_list[i];
			Actor* actor = (Actor*)GetCore()->GetEntity(child.id);

			if (NULL == actor)
			{
				continue;
			}

			actor->SetCurrentPose(child.pose);
			actor->DeletePose(child.pose);
		}

		child_list.clear();
	}
}

void BrokenShadow::AddNode(TArrayPod<child_t, 1, TCoreAlloc>& info, 
	IVisBase* pVisBase)
{
	if (NULL == pVisBase)
	{
		return;
	}

	if (pVisBase->GetEntInfo()->IsKindOf("Actor"))
	{
		Actor* pActor = (Actor*)pVisBase;

		if (pActor->GetVisible())
		{
			child_t self;

			self.id = pActor->GetID();
			self.pose = pActor->GetCopyPose();

			if (self.pose)
			{
				info.push_back(self);
			}
		}

		CVarList args, result;

		pActor->GetVisualList(args, result);

		for (size_t i = 0; i < result.GetCount(); ++i)
		{
			IVisBase* p = (IVisBase*)GetCore()->GetEntity(result.ObjectVal(i));

			if (p)
			{
				AddNode(info, p);
			}
		}
	}
}

void BrokenShadow::ClearData()
{
	if (OBJECT_TYPE_ACTOR == m_ObjectType)
	{
		Actor* pActor = (Actor*)GetCore()->GetEntity(m_ObjectID);

		if (pActor)
		{
			size_t bs_count = m_vBSInfo.size();

			for (size_t i = 0; i < bs_count; ++i)
			{
				for (size_t j = 0; j < m_vBSInfo[i]->child.size(); ++j)
				{
					pActor->DeletePose(m_vBSInfo[i]->child[j].pose);
				}

				m_vBSInfo[i]->child.clear();
			}
		}
	}

	ClearBSInfo();
}

void BrokenShadow::ClearData(size_t index)
{
	if (OBJECT_TYPE_ACTOR == m_ObjectType)
	{
		Actor* pActor = (Actor*)GetCore()->GetEntity(m_ObjectID);

		if (pActor)
		{
			for (size_t i = 0; i < m_vBSInfo[index]->child.size(); ++i)
			{
				pActor->DeletePose(m_vBSInfo[index]->child[i].pose);
			}

			m_vBSInfo[index]->child.clear();
		}
	}

	CORE_DELETE(m_vBSInfo[index]);
	m_vBSInfo.remove(index);
}

void BrokenShadow::SetObject(const PERSISTID id)
{
	m_ObjectID = id;

	IVisBase* p = (IVisBase*)GetCore()->GetEntity(id);

	if (p->GetEntInfo()->IsKindOf("Actor"))
	{
		m_ObjectType = OBJECT_TYPE_ACTOR;
	}
	else if (p->GetEntInfo()->IsKindOf("Model"))
	{
		m_ObjectType = OBJECT_TYPE_MODEL;
	}
	else if (p->GetEntInfo()->IsKindOf("EffectModel"))
	{
		m_ObjectType = OBJECT_TYPE_EFFECTMODEL;
	} 
	else
	{
		m_ObjectType = OBJECT_TYPE_INVALID;
	}
}

PERSISTID BrokenShadow::GetObjectID() const
{
	return m_ObjectID;
}

void BrokenShadow::SetInterval(float value)
{
	if (value > 0.0F)
	{
		m_fInterval = value;
		m_nMaxShadowCount = int(m_fLifeTime / m_fInterval) + 1;
	}
}

float BrokenShadow::GetInterval() const
{
	return m_fInterval;
}

void BrokenShadow::SetLifeTime(float value)
{
	if (value > 0.0F)
	{
		m_fLifeTime = value;
		m_nMaxShadowCount = int(m_fLifeTime / m_fInterval) + 1;
	}
}

float BrokenShadow::GetLifeTime() const
{
	return m_fLifeTime;
}

void BrokenShadow::SetShadowCount(int value)
{
	if (value < BS_MIN_COUNT || value > BS_MAX_COUNT)
	{
		return;
	}

	m_nShadowCount = value;
}

int BrokenShadow::GetShadowCount()
{
	return m_nShadowCount;
}

void BrokenShadow::SetColorString(const char* value)
{
	Assert(value != NULL);

	m_nColor = VisUtil_ColorToInt(value);
}

result_string BrokenShadow::GetColorString() const
{
	return VisUtil_IntToColor(m_nColor);
}
