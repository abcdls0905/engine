//--------------------------------------------------------------------
// 文件名:		particle_info.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PARTICLE_INFO_H
#define _PARTICLE_INFO_H

#include "../public/module.h"
#include "particle_data.h"

// 粒子参数

class ParticleInfo: public IEntity
{
public:
	ParticleInfo();
	virtual ~ParticleInfo();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 获得粒子数据
	CParticleData* GetData() const { return m_pData; }

	// 对应的粒子配置文件名
	void SetIniName(const char* value);
	const char* GetIniName() const;
	
	// 贴图文件名
	void SetTextureName(const char* value);
	const char* GetTextureName() const;

	// 随机颜色贴图文件名
	void SetRandomColorTex(const char* value);
	const char* GetRandomColorTex() const;

	// 折射贴图名
	void SetRefractionTex(const char* value);
	const char* GetRefractionTex() const;

	// 显示效果
	void SetEffectName(const char* value);
	const char* GetEffectName() const;

	// 广告牌模式
	void SetBillboardType(const char* value); 
	const char* GetBillboardType() const;

	// 粒子计算模式
	void SetCalculateMode(const char* value); 
	const char* GetCalculateMode() const;

	// 后续播放粒子
	void SetNextParticle(const char* value);
	const char* GetNextParticle() const;
	
	// 方向偏移
	void SetDirectionDeviation(float value);
	float GetDirectionDeviation() const;

	// 风的影响系数
	void SetWindEffect(float value);
	float GetWindEffect() const;

	// 重力影响
	void SetGravity(float value);
	float GetGravity() const;

	// 初始大小
	void SetInitSize(float value);
	float GetInitSize() const;

	// 大小偏移
	void SetSizeDeviation(float value);
	float GetSizeDeviation() const;

	// 长宽比例
	void SetAspectRatio(float value);
	float GetAspectRatio() const;

	// 初始速度
	void SetInitSpeed(float value);
	float GetInitSpeed() const;

	// 速度偏移
	void SetSpeedDeviation(float value);
	float GetSpeedDeviation() const;

	// 旋转
	void SetSpin(float value);
	float GetSpin() const;

	// 旋转偏移
	void SetSpinDeviation(float value);
	float GetSpinDeviation() const;

	// 重量
	void SetWeight(float value);
	float GetWeight() const;

	// 重量偏移
	void SetWeightDeviation(float value);
	float GetWeightDeviation() const;

	// 发射半径X
	void SetEmitRadiusX(float value);
	float GetEmitRadiusX() const;

	// 发射半径Y
	void SetEmitRadiusY(float value);
	float GetEmitRadiusY() const;

	// 发射半径Z
	void SetEmitRadiusZ(float value);
	float GetEmitRadiusZ() const;

	// 发射半径X随机范围
	void SetEmitRadiusXRand(float value);
	float GetEmitRadiusXRand() const;

	// 发射半径Y随机范围
	void SetEmitRadiusYRand(float value);
	float GetEmitRadiusYRand() const;

	// 发射半径Z随机范围
	void SetEmitRadiusZRand(float value);
	float GetEmitRadiusZRand() const;

	// 回收半径
	void SetTrackPointRadius(float value);
	float GetTrackPointRadius() const;

	// 到平面的距离
	void SetSurfaceOffset(float value);
	float GetSurfaceOffset() const;

	// 发射时间
	void SetEmissionTime(float value);
	float GetEmissionTime() const;
	
	// 发射时间随机值
	void SetEmissionTimeRand(float value);
	float GetEmissionTimeRand() const;
	
	// 位置抖动值
	void SetChaosValue(float value);
	float GetChaosValue() const;
	
	// 速度抖动值
	void SetSpeedChaos(float value);
	float GetSpeedChaos() const;

	// 贴图分块数量
	void SetTextureFrame(int value);
	int GetTextureFrame() const;

	// 粒子数量
	void SetParticleNum(int value);
	int GetParticleNum() const;
	
	// 粒子生存时间
	void SetParticleLife(int value);
	int GetParticleLife() const;
	
	// 拖尾生存时间
	void SetTailLife(int value);
	int GetTailLife() const;

	// 一次发射的粒子数量
	void SetOneEmitNumber(int value);
	int GetOneEmitNumber() const;

	// 粒子贴图块数量
	void SetParticleFrame(int value);
	int GetParticleFrame() const;

	// 粒子颜色
	void SetParticleColor(unsigned int value);
	unsigned int GetParticleColor() const;

	// 粒子颜色
	void SetParticleColorString(const char* value);
	result_string GetParticleColorString() const;

	// 重复发射
	void SetRepeat(bool value);
	bool GetRepeat() const;

	// 回收标志
	void SetTrackPointReturn(bool value);
	bool GetTrackPointReturn() const;

	// 到达后消失
	void SetTrackPointDisappear(bool value);
	bool GetTrackPointDisappear() const;

	// 保持与邦定物体的相对位置
	void SetLocalSpaceEmitter(bool value);
	bool GetLocalSpaceEmitter() const;

	// 保持与邦定物体的相对方向
	void SetLocalAngleEmitter(bool value);
	bool GetLocalAngleEmitter() const;

	// 保持与绑定物体的速度方向
	void SetAngleLinkSpeed(bool value);
	bool GetAngleLinkSpeed() const;

	// 保持与绑定物体的发射器方向
	void SetAngleLinkEmitter(bool value);
	bool GetAngleLinkEmitter() const;

	// 随机方向标志
	void SetRandomDirection(bool value);
	bool GetRandomDirection() const;

	// 限制一个轴的发射方向
	void SetOneAxisNotRand(const char* value);
	result_string GetOneAxisNotRand() const;

	// 粒子拖尾标志
	void SetFlashTail(bool value);
	bool GetFlashTail() const;

	// 粒子拖尾尺寸是否固定
	void SetFixupTailSize(bool value);
	bool GetFixupTailSize() const;

	// 轨迹信息
	bool AddTrack(const char* type, int time, float value);
	bool SetTrack(const char* type, int time, float value);
	bool SetTrackByIndex(const char* type, int index, int time, float value);
	bool ClearTrack(const char* type);
	int GetTrackCount(const char* type);
	int GetTrackTime(const char* type, int index);
	float GetTrackValue(const char* type, int index);

	// 粒子亮度
	void SetLuminance(float value);
	float GetLuminance() const;

	// 扭曲强度
	void SetDistortionIntensity(float value);
	float GetDistortionIntensity() const;

	// 延迟卸载粒子
	void SetDelayUnload(bool value);
	bool GetDelayUnload() const;

	// 使用颜色动画
	void SetColorAni(bool value);
	bool GetColorAni() const;

	// 不降级显示
	void SetNotDownLevel(bool value);
	bool GetNotDownLevel() const;

private:
	CParticleData::track_event_t* GetTrackEvent(const char* type);

private:
	CParticleData* m_pData;
};

#endif // _PARTICLE_INFO_H
