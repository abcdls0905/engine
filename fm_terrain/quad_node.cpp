//--------------------------------------------------------------------
// 文件名:		quad_node.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年1月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "quad_node.h"
#include "quad_tree.h"
#include "quad_ground.h"
#include "quad_visual.h"
#include "../utils/tick_timer.h"

// CQuadNode

CQuadNode::CQuadNode(CQuadTree* tree, CQuadNode* parent, size_t scale, 
	float x, float z)
{
	m_pTree = tree;
	m_pParent = parent;
	m_nScale = (unsigned int)scale;
	m_pChild0 = NULL;
	m_pChild1 = NULL;
	m_pChild2 = NULL;
	m_pChild3 = NULL;
	m_vMin.x = x;
	//m_vMin.y = 0.0F;
	m_vMin.y = 1000.0F;
	m_vMin.z = z;
	m_vMax.x = x + scale;
	//m_vMax.y = 0.0F;
	m_vMax.y = -1000.0F;
	m_vMax.z = z + scale;
	m_pGround = NULL;
}

CQuadNode::~CQuadNode()
{
	if (m_pChild0)
	{
		m_pTree->DeleteNode(m_pChild0);
	}

	if (m_pChild1)
	{
		m_pTree->DeleteNode(m_pChild1);
	}

	if (m_pChild2)
	{
		m_pTree->DeleteNode(m_pChild2);
	}

	if (m_pChild3)
	{
		m_pTree->DeleteNode(m_pChild3);
	}
}

// 点是否在矩形中
static inline bool point_in_rect(float x, float z, float x1, float z1, 
	float x2, float z2)
{
	return (x >= x1) && (x < x2) && (z >= z1) && (z < z2);
}

CQuadNode* CQuadNode::GetInNode(size_t scale, float x, float z)
{
	if (m_nScale <= scale)
	{
		return this;
	}
	
	if (m_pChild0)
	{
		if (point_in_rect(x, z, m_pChild0->m_vMin.x, m_pChild0->m_vMin.z, 
			m_pChild0->m_vMax.x, m_pChild0->m_vMax.z))
		{
			return m_pChild0->GetInNode(scale, x, z);
		}
	}
	
	if (m_pChild1)
	{
		if (point_in_rect(x, z, m_pChild1->m_vMin.x, m_pChild1->m_vMin.z, 
			m_pChild1->m_vMax.x, m_pChild1->m_vMax.z))
		{
			return m_pChild1->GetInNode(scale, x, z);
		}
	}

	if (m_pChild2)
	{
		if (point_in_rect(x, z, m_pChild2->m_vMin.x, m_pChild2->m_vMin.z, 
			m_pChild2->m_vMax.x, m_pChild2->m_vMax.z))
		{
			return m_pChild2->GetInNode(scale, x, z);
		}
	}

	if (m_pChild3)
	{
		if (point_in_rect(x, z, m_pChild3->m_vMin.x, m_pChild3->m_vMin.z, 
			m_pChild3->m_vMax.x, m_pChild3->m_vMax.z))
		{
			return m_pChild3->GetInNode(scale, x, z);
		}
	}

	return NULL;
}

CQuadNode* CQuadNode::GetInNewNode(size_t scale, float x, float z)
{
	if (m_nScale <= scale)
	{
		return this;
	}

	size_t hscale = m_nScale / 2;

	if (NULL == m_pChild0)
	{
		m_pChild0 = m_pTree->NewNode(this, hscale, m_vMin.x, m_vMin.z);
	}

	if (NULL == m_pChild1)
	{
		m_pChild1 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z);
	}

	if (NULL == m_pChild2)
	{
		m_pChild2 = m_pTree->NewNode(this, hscale, m_vMin.x, 
			m_vMin.z + hscale);
	}

	if (NULL == m_pChild3)
	{
		m_pChild3 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z + hscale);
	}

	if (point_in_rect(x, z, m_pChild0->m_vMin.x, m_pChild0->m_vMin.z, 
		m_pChild0->m_vMax.x, m_pChild0->m_vMax.z))
	{
		return m_pChild0->GetInNewNode(scale, x, z);
	}

	if (point_in_rect(x, z, m_pChild1->m_vMin.x, m_pChild1->m_vMin.z, 
		m_pChild1->m_vMax.x, m_pChild1->m_vMax.z))
	{
		return m_pChild1->GetInNewNode(scale, x, z);
	}

	if (point_in_rect(x, z, m_pChild2->m_vMin.x, m_pChild2->m_vMin.z, 
		m_pChild2->m_vMax.x, m_pChild2->m_vMax.z))
	{
		return m_pChild2->GetInNewNode(scale, x, z);
	}

	if (point_in_rect(x, z, m_pChild3->m_vMin.x, m_pChild3->m_vMin.z, 
		m_pChild3->m_vMax.x, m_pChild3->m_vMax.z))
	{
		return m_pChild3->GetInNewNode(scale, x, z);
	}

	return NULL;
}

