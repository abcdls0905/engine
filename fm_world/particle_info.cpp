//--------------------------------------------------------------------
// 文件名:		particle_info.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "particle_info.h"
#include "../visual/vis_utils.h"
#include "../public/inlines.h"

/// \file particle_info.cpp
/// \brief 粒子参数

/// entity: ParticleInfo
/// \brief 粒子参数实体
/// \see 继承自IVisBase
DECLARE_ENTITY(ParticleInfo, 0, IEntity);

/// property: string IniName
/// \brief 对应的粒子配置文件名
DECLARE_PROPERTY(const char*, ParticleInfo, IniName, GetIniName, SetIniName);
/// property: string Texture
/// \brief 贴图文件名
DECLARE_PROPERTY(const char*, ParticleInfo, Texture, GetTextureName, 
	SetTextureName);
/// property: string Texture
/// \brief 贴图文件名
DECLARE_PROPERTY(const char*, ParticleInfo, RandomColorTex, GetRandomColorTex, 
	SetRandomColorTex);
/// property: string RefractionTex
/// \brief 折射贴图名
DECLARE_PROPERTY(const char*, ParticleInfo, RefractionTex, GetRefractionTex, 
	SetRefractionTex);
/// property: string Effect
/// \brief 显示效果
DECLARE_PROPERTY(const char*, ParticleInfo, Effect, GetEffectName, 
	SetEffectName);
/// property: string BillboardType
/// \brief 效果文件名
DECLARE_PROPERTY(const char*, ParticleInfo, BillboardType, GetBillboardType, 
	SetBillboardType);
/// property: string CalculateMode
/// \brief 粒子计算模式
DECLARE_PROPERTY(const char*, ParticleInfo, CalculateMode, GetCalculateMode, 
	SetCalculateMode);
/// property: string NextParticle
/// \brief 后续播放粒子
DECLARE_PROPERTY(const char*, ParticleInfo, NextParticle, GetNextParticle, 
	SetNextParticle);
/// property: float DirectionDeviation
/// \brief 方向偏移量
DECLARE_PROPERTY(float, ParticleInfo, DirectionDeviation, 
	GetDirectionDeviation, SetDirectionDeviation);
/// property: float WindEffect
/// \brief 风的影响系数
DECLARE_PROPERTY(float, ParticleInfo, WindEffect, GetWindEffect, 
	SetWindEffect);
/// property: float Gravity
/// \brief 重力影响系数
DECLARE_PROPERTY(float, ParticleInfo, Gravity, GetGravity, SetGravity);
/// property: float InitSize
/// \brief 初始大小
DECLARE_PROPERTY(float, ParticleInfo, InitSize, GetInitSize, SetInitSize);
/// property: float SizeDeviation
/// \brief 大小偏移量
DECLARE_PROPERTY(float, ParticleInfo, SizeDeviation, GetSizeDeviation, 
	SetSizeDeviation);
/// property: float AspectRatio
/// \brief 长宽比例
DECLARE_PROPERTY(float, ParticleInfo, AspectRatio, GetAspectRatio, 
	SetAspectRatio);
/// property: float InitSpeed
/// \brief 初始速度
DECLARE_PROPERTY(float, ParticleInfo, InitSpeed, GetInitSpeed, SetInitSpeed);
/// property: float SpeedDeviation
/// \brief 速度偏移量
DECLARE_PROPERTY(float, ParticleInfo, SpeedDeviation, GetSpeedDeviation, 
	SetSpeedDeviation);
/// property: float Spin
/// \brief 旋转
DECLARE_PROPERTY(float, ParticleInfo, Spin, GetSpin, SetSpin);
/// property: float SpinDeviation
/// \brief 旋转偏移量
DECLARE_PROPERTY(float, ParticleInfo, SpinDeviation, GetSpinDeviation, 
	SetSpinDeviation);
/// property: float Weight
/// \brief 重量
DECLARE_PROPERTY(float, ParticleInfo, Weight, GetWeight, SetWeight);
/// property: float WeightDeviation
/// \brief 重量偏移量
DECLARE_PROPERTY(float, ParticleInfo, WeightDeviation, GetWeightDeviation, 
	SetWeightDeviation);

