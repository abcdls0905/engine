//--------------------------------------------------------------------
// 文件名:		particle.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "particle.h"
#include "particle_node.h"
#include "../visual/i_context.h"
#include "../visual/vis_utils.h"

/// \file particle.cpp
/// \brief 粒子

/// entity: Particle
/// \brief 粒子实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Particle, 0, IVisBase);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, Particle, Visible, GetVisible, SetVisible);
// property: bool TraceEanble
/// \brief 是否检测碰撞
DECLARE_PROPERTY(bool, Particle, TraceEnable, GetTraceEnable, SetTraceEnable);
/// readonly: string Name
/// \brief 粒子名字
DECLARE_PROPERTY_GET(const char*, Particle, Name, GetName);
/// readonly: string Config
/// \brief 粒子配置文件
DECLARE_PROPERTY_GET(const char*, Particle, Config, GetConfig);
/// readonly: object Info
/// \brief 粒子参数对象
DECLARE_PROPERTY_GET(PERSISTID, Particle, Info, GetInfoID);
/// readonly: object Bind
/// \brief 当前绑定的对象
DECLARE_PROPERTY_GET(PERSISTID, Particle, Bind, GetBindID);
/// readonly: bool WaterWave
/// \brief 是否水面波纹
DECLARE_PROPERTY_GET(bool, Particle, WaterWave, GetWaterWave);

/// property: float Dealy
/// \brief 发射延时
DECLARE_PROPERTY(float, Particle, Delay, GetDelay, SetDelay);
/// property: bool EmitSwitch
/// \brief 粒子发射开关
DECLARE_PROPERTY(bool, Particle, EmitSwitch, GetEmitSwitch, SetEmitSwitch);
/// property: float LifeTime
/// \brief 生存时间
DECLARE_PROPERTY(float, Particle, LifeTime, GetLifeTime, SetLifeTime);
/// property: float SpeedFactor
/// \brief 播放速度调节因子（正常速度为1.0）
DECLARE_PROPERTY(float, Particle, SpeedFactor, GetSpeedFactor, 
	SetSpeedFactor);
/// property: float LodEnd
/// \brief LOD结束距离
DECLARE_PROPERTY(float, Particle, LodEnd, GetLodEnd, SetLodEnd);
/// property: bool Repeat
/// \brief 是否重复发射
DECLARE_PROPERTY(bool, Particle, Repeat, GetRepeat, SetRepeat);
/// property: bool UpdateBoundBox
/// \brief 是否动态更新包围盒
DECLARE_PROPERTY(bool, Particle, UpdateBoundBox, GetUpdateBoundBox, 
	SetUpdateBoundBox);
/// property: bool ShowBoundBox
/// \brief 是否显示包围盒
DECLARE_PROPERTY(bool, Particle, ShowBoundBox, GetShowBoundBox, 
	SetShowBoundBox);
/// property: bool Alpha
/// \brief 粒子整体透明度
DECLARE_PROPERTY(unsigned int, Particle, Alpha, GetAlpha, SetAlpha);
/// property: string Color
/// \brief 粒子附加颜色
DECLARE_PROPERTY(result_string, Particle, Color, GetColorString, SetColorString);
/// property: bool ClipPlaneEnable
/// \brief 粒子裁剪面开关
DECLARE_PROPERTY(bool, Particle, ClipPlaneEnable, GetClipPlaneEnable, 
	SetClipPlaneEnable);


/*
/// property: bool SetClipPlane(float x, float y, float z, float w)
/// \brief 设置粒子裁剪面
DECLARE_METHOD_4(bool, Particle, SetClipPlane, float, float, float, float);
/// method: bool Stop()
/// \brief 停止
DECLARE_METHOD_0(bool, Particle, Stop);
/// method: bool Pause()
/// \brief 暂停
DECLARE_METHOD_0(bool, Particle, Pause);
/// method: bool Resume()
/// \brief 恢复
DECLARE_METHOD_0(bool, Particle, Resume);

/// method: bool LinkToObject(object obj, float pos_x, float pos_y, float pos_z)
/// \brief 连接到其他对象
/// \param obj 连接对象
/// \param pos_x 连接位置X
/// \param pos_y 连接位置Y
/// \param pos_z 连接位置Z
DECLARE_METHOD_4(bool, Particle, LinkToObject, const PERSISTID&, float, 
	float, float);

/// method: bool LinkToObjectPoint(object obj, string helper_name)
/// \brief 连接粒子对象到一个高级动画模型的指定辅助点上
/// \param obj 高级动画模型
/// \param helper_name 高级动画模型的辅助点名
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
		// 设置碰撞到的对象ID
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
		// 另外记录粒子的名字，因为粒子节点可能在某些需要获得名字的情况下失效
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

