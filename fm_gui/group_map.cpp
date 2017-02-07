//--------------------------------------------------------------------
// �ļ���:		GroupMap.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2010��4��23��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "group_map.h"
#include "mark_string.h"
#include "picture.h"
#include "imageanimation_manager.h"
#include "../visual/i_vis_base.h"

/// \file GroupMap.cpp
/// \brief �ı���ͼƬ��ǩ�ؼ�

/// entity: GroupMap
/// \brief �ı���ͼƬ��ǩ�ؼ�ʵ��
/// \see �̳���IControl
DECLARE_ENTITY(GroupMap, 0, IControl);

/// property: object MapControl
/// \brief ���õ�ͼ�ؼ�
DECLARE_PROPERTY(PERSISTID, GroupMap, MapControl, GetMapControl, 
	SetMapControl);

#if 0	//DECLARE_METHOD
	/// method: bool SetShape(int shape)
/// \brief  ������״
DECLARE_METHOD_1(bool, GroupMap, SetShape, int);

/// method: bool AddType(string type, string pic)
/// \brief  ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ��
DECLARE_METHOD_2(bool, GroupMap, AddType, const char*, const char*);

/// method: bool AddType(string type, string pic, int type)
/// \brief  ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ��
DECLARE_METHOD_5(bool, GroupMap, AddTypeEx, const char*, const char*, int, int, int);

/// method: bool AddMainPlayerBind(string pic, object visobj)
/// \brief  �����ҿ��Ӷ���İ�����������⴦��
DECLARE_METHOD_2(bool, GroupMap, AddMainPlayerBind, const char*, const PERSISTID&);

/// method: bool AddBind(string type, object visobj)
/// \brief  ��ӿ��Ӷ���İ�
DECLARE_METHOD_2(bool, GroupMap, AddBind, const char*, const PERSISTID&);

/// method: bool DelBind(const PERSISTID& visobj)
/// \brief  ��ӿ��Ӷ���İ�
DECLARE_METHOD_1(bool, GroupMap, DelBind, const PERSISTID&);

/// method: bool Clear()
/// \brief  ������ж���İ�
DECLARE_METHOD_0(bool, GroupMap, Clear);

/// method: bool InitTerrain(float start_x, float start_z, float width, float height)
/// \brief  ��ʼ��������С��Ϣ
DECLARE_METHOD_4(bool, GroupMap, InitTerrain, float, float, float, float);

/// method: [map_x, map_z] GetPosInMap(float x, float z)
/// \brief  ����������ת������ͼͼƬ������
DECLARE_METHOD_N(GroupMap, GetPosInMap);

/// method: bool ShowType(int npc_type, bool flag)
/// \brief  ���ƿ��Ӷ������ʾ
DECLARE_METHOD_2(bool, GroupMap, ShowType, int, bool);

/// method: bool SetPlayerOffsetAngleY(float fAngle)
/// \brief  // ������ҵ�ƫ�ƽǶ�
DECLARE_METHOD_1(bool, GroupMap, SetPlayerOffsetAngleY, float);
#endif

/// property:
/// event: int on_click(object self, object obj)
/// \brief ���������
/// \param self ���ؼ�
/// \param obj ����ID
DECLARE_EVENT(on_click_obj);

/// event: int on_mouse_in_obj(object self, object obj)
/// \brief ����������ص�
/// \param self ���ؼ�
/// \param obj ����ID
DECLARE_EVENT(on_mouse_in_obj);

/// event: int on_mouse_out_obj(object self, object obj)
/// \brief ����������ص�
/// \param self ���ؼ�
/// \param obj ����ID
DECLARE_EVENT(on_mouse_out_obj);

// GroupMap

GroupMap::GroupMap()
{
	m_bSolid = false;
	// ȱʡΪ�ж�͸��
	//m_bTransparent = false;
	//SetTransparentDefault(false);
	m_bClickEvent = false;
	m_pLblMainPlayer = NULL;
	m_fOffsetAngleY = 0.0f;

	SetDesignSize(80, 16);
	SetCanSelect(true);
	SetNoFrame(true);
	SetShape(ShapeType_Rect);
}

GroupMap::~GroupMap()
{
}