/// property: float EmitRadiusX
/// \brief 发射半径X
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusX, GetEmitRadiusX, 
	SetEmitRadiusX);
/// property: float EmitRadiusY
/// \brief 发射半径Y
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusY, GetEmitRadiusY, 
	SetEmitRadiusY);
/// property: float EmitRadiusZ
/// \brief 发射半径Z
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusZ, GetEmitRadiusZ, 
	SetEmitRadiusZ);
/// property: float EmitRadiusXRand
/// \brief 发射半径X随机范围
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusXRand, GetEmitRadiusXRand, 
	SetEmitRadiusXRand);
/// property: float EmitRadiusYRand
/// \brief 发射半径Y随机范围
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusYRand, GetEmitRadiusYRand, 
	SetEmitRadiusYRand);
/// property: float EmitRadiusZRand
/// \brief 发射半径Z随机范围
DECLARE_PROPERTY(float, ParticleInfo, EmitRadiusZRand, GetEmitRadiusZRand, 
	SetEmitRadiusZRand);

/// property: float TrackPointRadius
/// \brief 回收半径
DECLARE_PROPERTY(float, ParticleInfo, TrackPointRadius, GetTrackPointRadius, 
	SetTrackPointRadius);
/// property: float SurfaceOffset
/// \brief 到绑定平面的距离
DECLARE_PROPERTY(float, ParticleInfo, SurfaceOffset, GetSurfaceOffset, 
	SetSurfaceOffset);
/// property: float EmissionTime
/// \brief 发射时间间隔
DECLARE_PROPERTY(float, ParticleInfo, EmissionTime, GetEmissionTime, 
	SetEmissionTime);
/// property: float EmissionTimeRand
/// \brief 发射时间间隔随机值
DECLARE_PROPERTY(float, ParticleInfo, EmissionTimeRand, GetEmissionTimeRand, 
	SetEmissionTimeRand);
/// property: float ChaosValue
/// \brief 粒子位置抖动值
DECLARE_PROPERTY(float, ParticleInfo, ChaosValue, GetChaosValue, 
	SetChaosValue);
/// property: float SpeedChaos
/// \brief 粒子速度抖动值
DECLARE_PROPERTY(float, ParticleInfo, SpeedChaos, GetSpeedChaos, 
	SetSpeedChaos);
/// property: int TextureFrame
/// \brief 贴图分块数量
DECLARE_PROPERTY(int, ParticleInfo, TextureFrame, GetTextureFrame, 
	SetTextureFrame);
/// property: int ParticleNum
/// \brief 粒子数量
DECLARE_PROPERTY(int, ParticleInfo, ParticleNum, GetParticleNum, 
	SetParticleNum);
/// property: int ParticleLife
/// \brief 粒子生存时间（单位毫秒）
DECLARE_PROPERTY(int, ParticleInfo, ParticleLife, GetParticleLife, 
	SetParticleLife);
/// property: int TailLife
/// \brief 粒子拖尾生存时间（单位毫秒）
DECLARE_PROPERTY(int, ParticleInfo, TailLife, GetTailLife, SetTailLife);
/// property: int OneEmitNumber
/// \brief 一次发射的粒子数量
DECLARE_PROPERTY(int, ParticleInfo, OneEmitNumber, GetOneEmitNumber, 
	SetOneEmitNumber);
/// property: int ParticleFrame
/// \brief 粒子贴图分块数量
DECLARE_PROPERTY(int, ParticleInfo, ParticleFrame, GetParticleFrame, 
	SetParticleFrame);
/// property: int ParticleColor
/// \brief 粒子颜色
DECLARE_PROPERTY(result_string, ParticleInfo, ParticleColor, 
	GetParticleColorString, SetParticleColorString);
/// property: float Luminance
/// \brief 粒子颜色
DECLARE_PROPERTY(float, ParticleInfo, Luminance, 
	GetLuminance, SetLuminance);
///// property: bool RandomColor
///// \brief 是否随机颜色
//DECLARE_PROPERTY(bool, ParticleInfo, RandomColor, GetRandomColor, 
//	SetRandomColor);
/// property: bool ColorAni
/// \brief 是否颜色动画
DECLARE_PROPERTY(bool, ParticleInfo, ColorAni, GetColorAni, 
	SetColorAni);
