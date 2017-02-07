#include "../public/module.h"
#include "game.h"
#ifdef _WIN32
#include "camera/free_camera.h"
#endif
#include "object_motion.h"
#include "camera/camera_control.h"
#include "camera/camera_3rd.h"
#include "role_composite.h"
#include "fight_net.h"
#include "ui3d.h"

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());

void lib_main_game()
{
	REG_ENTITY(Game);
	REG_ENTITY(GameNet);
#ifdef TEST_3DUI
	REG_ENTITY(UI3D);
	REG_ENTITY(UI3DTest);
#endif
	
#ifdef _WIN32
	REG_ENTITY(WinFreeCamera);
#endif
	REG_ENTITY(Camera3rd);
	REG_ENTITY(CameraControl);
	REG_ENTITY(GameObjMotion);
	REG_ENTITY(StageLogin);
	REG_ENTITY(StageMain);
	REG_ENTITY(StageClient);
	REG_ENTITY(StageSwitchScene);
	REG_ENTITY(StageTestModel);

	REG_ENTITY(RoleComposite);
	REG_ENTITY(GameControl);

	REG_ENTITY(FightNet);
#define TEST_RT
#ifdef TEST_RT
	REG_ENTITY(TestRT);
#endif
  REG_ENTITY(SceneObjManager);
  REG_ENTITY(StateMachine);
}