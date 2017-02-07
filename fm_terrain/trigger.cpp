//--------------------------------------------------------------------
// �ļ���:		trigger.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "trigger.h"
#include "terrain.h"
#include "../public/var_list.h"
#include "../visual/vis_utils.h"

/// \file trigger.cpp
/// \brief ������

/// entity: Trigger
/// \brief ������ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Trigger, 0, IVisBase);

/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, Trigger, Visible, GetVisible, SetVisible);
// property: bool TraceEanble
/// \brief �Ƿ�����ײ
DECLARE_PROPERTY(bool, Trigger, TraceEnable, GetTraceEnable, SetTraceEnable);
/// property: float SizeX
/// \brief �߿�ߴ�X
DECLARE_PROPERTY(float, Trigger, SizeX, GetSizeX, SetSizeX);
/// property: float SizeY
/// \brief �߿�ߴ�Y
DECLARE_PROPERTY(float, Trigger, SizeY, GetSizeY, SetSizeY);
/// property: float SizeZ
/// \brief �߿�ߴ�Z
DECLARE_PROPERTY(float, Trigger, SizeZ, GetSizeZ, SetSizeZ);
/// property: bool ShowBound
/// \brief �Ƿ���ʾ�߿�
DECLARE_PROPERTY(bool, Trigger, ShowBound, GetShowBound, SetShowBound);
/// property: bool ShowBoundBox
/// \brief �Ƿ���ʾ��Χ��
DECLARE_PROPERTY(bool, Trigger, ShowBoundBox, GetShowBoundBox, 
	SetShowBoundBox);
/// property: object Terrain
/// \brief �����ĵ��ζ���
DECLARE_PROPERTY(PERSISTID, Trigger, Terrain, GetTerrainID, SetTerrainID);
/// property: string ScriptName
/// \brief �󶨵Ľű���
DECLARE_PROPERTY(const char*, Trigger, ScriptName, GetScriptName, 
	SetScriptName);
/// property: string Parameter
/// \brief �¼�����
DECLARE_PROPERTY(const char*, Trigger, Parameter, GetParameter, SetParameter);
/// property: float Period
/// \brief ��ʱʱ��
DECLARE_PROPERTY(float, Trigger, Period, GetPeriod, SetPeriod);
/// property: object Bind
/// \brief ��ǰ�󶨵Ķ���
DECLARE_PROPERTY(PERSISTID, Trigger, Bind, GetBindID, SetBindID);

/// readonly: float BindPositionX
/// \brief ��ǰ�󶨵����λ������X
DECLARE_PROPERTY_GET(float, Trigger, BindPositionX, GetBindPositionX);
/// readonly: float BindPositionY
/// \brief ��ǰ�󶨵����λ������Y
DECLARE_PROPERTY_GET(float, Trigger, BindPositionY, GetBindPositionY);
/// readonly: float BindPositionZ
/// \brief ��ǰ�󶨵����λ������Z
DECLARE_PROPERTY_GET(float, Trigger, BindPositionZ, GetBindPositionZ);

/*
/// method: bool CheckInit()
/// \brief ��ʼ��
DECLARE_METHOD_0(bool, Trigger, CheckInit);

/// method: bool SetBindPosition(float x, float y, float z)
/// \brief ���ð�λ��
/// \param x,y,z ��λ��
DECLARE_METHOD_3(bool, Trigger, SetBindPosition, float, float, float);

/// event: int on_init(object self, string para)
/// \brief ��ʼ��
/// \param self ����������
/// \param para ����
DECLARE_EVENT(on_init);
/// event: int on_timer(object self, float seconds, string para)
/// \brief ��ʱ����
/// \param self ����������
/// \param seconds ���ʱ��
/// \param para ����
DECLARE_EVENT(on_timer);
/// event: int on_entry(object self, object player, string para)
/// \brief ���봥������Χ
/// \param self ����������
/// \param player �������
/// \param para ����
DECLARE_EVENT(on_entry);
/// event: int on_leave(object self, object player, string para)
/// \brief �뿪��������Χ
/// \param self ����������
/// \param player �������
/// \param para ����
DECLARE_EVENT(on_leave);
*/
// Trigger