/// property: bool Repeat
/// \brief 是否重复发射
DECLARE_PROPERTY(bool, ParticleInfo, Repeat, GetRepeat, SetRepeat);
/// property: bool TrackPointReturn
/// \brief 回收标志
DECLARE_PROPERTY(bool, ParticleInfo, TrackPointReturn, GetTrackPointReturn, 
	SetTrackPointReturn);
/// property: bool TrackPointDisappear
/// \brief 到达后消失
DECLARE_PROPERTY(bool, ParticleInfo, TrackPointDisappear, 
	GetTrackPointDisappear, SetTrackPointDisappear);
/// property: bool LocalSpaceEmitter
/// \brief 是否保持与绑定物体的相对位置
DECLARE_PROPERTY(bool, ParticleInfo, LocalSpaceEmitter, GetLocalSpaceEmitter, 
	SetLocalSpaceEmitter);
/// property: bool LocalAngleEmitter
/// \brief 是否保持与绑定物体的相对方向
DECLARE_PROPERTY(bool, ParticleInfo, LocalAngleEmitter, GetLocalAngleEmitter, 
	SetLocalAngleEmitter);
/// property: bool AngleLinkSpeed
/// \brief 是否保持与绑定物体的相对位置
DECLARE_PROPERTY(bool, ParticleInfo, AngleLinkSpeed, GetAngleLinkSpeed, 
	SetAngleLinkSpeed);
/// property: bool AngleLinkSpeed
/// \brief 是否保持与绑定物体的相对位置
DECLARE_PROPERTY(bool, ParticleInfo, AngleLinkEmitter, GetAngleLinkEmitter, 
	SetAngleLinkEmitter);
/// property: bool RandomDirection
/// \brief 随机方向
DECLARE_PROPERTY(bool, ParticleInfo, RandomDirection, GetRandomDirection, 
	SetRandomDirection);
/// property: string OneAxisNotRand
/// \brief 限制一个轴的发射方向（"XAxis"-X轴,"YAxis"-Y轴,"ZAxis"-Z轴）
DECLARE_PROPERTY(result_string, ParticleInfo, OneAxisNotRand, 
	GetOneAxisNotRand, SetOneAxisNotRand);
/// property: bool FlashTail
/// \brief 是否生成粒子拖尾
DECLARE_PROPERTY(bool, ParticleInfo, FlashTail, GetFlashTail, SetFlashTail);
/// property: bool FixupTailSize
/// \brief 拖尾尺寸是否固定
DECLARE_PROPERTY(bool, ParticleInfo, FixupTailSize, GetFixupTailSize, 
	SetFixupTailSize);
/// property: bool DistortionIntensity
/// \brief 扭曲强度
DECLARE_PROPERTY(float, ParticleInfo, DistortionIntensity,
	GetDistortionIntensity, SetDistortionIntensity);
/// property: bool DelayUnload
/// \brief 延迟卸载粒子
DECLARE_PROPERTY(bool, ParticleInfo, DelayUnload, GetDelayUnload, 
	SetDelayUnload);
/// property: bool NotDownLevel
/// \brief 不降级显示
DECLARE_PROPERTY(bool, ParticleInfo, NotDownLevel, GetNotDownLevel, 
	SetNotDownLevel);

