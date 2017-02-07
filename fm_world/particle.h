//--------------------------------------------------------------------
// 文件名:		particle.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_particle.h"
// 粒子

class CParticleNode;

class Particle: public IParticle
{
public:
	Particle();
	virtual ~Particle();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Realize();
	
	// 是否加载完成
	virtual bool GetLoadFinish(); 

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 方向
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// 缩放
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// 中心点位置
	virtual FmVec3 GetCenter();
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize();
	// 半径
	virtual float GetRadius();
	
	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// 是否在水下
	virtual void SetUnderWater(bool value);
	virtual bool GetUnderWater() const;

	// 附加颜色
	virtual void SetColor(unsigned int value);
	virtual unsigned int GetColor() const;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value) { m_fAlphaValue = value; }
	virtual float GetAlphaValue() const { return m_fAlphaValue; }

	// 粒子节点
	void SetParticleNode(CParticleNode* value);
	CParticleNode* GetParticleNode() const { return m_pNode; }

	// 粒子参数对象
	void SetInfoID(const PERSISTID& value) { m_InfoID = value; }
	PERSISTID GetInfoID() const { return m_InfoID; }

	// 粒子名字
	const char* GetName() const;
	// 粒子配置文件
	const char* GetConfig() const;
	// 绑定的对象
	PERSISTID GetBindID() const;

	// 是否水面波纹
	bool GetWaterWave() const;

	// 发射延时
	void SetDelay(float value);
	float GetDelay() const;

	// 粒子发射开关
	void SetEmitSwitch(bool value);
	bool GetEmitSwitch() const;
	
	// 生存时间
	void SetLifeTime(float value);
	float GetLifeTime() const;

	// 播放速度调节因子
	void SetSpeedFactor(float value);
	float GetSpeedFactor() const;

	// LOD结束距离
	void SetLodEnd(float value);
	float GetLodEnd() const;

	// 是否重复发射
	void SetRepeat(bool value);
	bool GetRepeat() const;

	// 是否动态更新包围盒
	void SetUpdateBoundBox(bool value);
	bool GetUpdateBoundBox() const;

	// 是否显示包围盒
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// 透明度
	void SetAlpha(unsigned int value);
	unsigned int GetAlpha() const;

	// 附加颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 裁剪面开关
	void SetClipPlaneEnable(bool value);
	bool GetClipPlaneEnable() const;

	// 设置裁剪面
	bool SetClipPlane(float x, float y, float z, float w);

	// 停止
	bool Stop();
	// 暂停
	bool Pause();
	// 恢复
	bool Resume();
	
	// 连接到其他对象
	bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z);
	// 连接到其他对象的辅助点上
	bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name);

private:
	core_string m_strName;
	CParticleNode* m_pNode;
	PERSISTID m_InfoID;
	bool m_bShowBoundBox;
	bool m_bTraceEnable;
	float m_fAlphaValue;
};

#endif // _PARTICLE_H
