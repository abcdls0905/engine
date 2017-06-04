

#include "game_control.h"
#include "../visual/i_terrain.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_game_sock.h"
#include "../visual/i_game_client.h"
#include "../visual/i_actor.h"
#include "../visual/i_camera.h"
#include "../visual/i_world.h"
#include "../fm_world/world.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/gui_input.h"
#include "helper.h"
#include "global.h"
#include "../extern/tinyxml/tinyxml.h"
#include "camera/camera_control.h"
#include "../visual/i_soundsystem.h"
#include "custom_prop_define.h"

#include "util/macro_util.h"
#include "scenemanager.h"
#include "../interface/global_prop_define.h"
#include "../fm_world/actor.h"
#include "statemachine/statemachine.h"
#include "../tools/util_func.h"
#include "statemachine/util_state.h"
#include "../fm_world/model.h"


const float BIND_HEIGHT = 1.8f;  //摄像机绑定高度

DECLARE_ENTITY(GameControl, 0, IEntity);

GameControl::GameControl(void):
m_bLBtnDown(false),
m_bMouseMove(false),
m_iStartX(-1),
m_iStartY(-1)
{
#ifdef _WIN32
    m_bKBPathFinding = false;
    m_bKBJoyClicking = false;
#endif
}

GameControl::~GameControl(void)
{
}

bool GameControl::Init(const IVarList& args)
{
  g_gamecontrol = this;

  CORE_ADD_EXECUTE(this);

	g_core->AddMsgProc(this, WM_LBUTTONDOWN);
	g_core->AddMsgProc(this, WM_LBUTTONUP);
	g_core->AddMsgProc(this, WM_MBUTTONDOWN);
	g_core->AddMsgProc(this, WM_MBUTTONUP);
	g_core->AddMsgProc(this, WM_KEYDOWN);
	g_core->AddMsgProc(this, WM_KEYUP);
  g_core->AddMsgProc(this, WM_MOUSEMOVE);
  g_core->AddMsgProc(this, WM_SIZE);

	g_core->AddMsgProc(this, FM_TOUCH_BEGIN);
	g_core->AddMsgProc(this, FM_TOUCH_END);
	g_core->AddMsgProc(this, FM_TOUCH_MOVE);
	g_core->AddMsgProc(this, FM_TOUCH_CANCELLED);

	//g_core->AddMsgProc(this, FM_TOUCH_PAUSE);
	//g_core->AddMsgProc(this, FM_TOUCH_RESUME);

	return true;
}

bool GameControl::Shut()
{
	g_core->RemoveMsgProc(this, WM_LBUTTONDOWN);
	g_core->RemoveMsgProc(this, WM_LBUTTONUP);
	g_core->RemoveMsgProc(this, WM_MBUTTONDOWN);
	g_core->RemoveMsgProc(this, WM_MBUTTONUP);
	g_core->RemoveMsgProc(this, WM_KEYDOWN);
	g_core->RemoveMsgProc(this, WM_KEYUP);
  g_core->RemoveMsgProc(this, WM_MOUSEMOVE);
  g_core->RemoveMsgProc(this, WM_SIZE);

	g_core->RemoveMsgProc(this, FM_TOUCH_BEGIN);
	g_core->RemoveMsgProc(this, FM_TOUCH_END);
	g_core->RemoveMsgProc(this, FM_TOUCH_MOVE);
	g_core->RemoveMsgProc(this, FM_TOUCH_CANCELLED);

	//g_core->RemoveMsgProc(this, FM_TOUCH_PAUSE);
	//g_core->RemoveMsgProc(this, FM_TOUCH_RESUME);

	return true;
}

