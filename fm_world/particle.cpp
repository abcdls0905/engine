//--------------------------------------------------------------------
// �ļ���:		particle.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "particle.h"
#include "particle_node.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"

/// \file particle.cpp
/// \brief ����

/// entity: Particle
/// \brief ����ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Particle, 0, IVisBase);

/// property: bool Visible
/// \brief �Ƿ�ɼ�
DECLARE_PROPERTY(bool, Particle, Visible, GetVisible, SetVisible);
// property: bool TraceEanble
/// \brief �Ƿ�����ײ
DECLARE_PROPERTY(bool, Particle, TraceEnable, GetTraceEnable, SetTraceEnable);
/// readonly: string Name
/// \brief ��������
DECLARE_PROPERTY_GET(const char*, Particle, Name, GetName);
/// readonly: string Config
/// \brief ���������ļ�
DECLARE_PROPERTY_GET(const char*, Particle, Config, GetConfig);
/// readonly: object Info
/// \brief ���Ӳ�������
DECLARE_PROPERTY_GET(PERSISTID, Particle, Info, GetInfoID);
/// readonly: object Bind
/// \brief ��ǰ�󶨵Ķ���
DECLARE_PROPERTY_GET(PERSISTID, Particle, Bind, GetBindID);
/// readonly: bool WaterWave
/// \brief �Ƿ�ˮ�沨��
DECLARE_PROPERTY_GET(bool, Particle, WaterWave, GetWaterWave);

/// property: float Dealy
/// \brief ������ʱ
DECLARE_PROPERTY(float, Particle, Delay, GetDelay, SetDelay);
/// property: bool EmitSwitch
/// \brief ���ӷ��俪��
DECLARE_PROPERTY(bool, Particle, EmitSwitch, GetEmitSwitch, SetEmitSwitch);
/// property: float LifeTime
/// \brief ����ʱ��
DECLARE_PROPERTY(float, Particle, LifeTime, GetLifeTime, SetLifeTime);
/// property: float SpeedFactor
/// \brief �����ٶȵ������ӣ������ٶ�Ϊ1.0��
DECLARE_PROPERTY(float, Particle, SpeedFactor, GetSpeedFactor, 
	SetSpeedFactor);
/// property: float LodEnd
/// \brief LOD��������
DECLARE_PROPERTY(float, Particle, LodEnd, GetLodEnd, SetLodEnd);
/// property: bool Repeat
/// \brief �Ƿ��ظ�����
DECLARE_PROPERTY(bool, Particle, Repeat, GetRepeat, SetRepeat);
/// property: bool UpdateBoundBox
/// \brief �Ƿ�̬���°�Χ��
DECLARE_PROPERTY(bool, Particle, UpdateBoundBox, GetUpdateBoundBox, 
	SetUpdateBoundBox);
/// property: bool ShowBoundBox
/// \brief �Ƿ���ʾ��Χ��
DECLARE_PROPERTY(bool, Particle, ShowBoundBox, GetShowBoundBox, 
	SetShowBoundBox);
/// property: bool Alpha
/// \brief ��������͸����
DECLARE_PROPERTY(unsigned int, Particle, Alpha, GetAlpha, SetAlpha);
/// property: string Color
/// \brief ���Ӹ�����ɫ
DECLARE_PROPERTY(result_string, Particle, Color, GetColorString, SetColorString);
/// property: bool ClipPlaneEnable
/// \brief ���Ӳü��濪��
DECLARE_PROPERTY(bool, Particle, ClipPlaneEnable, GetClipPlaneEnable, 
	SetClipPlaneEnable);


/*
/// property: bool SetClipPlane(float x, float y, float z, float w)
/// \brief �������Ӳü���
DECLARE_METHOD_4(bool, Particle, SetClipPlane, float, float, float, float);
/// method: bool Stop()
/// \brief ֹͣ
DECLARE_METHOD_0(bool, Particle, Stop);
/// method: bool Pause()
/// \brief ��ͣ
DECLARE_METHOD_0(bool, Particle, Pause);
/// method: bool Resume()
/// \brief �ָ�
DECLARE_METHOD_0(bool, Particle, Resume);

/// method: bool LinkToObject(object obj, float pos_x, float pos_y, float pos_z)
/// \brief ���ӵ���������
/// \param obj ���Ӷ���
/// \param pos_x ����λ��X
/// \param pos_y ����λ��Y
/// \param pos_z ����λ��Z
DECLARE_METHOD_4(bool, Particle, LinkToObject, const PERSISTID&, float, 
	float, float);

/// method: bool LinkToObjectPoint(object obj, string helper_name)
/// \brief �������Ӷ���һ���߼�����ģ�͵�ָ����������
/// \param obj �߼�����ģ��
/// \param helper_name �߼�����ģ�͵ĸ�������
DECLARE_METHOD_2(bool, Particle, LinkToObjectPoint, const PERSISTID&, 
	const char*);
*/

// Particle

Particle::Particle()
{
	m_pNode = NULL;
	m_bShowBoundBox = false;
	m_bTraceEnable = true;
	m_fAlphaValue = 1.0F;
}

Particle::~Particle()
{
}

bool Particle::Init(const IVarList& args)
{
 
	return true;
}

bool Particle::Shut()
{
	return true;
}

void Particle::Realize()
{
	if (!m_pNode->GetVisible())
	{
		return;
	}

	m_pNode->SetShowState(true);

  m_bShowBoundBox = true;
	if (m_bShowBoundBox)
	{
		FmMat4 mat;

		FmMat4Identity(&mat);

		IRender* pRender = (IRender*)GetCore()->GetInterface("Render");
		if(pRender)
		{
			pRender->DrawBoundBox(mat, GetCenter(), GetBoxSize(), (const float&)0xFFFFFFFF);
		}
	}
}

