//--------------------------------------------------------------------
// 文件名:		i_whip_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2014年4月11日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_WHIP_MANAGER_H
#define _I_WHIP_MANAGER_H

#include "../visual/i_vis_base.h"

// 鞭子管理器

enum WhipType
{
    WHIPTYPE_INVALID            = 0,
    WHIPTYPE_ROPE               = 1,    // 绳索
    WHIPTYPE_SIN_WHIP           = 2,    // SIN曲线的鞭子
    WHIPTYPE_CIRCLE_WHIP        = 3,    // 圆形的鞭子
    WHIPTYPE_SIN_WHIP_2         = 4,    // SIN曲线的鞭子2
    WHIPTYPE_CIRCLE_WHIP_2      = 5,    // 圆形的鞭子2
    WHIPTYPE_CIRCLE_SABER       = 6,    // 圆形轨迹的刀光
    WHIPTYPE_SIMI_CIRCLE_SABER  = 7,    // 半圆形轨迹的刀光
};

class IWhipManager: public IVisBase
{
public:
	// 运动贴图文件路径
	virtual void SetTexturePath(const char* value) = 0;
	virtual const char* GetTexturePath() const = 0;

    // 根据模型信息创建拖尾
    virtual PERSISTID CreateFromModel(
        WhipType type, 
        const FmVec3& vLeft,            // 鞭子的切角方向,控制其运动平面
        const char* szTexName,          // 贴图
        float inTime,                   // 进入时间
        float pauseTime,                // 暂停时间
        float outTime,                  // 退出时间
        unsigned int colorStart,        // 起始颜色
        unsigned int colorEnd,          // 结束颜色
        bool highlight,                 // 是否高亮
        float bladeWidth,               // 宽度
        FmVec4 params,                  // 其他参数(params.x控制鞭子弧度,params.y控制角度,params.w控制时间曲线)
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label) = 0; 

	// 根据配置文件创建拖尾
    virtual PERSISTID CreateFromIni(
        const FmVec3& vLeft,            // 鞭子的切角方向,控制其运动平面
        const PERSISTID& attack_id, const char* attack_label, 
        const PERSISTID& suffer_id, const char* suffer_label,
		const char* ini_name, const char* arc_name) = 0;

	// 删除鞭子
	virtual bool Delete(const PERSISTID& id) = 0;
};

#endif // _I_WHIP_MANAGER_H