void GameControl::Execute(float seconds)
{
#ifndef SOUND_FM_FMOD
  
    ICamera* pCamera = (ICamera*)CHelper::GetEntity(g_scene->GetCameraID());
	if(pCamera)
	{  
	}
#endif


#ifdef _WIN32

	//自由摄像机不需要控制角色移动
	CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
	if (pCameraControl != NULL)
	{
		if (pCameraControl->GetActiveType() != CAMERA_FREE)
		{
			//键盘移动主角，仅用于Windows平台上的测试
			IVisBase* pRole = CHelper::GetPlayer();

			float fFormatAngle = 0.0f;
			bool bWKeyDown = (::GetAsyncKeyState('W') < 0);
			bool bSKeyDown = (::GetAsyncKeyState('S') < 0);
			bool bAKeyDown = (::GetAsyncKeyState('A') < 0);
			bool bDKeyDown = (::GetAsyncKeyState('D') < 0);

      if (bWKeyDown == bSKeyDown)
      {
        if (bAKeyDown == bDKeyDown)
        {
          player_stop_move(pRole);
        }
        else
        {
          fFormatAngle = (bAKeyDown? (PI2*0.75f) : (PI2*0.25f));
        }
      }
      else
      {
        if (bAKeyDown == bDKeyDown)
        {
          fFormatAngle = (bWKeyDown? 0 : PI);
        }
        else
        {
          if (bAKeyDown)
          {
            fFormatAngle = (bWKeyDown? (PI2*0.875f) : (PI2*0.625f));
          }
          else
          {
            fFormatAngle = (bWKeyDown? (PI2*0.125f) : (PI2*0.375f));
          }
        }
      }
      if (bWKeyDown || bAKeyDown || bSKeyDown || bDKeyDown)
      {
        player_begin_move(pRole, fFormatAngle);
      }
		}
	}
	
#endif

}

bool GameControl::MsgProc(unsigned int msg, size_t param1, size_t param2, int& result)
{
	//任何时候保证GetMousePos是正确的
	if (WM_MOUSEMOVE == msg)
	{
		//int x = LOWORD(param2);
		//int y = HIWORD(param2);
		g_mouse_pos_x = LOWORD(param2);
		g_mouse_pos_y = HIWORD(param2);
	}

	switch(msg)
	{
  case WM_SIZE:
    {
      return MsgSize(param1, param2);
    }
	case WM_MOUSEMOVE:
		{
			return MsgMouseMove(param1, param2);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			return MsgLButtonDown(param1, param2);
		}
		break;
	case WM_LBUTTONUP:
		{
			return MsgLButtonUp(param1, param2);
		}
		break;
	case WM_KEYUP:
		{
			return MsgKeyUp(param1, param2);
		}
		break;
	case WM_KEYDOWN:
		{
			return MsgKeyDown(param1 ,param2);
		}
		break;


	case FM_TOUCH_BEGIN:
		{
			return MsgTouchBegin(param1, param2);
		}
		break;
	case FM_TOUCH_END:
		{
			return MsgTouchEnd(param1, param2);
		}
		break;
	case FM_TOUCH_MOVE:
		{
			return MsgTouchMove(param1, param2);
		}
		break;
	case FM_TOUCH_CANCELLED:
		{
			return MsgTouchCancel(param1, param2);
		}
		break;
	//case FM_TOUCH_PAUSE:
	//	{
	//		MsgPauseAndResume(true);
	//	}
	//	break;

	//case FM_TOUCH_RESUME:
	//	{
	//		MsgPauseAndResume(false);
	//	}
		break;

	}

	return false;
}

bool GameControl::MsgLButtonDown(size_t param1, size_t param2)
{
    size_t id = param1;
	int x = LOWORD(param2);
	int y = HIWORD(param2);

	m_iStartX = x;
	m_iStartY = y;
	m_bMouseMove = false;
	m_bLBtnDown = true;


#ifdef _WIN32
    if (IsMouseClickUI(id, x, y))
    {
        return false;
    }

    if (TestClickModel(x, y))
    {
        return false;
    }

	//WINDOWS下鼠标模拟触摸屏
	MsgTouchBegin(0, param2);
#endif

	return false;
}