CQuadNode* CQuadNode::GetLeftNode()
{
	if (NULL == m_pParent)
	{
		return NULL;
	}
		
	if (m_pParent->m_pChild0 == this)
	{
		CQuadNode* pLeft = m_pParent->GetLeftNode();

		if (pLeft)
		{
			return pLeft->m_pChild1;
		}
	}
	else if (m_pParent->m_pChild1 == this)
	{
		return m_pParent->m_pChild0;
	}
	else if (m_pParent->m_pChild2 == this)
	{
		CQuadNode* pLeft = m_pParent->GetLeftNode();
		
		if (pLeft)
		{
			return pLeft->m_pChild3;
		}
	}
	else if (m_pParent->m_pChild3 == this)
	{
		return m_pParent->m_pChild2;
	}

	return NULL;
}

CQuadNode* CQuadNode::GetTopNode()
{
	if (NULL == m_pParent)
	{
		return NULL;
	}
	
	if (m_pParent->m_pChild0 == this)
	{
		CQuadNode* pTop = m_pParent->GetTopNode();
		
		if (pTop)
		{
			return pTop->m_pChild2;
		}
	}
	else if (m_pParent->m_pChild1 == this)
	{
		CQuadNode* pTop = m_pParent->GetTopNode();
		
		if (pTop)
		{
			return pTop->m_pChild3;
		}
	}
	else if (m_pParent->m_pChild2 == this)
	{
		return m_pParent->m_pChild0;
	}
	else if (m_pParent->m_pChild3 == this)
	{
		return m_pParent->m_pChild1;
	}
	
	return NULL;
}

CQuadNode* CQuadNode::GetRightNode()
{
	if (NULL == m_pParent)
	{
		return NULL;
	}
	
	if (m_pParent->m_pChild0 == this)
	{
		return m_pParent->m_pChild1;
	}
	else if (m_pParent->m_pChild1 == this)
	{
		CQuadNode* pRight = m_pParent->GetRightNode();
		
		if (pRight)
		{
			return pRight->m_pChild0;
		}
	}
	else if (m_pParent->m_pChild2 == this)
	{
		return m_pParent->m_pChild3;
	}
	else if (m_pParent->m_pChild3 == this)
	{
		CQuadNode* pRight = m_pParent->GetRightNode();
		
		if (pRight)
		{
			return pRight->m_pChild2;
		}
	}

	return NULL;
}

CQuadNode* CQuadNode::GetBottomNode()
{
	if (NULL == m_pParent)
	{
		return NULL;
	}
	
	if (m_pParent->m_pChild0 == this)
	{
		return m_pParent->m_pChild2;
	}
	else if (m_pParent->m_pChild1 == this)
	{
		return m_pParent->m_pChild3;
	}
	else if (m_pParent->m_pChild2 == this)
	{
		CQuadNode* pBottom = m_pParent->GetBottomNode();
		
		if (pBottom)
		{
			return pBottom->m_pChild0;
		}
	}
	else if (m_pParent->m_pChild3 == this)
	{
		CQuadNode* pBottom = m_pParent->GetBottomNode();
		
		if (pBottom)
		{
			return pBottom->m_pChild1;
		}
	}
	
	return NULL;
}

// 矩形是否相交
static inline bool rect_intersect(float x1, float z1, float x2, float z2,
	float x3, float z3, float x4, float z4)
{
	return (x2 >= x3) && (x4 >= x1)	&& (z2 >= z3) && (z4 >= z1);
}