/*
/// method: bool AddTrack(string type, int time, float value)
/// \brief 添加可见性轨迹信息
/// \param type 轨迹类型
///		"alpha"-可见性，"size"-粒子大小，"speed"-粒子速度，"spin"-粒子旋转，
///		"angle"-粒子角度，"wind"-风影响度，"dir"-粒子方向
///		"emit_radius_x"-发射半径X，"emit_radius_y"-发射半径Y，"emit_radius_z"-发射半径Z
///		"red"-红色，"green"-绿色，"blue"-蓝色
/// \param time 时间
/// \param value 数值
DECLARE_METHOD_3(bool, ParticleInfo, AddTrack, const char*, int, float);
/// method: bool SetTrack(string type, int time, float value)
/// \brief 重设轨迹信息
/// \param type 轨迹类型
/// \param time 时间
/// \param value 数值
DECLARE_METHOD_3(bool, ParticleInfo, SetTrack, const char*, int, float);
/// method: bool SetTrackByIndex(string type, int index, int time, float value)
/// \brief 重设指定索引的轨迹信息
/// \param type 轨迹类型
/// \param index 索引值
/// \param time 时间
/// \param value 数值
DECLARE_METHOD_4(bool, ParticleInfo, SetTrackByIndex, const char*, int, int, 
	float);
/// method: bool ClearTrack(string type)
/// \brief 清空轨迹信息
/// \param type 轨迹类型
DECLARE_METHOD_1(bool, ParticleInfo, ClearTrack, const char*);
/// method: bool GetTrackCount(string type)
/// \brief 获得轨迹数量
/// \param type 轨迹类型
DECLARE_METHOD_1(bool, ParticleInfo, GetTrackCount, const char*);
/// method: bool GetTrackTime(string type, int index)
/// \brief 获得指定轨迹的时间点
/// \param type 轨迹类型
/// \param index 索引值
DECLARE_METHOD_2(int, ParticleInfo, GetTrackTime, const char*, int);
/// method: bool GetTrackValue(string type, int index)
/// \brief 获得指定轨迹的数值
/// \param type 轨迹类型
/// \param index 索引值
DECLARE_METHOD_2(float, ParticleInfo, GetTrackValue, const char*, int);
*/

// ParticleInfo

ParticleInfo::ParticleInfo()
{
	m_pData = CParticleData::NewInstance();
}

ParticleInfo::~ParticleInfo()
{
	SAFE_RELEASE(m_pData);
}

bool ParticleInfo::Init(const IVarList& args)
{
	return true;
}

bool ParticleInfo::Shut()
{
	return true;
}

void ParticleInfo::SetIniName(const char* value)
{
	m_pData->SetIniName(value);
}

const char* ParticleInfo::GetIniName() const
{
	return m_pData->GetIniName();
}

void ParticleInfo::SetTextureName(const char* value) 
{ 
	Assert(value != NULL);
	
	m_pData->m_strTextureName = value;
}

const char* ParticleInfo::GetTextureName() const 
{ 
	return m_pData->m_strTextureName.c_str(); 
}

void ParticleInfo::SetRandomColorTex(const char* value) 
{ 
	Assert(value != NULL);
	
	m_pData->m_strRandomColorTex = value;
}

const char* ParticleInfo::GetRandomColorTex() const 
{ 
	return m_pData->m_strRandomColorTex.c_str(); 
}

void ParticleInfo::SetRefractionTex(const char* value) 
{ 
	Assert(value != NULL);

	m_pData->m_strRefractionTex = value;
}

const char* ParticleInfo::GetRefractionTex() const 
{ 
	return m_pData->m_strRefractionTex.c_str(); 
}

void ParticleInfo::SetEffectName(const char* value) 
{ 
	Assert(value != NULL);
	
	m_pData->m_strEffectName = value;
}

const char* ParticleInfo::GetEffectName() const 
{ 
	return m_pData->m_strEffectName.c_str(); 
}

void ParticleInfo::SetBillboardType(const char* value) 
{ 
	Assert(value != NULL);

	m_pData->m_strBillboardType = value;
}

const char* ParticleInfo::GetBillboardType() const 
{ 
	return m_pData->m_strBillboardType.c_str(); 
}

void ParticleInfo::SetCalculateMode(const char* value)
{
	Assert(value != NULL);

	m_pData->m_strCalculateMode = value;
}

const char* ParticleInfo::GetCalculateMode() const
{
	return m_pData->m_strCalculateMode.c_str();
}

void ParticleInfo::SetNextParticle(const char* value)
{
	Assert(value != NULL);

	m_pData->m_strNextParticle = value;
}

const char* ParticleInfo::GetNextParticle() const
{
	return m_pData->m_strNextParticle.c_str();
}

void ParticleInfo::SetDirectionDeviation(float value)
{ 
	m_pData->m_fDirectionDeviation = value; 
}