bool Particle::GetLoadFinish()
{
	return m_pNode->GetLoadFinish();
}

void Particle::SetVisible(bool value)
{ 
	m_pNode->SetVisible(value);
}

bool Particle::GetVisible() const
{ 
	return m_pNode->GetVisible();
}

void Particle::SetUnderWater(bool value)
{
	m_pNode->SetUnderWater(value);
}

bool Particle::GetUnderWater() const
{
	return m_pNode->GetUnderWater();
}

bool Particle::SetPosition(float x, float y, float z) 
{ 
	m_pNode->SetEmitterPos(FmVec3(x, y, z));
	
	return true; 
}

FmVec3 Particle::GetPosition() const 
{ 
	return m_pNode->GetEmitterPos();
}

bool Particle::SetAngle(float x, float y, float z) 
{ 
	m_pNode->SetEmitterAngle(FmVec3(x, y, z));
	
	return true; 
}

FmVec3 Particle::GetAngle() const 
{ 
	return m_pNode->GetEmitterAngle();
}

bool Particle::SetScale(float x, float y, float z)
{
	m_pNode->SetScale(FmVec3(x, y, z));

	return true;
}

FmVec3 Particle::GetScale() const
{
	return m_pNode->GetScale();
}

FmVec3 Particle::GetCenter()
{
	return m_pNode->GetEmitterPos() + m_pNode->GetCenter();
}

FmVec3 Particle::GetBoxSize()
{
	return m_pNode->GetBoxSize();
}

float Particle::GetRadius()
{
	return m_pNode->GetRadius();
}

bool Particle::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	if (!m_pNode->GetVisible())
	{
		return false;
	}
	
	if (!m_bTraceEnable)
	{
		return false;
	}

	FmVec3 position = GetPosition();
	FmMat4 mat;

	FmMat4Translation(&mat, position.x, position.y, position.z);

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

void Particle::SetParticleNode(CParticleNode* value)
{ 
	m_pNode = value; 

	if (m_pNode)
	{
		// �����¼���ӵ����֣���Ϊ���ӽڵ������ĳЩ��Ҫ������ֵ������ʧЧ
		m_strName = m_pNode->GetName();
	}
}

const char* Particle::GetName() const
{
	return m_strName.c_str();
}

const char* Particle::GetConfig() const
{
	return m_pNode->GetConfig();
}

PERSISTID Particle::GetBindID() const
{
	return m_pNode->GetBindID();
}

bool Particle::GetWaterWave() const
{
	if (NULL == m_pNode)
	{
		return false;
	}

	return m_pNode->GetWaveParticle();
}

void Particle::SetDelay(float value)
{
	m_pNode->SetSystemDelay(value);
}

float Particle::GetDelay() const
{
	return m_pNode->GetSystemDelay();
}

void Particle::SetEmitSwitch(bool value)
{
	m_pNode->SetEmitSwitch(value);
}

bool Particle::GetEmitSwitch() const
{
	return m_pNode->GetEmitSwitch();
}

void Particle::SetLifeTime(float value)
{
	m_pNode->SetSystemLifeTime(value);
}

float Particle::GetLifeTime() const
{
	return m_pNode->GetSystemLifeTime();
}

void Particle::SetSpeedFactor(float value)
{
	m_pNode->SetSpeedFactor(value);
}

float Particle::GetSpeedFactor() const
{
	return m_pNode->GetSpeedFactor();
}

void Particle::SetLodEnd(float value)
{
	m_pNode->SetLodEnd(value);
}

float Particle::GetLodEnd() const
{
	return m_pNode->GetLodEnd();
}

void Particle::SetRepeat(bool value)
{
	m_pNode->SetRepeat(value);
}

bool Particle::GetRepeat() const
{
	return m_pNode->GetRepeat();
}

void Particle::SetUpdateBoundBox(bool value)
{
	m_pNode->SetUpdateBoundBox(value);
}

bool Particle::GetUpdateBoundBox() const
{
	return m_pNode->GetUpdateBoundBox();
}

void Particle::SetAlpha(unsigned int value)
{
	m_pNode->SetAlpha(value);
}

unsigned int Particle::GetAlpha() const
{
	return m_pNode->GetAlpha();
}

void Particle::SetColorString(const char* value)
{
	m_pNode->SetAppendColor(VisUtil_ColorToInt(value));
}

result_string Particle::GetColorString() const
{
	return VisUtil_IntToColor(m_pNode->GetAppendColor());
}

void Particle::SetColor(unsigned int value)
{
	m_pNode->SetAppendColor(value);
}

unsigned int Particle::GetColor() const
{
	return m_pNode->GetAppendColor();
}

void Particle::SetClipPlaneEnable(bool value)
{
	m_pNode->SetClipPlaneEnable(value);
}

bool Particle::GetClipPlaneEnable() const
{
	return m_pNode->GetClipPlaneEnable();
}

bool Particle::SetClipPlane(float x, float y, float z, float w)
{
	FmVec4 value(x, y, z, w);

	m_pNode->SetClipPlane(value);

	return true;
}

bool Particle::Stop()
{
	m_pNode->Stop();
	
	return true;
}

bool Particle::Pause()
{
	m_pNode->Pause();
	
	return true;
}

bool Particle::Resume()
{
	m_pNode->Resume();
	
	return true;
}

bool Particle::LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
	float pos_z)
{
	IEntity* pEntity = (IEntity*)GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return false;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}
	
	return m_pNode->LinkToObject(id, FmVec3(pos_x, pos_y, pos_z));
}

bool Particle::LinkToObjectPoint(const PERSISTID& id, const char* helper_name)
{
	IEntity* pEntity = (IEntity*)GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return false;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	return m_pNode->LinkToObjectPoint(id, helper_name);
}