bool GroupMap::Init(const IVarList& args)
{
	if (!IControl::Init(args))
	{
		return false;
	}

	m_pLblMainPlayer = (Label*)GetGui()->CreateControl("Label");

	if (m_pLblMainPlayer != NULL)
	{
		m_pLblMainPlayer->SetParentControl(this);
		m_pLblMainPlayer->SetText(L"*");
	}

	return true;
}

bool GroupMap::Shut()
{
	if (GetPainter() != NULL)
	{
		for (size_t i = 0; i < m_vTypes.size(); ++i)
		{
			GetPainter()->ReleaseGlyph(m_vTypes[i].pic_id);
		}
	}

	m_vTypes.clear();

	if(m_pLblMainPlayer != NULL && GetGui() != NULL)
	{
		GetGui()->DeleteControl(m_pLblMainPlayer);
		m_pLblMainPlayer = NULL;
	}

	return IControl::Shut();
}

void GroupMap::PrepareDesignInfo(IDesignInfo* di)
{
	IControl::PrepareDesignInfo(di);

	di->RemovePropInfo("NoFrame");
	
	CVar varDefault;
	di->AddPropInfo("NoFrame", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true);
	//di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
	//	&CVar(VTYPE_BOOL, true));	

	di->AddEventInfo("on_click_obj");
	di->AddEventInfo("on_mouse_in_obj");
	di->AddEventInfo("on_mouse_out_obj");
}

//����������ת������ͼͼƬ������
bool GroupMap::TransPosToMap(float x, float z, float& map_x, float& map_z)
{
	Picture* pPicture = (Picture*)GetCore()->GetEntity(m_Control);

	if (NULL != pPicture)
	{
		float map_width = (float)pPicture->GetImageWidth();
		float map_height = (float)pPicture->GetImageHeight();

		map_x = map_width - ((x - m_fTerrainStartX) / m_fTerrainWidth) * map_width;
		map_z = ((z - m_fTerrainStartZ) / m_fTerrainHeight) * map_height;
		return true;
	}

	return false;
}


// ����������ת������ͼͼƬ������
void GroupMap::GetPosInMap(const IVarList& params, IVarList& result)
{
	float x = params.FloatVal(0);
	float z = params.FloatVal(1);
	float map_x, map_z;

	if (TransPosToMap(x, z, map_x, map_z))
	{
		result.AddFloat(map_x);
		result.AddFloat(map_z);
	}

	result.AddFloat(-1.0f);
	result.AddFloat(-1.0f);
}

// ������ҵ�ƫ�ƽǶ�
bool GroupMap::SetPlayerOffsetAngleY(float fAngleY)
{
	m_fOffsetAngleY = fAngleY;
	return true;
}

bool GroupMap::RefreshInnerAbsPos()
{
	if (GetRefreshInnerAbsPos())
	{
		if (m_pLblMainPlayer != NULL) 
		{
			m_pLblMainPlayer->SetRefreshInnerAbsPos(true);
		}
		
		IControl::RefreshInnerAbsPos();
	}

	return true;
}

