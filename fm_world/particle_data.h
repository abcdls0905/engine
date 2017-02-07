//--------------------------------------------------------------------
// 文件名:		particle_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PARTICLE_DATA_H
#define _PARTICLE_DATA_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"


#include "../visual/fm_header.h"

// 粒子数据

class IIniFile;

class CParticleData
{
public:
	// 创建新实例
	static CParticleData* NewInstance();

	enum { TRACK_EVENT_MAX = 16 };

	enum
	{
		AXIS_NONE,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	struct track_event_t
	{
		int time;
		float value;
	};
	
	track_event_t m_Visibility[TRACK_EVENT_MAX];
	track_event_t m_ParticleDir[TRACK_EVENT_MAX];
	track_event_t m_ParticleSize[TRACK_EVENT_MAX];
	track_event_t m_ParticleSpeed[TRACK_EVENT_MAX];
	track_event_t m_ParticleSpin[TRACK_EVENT_MAX];
	track_event_t m_ParticleAngle[TRACK_EVENT_MAX];
	track_event_t m_WindEffect[TRACK_EVENT_MAX];
	track_event_t m_GravityEffect[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusX[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusY[TRACK_EVENT_MAX];
	track_event_t m_EmitRadiusZ[TRACK_EVENT_MAX];
	//track_event_t m_ColorRed[TRACK_EVENT_MAX];
	//track_event_t m_ColorGreen[TRACK_EVENT_MAX];
	//track_event_t m_ColorBlue[TRACK_EVENT_MAX];

	// 不可改变以下变量数组的顺序(GPU数据段整体传输)
	FmVec4 m_vVisibilityTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpeedTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpeedDelta[TRACK_EVENT_MAX];
	FmVec4 m_vWindEffectTrack[TRACK_EVENT_MAX];
	FmVec4 m_vWindEffectDelta[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSizeTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpinTrack[TRACK_EVENT_MAX];
	FmVec4 m_vParticleSpinDelta[TRACK_EVENT_MAX];
	
	core_string m_strTextureName;	// 贴图文件名
	core_string m_strRandomColorTex; // 随机颜色贴图
	core_string m_strRefractionTex; // 折射贴图
	core_string m_strEffectName; // 显示效果
	core_string m_strNextParticle; // 播放完成后立即播放的下一个粒子
	core_string m_strBillboardType; // 广告牌模式
	core_string m_strCalculateMode; // 粒子计算模式
	
	float m_fDirectionDeviation;	// 方向偏移
	float m_fWindEffect;			// 风的影响系数
	float m_fGravity;				// 重力影响
	float m_fInitSpeed;				// 初始速度
	float m_fSpeedDeviation;		// 速度偏移
	float m_fInitSize;				// 初始大小
	float m_fSizeDeviation;			// 大小偏移
	float m_fSpin;					// 旋转
	float m_fSpinDeviation;			// 旋转偏移
	float m_fWeight;				// 重量
	float m_fWeightDeviation;		// 重量偏移

	float m_fEmitRadiusX;			// 发射范围
	float m_fEmitRadiusY;
	float m_fEmitRadiusZ;
	float m_fEmitRadiusXRand;		// 发射范围随机偏移
	float m_fEmitRadiusYRand;
	float m_fEmitRadiusZRand;
	
	float m_fTrackPointRadius;		// 回收半径
	float m_fSurfaceOffset;			// 到平面的距离
	float m_fEmissionTime;			// 发射时间
	float m_fEmissionTimeRand;		// 发射时间随机值
	float m_fChaosVal;				// 位置抖动值
	float m_fSpeedChaos;			// 速度抖动值
	float m_fAspectRatio;			// 粒子贴图长宽比例
	float m_fLuminance;				// 粒子亮度
	float m_fDistortionIntensity;	// 扭曲强度
	
	int m_nTextureFrame;			// 贴图分块数量
	int m_nParticleNum;				// 粒子数量
	int m_nParticleLife;			// 粒子生存时间
	int m_nTailLife;				// 拖尾生存时间
	int m_nOneEmitNumber;			// 一次发射的粒子数量
	int m_nParticleFrame;			// 粒子贴图块数量
	int m_nOneAxisNotRand;			// 限制一个轴的发射方向
	unsigned int m_nParticleColor;	// 粒子颜色
	
	bool m_bRepeat;					// 重复发射
	bool m_bTrackPointReturn;		// 回收标志
	bool m_bTrackPointDisappear;	// 到达后消失
	bool m_bLocalSpaceEmitter;		// 保持与绑定物体的相对位置
	bool m_bLocalAngleEmitter;		// 保持与绑定物体的相对方向
	bool m_bAngleLinkSpeed;			// 保持与绑定物体的速度方向
	bool m_bAngleLinkEmitter;		// 保持与绑定物体的发射器方向
	bool m_bRandomDirection;		// 随机方向标志
	bool m_bFlashTail;				// 粒子拖尾标志
	bool m_bFixupTailSize;			// 修正拖尾尺寸
	bool m_bDelayUnload;			// 延迟卸载粒子
	bool m_bNotDownLevel;			// 不降级显示
	bool m_bColorAni;				// 颜色动画

	bool m_bStaticBoundBox;			// 是否固定包围盒
	FmVec3 m_vBoxMin;			// 包围盒
	FmVec3 m_vBoxMax;

public:
	CParticleData();
	~CParticleData();

	// 配置文件名
	void SetIniName(const char* value) { m_strIniName = value; }
	const char* GetIniName() const { return m_strIniName.c_str(); }

	// 名称
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// 哈希值
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }
	
	// 获得引用计数
	int GetRefs() const;
	// 增加引用
	int IncRefs();
	// 释放引用
	void Release();

	// 从配置文件加载数据
	bool LoadIni(IIniFile* ini, const char* psname, const char* tex_path);
	
	// 初始化用于渲染的轨迹数据
	void InitTrackData();

	// 获得粒子轨迹值
	float GetTrackValue(track_event_t* track, float time);
	
private:
	// 建立粒子轨迹数据
	bool BuildTrack(IIniFile* ini, track_event_t* track, 
		const char* psname, size_t sect_index, const char* key_name);
	
private:
	core_string m_strIniName;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nRefs;
};

#endif // _PARTICLE_DATA_H