Trigger::Trigger()
{
	m_pTerrain = NULL;
	m_vBoxSize = FmVec3(2.0F, 2.0F, 2.0F);
	m_vBindPos = FmVec3(0.0F, 0.0F, 0.0F);
	m_bVisible = true;
	m_bTraceEnable = true;
	m_bShowBound = true;
	m_bShowBoundBox = false;
	m_fPeriod = 0.0F;
	m_fCounter = 0.0F;
	FmMat4Identity(&m_mtxWorld);
}

Trigger::~Trigger()
{
}

bool Trigger::Init(const IVarList& args)
{
//	if (!IVisual::Init(args))
//	{
//		return false;
//	}

	return true;
}

bool Trigger::Shut()
{
	if (!m_EntryPlayerID.IsNull())
	{
//		VisUtil_RunCallback(this, "on_leave", 
//			CVarList() << PERSISTID() << m_strParameter.c_str());

		m_EntryPlayerID = PERSISTID();
	}

//	return IVisual::Shut();
	return true;
}

void Trigger::Update(float seconds)
{
	UpdateBind();
	CheckTimer(seconds);

	if (m_pTerrain)
	{
		IVisBase* pPlayer = (IVisBase*)GetCore()->GetEntity(
			m_pTerrain->GetPlayerID());

		if (pPlayer)
		{
			CheckPlayer(pPlayer);
		}
		else if (!m_EntryPlayerID.IsNull())
		{
//			VisUtil_RunCallback(this, "on_leave", 
//				CVarList() << PERSISTID() << m_strParameter.c_str());

			m_EntryPlayerID = PERSISTID();
		}
	}
}

void Trigger::Realize(float offset_seconds)
{
	if (m_bShowBoundBox)
	{

		FmVec3 center(0.0F, m_vBoxSize.y * 0.5F, 0.0F);
		
//		VisUtil_DrawBoundBox(m_pRender, m_mtxWorld, center, m_vBoxSize, 
//			0xFFFFFFFF);
	}
	else if (m_bShowBound)
	{
		unsigned int line_color = 0xFF0000FF;

//		if (this->GetScript() == NULL)
//		{
//			line_color = 0xFFFF0000;
//		}
		
		FmVec3 center(0.0F, m_vBoxSize.y * 0.5F, 0.0F);

//		VisUtil_DrawBoundBox(m_pRender, m_mtxWorld, center, m_vBoxSize, 
//			line_color);
		m_pRender->DrawBoundBox( m_mtxWorld, center, m_vBoxSize,line_color);
	}
}

bool Trigger::SetPosition(float x, float y, float z)
{
	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;

	return true;
}

FmVec3 Trigger::GetPosition() const
{
	return FmVec3(m_mtxWorld._41, m_mtxWorld._42, 
		m_mtxWorld._43);
}

bool Trigger::SetAngle(float x, float y, float z)
{
	VisUtil_SetMatrixAngle(&m_mtxWorld, x, y, z);
	
	return true;
}

FmVec3 Trigger::GetAngle() const
{
	FmVec3 angle;

	VisUtil_GetMatrixAngle(&angle, &m_mtxWorld);

	return angle;
}

bool Trigger::SetScale(float x, float y, float z)
{
	VisUtil_SetMatrixScale(&m_mtxWorld, x, y, z);

	return true;
}

FmVec3 Trigger::GetScale() const
{
	FmVec3 scale;

	VisUtil_GetMatrixScale(&scale, &m_mtxWorld);

	return scale;
}

FmVec3 Trigger::GetCenter()
{
	FmVec3 center(0.0F, m_vBoxSize.y * 0.5F, 0.0F);

	FmVec3TransformCoord(&center, &center, &m_mtxWorld);

	return center;
}