float ParticleInfo::GetDirectionDeviation() const 
{ 
	return m_pData->m_fDirectionDeviation; 
}

void ParticleInfo::SetWindEffect(float value) 
{ 
	m_pData->m_fWindEffect = value; 
}

float ParticleInfo::GetWindEffect() const 
{ 
	return m_pData->m_fWindEffect; 
}

void ParticleInfo::SetGravity(float value) 
{ 
	m_pData->m_fGravity = value; 
}

float ParticleInfo::GetGravity() const 
{ 
	return m_pData->m_fGravity; 
}

void ParticleInfo::SetInitSize(float value) 
{
	if (value <= 0.0F)
	{
		return;
	}
	m_pData->m_fInitSize = value; 
}

float ParticleInfo::GetInitSize() const 
{ 
	return m_pData->m_fInitSize; 
}

void ParticleInfo::SetSizeDeviation(float value) 
{ 
	m_pData->m_fSizeDeviation = value; 
}

float ParticleInfo::GetSizeDeviation() const 
{ 
	return m_pData->m_fSizeDeviation; 
}

void ParticleInfo::SetAspectRatio(float value)
{
	if (value <= 0.0F)
	{
		return;
	}

	m_pData->m_fAspectRatio = value;
}

float ParticleInfo::GetAspectRatio() const
{
	return m_pData->m_fAspectRatio;
}

void ParticleInfo::SetInitSpeed(float value) 
{ 
	m_pData->m_fInitSpeed = value; 
}

float ParticleInfo::GetInitSpeed() const 
{ 
	return m_pData->m_fInitSpeed; 
}

void ParticleInfo::SetSpeedDeviation(float value) 
{ 
	m_pData->m_fSpeedDeviation = value; 
}

float ParticleInfo::GetSpeedDeviation() const 
{ 
	return m_pData->m_fSpeedDeviation; 
}

void ParticleInfo::SetSpin(float value) 
{ 
	m_pData->m_fSpin = value; 
}

float ParticleInfo::GetSpin() const 
{ 
	return m_pData->m_fSpin; 
}

void ParticleInfo::SetSpinDeviation(float value) 
{ 
	m_pData->m_fSpinDeviation = value; 
}

float ParticleInfo::GetSpinDeviation() const 
{ 
	return m_pData->m_fSpinDeviation; 
}

void ParticleInfo::SetWeight(float value) 
{ 
	m_pData->m_fWeight = value; 
}

float ParticleInfo::GetWeight() const 
{ 
	return m_pData->m_fWeight; 
}

void ParticleInfo::SetWeightDeviation(float value) 
{ 
	m_pData->m_fWeightDeviation = value; 
}

float ParticleInfo::GetWeightDeviation() const 
{ 
	return m_pData->m_fWeightDeviation; 
}

void ParticleInfo::SetEmitRadiusX(float value)
{
	m_pData->m_fEmitRadiusX = value;
}

float ParticleInfo::GetEmitRadiusX() const
{
	return m_pData->m_fEmitRadiusX;
}

void ParticleInfo::SetEmitRadiusY(float value)
{
	m_pData->m_fEmitRadiusY = value;
}

float ParticleInfo::GetEmitRadiusY() const
{
	return m_pData->m_fEmitRadiusY;
}

void ParticleInfo::SetEmitRadiusZ(float value)
{
	m_pData->m_fEmitRadiusZ = value;
}

float ParticleInfo::GetEmitRadiusZ() const
{
	return m_pData->m_fEmitRadiusZ;
}

void ParticleInfo::SetEmitRadiusXRand(float value)
{
	m_pData->m_fEmitRadiusXRand = value;
}

float ParticleInfo::GetEmitRadiusXRand() const
{
	return m_pData->m_fEmitRadiusXRand;
}

void ParticleInfo::SetEmitRadiusYRand(float value)
{
	m_pData->m_fEmitRadiusYRand = value;
}

float ParticleInfo::GetEmitRadiusYRand() const
{
	return m_pData->m_fEmitRadiusYRand;
}

void ParticleInfo::SetEmitRadiusZRand(float value)
{
	m_pData->m_fEmitRadiusZRand = value;
}

