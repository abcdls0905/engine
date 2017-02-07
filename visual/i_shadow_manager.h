
#ifndef _I_SHADOW_MANAGER_H_
#define _I_SHADOW_MANAGER_H_
#include "i_vis_base.h"

class IShadowManager: public IVisBase
{
public:
	// 加载阴影管理器相关资源
	virtual bool Load() = 0;

	// 是否可见
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// 最多影子数量
	virtual void SetMaxShadowNum(int value) = 0;
	virtual int GetMaxShadowNum() const = 0;

	// 主角玩家对象的影子
	virtual void SetPlayerShadowID(const PERSISTID& value) = 0;
	virtual PERSISTID GetPlayerShadowID() const = 0;

	// 贴图尺寸
	virtual void SetTextureSize(int value) = 0;
	virtual int GetTextureSize() const = 0;

	// 设置是否只用简易阴影
	virtual void SetUseSimpleShadow(bool value) = 0;
	virtual bool GetUseSimpleShadow() const = 0;

	// 创建影子对象
	virtual PERSISTID Create(const PERSISTID& id) = 0;
	// 删除影子对象
	virtual bool Delete(const PERSISTID& id) = 0;

	// 返回影子列表
	virtual void GetShadowList(const IVarList& args, IVarList& result) = 0;
};



#endif