FmVec3 Trigger::GetBoxSize()
{
	FmVec3 scale = GetScale();
	FmVec3 box_size = m_vBoxSize;

	box_size.x *= scale.x;
	box_size.y *= scale.y;
	box_size.z *= scale.z;
	
	return box_size;
}

float Trigger::GetRadius()
{
	FmVec3 box_size = GetBoxSize();

	return VisUtil_Vec3Length(&box_size) * 0.5F;
}

bool Trigger::TraceDetail(const FmVec4& src, const FmVec4& dst, 
	trace_info_t& result)
{
	if (!m_bVisible)
	{
		return false;
	}

	if (!m_bTraceEnable)
	{
		return false;
	}

	FmVec3 position = GetPosition();
	FmVec3 angle = GetAngle();
	FmMat4 mat;

	VisUtil_MatrixRotationTranslation(&mat, &angle, &position);
	
	float scale = this->GetRadius();

	if (scale < 0.1F)
	{
		scale = 0.1F;
	}

	if (scale > 1.0F)
	{
		scale = 1.0F;
	}

	int old_hit_count = result.nHitCount;
	
	bool succeed = VisUtil_TraceBoundBox(mat, 
		FmVec3(0.0F, scale * 0.5F, 0.0F), 
		FmVec3(scale, scale, scale), src, dst, result);

	if (succeed)
	{
		// ������ײ���Ķ���ID
		if (result.nHitMax > 1)
		{
			for (int t = old_hit_count; t < result.nHitCount; ++t)
			{
				result.Hits[t].TraceID = this->GetID();
			}
		}
		else
		{
			result.Hits[0].TraceID = this->GetID();
		}
	}

	return succeed;
}

void Trigger::SetVisible(bool value)
{ 
	m_bVisible = value;
}

bool Trigger::GetVisible() const
{ 
	return m_bVisible;
}

void Trigger::SetSizeX(float value)
{
	if (value < 0.0F)
	{
		return;
	}

	m_vBoxSize.x = value;
}

float Trigger::GetSizeX() const
{
	return m_vBoxSize.x;
}

void Trigger::SetSizeY(float value)
{
	if (value < 0.0F)
	{
		return;
	}

	m_vBoxSize.y = value;
}

float Trigger::GetSizeY() const 
{
	return m_vBoxSize.y;
}

void Trigger::SetSizeZ(float value)
{
	if (value < 0.0F)
	{
		return;
	}

	m_vBoxSize.z = value;
}

float Trigger::GetSizeZ() const
{
	return m_vBoxSize.z;
}

void Trigger::SetTerrainID(const PERSISTID& value)
{
	Terrain* pTerrain = (Terrain*)GetCore()->GetEntity(value);
	
	if (NULL == pTerrain)
	{
		return;
	}


	m_pTerrain = pTerrain;
}

PERSISTID Trigger::GetTerrainID() const
{
	if (NULL == m_pTerrain)
	{
		return PERSISTID();
	}

	return m_pTerrain->GetID();
}

void Trigger::SetScriptName(const char* value)
{
	Assert(value != NULL);

	/*
	if (!GetCore()->BindScript(this, value))
	{
		return;
	}
	*/

	m_strScriptName = value;
}

const char* Trigger::GetScriptName() const
{
	return m_strScriptName.c_str();
}

void Trigger::SetParameter(const char* value)
{
	Assert(value != NULL);

	m_strParameter = value;
}

const char* Trigger::GetParameter() const
{
	return m_strParameter.c_str();
}

void Trigger::SetPeriod(float value)
{
	if (value < 0.0F)
	{
		return;
	}

	m_fPeriod = value;
	m_fCounter = 0.0F;
}

float Trigger::GetPeriod() const
{
	return m_fPeriod;
}

