#include "game_main.h"

#include "../visual/i_world.h"
#include "../visual/i_scene.h"

#include "../visual/i_camera.h"
#include "../visual/i_particle_manager.h"
#include "../visual/i_particle.h"

#include "../visual/i_terrain.h"

#include "../visual/i_vis_base.h"

#include "../public/i_ini_file.h"
#include "game_scene.h"

#ifdef FX_SYSTEM_WINDOWS
#include "win_input.h"
#endif

#include "../fm_gui/gui.h"
 
#include "helper.h"


#include "../fm_gui/gui.h"
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../fm_gui/edit.h"

#ifdef USE_FREE_CAMERA
#include "free_camera.h"
#endif


extern ICore* g_pCorePtr;

#define TEST_9YIN
//#define TEST_9YIN_TEST1
//#define TEST_9YIN_TEST
//#define TEST_TERRIN
//#define TEST_WATER
//#define TEST_SUN_GLOW
//#define TEST_DIGUO
//#define TEST_DIGUO_YIN
//#define TEST_MODEL
//#define TEST_pad_test
//#define TEST_login2d
//#define TEST_F_outcity_1
//#define TEST_f_kqhz
//#define TEST_f_1130miwusenlin 
//#define TEST_pad_login
//#define TEST_test_effect
//#define TEST_test_particle
//#define TEST_unreal
//#define TEST_mobile_test01
//#define TEST_test_shadow
//#define TEST_demo
//#define TEST_yidong9yin
//#define TEST_damo01
//#define TEST_damo02
//#define TEST_f_kelisi
//#define TEST_HEIGHT_FOG0
//#define TEST_HEIGHT_FOG1
//#define TEST_RECEIVE_SHADOW

ICamera* g_test_camera = NULL;
IScene* g_test_scene = NULL;

