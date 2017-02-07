#ifndef _I_SKY_BOX_H
#define _I_SKY_BOX_H

#include "i_vis_base.h"

class ISkyBox: public IVisBase
{
public:
	virtual bool Load() = 0;

	// 是否加载完成
	virtual bool GetLoadFinish() = 0;

	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 方向
	virtual bool SetAngle(float x, float y, float z) = 0;
	virtual FmVec3 GetAngle() const = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;
	
	// 是否异步加载贴图
	virtual void SetAsyncLoad(bool value) = 0;
	virtual bool GetAsyncLoad() const = 0;
	
	// 是否使用深度测试
	virtual void SetZEnable(bool value) = 0;
	virtual bool GetZEnable() const = 0;

	// 乘法因子
	virtual void SetMulFactor(float value) = 0;
	virtual float GetMulFactor() const = 0;

	// Y轴旋转速度
	virtual void SetYawSpeed(float value) = 0;
	virtual float GetYawSpeed() const = 0;

	// 天空四周的贴图
	virtual void SetSideTex(const char* value) = 0;
	virtual const char* GetSideTex() const = 0;
	
	// 天空顶的贴图
	virtual void SetUpTex(const char* value) = 0;
	virtual const char* GetUpTex() const = 0;

	// 淡入的时间
	virtual void SetFadeInTime(float value) = 0;
	virtual float GetFadeInTime() const = 0;

	// 是否渐变结束
	virtual bool GetFadeInFinish() const = 0;

	// 更新贴图
	virtual bool UpdateTexture() = 0;
};

#endif