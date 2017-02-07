//--------------------------------------------------------------------
// 文件名:		lib_main.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../public/module.h"

#define REG_ENTITY(class_name) 	extern IEntCreator& class_name##_EntCreator();\
	                             LinkEntCreator(class_name##_EntCreator())
	                            


void lib_main_world()
{
	REG_ENTITY(World);
	REG_ENTITY(Camera);
	REG_ENTITY(Scene);
    REG_ENTITY(SkyBox);
	REG_ENTITY(Weather);
	REG_ENTITY(Model);
	REG_ENTITY(Particle);
	REG_ENTITY(ParticleInfo);
	REG_ENTITY(ParticleManager);
	REG_ENTITY(LightManager);
	REG_ENTITY(LightSource);
	REG_ENTITY(SunGlow);
	REG_ENTITY(IActorCallee);
	REG_ENTITY(Actor);
	REG_ENTITY(ActionControl);
	REG_ENTITY(BrokenShadow);
	REG_ENTITY(EffectModel);
    REG_ENTITY(Skin);

    REG_ENTITY(SaberArc);
    REG_ENTITY(SaberArcManager);
    REG_ENTITY(Whip);
    REG_ENTITY(WhipManager);

	REG_ENTITY(DynamicWeather);

	REG_ENTITY(ShadowManager);
	REG_ENTITY(Shadow);
    
	REG_ENTITY(CPostEffect);
	REG_ENTITY(CPostDebug);
	REG_ENTITY(CPostEffectFilter);
	REG_ENTITY(PSSM);
}


