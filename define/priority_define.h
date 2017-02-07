//----------------------------------------------------------
// 文件名:		priority_define.h
// 内  容:		优先级定义
// 说  明:		
// 创建人:		
// 创建日期:    2014/04/2
// 版权所有:    苏州蜗牛电子有限公司
//----------------------------------------------------------

#ifndef __PRORIORITY_DEFINE_H_
#define __PRORIORITY_DEFINE_H_

//
enum PriorityScene
{
	Priority_Scene_LightManager		= 3, //灯光
	Priority_Scene_Geometry			= 10,  //绘制图形
	Priority_Scene_LinesDrawer		= 10, //线绘制
	Priority_Scene_UI3D				= 20,  //绘制3DUI
	Priority_Scene_TimeAxis			= 20,  //时间轴
	Priority_Scene_Actor			= 20,  
	Priority_Scene_Model			= 20, 
	Priority_Scene_EffectModel		= 20, //特效模型
	Priority_Scene_SoundManager		= 27, //声音
	Priority_Scene_Terrain			= 30, //地形
	Priority_Scene_ShadowManager	= 70, //阴影
	Priority_Scene_PostEffectFilter	= 80, //后处理
	Priority_Scene_SunGlow			= 80, //光晕
	Priority_Scene_ParticleManager	= 100, //粒子
	Priority_Scene_CameraWrapper	= 100, //相机封装
	Priority_Scene_DynamicWeather	= 200, //动态天气
	Priority_Scene_SaberArcManager	= 512, //拖尾
};
#endif