bool CQuadNode::AddGround(CQuadGround* pGround)
{ 
	Assert(pGround != NULL);
	
	float x1 = pGround->m_vMin.x;
	float z1 = pGround->m_vMin.z;
	float x2 = pGround->m_vMax.x;
	float z2 = pGround->m_vMax.z;

	if (!rect_intersect(x1, z1, x2, z2, m_vMin.x, m_vMin.z, m_vMax.x, 
		m_vMax.z))
	{
		// 物体与节点不相交
		return false;
	}

	// 尺度是否匹配
	if (m_nScale <= pGround->m_nScale)
	{
		Assert(NULL == m_pGround);
		m_pGround = pGround;
		pGround->m_pNode = this;
		UpdateScaleY();
		return true;
	}

	size_t hscale = m_nScale / 2;

	if (NULL == m_pChild0)
	{
		m_pChild0 = m_pTree->NewNode(this, hscale, m_vMin.x, m_vMin.z);
	}

	if (NULL == m_pChild1)
	{
		m_pChild1 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z);
	}

	if (NULL == m_pChild2)
	{
		m_pChild2 = m_pTree->NewNode(this, hscale, m_vMin.x, 
			m_vMin.z + hscale);
	}

	if (NULL == m_pChild3)
	{
		m_pChild3 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z + hscale);
	}

	if (m_pChild0->AddGround(pGround))
	{
		return true;
	}

	if (m_pChild1->AddGround(pGround))
	{
		return true;
	}

	if (m_pChild2->AddGround(pGround))
	{
		return true;
	}

	if (m_pChild3->AddGround(pGround))
	{
		return true;
	}

	return false;
}

bool CQuadNode::RemoveGround(CQuadGround* pGround)
{
	Assert(pGround != NULL);

	if (m_pGround == pGround)
	{
		m_pGround = NULL;
		UpdateScaleY();
		return true;
	}

	return false;
}

bool CQuadNode::AddObject(CQuadVisual* pObject)
{
	Assert(pObject != NULL);

	float x1 = pObject->m_vMin.x;
	float z1 = pObject->m_vMin.z;
	float x2 = pObject->m_vMax.x;
	float z2 = pObject->m_vMax.z;

	if (!rect_intersect(x1, z1, x2, z2, m_vMin.x, m_vMin.z, m_vMax.x, 
		m_vMax.z))
	{
		// 物体与节点不相交
		return false;
	}

	// 尺度是否匹配
	if (m_nScale <= pObject->m_nScale)
	{
		m_Objects.push_back(pObject);
		// 碰撞对象记录此节点
		pObject->AddNode(this);
		
		if (pObject->m_vMin.y < m_vMin.y)
		{
			m_vMin.y = pObject->m_vMin.y - 0.001F;
		}
		
		if(pObject->m_vMax.y > m_vMax.y)
		{
			m_vMax.y = pObject->m_vMax.y + 0.001F;
		}
		//UpdateScaleY();

		return true;
	}

	size_t hscale = m_nScale / 2;

	if (NULL == m_pChild0)
	{
		m_pChild0 = m_pTree->NewNode(this, hscale, m_vMin.x, m_vMin.z);
	}

	if (NULL == m_pChild1)
	{
		m_pChild1 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z);
	}

	if (NULL == m_pChild2)
	{
		m_pChild2 = m_pTree->NewNode(this, hscale, m_vMin.x, 
			m_vMin.z + hscale);
	}

	if (NULL == m_pChild3)
	{
		m_pChild3 = m_pTree->NewNode(this, hscale, m_vMin.x + hscale, 
			m_vMin.z + hscale);
	}

	m_pChild0->AddObject(pObject);
	m_pChild1->AddObject(pObject);
	m_pChild2->AddObject(pObject);
	m_pChild3->AddObject(pObject);

	return true;
}

bool CQuadNode::RemoveObject(CQuadVisual* pObject)
{
	Assert(pObject != NULL);
	
	size_t object_num = m_Objects.size();
	CQuadVisual** objects = m_Objects.data();
	
	for (size_t i = 0; i < object_num; ++i)
	{
		if (objects[i] == pObject)
		{
			m_Objects.remove(i);
			
			if ((pObject->m_vMin.y < (m_vMin.y + 0.002f))
				|| (pObject->m_vMax.y > (m_vMax.y - 0.002f)))
			{
				UpdateScaleY();
			}

			return true;
		}
	}
	
	return false;
}