float ParticleInfo::GetEmitRadiusZRand() const
{
	return m_pData->m_fEmitRadiusZRand;
}

void ParticleInfo::SetTrackPointRadius(float value) 
{ 
	m_pData->m_fTrackPointRadius = value; 
}

float ParticleInfo::GetTrackPointRadius() const 
{ 
	return m_pData->m_fTrackPointRadius; 
}

void ParticleInfo::SetSurfaceOffset(float value) 
{ 
	m_pData->m_fSurfaceOffset = value; 
}

float ParticleInfo::GetSurfaceOffset() const 
{ 
	return m_pData->m_fSurfaceOffset; 
}

void ParticleInfo::SetEmissionTime(float value) 
{
	if (value < 1.0F)
	{
		return;
	}

	m_pData->m_fEmissionTime = value; 
}

float ParticleInfo::GetEmissionTime() const 
{ 
	return m_pData->m_fEmissionTime; 
}

void ParticleInfo::SetEmissionTimeRand(float value) 
{ 
	m_pData->m_fEmissionTimeRand = value; 
}

float ParticleInfo::GetEmissionTimeRand() const 
{ 
	return m_pData->m_fEmissionTimeRand; 
}

void ParticleInfo::SetChaosValue(float value) 
{ 
	m_pData->m_fChaosVal = value; 
}

float ParticleInfo::GetChaosValue() const 
{ 
	return m_pData->m_fChaosVal; 
}

void ParticleInfo::SetSpeedChaos(float value) 
{ 
	m_pData->m_fSpeedChaos = value; 
}

float ParticleInfo::GetSpeedChaos() const 
{ 
	return m_pData->m_fSpeedChaos; 
}

void ParticleInfo::SetTextureFrame(int value)
{
	if (value < 1)
	{
		value = 1;
	}
	
	m_pData->m_nTextureFrame = value;
}

int ParticleInfo::GetTextureFrame() const
{
	return m_pData->m_nTextureFrame;
}

void ParticleInfo::SetParticleNum(int value)
{
	if (value < 1)
	{
		return;
	}
	
	m_pData->m_nParticleNum = value;
}

int ParticleInfo::GetParticleNum() const
{
	return m_pData->m_nParticleNum;
}

void ParticleInfo::SetParticleLife(int value) 
{ 
	m_pData->m_nParticleLife = value; 
}

int ParticleInfo::GetParticleLife() const 
{ 
	return m_pData->m_nParticleLife; 
}

void ParticleInfo::SetTailLife(int value) 
{ 
	m_pData->m_nTailLife = value; 
}

int ParticleInfo::GetTailLife() const 
{ 
	return m_pData->m_nTailLife; 
}

void ParticleInfo::SetOneEmitNumber(int value) 
{ 
	m_pData->m_nOneEmitNumber = value; 
}

int ParticleInfo::GetOneEmitNumber() const 
{ 
	return m_pData->m_nOneEmitNumber; 
}

void ParticleInfo::SetParticleFrame(int value) 
{ 
	if (value < 1)
	{
		value = 1;
	}

	m_pData->m_nParticleFrame = value; 
}

int ParticleInfo::GetParticleFrame() const 
{ 
	return m_pData->m_nParticleFrame; 
}

void ParticleInfo::SetParticleColor(unsigned int value) 
{ 
	m_pData->m_nParticleColor = value; 
}

unsigned int ParticleInfo::GetParticleColor() const 
{ 
	return m_pData->m_nParticleColor; 
}

void ParticleInfo::SetParticleColorString(const char* value)
{
	Assert(value != NULL);

	m_pData->m_nParticleColor = VisUtil_ColorToInt(value);
}

result_string ParticleInfo::GetParticleColorString() const
{
	return VisUtil_IntToColor(m_pData->m_nParticleColor);
}

void ParticleInfo::SetRepeat(bool value) 
{ 
	m_pData->m_bRepeat = value; 
}

bool ParticleInfo::GetRepeat() const 
{ 
	return m_pData->m_bRepeat; 
}

void ParticleInfo::SetTrackPointReturn(bool value) 
{ 
	m_pData->m_bTrackPointReturn = value; 
}

