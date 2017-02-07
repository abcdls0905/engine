#ifndef _I_SHADOW_H_
#define _I_SHADOW_H_

#include "i_vis_base.h"

class IShadow: public IVisBase
{
public:
    //加载
	virtual bool Load() = 0;
	// 位置
	virtual bool SetPosition(float x, float y, float z) = 0;
	virtual FmVec3 GetPosition() const = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// 绑定的对象
	virtual void SetBindID(const PERSISTID& value)= 0;
	virtual PERSISTID GetBindID() const= 0;

	// 阴影模糊度
	virtual void SetLightDispersion(float value)= 0;
	virtual float GetLightDispersion() const= 0;

	// 阴影顶部颜色
	virtual void SetShadowTopColorString(const char* value)= 0;
	virtual result_string GetShadowTopColorString() const= 0;

	// 阴影底部颜色
	virtual void SetShadowBottomColorString(const char* value)= 0;
	virtual result_string GetShadowBottomColorString() const= 0;

	// 使用简易阴影
	virtual void SetUseSimpleShadow(bool value)= 0;
	virtual bool GetUseSimpleShadow() const= 0;
};


#endif