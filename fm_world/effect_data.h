//--------------------------------------------------------------------
// 文件名:		effect_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年10月18日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _EFFECT_DATA_H
#define _EFFECT_DATA_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/tick_timer.h"
#include "../utils/array_pod.h"

// 特效数据缓冲

class IRender;
class IIniFile;

class CEffectData
{
public:
	struct particle_info_t
	{
		core_string helper_name; // 辅助点名
		core_string particle_name; // 粒子名
		int open_emit;				// 打开粒子发射开关
		int close_emit;				// 关闭粒子发射开关
	};

	struct saberarc_info_t
	{
		core_string helper_name0;	// 辅助点0
		core_string helper_name1;	// 辅助点1
		core_string saberarc_name; // 刀光名
		bool is_saber_start;
	};

	struct light_info_t
	{
		core_string helper_name; // 辅助点名
		core_string light_name; // 灯光名
		int open_emit;				// 打开灯光发射开关
		int close_emit;				// 关闭灯光发射开关
	};

	TArrayPod<particle_info_t*, 10, TCoreAlloc> m_ParticleInfo;	// 粒子容器
	TArrayPod<saberarc_info_t*, 2, TCoreAlloc> m_SaberArcInfo;	// 运动托尾
	TArrayPod<light_info_t*, 1, TCoreAlloc> m_LightInfo;		// 灯光

	core_string m_strModelFile; // 特效模型文件
	core_string m_strMaterialFile; // 特效模型材质
	core_string m_strParticleFile; // 粒子文件名
	core_string m_strSaberArcFile; // 刀光文件名
	core_string m_strLightFile; // 光源文件名
	bool m_bClipPlaneEnable;					// 裁剪面开关

public:
	// 创建新实例
	static CEffectData* NewInstance(IRender* pRender);

	CEffectData(IRender* pRender);
	~CEffectData();
	
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

	// 不需要短暂缓冲的声音数据
	void SetNoCache(bool value) { m_bNoCache = value; }
	bool GetNoCache() const { return m_bNoCache; }

	// 未使用时间
	void SetUnuseTime(float value) { m_fUnuseTime = value; }
	float GetUnuseTime() const { return m_fUnuseTime; }

	// 从配置文件加载数据
	bool LoadIni(IIniFile* pIni, const char* em_name);

private:
	IRender* m_pRender;
	core_string m_strIniName;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nRefs;
	bool m_bNoCache;
	float m_fUnuseTime;
};

#endif // _EFFECT_DATA_H