bool ParticleInfo::GetTrackPointReturn() const 
{ 
	return m_pData->m_bTrackPointReturn; 
}

void ParticleInfo::SetTrackPointDisappear(bool value)
{
	m_pData->m_bTrackPointDisappear = value;
}

bool ParticleInfo::GetTrackPointDisappear() const
{
	return m_pData->m_bTrackPointDisappear;
}

void ParticleInfo::SetLocalSpaceEmitter(bool value)
{
	m_pData->m_bLocalSpaceEmitter = value;
}

bool ParticleInfo::GetLocalSpaceEmitter() const
{
	return m_pData->m_bLocalSpaceEmitter;
}

void ParticleInfo::SetLocalAngleEmitter(bool value)
{
	m_pData->m_bLocalAngleEmitter = value;
}

bool ParticleInfo::GetLocalAngleEmitter() const
{
	return m_pData->m_bLocalAngleEmitter;
}

void ParticleInfo::SetAngleLinkSpeed(bool value)
{
	m_pData->m_bAngleLinkSpeed = value;
}

bool ParticleInfo::GetAngleLinkSpeed() const
{
	return m_pData->m_bAngleLinkSpeed;
}

void ParticleInfo::SetAngleLinkEmitter(bool value)
{
	m_pData->m_bAngleLinkEmitter = value;
}

bool ParticleInfo::GetAngleLinkEmitter() const
{
	return m_pData->m_bAngleLinkEmitter;
}

void ParticleInfo::SetRandomDirection(bool value) 
{ 
	m_pData->m_bRandomDirection = value; 
}

bool ParticleInfo::GetRandomDirection() const 
{ 
	return m_pData->m_bRandomDirection; 
}

void ParticleInfo::SetOneAxisNotRand(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "XAxis") == 0)
	{
		m_pData->m_nOneAxisNotRand = CParticleData::AXIS_X;
	}
	else if (stricmp(value, "YAxis") == 0)
	{
		m_pData->m_nOneAxisNotRand = CParticleData::AXIS_Y;
	}
	else if (stricmp(value, "ZAxis") == 0)
	{
		m_pData->m_nOneAxisNotRand = CParticleData::AXIS_Z;
	}
	else
	{
		m_pData->m_nOneAxisNotRand = CParticleData::AXIS_NONE;
	}
}

result_string ParticleInfo::GetOneAxisNotRand() const
{
	if (m_pData->m_nOneAxisNotRand == CParticleData::AXIS_X)
	{
		return result_string("XAxis");
	}
	else if (m_pData->m_nOneAxisNotRand == CParticleData::AXIS_Y)
	{
		return result_string("YAxis");
	}
	else if (m_pData->m_nOneAxisNotRand == CParticleData::AXIS_Z)
	{
		return result_string("ZAxis");
	}

	return result_string();
}

void ParticleInfo::SetFlashTail(bool value) 
{ 
	m_pData->m_bFlashTail = value; 
}

bool ParticleInfo::GetFlashTail() const 
{ 
	return m_pData->m_bFlashTail; 
}

void ParticleInfo::SetFixupTailSize(bool value)
{
	m_pData->m_bFixupTailSize = value;
}

bool ParticleInfo::GetFixupTailSize() const
{
	return m_pData->m_bFixupTailSize;
}

