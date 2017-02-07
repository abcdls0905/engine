#ifndef _I_PARTICLE_H
#define _I_PARTICLE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"

class IParticle: public IVisBase
{
public:
	
	// 是否加载完成
	virtual bool GetLoadFinish() = 0;

	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 缩放
	virtual bool SetScale(float x, float y, float z) = 0;
	virtual FmVec3 GetScale() const = 0;

	// 中心点位置
	virtual FmVec3 GetCenter() = 0;
	// 包围盒尺寸
	virtual FmVec3 GetBoxSize() = 0;
	// 半径
	virtual float GetRadius() = 0;
	
	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;
	
	// 是否检测碰撞
	virtual void SetTraceEnable(bool value) = 0;
	virtual bool GetTraceEnable() const = 0;

	// 是否在水下
	virtual void SetUnderWater(bool value) = 0;
	virtual bool GetUnderWater() const = 0;

	// 附加颜色
	virtual void SetColor(unsigned int value) = 0;
	virtual unsigned int GetColor() const = 0;

	// 记录临时的透明度变量（只供地形系统使用）
	virtual void SetAlphaValue(float value)  = 0;
	virtual float GetAlphaValue() const = 0;

	// 连接到其他对象
	virtual bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z) = 0;
	// 连接到其他对象的辅助点上
	virtual bool LinkToObjectPoint(const PERSISTID& id, const char* helper_name) = 0;
};

#endif