void GroupMap::Paint(float seconds)
{
	if (!GetVisible())
	{
		return;
	}
	
	RefreshInnerAbsPos();

	int x1 = GetAbsLeft();
	int y1 = GetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

	if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}
	else
	{
		if (m_bSolid)
		{
			GetPainter()->FillRect(x1, y1, x2, y2, GetBackColor());
		}
		
		if (!GetNoFrame())
		{
			GetPainter()->DrawRect(x1, y1, x2, y2, GetLineColor());
		}
	}

	Picture* pPicture = (Picture*)GetCore()->GetEntity(m_Control);

	// ��ͼ��ʾ�ؼ�������
	if (NULL == pPicture)
	{
		return;
	}

	if (m_pLblMainPlayer != NULL)
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_MainPlayerID);
		
		if (pVisBase != NULL)
		{
			FmVec3 pos = pVisBase->GetPosition();
			float angle_y = pVisBase->GetAngle().y;

			m_pLblMainPlayer->SetAngleZ(-angle_y + 3.1416f + m_fOffsetAngleY);

			float map_x, map_z;
			if (TransPosToMap(pos.x, pos.z, map_x, map_z))
			{
				//��ͼ�ĳߴ�
				int center_x = (int)map_x;
				int center_z = (int)map_z;
				int map_width = pPicture->GetImageWidth();
				int map_height = pPicture->GetImageHeight();
				
				if (center_x < pPicture->GetWidth() / 2)
				{
					center_x = pPicture->GetWidth() / 2;
				}
				else if (center_x > map_width - pPicture->GetWidth() / 2)
				{
					center_x = map_width - pPicture->GetWidth() / 2;
				}

				if (center_z < pPicture->GetHeight() / 2)
				{
					center_z = pPicture->GetHeight() / 2;
				}
				else if (center_z > map_height - pPicture->GetHeight() / 2)
				{
					center_z = map_height - pPicture->GetHeight() / 2;
				}

				pPicture->SetCenterX((int)center_x);
				pPicture->SetCenterY((int)center_z);
			}
		}
	}

	int pict_center_x = pPicture->GetCenterX();
	int pict_center_y = pPicture->GetCenterY();
	int pict_width = pPicture->GetWidth();
	int pict_height = pPicture->GetHeight();
	float pict_zoom_width = float(pPicture->GetZoomWidth());
	float pict_zoom_height = float(pPicture->GetZoomHeight());
	std::vector<BindElement>::iterator it = m_vElements.begin();

	while(it != m_vElements.end())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(it->visual_obj);

		if (NULL == pVisBase)
		{
			// �����������ʧ���Ƴ���
			it = m_vElements.erase(it);
		}
		else
		{
			// �Ƿ���Ҫ��ʾ?
			bool flag;

			if (!m_mapShows.GetData(it->bind_type, flag))
			{
				++it;
				continue;
			}

			if (!flag)
			{
				++it;
				continue;
			}

			//std::map<int, bool>::iterator iter = m_mapShows.find(it->bind_type);
			//if ((m_mapShows.end() == iter) || (m_mapShows.end() != iter && !iter->second))
			//{
			//	++it;
			//	continue;
			//}

			// ���������ڣ���ȡ����������λ��
			FmVec3 pos = pVisBase->GetPosition();
			// ת����ͼ������
			float map_x, map_z;

			if (!TransPosToMap(pos.x, pos.z, map_x, map_z))
			{
				++it;
				continue;
			}

			// ת������Ե�ǰ��ʾ�е�����, �ݵ�ͼ����������ʵ����ʾ����
			float sx = (map_x - pict_center_x) * pict_zoom_width;
			float sz = (map_z - pict_center_y) * pict_zoom_height;

			// ת�ɵ�ͼ�ؼ��������,��ͼ�ؼ��ϵ�λ��
			sx = sx + pict_width / 2;
			sz = sz + pict_height / 2;

			// �����ؼ���ʾ��Χ
			if (ShapeType_Rect == m_Shape
				&& !GuiUtil_PointInRect(int(sx), int(sz), 0, 0, pict_width, pict_height))
			{
				++it;
				continue;
			}
			else if (ShapeType_Round == m_Shape
				&& !GuiUtil_PointInRound(int(sx), int(sz), 0, 0, pict_width, pict_height))
			{
				++it;
				continue;
			}

			// ����
			ElementType& element_type = m_vTypes[it->type_index];

			int bx = int(sx - ((float)element_type.width) / 2.0f);
			int bz = int(sz - ((float)element_type.height) / 2.0f);
			int ex = bx + element_type.width;
			int ez = bz + element_type.height;
			
			it->rect.left = bx;
			it->rect.top = bz;
			it->rect.right = ex;
			it->rect.bottom = ez;

			switch(element_type.pic_type)
			{
			case PicType_Glyph:
				GetPainter()->DrawGlyph(x1 + bx, y1 + bz, x1 + ex, y1 + ez, 0, 0, 
					element_type.width, element_type.height, element_type.pic_id);
				break;
			case PicType_Animation:
				ImageAnimationManager* pAniMng = GetGui()->GetAnimationManager();
				rect_t rect = { x1 + bx, y1 + bz, x1 + ex, y1 + ez };
				pAniMng->DrawAnimation(element_type.pic.c_str(), x1 + bx, y1 + bz, 
					rect, 0xffffffff);
				break;
			}

			//
			++it;
		}
	}

	if (m_pLblMainPlayer != NULL)
	{
		m_pLblMainPlayer->Paint(seconds);
	}
}

void GroupMap::SetSolid(bool value) 
{ 
	m_bSolid = value; 
}

bool GroupMap::GetSolid() const 
{ 
	return m_bSolid; 
}