CParticleData::track_event_t* ParticleInfo::GetTrackEvent(const char* type)
{
	Assert(type != NULL);

	if (stricmp(type, "alpha") == 0)
	{
		return m_pData->m_Visibility;
	}
	else if (stricmp(type, "dir") == 0)
	{
		return m_pData->m_ParticleDir;
	}
	else if (stricmp(type, "size") == 0)
	{
		return m_pData->m_ParticleSize;
	}
	else if (stricmp(type, "speed") == 0)
	{
		return m_pData->m_ParticleSpeed;
	}
	else if (stricmp(type, "spin") == 0)
	{
		return m_pData->m_ParticleSpin;
	}
	else if (stricmp(type, "angle") == 0)
	{
		return m_pData->m_ParticleAngle;
	}
	else if (stricmp(type, "wind") == 0)
	{
		return m_pData->m_WindEffect;
	}
	else if (stricmp(type, "emit_radius_x") == 0)
	{
		return m_pData->m_EmitRadiusX;
	}
	else if (stricmp(type, "emit_radius_y") == 0)
	{
		return m_pData->m_EmitRadiusY;
	}
	else if (stricmp(type, "emit_radius_z") == 0)
	{
		return m_pData->m_EmitRadiusZ;
	}
	else if (stricmp(type, "gravity") == 0)
	{
		return m_pData->m_GravityEffect;
	}
	//else if (stricmp(type, "red") == 0)
	//{
	//	return m_pData->m_ColorRed;
	//}
	//else if (stricmp(type, "green") == 0)
	//{
	//	return m_pData->m_ColorGreen;
	//}
	//else if (stricmp(type, "blue") == 0)
	//{
	//	return m_pData->m_ColorBlue;
	//}

	return NULL;
}

bool ParticleInfo::AddTrack(const char* type, int time, float value)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return false;
	}

	for (size_t i = 0; i < (CParticleData::TRACK_EVENT_MAX - 1); ++i)
	{
		if (track[i].time == -1)
		{
			track[i].time = time;
			track[i].value = value;
			track[i + 1].time = -1;
			m_pData->InitTrackData();
			return true;
		}
	}

	return false;
}

bool ParticleInfo::SetTrack(const char* type, int time, float value)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return false;
	}

	for (size_t i = 0; i < (CParticleData::TRACK_EVENT_MAX - 1); ++i)
	{
		if (track[i].time == -1)
		{
			break;
		}

		if (track[i].time == time)
		{
			track[i].value = value;
			m_pData->InitTrackData();
			return true;
		}
	}

	return false;
}

bool ParticleInfo::SetTrackByIndex(const char* type, int index, int time, 
	float value)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return false;
	}

	if (size_t(index) >= (CParticleData::TRACK_EVENT_MAX - 1) )
	{
		return false;
	}

	track[index].time = time;
	track[index].value = value;
	m_pData->InitTrackData();
	
	return true;
}

bool ParticleInfo::ClearTrack(const char* type)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return false;
	}

	track[0].time = -1;

	m_pData->InitTrackData();

	return true;
}

int ParticleInfo::GetTrackCount(const char* type)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return 0;
	}

	for (int i = 0; i < (CParticleData::TRACK_EVENT_MAX - 1); ++i)
	{
		if (track[i].time == -1)
		{
			return i;
		}
	}

	return 0;
}

int ParticleInfo::GetTrackTime(const char* type, int index)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return 0;
	}

	if (size_t(index) >= CParticleData::TRACK_EVENT_MAX)
	{
		return 0;
	}

	return track[index].time;
}

float ParticleInfo::GetTrackValue(const char* type, int index)
{
	CParticleData::track_event_t* track = GetTrackEvent(type);

	if (NULL == track)
	{
		return 0.0F;
	}

	if (size_t(index) >= CParticleData::TRACK_EVENT_MAX)
	{
		return 0.0F;
	}

	return track[index].value;
}

void ParticleInfo::SetLuminance(float value)
{
	m_pData->m_fLuminance = value;
}

float ParticleInfo::GetLuminance() const
{
	return m_pData->m_fLuminance;
}

void ParticleInfo::SetDistortionIntensity(float value)
{
	m_pData->m_fDistortionIntensity = value;
}

float ParticleInfo::GetDistortionIntensity() const
{
	return m_pData->m_fDistortionIntensity;
}

void ParticleInfo::SetDelayUnload(bool value)
{
	m_pData->m_bDelayUnload = value;
}

bool ParticleInfo::GetDelayUnload() const
{
	return m_pData->m_bDelayUnload;
}

void ParticleInfo::SetColorAni(bool value)
{
	m_pData->m_bColorAni = value;
}

bool ParticleInfo::GetColorAni() const
{
	return m_pData->m_bColorAni;
}

void ParticleInfo::SetNotDownLevel(bool value)
{
	m_pData->m_bNotDownLevel = value;
}

bool ParticleInfo::GetNotDownLevel() const
{
	return m_pData->m_bNotDownLevel;
}

