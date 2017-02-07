#include "stage_main.h"
#include "utility.h"
#include "global.h"
#include "../public/var_list.h"
#include "../visual/i_terrain.h"
#include "../visual/i_game_client.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_actor.h"
#include "../fm_gui/gui.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include "../fm_gui/gui_input.h"
#include "camera/camera_control.h"

// ������ƶ�ģʽ
enum MOTION_MODE_ENUM
{
	MMODE_STOP,		// ֹͣ
	MMODE_MOTION,	// �ر��ƶ�
	MMODE_JUMP,		// ��Ծ
	MMODE_JUMPTO,	// �ı���Ծ��Ŀ�귽��
	MMODE_FLY,		// �����ƶ�
	MMODE_SWIM,		// ˮ���ƶ�
	MMODE_DRIFT,	// ˮ���ƶ�
	MMODE_CLIMB,	// ��ǽ
	MMODE_SLIDE,	// �ڲ������߷�Χ�ڻ���
	MMODE_SINK,		// �³�
	MMODE_LOCATE,	// ǿ�ƶ�λ
};

DECLARE_ENTITY(StageMain, 0, IEntity);

StageMain::StageMain()
{
	m_isshow_gmcommand = false;
}

void StageMain::OnEnterStage()
{
	//m_form_gmcccommand.Show();
	g_core->AddMsgProc(this, WM_LBUTTONDOWN);
	g_core->AddMsgProc(this, WM_KEYDOWN);
	CameraControl* pCameraControl = (CameraControl*)g_core->LookupEntity("CameraControl");
	pCameraControl->SetActive(CAMERA_3RD);
}

void StageMain::OnLeaveStage()
{
	m_form_gmcccommand.Close();
	g_core->RemoveMsgProc(this, WM_LBUTTONDOWN);
	g_core->RemoveMsgProc(this, WM_KEYDOWN);
}

bool StageMain::MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
{

	switch(msg)
	{
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(param2);
			int y = HIWORD(param2);
			TouchScreen(x, y);
		}
		break;
	case WM_KEYDOWN:
		{
#ifdef _WIN32
			switch(param1)
			{
				//�Ƿ���GM����
			case VK_F1:
				m_isshow_gmcommand = !m_isshow_gmcommand;
				if(m_isshow_gmcommand)
					m_form_gmcccommand.Show();
				else
					m_form_gmcccommand.Close();
				break;
				//�����л������˳ƺ������ӽ�
			case VK_F2:
				break;
			}
#endif
		}
		break;
	}
	return false;
}

void StageMain::TouchScreen(int x, int y)
{
	PERSISTID id = g_gui->GetCaptureID();
	//if(!id.IsNull())
	//	return;

	IActor* role = actor_getrole();
	CVarList result;
	g_terrain->PickGround(CVarList() << x << y << 300, result);
	if(result.BoolVal(0))
	{
		IGameObject * pGameObj = (IGameObject*)g_core->GetEntity(g_client->GetPlayer());
		if(pGameObj)
		{
			float _speed = pGameObj->GetMoveSpeed();
			float _rspeed = pGameObj->GetRotateSpeed();
			float mspeed = pGameObj->QueryPropFloat("MoveSpeed");
			float rspeed = pGameObj->QueryPropFloat("RollSpeed");
			g_gamesender->RequestMove(CVarList() << MMODE_MOTION << 7 << 
				role->GetPosition().x  << role->GetPosition().y  << role->GetPosition().z <<
				result.FloatVal(1) << result.FloatVal(3) << mspeed << rspeed, result) ;
		//	float y = g_terrain->GetPosiY(result.FloatVal(1), result.FloatVal(3));
		}
		
		
	
		//actor_motion(m_role, result.FloatVal(1), result.FloatVal(3), mspeed);
	}
    else
    {
        result.Clear();
        g_terrain->Pick(CVarList() << x << y << 300, result);
        if(result.BoolVal(0))
        {
            IGameObject * pGameObj = (IGameObject*)g_core->GetEntity(g_client->GetPlayer());
            if(pGameObj)
            {
                float _speed = pGameObj->GetMoveSpeed();
                float _rspeed = pGameObj->GetRotateSpeed();
                float mspeed = pGameObj->QueryPropFloat("MoveSpeed");
                float rspeed = pGameObj->QueryPropFloat("RollSpeed");
                g_gamesender->RequestMove(CVarList() << MMODE_MOTION << 7 <<
                                          role->GetPosition().x  << role->GetPosition().y  << role->GetPosition().z <<
                                          result.FloatVal(1) << result.FloatVal(3) << mspeed << rspeed, result) ;
                //	float y = g_terrain->GetPosiY(result.FloatVal(1), result.FloatVal(3));
            }
            
            
            
            //actor_motion(m_role, result.FloatVal(1), result.FloatVal(3), mspeed);
        }
    }
}