void CQuadNode::UpdateParentScaleY(size_t zone_scale)
{
	CQuadNode* parent = m_pParent;
	
	if (parent && (parent->m_nScale <= zone_scale))
	{
		if (m_vMin.y < parent->m_vMin.y)
		{
			parent->m_vMin.y = m_vMin.y;
		
			if (m_vMax.y > parent->m_vMax.y)
			{
				parent->m_vMax.y = m_vMax.y;
			}

			parent->UpdateParentScaleY(zone_scale);
		}
		else if (m_vMax.y > parent->m_vMax.y)
		{
			parent->m_vMax.y = m_vMax.y;
			parent->UpdateParentScaleY(zone_scale);
		}
	}
}

void CQuadNode::UpdateScaleY()
{
	if (NULL == m_pGround)
	{
		m_vMin.y = 0.0F;
		m_vMax.y = 0.0F;
		return;
	}

	float min_y = m_pGround->m_vMin.y - 0.001F;
	float max_y = m_pGround->m_vMax.y + 0.001F;
	size_t object_num = m_Objects.size();
	CQuadVisual** objects = m_Objects.data();

	for (size_t i = 0; i < object_num; ++i)
	{
		CQuadVisual* object = objects[i];

		if (object->m_vMin.y < min_y)
		{
			min_y = object->m_vMin.y - 0.001F;
		}

		if (object->m_vMax.y > max_y)
		{
			max_y = object->m_vMax.y + 0.001F;
		}
	}

	m_vMin.y = min_y;
	m_vMax.y = max_y;

	// 现在暂不使用此代码 
	//UpdateParentScaleY(m_pTree->GetZoneScale());
}

void CQuadNode::Compress(CTickTimer* tick_counter, size_t ms)
{
	if (tick_counter)
	{
		// 控制运行的时间
		if (tick_counter->GetElapseMillisec(0) >= int(ms))
		{
			return;
		}
	}
	
	if (m_pChild0)
	{
		m_pChild0->Compress(tick_counter, ms);

		if ((NULL == m_pChild0->m_pGround) 
			&& m_pChild0->m_Objects.empty()
			&& (NULL == m_pChild0->m_pChild0) 
			&& (NULL == m_pChild0->m_pChild1)
			&& (NULL == m_pChild0->m_pChild2)
			&& (NULL == m_pChild0->m_pChild3))
		{
			m_pTree->DeleteNode(m_pChild0);
			m_pChild0 = NULL;
		}
	}
	
	if (m_pChild1)
	{
		m_pChild1->Compress(tick_counter, ms);

		if ((NULL == m_pChild1->m_pGround)
			&& m_pChild1->m_Objects.empty()
			&& (NULL == m_pChild1->m_pChild0) 
			&& (NULL == m_pChild1->m_pChild1)
			&& (NULL == m_pChild1->m_pChild2)
			&& (NULL == m_pChild1->m_pChild3))
		{
			m_pTree->DeleteNode(m_pChild1);
			m_pChild1 = NULL;
		}
	}
	
	if (m_pChild2)
	{
		m_pChild2->Compress(tick_counter, ms);

		if ((NULL == m_pChild2->m_pGround)
			&& m_pChild2->m_Objects.empty()
			&& (NULL == m_pChild2->m_pChild0) 
			&& (NULL == m_pChild2->m_pChild1)
			&& (NULL == m_pChild2->m_pChild2)
			&& (NULL == m_pChild2->m_pChild3))
		{
			m_pTree->DeleteNode(m_pChild2);
			m_pChild2 = NULL;
		}
	}
	
	if (m_pChild3)
	{
		m_pChild3->Compress(tick_counter, ms);

		if ((NULL == m_pChild3->m_pGround)
			&& m_pChild3->m_Objects.empty()
			&& (NULL == m_pChild3->m_pChild0) 
			&& (NULL == m_pChild3->m_pChild1)
			&& (NULL == m_pChild3->m_pChild2)
			&& (NULL == m_pChild3->m_pChild3))
		{
			m_pTree->DeleteNode(m_pChild3);
			m_pChild3 = NULL;
		}
	}
}