bool GameControl::MsgLButtonUp(size_t param1, size_t param2)
{
	//int x = LOWORD(param2);
	//int y = HIWORD(param2);

	m_bLBtnDown = false;

#ifdef _WIN32
	//WINDOWS下鼠标模拟触摸屏
	MsgTouchEnd(0, param2);
#endif


	return false;
}

bool GameControl::MsgMouseMove(size_t param1, size_t param2)
{
	int x = LOWORD(param2);
	int y = HIWORD(param2);

	if (m_iStartX != x || m_iStartY != y)
	{
		m_bMouseMove = true;
	}

#ifdef _WIN32
	//WINDOWS下鼠标模拟触摸屏
    if (!m_bKBPathFinding && !m_bKBJoyClicking)
    {
	    MsgTouchMove(0, param2);
    }
#endif

	return false;
}

bool GameControl::MsgKeyDown(size_t param1, size_t param2)
{
#ifdef _WIN32
	switch(param1)
	{
		//是否开启GM命令
	case VK_F1:
		{
		}
		break;
	case VK_F2:
		{
			//用来切换第三人称和自由视角
			CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
			if (pCameraControl != NULL)
			{
				int iCameraType = pCameraControl->GetActiveType();
				if (iCameraType != MAX_CAMERA-1)
				{
					pCameraControl->SetActive(iCameraType+1);
				}
				else
				{
					pCameraControl->SetActive(CAMERA_NONE+1);
				}
			}
		}
		break;
	case VK_F3:
		{
		}
		break;
	case VK_F4:
		{

		}
		break;
	case VK_F9:
		{
			//显示地形信息
			if(!g_terrain->GetShowWalkable())
			{
				g_terrain->SetShowWalkable(true);
				g_terrain->SetShowSpaceHeight(true);
                g_terrain->SetGroundVisible(true);
			}
			else
			{
				g_terrain->SetShowWalkable(false);
				g_terrain->SetShowSpaceHeight(false);
                g_terrain->SetGroundVisible(false);
			}
		}
		break;
    case 'J':
    case 'K':
    case 'L':
	case 'U':
	case 'I':
        {
            int nIndex = 0;
            if (param1 == 'J')
                nIndex = 0;
            else if (param1 == 'K')
                nIndex = 1;
            else if (param1 == 'L')
                nIndex = 2;
            else if (param1 == 'U')
                nIndex = 3;
            else if (param1 == 'I')
                nIndex = 4;

        }
        break;
	case 'M':
		{
			IVisBase* pRole = CHelper::GetPlayer();
			if (!pRole)
				break;
      g_terrain->RemoveVisual(pRole->GetID());
      SceneObjManager* sceneMgr = (SceneObjManager*)CHelper::GetGlobalEntity(GLOBAL_SCENEMANAGER);
			sceneMgr->SetPlayer(PERSISTID());
			break;
		}
	case 'N':
    {
		//测试创建角色
		  SceneObjManager* sceneMgr = (SceneObjManager*)CHelper::GetGlobalEntity(GLOBAL_SCENEMANAGER);
      CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
      if (!pCameraControl)
        break;
      CameraBase* pCamera = pCameraControl->GetActiveCamera();
      if (!pCamera)
        break;
      if (CHelper::GetPlayer())
      {
        IVisBase* pRole = CHelper::GetPlayer();
        Actor* pActor = (Actor*)pRole;

        CVarList result;
        int index = rand() % CHelper::GetActor(pRole)->GetActionCount();
        const char* szAction = "pos_0h_stand";//pActor->GetActionName(index);
        pActor->BlendAction(CVarList()<< szAction << false << false, result);
        pActor->SetShowBoundBox(true);
        pCamera->GetCamera()->SetBindID(pRole->GetID());
        pCamera->SetEnable(true);
        FmVec3 position = pCamera->GetCamera()->GetPosition();
        break;
      }
      IActor* pActor = (IActor*)g_pCore->CreateEntity("Actor");
      pActor->SetContext(g_terrain->GetContext());
      pActor->CreateFromIni("obj\\player\\fitxm\\fitxm_equit_001.ini");
      FmVec3 pos = pCamera->GetCamera()->GetPosition();
      IRenderContext* context = g_render->GetContext();
      pActor->SetCastShadow(true);
      pActor->SetReceiveShadow(true);
      pActor->SetPosition(pos.x+3, pos.y, pos.z);

      g_terrain->AddVisualRole("", pActor->GetID());
      g_terrain->SetPlayerID(pActor->GetID());
      g_terrain->RelocateVisual(pActor->GetID(), pos.x+2, pos.y-3, pos.z+2);

      sceneMgr->SetPlayer(pActor->GetID());
    }
    break;
  case 'O':
    {
      CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
      if (!pCameraControl)
        break;
      CameraBase* pCamera = pCameraControl->GetActiveCamera();
      pCamera->GetCamera()->SetPosition(360, 191, -519);
      break;
    }
  case 'Y':
    {
      Model* model = (Model*)g_pCore->CreateEntity("Model");

      CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
      if (!pCameraControl)
        break;
      CameraBase* pCamera = pCameraControl->GetActiveCamera();
      if (!pCamera)
        break;
      model->SetContext(g_terrain->GetContext());
      model->SetModelFile("effect\\common\\common_battle_002b.xmod");
      model->SetMaterialFile("effect\\common\\common_battle_002b.mtl");
      model->Load();
      FmVec3 pos = pCamera->GetCamera()->GetPosition();
      model->SetPosition(pos.x, pos.y, pos.z);
      g_terrain->AddVisual("", model->GetID());
      break;
    }
	}
#endif

	return false;
}