void game_main()
{
	//先创建一个场景
	IScene* pScene = (IScene*)g_pCorePtr->CreateEntity("Scene");
	g_test_scene = pScene;

	pScene->GetCameraID();

	//获取主实体world的
	IWorld* pWorld = (IWorld*)g_pCorePtr->GetMainEntity();

	//设置主场景实体
	pWorld->SetMainSceneID(pScene->GetID());

	//修改摄像机方向
	ICamera* pCamera = (ICamera*) g_pCorePtr->GetEntity(pScene->GetCameraID());
	g_test_camera = pCamera;

	pCamera->SetAngleSpeed(-0.21544901f,11.042315f,0.0f);

	Gui* pGui = (Gui*)g_pCorePtr->CreateEntity("Gui");
//  注释 宕机
	pWorld->SetMainGuiID(pGui->GetID());

	pGui->SetShowRenderInfo(false);
	pGui->SetShowFPS(true);

	//创建一个自由的摄像机
#ifdef USE_FREE_CAMERA
	FreeCamera* pC = (FreeCamera*)g_pCorePtr->CreateEntity("FreeCamera");
	pC->SetBindCamera(pScene->GetCameraID());
	pC->SetAllowControl(true);
#endif

#ifdef FX_SYSTEM_WINDOWS
 	CInput*  pCInput = (CInput*)g_pCorePtr->CreateEntity("CInput");
#endif
	//光源管理器
    PERSISTID light_man_id = pScene->Create("LightManager");
	pScene->AddObject(light_man_id,3);
 
	char mbzTemp[512];
#ifdef TEST_9YIN
 	pCamera->SetPosition(529.8,11.6,123.5);
 	pCamera->YawAngle(-FM_PI*1.3/3.0);
 	sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\9yin\\");
#endif

#ifdef TEST_9YIN_TEST
 	pCamera->SetPosition(377.004,13.784,81.802);
 	pCamera->SetAngle(0.047,-2.523,0.0);
 	sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\9yin\\");
#endif

#ifdef TEST_9YIN_TEST1
 	pCamera->SetPosition(527.524,15.350,106.816);
 	pCamera->SetAngle(0.561,-1.355,0.0);
 	sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\9yin\\");
#endif

#ifdef TEST_demo
	pCamera->SetPosition(58.123,67.169,-48.858);
    pCamera->SetAngle(1.724,-6.123,0.0);
 	sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\demo\\");
#endif

#ifdef TEST_TERRIN
	pCamera->SetPosition(-22.129,14.803,262.309);
    pCamera->SetAngle(0.527,-1.553,0.0);
 	sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\test_terrin\\");
#endif
 
#ifdef TEST_SUN_GLOW
   pCamera->SetPosition(26.201,44.869,-11.486);    
   pCamera->SetAngle(-0.695,-3.299,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\watertest\\");
#endif

#ifdef TEST_DIGUO
   pCamera->SetPosition(158.840,85.698,349.214);
   pCamera->SetAngle(0.527,-3.140,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\diguowenming2\\");
#endif

#ifdef TEST_DIGUO_YIN
   pCamera->SetPosition(131.38f,38.606f,-14.58f);
   pCamera->SetAngle(1.1580f,-3.050f,0.0f);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\diguowenming2_yin\\");
#endif

#define TEST_WATER
#ifdef TEST_WATER
   pCamera->SetPosition(-12.611,14.804,-9.336);    
   pCamera->SetAngle(0.573,-2.638,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\watertest\\");
#endif

#ifdef TEST_MODEL
   pCamera->SetPosition(154.888,52.320,301.548);    
   pCamera->SetAngle(1.004,-2.415,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\testshu\\");
#endif


#ifdef TEST_pad_test
   pCamera->SetPosition(35.561,22.521,-12.139);    
   pCamera->SetAngle(0.468,-1.336,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\pad_test\\");
#endif 

#ifdef TEST_login2d
   pCamera->SetPosition(19.437,12.91,-6.498);    
   pCamera->SetAngle(0.291,-1.524,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\login2d\\");
#endif 

#ifdef TEST_F_outcity_1
   pCamera->SetPosition(540.262,28.836,-491.011);    
   pCamera->SetAngle(0.758,-8.704,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\F_outcity_1\\");
#endif 

//
#ifdef TEST_f_kqhz
   pCamera->SetPosition(366.369,30.48,-825.328);    
   pCamera->SetAngle(0.390,-8.811,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\f_kqhz\\");
#endif 

#ifdef TEST_test_effect
   pCamera->SetPosition(-18.732,21.395,-18.347);    
    pCamera->SetAngle(0.595,-1.954,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\test_effect\\");
#endif 

#ifdef TEST_pad_login
   pCamera->SetPosition(9.248,12.177,4.937);    
   pCamera->SetAngle(-0.062,-2.652,0.00);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\pad_login\\");
#endif

#ifdef TEST_f_1130miwusenlin
   pCamera->SetPosition(354.813,33.192,-835.174);    
   pCamera->SetAngle(0.148,-8.570,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\f_1130miwusenlin\\");
#endif

#ifdef TEST_test_particle
  pCamera->SetPosition(-3.041,26.241,-12.577);    
   pCamera->SetAngle(0.447,-12.533,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\test_particle\\");
#endif

#ifdef TEST_unreal
   pCamera->SetPosition(1435.00,76.260,-1150.50 );    
   pCamera->SetAngle(0.576,-7.758,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\unreal\\");
#endif

#ifdef TEST_mobile_test01
   pCamera->SetPosition(-13.699,15.143,-17.368);    
   pCamera->SetAngle(0.136,-11.902,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\mobile_test01\\");
#endif

#ifdef TEST_test_shadow
   pCamera->SetPosition(-12.835,18.094,13.240);    
   pCamera->SetAngle(0.809,-16.149,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\test_shadow\\");
#endif

#ifdef TEST_yidong9yin
   pCamera->SetPosition(36.207,30.458,0.351);    
   pCamera->SetAngle(1.106,-18.800,0.00);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\yidong9yin\\");
#endif

#ifdef TEST_damo01
   pCamera->SetPosition(22.705,27.913,17.620);    
   pCamera->SetAngle(1.170,-19.012,0.00);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\damo01\\");
#endif

#ifdef TEST_damo02
   pCamera->SetPosition(22.705,27.913,17.620);    
   pCamera->SetAngle(1.170,-19.012,0.00);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\damo02\\");
#endif

#ifdef TEST_f_kelisi
   pCamera->SetPosition(10.749,36.628,5.293);    
   pCamera->SetAngle(1.380,-21.675,0.0);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\f_kelisi\\");
#endif

#ifdef TEST_HEIGHT_FOG0
   pCamera->SetPosition(10.749f,36.628f,5.293f);    
   pCamera->SetAngle(1.380f,-21.675f,0.0f);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\forest_s1_3_shourenmu\\");
#endif

#ifdef TEST_HEIGHT_FOG1
   pCamera->SetPosition(-37.696f,37.727f,7.813f);    
   pCamera->SetAngle(0.425f,-22.366f,0.0f);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\tower_tongtianta\\");
#endif

#ifdef TEST_RECEIVE_SHADOW
   pCamera->SetPosition(9.638f, 15.774f, 143.731f);    
   pCamera->SetAngle(0.375f,FM_PIf,0.0f);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"ter\\ceshi\\");
#endif

#define TEST_LS_SENCE
#ifdef TEST_LS_SENCE
   //g_test_camera->SetPosition(24,13,-52);
   g_test_camera->SetPosition(-18.583885, 20.963966, 9.031261);
   g_test_camera->SetAngle(0.0f, FM_PIf,0.0f);
   sprintf(mbzTemp,"%s%s",g_pCorePtr->GetResourcePath(),"map\\ter\\home_s1_1\\");
#endif

   Game_Scene* pGameScene = (Game_Scene*)g_pCorePtr->CreateEntity("Game_Scene");
   pGameScene->Load(mbzTemp,pScene,pWorld);

   //#ifdef _DEBUG_RT_
   //开启RT调试模式
   PERSISTID post_debug_rt = pScene->Create("CPostDebug");
   //pScene->AddObject(post_debug_rt,10000);
   //#endif 

   pScene->SetEnableRealizeTempColorRT(true);
  pScene->SetEnableRealizeTempDepthRT(true);

   //开启水的折射
  pScene->SetEnableRealizeCopyTempColorRT(true);
   pScene->SetEnableRealizeCopyTempDepthRT(true);

   pScene->SetEnableDynamicShadow(true);


}