//void GroupMap::SetTransparent(bool value)
//{
//	m_bTransparent = value;
//}
//
//bool GroupMap::GetTransparent() const
//{
//	return m_bTransparent;
//}

void GroupMap::SetClickEvent(bool value)
{
	m_bClickEvent = value;

	// ͬʱҪ�ı��ܷ���״̬
	SetCanSelect(value);
}

bool GroupMap::GetClickEvent() const
{
	return m_bClickEvent;
}

bool GroupMap::SetShape(int shape)
{
	m_Shape = ShapeType_Rect;
	if (ShapeType_Rect == shape || ShapeType_Round == shape)
		m_Shape = shape;

	return true;
}

// ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ��
bool GroupMap::AddType(const char* type, const char* pic)
{
	return AddTypeEx(type, pic, PicType_Glyph, 32, 32);
}

// ��ʼ��ʱ��ӣ�ֻ��Ӳ�ɾ�� ����ͼƬ����
bool GroupMap::AddTypeEx(const char* type, const char* pic, int pic_type,
	int w, int h)
{
	if (m_mapTypes.Exists(type))
	{
		return false;
	}

	if (pic_type >= PicType_Max)
	{
		return false;
	}

	if (pic_type == PicType_Animation)
	{
		ImageAnimationManager* pAniMng = GetGui()->GetAnimationManager();

		if (!pAniMng->IsAnimation(pic))
		{	
			return false;
		}
	}

	ElementType element_type;

	element_type.type = type;
	element_type.pic = pic;
	element_type.pic_type = pic_type;

	switch(element_type.pic_type)
	{
	case PicType_Glyph:
		element_type.pic_id = GetPainter()->CreateGlyph(pic, false);
		element_type.width = GetPainter()->GetGlyphWidth(element_type.pic_id);
		element_type.height = GetPainter()->GetGlyphHeight(element_type.pic_id);
		break;
	case PicType_Animation:
		element_type.pic_id = PERSISTID();
		element_type.width = w;
		element_type.height = h;
		break;
	}

	m_vTypes.push_back(element_type);
	m_mapTypes.Add(type, m_vTypes.size() - 1);

	return true;
}

// �����ҿ��Ӷ���İ�����������⴦��
bool GroupMap::AddMainPlayerBind(const char* pic, const PERSISTID& visobj)
{
	if (m_pLblMainPlayer != NULL)
	{
		m_pLblMainPlayer->SetText(L"");
		m_pLblMainPlayer->SetBackImage(pic);
		m_pLblMainPlayer->SetAutoSize(true);
		m_pLblMainPlayer->SetRotate(true);
		m_pLblMainPlayer->SetTransparent(true);
		m_pLblMainPlayer->SetLeft((this->GetWidth() - m_pLblMainPlayer->GetWidth()) / 2);
		m_pLblMainPlayer->SetTop((this->GetHeight() - m_pLblMainPlayer->GetHeight()) / 2);
		m_MainPlayerID = visobj;
	}
	return true;
}

int GroupMap::GetCustomInt(IEntity* pEntity, const char* name)
{
	if (NULL == pEntity)
	{
		return 0;
	}
	IVar* pVar = pEntity->GetCustoms()->GetValue(name);

	if (NULL == pVar)
	{
		return 0;
	}

	return pVar->IntVal();
}

// ��ӿ��Ӷ���İ�
bool GroupMap::AddBind(const char* type, const PERSISTID& visobj)
{
	size_t index;

	if (!m_mapTypes.GetData(type, index))
	{
		return false;
	}

	BindElement bindelement;

	IEntity* pVis = GetCore()->GetEntity(visobj);
	bindelement.bind_type = GetCustomInt(pVis, "bind_type");

	bindelement.type_index = index;
	bindelement.visual_obj = visobj;
    bindelement.rect.left = 0;
    bindelement.rect.top = 0;
    bindelement.rect.right = 1;
    bindelement.rect.bottom = 1;

	m_vElements.push_back(bindelement);

	return true;
}

// ɾ���󶨶���
bool GroupMap::DelBind(const PERSISTID& visobj)
{
	std::vector<BindElement>::iterator iter = m_vElements.begin();
	std::vector<BindElement>::iterator end = m_vElements.end();
	for (; iter != end; ++iter)
	{
		if (visobj == iter->visual_obj)
		{
			m_vElements.erase(iter);
			return true;
		}
	}

	return false;
}