#ifdef _WIN32
#define LOWORD_WIN(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD_WIN(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#endif

bool GameControl::MsgSize(size_t param1, size_t param2)
{
#ifdef _WIN32
  int nWidth = LOWORD_WIN(param2); // width of client area
  int nHeight = HIWORD_WIN(param2); // height of client area
  World* world = (World*)g_world;
  world->SetWidth(nWidth);
  world->SetHeight(nHeight);
  world->SetDeviceParam();
#endif
  return false;
}
bool GameControl::MsgKeyUp(size_t param1, size_t param2)
{
#ifdef _WIN32
    switch(param1)
    {
    case 'J':
        {   
        }   
        break;
    }

#endif

	return false;
}

bool GameControl::MsgTouchBegin(size_t param1, size_t param2)
{
	size_t id = param1;
	int x = LOWORD(param2);
	int y = HIWORD(param2);

    if (IsMouseClickUI(id, x, y))
    {
        return false;
    }

	return false;
}

bool GameControl::MsgTouchEnd(size_t param1, size_t param2)
{
	long id = (long)param1;
	int x = LOWORD(param2);
	int y = HIWORD(param2);

	return false;
}

bool GameControl::MsgTouchMove(size_t param1, size_t param2)
{
	long id = (long)param1;
	int x = LOWORD(param2);
	int y = HIWORD(param2);

	return false;
}

bool GameControl::MsgTouchCancel(size_t param1, size_t param2)
{
	return false;
}

//void GameControl::MsgPauseAndResume(bool bPause)
//{
//    if (!bPause)
//    {
//        TouchManager* pTouchManager = GetTouchManager();
//        if (pTouchManager != NULL)
//        {
//            pTouchManager->OnTouchCancel();
//        }
//    }
//}

bool GameControl::IsMouseClickUI(size_t id, int x, int y)
{
    return false;
}

bool GameControl::TestClickModel(int x, int y)
{
    if (!g_terrain)
    {
        return false;
    }

    //是否点到人或NPC
    unsigned int nMaskValue = g_terrain->GetTraceMaskValue();

    g_terrain->SetTraceMask("Model", true);
    g_terrain->SetTraceMask("Role", false);
    g_terrain->SetTraceMask("Actor2", false);
    g_terrain->SetTraceMask("Actor", false);

    trace_info_t traceInfo;

    g_terrain->InnerPick(x, y, 300, traceInfo);

    return false;
}