bool Trigger::CheckInit()
{
	/*
	if (this->GetScript() == NULL)
	{
		return false;
	}

	this->GetScript()->AddCallback("on_timer", "", "on_timer");
	this->GetScript()->AddCallback("on_entry", "", "on_entry");
	this->GetScript()->AddCallback("on_leave", "", "on_leave");
	this->GetScript()->AddCallback("on_init", "", "on_init");

	VisUtil_RunCallback(this, "on_init", 
		CVarList() << m_strParameter.c_str());
    */
	
	return true;
}

bool Trigger::CheckTimer(float seconds)
{
//	if (this->GetScript() == NULL)
//	{
//		return false;
//	}

	if (m_fPeriod <= 0.0F)
	{
		return false;
	}

	m_fCounter += seconds;

	if (m_fCounter < m_fPeriod)
	{
		return false;
	}

//	VisUtil_RunCallback(this, "on_timer", 
//		CVarList() << m_fCounter << m_strParameter.c_str());

	m_fCounter = 0.0F;

	return true;
}

bool Trigger::CheckPlayer(IVisBase* pPlayer)
{
//	if (this->GetScript() == NULL)
//	{
//		return false;
//	}

	FmVec3 player_pos = pPlayer->GetPosition();
	
	player_pos.y += 0.9F;

	FmVec3 center = GetCenter();
	FmVec3 box_size = GetBoxSize();
	FmVec3 vec3Temp = center - player_pos;
	float distance_sq = FmVec3LengthSq(&vec3Temp);

	if (distance_sq > FmVec3LengthSq(&box_size))
	{
		return PlayerLeave(pPlayer);
	}

	FmMat4 mtxInverse;

	FmMat4Inverse(&mtxInverse, NULL, &m_mtxWorld);
	FmVec3TransformCoord(&player_pos, &player_pos, &mtxInverse);

	// ��ҵ�ǰ�Ƿ��ڴ������ķ�Χ��
	bool player_in_box = (player_pos.x >= -m_vBoxSize.x * 0.5F)
		&& (player_pos.x <= m_vBoxSize.x * 0.5F)
		&& (player_pos.z >= -m_vBoxSize.z * 0.5F)
		&& (player_pos.z <= m_vBoxSize.z * 0.5F)
		&& (player_pos.y >= 0.0F) && (player_pos.y <= m_vBoxSize.y);

	if (!player_in_box)
	{
		return PlayerLeave(pPlayer);
	}

	return PlayerEntry(pPlayer);
}

bool Trigger::PlayerEntry(IVisBase* pPlayer)
{
	if (pPlayer->GetID() == m_EntryPlayerID)
	{
		return false;
	}

//	VisUtil_RunCallback(this, "on_entry", 
//		CVarList() << pPlayer->GetID() << m_strParameter.c_str());

	m_EntryPlayerID = pPlayer->GetID();

	return true;
}

bool Trigger::PlayerLeave(IVisBase* pPlayer)
{
	if (pPlayer->GetID() != m_EntryPlayerID)
	{
		return false;
	}

//	VisUtil_RunCallback(this, "on_leave", 
//		CVarList() << pPlayer->GetID() << m_strParameter.c_str());

	m_EntryPlayerID = PERSISTID();

	return true;
}

bool Trigger::UpdateBind() 
{ 
	if (m_BindID.IsNull())
	{
		return false;
	}
	
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(m_BindID);
	
	if (NULL == pVisBase)
	{
		return false;
	}

	FmVec3 pos = pVisBase->GetPosition() + m_vBindPos;

	SetPosition(pos.x, pos.y, pos.z);

	return true;
}

void Trigger::SetBindID(const PERSISTID& id)
{
	if (id.IsNull())
	{
		m_BindID = PERSISTID();
		return;
	}
	
	IEntity* pEntity = (IEntity*)GetCore()->GetEntity(id);
	
	if (NULL == pEntity)
	{
		return;
	}
	
	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return;
	}
	
	m_BindID = id;
	
	UpdateBind();
}

bool Trigger::SetBindPosition(float x, float y, float z)
{
	m_vBindPos.x = x;
	m_vBindPos.y = y;
	m_vBindPos.z = z;
	
	UpdateBind();

	return true;
}