// ������ж���İ�
bool GroupMap::Clear()
{
	m_vElements.clear();
	return true;
}

// ���ƿ��Ӷ������ʾ
bool GroupMap::ShowType(int npc_type, bool flag)
{
	//m_mapShows.Set(npc_type, flag);
	//m_mapShows[npc_type] = flag;
	if( m_mapShows.Find(npc_type)==m_mapShows.End() )
	{
		m_mapShows.Add(npc_type, flag);
	}
	
	return true;
}

// ���õ�ͼ�ؼ�
void GroupMap::SetMapControl(const PERSISTID& control)
{
	m_Control = control;
}

PERSISTID GroupMap::GetMapControl() const
{
	return m_Control;
}

// ��ʼ��������С��Ϣ
bool GroupMap::InitTerrain(float start_x, float start_z, float width, 
	float height)
{
	// ������Ϣ
	m_fTerrainStartX = start_x;
	m_fTerrainStartZ = start_z;
	m_fTerrainWidth = width;
	m_fTerrainHeight = height;
	return true;
}

int GroupMap::OnSetCursor(int x, int y)
{
	return IControl::OnSetCursor(x, y);
}

bool GroupMap::TestTransparent(int x, int y)
{
//     if (GetTransparent())
//     {
//         // ͸���Ļ���ȫ͸�����ÿ����κ�Ԫ��
//         return true;
//     }

    // ��ȫ͸������ж��Ƿ�Ҫ����͸������
	if (!GetTestTrans())
	{
		// ������͸�����ԣ�����ȫ��͸��
		return false;
	}
	
    // ����͸������
	if (!HasBackImage())
	{
		return true;
	}

	int x1 = GetAbsLeft();
	int y1 = GetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();
	
    // ��ȡ����ڿؼ����ϵ�δ������ƫ��
	int offset_x = x - x1;
	int offset_y = y - y1;

	int alpha;

	if (!InnerGetBackImage().GetAlpha(GetDrawMode(), 0, 0, 
		GetWidth(), GetHeight(), offset_x, offset_y, alpha))
	{
		return true;
	}
	
	return (alpha == 0);
}

int GroupMap::OnLButtonDown(int x, int y)
{ 
	if (!GetEnabled())
	{
		return 0;
	}
	
	int res = 1;

	if (TestTransparent(x, y))
	{
		res = 0;
	}
	
	// ��������������Ϣ
	if (NULL != GetCore()->GetEntity(m_MouseInObj))
	{
		GuiUtil_RunCallback(this, "on_click_obj", CVarList() << m_MouseInObj);

		res = 1;
	}

	return res;
}

// ����ƶ�
int GroupMap::OnMouseMove(int x, int y)
{
	return 1;
}

int GroupMap::MouseMove(int x, int y)
{
	int res = 0;
	bool bmousein = false;
	PERSISTID old_obj = m_MouseInObj;

	m_MouseInObj = PERSISTID();

	std::vector<BindElement>::iterator it = m_vElements.begin();

	point_t pt = {x - GetAbsLeft(), y - GetAbsTop()};

	while (it != m_vElements.end())
	{
		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(it->visual_obj);

		if (NULL == pVisBase)
		{
			// �����������ʧ���Ƴ���
			it = m_vElements.erase(it);
		}
		else
		{
			// ������ڣ����ж��Ƿ��ڶ�����
			if (PtInRect(&it->rect, pt))
			{
				m_MouseInObj = it->visual_obj;
				if (old_obj != it->visual_obj)
				{
					bmousein = true;
				}

				res = 1;
				break;
			}

			it++;
		}
	}

	if (old_obj != PERSISTID() && old_obj != m_MouseInObj)
	{
		GuiUtil_RunCallback(this, "on_mouse_out_obj", 
			CVarList() << old_obj << x << y);
	}

	if (bmousein)
	{
		GuiUtil_RunCallback(this, "on_mouse_in_obj", 
			CVarList() << m_MouseInObj << x << y);
	}

	return res;
}

IControl* GroupMap::OnGetInWhat(int x, int y)
{
	if (GetDesignMode())
	{
		// ���״̬
		return this;
	}
	
	if (MouseMove(x, y) > 0)
	{
		return this;
	}

	return NULL;
}

